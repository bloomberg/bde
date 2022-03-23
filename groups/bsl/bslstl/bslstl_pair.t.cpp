// bslstl_pair.t.cpp                                                  -*-C++-*-
#include <bslstl_pair.h>

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

#include <bslmf_assert.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isintegral.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_types.h>
#include <bsls_util.h>

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

#include <stddef.h>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

#include <algorithm>    // 'std::swap'

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
#include <type_traits>  // No 'bslmf' support for 'is_constructible'
#endif

// Local macros to detect and work around compiler defects.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
// We don't have a specific macro asserting that 'std::swap' implements array
// support, so proxy off the baseline macro until we prove we need more.
# define BSLSTL_PAIR_SWAP_SUPPORTS_ARRAYS 1
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
// MSVC 2013 cannot generate the correct implicit constructors/assignment
// operators for move-only types as members.  Further investigation for
// workarounds is required.
# define BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013 1
#endif

#if !defined(BSLSTL_PAIR_DO_NOT_DEFAULT_THE_DEFAULT_CONSTRUCTOR)              \
  && defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
// The presence of a default constructor of a pair is determined by the
// default-constructibility of its constituent parts on sufficiently complete
// C++11 or later compilers (in C++11 or later mode).  We need the presence of
// the 'is_default_constructible' type trait to be able to test the
// conditional presence of the 'pair' default constructor.  We should have
// checked for 'BSLS_LIBRARYFEATURES_HAS_IS_DEFAULT_CONSTRUCTIBLE' instead of
// 'BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER', but there is no such thing.
// So if you are trying to build this on a new C++11 compiler (like an AIX or
// a Solaris) and you get that 'std::is_default_constructible' does not exist,
// you will need to add some more conditions above.  On all C++11 compilers we
// support in 2020, if '<type_traits>' exists it does define the required
// 'is_default_constructible' trait and it is sufficiently functional.
# define BSLSTL_PAIR_TEST_CONDITIONAL_DEFAULT_CTOR 1
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
// [ 2] pair(bslma::Allocator *basicAllocator);
// [ 2] pair(const T1& a, const T2& b);
// [ 2] pair(const T1& a, const T2& b, bslma::Allocator *basicAllocator);
// [10] template <class U1, class U2> pair(U1&& a, U2&& b);
// [10] template <class U1, class U2> pair(U1&& a, U2&& b, Alloc *a);
// [10] pair(first_type&& a, second_type&& b);
// [10] pair(first_type&& a, second_type&& b, bslma::Allocator *a);
// [11] template <class U1, class U2> pair(U1&& a, const U2& b);
// [11] template <class U1, class U2> pair(const U1& a, U2&& b);
// [11] template <class U1, class U2> pair(const U1& a, const U2& b);
// [11] template <class U1, class U2> pair(const U1& a, U2&& b, Alloc *a);
// [11] template <class U1, class U2> pair(U1&& a, const U2& b, Alloc *a);
// [11] template <class U1, class U2> pair(const U1& a, const U2& b, A *a);
// [11] template <class U1, class U2> pair(const pair<U1, U2>& pr);
// [11] template <class U1, class U2> pair(const pair<U1, U2>& pr, Alloc *a);
// [11] pair(first_type&& a, const second_type& b);
// [11] pair(const first_type& a, second_type&& b);
// [11] pair(const first_type& a, const second_type& b);
// [11] pair(const first_type& a, second_type&& b, Alloc *a);
// [11] pair(first_type&& a, const second_type& b, Alloc *a);
// [11] pair(const first_type& a, const second_type& b, A *a);
// [11] pair(const pair<first_type, second_type>& pr);
// [11] pair(const pair<first_type, second_type>& pr, Alloc *a);
// [13] pair(piecewise_construct_t, tuple, tuple)
// [13] pair(piecewise_construct_t, tuple, tuple, basicAllocator)
// [14] pair(std::pair<*>, bool>)
// [ 2] pair(const pair& original);
// [ 2] pair(const pair& original, bslma::Allocator *basicAllocator);
// [ 4] pair(const pair<U1, U2>& rhs);
// [ 4] pair(const pair<U1, U2>& rhs, bslma::Allocator *basicAllocator);
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other)
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other, Allocator*)
// [ 9] pair(pair&& original)
// [ 9] pair(pair&& original, bslma::Allocator *basicAllocator)
// [  ] pair(const std::pair<U1, U2>& rhs);
// [  ] pair(const std::pair<U1, U2>&, bslma::Allocator *basicAllocator);
// [ 2] ~pair();
// [12] pair& operator=(const pair& rhs);
// [12] pair& operator=(pair&& rhs);
// [12] pair& operator=(const pair<U1, U2>& rhs)
// [12] pair& operator=(pair<U1, U2>&& rhs)
// [  ] pair& operator=(const std::pair<U1, U2>& rhs);
// [ 2] bool operator==(const pair& x, const pair& y);
// [ 2] bool operator!=(const pair& x, const pair& y);
// [ 2] bool operator<(const pair& x, const pair& y);
// [ 2] bool operator>(const pair& x, const pair& y);
// [ 2] bool operator<=(const pair& x, const pair& y);
// [ 2] bool operator>=(const pair& x, const pair& y);
// [ 2] auto operator<=>(const pair& x, const pair& y);
// [ 5] void pair::swap(pair& rhs);
// [ 5] void swap(pair& lhs, pair& rhs);
// [ 6] hashAppend(HASHALG& hashAlg, const pair<T1,T2>&  input);
// [13] bsl::pair(piecewise_construct, tuple, tuple);
// [13] bsl::pair(piecewise_construct, tuple, tuple, alloc);
// [15] std::tuple_element<bsl::pair<T1, T2> >
// [15] std::tuple_size<bsl::pair<T1, T2> >
// [15] tuple_element<>::type& get<INDEX, T1, T2>(bsl::pair<T1, T2>& p)
// [15] tuple_element<>::type& get<TYPE, T1, T2>(bsl::pair<T1, T2>& p)
// [17] template <class U1, class U2> operator std::tuple<U1&, U2&>()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [27] CLASS TEMPLATE DEDUCTION GUIDES
// [28] USAGE EXAMPLE
// [ 3] Type Traits
// [ 7] Concern: Can create a pointer-to-member for 'first' and 'second'
// [ 8] Concern: Can assign to a 'pair' of references
// [16] Concern: Methods marked 'noexcept' in standard are so implemented
// [18] Concern: Fix for DRQS 122792538
// [19] Concern: pairs of C++03 movable types work correctly
// [20] Concern: pairs with 'const' members work correctly
// [21] Concern: pairs of arrays work correctly
// [22] Concern: pairs of references work correctly
// [23] Concern: 'pair' constructors SFINAE when required by standard
// [24] Concern: construct from '0' as null pointer literal
// [25] Concern: 'return' by brace initialization
// [26] Concern: can construct pair of objects that are not copyable

// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition of
// that macro immediately above the test, to easily enable compiling that test
// while in development.  Below is the list of all macros that control the
// availability of these tests:
//  #define BSLSTL_PAIR_COMPILE_FAIL_NOT_AN_ALLOCATOR

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    ~Base() = default;
        // Destroy this object.

    // MANIPULATORS
    Base& operator=(const Base&) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif

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
    AlBase()
    : d_allocator_p(bslma::Default::allocator())
    {
        d_data_p = new (*d_allocator_p) int(0);
    }

    explicit
    AlBase(bslma::Allocator *allocator)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_data_p = new (*d_allocator_p) int(0);
    }

    explicit
    AlBase(int data)
    : d_allocator_p(bslma::Default::allocator())
    {
        d_data_p = new (*d_allocator_p) int(data);
    }

    explicit
    AlBase(int data, bslma::Allocator *allocator)
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
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    Node& operator=(const Node&) = default;
#endif


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
    AlDerived()
    : AlBase()
    {}

    explicit
    AlDerived(bslma::Allocator *allocator)
    : AlBase(allocator)
    {}

    explicit
    AlDerived(int data)
    : AlBase(data)
    {}

    AlDerived(int data, bslma::Allocator *allocator)
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
              || bsl::is_same<TYPE,
                                bsltf::WellBehavedMoveOnlyAllocTestType>::value
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
                     bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value ||
                     bsl::is_same<TYPE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value>
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
int valueOf(const std::pair<U, V>& pr)
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
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= value);  BSLS_ASSERT(value < 128);

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

template <class ARG>
struct ArgHolder  {
    // This 'struct' holds an argument type of the specified (template
    // parameter) type 'ARG', which must be 'bsltf::EmplacableTestType' or
    // 'bsltf::AllocEmplacableTestType', and provides constructors taking a
    // 'bslma::Allocator*', which can be conditionally ignored.

  private:
    // DATA
    ARG d_arg;

    // NOT IMPLEMENTED
    ArgHolder(const ArgHolder&);
    ArgHolder& operator=(const ArgHolder&);

  public:
    // CREATORS
    ArgHolder(int value, bsl::false_type, bslma::Allocator*)
    : d_arg(value)
        // Construct 'd_arg' using the specified 'value'.
    {
    }

    ArgHolder(int value, bsl::true_type, bslma::Allocator *basicAllocator)
    : d_arg(value, basicAllocator)
        // Construct 'd_arg' using the specified 'value' and the specified
        // 'basicAllocator'.
    {
    }

    // ACCESSORS
    ARG& arg()
    {
        return d_arg;
    } 
};

                         // =======================
                         // class StrictlyAllocated
                         // =======================

class StrictlyAllocated {
    // This class provides testing for a particularly awkward element in a
    // 'pair', that does not have public copy or move constructors, but insists
    // on the caller providing an allocator when making copies.

    // DATA
    int               d_index;
    bslma::Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED
    StrictlyAllocated(const StrictlyAllocated&) BSLS_KEYWORD_DELETED;
    explicit StrictlyAllocated(int) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit StrictlyAllocated(bslma::Allocator *basicAllocator);
        // Create a 'StrictlyAllocated' object having the index 0, using the
        // specified 'basicAllocator'.  Note that no memory is actually
        // allocator, the allocator is merely stored for test purposes.

    explicit StrictlyAllocated(int               columnIndex,
                               bslma::Allocator *basicAllocator);
        // Create a 'StrictlyAllocated' object having an index with the
        // specified 'columIndex' value and using the specified
        // 'basicAllocator'.  Note that no memory is actually allocator, the
        // allocator is merely stored for test purposes.

    StrictlyAllocated(const StrictlyAllocated&  other,
                      bslma::Allocator         *basicAllocator);
        // Create a copy of the specified 'other' object using the specified
        // 'basicAllocator', having the same index value as the 'other' object.
        // Note that no memory is actually allocator, the allocator is merely
        // stored for test purposes.

    ~StrictlyAllocated();
        // Destroy this 'StrictlyAllocated' object.

    // MANIPULATORS
    StrictlyAllocated& operator=(const StrictlyAllocated& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    int index() const;
        // Return the 'index' attribute of this object.

    bslma::Allocator *allocator() const;
        // Return the 'allocator' used by this object.
};

                         // -----------------------
                         // class StrictlyAllocated
                         // -----------------------

inline
StrictlyAllocated::StrictlyAllocated(bslma::Allocator *basicAllocator)
: d_index(0)
, d_allocator_p(basicAllocator)
{
}

inline
StrictlyAllocated::StrictlyAllocated(int               columnIndex,
                                     bslma::Allocator *basicAllocator)
: d_index(columnIndex)
, d_allocator_p(basicAllocator)
{
}

inline
StrictlyAllocated::StrictlyAllocated(const StrictlyAllocated&  other,
                                     bslma::Allocator         *basicAllocator)
: d_index(other.d_index)
, d_allocator_p(basicAllocator)
{
}

inline
StrictlyAllocated::~StrictlyAllocated()
{
}

inline
StrictlyAllocated& StrictlyAllocated::operator=(const StrictlyAllocated& rhs)
{
    d_index = rhs.d_index;
    return *this;
}

inline
bslma::Allocator *StrictlyAllocated::allocator() const
{
    return d_allocator_p;
}

inline
int StrictlyAllocated::index() const
{
    return d_index;
}

}  // close namespace u

                         // =========================
                         // class VolatileMovableType
                         // =========================

class  VolatileMovableType {
    // This class template declares a type, that supports copy and move
    // semantics for volatile objects.  This class is primarily provided to
    // unify testing of tuple-like APIs for 'bsl::pair' objects, having
    // ordinary, constant and volatile elements.

  public:
    // TYPES
    enum Enum {
        // Enumeration of move state.
        e_NOT_MOVED,            // The object was not involved in a move
                                // operation.

        e_MOVED,                // The object was involved in a move operation.

        e_NOT_MOVED_BUT_COPIED  // The constant object was involved in a move
                                // operation.
    };

  private:
    // DATA
    int              *d_data_p;       // pointer to the data value (owned)
    mutable Enum      d_movedFrom;    // moved-from state
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  public:
    // CREATORS
    explicit  VolatileMovableType(bslma::Allocator *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the (default)
        // attribute value '-1'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that the default constructor does
        // not allocate memory.

    explicit VolatileMovableType(int               value,
                                 bslma::Allocator *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the specified 'value'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'value >= 0'.

    VolatileMovableType(volatile VolatileMovableType&  original,
                        bslma::Allocator              *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that no memory is allocated if
        // 'original' refers to a default-constructed object.

    VolatileMovableType(
                      const volatile VolatileMovableType&  original,
                      bslma::Allocator                    *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that no memory is allocated if
        // 'original' refers to a default-constructed object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    VolatileMovableType(volatile VolatileMovableType&&  original,
                        bslma::Allocator               *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    VolatileMovableType(
                     const volatile VolatileMovableType&&  original,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create an 'VolatileMovableType' object having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~VolatileMovableType();
        // Destroy this object.

    // MANIPULATORS
    VolatileMovableType& operator=(volatile VolatileMovableType& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    VolatileMovableType&
    operator=(bslmf::MovableRef<volatile VolatileMovableType> rhs);
        // Assign to this object the value of the specified 'rhs' object.  Note
        // that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    int value() const volatile;
        // Return the value of this object.

    Enum movedFrom() const volatile;
        // Return the move state of this object as source of a move operation.
};

// FREE OPERATORS
bool operator==(volatile VolatileMovableType& lhs,
                volatile VolatileMovableType& rhs);
    // Return true if the specified 'lhs' and 'rhs' objects have the same
    // value and false otherwise.  'lhs' has the same value as 'rhs' if
    // 'lhs.value() == rhs.value()'.

                        // -------------------------
                        // class VolatileMovableType
                        // -------------------------

// CREATORS
inline
VolatileMovableType::VolatileMovableType(bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that the default constructor does not allocate memory.
}

inline
VolatileMovableType::VolatileMovableType(int               value,
                                         bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(value >= 0);

    d_data_p = new (*d_allocator_p) int(value);
}

inline
VolatileMovableType::VolatileMovableType(
                                 volatile VolatileMovableType&  original,
                                 bslma::Allocator              *basicAllocator)
: d_data_p(0)
, d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (original.d_data_p) {
        d_data_p = new (*d_allocator_p) int(original.value());
    }
}

inline
VolatileMovableType::VolatileMovableType(
                           const volatile VolatileMovableType&  original,
                           bslma::Allocator                    *basicAllocator)
: d_data_p(0)
, d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (original.d_data_p) {
        d_data_p = new (*d_allocator_p) int(original.value());
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
inline
VolatileMovableType::VolatileMovableType(
                                volatile VolatileMovableType&&  original,
                                bslma::Allocator               *basicAllocator)
: d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    volatile VolatileMovableType& lvalue = original;

    if (d_allocator_p == lvalue.d_allocator_p) {
        d_data_p = lvalue.d_data_p;
    } else {
        d_data_p = new (*d_allocator_p) int(lvalue.value());
        lvalue.d_allocator_p->deleteObjectRaw(lvalue.d_data_p);
    }

    lvalue.d_data_p    = 0;
    lvalue.d_movedFrom = VolatileMovableType::e_MOVED;
}

inline
VolatileMovableType::VolatileMovableType(
                          const volatile VolatileMovableType&&  original,
                          bslma::Allocator                     *basicAllocator)
: d_data_p(0)
, d_movedFrom(VolatileMovableType::e_NOT_MOVED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    const volatile VolatileMovableType& lvalue = original;

    if (lvalue.d_data_p) {
        d_data_p = new (*d_allocator_p) int(lvalue.value());
    }

    lvalue.d_movedFrom = VolatileMovableType::e_NOT_MOVED_BUT_COPIED;
}
#endif

inline
VolatileMovableType::~VolatileMovableType()
{
    d_allocator_p->deleteObjectRaw(d_data_p);
}

// MANIPULATORS
inline
VolatileMovableType&
VolatileMovableType::operator=(volatile VolatileMovableType& rhs)
{
    if (this != &rhs) {
        int *newValue = 0;
        if (rhs.d_data_p) {
            newValue = d_data_p = new (*d_allocator_p) int(rhs.value());
        }
        if (d_data_p) {
            d_allocator_p->deleteObjectRaw(d_data_p);
        }

        d_data_p    = newValue;
        d_movedFrom = e_NOT_MOVED;
    }
    return *this;
}

inline
VolatileMovableType& VolatileMovableType::operator=(
              BloombergLP::bslmf::MovableRef<volatile VolatileMovableType> rhs)
{
    volatile VolatileMovableType& lvalue = rhs;

    if (this != &lvalue) {
        d_data_p           = lvalue.d_data_p;
        lvalue.d_data_p    = 0;
        d_movedFrom        = e_NOT_MOVED;
        lvalue.d_movedFrom = e_MOVED;
    }
    return *this;
}

// ACCESSORS
inline
int VolatileMovableType::value() const volatile
{
    return d_data_p ? *d_data_p : -1;
}

inline
VolatileMovableType::Enum VolatileMovableType::movedFrom() const volatile
{
    return d_movedFrom;
}


// FREE OPERATORS
inline
bool operator==(volatile VolatileMovableType& lhs,
                volatile VolatileMovableType& rhs)
{
    return lhs.value() == rhs.value();
}

template <class T>
bool isRefConstant(const T&)
    // This specialization of 'isRefConstant' is called for const reference
    // types and always returns 'true'.  The function is used to check
    // const qualifier of parameter, returned by bsl::get(bsl::pair<T1, T2>).
{
    return true;
}

template <class T>
bool isRefConstant(T&)
    // This specialization of 'isRefConstant' is called for reference types and
    // always returns 'false'.  The function is used to check const qualifier
    // of parameter, returned by bsl::get(bsl::pair<T1, T2>).
{
    return false;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template<class T>
struct TupleApiTestDriver
    // This utility class template provides functions for testing tuple-like
    // APIs.
{
  private:
    // PRIVATE TYPES
    typedef                bsl::pair<         T, int       >   TIP ;
    typedef                bsl::pair<const    T, int       >  CTIP ;
    typedef                bsl::pair<volatile T, int       >  VTIP ;

    typedef                bsl::pair<int       ,          T>   ITP ;
    typedef                bsl::pair<int       , const    T>   ICTP;
    typedef                bsl::pair<int       , volatile T>   IVTP;

    typedef                bsl::pair<         T,          T>   TTP ;
    typedef                bsl::pair<const    T,          T>  CTTP ;
    typedef                bsl::pair<volatile T,          T>  VTTP ;
    typedef                bsl::pair<         T, const    T>   TCTP;
    typedef                bsl::pair<         T, volatile T>   TVTP;

    typedef const          bsl::pair<         T, int       > C_TIP ;
    typedef const          bsl::pair<int      ,           T> C_ITP ;
    typedef const          bsl::pair<         T,          T> C_TTP ;

    typedef       volatile bsl::pair<         T, int       >  V_TIP;
    typedef       volatile bsl::pair<int      ,           T>  V_ITP;
    typedef       volatile bsl::pair<         T,          T>  V_TTP;

    typedef const volatile bsl::pair<         T, int       > CV_TIP;
    typedef const volatile bsl::pair<int      ,           T> CV_ITP;
    typedef const volatile bsl::pair<         T,          T> CV_TTP;

  public:
    // CLASS METHODS
    static void metaFunctionsTest()
        // Test 'tuple_element' and 'tuple_size' meta-functions.
    {
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        // Testing 'tuple_element'.

        typedef std::tuple_element<0 ,   TIP >  TE0_TIP;
        typedef std::tuple_element<0 ,  CTIP >  TE0_CTIP;
        typedef std::tuple_element<0 ,  VTIP >  TE0_VTIP;
        typedef std::tuple_element<1u,   TIP >  TE1_TIP;
        typedef std::tuple_element<1u,  CTIP >  TE1_CTIP;
        typedef std::tuple_element<1u,  VTIP >  TE1_VTIP;

        typedef std::tuple_element<0 ,   ITP >  TE0_ITP;
        typedef std::tuple_element<0 ,   ITP >  TE0_ICTP;
        typedef std::tuple_element<0 ,   ITP >  TE0_IVTP;
        typedef std::tuple_element<1u,   ITP >  TE1_ITP;
        typedef std::tuple_element<1u,   ICTP>  TE1_ICTP;
        typedef std::tuple_element<1u,   IVTP>  TE1_IVTP;

        typedef std::tuple_element<0 ,   TTP >  TE0_TTP;
        typedef std::tuple_element<0 ,  CTTP >  TE0_CTTP;
        typedef std::tuple_element<0 ,  VTTP >  TE0_VTTP;

        typedef std::tuple_element<1u,   TTP >  TE1_TTP;
        typedef std::tuple_element<1u,   TCTP>  TE1_TCTP;
        typedef std::tuple_element<1u,   TVTP>  TE1_TVTP;

        // Aliases for const and volatile objects.

        typedef std::tuple_element<0 , C_TIP >  TE0_C_TIP;
        typedef std::tuple_element<0 , C_ITP >  TE0_C_ITP;
        typedef std::tuple_element<0 , C_TTP >  TE0_C_TTP;

        typedef std::tuple_element<1u, C_TIP >  TE1_C_TIP;
        typedef std::tuple_element<1u, C_ITP >  TE1_C_ITP;
        typedef std::tuple_element<1u, C_TTP >  TE1_C_TTP;

        typedef std::tuple_element<0 ,  V_TIP > TE0_V_TIP;
        typedef std::tuple_element<0 ,  V_ITP > TE0_V_ITP;
        typedef std::tuple_element<0 ,  V_TTP > TE0_V_TTP;

        typedef std::tuple_element<1u,  V_TIP > TE1_V_TIP;
        typedef std::tuple_element<1u,  V_ITP > TE1_V_ITP;
        typedef std::tuple_element<1u,  V_TTP > TE1_V_TTP;

        typedef std::tuple_element<0 , CV_TIP > TE0_CV_TIP;
        typedef std::tuple_element<0 , CV_ITP > TE0_CV_ITP;
        typedef std::tuple_element<0 , CV_TTP > TE0_CV_TTP;

        typedef std::tuple_element<1u, CV_TIP > TE1_CV_TIP;
        typedef std::tuple_element<1u, CV_ITP > TE1_CV_ITP;
        typedef std::tuple_element<1u, CV_TTP > TE1_CV_TTP;

        // Aliases for returned types.

        typedef const          int C_I;
        typedef const          T   C_T;
        typedef       volatile int  V_I;
        typedef       volatile T    V_T;
        typedef const volatile int CV_I;
        typedef const volatile T   CV_T;

        // Testing objects with various types of elements.

        ASSERT((bsl::is_same<  T,  typename TE0_TIP::type   >::value));
        ASSERT((bsl::is_same<C_T,  typename TE0_CTIP::type  >::value));
        ASSERT((bsl::is_same<V_T,  typename TE0_VTIP::type  >::value));

        ASSERT((bsl::is_same<int,  typename TE1_TIP::type   >::value));
        ASSERT((bsl::is_same<int,  typename TE1_CTIP::type  >::value));
        ASSERT((bsl::is_same<int,  typename TE1_VTIP::type  >::value));

        ASSERT((bsl::is_same<int,  typename TE0_ITP::type   >::value));
        ASSERT((bsl::is_same<int,  typename TE0_ICTP::type  >::value));
        ASSERT((bsl::is_same<int,  typename TE0_IVTP::type  >::value));

        ASSERT((bsl::is_same<  T,  typename TE1_ITP::type   >::value));
        ASSERT((bsl::is_same<C_T,  typename TE1_ICTP::type  >::value));
        ASSERT((bsl::is_same<V_T,  typename TE1_IVTP::type  >::value));

        ASSERT((bsl::is_same<  T,  typename TE0_TTP::type   >::value));
        ASSERT((bsl::is_same<C_T,  typename TE0_CTTP::type  >::value));
        ASSERT((bsl::is_same<V_T,  typename TE0_VTTP::type  >::value));

        ASSERT((bsl::is_same<  T,  typename TE1_TTP::type   >::value));
        ASSERT((bsl::is_same<C_T,  typename TE1_TCTP::type  >::value));
        ASSERT((bsl::is_same<V_T,  typename TE1_TVTP::type  >::value));

        // Testing const and volatile objects.  Note that 'std::tuple_element'
        // isn't specialized for 'const bsl::pair<T1, T2>',
        // 'volatile bsl::pair<T1, T2>', or 'const volatile bsl::pair<T1, T2>',
        // so generic specializations for cv-qualified types are tested.

        ASSERT((bsl::is_same<C_T,  typename TE0_C_TIP::type >::value));
        ASSERT((bsl::is_same<C_I,  typename TE0_C_ITP::type >::value));
        ASSERT((bsl::is_same<C_T,  typename TE0_C_TTP::type >::value));

        ASSERT((bsl::is_same<C_I,  typename TE1_C_TIP::type >::value));
        ASSERT((bsl::is_same<C_T,  typename TE1_C_ITP::type >::value));
        ASSERT((bsl::is_same<C_T,  typename TE1_C_TTP::type >::value));

        ASSERT((bsl::is_same<V_T,  typename TE0_V_TIP::type >::value));
        ASSERT((bsl::is_same<V_I,  typename TE0_V_ITP::type >::value));
        ASSERT((bsl::is_same<V_T,  typename TE0_V_TTP::type >::value));

        ASSERT((bsl::is_same<V_I,  typename TE1_V_TIP::type >::value));
        ASSERT((bsl::is_same<V_T,  typename TE1_V_ITP::type >::value));
        ASSERT((bsl::is_same<V_T,  typename TE1_V_TTP::type >::value));

        ASSERT((bsl::is_same<CV_T, typename TE0_CV_TIP::type>::value));
        ASSERT((bsl::is_same<CV_I, typename TE0_CV_ITP::type>::value));
        ASSERT((bsl::is_same<CV_T, typename TE0_CV_TTP::type>::value));

        ASSERT((bsl::is_same<CV_I, typename TE1_CV_TIP::type>::value));
        ASSERT((bsl::is_same<CV_T, typename TE1_CV_ITP::type>::value));
        ASSERT((bsl::is_same<CV_T, typename TE1_CV_TTP::type>::value));

        // Testing 'tuple_size'.

        typedef std::tuple_size< TIP> TS_TIP;
        typedef std::tuple_size<CTIP> TS_CTIP;
        typedef std::tuple_size<VTIP> TS_VTIP;
        typedef std::tuple_size<ITP > TS_ITP;
        typedef std::tuple_size<ICTP> TS_ICTP;
        typedef std::tuple_size<IVTP> TS_IVTP;
        typedef std::tuple_size< TTP> TS_TTP;
        typedef std::tuple_size<CTTP> TS_CTTP;
        typedef std::tuple_size<VTTP> TS_VTTP;
        typedef std::tuple_size<TCTP> TS_TCTP;
        typedef std::tuple_size<TVTP> TS_TVTP;

        // Aliases for const and volatile objects.

        typedef std::tuple_size<C_TIP > TS_C_TIP;
        typedef std::tuple_size<C_ITP > TS_C_ITP;
        typedef std::tuple_size<C_TTP > TS_C_TTP;
        typedef std::tuple_size<V_TIP > TS_V_TIP;
        typedef std::tuple_size<V_ITP > TS_V_ITP;
        typedef std::tuple_size<V_TTP > TS_V_TTP;
        typedef std::tuple_size<CV_TIP> TS_CV_TIP;
        typedef std::tuple_size<CV_ITP> TS_CV_ITP;
        typedef std::tuple_size<CV_TTP> TS_CV_TTP;

        // Testing objects with various types of elements.

        ASSERT((2u == TS_TIP::value ));
        ASSERT((2u == TS_CTIP::value));
        ASSERT((2u == TS_VTIP::value));
        ASSERT((2u == TS_ITP::value ));
        ASSERT((2u == TS_ICTP::value));
        ASSERT((2u == TS_IVTP::value));
        ASSERT((2u == TS_TTP::value ));
        ASSERT((2u == TS_CTTP::value));
        ASSERT((2u == TS_VTTP::value));
        ASSERT((2u == TS_TCTP::value));
        ASSERT((2u == TS_TVTP::value));

        // Testing const and volatile objects.  Note that 'std::tuple_size'
        // isn't specialized for 'const bsl::pair<T1, T2>',
        // 'volatile bsl::pair<T1, T2>', or 'const volatile bsl::pair<T1, T2>',
        // so generic specializations for cv-qualified types are tested.


        ASSERT((2u == TS_C_TIP::value ));
        ASSERT((2u == TS_C_ITP::value ));
        ASSERT((2u == TS_C_TTP::value ));
        ASSERT((2u == TS_V_TIP::value ));
        ASSERT((2u == TS_V_ITP::value ));
        ASSERT((2u == TS_V_TTP::value ));
        ASSERT((2u == TS_CV_TIP::value));
        ASSERT((2u == TS_CV_ITP::value));
        ASSERT((2u == TS_CV_TTP::value));
    }

    static void getByIndexCopyTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' functions, that accept element
        // index as a template parameter and reference/const reference as a
        // parameter.
    {
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        TIP       tip;
        const TIP TIP_COPY = tip;

        ASSERT((TIP_COPY.first  ==  bsl::get<0        >(tip)));
        ASSERT((BSLS_UTIL_ADDRESSOF(tip.first) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<0        >(tip))));
        ASSERT(      !isRefConstant(bsl::get<0        >(tip)));
        ASSERT((TIP_COPY        ==                      tip ));
        ASSERT((TIP_COPY.first  ==  bsl::get<0, T, int>(tip)));
        ASSERT((BSLS_UTIL_ADDRESSOF(tip.first) ==
               BSLS_UTIL_ADDRESSOF((bsl::get<0, T, int>(tip)))));
        ASSERT(      !isRefConstant(bsl::get<0, T, int>(tip)));
        ASSERT((TIP_COPY        ==                      tip ));

        ASSERT((TIP_COPY.second ==  bsl::get<1        >(tip)));
        ASSERT((    &tip.second == &bsl::get<1        >(tip)));
        ASSERT(      !isRefConstant(bsl::get<1        >(tip)));
        ASSERT((TIP_COPY        ==                      tip ));

        ASSERT((TIP_COPY.second ==  bsl::get<1, T, int>(tip)));
        ASSERT((    &tip.second == &bsl::get<1, T, int>(tip)));
        ASSERT(      !isRefConstant(bsl::get<1, T, int>(tip)));
        ASSERT((TIP_COPY        ==                      tip ));

        ITP       itp;
        const ITP ITP_COPY = itp;

        ASSERT((ITP_COPY.first  ==  bsl::get<0        >(itp)));
        ASSERT((    &itp.first  == &bsl::get<0        >(itp)));
        ASSERT(      !isRefConstant(bsl::get<0        >(itp)));
        ASSERT((ITP_COPY        ==                      itp ));

        ASSERT((ITP_COPY.first  ==  bsl::get<0, int, T>(itp)));
        ASSERT((    &itp.first  == &bsl::get<0, int, T>(itp)));
        ASSERT(      !isRefConstant(bsl::get<0, int, T>(itp)));
        ASSERT((ITP_COPY        ==                      itp ));

        ASSERT((ITP_COPY.second ==  bsl::get<1        >(itp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(itp.second) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<1        >(itp))));
        ASSERT(      !isRefConstant(bsl::get<1        >(itp)));
        ASSERT((ITP_COPY        ==                      itp ));

        ASSERT((ITP_COPY.second ==  bsl::get<1, int, T>(itp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(itp.second) ==
               BSLS_UTIL_ADDRESSOF((bsl::get<1, int, T>(itp)))));
        ASSERT(      !isRefConstant(bsl::get<1, int, T>(itp)));
        ASSERT((ITP_COPY        ==                      itp ));

        TTP       ttp;
        const TTP TTP_COPY = ttp;

        ASSERT((TTP_COPY.first  ==  bsl::get<0        >(ttp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(ttp.first) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<0        >(ttp))));
        ASSERT(      !isRefConstant(bsl::get<0        >(ttp)));
        ASSERT((TTP_COPY        ==                      ttp ));

        ASSERT((TTP_COPY.first  ==  bsl::get<0,   T, T>(ttp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(ttp.first) ==
               BSLS_UTIL_ADDRESSOF((bsl::get<0,   T, T>(ttp)))));
        ASSERT(      !isRefConstant(bsl::get<0,   T, T>(ttp)));
        ASSERT((TTP_COPY        ==                      ttp ));

        ASSERT((TTP_COPY.second ==  bsl::get<1        >(ttp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(ttp.second) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<1        >(ttp))));
        ASSERT(      !isRefConstant(bsl::get<1        >(ttp)));
        ASSERT((TTP_COPY        ==                      ttp ));

        ASSERT((TTP_COPY.second ==  bsl::get<1,   T, T>(ttp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(ttp.second) ==
               BSLS_UTIL_ADDRESSOF((bsl::get<1,   T, T>(ttp)))));
        ASSERT(      !isRefConstant(bsl::get<1,   T, T>(ttp)));
        ASSERT((TTP_COPY        ==                      ttp ));

        // Testing objects with const qualifier.

        C_TIP       c_tip;
        const C_TIP C_TIP_COPY = c_tip;

        ASSERT((C_TIP_COPY.first  ==  bsl::get<0          >(c_tip)));
        ASSERT((    &c_tip.first  == &bsl::get<0          >(c_tip)));
        ASSERT(         isRefConstant(bsl::get<0          >(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.first  ==  bsl::get<0,   T, int>(c_tip)));
        ASSERT((    &c_tip.first  == &bsl::get<0,   T, int>(c_tip)));
        ASSERT(         isRefConstant(bsl::get<0,   T, int>(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.second ==  bsl::get<1          >(c_tip)));
        ASSERT((    &c_tip.second == &bsl::get<1          >(c_tip)));
        ASSERT(         isRefConstant(bsl::get<1          >(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.second ==  bsl::get<1,   T, int>(c_tip)));
        ASSERT((    &c_tip.second == &bsl::get<1,   T, int>(c_tip)));
        ASSERT(         isRefConstant(bsl::get<1,   T, int>(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        C_ITP       c_itp;
        const C_ITP C_ITP_COPY = c_itp;

        ASSERT((C_ITP_COPY.first  ==  bsl::get<0          >(c_itp)));
        ASSERT((    &c_itp.first  == &bsl::get<0          >(c_itp)));
        ASSERT(         isRefConstant(bsl::get<0          >(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        ASSERT((C_ITP_COPY.first  ==  bsl::get<0, int,   T>(c_itp)));
        ASSERT((    &c_itp.first  == &bsl::get<0, int,   T>(c_itp)));
        ASSERT(         isRefConstant(bsl::get<0, int,   T>(c_itp)));
        ASSERT((C_ITP_COPY        ==                          c_itp ));

        ASSERT((C_ITP_COPY.second ==  bsl::get<1          >(c_itp)));
        ASSERT((    &c_itp.second == &bsl::get<1          >(c_itp)));
        ASSERT(         isRefConstant(bsl::get<1          >(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        ASSERT((C_ITP_COPY.second ==  bsl::get<1, int,   T>(c_itp)));
        ASSERT((    &c_itp.second == &bsl::get<1, int,   T>(c_itp)));
        ASSERT(         isRefConstant(bsl::get<1, int,   T>(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        C_TTP       c_ttp;
        const C_TTP C_TTP_COPY = c_ttp;

        ASSERT((C_TTP_COPY.first  ==  bsl::get<0          >(c_ttp)));
        ASSERT((    &c_ttp.first  == &bsl::get<0          >(c_ttp)));
        ASSERT(         isRefConstant(bsl::get<0          >(c_ttp)));
        ASSERT((C_TTP_COPY        ==                        c_ttp ));

        ASSERT((C_TTP_COPY.first  ==  bsl::get<0,   T,   T>(c_ttp)));
        ASSERT((    &c_ttp.first  == &bsl::get<0,   T,   T>(c_ttp)));
        ASSERT(         isRefConstant(bsl::get<0,   T,   T>(c_ttp)));
        ASSERT((C_TTP_COPY        ==                        c_ttp ));

        ASSERT((C_TTP_COPY.second ==  bsl::get<1          >(c_ttp)));
        ASSERT((    &c_ttp.second == &bsl::get<1          >(c_ttp)));
        ASSERT(         isRefConstant(bsl::get<1          >(c_ttp)));
        ASSERT((C_TTP_COPY        ==                        c_ttp ));

        ASSERT((C_TTP_COPY.second ==  bsl::get<1,   T,   T>(c_ttp)));
        ASSERT((    &c_ttp.second == &bsl::get<1,   T,   T>(c_ttp)));
        ASSERT(         isRefConstant(bsl::get<1,   T,   T>(c_ttp)));
        ASSERT((C_TTP_COPY        ==                        c_ttp ));
    }

    static void getByIndexMoveTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' function, that accepts element
        // index as a template parameter and rvalue reference as a parameter.
    {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        // Testing first element extraction.
        {
            TIP      tip(T(1), 1);
            const T& crt     = tip.first;
            const T  ORIG_T  = crt;

            CTIP     ctip(T(2), 1);
            const T& crct    = ctip.first;
            const T  ORIG_CT = crct;

            ASSERT(MoveState::e_NOT_MOVED == crt.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct.movedFrom());

            const T  tipCopy(bsl::get<0>(MoveUtil::move( tip)));
            const T ctipCopy(bsl::get<0>(MoveUtil::move(ctip)));

            ASSERT(ORIG_T                 == tipCopy);
            ASSERT(MoveState::e_MOVED     == crt.movedFrom());
            ASSERT(ORIG_CT                == ctipCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct.movedFrom());
        }

        // Testing second element extraction.
        {
            ITP      itp(1, T(3));
            const T& crt     = itp.second;
            const T  ORIG_T  = crt;

            ICTP     ictp(1, T(4));
            const T& crct    = ictp.second;
            const T  ORIG_CT = crct;

            ASSERT(MoveState::e_NOT_MOVED == crt.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct.movedFrom());

            const T  itpCopy(bsl::get<1>(MoveUtil::move( itp)));
            const T ictpCopy(bsl::get<1>(MoveUtil::move(ictp)));

            ASSERT(ORIG_T                 == itpCopy);
            ASSERT(MoveState::e_MOVED     == crt.movedFrom());
            ASSERT(ORIG_CT                == ictpCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct.movedFrom());
        }

        // Testing elements extraction from pair with elements of the same
        // type.
        {
            TTP      ttp(T(5), T(6));
            const T& crt1     = ttp.first;
            const T  ORIG_T1  = crt1;
            const T& crt2     = ttp.second;
            const T  ORIG_T2  = crt2;

            ASSERT(MoveState::e_NOT_MOVED == crt1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());

            const T ttpCopy1(bsl::get<0>(MoveUtil::move(ttp)));

            ASSERT(ORIG_T1                == ttpCopy1);
            ASSERT(MoveState::e_MOVED     == crt1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());

            const T ttpCopy2(bsl::get<1>(MoveUtil::move(ttp)));

            ASSERT(ORIG_T2                == ttpCopy2);
            ASSERT(MoveState::e_MOVED     == crt1.movedFrom());
            ASSERT(MoveState::e_MOVED     == crt2.movedFrom());
        }
#endif
    }

    static void getByIndexVolatileMoveTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' function, that accepts element
        // index as a template parameter and rvalue reference to the pair,
        // having at least one volatile element, as a parameter.
    {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        // Testing first element extraction.
        {
            VTIP      vtip(T(1), 1);
            volatile T& crvt     = vtip.first;
            volatile T  ORIG_VT  = crvt;

            ASSERT(VolatileMovableType::e_NOT_MOVED == crvt.movedFrom());

            volatile T  vtipCopy(bsl::get<0>(MoveUtil::move( vtip)));

            ASSERT(ORIG_VT                          == vtipCopy);
            ASSERT(VolatileMovableType::e_MOVED     == crvt.movedFrom());
        }

        // Testing second element extraction.
        {
            ITP         ivtp(1, T(3));
            volatile T& crvt    = ivtp.second;
            volatile T  ORIG_VT = crvt;

            ASSERT(VolatileMovableType::e_NOT_MOVED == crvt.movedFrom());

            volatile T  ivtpCopy(bsl::get<1>(MoveUtil::move( ivtp)));

            ASSERT(ORIG_VT                          == ivtpCopy);
            ASSERT(VolatileMovableType::e_MOVED     == crvt.movedFrom());
        }
#endif
    }

    static void getByTypeCopyTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' function, that accepts element
        // type as a template parameter and reference/const reference as a
        // parameter.
    {
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        TIP       tip;
        const TIP TIP_COPY = tip;

        ASSERT((TIP_COPY.first  ==  bsl::get<T          >(tip)));
        ASSERT((BSLS_UTIL_ADDRESSOF(tip.first) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<T          >(tip))));
        ASSERT(      !isRefConstant(bsl::get<T          >(tip)));
        ASSERT((TIP_COPY        ==                        tip ));

        ASSERT((TIP_COPY.first  ==  bsl::get<T  , T, int>(tip)));
        ASSERT((BSLS_UTIL_ADDRESSOF(tip.first) ==
                BSLS_UTIL_ADDRESSOF((bsl::get<T  , T, int>(tip)))));
        ASSERT(      !isRefConstant(bsl::get<T  , T, int>(tip)));
        ASSERT((TIP_COPY        ==                        tip ));

        ASSERT((TIP_COPY.second ==  bsl::get<int        >(tip)));
        ASSERT((    &tip.second == &bsl::get<int        >(tip)));
        ASSERT(      !isRefConstant(bsl::get<int        >(tip)));
        ASSERT((TIP_COPY        ==                        tip ));

        ASSERT((TIP_COPY.second ==  bsl::get<int, T, int>(tip)));
        ASSERT((    &tip.second == &bsl::get<int, T, int>(tip)));
        ASSERT(      !isRefConstant(bsl::get<int, T, int>(tip)));
        ASSERT((TIP_COPY        ==                        tip ));

        ITP       itp;
        const ITP ITP_COPY = itp;

        ASSERT((ITP_COPY.first  ==  bsl::get<int        >(itp)));
        ASSERT((    &itp.first  == &bsl::get<int        >(itp)));
        ASSERT(      !isRefConstant(bsl::get<int        >(itp)));
        ASSERT((ITP_COPY        ==                        itp ));

        ASSERT((ITP_COPY.first  ==  bsl::get<int, int, T>(itp)));
        ASSERT((    &itp.first  == &bsl::get<int, int, T>(itp)));
        ASSERT(      !isRefConstant(bsl::get<int, int, T>(itp)));
        ASSERT((ITP_COPY        ==                        itp ));

        ASSERT((ITP_COPY.second ==  bsl::get<T          >(itp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(itp.second) ==
                BSLS_UTIL_ADDRESSOF(bsl::get<T          >(itp))));
        ASSERT(      !isRefConstant(bsl::get<T          >(itp)));
        ASSERT((ITP_COPY        ==                        itp ));

        ASSERT((ITP_COPY.second ==  bsl::get<T  , int, T>(itp)));
        ASSERT((BSLS_UTIL_ADDRESSOF(itp.second) ==
                BSLS_UTIL_ADDRESSOF((bsl::get<T  , int, T>(itp)))));
        ASSERT(      !isRefConstant(bsl::get<T  , int, T>(itp)));
        ASSERT((ITP_COPY        ==                        itp ));

        // Testing objects with const qualifier.

        C_TIP       c_tip;
        const C_TIP C_TIP_COPY = c_tip;

        ASSERT((C_TIP_COPY.first  ==  bsl::get<T          >(c_tip)));
        ASSERT((    &c_tip.first  == &bsl::get<T          >(c_tip)));
        ASSERT(         isRefConstant(bsl::get<T          >(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.first  ==  bsl::get<T  , T, int>(c_tip)));
        ASSERT((    &c_tip.first  == &bsl::get<T  , T, int>(c_tip)));
        ASSERT(         isRefConstant(bsl::get<T  , T, int>(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.second ==  bsl::get<int        >(c_tip)));
        ASSERT((    &c_tip.second == &bsl::get<int        >(c_tip)));
        ASSERT(         isRefConstant(bsl::get<int        >(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        ASSERT((C_TIP_COPY.second ==  bsl::get<int, T, int>(c_tip)));
        ASSERT((    &c_tip.second == &bsl::get<int, T, int>(c_tip)));
        ASSERT(         isRefConstant(bsl::get<int, T, int>(c_tip)));
        ASSERT((C_TIP_COPY        ==                        c_tip ));

        C_ITP       c_itp;
        const C_ITP C_ITP_COPY = c_itp;

        ASSERT((C_ITP_COPY.first  ==  bsl::get<int        >(c_itp)));
        ASSERT((    &c_itp.first  == &bsl::get<int        >(c_itp)));
        ASSERT(         isRefConstant(bsl::get<int        >(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        ASSERT((C_ITP_COPY.first  ==  bsl::get<int, int, T>(c_itp)));
        ASSERT((    &c_itp.first  == &bsl::get<int, int, T>(c_itp)));
        ASSERT(         isRefConstant(bsl::get<int, int, T>(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        ASSERT((C_ITP_COPY.second ==  bsl::get<T          >(c_itp)));
        ASSERT((    &c_itp.second == &bsl::get<T          >(c_itp)));
        ASSERT(         isRefConstant(bsl::get<T          >(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));

        ASSERT((C_ITP_COPY.second ==  bsl::get<T  , int, T>(c_itp)));
        ASSERT((    &c_itp.second == &bsl::get<T  , int, T>(c_itp)));
        ASSERT(         isRefConstant(bsl::get<T  , int, T>(c_itp)));
        ASSERT((C_ITP_COPY        ==                        c_itp ));
    }

    static void getByTypeMoveTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' functions, that accept element
        // index as a template parameter and rvalue reference/const rvalue
        // reference as a parameter.
    {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        // Testing first element extraction.
        {
            TIP        tip(T(1), 1);
            const T&   crt1     = tip.first;
            const T    ORIG_T1  = crt1;

            const TIP  tipc(T(2), 1);
            const T&   crt2     = tipc.first;
            const T    ORIG_T2  = crt2;

            CTIP       ctip(T(3), 1);
            const T&   crct1    = ctip.first;
            const T    ORIG_CT1 = crct1;

            const CTIP ctipc(T(4), 1);
            const T&   crct2    = ctipc.first;
            const T    ORIG_CT2 = crct2;

            ASSERT(MoveState::e_NOT_MOVED == crt1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct2.movedFrom());

            const T  tipCopy (bsl::get<      T>(MoveUtil::move( tip )));
            const T  tipcCopy(bsl::get<      T>(MoveUtil::move( tipc)));
            const T ctipCopy (bsl::get<const T>(MoveUtil::move(ctip )));
            const T ctipcCopy(bsl::get<const T>(MoveUtil::move(ctipc)));

            ASSERT(ORIG_T1                == tipCopy);
            ASSERT(MoveState::e_MOVED     == crt1.movedFrom());
            ASSERT(ORIG_T2                == tipcCopy);
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());
            ASSERT(ORIG_CT1               == ctipCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct1.movedFrom());
            ASSERT(ORIG_CT2               == ctipcCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct2.movedFrom());

            ASSERT((1 == bsl::get<int>(MoveUtil::move( tip ))));
            ASSERT((1 == bsl::get<int>(MoveUtil::move( tipc))));
            ASSERT((1 == bsl::get<int>(MoveUtil::move(ctip ))));
            ASSERT((1 == bsl::get<int>(MoveUtil::move(ctipc))));
        }

        // Testing second element extraction.
        {
            ITP        itp(1, T(5));
            const T&   crt1     = itp.second;
            const T    ORIG_T1  = crt1;

            const ITP  itpc(1, T(6));
            const T&   crt2     = itpc.second;
            const T    ORIG_T2  = crt2;

            ICTP       ictp(1, T(7));
            const T&   crct1    = ictp.second;
            const T    ORIG_CT1 = crct1;

            const ICTP ictpc(1, T(8));
            const T&   crct2    = ictpc.second;
            const T    ORIG_CT2 = crct2;

            ASSERT(MoveState::e_NOT_MOVED == crt1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct1.movedFrom());
            ASSERT(MoveState::e_NOT_MOVED == crct2.movedFrom());

            const T  itpCopy (bsl::get<      T>(MoveUtil::move(itp  )));
            const T  itpcCopy(bsl::get<      T>(MoveUtil::move(itpc )));
            const T ictpCopy (bsl::get<const T>(MoveUtil::move(ictp )));
            const T ictpcCopy(bsl::get<const T>(MoveUtil::move(ictpc)));

            ASSERT(ORIG_T1                == itpCopy);
            ASSERT(MoveState::e_MOVED     == crt1.movedFrom());
            ASSERT(ORIG_T2                == itpcCopy);
            ASSERT(MoveState::e_NOT_MOVED == crt2.movedFrom());
            ASSERT(ORIG_CT1               == ictpCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct1.movedFrom());
            ASSERT(ORIG_CT2               == ictpcCopy);
            ASSERT(MoveState::e_NOT_MOVED == crct2.movedFrom());

            ASSERT(1 == bsl::get<int>(MoveUtil::move(itp  )));
            ASSERT(1 == bsl::get<int>(MoveUtil::move(itpc )));
            ASSERT(1 == bsl::get<int>(MoveUtil::move(ictp )));
            ASSERT(1 == bsl::get<int>(MoveUtil::move(ictpc)));
        }
#endif
    }

    static void getByTypeVolatileMoveTest()
        // Test 'bsl::get(bsl::pair<T1, T2>)' function, that accepts element
        // type as a template parameter and rvalue reference to the pair,
        // having at least one volatile element, as a parameter.
    {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        typedef VolatileMovableType VMT;

        // Testing first element extraction.
        {
            VTIP              vtip(T(9), 1);
            volatile T&       crvt1      = vtip.first;
            volatile T        ORIG_VT1   = crvt1;

            const VTIP        vtipc(T(10), 1);
            const volatile T& crvt2 = vtipc.first;
            volatile T        ORIG_VT2   = crvt2;

            ASSERT(VMT::e_NOT_MOVED            == crvt1.movedFrom());
            ASSERT(VMT::e_NOT_MOVED            == crvt2.movedFrom());

            volatile T vtipCopy (bsl::get<volatile T>(MoveUtil::move(vtip )));
            volatile T vtipcCopy(bsl::get<volatile T>(MoveUtil::move(vtipc)));

            ASSERT(ORIG_VT1                    == vtipCopy);
            ASSERT(VMT::e_MOVED                == crvt1.movedFrom());
            ASSERT(ORIG_VT2                    == vtipcCopy);
            ASSERT(VMT::e_NOT_MOVED_BUT_COPIED == crvt2.movedFrom());

            ASSERT(1 == bsl::get<int>(MoveUtil::move(vtip )));
            ASSERT(1 == bsl::get<int>(MoveUtil::move(vtipc)));
        }

        // Testing second element extraction.
        {
            IVTP              ivtp(1, T(9));
            volatile T&       crvt1      = ivtp.second;
            volatile T        ORIG_VT1   = crvt1;

            const IVTP        ivtpc(1, T(10));
            const volatile T& crvt2 = ivtpc.second;
            volatile T        ORIG_VT2   = crvt2;

            ASSERT(VMT::e_NOT_MOVED            == crvt1.movedFrom());
            ASSERT(VMT::e_NOT_MOVED            == crvt2.movedFrom());

            volatile T ivtpCopy (bsl::get<volatile T>(MoveUtil::move(ivtp )));
            volatile T ivtpcCopy(bsl::get<volatile T>(MoveUtil::move(ivtpc)));

            ASSERT(ORIG_VT1                    == ivtpCopy);
            ASSERT(VMT::e_MOVED                == crvt1.movedFrom());
            ASSERT(ORIG_VT2                    == ivtpcCopy);
            ASSERT(VMT::e_NOT_MOVED_BUT_COPIED == crvt2.movedFrom());

            ASSERT(1 == bsl::get<int>(MoveUtil::move(ivtp )));
            ASSERT(1 == bsl::get<int>(MoveUtil::move(ivtpc)));
        }
#endif
    }
};
#endif

}  // close unnamed namespace

// TRAITS
namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<u::AlBase> : bsl::true_type {};
template <>
struct UsesBslmaAllocator<u::AlDerived> : bsl::true_type {};
template <>
struct UsesBslmaAllocator<u::StrictlyAllocated> : bsl::true_type {};
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

BSLA_MAYBE_UNUSED inline
void debugprint(const u::Node& node)
{
    bsls::BslTestUtil::callDebugprint(static_cast<char>(node.data()));
}

#endif

BSLA_MAYBE_UNUSED inline
void debugprint(const u::Base& base)
{
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                    TTF::getIdentifier(base)));
}

}  // close namespace bsl


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
    typedef void allocator_type;  // Don't inherit this type from base class

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    ~my_NoAllocString() = default;
        // Destroy this object.

    // MANIPULATORS
    my_NoAllocString& operator=(const my_NoAllocString& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif
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

// Empty classes are bitwise movable by default.  Specialize for 'my_NoTraits'
// to make it NOT bitwise movable.
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
//          HELPER CLASS TO TEST CONVERTING CONSTRUCTOR
//-----------------------------------------------------------------------------
class NonCopyable131875306 {
private:
    int d_val;

    // NOT IMPLEMENTED
    NonCopyable131875306 (const NonCopyable131875306 &rhs);

public:
    explicit NonCopyable131875306(int i) : d_val(i) {}
        // Construct an object containing a copy of the specified int 'i'

    int get () const { return d_val; }
        // return the contained value
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
        std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
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
        std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
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
        std::pair<const char*, const char*> p1("Hello", "World"),
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
        std::pair<const char*, const char*> p1("Hello", "World"),
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
        std::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
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
        std::pair<const char*, const char*> p1("Hello", "World"),
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
void testFunctionality()
    // Test functionality of 'bsl::pair<T1,T2>', with and without
    // explicitly-specified 'bslma::Allocator*' constructor arguments.  (If
    // neither 'T1' nor 'T2' uses 'bslma::Allocator', then the allocator
    // argument should be ignored when supplied, instead of resulting in a
    // compilation error.)
{
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
    bslma::TestAllocator ta1(veryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVerbose);
    bslma::TestAllocator ta3(veryVeryVerbose);

    bslma::DefaultAllocatorGuard allocGuard(&ta0);

    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    {
        // Compute minimum number of allocations using default allocator.
        int minDefaultAllocations = 0;  // Allocations from default allocator
        if (bslma::UsesBslmaAllocator<T1>::value)
            minDefaultAllocations += 3;
        if (bslma::UsesBslmaAllocator<T2>::value)
            minDefaultAllocations += 3;

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
        ASSERT(minDefaultAllocations <= ta0.numBlocksInUse());

        // Compute number of allocations per constructor from supplied
        // allocator.
        int minAllocationsPerAllocator = 0;
        if (bslma::UsesBslmaAllocator<T1>::value)
            minAllocationsPerAllocator += 1;
        if (bslma::UsesBslmaAllocator<T2>::value)
            minAllocationsPerAllocator += 1;

        Int64 numDefaultAllocationsSoFar = ta0.numBlocksInUse();

        // Default construct with supplied allocator
        Obj p4(&ta1); const Obj& P4 = p4;
        ASSERT(NULL_FIRST  == P4.first);
        ASSERT(NULL_SECOND == P4.second);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(numDefaultAllocationsSoFar == ta0.numBlocksInUse());
        ASSERT(minAllocationsPerAllocator <= ta1.numBlocksInUse());

        // Construct with two values and supplied allocator
        Obj p5(VALUE_FIRST, VALUE_SECOND, &ta2); const Obj& P5 = p5;
        ASSERT(VALUE_FIRST  == P5.first);
        ASSERT(VALUE_SECOND == P5.second);
        ASSERT(matchAllocator(P5.first, &ta2));
        ASSERT(matchAllocator(P5.second, &ta2));
        ASSERT(numDefaultAllocationsSoFar == ta0.numBlocksInUse());
        ASSERT(minAllocationsPerAllocator <= ta2.numBlocksInUse());

        // Copy construct with supplied allocator
        Obj p6(P5, &ta3); const Obj& P6 = p6;
        ASSERT(VALUE_FIRST == P6.first);
        ASSERT(VALUE_SECOND == P6.second);
        ASSERT(matchAllocator(P6.first, &ta3));
        ASSERT(matchAllocator(P6.second, &ta3));
        ASSERT(numDefaultAllocationsSoFar == ta0.numBlocksInUse());
        ASSERT(minAllocationsPerAllocator <= ta3.numBlocksInUse());

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

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
        ASSERT(P2 <=> P3 == 0);
        ASSERT(P1 <=> P2 != 0);
        ASSERT(P1 <=> P2 <  0);
        ASSERT(P2 <=> P1 >  0);
        ASSERT(P1 <=> P2 <= 0);
        ASSERT(P2 <=> P1 >= 0);
        ASSERT(P3 <=> P2 <= 0);
        ASSERT(P2 <=> P3 >= 0);

        ASSERT(! (P1 <=> P2 == 0));
        ASSERT(! (P3 <=> P2 != 0));
        ASSERT(! (P2 <=> P1 <  0));
        ASSERT(! (P1 <=> P2 >  0));
        ASSERT(! (P2 <=> P3 <  0));
        ASSERT(! (P3 <=> P2 >  0));
        ASSERT(! (P2 <=> P1 <= 0));
        ASSERT(! (P1 <=> P2 >= 0));
#endif
        // Test assignment
        p1 = P2;
        ASSERT(P1 == P2);
        ASSERT(matchAllocator(P1.first, &ta0));
        ASSERT(matchAllocator(P1.second, &ta0));
        ASSERT(minDefaultAllocations <= ta0.numBlocksInUse());

        numDefaultAllocationsSoFar = ta0.numBlocksInUse();
        p4 = P5;
        ASSERT(P4 == P5);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(numDefaultAllocationsSoFar == ta0.numBlocksInUse());
    }

    // Test that any memory allocated was deallocated
    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)

                         // ===========================
                         // class CloneDisabledTestType
                         // ===========================

class CloneDisabledTestType {
    // This class provides a test object used to check that all members are
    // created inplace during piecewise pair construction.  Methods making
    // clones of its objects are disabled.

  public:
    // PUBLIC TYPES
    typedef bsltf::ArgumentType<1> ArgType;

  private:
    // DATA
    ArgType d_arg;  // value

    // NOT IMPLEMENTED
    CloneDisabledTestType(const CloneDisabledTestType& original);
    CloneDisabledTestType(bslmf::MovableRef<CloneDisabledTestType>);
    CloneDisabledTestType& operator=(const CloneDisabledTestType& rhs);
    CloneDisabledTestType& operator=(
                                 bslmf::MovableRef<CloneDisabledTestType> rhs);

  public:
    // CREATORS
    CloneDisabledTestType();
        // Create an 'CloneDisabledTestType' object having the default
        // attribute value '-1'.

    explicit CloneDisabledTestType(ArgType arg);
        // Create a 'CloneDisabledTestType' object having the specified 'arg'
        // attribute value.

    // ACCESSORS
    const ArgType& arg() const;
        // Return the value of the argument that was passed to the constructor
        // of this object.

    bool isEqual(const CloneDisabledTestType& other) const;
        // Return 'true' if the specified 'other' object has the same value as
        // this object, and 'false' otherwise.  Two 'CloneDisabledTestType'
        // objects have the same value if their corresponding attributes have
        // the same value.
};

// FREE OPERATORS
bool operator==(const CloneDisabledTestType& lhs,
                const CloneDisabledTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'CloneDisabledTestType' objects have
    // the same value if their corresponding attributes have the same value.

                        // ---------------------------
                        // class CloneDisabledTestType
                        // ---------------------------

// CREATORS
CloneDisabledTestType::CloneDisabledTestType()
{}

CloneDisabledTestType::CloneDisabledTestType(ArgType arg)
: d_arg(arg)
{}

// ACCESSORS
inline
const CloneDisabledTestType::ArgType& CloneDisabledTestType::arg() const
{
    return d_arg;
}

inline
bool CloneDisabledTestType::isEqual(const CloneDisabledTestType& other) const
{
    return d_arg == other.d_arg;
}

inline
bool operator==(const CloneDisabledTestType& lhs,
                const CloneDisabledTestType& rhs)
{
    return lhs.isEqual(rhs);
}


class TupleTestDriver {
    // This 'class' is used for testing piecewise construction of 'bsl::pair'
    // object.

    // PRIVATE CLASS METHODS
    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
        // If the second argument is a 'true_type', return the argument moved.
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T& testArg(T& t, bsl::false_type)
        // If the second argument is a 'false_type', return a reference
        // providing non-modifiable access to the argument.
    {
        return t;
    }

    template <class T>
    static bslmf::MovableRef<T> testArg(u::ArgHolder<T>& t, bsl::true_type)
        // If the second argument is a 'true_type', return the held argument,
        // moved.
    {
        return MoveUtil::move(t.arg());
    }

    template <class T>
    static const T& testArg(u::ArgHolder<T>& t, bsl::false_type)
        // If the second argument is a 'false_type', return a reference
        // providing non-modifiable access to the held argument.
    {
        return t.arg();
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
        // Passed as run time 'int's the template args to a 'runTestAlloc' and
        // 'runTestNoAlloc' routines, check for sanity, where the specified
        // 'displayName' is the name of the passed display type.  The thinking
        // here is to shrink code size by handling this in a single,
        // non-inline, non-template routine called by all the different
        // template instantiations.

    template <class FIRST_TYPE,
              int NUM_FIRST_ARGS,
              int NF1,
              int NF2,
              int NF3,
              class SECOND_TYPE,
              int NUM_SECOND_ARGS,
              int NS1,
              int NS2,
              int NS3>
    static void runTestAllocImpl();
        // Construct a 'bsl::pair<FIRST_TYPE, SECOND_TYPE>' with two tuples and
        // a 'bslma'-style allocator argument, where the first tuple contains
        // 'NUM_FIRST_ARGS' elements, and the second 'NUM_SECOND_ARGS'
        // elements.  See the documentation of the public runTestAlloc below
        // for the meaning of 'NF1', 'NF2', 'NF3', 'NS1', 'NS2', and 'NS3'.
        // The behavior is undefined unless each of 'FIRST_TYPE' and
        // 'SECOND_TYPE' is either 'bsltf::EmplacableTestType' or
        // 'bsltf::AllocEmplacableTestType'.

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
        // Call runTestAllocImpl once with every possible pair with element
        // types drawn from the set of 'bsltf::EmplacableTestType' and 
        // bsltf::AllocEmplacableTestType' (4 combinations total), in each case
        // constructing the pair type from two tuples (each with 0-3 elements)
        // and a 'bslma'-style allocator.  The number of args for the first
        // tuple is the specified 'NUM_FIRST_ARGS', and the number for the
        // second is the specified 'NUM_SECOND_ARGS'.  Interpret the values of
        // 'NFi' and 'NSi' as follows:
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
        // parameters are in the range '[0 .. 2]', and for each 'i' in the
        // range '[1 .. 3]', '2 == NFi' if and only if 'i > NUM_FIRST_ARGS',
        // and '2 == NSi' if and only if 'i > NUM_SECOND_ARGS'.

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

    template <class TYPE, int NF, int NS>
    static void runTestInplaceMemberConstruction();
        // Construct one pair of (template parameter) 'TYPE' with two tuples,
        // each taking one argument.  Interpret the values of (template
        // parameters) 'NF' and 'NS' as follows:
        //..
        //  NF == 0 => forward the 'first' argument using copy semantics
        //  NS == 0 => forward the 'second' argument using copy semantics
        //
        //  NF == 1 => forward the 'first' argument using move semantics
        //  NS == 1 => forward the 'second' argument using move semantics
        //..
        // The behavior is undefined unless  'NF' and 'NS' parameters are in
        // the range '[0 .. 1]'.

    static void runTestAllocatorPropagation();
        // Test correctness of allocator propagation performed by all forms of
        // 'bsl::pair' piecewise constructor.
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

template <class FIRST_TYPE,
          int NUM_FIRST_ARGS,
          int NF1,
          int NF2,
          int NF3,
          class SECOND_TYPE,
          int NUM_SECOND_ARGS,
          int NS1,
          int NS2,
          int NS3>
void TupleTestDriver::runTestAllocImpl()
{
    typedef bsl::pair<FIRST_TYPE, SECOND_TYPE>                     Pair;
    typedef u::DisplayType<Pair, NUM_FIRST_ARGS,  NF1, NF2, NF3,
                                 NUM_SECOND_ARGS, NS1, NS2, NS3> DT;
    typedef typename FIRST_TYPE::ArgType01                         FArg1;
    typedef typename FIRST_TYPE::ArgType02                         FArg2;
    typedef typename FIRST_TYPE::ArgType03                         FArg3;
    typedef typename SECOND_TYPE::ArgType01                        SArg1;
    typedef typename SECOND_TYPE::ArgType02                        SArg2;
    typedef typename SECOND_TYPE::ArgType03                        SArg3;

    typedef bslma::UsesBslmaAllocator<FIRST_TYPE> FirstUsesAllocator;
    typedef bslma::UsesBslmaAllocator<SECOND_TYPE> SecondUsesAllocator;

    if (veryVerbose) printf("runTestAlloc(%s, %d, %d,%d,%d, %d, %d,%d,%d);\n",
                            static_cast<const char*>(NameOf<Pair>()),
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
        u::ArgHolder<FArg1> AF1(1,  FirstUsesAllocator(), &aa);
        u::ArgHolder<FArg2> AF2(20, FirstUsesAllocator(), &aa);
        u::ArgHolder<FArg3> AF3(23, FirstUsesAllocator(), &aa);

        u::ArgHolder<SArg1> AS1(2,  SecondUsesAllocator(), &aa);
        u::ArgHolder<SArg2> AS2(18, SecondUsesAllocator(), &aa);
        u::ArgHolder<SArg3> AS3(31, SecondUsesAllocator(), &aa);

        bsls::ObjectBuffer<Pair> oDst;
        Pair *p = oDst.address();

        switch (NUM_FIRST_ARGS) {
          case 0: {
            switch (NUM_SECOND_ARGS) {
              case 0: {
                new (p) Pair(std::piecewise_construct,
                             std::forward_as_tuple(),
                             std::forward_as_tuple(),
                             &aa);
              } break;
              case 1: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                       testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2)),
                          std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2),
                                                testArg(AF3, MOVE_F3)),
                          std::forward_as_tuple(),
                          &aa);
              } break;
              case 1: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2),
                                                testArg(AF3, MOVE_F3)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1)),
                          &aa);
              } break;
              case 2: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2),
                                                testArg(AF3, MOVE_F3)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                testArg(AS2, MOVE_S2)),
                          &aa);
              } break;
              case 3: {
                new (p) Pair(
                          std::piecewise_construct,
                          std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                                testArg(AF2, MOVE_F2),
                                                testArg(AF3, MOVE_F3)),
                          std::forward_as_tuple(testArg(AS1, MOVE_S1),
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

        ASSERTV(name, MOVE_F1, AF1.arg().movedFrom(),
                MOVE_F1 == (MoveState::e_MOVED == AF1.arg().movedFrom()) ||
                    2 == NF1);
        ASSERTV(name, MOVE_F2, AF2.arg().movedFrom(),
                MOVE_F2 == (MoveState::e_MOVED == AF2.arg().movedFrom()) ||
                    2 == NF2);
        ASSERTV(name, MOVE_F3, AF3.arg().movedFrom(),
                MOVE_F3 == (MoveState::e_MOVED == AF3.arg().movedFrom()) ||
                    2 == NF3);

        ASSERTV(name, MOVE_S1, AS1.arg().movedFrom(),
                MOVE_S1 == (MoveState::e_MOVED == AS1.arg().movedFrom()) ||
                    2 == NS1);
        ASSERTV(name, MOVE_S2, AS2.arg().movedFrom(),
                MOVE_S2 == (MoveState::e_MOVED == AS2.arg().movedFrom()) ||
                    2 == NS2);
        ASSERTV(name, MOVE_S3, AS3.arg().movedFrom(),
                MOVE_S3 == (MoveState::e_MOVED == AS3.arg().movedFrom()) ||
                    2 == NS3);

        const FIRST_TYPE& F = p->first;

        ASSERTV(name, 1  == F.arg01() || 2 == NF1);
        ASSERTV(name, 20 == F.arg02() || 2 == NF2);
        ASSERTV(name, 23 == F.arg03() || 2 == NF3);

        const SECOND_TYPE& S = p->second;

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

    const bool firstCopyHappened  = !NF1 || !NF2 || !NF3;
    const bool secondCopyHappened = !NS1 || !NS2 || !NS3;
    const bool fua = FirstUsesAllocator::value;
    const bool sua = SecondUsesAllocator::value;

    ASSERTV(name, da.numAllocations(),
            fua, firstCopyHappened,
            sua, secondCopyHappened,
            (fua && firstCopyHappened) ||
                (sua && secondCopyHappened) == (0 < da.numAllocations()));
    ASSERTV(name, aa.numAllocations(), fua, sua,
            (fua || sua) == (0 < aa.numAllocations()));
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
    runTestAllocImpl<bsltf::EmplacableTestType,
                     NUM_FIRST_ARGS, NF1, NF2, NF3,
                     bsltf::EmplacableTestType,
                     NUM_SECOND_ARGS, NS1, NS2, NS3>();
    runTestAllocImpl<bsltf::AllocEmplacableTestType,
                     NUM_FIRST_ARGS, NF1, NF2, NF3,
                     bsltf::EmplacableTestType,
                     NUM_SECOND_ARGS, NS1, NS2, NS3>();
    runTestAllocImpl<bsltf::EmplacableTestType,
                     NUM_FIRST_ARGS, NF1, NF2, NF3,
                     bsltf::AllocEmplacableTestType,
                     NUM_SECOND_ARGS, NS1, NS2, NS3>();
    runTestAllocImpl<bsltf::AllocEmplacableTestType,
                     NUM_FIRST_ARGS, NF1, NF2, NF3,
                     bsltf::AllocEmplacableTestType,
                     NUM_SECOND_ARGS, NS1, NS2, NS3>();
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
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(),
                         std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                               testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2)),
                         std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                               testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
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
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2),
                                               testArg(AF3, MOVE_F3)),
                         std::forward_as_tuple());
          } break;
          case 1: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2),
                                               testArg(AF3, MOVE_F3)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1)));
          } break;
          case 2: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2),
                                               testArg(AF3, MOVE_F3)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
                                                      testArg(AS2, MOVE_S2)));
          } break;
          case 3: {
            new (p) Pair(std::piecewise_construct,
                         std::forward_as_tuple(testArg(AF1, MOVE_F1),
                                               testArg(AF2, MOVE_F2),
                                               testArg(AF3, MOVE_F3)),
                         std::forward_as_tuple(testArg(AS1, MOVE_S1),
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

template <class TYPE, int NF, int NS>
void TupleTestDriver::runTestInplaceMemberConstruction()
{
    typedef bsl::pair<TYPE, TYPE>  Pair;
    typedef typename TYPE::ArgType Arg;

    const char *name = NameOf<TYPE>();
    if (veryVerbose) printf("runTestInplaceMemberConstruction(%s, %d, %d);\n",
                            name,
                            NF,
                            NS);

    ASSERTV(name, 0 <= NF && NF <= 1);
    ASSERTV(name, 0 <= NF && NF <= 1);

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NF == 1> MOVE_F = {};
    static const bsl::integral_constant<bool, NS == 1> MOVE_S = {};

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    Arg AF(1);
    Arg AS(2);

    bsls::ObjectBuffer<Pair>  oDst;
    Pair                     *p = oDst.address();

    new (p) Pair(std::piecewise_construct,
                 std::forward_as_tuple(testArg(AF, MOVE_F)),
                 std::forward_as_tuple(testArg(AS, MOVE_S)));

    ASSERTV(name, MOVE_F, AF.movedFrom(),
            MOVE_F == (MoveState::e_MOVED == AF.movedFrom()));

    ASSERTV(name, MOVE_S, AS.movedFrom(),
            MOVE_S == (MoveState::e_MOVED == AS.movedFrom()));

    const TYPE& F = p->first;

    ASSERTV(name, 1  == F.arg());

    const TYPE& S = p->second;

    ASSERTV(name, 2  == S.arg());

    ASSERTV(name, da.numAllocations(), 0 == da.numAllocations());
}

void TupleTestDriver::runTestAllocatorPropagation()
{

    typedef bsl::pair<my_NoAllocString, my_NoAllocString>       NoAllocPair;
    typedef bsl::pair<my_String, my_String>                     BslmaAllocPair;
    typedef bsl::pair<my_BslmaAllocArgStr, my_BslmaAllocArgStr> ArgAllocPair;

    bslma::TestAllocator          da("default", veryVeryVeryVerbose);
    bslma::TestAllocator          oa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator         *dma = my_STLCharAlloc::defaultMechanism();
    bslma::DefaultAllocatorGuard  dag(&da);

    ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
    ASSERTV(oa.numBytesInUse(),   0 == oa.numBytesInUse());
    ASSERTV(dma->numBytesInUse(), 0 == dma->numBytesInUse());
    {
        // 'my_NoAllocString' object uses 'my_STLCharAlloc::defaultMechanism()'
        // if allocator wasn't specified by client on construction.

        NoAllocPair    npa(std::piecewise_construct,
                           std::forward_as_tuple("a"),
                           std::forward_as_tuple("b"));

        ASSERTV(dma == npa.first.allocator());
        ASSERTV(dma == npa.second.allocator());
        ASSERTV(0   == strcmp("a", npa.first.c_str()));
        ASSERTV(0   == strcmp("b", npa.second.c_str()));

        // 'my_String' object uses 'bslma::Default::defaultAllocator()' if
        // allocator wasn't specified by client on construction.

        BslmaAllocPair bpa(std::piecewise_construct,
                           std::forward_as_tuple("c"),
                           std::forward_as_tuple("d"));

        ASSERTV(&da == bpa.first.allocator());
        ASSERTV(&da == bpa.second.allocator());
        ASSERTV(0   == strcmp("c", bpa.first.c_str()));
        ASSERTV(0   == strcmp("d", bpa.second.c_str()));

        // 'my_BslmaAllocArgStr' object uses
        // 'bslma::Default::defaultAllocator()' if allocator wasn't specified
        // by client on construction.

        ArgAllocPair   apa(std::piecewise_construct,
                           std::forward_as_tuple("e"),
                           std::forward_as_tuple("f"));

        ASSERTV(&da == apa.first.allocator());
        ASSERTV(&da == apa.second.allocator());
        ASSERTV(0   == strcmp("e", apa.first.c_str()));
        ASSERTV(0   == strcmp("f", apa.second.c_str()));

        ASSERTV(da.numBytesInUse(),   8 == da.numBytesInUse());
        ASSERTV(oa.numBytesInUse(),   0 == oa.numBytesInUse());
        ASSERTV(dma->numBytesInUse(), 4 == dma->numBytesInUse());
    }

    ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
    ASSERTV(oa.numBytesInUse(),   0 == oa.numBytesInUse());
    ASSERTV(dma->numBytesInUse(), 0 == dma->numBytesInUse());

    {
        // The allocator argument in this case should be ignored since
        // 'bslma::UsesBslmaAllocator<my_NoAllocString>' does not inherit from
        // 'bsl::true_type'.
        NoAllocPair    npa(std::piecewise_construct,
                           std::forward_as_tuple("c"),
                           std::forward_as_tuple("d"),
                           &oa);

        ASSERTV(dma == npa.first.allocator());
        ASSERTV(dma == npa.second.allocator());
        ASSERTV(0   == strcmp("c", npa.first.c_str()));
        ASSERTV(0   == strcmp("d", npa.second.c_str()));

        ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
        ASSERTV(oa.numBytesInUse(),   0 == oa.numBytesInUse());
        ASSERTV(dma->numBytesInUse(), 4 == dma->numBytesInUse());
    }

    {
        BslmaAllocPair bpa(std::piecewise_construct,
                           std::forward_as_tuple("c"),
                           std::forward_as_tuple("d"),
                           &oa);

        ASSERTV(&oa == bpa.first.allocator());
        ASSERTV(&oa == bpa.second.allocator());
        ASSERTV(0   == strcmp("c", bpa.first.c_str()));
        ASSERTV(0   == strcmp("d", bpa.second.c_str()));

        ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
        ASSERTV(oa.numBytesInUse(),   4 == oa.numBytesInUse());
        ASSERTV(dma->numBytesInUse(), 0 == dma->numBytesInUse());
    }

    ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
    ASSERTV(oa.numBytesInUse(),   0 == oa.numBytesInUse());
    ASSERTV(dma->numBytesInUse(), 0 == dma->numBytesInUse());

    {
        ArgAllocPair   apa(std::piecewise_construct,
                           std::forward_as_tuple("e"),
                           std::forward_as_tuple("f"),
                           &oa);

        ASSERTV(&oa == apa.first.allocator());
        ASSERTV(&oa == apa.second.allocator());
        ASSERTV(0   == strcmp("e", apa.first.c_str()));
        ASSERTV(0   == strcmp("f", apa.second.c_str()));

        ASSERTV(da.numBytesInUse(),   0 == da.numBytesInUse());
        ASSERTV(oa.numBytesInUse(),   4 == oa.numBytesInUse());
        ASSERTV(dma->numBytesInUse(), 0 == dma->numBytesInUse());
    }
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
    static void testCase14(bsl::false_type pairAllocates);
    static void testCase14(bsl::true_type  pairAllocates);
    static void testCase14();
        // Test constructor from 'std::pair' in contexts with nested
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

    static void testCase10();
        // Testing move constructors moving the two elements of the pair into
        // the c'tor separately.

    static void testCase9();
        // Testing pair to pair move c'tors.
};

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase14(
                                                               bsl::false_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case14, %s\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name(),
                            "no alloc");

    ASSERT(false == k_ALLOC);

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVeryVerbose);
    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    for (int bb = 0; bb < 2; ++bb) {
        const bool b = bb;

        // pair in 'first' element

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<FromPair, bool> tp(std::make_pair(fp, b));
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<ToPair, bool> tp(std::make_pair(fp, b));
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<FromPair, bool> np(fp, b);

            bsl::pair<FromPair, bool> tp(np);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<FromPair, bool> np(fp, b);

            bsl::pair<ToPair, bool> tp(np);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        // pair in 'second' element

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, FromPair> tp(std::make_pair(b, fp));
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, ToPair> tp(std::make_pair(b, fp));
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, FromPair> tp(np);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, ToPair> tp(np);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));
        }

        ASSERT(0 == ta.numAllocations());
        ASSERT(0 == tb.numAllocations());
        ASSERT(0 == da.numAllocations());
    }
}
template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase14(
                                                                bsl::true_type)
{
    if (veryVerbose) printf("TD<%s, %s>::case14, %s\n",
                            NameOf<ToFirst>().name(),
                            NameOf<ToSecond>().name(),
                            "alloc");

    ASSERT(true == k_ALLOC);

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

            bsl::pair<FromPair, bool> tp(std::make_pair(fp, b), &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<ToPair, bool> tp(std::make_pair(fp, b), &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<FromPair, bool> np(fp, b);

            bsl::pair<FromPair, bool> tp(np, &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<FromPair, bool> np(fp, b);

            bsl::pair<ToPair, bool> tp(np, &tb);
            ASSERT(b == tp.second);
            ASSERT('F'                    == u::valueOf(tp.first.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.first.second));

            ASSERT(u::allocatorMatches(tp.first.first,  &tb));
            ASSERT(u::allocatorMatches(tp.first.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        // pair in 'second' element

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, FromPair> tp(std::make_pair(b, fp), &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            bsl::pair<bool, ToPair> tp(std::make_pair(b, fp), &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, FromPair> tp(np, &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();

        {
            bsls::ObjectBuffer<FromPair> ofp;
            FromPair& fp = u::initPair(&ofp, 'F', &ta);
            u::PairGuard<FromPair> fpg(&fp);

            std::pair<bool, FromPair> np(b, fp);

            bsl::pair<bool, ToPair> tp(np, &tb);
            ASSERT(b == tp.first);
            ASSERT('F'                    == u::valueOf(tp.second.first));
            ASSERT('F' + u::k_VALUE_SHIFT == u::valueOf(tp.second.second));

            ASSERT(u::allocatorMatches(tp.second.first,  &tb));
            ASSERT(u::allocatorMatches(tp.second.second, &tb));
        }

        ASSERT(taSoFar < ta.numAllocations());
        ASSERT(tbSoFar < tb.numAllocations());
        ASSERT(daSoFar < da.numAllocations());
        taSoFar = ta.numAllocations();
        tbSoFar = tb.numAllocations();
        daSoFar = da.numAllocations();
    }
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase14()
{
    // Dispatch depending on whether any of the parameter types allocate
    // memory.

    testCase14(UsesBslma());
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

    // Ensure that we can pass an allocator argument anyway, which is ignored.
    bsls::ObjectBuffer<FromPair> ofp;
    FromPair& fp = u::initPair(&ofp, 'A', &ta);        const FromPair& FP = fp;
    u::PairGuard<FromPair> fpg(&fp);

    ASSERT(u::isNotMovedInto(fp));
    ASSERT(u::isNotMovedFrom(fp));
    ASSERT('A' == u::valueOf(fp));

    const ToPair tp(FP.first, MoveUtil::move(fp.second), &ta);

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
    u::initPair(&ofp, 'B', &ta);
    ASSERT('B' == u::valueOf(fp));

    const ToPair tpa(MoveUtil::move(fp.first), FP.second, &ta);

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
    u::initPair(&ofp, 'C', &ta);
    ASSERT('C' == u::valueOf(fp));

    const ToPair tpb(FP.first, FP.second, &ta);

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
    u::initPair(&ofp, 'D', &ta);
    ASSERT('D' == u::valueOf(fp));

    const ToPair tpc(FP, &ta);

    ASSERT(u::isNotMovedFrom(fp.first));
    ASSERT(u::isNotMovedInto(fp.first));
    ASSERT(u::isNotMovedFrom(fp.second));
    ASSERT(u::isNotMovedInto(fp.second));
    ASSERT(u::isNotMovedInto(tpc.first));
    ASSERT(u::isNotMovedFrom(tpc.first));
    ASSERT(u::isNotMovedInto(tpc.second));
    ASSERT(u::isNotMovedFrom(tpc.second));

    ASSERT('D' == u::valueOf(tpc));

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
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase10()
{
    if (veryVerbose) printf("TD<%s, %s>::case10\n",
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
        // lvalue references to const and non-'const' values are being
        // processed correctly.

        {
            typedef ManagedWrapper<ToFirst>              WrappedType;
            typedef bsl::pair<WrappedType, int *>        ManagedType;
            bslma::ManagedPtr<ToFirst> mp;
            ManagedType mt(mp, 0);
        }
        {
            typedef ManagedWrapper<ToFirst>              WrappedType;
            typedef bsl::pair<int *, WrappedType>        ManagedType;
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

    ASSERTV(k_ALLOC, ta.numAllocations(),
                                   (0 < k_ALLOC) == (0 < ta.numAllocations()));
    ASSERTV(k_ALLOC, tb.numAllocations(),
                                   (0 < k_ALLOC) == (0 < tb.numAllocations()));
    ASSERT(da.numAllocations() == numDeliberateDefaultAllocs);
}

template <class TO_FIRST, class TO_SECOND, class FROM_FIRST, class FROM_SECOND>
void TestDriver<TO_FIRST, TO_SECOND, FROM_FIRST, FROM_SECOND>::testCase9()
{
    if (veryVerbose) printf("TD<%s, %s>::case9\n",
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

    ASSERTV(k_ALLOC, ta.numAllocations(),
                                   (0 < k_ALLOC) == (0 < ta.numAllocations()));
    ASSERTV(k_ALLOC, tb.numAllocations(),
                                   (0 < k_ALLOC) == (0 < tb.numAllocations()));
    ASSERT(da.numAllocations() == numDeliberateDefaultAllocs);
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
    // where 'ToPair' should be implicitly convertible from a 'FromPair'.  We
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
template<class T>
struct TupleConversionDriver
    // This utility class template provides functions for testing conversion to
    // 'std::tuple' operator.
{
    static void operatorTest()
        // Formally test coversion operator.
    {
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        int value = 0;

        T   t(  value);
        int i(++value);

        bsl::pair<  T, int> mX1(t, i);
        bsl::pair<int,   T> mX2(i, t);

        ASSERTV(t == mX1.first);
        ASSERTV(t == mX2.second);
        ASSERTV(i == mX1.second);
        ASSERTV(i == mX2.first);

        std::tuple<  T&, int&>       mY1 = mX1.operator std::tuple<T&, int&>();
        const std::tuple<  T&, int&> Y1  = mY1;
        std::tuple<int&,   T&>       mY2 = mX2.operator std::tuple<int&, T&>();
        const std::tuple<int&,   T&> Y2  = mY2;

        ASSERTV(t == std::get<0>(Y1));
        ASSERTV(t == std::get<1>(Y2));
        ASSERTV(i == std::get<1>(Y1));
        ASSERTV(i == std::get<0>(Y2));

        // Changing 'bsl::pair'.

        t.setData(++value);
        mX1.first.setData(value);
        mX2.second.setData(value);

        i          = ++value;
        mX1.second = i;
        mX2.first  = i;

        ASSERTV(t == std::get<0>(Y1));
        ASSERTV(t == std::get<1>(Y2));
        ASSERTV(i == std::get<1>(Y1));
        ASSERTV(i == std::get<0>(Y2));

        // Changing 'std::tuple'.

        t.setData(++value);
        std::get<0>(Y1).setData(value);
        std::get<1>(Y2).setData(value);

        i               = ++value;
        std::get<1>(Y1) = i;
        std::get<0>(Y2) = i;

        ASSERTV(t == mX1.first);
        ASSERTV(t == mX2.second);
        ASSERTV(i == mX1.second);
        ASSERTV(i == mX2.first);
    }

    static void tieTest()
        // Check compatibility of 'bsl::pair' with 'std::tie()' function.
    {
        if (veryVeryVerbose) printf("\t\twith %s\n", NameOf<T>().name());

        T lStorage(0);
        T rStorage(0);

        int value = 0;
        T   lValue1(++value);
        T   rValue1(++value);

        bsl::pair<T, T> mX1(lValue1, rValue1);

        ASSERT(T(0) == lStorage);
        ASSERT(T(0) == rStorage);

        std::tie(std::ignore, std::ignore) = mX1;

        ASSERT(T(0) == lStorage);
        ASSERT(T(0) == rStorage);

        std::tie(lStorage, std::ignore) = mX1;

        ASSERT(lValue1 == lStorage);
        ASSERT(T(0)    == rStorage);

        T lValue2(++value);
        T rValue2(++value);

        bsl::pair<T, T> mX2(lValue2, rValue2);

        std::tie(std::ignore, rStorage) = mX2;

        ASSERT(lValue1 == lStorage);
        ASSERT(rValue2 == rStorage);

        T lValue3(++value);
        T rValue3(++value);

        bsl::pair<T, T> mX3(lValue3, rValue3);

        std::tie(lStorage, rStorage) = mX3;

        ASSERT(lValue3 == lStorage);
        ASSERT(rValue3 == rStorage);
    }
};
#endif

#if defined(BSLSTL_PAIR_TEST_CONDITIONAL_DEFAULT_CTOR)

namespace IsDefaultConstructibleTestTypes {

                // Types to Test Default Constructibility

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
    struct DeletedDefault {
        DeletedDefault() = delete;
    };

    struct DeletedDefault2 : DeletedDefault {
    };
#endif

    struct NoDefault {
        NoDefault(int);
    };

    struct NoDefault2 : NoDefault {};

    struct Empty {};

    struct DefArgDefault {
        DefArgDefault(int * = 0);
    };

    struct ExpDefArgDef {
        explicit ExpDefArgDef(int * = 0);
    };

    class PrivDefault {
        PrivDefault();
    };

    class PrivDefault2 : public PrivDefault {
    };
}  // close namespace IsDefaultConstructibleTestTypes
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::pair' cannot be deduced from the constructor parameters.
    //..
    // pair()
    // pair(bslma::Allocator *)
    // pair(piecewise_construct_t, ...);
    // pair(piecewise_construct_t, ..., bslma::Allocator *);
    // All the converting constructors
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void BslPairConstructors ()
        // Test that constructing a 'bsl::pair' from a 'bsl::pair' and
        // (optionally) an allocator deduces the correct type.
        //..
        // pair(const pair&  t) -> decltype(t)
        // pair(      pair&& t) -> decltype(t)
        // pair(const pair&  t, bslma::Allocator *) -> decltype(t)
        // pair(      pair&& t, bslma::Allocator *) -> decltype(t)
        //..
    {
        my_String             s ("abc");
        bslma::Allocator     *a1 = NULL;
        bslma::TestAllocator *a2 = NULL;

        bsl::pair<double, long> const p1(0.0, 42L);
        bsl::pair                     p1a(p1);
        ASSERT_SAME_TYPE(decltype(p1a), bsl::pair<double, long>);

        bsl::pair<my_String, void*> p2("abc", nullptr);
        bsl::pair                   p2a(std::move(p2));
        ASSERT_SAME_TYPE(decltype(p2a), bsl::pair<my_String, void*>);

        bsl::pair<my_String, void*> p3("abc", nullptr);
        bsl::pair                   p3a(p3, a1);
        bsl::pair                   p3b(p3, a2);
        ASSERT_SAME_TYPE(decltype(p3a), bsl::pair<my_String, void*>);
        ASSERT_SAME_TYPE(decltype(p3b), bsl::pair<my_String, void*>);

        bsl::pair<my_String, void*> p4("abc", nullptr);
        bsl::pair                   p4a(std::move(p4), a1);
        bsl::pair                   p4b(std::move(p4), a2);
        ASSERT_SAME_TYPE(decltype(p4a), bsl::pair<my_String, void*>);
        ASSERT_SAME_TYPE(decltype(p4b), bsl::pair<my_String, void*>);

        bsl::pair<double, long> const p5(0.0, 42L);
        bsl::pair                     p5a(a1, p5);
        ASSERT_SAME_TYPE(decltype(p5a), bsl::pair<bslma::Allocator *,
                                                  bsl::pair<double, long>>);
    }

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::pair' from a two arguments and
        // (optionally) an allocator deduces the correct type.
        //..
        // pair(const T1&,  const T2& ) -> pair<T1, T2>
        // pair(      T1&&,       T2&&) -> pair<T1, T2>
        // pair(const T1&, const T2&, bslma::Allocator *) -> pair<T1, T2>
        //..
    {
        int                   x = 42;
        my_String             s ("abc");
        bslma::Allocator     *a1 = NULL;
        bslma::TestAllocator *a2 = NULL;

        bsl::pair p1("abc", x);
        ASSERT_SAME_TYPE(decltype(p1), bsl::pair<const char*, int>);

        bsl::pair p2(std::move(x), std::move(s));
        ASSERT_SAME_TYPE(decltype(p2), bsl::pair<int, my_String>);

        bsl::pair p3a(s, 42, a1);
        bsl::pair p3b(42, s, a2);
        ASSERT_SAME_TYPE(decltype(p3a), bsl::pair<my_String, int>);
        ASSERT_SAME_TYPE(decltype(p3b), bsl::pair<int, my_String>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests

//#define BSLSTL_PAIR_COMPILE_FAIL_NOT_AN_ALLOCATOR
#if defined(BSLSTL_PAIR_COMPILE_FAIL_NOT_AN_ALLOCATOR)
        my_String *ps = nullptr;
        bsl::pair  p99(s, 42, ps);
        // this should fail to compile
#endif
    }

    static void StdPairConstructors ()
        // Test that constructing a 'bsl::pair' from a 'std::pair' and
        // (optionally) an allocator deduces the correct type.
        //..
        // pair(const std::pair<T1, T2>&)  -> pair<T1, T2>
        // pair(      std::pair<T1, T2>&&) -> pair<T1, T2>
        // pair(const std::pair<T1, T2>&,  bslma::Allocator *) -> pair<T1, T2>
        // pair(      std::pair<T1, T2>&&, bslma::Allocator *) -> pair<T1, T2>
        //..
    {
        my_String             s ("abc");
        bslma::Allocator     *a1 = NULL;
        bslma::TestAllocator *a2 = NULL;

        std::pair<const char*, int> p1("abc", 42);
        bsl::pair                   p1a(p1);
        ASSERT_SAME_TYPE(decltype(p1a), bsl::pair<const char*, int>);

        std::pair<my_String, void*> p2(s, nullptr);
        bsl::pair                   p2a(std::move(p2));
        ASSERT_SAME_TYPE(decltype(p2a), bsl::pair<my_String, void*>);

        std::pair<my_String, int> p3(s, 42);
        bsl::pair                 p3a(p3, a1);
        bsl::pair                 p3b(p3, a2);
        ASSERT_SAME_TYPE(decltype(p3a), bsl::pair<my_String, int>);
        ASSERT_SAME_TYPE(decltype(p3b), bsl::pair<my_String, int>);

        std::pair<my_String, void*> p4(s, nullptr);
        bsl::pair                   p4a(std::move(p4), a1);
        bsl::pair                   p4b(std::move(p4), a2);
        ASSERT_SAME_TYPE(decltype(p4a), bsl::pair<my_String, void*>);
        ASSERT_SAME_TYPE(decltype(p4b), bsl::pair<my_String, void*>);

        std::pair<double, long> const p5(0.0, 42L);
        bsl::pair                     p5a(a1, p5);
        ASSERT_SAME_TYPE(decltype(p5a),
                                    bsl::pair<bslma::Allocator *,
                                              std::pair<double, long>>);
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

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

    bslma::TestAllocator defaultMainAllocator(veryVeryVeryVerbose);
    int                  status;
    status = bslma::Default::setDefaultAllocator(&defaultMainAllocator);
    ASSERTV(status, 0 == status);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 28: {
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
      case 27: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Simple two argument constructors deduce the template arguments.
        //:
        //: 2 Providing an allocator as the third argument does not suppress
        //:   deduction.
        //:
        //: 3 Constructing a 'pair' from 'std::pair' deduces the
        //:   template arguments.
        //
        //: 4 Constructing a 'pair' from 'pair' (std or bsl) and an
        //:   allocator deduces a pair + allocator, rather than a
        //:   pair<pair, Allocator>.
        //
        // Plan:
        //: 1 Create a pair by invoking the constructor without supplying the
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
      case 26: {
        // --------------------------------------------------------------------
        // TESTING FIX FOR DRQS 131875306
        //  This test case concerns report that a user was unable to emplace
        //  a non-copyable type into a bsl::unordered_map.  The fix was to
        //  rework the constraints on the converting constructor for
        //  'bsl::pair'.
        //
        // Concerns:
        //: 1 Pre-existing code does not fail to compile with thes changes.
        //
        // Plan:
        //: 1 Provide a minimal code example of the kind of valid code that was
        //:   surprisingly failing to compile.  This test remains as a canary
        //:   should a further regression introduce a similar problem.
        //
        // Testing:
        //   Concern: can construct pair of objects that are not copyable
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FIX FOR DRQS 131875306"
                            "\n==============================\n");
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        bsl::pair<NonCopyable131875306, int> o1(1,2);
        bsl::pair<int, NonCopyable131875306> o2(1,2);
            // These would fail to compile, reporting an attempt to access
            // private constructors prior to applying the patch for the ticket
            // above.
        ASSERT(1 == o1.first.get());
        ASSERT(2 == o1.second);
        ASSERT(1 == o2.first);
        ASSERT(2 == o2.second.get());
#endif
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: RETURN BY BRACE INITIALIZATION
        //  This surprising test case arose when code stopped building after an
        //  "obvious" simplification of 'pair'.  The syntax may not be BDE's
        //  notion of good style, but is valid C++11 and should be supported.
        //
        // Concerns:
        //: 1 'pair's can be constructed by providing brace-initializaers for
        //:   each member, which should construct and move a temporary with
        //:   that value.
        //
        // Plan:
        //: 1 ....
        //
        // Testing:
        //   Concern: 'return' by brace initialization
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "\nTESTING CONCERN: RETURN BY BRACE INITIALIZATION"
                        "\n===============================================\n");

#if __cplusplus >= 201103L
        typedef bsl::pair<int, int> Obj;

        auto call = [](Obj arg) -> Obj {
            return { {arg.first}, arg.second };
        };

        Obj mX = call({3, {4}});   const Obj& X = mX;

        ASSERTV(X.first,  3 == X.first);
        ASSERTV(X.second, 4 == X.second);
#endif

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: '0' AS NULL POINTER LITERAL
        //  This surprising test case arose when code stopped building after an
        //  "obvious" simplification of 'pair'.  The issue is that '0' becomes
        //  an 'int' when passed through perfect-forwarding, even where the
        //  intent was that it be a null pointer.
        //
        // Concerns:
        //: 1 Constructors for (appropriate) smart pointers should accept the
        //:   zero literal, '0', as a valid null-pointer literal.
        //
        // Plan:
        //: 1 ....
        //
        // Testing:
        //   Concern: construct from '0' as null pointer literal
        // --------------------------------------------------------------------

        if (verbose) printf(
                           "\nTESTING CONCERN: '0' AS NULL POINTER LITERAL"
                           "\n============================================\n");

        {
            typedef bsl::pair<const char *, int         > ObjL;
            typedef bsl::pair<int,          const char *> ObjR;
            typedef bsl::pair<const char *, const char *> ObjP;

            ObjL mX(0, 0);
            ObjR mY(0, 0);
            ObjP mZ(0, 0);
            (void) mX;
            (void) mY;
            (void) mZ;
        }

#if 0   // This test requires a 'bslmf' implementation of 'is_constructible' in
        // order to properly constrain the constructor template that greedily
        // matches '0' as an 'int'.
        {
            typedef bsl::pair<const char *, bsltf::MoveOnlyAllocTestType> ObjL;
            typedef bsl::pair<const char *, bsltf::MoveOnlyAllocTestType> ObjR;
            typedef bsl::pair<bsltf::MoveOnlyAllocTestType,
                              bsltf::MoveOnlyAllocTestType>               ObjP;

            ObjL mX(0, 0, 0);
            ObjR mY(0, 0, 0);
            ObjP mZ(0, 0, 0);
        }
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CONSTRUCTOR SFINAE
        //
        // Concerns:
        //: 1 All 'pair' constructors for complete types should fail to compile
        //:   by dropping out of the overload set in a SFINAE-friendly manner
        //:   if they would otherwise not instantiate.
        //
        // Plan:
        //: 1 Use 'is_constructible' type trait to determine whether ill-formed
        //:   constructors drop out of overload resolution through SFINAE.
        //:   Note that this test may rely on the C++11 library implementation
        //:   of <type_traits>.
        //
        // Testing:
        //   Concern: 'pair' constructors SFINAE when required by standard
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONCERN: CONSTRUCTOR SFINAE"
                            "\n===================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)                     \
 || (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
        // MSVC 2013 does not support expression-SFINAE well enough to handle
        // non-constructible cases.

        if (verbose) printf("Test not supported without native type traits\n");
#else

        if (verbose) printf("\tDefault constructors\n");
        {
            ASSERTV( (std::is_constructible<bsl::pair<int,
                                                      int > >::value));

            ASSERTV( (std::is_constructible<bsl::pair<const int,
                                                            int> >::value));
            ASSERTV( (std::is_constructible<bsl::pair<      int,
                                                      const int> >::value));
            ASSERTV( (std::is_constructible<bsl::pair<const int,
                                                      const int> >::value));

            ASSERTV(!(std::is_constructible<bsl::pair<int&,
                                                      int > >::value));
            ASSERTV(!(std::is_constructible<bsl::pair<int ,
                                                      int&> >::value));
            ASSERTV(!(std::is_constructible<bsl::pair<int&,
                                                      int&> >::value));
        }
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: PAIR OF REFERENCES
        //
        // Concerns:
        //: 1 'pair' can be instantiated and constructed with elements that are
        //:   references to (cv-qualified) complete object types.
        //: 2 'pair' of reference to complete object type support copy and
        //:   move assignment if the referenced object type supports copy and
        //:   move assignment.
        //: 3 'pair' of reference to base-class can be constructed and assigned
        //:   to by objects and references to derived types, as long as the
        //:   derived object/reference is no more cv-qualified than the base
        //:   reference.
        //: 4 'pair' of reference to incomplete type is supported and can be
        //:   constructed and copied, although assignment and 'swap' would
        //:   require the type to be complete before such calls.
        //: 5 Concerns (3) and (4) apply when constructing/assigning from a
        //:   type that is convertible to the corresponding reference type.
        //: 6 'pair' of function references can be constructed and copied, but
        //:   not assigned to or swapped.
        //: 7 'pair' of reference to object type is swappable if both member
        //:   types are swappable.  Note that member types nned not be complete
        //:   to be swappable, as an appropriate 'swap' function may be
        //:   discovered through ADL.
        //: 8 Accessors and manipulators for reference members act on the
        //:   bound (referenced) object, not on the reference itself.
        //
        // Plan:
        //: 1 ...
        //
        // Testing:
        //   Concern: pairs of references work correctly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONCERN: PAIR OF REFERENCES"
                            "\n====================================\n");

        if (verbose) printf("\twith lvalue-references\n");

        if (verbose) printf("\t\twith simple reference for 'first'\n");
        {
            typedef bsl::pair<int&, int> ObjL;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mB(a, b);  const ObjL& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjL mC = B;    const ObjL& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

            typedef bsl::pair<int&,  double> Obj;
            if (veryVerbose) printf(
                          "\t\treference object for converting constructor\n");

            Obj mA(b, 3.14);     const Obj& A = mA;

            ASSERTV(A.first,  42   == A.first);
            ASSERTV(A.second, 3.14 == A.second);

            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjL mD = mA;   const ObjL& D = mD;

            ASSERTV(D.first,  42 == D.first);
            ASSERTV(D.second,  3 == D.second);

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mE = MoveUtil::move(B);     const ObjL& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);

#if 0       // Cannot bind moved reference to an lvalue reference, but the
            // regular move constructor works?
            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjL mF = MoveUtil::move(mA);     const ObjL& F = mF;

            ASSERTV(F.first,  42 == F.first);
            ASSERTV(F.second,  3 == F.second);
#endif
            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

#if 0
            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(F);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);
#endif

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));
#if 0
            ASSERT(!(B == F));
            ASSERT( (B != F));
            ASSERT(!(B <  F));
            ASSERT(!(B <= F));
            ASSERT( (B >= F));
            ASSERT( (B >  F));
#endif
        }

        if (verbose) printf("\t\twith simple reference for 'second'\n");
        {
            typedef bsl::pair<int, int>  Obj;
            typedef bsl::pair<int, int&> ObjR;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\treference object without 'const'\n");

            Obj mA;         const Obj& A = mA;

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjR mB(a, b);  const ObjR& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjR mC = B;    const ObjR& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

#if 0
            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjR mD = mA;   const ObjR& D = mD;

            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);
#endif
            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjR mE = MoveUtil::move(B);     const ObjR& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);

#if 0
            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjR mF = MoveUtil::move(mA);     const ObjR& F = mF;

            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);
#endif
            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

#if 0
            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(F);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);
#endif
            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));
#if 0
            ASSERT(!(B == F));
            ASSERT( (B != F));
            ASSERT(!(B <  F));
            ASSERT(!(B <= F));
            ASSERT( (B >= F));
            ASSERT( (B >  F));
#endif
        }

        if (verbose) printf("\t\twith two simple references\n");
        {
            typedef bsl::pair<int,  int>  Obj;
            typedef bsl::pair<int&, int&> ObjP;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\treference object without 'const'\n");

            Obj mA;         const Obj& A = mA;

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjP mB(a, b);  const ObjP& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjP mC = B;    const ObjP& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

#if 0
            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjP mD = A;    const ObjP& D = mD;

            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);
#endif
            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjP mE = MoveUtil::move(B);     const ObjP& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);
#if 0
            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjP mF = MoveUtil::move(A);     const ObjP& F = mF;

            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);
#endif
            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);
#if 0
            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(D);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);
#endif
            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));

#if 0
            ASSERT(!(B == D));
            ASSERT( (B != D));
            ASSERT(!(B <  D));
            ASSERT(!(B <= D));
            ASSERT( (B >= D));
            ASSERT( (B >  D));
#endif
        }

#if defined(BSLSTL_PAIR_CORRECTLY_FORWARDS_ALLOCATORS)  // NEVER DEFINED
        // NOTE: The tests below varify that allocators are correctly forwarded
        // through perfect forwarding constructors, especially when such
        // forwarding turns into a move construction.  The implementation we
        // have inherited from older BDE does not do this correctly, so while
        // the intent is to update the contract to pass these tests below, they
        // are currently disabled pending a follow-up piece of work.

        if (verbose) printf("\twith allocator-aware types\n");
            // TBD REVISE COMMENT
            // Note that an array can never be allocator aware, so we test
            // allocator-aware behavior for only the non-array member.  This
            // is needed to test the allocator-aware constructors.


#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 &&!defined(BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013)
        // We do not have constructors with perfect-forwarding for the C++03
        // move-emulating library, so these tests require true support for
        // rvalue-references.

        if (verbose) printf("\t\twith move-only type as 'second'\n");
        {
            typedef bsltf::MoveOnlyAllocTestType MoveOnly;

            typedef bsl::pair<int&, MoveOnly>    ObjL;

            bslma::Allocator *const pDA = &defaultMainAllocator;
            bslma::TestAllocator    ta("Managed 'first'", veryVeryVeryVerbose);

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\t... without allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mB(a, MoveOnly(b, &ta));  const ObjL& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(pDA, B.second.allocator(), pDA == B.second.allocator());
            ASSERTV(&ta, B.second.allocator(), &ta != B.second.allocator());
            ASSERTV(B.second.data(), 42 == B.second.data());

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mE = MoveUtil::move(mB);     const ObjL& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(pDA, E.second.allocator(), pDA == E.second.allocator());
            ASSERTV(E.second.data(), 42 == E.second.data());

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mB = MoveUtil::move(mE);

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second.data(), 42 == B.second.data());
            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second.data(), 0 == E.second.data());

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));


            if (veryVerbose) printf("\t\t... passing allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mC(a, MoveOnly(b), &ta);  const ObjL& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(pDA, C.second.allocator(), pDA != C.second.allocator());
            ASSERTV(&ta, C.second.allocator(), &ta == C.second.allocator());
            ASSERTV(C.second.data(), 42 == C.second.data());

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mF(MoveUtil::move(mC), &ta);     const ObjL& F = mF;

            ASSERTV(F.first,  13 == F.first);
            ASSERTV(&ta, F.second.allocator(), &ta == F.second.allocator());
            ASSERTV(F.second.data(), 42 == F.second.data());

        }

        if (verbose) printf(
                         "\t\twith well behaved move-only type as 'second'\n");
        {
            typedef bsltf::WellBehavedMoveOnlyAllocTestType WBMoveOnly;

            typedef bsl::pair<int&, WBMoveOnly>    ObjL;

            bslma::Allocator *const pDA = &defaultMainAllocator;
            bslma::TestAllocator    ta("Managed 'first'", veryVeryVeryVerbose);

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\t... without allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mB(a, WBMoveOnly(b, &ta));  const ObjL& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(pDA, B.second.allocator(), pDA == B.second.allocator());
            ASSERTV(&ta, B.second.allocator(), &ta != B.second.allocator());
            ASSERTV(B.second.data(), 42 == B.second.data());

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mE = MoveUtil::move(mB);     const ObjL& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(pDA, E.second.allocator(), pDA == E.second.allocator());
            ASSERTV(E.second.data(), 42 == E.second.data());

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mB = MoveUtil::move(mE);

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second.data(), 42 == B.second.data());
            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second.data(), 0 == E.second.data());

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));


            if (veryVerbose) printf("\t\t... passing allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mC(a, WBMoveOnly(b), &ta);  const ObjL& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(pDA, C.second.allocator(), pDA != C.second.allocator());
            ASSERTV(&ta, C.second.allocator(), &ta == C.second.allocator());
            ASSERTV(C.second.data(), 42 == C.second.data());

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mF(MoveUtil::move(mC), &ta);     const ObjL& F = mF;

            ASSERTV(F.first,  13 == F.first);
            ASSERTV(&ta, F.second.allocator(), &ta == F.second.allocator());
            ASSERTV(F.second.data(), 42 == F.second.data());

        }
#endif

        if (verbose) printf("\t\twith move-optimized type as 'second'\n");
        {
            typedef bsltf::MovableAllocTestType Movable;

            typedef bsl::pair<int&, Movable>    ObjL;

            bslma::Allocator *const pDA = &defaultMainAllocator;
            bslma::TestAllocator    ta("Managed 'first'", veryVeryVeryVerbose);

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\t... without allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            const Movable REF_VALUE(b, &ta);
            ObjL mB(a, Movable(b, &ta));  const ObjL& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(pDA, B.second.allocator(), pDA == B.second.allocator());
            ASSERTV(&ta, B.second.allocator(), &ta != B.second.allocator());
            ASSERTV(B.second.data(), 42 == B.second.data());

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjL mC = B;     const ObjL& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(pDA, C.second.allocator(), pDA == C.second.allocator());
            ASSERTV(C.second.data(), 42 == C.second.data());

            if (veryVerbose) printf("\t\tmove constructor\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Never clariffied why C++03 requires direct rather than copy
            // initialization, but this flags as an error with both Clang and
            // xlC otherwise.
            ObjL mE(MoveUtil::move(mB));      const ObjL& E = mE;
#else
            ObjL mE = MoveUtil::move(mB);     const ObjL& E = mE;
#endif

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(pDA, E.second.allocator(), pDA == E.second.allocator());
            ASSERTV(E.second.data(), 42 == E.second.data());

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mB = MoveUtil::move(mE);

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second.data(), 42 == B.second.data());
            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second.data(), 0 == E.second.data());

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));


            if (veryVerbose) printf("\t\t... passing allocators\n");

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mD(a, Movable(b), &ta);  const ObjL& D = mD;

            ASSERTV(D.first,  13 == D.first);
            ASSERTV(pDA, D.second.allocator(), pDA != D.second.allocator());
            ASSERTV(&ta, D.second.allocator(), &ta == D.second.allocator());
            ASSERTV(D.second.data(), 42 == D.second.data());

            if (veryVerbose) printf("\t\textended copy constructor\n");

            ObjL mF(D, &ta);     const ObjL& F = mF;

            ASSERTV(F.first,  13 == F.first);
            ASSERTV(&ta, F.second.allocator(), &ta == F.second.allocator());
            ASSERTV(F.second.data(), 42 == F.second.data());

            if (veryVerbose) printf("\t\textended move constructor\n");

            ObjL mG(MoveUtil::move(mD), &ta);     const ObjL& G = mG;

            ASSERTV(G.first,  13 == G.first);
            ASSERTV(&ta, G.second.allocator(), &ta == G.second.allocator());
            ASSERTV(G.second.data(), 42 == G.second.data());

        }
#endif  // BSLSTL_PAIR_CORRECTLY_FORWARDS_ALLOCATORS
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: PAIR OF ARRAYS
        //  Note the array types do not support "copy-construction" when passed
        //  an array argument by reference, so value-constructors will not be
        //  tested.  Those constructors should not be available if queried with
        //  the 'is_constructible' trait.
        //
        // Concerns:
        //: 1 A pair with an array type for one or both members can be
        //:   constructed only by the default constructor, or a constructor
        //:   that has the semantics of the default constructor.
        //: 2 'pair's with array members can be copy and move constructed if
        //:   the elements of the array-type (and other member) can be copy and
        //:   move constructed.
        //: 3 'pair's with array members can be swapped in C++11, where
        //:   'std::swap' is overloaded for arrays.
        //: 4 All of the accessors of 'pair' work as previously tested for
        //:   'pair's having array data members.
        //
        // Plan:
        //: 1 Use 'is_constructible' type trait to determine whether ill-formed
        //:   constructors drop out of overload resolution through SFINAE.
        //:   Note that this test may rely on the C++11 library implementation
        //:   of <type_traits>.
        //
        // Testing:
        //   Concern: pairs of arrays work correctly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONCERN: PAIR OF ARRAYS"
                            "\n===============================\n");

        if (verbose) printf("\twith no-allocating types\n");

        if (verbose) printf("\t\twith array of 'int' for 'first'\n");
        {
            typedef bsl::pair<int[3], int> ObjL;

            if (veryVerbose) printf("\t\tdefault constructor\n");

            ObjL mA;  const ObjL& A = mA;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i],  0 == A.first[i]);
            }
            ASSERTV(A.second, 0 == A.second);

            mA.first[0] = 9;
            mA.first[1] = 99;
            mA.first[2] = 999;
            mA.second   = 9999;

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjL mB = A;    const ObjL& B = mB;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], B.first[i], A.first[i] == B.first[i]);
            }
            ASSERTV(A.second, B.second, A.second == B.second);

            if (veryVerbose) printf("\t\tmove constructor\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

            ObjL mC = MoveUtil::move(mB);     const ObjL& C = mC;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            ASSERTV(A.second, C.second, A.second == C.second);
#endif

#if defined(BSLSTL_PAIR_SWAP_SUPPORTS_ARRAYS)
            mB.first[0] = 13;
            mB.first[1] = 14;
            mB.first[2] = 15;
            mB.second   = 42;

            mB.swap(mC);

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], B.first[i], A.first[i] == B.first[i]);
            }
            ASSERTV(A.second, B.second, A.second == B.second);

            ASSERTV(mC.first[0], 13 == mC.first[0]);
            ASSERTV(mC.first[1], 14 == mC.first[1]);
            ASSERTV(mC.first[2], 15 == mC.first[2]);
            ASSERTV(mC.second,   42 == mC.second  );

            swap(mC, mB);

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            ASSERTV(A.second, C.second, A.second == C.second);

            ASSERTV(mB.first[0], 13 == mB.first[0]);
            ASSERTV(mB.first[1], 14 == mB.first[1]);
            ASSERTV(mB.first[2], 15 == mB.first[2]);
            ASSERTV(mB.second,   42 == mB.second  );
#endif
        }

        if (verbose) printf("\t\twith array of 'int' for 'second'\n");
        {
            typedef bsl::pair<int, int[3]> ObjR;

            if (veryVerbose) printf("\t\tdefault constructor\n");

            ObjR mA;  const ObjR& A = mA;

            ASSERTV(A.first, 0 == A.first);
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],  0 == A.second[i]);
            }

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjR mB = A;    const ObjR& B = mB;

            ASSERTV(A.first, B.first, A.first == B.first);
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   B.second[i],
                           A.second[i] == B.second[i]);
            }

            if (veryVerbose) printf("\t\tmove constructor\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

            ObjR mC = MoveUtil::move(mB);     const ObjR& C = mC;

            ASSERTV(A.first, C.first, A.first == C.first);
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   C.second[i],
                           A.second[i] == C.second[i]);
            }
#endif

#if defined(BSLSTL_PAIR_SWAP_SUPPORTS_ARRAYS)
            mB.first     = 42;
            mB.second[0] = 13;
            mB.second[1] = 14;
            mB.second[2] = 15;

            mB.swap(mC);

            ASSERTV(A.first, B.first, A.first == B.first);
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   B.second[i],
                           A.second[i] == B.second[i]);
            }

            ASSERTV(mC.first,     42 == mC.first    );
            ASSERTV(mC.second[0], 13 == mC.second[0]);
            ASSERTV(mC.second[1], 14 == mC.second[1]);
            ASSERTV(mC.second[2], 15 == mC.second[2]);

            swap(mC, mB);

            ASSERTV(A.first, C.first, A.first == C.first);
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   C.second[i],
                           A.second[i] == C.second[i]);
            }

            ASSERTV(mB.first,     42 == mB.first    );
            ASSERTV(mB.second[0], 13 == mB.second[0]);
            ASSERTV(mB.second[1], 14 == mB.second[1]);
            ASSERTV(mB.second[2], 15 == mB.second[2]);
#endif
        }


        if (verbose) printf("\t\twith two arrays of 'int'\n");
        {
            typedef bsl::pair<int[3], int[3]> ObjP;

            if (veryVerbose) printf("\t\tdefault constructor\n");

            ObjP mA;  const ObjP& A = mA;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i],  0 == A.first[i]);
            }
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],  0 == A.second[i]);
            }

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjP mB = A;    const ObjP& B = mB;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], B.first[i], A.first[i] == B.first[i]);
            }
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   B.second[i],
                           A.second[i] == B.second[i]);
            }

            if (veryVerbose) printf("\t\tmove constructor\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

            ObjP mC = MoveUtil::move(mB);     const ObjP& C = mC;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   C.second[i],
                           A.second[i] == C.second[i]);
            }
#endif

#if defined(BSLSTL_PAIR_SWAP_SUPPORTS_ARRAYS)
            mB.first[0]  =  3;
            mB.first[1]  =  4;
            mB.first[2]  =  5;
            mB.second[0] = 13;
            mB.second[1] = 14;
            mB.second[2] = 15;

            mB.swap(mC);

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], B.first[i], A.first[i] == B.first[i]);
            }
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   B.second[i],
                           A.second[i] == B.second[i]);
            }

            ASSERTV(mC.first[0],   3 == mC.first[0] );
            ASSERTV(mC.first[1],   4 == mC.first[1] );
            ASSERTV(mC.first[2],   5 == mC.first[2] );
            ASSERTV(mC.second[0], 13 == mC.second[0]);
            ASSERTV(mC.second[1], 14 == mC.second[1]);
            ASSERTV(mC.second[2], 15 == mC.second[2]);

            swap(mC, mB);

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.second[i],   C.second[i],
                           A.second[i] == C.second[i]);
            }

            ASSERTV(mB.first[0],   3 == mB.first[0] );
            ASSERTV(mB.first[1],   4 == mB.first[1] );
            ASSERTV(mB.first[2],   5 == mB.first[2] );
            ASSERTV(mB.second[0], 13 == mB.second[0]);
            ASSERTV(mB.second[1], 14 == mB.second[1]);
            ASSERTV(mB.second[2], 15 == mB.second[2]);
#endif
        }


        if (verbose) printf("\twith allocator-aware types\n");
            // Note that an array can never be allocator aware, so we test
            // allocator-aware behavior for only the non-array member.  This
            // is needed to test the allocator-aware constructors.


#if !defined(BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013)
        if (verbose) printf("\t\twith array of 'int' for 'first'\n");
        {
            typedef bsl::pair<int[3], bsltf::MoveOnlyAllocTestType> ObjL;

            bslma::Allocator *const pDA = &defaultMainAllocator;
            bslma::TestAllocator    ta("'first' array", veryVeryVeryVerbose);

            if (veryVerbose) printf("\t\tdefault constructor\n");

            ObjL mA;  const ObjL& A = mA;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i],  0 == A.first[i]);
            }
            ASSERTV(pDA, A.second.allocator(), pDA == A.second.allocator());
            ASSERTV(A.second.data(), 0 == A.second.data());

            mA.first[0] = 9;
            mA.first[1] = 99;
            mA.first[2] = 999;
            mA.second.setData(9999);

            if (veryVerbose) printf("\t\tmove constructor\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

            ObjL mC = MoveUtil::move(mA);     const ObjL& C = mC;

            // moving an array of 'int's is the same as a copy
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            // verify 'second' has moved correctly
            ASSERTV(pDA, C.second.allocator(), pDA == C.second.allocator());
            ASSERTV(C.second.data(), 9999 == C.second.data());
            ASSERTV(pDA, A.second.allocator(), pDA == A.second.allocator());
            ASSERTV(A.second.data(), 0 == A.second.data());
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            if (veryVerbose) printf(
                                  "\t\tallocator-aware default constructor\n");

            ObjL mD(&ta);  const ObjL& D = mD;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, D.first[i],  0 == D.first[i]);
            }
            ASSERTV(pDA, D.second.allocator(), &ta == D.second.allocator());
            ASSERTV(D.second.data(), 0 == D.second.data());

            mD.first[0] = 8;
            mD.first[1] = 88;
            mD.first[2] = 888;
            mD.second.setData(8888);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

#if 0   //  TBD: need to forward directly to the move-constructor for the
        //  'FirstBase' class and not try to pass an array by reference as an
        //  argument, or this case will not compile.  Requires more work in the
        //  component header.

            if (veryVerbose) printf("\t\tallocator-aware move constructor\n");

            ObjL mF(MoveUtil::move(mD), &ta);   const ObjL& F = mF;

            // moving an array of 'int's is the same as a copy
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, D.first[i], F.first[i], D.first[i] == F.first[i]);
            }
            // verify 'second' has moved correctly
            ASSERTV(pDA, F.second.allocator(), pDA == F.second.allocator());
            ASSERTV(F.second.data(), 8888 == F.second.data());
            ASSERTV(pDA, D.second.allocator(), pDA == D.second.allocator());
            ASSERTV(D.second.data(), 0 == D.second.data());
#endif
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#endif // BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013

#if !defined(BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013)
        if (verbose) printf("\t\twith array of 'int' for 'first'\n");
        {
            typedef bsl::pair<int[3], bsltf::WellBehavedMoveOnlyAllocTestType>
                                                                          ObjL;

            bslma::Allocator *const pDA = &defaultMainAllocator;
            bslma::TestAllocator    ta("'first' array", veryVeryVeryVerbose);

            if (veryVerbose) printf("\t\tdefault constructor\n");

            ObjL mA;  const ObjL& A = mA;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i],  0 == A.first[i]);
            }
            ASSERTV(pDA, A.second.allocator(), pDA == A.second.allocator());
            ASSERTV(A.second.data(), 0 == A.second.data());

            mA.first[0] = 9;
            mA.first[1] = 99;
            mA.first[2] = 999;
            mA.second.setData(9999);

            if (veryVerbose) printf("\t\tmove constructor\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

            ObjL mC = MoveUtil::move(mA);     const ObjL& C = mC;

            // moving an array of 'int's is the same as a copy
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, A.first[i], C.first[i], A.first[i] == C.first[i]);
            }
            // verify 'second' has moved correctly
            ASSERTV(pDA, C.second.allocator(), pDA == C.second.allocator());
            ASSERTV(C.second.data(), 9999 == C.second.data());
            ASSERTV(pDA, A.second.allocator(), pDA == A.second.allocator());
            ASSERTV(A.second.data(), 0 == A.second.data());
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            if (veryVerbose) printf(
                                  "\t\tallocator-aware default constructor\n");

            ObjL mD(&ta);  const ObjL& D = mD;

            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, D.first[i],  0 == D.first[i]);
            }
            ASSERTV(pDA, D.second.allocator(), &ta == D.second.allocator());
            ASSERTV(D.second.data(), 0 == D.second.data());

            mD.first[0] = 8;
            mD.first[1] = 88;
            mD.first[2] = 888;
            mD.second.setData(8888);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Arrays do not move using BDE emulated move semantics, so this
            // test would fail to compile.

#if 0   //  TBD: need to forward directly to the move-constructor for the
        //  'FirstBase' class and not try to pass an array by reference as an
        //  argument, or this case will not compile.  Requires more work in the
        //  component header.

            if (veryVerbose) printf("\t\tallocator-aware move constructor\n");

            ObjL mF(MoveUtil::move(mD), &ta);   const ObjL& F = mF;

            // moving an array of 'int's is the same as a copy
            for (int i = 0; i != 3; ++i) {
                ASSERTV(i, D.first[i], F.first[i], D.first[i] == F.first[i]);
            }
            // verify 'second' has moved correctly
            ASSERTV(pDA, F.second.allocator(), pDA == F.second.allocator());
            ASSERTV(F.second.data(), 8888 == F.second.data());
            ASSERTV(pDA, D.second.allocator(), pDA == D.second.allocator());
            ASSERTV(D.second.data(), 0 == D.second.data());
#endif
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#endif // BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: PAIR OF 'const' MEMBERS
        //  An important use-case for 'pair' is 'pair<const Key, Value' as the
        //  element type of a 'map'.  Such 'pair's are neither assignable nor
        //  swappable, but many other contracts still hold.
        //
        // Concerns:
        //: 1 A pair with a 'const' member is default constructible as long as
        //:   the 'const-qualified type is non-trivially default constructible.
        //: 2 A pair with a 'const' member can be copy-constructed, holding a
        //:   copy of the original value.
        //: 3 A pair with a 'const' member may be move-constructed, as long as
        //:   the 'const'-qualified member is copy constructible.
        //: 4 All of the accessors of 'pair' work as previously tested for a
        //:   'pair' having a 'const'-qualified member.
        //
        // Plan:
        //: 1 ...
        //
        // Testing:
        //   Concern: pairs with 'const' members work correctly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONCERN: PAIR OF 'const' MEMBERS"
                            "\n========================================\n");

        if (verbose) printf("\twith lvalue-references\n");

        if (verbose) printf("\t\twith simple reference for 'first'\n");
        {
            typedef bsl::pair<int, int>       Obj;
            typedef bsl::pair<const int, int> ObjL;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\treference object without 'const'\n");

            Obj mA;         const Obj& A = mA;

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjL mB(a, b);  const ObjL& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjL mC = B;    const ObjL& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjL mD = A;    const ObjL& D = mD;

            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjL mE = MoveUtil::move(B);     const ObjL& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);

            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjL mF = MoveUtil::move(mA);     const ObjL& F = mF;

            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);

            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(mD);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));

            ASSERT(!(B == D));
            ASSERT( (B != D));
            ASSERT(!(B <  D));
            ASSERT(!(B <= D));
            ASSERT( (B >= D));
            ASSERT( (B >  D));
        }

        if (verbose) printf("\t\twith simple reference for 'second'\n");
        {
            typedef bsl::pair<int, int>       Obj;
            typedef bsl::pair<int, const int> ObjR;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\treference object without 'const'\n");

            Obj mA;         const Obj& A = mA;

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjR mB(a, b);  const ObjR& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjR mC = B;    const ObjR& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjR mD = A;    const ObjR& D = mD;

            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjR mE = MoveUtil::move(B);     const ObjR& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);

            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjR mF = MoveUtil::move(mA);     const ObjR& F = mF;

            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);

            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(mD);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));

            ASSERT(!(B == D));
            ASSERT( (B != D));
            ASSERT(!(B <  D));
            ASSERT(!(B <= D));
            ASSERT( (B >= D));
            ASSERT( (B >  D));
        }

        if (verbose) printf("\t\twith two simple references\n");
        {
            typedef bsl::pair<int, int>       Obj;
            typedef bsl::pair<const int, const int> ObjP;

            int a = 13;
            int b = 42;

            if (veryVerbose) printf("\t\treference object without 'const'\n");

            Obj mA;         const Obj& A = mA;

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);

            if (veryVerbose) printf("\t\tconstruct with two arguments\n");

            ObjP mB(a, b);  const ObjP& B = mB;

            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tcopy constructor\n");

            ObjP mC = B;    const ObjP& C = mC;

            ASSERTV(C.first,  13 == C.first);
            ASSERTV(C.second, 42 == C.second);

            if (veryVerbose) printf("\t\tconverting 'pair' constructor\n");

            ObjP mD = A;    const ObjP& D = mD;

            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tmove constructor\n");

            ObjP mE = MoveUtil::move(B);     const ObjP& E = mE;

            ASSERTV(E.first,  13 == E.first);
            ASSERTV(E.second, 42 == E.second);

            if (veryVerbose) printf("\t\tconverting move constructor\n");

            ObjP mF = MoveUtil::move(mA);     const ObjP& F = mF;

            ASSERTV(F.first,  0 == F.first);
            ASSERTV(F.second, 0 == F.second);

            // TDB Piecewise construction from native tuple of references

            if (veryVerbose) printf("\t\tassign from 'pair'\n");

            mA = B;

            ASSERTV(A.first,  13 == A.first);
            ASSERTV(A.second, 42 == A.second);
            ASSERTV(B.first,  13 == B.first);
            ASSERTV(B.second, 42 == B.second);

            if (veryVerbose) printf("\t\tmove-assign from 'pair'\n");

            mA = MoveUtil::move(mD);

            ASSERTV(A.first,  0 == A.first);
            ASSERTV(A.second, 0 == A.second);
            ASSERTV(D.first,  0 == D.first);
            ASSERTV(D.second, 0 == D.second);

            if (veryVerbose) printf("\t\tcomparison operators\n");

            ASSERT( (B == B));
            ASSERT(!(B != B));
            ASSERT(!(B <  B));
            ASSERT( (B <= B));
            ASSERT( (B >= B));
            ASSERT(!(B >  B));

            ASSERT(!(B == D));
            ASSERT( (B != D));
            ASSERT(!(B <  D));
            ASSERT(!(B <= D));
            ASSERT( (B >= D));
            ASSERT( (B >  D));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: C++03 MOVABLE TYPES WORK CORRECTLY
        //  Prior to C++11, BDE supported move operations through types like
        //  'bslma::ManagedPtr' that emulate move semantics in the same way
        //  that 'std::auto_ptr' did.  Such code must still continue to compile
        //  for Bloomberg with a C++11 library, even though the standard makes
        //  no such requirement.  Note that due to implementation details of
        //  the 'pair' class, it is necessary to test support as 'first' and
        //  'second' independently.
        //
        // Concerns:
        //: 1 A 'pair' object with an instantiation of 'bslma::ManagedPtr' for
        //:   either 'first', 'second', or both, can be default-constructed.
        //: 2 A 'pair' object with an instantiation of 'bslma::ManagedPtr' for
        //:   either 'first', 'second', or both, can be copy-constructed,
        //:   copy-assigned, and swapped with the expected move-semantic
        //:   behavior.
        //: 3 A 'pair' object with an instantiation of 'bslma::ManagedPtr' for
        //:   either 'first', 'second', or both, can be move-constructed,
        //:   move-assigned, and swapped with the expected move-semantic
        //:   behavior.
        //
        // Plan:
        //: 1 ...
        //
        // Testing:
        //   Concern: pairs of C++03 movable types work correctly
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTESTING CONCERN: C++03 MOVABLE TYPES WORK CORRECTLY"
                    "\n===================================================\n");

        if (verbose) printf("\twith movable type as 'first'\n");
        {
            typedef bsl::pair<bslma::ManagedPtr<int>, int>  ObjL;

            bslma::TestAllocator ta("Left managed pointer");

            ObjL mX;   const ObjL& X = mX;

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second, 0 == X.second);

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjL mY(managed, 42);   const ObjL& Y = mY;

            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(Y.second, 42 == Y.second);

            mX = mY;

            ASSERTV(X.first.get(), pI == X.first.get());
            ASSERTV(X.second, 42 == X.second);
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second, 42 == Y.second);

            mY.second = 13;
            mX.swap(mY);

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second, 13 == X.second);
            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(Y.second, 42 == Y.second);
        }

        if (verbose) printf("\twith movable type as 'second'\n");
        {
            typedef bsl::pair<int, bslma::ManagedPtr<int> > ObjR;

            bslma::TestAllocator ta("Right managed pointer");

            ObjR mX;   const ObjR& X = mX;

            ASSERTV(X.first, 0 == X.first);
            ASSERTV(X.second.get(), 0 == X.second.get());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjR mY(42, managed);   const ObjR& Y = mY;

            ASSERTV(Y.first, 42 == Y.first);
            ASSERTV(Y.second.get(), pI == Y.second.get());

            mX = mY;

            ASSERTV(X.first, 42 == X.first);
            ASSERTV(X.second.get(), pI == X.second.get());
            ASSERTV(Y.first, 42 == Y.first);
            ASSERTV(Y.second.get(), 0 == Y.second.get());

            mY.first = 13;
            mX.swap(mY);

            ASSERTV(X.first, 13 == X.first);
            ASSERTV(X.second.get(), 0 == X.second.get());
            ASSERTV(Y.first, 42 == Y.first);
            ASSERTV(Y.second.get(), pI == Y.second.get());
        }

        if (verbose) printf("\twith movable type for both members\n");
        {
            typedef bsl::pair<bslma::ManagedPtr<int>, bslma::ManagedPtr<int> >
                                                                          ObjB;

            bslma::TestAllocator ta("Left managed pointer");

            ObjB mX;   const ObjB& X = mX;

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second.get(), 0 == X.second.get());

            int *pA = new(ta) int(13);
            bslma::ManagedPtr<int> managedA(pA, &ta);

            int *pB = new(ta) int(42);
            bslma::ManagedPtr<int> managedB(pB, &ta);

            ObjB mY(managedA, managedB);   const ObjB& Y = mY;

            ASSERTV(Y.first.get(), pA == Y.first.get());
            ASSERTV(Y.second.get(), pB == Y.second.get());

            mX = mY;

            ASSERTV(X.first.get(), pA == X.first.get());
            ASSERTV(X.second.get(), pB == X.second.get());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.get(), 0 == Y.second.get());

            mX.swap(mY);

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second.get(), 0 == X.second.get());
            ASSERTV(Y.first.get(), pA == Y.first.get());
            ASSERTV(Y.second.get(), pB == Y.second.get());
        }

        if (verbose) printf("\twith allocator-aware types\n");
            // Note that we do not have an allocator aware implicit-movable
            // type for testing, so we test allocator-aware behavior for only
            // the other member.  This is needed to test the allocator-aware
            // constructors and 'swap'

#if !defined(BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013)
        if (verbose) printf("\t\twith move-only type as 'second'\n");
        {
            typedef bsl::pair<bslma::ManagedPtr<int>,
                              bsltf::MoveOnlyAllocTestType>  ObjL;

            bslma::TestAllocator ta("Managed 'first'", veryVeryVeryVerbose);

            ObjL mX(&ta);   const ObjL& X = mX;

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(X.second.data(), 0 == X.second.data());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjL mY(managed, 42, &ta);   const ObjL& Y = mY;

            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

            mX = MoveUtil::move(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());

            ASSERTV(X.first.get(), pI == X.first.get());
            ASSERTV(X.second.data(), 42 == X.second.data());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.data(), 0 == Y.second.data());

            mX.swap(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second.data(), 0 == X.second.data());
            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

            ObjL mZ(MoveUtil::move(mY), &ta);   const ObjL& Z = mZ;

            ASSERTV(Z.first.get(), pI == Z.first.get());
            ASSERTV(&ta, Z.second.allocator(), &ta == Z.second.allocator());
            ASSERTV(Z.second.data(), 42 == Z.second.data());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.data(), 0 == Y.second.data());
        }

        if (verbose) printf(
                         "\t\twith well-behaved move-only type as 'second'\n");
        {
            typedef bsl::pair<bslma::ManagedPtr<int>,
                              bsltf::WellBehavedMoveOnlyAllocTestType>  ObjL;

            bslma::TestAllocator ta("Managed 'first'", veryVeryVeryVerbose);

            ObjL mX(&ta);   const ObjL& X = mX;

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(X.second.data(), 0 == X.second.data());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjL mY(managed, 42, &ta);   const ObjL& Y = mY;

            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

            mX = MoveUtil::move(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());

            ASSERTV(X.first.get(), pI == X.first.get());
            ASSERTV(X.second.data(), 42 == X.second.data());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.data(), 0 == Y.second.data());

            mX.swap(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second.data(), 0 == X.second.data());
            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

            ObjL mZ(MoveUtil::move(mY), &ta);   const ObjL& Z = mZ;

            ASSERTV(Z.first.get(), pI == Z.first.get());
            ASSERTV(&ta, Z.second.allocator(), &ta == Z.second.allocator());
            ASSERTV(Z.second.data(), 42 == Z.second.data());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.data(), 0 == Y.second.data());
        }
#endif // BSLSTL_PAIR_DISABLE_MOVEONLY_TESTING_ON_VC2013

        if (verbose) printf("\t\twith move-optimized type as 'second'\n");
        {
            typedef bsl::pair<bslma::ManagedPtr<int>,
                              bsltf::MovableAllocTestType>  ObjL;

            bslma::TestAllocator ta("Managed 'first'", veryVeryVeryVerbose);

            ObjL mX(&ta);   const ObjL& X = mX;

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(X.second.data(), 0 == X.second.data());


            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjL mY(managed, 42, &ta);   const ObjL& Y = mY;

            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

            mX = MoveUtil::move(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());

            ASSERTV(X.first.get(), pI == X.first.get());
            ASSERTV(X.second.data(), 42 == X.second.data());
            ASSERTV(Y.first.get(), 0 == Y.first.get());
            ASSERTV(Y.second.data(), 0 == Y.second.data());

            mX.swap(mY);
            ASSERTV(&ta, X.second.allocator(), &ta == X.second.allocator());
            ASSERTV(&ta, Y.second.allocator(), &ta == Y.second.allocator());

            ASSERTV(X.first.get(), 0 == X.first.get());
            ASSERTV(X.second.data(), 0 == X.second.data());
            ASSERTV(Y.first.get(), pI == Y.first.get());
            ASSERTV(Y.second.data(), 42 == Y.second.data());

#if 0   //  TBD This fails to compile as the allocator-aware copy constructor
        //  signature is not SFINAE-friendly.  However, this failure does not
        //  look to be a new bug for the C++14 branch

            ObjL mZ(mY, &ta);   const ObjL& Z = mZ;

            ASSERTV(Z.first.get(), pI == Z.first.get());
            ASSERTV(&ta, Z.second.allocator(), &ta == Z.second.allocator());
            ASSERTV(Z.second.data(), 42 == Z.second.data());
#endif
        }

        if (verbose) printf("\t\twith move-only type as 'first'\n");
        {
            typedef bsl::pair<bsltf::MoveOnlyAllocTestType,
                              bslma::ManagedPtr<int> >      ObjR;

            bslma::TestAllocator ta("Managed 'second'", veryVeryVeryVerbose);

            ObjR mX(&ta);   const ObjR& X = mX;

            ASSERTV(&ta, X.first.allocator(), &ta == X.first.allocator());
            ASSERTV(X.first.data(), 0 == X.first.data());
            ASSERTV(X.second.get(), 0 == X.second.get());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjR mY(42, managed, &ta);   const ObjR& Y = mY;

            ASSERTV(&ta, Y.first.allocator(), &ta == Y.first.allocator());
            ASSERTV(Y.first.data(), 42 == Y.first.data());
            ASSERTV(Y.second.get(), pI == Y.second.get());

            mX = MoveUtil::move(mY);

            ASSERTV(X.first.data(), 42 == X.first.data());
            ASSERTV(X.second.get(), pI == X.second.get());
            ASSERTV(Y.first.data(), 0 == Y.first.data());
            ASSERTV(Y.second.get(), 0 == Y.second.get());

            ObjR mZ(MoveUtil::move(mX), &ta);   const ObjR& Z = mZ;

            ASSERTV(&ta, Z.first.allocator(), &ta == Z.first.allocator());
            ASSERTV(Z.first.data(), 42 == Z.first.data());
            ASSERTV(Z.second.get(), pI == Z.second.get());
            ASSERTV(X.first.data(), 0 == X.first.data());
            ASSERTV(X.second.get(), 0 == X.second.get());
        }

        if (verbose) printf(
                          "\t\twith well behaved move-only type as 'first'\n");
        {
            typedef bsl::pair<bsltf::WellBehavedMoveOnlyAllocTestType,
                              bslma::ManagedPtr<int> >      ObjR;

            bslma::TestAllocator ta("Managed 'second'", veryVeryVeryVerbose);

            ObjR mX(&ta);   const ObjR& X = mX;

            ASSERTV(&ta, X.first.allocator(), &ta == X.first.allocator());
            ASSERTV(X.first.data(), 0 == X.first.data());
            ASSERTV(X.second.get(), 0 == X.second.get());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjR mY(42, managed, &ta);   const ObjR& Y = mY;

            ASSERTV(&ta, Y.first.allocator(), &ta == Y.first.allocator());
            ASSERTV(Y.first.data(), 42 == Y.first.data());
            ASSERTV(Y.second.get(), pI == Y.second.get());

            mX = MoveUtil::move(mY);

            ASSERTV(X.first.data(), 42 == X.first.data());
            ASSERTV(X.second.get(), pI == X.second.get());
            ASSERTV(Y.first.data(), 0 == Y.first.data());
            ASSERTV(Y.second.get(), 0 == Y.second.get());

            ObjR mZ(MoveUtil::move(mX), &ta);   const ObjR& Z = mZ;

            ASSERTV(&ta, Z.first.allocator(), &ta == Z.first.allocator());
            ASSERTV(Z.first.data(), 42 == Z.first.data());
            ASSERTV(Z.second.get(), pI == Z.second.get());
            ASSERTV(X.first.data(), 0 == X.first.data());
            ASSERTV(X.second.get(), 0 == X.second.get());
        }

        if (verbose) printf("\t\twith movable type as 'first'\n");
        {
            typedef bsl::pair<bsltf::MovableAllocTestType,
                              bslma::ManagedPtr<int> >     ObjR;

            bslma::TestAllocator ta("Managed 'second'", veryVeryVeryVerbose);

            ObjR mX(&ta);   const ObjR& X = mX;

            ASSERTV(&ta, X.first.allocator(), &ta == X.first.allocator());
            ASSERTV(X.first.data(), 0 == X.first.data());
            ASSERTV(X.second.get(), 0 == X.second.get());

            int *pI = new(ta) int(13);

            bslma::ManagedPtr<int> managed(pI, &ta);

            ObjR mY(42, managed, &ta);   const ObjR& Y = mY;

            ASSERTV(Y.first.data(), 42 == Y.first.data());
            ASSERTV(&ta, Y.first.allocator(), &ta == Y.first.allocator());
            ASSERTV(Y.second.get(), pI == Y.second.get());

            mX = mY;

            ASSERTV(X.first.data(), 42 == X.first.data());
            ASSERTV(X.second.get(), pI == X.second.get());
            ASSERTV(Y.first.data(), 42 == Y.first.data());
            ASSERTV(Y.second.get(), 0 == Y.second.get());

#if 0   //  TBD This fails to compile due to an incompatible allocator-aware
        //  copy constructor signature, but does not look to be a new bug for
        //  the C++14 branch

            ObjR mZ(mY, &ta);   const ObjR& Z = mZ;

            ASSERTV(&ta, Z.first.allocator(), &ta == Z.first.allocator());
            ASSERTV(Z.first.data(), 42 == Z.first.data());
            ASSERTV(Z.second.get(), pI == Z.second.get());
#endif
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING FIX FOR DRQS 122792538
        //  This test case concerns a regression that fell out of the SFINAE
        //  rules for constructors applied as part of the C++14 upgrade for
        //  'bsl::pair'.  The fix was to be more thorough about our treatment
        //  of SFINAE-friendly constructors.
        //
        // Concerns:
        //: 1 Pre-existing code does not fail to compile with the C++14 upgrade
        //:   to 'bsl::pair'.
        //
        // Plan:
        //: 1 Provide a minimal code example of the kind of valid code that was
        //:   surprisingly failing to compile.  This test remains as a canary
        //:   should a further regression introduce a similar problem.
        //
        // Testing:
        //   Concern: Fix for DRQS 122792538
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FIX FOR DRQS 122792538"
                            "\n==============================\n");

        typedef bsl::pair<u::StrictlyAllocated, int> Obj;

        bslma::TestAllocator tax("Unused allocator");
        Obj mX(&tax);    const Obj& X = mX;

        ASSERTV(X.first.index(), 0 == X.first.index());
        ASSERTV(X.first.allocator(), &tax == X.first.allocator());
        ASSERTV(X.second, 0 == X.second);

        bslma::TestAllocator tay("Unused allocator");
        Obj mY(X, &tay); const Obj& Y = mY;

        ASSERTV(Y.first.index(), 0 == X.first.index());
        ASSERTV(Y.first.allocator(), &tax == X.first.allocator());
        ASSERTV(Y.second, 0 == Y.second);

        ASSERT((!bsl::is_nothrow_move_constructible<Obj>::value));
            // The trait 'bsl::is_nothrow_move_constructible<Obj>' will fail to
            // instantiate, reporting an attempt to access private constructors
            // prior to applying the patch for the ticket above.
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO 'std::tuple'
        //
        // Concerns:
        //: 1 Operator returns a 'std::tuple', which members are references
        //:   providing modifiable access to the elements of calling object.
        //:
        //: 2 Operator is implicitly called during assignment of 'bsl::pair'
        //:   object to the result of 'std::tie' function, accepting two
        //:   parameters.
        //:
        //: 3 Operator is able to convert 'bsl::pair' object to the
        //:   'std::tuple', having one or both members of type of
        //:   'std::ignore'.
        //
        // Plan:
        //: 1 Create a 'bsl::pair' and convert it to 'std::tuple' using
        //:   conversion operator.  Modify original 'bsl::pair' object and
        //:   verify, that 'std::tuple' reflects these changes.  Then modify
        //:   tuple's members and verify that pair's members are changed too.
        //:   (C-1)
        //:
        //: 2 Assign 'bsl::pair' object to the result of 'std::tie' function
        //:   accepting two parameters and verify, that values of variables,
        //:   passed to the function are changed (if these variables are not
        //:   'std:ignore' values).  (C-2..3)
        //
        // Testing:
        //   template <class U1, class U2> operator std::tuple<U1&, U2&>()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO 'std::tuple'"
                            "\n==================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)

# if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1900
    // MSVC 2015 compler incorrectly handles tuple assigment so this test
    // compiling fails.  MSVC 2013 does not support tuples, so this test is
    // disabled for it anyway.  MSVC 2017 handles this test correctly.
    if (verbose) printf("\tThis test is disabled for MSVC 2015\n");

# else
        RUN_EACH_TYPE(TupleConversionDriver,
                      operatorTest,
                      bsltf::SimpleTestType,
                      bsltf::AllocTestType,
                      bsltf::BitwiseCopyableTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::AllocBitwiseMoveableTestType,
                      bsltf::NonTypicalOverloadsTestType);

        RUN_EACH_TYPE(TupleConversionDriver,
                      tieTest,
                      int,
                      long int,
                      bsltf::SimpleTestType,
                      bsltf::AllocTestType,
                      bsltf::BitwiseCopyableTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::AllocBitwiseMoveableTestType,
                      bsltf::NonTypicalOverloadsTestType);
# endif
#else
    if (verbose) printf(
         "\tThis test is disabled because compiler does not support tuples\n");
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'noexcept' SPECIFICATIONS
        //
        // Concerns:
        //: 1 The 'noexcept' specification has been applied to all class
        //:   interfaces required by the standard.
        //
        // Plan:
        //: 1 Apply the unary 'noexcept' operator to expressions that mimic
        //:   those appearing in the standard and confirm that calculated
        //:   boolean value matches the expected value.
        //:
        //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
        //:   of the container, we need test for just one general type and any
        //:   'TYPE' specializations.
        //
        // Testing:
        //   Concern: Methods marked 'noexcept' in standard are so implemented
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'noexcept' SPECIFICATIONS"
                            "\n=================================\n");

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        if (verbose) printf("'noexcept' not supported on this platform\n");
#else
        // N4594: 20.4: Pairs

        // pages 526-527: Class template pair
        //..
        //  pair& operator=(pair&& p) noexcept (see below);
        //  void pair::swap(pair& p) noexcept (see below);
        //  void swap(pair& a, pair& b) noexcept (see below);
        //  T& get<N>(pair &) noexcept;  // for all 4 c-ref-qualifications
        //  T& get<T>(pair &) noexcept;  // for all 4 c-ref-qualifications
        //..

        {
            bsl::pair<int, long> mX;
            bsl::pair<int, long> mP;

            const bsl::pair<int, long>&   X = mX;

            ASSERT(noexcept(mX = MoveUtil::move(mP)));
# if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERT(noexcept(mX.swap(mP)));
            ASSERT(noexcept(bsl::swap(mX, mP)));
# endif

# if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
            ASSERT(noexcept(bsl::get<0>(mX)));
            ASSERT(noexcept(bsl::get<0>( X)));
            ASSERT(noexcept(bsl::get<0>(std::move(mX))));
            ASSERT(noexcept(bsl::get<0>(std::move( X))));

            ASSERT(noexcept(bsl::get<1>(mX)));
            ASSERT(noexcept(bsl::get<1>( X)));
            ASSERT(noexcept(bsl::get<1>(std::move(mX))));
            ASSERT(noexcept(bsl::get<1>(std::move( X))));

            ASSERT(noexcept(bsl::get<int>(mX)));
            ASSERT(noexcept(bsl::get<int>( X)));
            ASSERT(noexcept(bsl::get<int>(std::move(mX))));
            ASSERT(noexcept(bsl::get<int>(std::move( X))));

            ASSERT(noexcept(bsl::get<long>(mX)));
            ASSERT(noexcept(bsl::get<long>( X)));
            ASSERT(noexcept(bsl::get<long>(std::move(mX))));
            ASSERT(noexcept(bsl::get<long>(std::move( X))));
# endif
        }
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING TUPLE-LIKE API
        //
        // Concerns:
        //: 1 The partial specialization of 'std::tuple_size' for 'bsl::pair'
        //:   always shows that there are two elements in pair, irrespectively
        //:   of their types.
        //:
        //: 2 The partial specializations of 'std::tuple_element' for
        //:   'bsl::pair' has 'type' attribute value equal to the type of the
        //:   first pair's element, if 'INDEX' template parameter is equal to
        //:   '0', and to the type of the second pair's element, if 'INDEX'
        //:   template parameter is equal to '1'.
        //:
        //: 3 The index-based overloads of 'get' function return pair's element
        //:   with requested index.
        //:
        //: 4 The type-based overloads of 'get' function return pair's element
        //:   with requested type.
        //:
        //: 5 All tuple-like APIs work correctly with pairs, having constant
        //:   and volatile elements.
        //:
        //: 6 There is no unexpected recursion when an element type is an
        //:   instantiation of 'bsl::pair<T1, T2>'.
        //
        // Plan:
        //: 1 Specify the set of types and verify attributes of 'tuple_size'
        //:   and 'tuple_element' partial specializations, having 'bsl::pair'
        //:   with elements of these types as template parameters.  (C-1..2, 6)
        //:
        //: 2 Specify the set of types and verify return values of 'bsl::get'
        //:   function overloads, having 'bsl::pair' with ordinary and constant
        //:   elements of these types as parameters.
        //:
        //: 4 Use special test class to verify return values of 'bsl::get'
        //:   function overloads, having 'bsl::pair' with volatile elements of
        //:   this type as parameters.  (C-3..5, 6)
        //
        // Testing:
        //   std::tuple_element<bsl::pair<T1, T2> >
        //   std::tuple_size<bsl::pair<T1, T2> >
        //   tuple_element<>::type& get<INDEX, T1, T2>(bsl::pair<T1, T2>& p)
        //   tuple_element<>::type& get<TYPE, T1, T2>(bsl::pair<T1, T2>& p)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TUPLE-LIKE API"
                            "\n======================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
        if (verbose) printf(
                "\tTesting meta-functions 'tuple_element' and 'tuple_size'\n");

        RUN_EACH_TYPE(TupleApiTestDriver,
                      metaFunctionsTest,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // Use 'bsl::pair<int, int>' class as a template parameter to check
        // code for recursive problems.

        TupleApiTestDriver<bsl::pair<int, int> >::metaFunctionsTest();


        if (verbose) printf(
                "\tTesting 'get' function, accepting index as a template"
                " parameter\n");

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByIndexCopyTest,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // Use 'bsl::pair<int, int>' class as a template parameter to check
        // code for recursive problems.

        TupleApiTestDriver<bsl::pair<int, int> >::getByIndexCopyTest();

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByIndexMoveTest,
                      bsltf::AllocArgumentType<1>,
                      bsltf::ArgumentType<1>,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByIndexVolatileMoveTest,
                      VolatileMovableType);

        if (verbose) printf(
                "\tTesting 'get' function, accepting type  as a template"
                " parameter\n");

        // We test 'get' function for 'bsl::pair<T, int>' and
        // 'bsl::pair<int, T>' objects.
        // BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR contains 'size_t'
        // type, but not 'int' type, so we will not hit the situation when pair
        // contains elements of the same type.

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByTypeCopyTest,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // Use 'bsl::pair<int, int>' class as a template parameter to check
        // code for recursive problems.

        TupleApiTestDriver<bsl::pair<int, int> >::getByTypeCopyTest();

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByTypeMoveTest,
                      bsltf::AllocArgumentType<1>,
                      bsltf::ArgumentType<1>,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        RUN_EACH_TYPE(TupleApiTestDriver,
                      getByTypeVolatileMoveTest,
                      VolatileMovableType);
#else
        if (verbose) printf("Tuple-like APIs are not supported\n");
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING C'TOR TO/FROM STD::PAIR, PAIRS WITHIN PAIRS
        //
        // This test case was written to expose a known problem, and verify its
        // fix, to do with pairs within pairs, and conversions between
        // 'bsl::pair' and 'std::pair'.
        //
        // Concerns:
        //: 1 In all cases, we will be dealing with cases where one member of
        //:   the pair is a pair, and the other member is a boolean.  We will
        //:   test everything in the cases
        //:   o 'pair<pair, bool>'
        //:   o 'pair<bool, pair>'
        //:
        //: 2 There is only a conversion from a 'std::pair' to a
        //:   'bsl::pair', not the other way around, even in the case where
        //:   'first_type' and 'second_type' match, so conversions from
        //:   'bsl::pair' to 'std::pair' are *NOT* tested.
        //:
        //: 3 Test a conversion from a pair where the nested pair type of the
        //:   source and destination match.
        //:
        //: 4 Test a conversation from a 'std::pair' to a 'bsl::pair'
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
        //:   creation of a 'std::pair' makes use of the default
        //:   allocator, and that the creation of 'bsl::pair' that has an
        //:   allocator passed to the c'tor makes use of the passed allocator.
        //
        // TESTING:
        //   pair(std::pair<*>, bool>)
        // --------------------------------------------------------------------

        if (verbose) printf(
             "\nTESTING C'TOR TO/FROM STD::PAIR, PAIRS WITHIN PAIRS"
             "\n===================================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

                      // 'bsltf::MoveOnlyAllocTestType' test disabled as
                      // copy-assign is needed
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TUPLE-BASED CONSTRUCTION
        //
        // Concerns:
        //: 1 That the constructor of 'bsl::pair' can properly propagate
        //:   complex argument types to a template constructors of its member
        //:   types.
        //:
        //: 2 Members of 'bsl::pair' are created in-place during piecewise
        //:   construction.
        //:
        //: 3 Allocators are correctly propagated to pair members
        //:   irrespectively of their allocator traits.
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
        //:
        //: 5 Call 'runTestInplaceMemberConstruction' function to create a pair
        //:   of objects of 'CloneDisabledTestType' class, that has private
        //:   copy and move constructors and assignment operators, to prove
        //:   that members of pair are created in-place.
        //:
        //: 6 Construct several pairs of types, having different allocator
        //:   traits (not taking allocators on construction, taking bslma
        //:   allocators, taking allocators preceded by 'bsl::allocator_arg')
        //:   with/without passing allocators on construction. Verify, that
        //:   constructed members use default/passed on construction allocator
        //:   to supply memory.
        //
        // Testing:
        //   pair(piecewise_construct_t, tuple, tuple);
        //   pair(piecewise_construct_t, tuple, tuple, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TUPLE-BASED CONSTRUCTION"
                            "\n================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
        typedef TupleTestDriver TTD;

        // These series were machine generated to generate all possible
        // combinations of calls with no repetition:

        TTD::runTestAlloc<0, 2,2,2, 0, 2,2,2>();
        TTD::runTestAlloc<0, 2,2,2, 1, 0,2,2>();
        TTD::runTestAlloc<0, 2,2,2, 1, 1,2,2>();
        TTD::runTestAlloc<0, 2,2,2, 2, 0,0,2>();
        TTD::runTestAlloc<0, 2,2,2, 2, 0,1,2>();
        TTD::runTestAlloc<0, 2,2,2, 2, 1,0,2>();
        TTD::runTestAlloc<0, 2,2,2, 2, 1,1,2>();
        TTD::runTestAlloc<0, 2,2,2, 3, 0,0,0>();
        TTD::runTestAlloc<0, 2,2,2, 3, 0,0,1>();
        TTD::runTestAlloc<0, 2,2,2, 3, 0,1,0>();
        TTD::runTestAlloc<0, 2,2,2, 3, 0,1,1>();
        TTD::runTestAlloc<0, 2,2,2, 3, 1,0,0>();
        TTD::runTestAlloc<0, 2,2,2, 3, 1,0,1>();
        TTD::runTestAlloc<0, 2,2,2, 3, 1,1,0>();
        TTD::runTestAlloc<0, 2,2,2, 3, 1,1,1>();
        TTD::runTestAlloc<1, 0,2,2, 0, 2,2,2>();
        TTD::runTestAlloc<1, 0,2,2, 1, 0,2,2>();
        TTD::runTestAlloc<1, 0,2,2, 1, 1,2,2>();
        TTD::runTestAlloc<1, 0,2,2, 2, 0,0,2>();
        TTD::runTestAlloc<1, 0,2,2, 2, 0,1,2>();
        TTD::runTestAlloc<1, 0,2,2, 2, 1,0,2>();
        TTD::runTestAlloc<1, 0,2,2, 2, 1,1,2>();
        TTD::runTestAlloc<1, 0,2,2, 3, 0,0,0>();
        TTD::runTestAlloc<1, 0,2,2, 3, 0,0,1>();
        TTD::runTestAlloc<1, 0,2,2, 3, 0,1,0>();
        TTD::runTestAlloc<1, 0,2,2, 3, 0,1,1>();
        TTD::runTestAlloc<1, 0,2,2, 3, 1,0,0>();
        TTD::runTestAlloc<1, 0,2,2, 3, 1,0,1>();
        TTD::runTestAlloc<1, 0,2,2, 3, 1,1,0>();
        TTD::runTestAlloc<1, 0,2,2, 3, 1,1,1>();
        TTD::runTestAlloc<1, 1,2,2, 0, 2,2,2>();
        TTD::runTestAlloc<1, 1,2,2, 1, 0,2,2>();
        TTD::runTestAlloc<1, 1,2,2, 1, 1,2,2>();
        TTD::runTestAlloc<1, 1,2,2, 2, 0,0,2>();
        TTD::runTestAlloc<1, 1,2,2, 2, 0,1,2>();
        TTD::runTestAlloc<1, 1,2,2, 2, 1,0,2>();
        TTD::runTestAlloc<1, 1,2,2, 2, 1,1,2>();
        TTD::runTestAlloc<1, 1,2,2, 3, 0,0,0>();
        TTD::runTestAlloc<1, 1,2,2, 3, 0,0,1>();
        TTD::runTestAlloc<1, 1,2,2, 3, 0,1,0>();
        TTD::runTestAlloc<1, 1,2,2, 3, 0,1,1>();
        TTD::runTestAlloc<1, 1,2,2, 3, 1,0,0>();
        TTD::runTestAlloc<1, 1,2,2, 3, 1,0,1>();
        TTD::runTestAlloc<1, 1,2,2, 3, 1,1,0>();
        TTD::runTestAlloc<1, 1,2,2, 3, 1,1,1>();
        TTD::runTestAlloc<2, 0,0,2, 0, 2,2,2>();
        TTD::runTestAlloc<2, 0,0,2, 1, 0,2,2>();
        TTD::runTestAlloc<2, 0,0,2, 1, 1,2,2>();
        TTD::runTestAlloc<2, 0,0,2, 2, 0,0,2>();
        TTD::runTestAlloc<2, 0,0,2, 2, 0,1,2>();
        TTD::runTestAlloc<2, 0,0,2, 2, 1,0,2>();
        TTD::runTestAlloc<2, 0,0,2, 2, 1,1,2>();
        TTD::runTestAlloc<2, 0,0,2, 3, 0,0,0>();
        TTD::runTestAlloc<2, 0,0,2, 3, 0,0,1>();
        TTD::runTestAlloc<2, 0,0,2, 3, 0,1,0>();
        TTD::runTestAlloc<2, 0,0,2, 3, 0,1,1>();
        TTD::runTestAlloc<2, 0,0,2, 3, 1,0,0>();
        TTD::runTestAlloc<2, 0,0,2, 3, 1,0,1>();
        TTD::runTestAlloc<2, 0,0,2, 3, 1,1,0>();
        TTD::runTestAlloc<2, 0,0,2, 3, 1,1,1>();
        TTD::runTestAlloc<2, 0,1,2, 0, 2,2,2>();
        TTD::runTestAlloc<2, 0,1,2, 1, 0,2,2>();
        TTD::runTestAlloc<2, 0,1,2, 1, 1,2,2>();
        TTD::runTestAlloc<2, 0,1,2, 2, 0,0,2>();
        TTD::runTestAlloc<2, 0,1,2, 2, 0,1,2>();
        TTD::runTestAlloc<2, 0,1,2, 2, 1,0,2>();
        TTD::runTestAlloc<2, 0,1,2, 2, 1,1,2>();
        TTD::runTestAlloc<2, 0,1,2, 3, 0,0,0>();
        TTD::runTestAlloc<2, 0,1,2, 3, 0,0,1>();
        TTD::runTestAlloc<2, 0,1,2, 3, 0,1,0>();
        TTD::runTestAlloc<2, 0,1,2, 3, 0,1,1>();
        TTD::runTestAlloc<2, 0,1,2, 3, 1,0,0>();
        TTD::runTestAlloc<2, 0,1,2, 3, 1,0,1>();
        TTD::runTestAlloc<2, 0,1,2, 3, 1,1,0>();
        TTD::runTestAlloc<2, 0,1,2, 3, 1,1,1>();
        TTD::runTestAlloc<2, 1,0,2, 0, 2,2,2>();
        TTD::runTestAlloc<2, 1,0,2, 1, 0,2,2>();
        TTD::runTestAlloc<2, 1,0,2, 1, 1,2,2>();
        TTD::runTestAlloc<2, 1,0,2, 2, 0,0,2>();
        TTD::runTestAlloc<2, 1,0,2, 2, 0,1,2>();
        TTD::runTestAlloc<2, 1,0,2, 2, 1,0,2>();
        TTD::runTestAlloc<2, 1,0,2, 2, 1,1,2>();
        TTD::runTestAlloc<2, 1,0,2, 3, 0,0,0>();
        TTD::runTestAlloc<2, 1,0,2, 3, 0,0,1>();
        TTD::runTestAlloc<2, 1,0,2, 3, 0,1,0>();
        TTD::runTestAlloc<2, 1,0,2, 3, 0,1,1>();
        TTD::runTestAlloc<2, 1,0,2, 3, 1,0,0>();
        TTD::runTestAlloc<2, 1,0,2, 3, 1,0,1>();
        TTD::runTestAlloc<2, 1,0,2, 3, 1,1,0>();
        TTD::runTestAlloc<2, 1,0,2, 3, 1,1,1>();
        TTD::runTestAlloc<2, 1,1,2, 0, 2,2,2>();
        TTD::runTestAlloc<2, 1,1,2, 1, 0,2,2>();
        TTD::runTestAlloc<2, 1,1,2, 1, 1,2,2>();
        TTD::runTestAlloc<2, 1,1,2, 2, 0,0,2>();
        TTD::runTestAlloc<2, 1,1,2, 2, 0,1,2>();
        TTD::runTestAlloc<2, 1,1,2, 2, 1,0,2>();
        TTD::runTestAlloc<2, 1,1,2, 2, 1,1,2>();
        TTD::runTestAlloc<2, 1,1,2, 3, 0,0,0>();
        TTD::runTestAlloc<2, 1,1,2, 3, 0,0,1>();
        TTD::runTestAlloc<2, 1,1,2, 3, 0,1,0>();
        TTD::runTestAlloc<2, 1,1,2, 3, 0,1,1>();
        TTD::runTestAlloc<2, 1,1,2, 3, 1,0,0>();
        TTD::runTestAlloc<2, 1,1,2, 3, 1,0,1>();
        TTD::runTestAlloc<2, 1,1,2, 3, 1,1,0>();
        TTD::runTestAlloc<2, 1,1,2, 3, 1,1,1>();
        TTD::runTestAlloc<3, 0,0,0, 0, 2,2,2>();
        TTD::runTestAlloc<3, 0,0,0, 1, 0,2,2>();
        TTD::runTestAlloc<3, 0,0,0, 1, 1,2,2>();
        TTD::runTestAlloc<3, 0,0,0, 2, 0,0,2>();
        TTD::runTestAlloc<3, 0,0,0, 2, 0,1,2>();
        TTD::runTestAlloc<3, 0,0,0, 2, 1,0,2>();
        TTD::runTestAlloc<3, 0,0,0, 2, 1,1,2>();
        TTD::runTestAlloc<3, 0,0,0, 3, 0,0,0>();
        TTD::runTestAlloc<3, 0,0,0, 3, 0,0,1>();
        TTD::runTestAlloc<3, 0,0,0, 3, 0,1,0>();
        TTD::runTestAlloc<3, 0,0,0, 3, 0,1,1>();
        TTD::runTestAlloc<3, 0,0,0, 3, 1,0,0>();
        TTD::runTestAlloc<3, 0,0,0, 3, 1,0,1>();
        TTD::runTestAlloc<3, 0,0,0, 3, 1,1,0>();
        TTD::runTestAlloc<3, 0,0,0, 3, 1,1,1>();
        TTD::runTestAlloc<3, 0,0,1, 0, 2,2,2>();
        TTD::runTestAlloc<3, 0,0,1, 1, 0,2,2>();
        TTD::runTestAlloc<3, 0,0,1, 1, 1,2,2>();
        TTD::runTestAlloc<3, 0,0,1, 2, 0,0,2>();
        TTD::runTestAlloc<3, 0,0,1, 2, 0,1,2>();
        TTD::runTestAlloc<3, 0,0,1, 2, 1,0,2>();
        TTD::runTestAlloc<3, 0,0,1, 2, 1,1,2>();
        TTD::runTestAlloc<3, 0,0,1, 3, 0,0,0>();
        TTD::runTestAlloc<3, 0,0,1, 3, 0,0,1>();
        TTD::runTestAlloc<3, 0,0,1, 3, 0,1,0>();
        TTD::runTestAlloc<3, 0,0,1, 3, 0,1,1>();
        TTD::runTestAlloc<3, 0,0,1, 3, 1,0,0>();
        TTD::runTestAlloc<3, 0,0,1, 3, 1,0,1>();
        TTD::runTestAlloc<3, 0,0,1, 3, 1,1,0>();
        TTD::runTestAlloc<3, 0,0,1, 3, 1,1,1>();
        TTD::runTestAlloc<3, 0,1,0, 0, 2,2,2>();
        TTD::runTestAlloc<3, 0,1,0, 1, 0,2,2>();
        TTD::runTestAlloc<3, 0,1,0, 1, 1,2,2>();
        TTD::runTestAlloc<3, 0,1,0, 2, 0,0,2>();
        TTD::runTestAlloc<3, 0,1,0, 2, 0,1,2>();
        TTD::runTestAlloc<3, 0,1,0, 2, 1,0,2>();
        TTD::runTestAlloc<3, 0,1,0, 2, 1,1,2>();
        TTD::runTestAlloc<3, 0,1,0, 3, 0,0,0>();
        TTD::runTestAlloc<3, 0,1,0, 3, 0,0,1>();
        TTD::runTestAlloc<3, 0,1,0, 3, 0,1,0>();
        TTD::runTestAlloc<3, 0,1,0, 3, 0,1,1>();
        TTD::runTestAlloc<3, 0,1,0, 3, 1,0,0>();
        TTD::runTestAlloc<3, 0,1,0, 3, 1,0,1>();
        TTD::runTestAlloc<3, 0,1,0, 3, 1,1,0>();
        TTD::runTestAlloc<3, 0,1,0, 3, 1,1,1>();
        TTD::runTestAlloc<3, 0,1,1, 0, 2,2,2>();
        TTD::runTestAlloc<3, 0,1,1, 1, 0,2,2>();
        TTD::runTestAlloc<3, 0,1,1, 1, 1,2,2>();
        TTD::runTestAlloc<3, 0,1,1, 2, 0,0,2>();
        TTD::runTestAlloc<3, 0,1,1, 2, 0,1,2>();
        TTD::runTestAlloc<3, 0,1,1, 2, 1,0,2>();
        TTD::runTestAlloc<3, 0,1,1, 2, 1,1,2>();
        TTD::runTestAlloc<3, 0,1,1, 3, 0,0,0>();
        TTD::runTestAlloc<3, 0,1,1, 3, 0,0,1>();
        TTD::runTestAlloc<3, 0,1,1, 3, 0,1,0>();
        TTD::runTestAlloc<3, 0,1,1, 3, 0,1,1>();
        TTD::runTestAlloc<3, 0,1,1, 3, 1,0,0>();
        TTD::runTestAlloc<3, 0,1,1, 3, 1,0,1>();
        TTD::runTestAlloc<3, 0,1,1, 3, 1,1,0>();
        TTD::runTestAlloc<3, 0,1,1, 3, 1,1,1>();
        TTD::runTestAlloc<3, 1,0,0, 0, 2,2,2>();
        TTD::runTestAlloc<3, 1,0,0, 1, 0,2,2>();
        TTD::runTestAlloc<3, 1,0,0, 1, 1,2,2>();
        TTD::runTestAlloc<3, 1,0,0, 2, 0,0,2>();
        TTD::runTestAlloc<3, 1,0,0, 2, 0,1,2>();
        TTD::runTestAlloc<3, 1,0,0, 2, 1,0,2>();
        TTD::runTestAlloc<3, 1,0,0, 2, 1,1,2>();
        TTD::runTestAlloc<3, 1,0,0, 3, 0,0,0>();
        TTD::runTestAlloc<3, 1,0,0, 3, 0,0,1>();
        TTD::runTestAlloc<3, 1,0,0, 3, 0,1,0>();
        TTD::runTestAlloc<3, 1,0,0, 3, 0,1,1>();
        TTD::runTestAlloc<3, 1,0,0, 3, 1,0,0>();
        TTD::runTestAlloc<3, 1,0,0, 3, 1,0,1>();
        TTD::runTestAlloc<3, 1,0,0, 3, 1,1,0>();
        TTD::runTestAlloc<3, 1,0,0, 3, 1,1,1>();
        TTD::runTestAlloc<3, 1,0,1, 0, 2,2,2>();
        TTD::runTestAlloc<3, 1,0,1, 1, 0,2,2>();
        TTD::runTestAlloc<3, 1,0,1, 1, 1,2,2>();
        TTD::runTestAlloc<3, 1,0,1, 2, 0,0,2>();
        TTD::runTestAlloc<3, 1,0,1, 2, 0,1,2>();
        TTD::runTestAlloc<3, 1,0,1, 2, 1,0,2>();
        TTD::runTestAlloc<3, 1,0,1, 2, 1,1,2>();
        TTD::runTestAlloc<3, 1,0,1, 3, 0,0,0>();
        TTD::runTestAlloc<3, 1,0,1, 3, 0,0,1>();
        TTD::runTestAlloc<3, 1,0,1, 3, 0,1,0>();
        TTD::runTestAlloc<3, 1,0,1, 3, 0,1,1>();
        TTD::runTestAlloc<3, 1,0,1, 3, 1,0,0>();
        TTD::runTestAlloc<3, 1,0,1, 3, 1,0,1>();
        TTD::runTestAlloc<3, 1,0,1, 3, 1,1,0>();
        TTD::runTestAlloc<3, 1,0,1, 3, 1,1,1>();
        TTD::runTestAlloc<3, 1,1,0, 0, 2,2,2>();
        TTD::runTestAlloc<3, 1,1,0, 1, 0,2,2>();
        TTD::runTestAlloc<3, 1,1,0, 1, 1,2,2>();
        TTD::runTestAlloc<3, 1,1,0, 2, 0,0,2>();
        TTD::runTestAlloc<3, 1,1,0, 2, 0,1,2>();
        TTD::runTestAlloc<3, 1,1,0, 2, 1,0,2>();
        TTD::runTestAlloc<3, 1,1,0, 2, 1,1,2>();
        TTD::runTestAlloc<3, 1,1,0, 3, 0,0,0>();
        TTD::runTestAlloc<3, 1,1,0, 3, 0,0,1>();
        TTD::runTestAlloc<3, 1,1,0, 3, 0,1,0>();
        TTD::runTestAlloc<3, 1,1,0, 3, 0,1,1>();
        TTD::runTestAlloc<3, 1,1,0, 3, 1,0,0>();
        TTD::runTestAlloc<3, 1,1,0, 3, 1,0,1>();
        TTD::runTestAlloc<3, 1,1,0, 3, 1,1,0>();
        TTD::runTestAlloc<3, 1,1,0, 3, 1,1,1>();
        TTD::runTestAlloc<3, 1,1,1, 0, 2,2,2>();
        TTD::runTestAlloc<3, 1,1,1, 1, 0,2,2>();
        TTD::runTestAlloc<3, 1,1,1, 1, 1,2,2>();
        TTD::runTestAlloc<3, 1,1,1, 2, 0,0,2>();
        TTD::runTestAlloc<3, 1,1,1, 2, 0,1,2>();
        TTD::runTestAlloc<3, 1,1,1, 2, 1,0,2>();
        TTD::runTestAlloc<3, 1,1,1, 2, 1,1,2>();
        TTD::runTestAlloc<3, 1,1,1, 3, 0,0,0>();
        TTD::runTestAlloc<3, 1,1,1, 3, 0,0,1>();
        TTD::runTestAlloc<3, 1,1,1, 3, 0,1,0>();
        TTD::runTestAlloc<3, 1,1,1, 3, 0,1,1>();
        TTD::runTestAlloc<3, 1,1,1, 3, 1,0,0>();
        TTD::runTestAlloc<3, 1,1,1, 3, 1,0,1>();
        TTD::runTestAlloc<3, 1,1,1, 3, 1,1,0>();
        TTD::runTestAlloc<3, 1,1,1, 3, 1,1,1>();

        TTD::runTestNoAlloc<0, 2,2,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<0, 2,2,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<1, 0,2,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<1, 0,2,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<1, 1,2,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<1, 1,2,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<2, 0,0,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<2, 0,0,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<2, 0,1,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<2, 0,1,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<2, 1,0,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<2, 1,0,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<2, 1,1,2, 0, 2,2,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 1, 0,2,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 1, 1,2,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 2, 0,0,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 2, 0,1,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 2, 1,0,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 2, 1,1,2>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 0,0,0>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 0,0,1>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 0,1,0>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 0,1,1>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 1,0,0>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 1,0,1>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 1,1,0>();
        TTD::runTestNoAlloc<2, 1,1,2, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 0,0,0, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 0,0,0, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 0,0,1, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 0,0,1, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 0,1,0, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 0,1,0, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 0,1,1, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 0,1,1, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 1,0,0, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 1,0,0, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 1,0,1, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 1,0,1, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 1,1,0, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 1,1,0, 3, 1,1,1>();
        TTD::runTestNoAlloc<3, 1,1,1, 0, 2,2,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 1, 0,2,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 1, 1,2,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 2, 0,0,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 2, 0,1,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 2, 1,0,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 2, 1,1,2>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 0,0,0>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 0,0,1>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 0,1,0>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 0,1,1>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 1,0,0>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 1,0,1>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 1,1,0>();
        TTD::runTestNoAlloc<3, 1,1,1, 3, 1,1,1>();

        TTD::runTestInplaceMemberConstruction<CloneDisabledTestType, 0, 0>();
        TTD::runTestInplaceMemberConstruction<CloneDisabledTestType, 0, 1>();
        TTD::runTestInplaceMemberConstruction<CloneDisabledTestType, 1, 0>();
        TTD::runTestInplaceMemberConstruction<CloneDisabledTestType, 1, 1>();

        TTD::runTestAllocatorPropagation();
#else
        if (verbose) printf("\nNot supported in this build configuration.\n");
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING MOVE AND COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 Construct two pairs, a 'ToPair' and a 'FromPair' (both types
        //:   defined and docced within the 'TestDriver' class) with different
        //:   types.
        //
        // Testing:
        //   pair& operator=(pair<U1, U2>&& rhs);
        //   pair& operator=(const pair<U1, U2>& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE AND COPY ASSIGNMENT"
                            "\n================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_move,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_move,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase12_copy,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

                      // 'bsltf::MoveOnlyAllocTestType' test disabled as
                      // copy-assign is needed
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

        if (verbose) printf(
           "\nTESTING MOVE CONSTRUCTOR WITH ONE ELEMENT MOVED, ONE COPIED"
           "\n===========================================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

                      // 'bsltf::MoveOnlyAllocTestType' test disabled as
                      // copy-assign is needed
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

        if (verbose) printf(
           "\nTESTING MOVE CONSTRUCTOR WITH 2 INDEPENDENTLY MOVED ELEMENTS"
           "\n============================================================\n");

        RUN_EACH_TYPE( MetaTestDriver
                     , testCase10
                     , BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
                     );

        RUN_EACH_TYPE( MetaTestDriver
                     , testCase10
                     , bsltf::MoveOnlyAllocTestType
                     , bsltf::WellBehavedMoveOnlyAllocTestType
                     );
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
        //   pair(pair&& original)
        //   pair(pair&& original, AllocatorPtr basicAllocator)
        // --------------------------------------------------------------------

        if (verbose) printf(
           "\nTESTING MOVE CONSTRUCTOR FOR DIFFERENT TYPE / SAME TYPE PAIR"
           "\n============================================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase9,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
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
            bsl::pair<int&, double >  target(i3, d3);
            bsl::pair<int&, double > *pTarget = &target;

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

            // this is a test for self-assignment, but the compiler warns on
            // explicit self-assignment, so we use a pointer.
            target = *pTarget;

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
            bsl::pair<int,  double&>  target(i3, d3);
            bsl::pair<int,  double&> *pTarget = &target;

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

            // this is a test for self-assignment, but the compiler warns on
            // explicit self-assignment, so we use a pointer.
            target = *pTarget;

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
            bsl::pair<int&, double&>  target(i3, d3);
            bsl::pair<int&, double&> *pTarget = &target;

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

            // this is a test for self-assignment, but the compiler warns on
            // explicit self-assignment, so we use a pointer.
            target = *pTarget;

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
        //: 3 Works for 'const' and non-'const' pairs, members
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
                   int(a1 == a2), int(a1 != a3), int(a1 != a4), int(a1 != a5),
                   int(a1 != a6));
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
                printf("\tp3/p4: %d, p3/p5: %d, p3/p6: %d\n",
                       int(a3 != a4), int(a3 != a5), int(a3 != a6));
            }
            ASSERT(a4 == a5);
            ASSERT(a4 != a6);
            if (veryVerbose) {
                printf("\tp4=p5: %d, p4/p6: %d\n",
                       int(a4 == a5), int(a4 != a6));
            }
            ASSERT(a5 != a6);
            if (veryVerbose) {
                printf("\tp5/p6: %d\n", int(a5 != a6));
            }

            ASSERT(a7 == a8);
            ASSERT(a1 == a8);
            if (veryVerbose) {
                printf("\tp7=p8: %d, p1=p8: %d\n",
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
        //   (including with an allocator argument, which should be ignored)
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
        //     'std::pair'.
        // - When an allocator is not supplied on construction, verify that
        //   the correct default is used by the STRING in the constructed pair.
        // - When 'STRING' does not use a 'bslma::Allocator' and an allocator
        //   is supplied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the appropriate default allocator.
        // - When 'STRING' does use a 'bslma::Allocator' and an allocator
        //   is supplied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the supplied allocator.
        // - Verify that there are no memory leaks.
        //
        // Testing:
        //   pair(const pair<U1, U2>&            rhs);
        //   pair(const pair<U1, U2>&            rhs,
        //        BloombergLP::bslma::Allocator *basicAllocator);
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

            bsl::pair<int, double> p3(P1, &ta1), &P3 = p3;
            ASSERT('A' == P3.first);
            ASSERT(8 == P3.second);

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
#undef U_DECLARE_CONST_VARIANTS

#if defined(BSLSTL_PAIR_TEST_CONDITIONAL_DEFAULT_CTOR)

        if (verbose)
            printf("Testing various pair's default constructibility\n");

        using namespace IsDefaultConstructibleTestTypes;

        if (veryVerbose)
            printf("Testing components (first/second) constructibility\n");

#define ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(TYPE)                                \
        ASSERT(false == std::is_default_constructible<TYPE>::value)

#define ASSERT_DEFAULT_CONSTRUCTIBLE(TYPE)                                    \
        ASSERT(true == std::is_default_constructible<TYPE>::value)

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2);
#endif
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault2);

        ASSERT_DEFAULT_CONSTRUCTIBLE(Empty);
        ASSERT_DEFAULT_CONSTRUCTIBLE(DefArgDefault);
        ASSERT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef);
        ASSERT_DEFAULT_CONSTRUCTIBLE(int);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault&);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2&);
#endif
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault&);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2&);

        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(Empty&);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&);
        ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(int&);

#undef ASSERT_NOT_DEFAULT_CONSTRUCTIBLE
#undef ASSERT_DEFAULT_CONSTRUCTIBLE

        if (veryVerbose)
            printf("All default constructible pair combinations.\n");

#define ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(TYPE1, TYPE2)                       \
        ASSERT((true == std::is_default_constructible<                        \
                                              bsl::pair<TYPE1, TYPE2>>::value))

        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(Empty, Empty);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(Empty, DefArgDefault);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(Empty, ExpDefArgDef);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(Empty, int);

        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(DefArgDefault, Empty);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(DefArgDefault, DefArgDefault);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(DefArgDefault, ExpDefArgDef);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(DefArgDefault, int);

        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, Empty);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, DefArgDefault);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, ExpDefArgDef);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, int);

        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(int, Empty);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(int, DefArgDefault);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(int, ExpDefArgDef);
        ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE(int, int);

#undef ASSERT_PAIR_DEFAULT_CONSTRUCTIBLE

        if (veryVerbose)
            printf("All not default constructible pair combinations.\n");

#define ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(TYPE1, TYPE2)                   \
        ASSERT((false == std::is_default_constructible<                       \
                                              bsl::pair<TYPE1, TYPE2>>::value))

        if (veryVeryVerbose)
            printf("pair<..>::second is not default constructible.\n");

        #ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int, int&);

        if (veryVeryVerbose)
            printf("pair<..>::first is not default constructible.\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, Empty);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault      , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2     , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault    , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault2   , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&         , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault& , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&  , Empty);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&           , Empty);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, DefArgDefault);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault      , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2     , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault    , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault2   , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&         , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault& , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&  , DefArgDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&           , DefArgDefault);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, ExpDefArgDef);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault      , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2     , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault    , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault2   , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&         , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault& , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&  , ExpDefArgDef);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&           , ExpDefArgDef);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, int);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault      , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2     , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault    , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(PrivDefault2   , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&         , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault& , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&  , int);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&           , int);

        if (veryVeryVerbose)
            printf("Neither 'first' or 'second' is default constructible.\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, DeletedDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault, int&);
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2,DeletedDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DeletedDefault2, int&);
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(NoDefault2, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(Empty&, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(DefArgDefault&, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(ExpDefArgDef&, int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, DeletedDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, DeletedDefault2);
#endif
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, NoDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, NoDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, PrivDefault);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, PrivDefault2);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, Empty&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, DefArgDefault&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, ExpDefArgDef&);
        ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE(int&, int&);

#undef ASSERT_PAIR_NOT_DEFAULT_CONSTRUCTIBLE
#endif
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
        //: 5 Operators ==, !=, <, >, <=, >=, and <=> work as designed.
        //: 6 Constructors and relational operators are 'constexpr' under C++14
        //:   mode.
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
        // - Use 'static_assert' to determine that the results of constructors
        //   and relational operators are compile-time constant expressions.
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
        //   auto operator<=>(const pair& x, const pair& y);
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
            testFunctionality<T1, T2>();                                \
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)

        // 'constexpr' tests

        using bsl::pair;

        // Simple constructors

        constexpr int f = 1, s = 2;

        constexpr pair<int, int> pairDefault;

        static_assert(pairDefault.second == 0,
                      "Default constructor is not 'constexpr'.");

        constexpr pair<int, int> pairII(f, s);

        static_assert(pairII.second == 2, "Constructor is not 'constexpr'.");

        // Relational operators

        static_assert(  pairII == pairII,  "Operator == is not 'constexpr'.");
        static_assert(  pairII >= pairII,  "Operator >= is not 'constexpr'.");
        static_assert(  pairII <= pairII,  "Operator <= is not 'constexpr'.");

        static_assert(!(pairII != pairII), "Operator != is not 'constexpr'.");
        static_assert(!(pairII >  pairII), "Operator > is not 'constexpr'.");
        static_assert(!(pairII <  pairII), "Operator < is not 'constexpr'.");

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
        static_assert(pairII <=> pairII == 0,
                      "Operator <=> is not 'constexpr'.");
#endif

        // Rvalue constructors

        static_assert(pair<int, int>(1, 2).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<int, int>(f, 2).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<int, int>(1, s).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<int, int>(f, s).second == 2,
                      "Constructor is not 'constexpr'.");

        // Both clang and gcc warn about integral truncation here, even though
        // this is all done at constexpr time and no truncation occurs.
        static_assert(pair<short, short>(1, 2).second == 2,
                      "Converting constructor is not 'constexpr'.");
        static_assert(pair<short, short>(f, 2).second == 2,
                      "Converting constructor is not 'constexpr'.");
        static_assert(pair<short, short>(1, s).second == 2,
                      "Converting constructor is not 'constexpr'.");
        static_assert(pair<short, short>(f, s).second == 2,
                      "Converting constructor is not 'constexpr'.");

        static_assert(pair<short, short>(pair<int, int>(1, 2)).second == 2,
                      "Templated constructor is not 'constexpr'.");

        static_assert(pair<int, int>(pair<int, int>(1, 2)).second == 2,
                      "Move constructor is not 'constexpr'.");

        // Copying constructors

        static_assert(pair<short, short>(pairII).second == 2,
                      "Templated constructor is not 'constexpr'.");

        static_assert(pair<int, int>(pairII).second == 2,
                      "Copy constructor is not 'constexpr'.");


        // Repeat for pairs holding reference types, as they use different
        // base class specializations for both 'first' and 'second'.

        // Simple constructors

        static_assert(pair<const int&, const int&>(f, s).second == 2,
                      "Constructor is not 'constexpr'.");

        // Relational operators

        static_assert(pair<const int&, const int&>(f, s) ==
                      pair<const int&, const int&>(f, s),
                      "Operator == is not 'constexpr'.");

        static_assert(pair<const int&, const int&>(s, f) >=
                      pair<const int&, const int&>(f, s),
                      "Operator >= is not 'constexpr'.");

        static_assert(pair<const int&, const int&>(s, f) >
                      pair<const int&, const int&>(f, s),
                      "Operator > is not 'constexpr'.");

        static_assert(pair<const int&, const int&>(s, f) !=
                      pair<const int&, const int&>(f, s),
                      "Operator != is not 'constexpr'.");

        static_assert(pair<const int&, const int&>(f, s) <=
                      pair<const int&, const int&>(f, s),
                      "Operator <= is not 'constexpr'.");
        static_assert(pair<const int&, const int&>(f, s) <
                      pair<const int&, const int&>(s, f),
                      "Operator < is not 'constexpr'.");
        static_assert(pair<const int&, const int&>(f, s) !=
                      pair<const int&, const int&>(s, f),
                      "Operator != is not 'constexpr'.");

        // Rvalue constructors

        static_assert(pair<int&&, int&&>(1, 2).second == 2,
                      "Constructor is not 'constexpr'.");
#if 0
        static_assert(pair<int&&, int&&>(f, 2).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<int&&, int&&>(1, s).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<int&&, int&&>(f, s).second == 2,
                      "Constructor is not 'constexpr'.");
#endif

        static_assert(pair<short, short>(1, 2).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<short, short>(f, 2).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<short, short>(1, s).second == 2,
                      "Constructor is not 'constexpr'.");
        static_assert(pair<short, short>(f, s).second == 2,
                      "Constructor is not 'constexpr'.");

        static_assert(pair<short, short>(pair<int, int>(1, 2)).second == 2,
                      "Templated constructor is not 'constexpr'.");
        static_assert(
  pair<const int&, const int&>(pair<const int&, const int&>(f, s)).second == s,
  "Move constructor is not 'constexpr'.");

        // Copying constructors
#if 0   // TBD Need further investigation why ref-binding is not constexpr

        constexpr pair<const int, const int> paircc(f, s);
        constexpr pair<const int&, const int&> pairrr = paircc;

        static_assert(pair<short, short>(pairrr).second == 2,
                      "Templated constructor is not 'constexpr'.");

        static_assert(pair<const int&, const int&>(pairrr).second == 2,
                      "Copy constructor is not 'constexpr'.");
#endif

#endif // BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR

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
