// bslalg_constructorproxy.t.cpp                                      -*-C++-*-

#include <bslalg_constructorproxy.h>

#include <bslma_aamodel.h>
#include <bslma_allocatorutil.h>
#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_hasallocatortype.h>
#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isconvertible.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_bsltestutil.h>
#include <bsls_macrorepeat.h>

#include <bsltf_argumenttype.h>
#include <bsltf_stdallocatoradaptor.h>

#include <cstdio>   // `printf`
#include <cstdlib>  // `atoi`
#include <cstring>

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using std::printf;
using std::fprintf;
using std::fflush;
using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component provides a class template, `ConstructorProxy`, that wraps an
// object whose type might or might not be Allocator Aware (AA). The wrapper
// itself provides no functionality other than the metaprogramming needed to
// pass an allocator to the object's constructor, if appropriate.  The minimal
// wrapper interface provides constructors having 1 to 15 arguments, a
// destructor, and `object` methods that retrieve the wrapped object.
//
// The tests instantiate `ConstructorProxy` with a number of test types and
// check that the constructors correctly transport an allocator argument to the
// proxied object.  The test types will have combinations of the following
// traits that exercise all the aspects of the template under test.
//
//  - Allocator awareness: Non-AA, pmr-AA, bsl-AA, legacy-AA, and STL-AA.
//  - Leading allocator argument-passing protocol (`X(bsl::allocator_arg_t`,
//    alloc, args...)` vs. trailing argument-passing protocol (`X(args...,
//    alloc)'
//  - Bitwise moveable or not.
//
// Constructor arguments are varied along the following axis:
//  - Number: 1 to 15, including a trailing allocator
//  - Type: Must be compatible with the proxied object constructor.
//  - Value category: `const` lvalue, non-`const` lvalue, or rvalue
//  - Proxied or not: A single value argument of proxied type is unwrapped
//    before being passed on.
//
// This proxy class has no basic manipulators.  The `object` methods are used
// to test that the proxied object in is in the expected state.
//-----------------------------------------------------------------------------
// NESTED TYPES AND TRAITS
// [ 3] NESTED TYPES
// [ 3] TRAITS
//
// CREATORS
// [ 4] ConstructorProxy(const allocator_type&);
// [ 6] ConstructorProxy(ARG01&,  const allocator_type&);
// [ 6] ConstructorProxy(ARG01&&, const allocator_type&);
// [ 5] ConstructorProxy(ARG01...ARG14, const allocator_type&);
// [ 4] ~ConstructorProxy();
//
// MANIPULATORS
// [ 4] TYPE& object();
//
// ACCESSORS
// [ 4] const TYPE& object() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 7] USAGE EXAMPLES
//-----------------------------------------------------------------------------

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

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose             = 0;
int veryVerbose         = 0;
int veryVeryVerbose     = 0;
int veryVeryVeryVerbose = 0; // For test allocators

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int g_objectCount = 0;  // Global counter for number of objects
                        // currently constructed.  If this value is not
                        // zero when the test driver exits, that means
                        // the constructor proxy did not construct or
                        // destroy the objects correctly.

// VALUES[0] is not used
static const int VALUES[] = {
    0, 20, 1, 23, 44, 66, 176, 878, 8, 912, 102, 111, 333, 712, 1414
};

typedef bsltf::CopyMoveState                          CMS;
typedef bslmf::MovableRefUtil                         MoveUtil;
typedef bsltf::StdAllocatorAdaptor<bsl::allocator<> > STLAllocator;

typedef bsltf::ArgumentType< 1> ArgType1;
typedef bsltf::ArgumentType< 2> ArgType2;
typedef bsltf::ArgumentType< 3> ArgType3;
typedef bsltf::ArgumentType< 4> ArgType4;
typedef bsltf::ArgumentType< 5> ArgType5;
typedef bsltf::ArgumentType< 6> ArgType6;
typedef bsltf::ArgumentType< 7> ArgType7;
typedef bsltf::ArgumentType< 8> ArgType8;
typedef bsltf::ArgumentType< 9> ArgType9;
typedef bsltf::ArgumentType<10> ArgType10;
typedef bsltf::ArgumentType<11> ArgType11;
typedef bsltf::ArgumentType<12> ArgType12;
typedef bsltf::ArgumentType<13> ArgType13;
typedef bsltf::ArgumentType<14> ArgType14;

/// Packet of 14 `bsltf::ArgumentType` values having initial values
/// `VALUES[1]` through `VALUES[14]`.
struct ArgPack {

    // PUBLIC DATA
    bsltf::ArgumentType< 1> d_arg1;
    bsltf::ArgumentType< 2> d_arg2;
    bsltf::ArgumentType< 3> d_arg3;
    bsltf::ArgumentType< 4> d_arg4;
    bsltf::ArgumentType< 5> d_arg5;
    bsltf::ArgumentType< 6> d_arg6;
    bsltf::ArgumentType< 7> d_arg7;
    bsltf::ArgumentType< 8> d_arg8;
    bsltf::ArgumentType< 9> d_arg9;
    bsltf::ArgumentType<10> d_arg10;
    bsltf::ArgumentType<11> d_arg11;
    bsltf::ArgumentType<12> d_arg12;
    bsltf::ArgumentType<13> d_arg13;
    bsltf::ArgumentType<14> d_arg14;

    // CREATORS
    ArgPack() { reset(); }

    //! ArgPack(const ArgPack&) = default;
    //! ~ArgPack() = default;

    // MANIPULATORS

    //! ArgPack& operator=(const ArgPack&) = default;

    void reset()
    {
        d_arg1  = VALUES[ 1];
        d_arg2  = VALUES[ 2];
        d_arg3  = VALUES[ 3];
        d_arg4  = VALUES[ 4];
        d_arg5  = VALUES[ 5];
        d_arg6  = VALUES[ 6];
        d_arg7  = VALUES[ 7];
        d_arg8  = VALUES[ 8];
        d_arg9  = VALUES[ 9];
        d_arg10 = VALUES[10];
        d_arg11 = VALUES[11];
        d_arg12 = VALUES[12];
        d_arg13 = VALUES[13];
        d_arg14 = VALUES[14];
    }
};

/// Class to hold the value and state of any `ArgumentType` object. Note
/// that this class does *not* track its own copy and move state; the state
/// is frozen at the value acquired from the original construction from
/// `ArgumentType<N>`.
class FrozenArg {

    int       d_value;
    CMS::Enum d_copyMoveState;

  public:
    FrozenArg() : d_value(-1), d_copyMoveState(CMS::e_ORIGINAL) { }

    template <int N>
    FrozenArg(const bsltf::ArgumentType<N>& a) { *this = a; }       // IMPLICIT

    //! FrozenArg(const FrozenArg&) = default;
    //! ~FrozenArg() = default;

    // MANIPULATORS
    //! FrozenArg& operator=(const FrozenArg& rhs) = default;

    template <int N>
    FrozenArg& operator=(const bsltf::ArgumentType<N>& a)
    {
        d_value         = a;
        d_copyMoveState = a.copyMoveState();

        return *this;
    }

    // ACCESSORS
    operator int()            const { return d_value; }
    CMS::Enum copyMoveState() const { return d_copyMoveState; }

    // HIDDEN FRIENDS
    friend
    CMS::Enum copyMoveState(const FrozenArg& fa) { return fa.copyMoveState(); }
};

/// Base class for the 'TestType` template holding the allocator and
/// providing allocator-related typedefs and accessors.  Each instantiation
/// has a protected `AllocArg` type used to pass an allocator to the
/// derived-class constructors and a `k_USES_BSLMA_ALLOCATOR` constant that
/// is `true` for AA types and false for non-AA types.  The primary template
/// is used for bsl-AA, pmr-AA, and STL-AA instantiations; specialization
/// `TestType_Base<void>` is provided for non-AA instantiations and
/// specialization `TestType_Base<bslma::Allocator *>` is provided for
/// legacy-AA instantiations.
template <class ALLOC>
class TestType_Base {

    // DATA
    ALLOC d_allocator;

    // NOT IMPLEMENTED
    TestType_Base& operator=(const TestType_Base&) BSLS_KEYWORD_DELETED;

  protected:
    // PROTECTED CONSTANTS
    enum { k_USES_BSLMA_ALLOCATOR =
           bsl::is_convertible<bslma::Allocator *, ALLOC>::value };

    // PROTECTED TYPES
    typedef const ALLOC& AllocArg;

  public:
    // TYPES
    typedef ALLOC allocator_type;

    // CREATORS
    TestType_Base()                     : d_allocator() { ++g_objectCount; }
    explicit TestType_Base(AllocArg a)  : d_allocator(a) { ++g_objectCount; }
    TestType_Base(const TestType_Base&) : d_allocator() { ++g_objectCount; }
    TestType_Base(bslmf::MovableRef<TestType_Base> original)
        : d_allocator(bslmf::MovableRefUtil::access(original).d_allocator)
        { ++g_objectCount; }
    ~TestType_Base() { --g_objectCount; }

    // ACCESSORS
    allocator_type get_allocator() const { return d_allocator; }

    /// Return `true` if the specified `a` matches the allocator.  The
    /// second overload relies on the fact that most STL allocators used for
    /// testing at Bloomberg can be constructed (not necessarily converted)
    /// from `bslma::Allocator *`; if not, a call to the second overload
    /// will fail to compile.
    bool matchAllocator(AllocArg a) const { return a == d_allocator; }
    bool matchAllocator(bslma::Allocator *a) const
    {
        allocator_type alloc(a);
        return alloc == d_allocator;
    }
};


/// Specialization of base class for derived classes not using an allocator.
template <>
class TestType_Base<void> {

    // NOT IMPLEMENTED
    TestType_Base& operator=(const TestType_Base&) BSLS_KEYWORD_DELETED;

  protected:
    // PROTECTED CONSTANTS
    enum { k_USES_BSLMA_ALLOCATOR = false };

    // PROTECTED TYPES
    class AllocArg { AllocArg(); };  // Non-constructable, non-allocator type

  public:
    // CREATORS
    TestType_Base()                     { ++g_objectCount; }
    TestType_Base(const TestType_Base&) { ++g_objectCount; }
    ~TestType_Base()                    { --g_objectCount; }

    // ACCESSORS
    template <class ANY_ALLOC>
    bool matchAllocator(const ANY_ALLOC&) const { return true; }
};

/// Specialization of base class for `bslma::Allocator *`
template <>
class TestType_Base<bslma::Allocator *> {

    // DATA
    bslma::Allocator *d_allocator;

    // NOT IMPLEMENTED
    TestType_Base& operator=(const TestType_Base&) BSLS_KEYWORD_DELETED;

  protected:
    // PROTECTED CONSTANTS
    enum { k_USES_BSLMA_ALLOCATOR = true };

    // PROTECTED TYPES
    typedef bslma::Allocator *AllocArg;

  public:
    // CREATORS
    TestType_Base()
        : d_allocator(bslma::Default::defaultAllocator()) { ++g_objectCount; }
    explicit TestType_Base(AllocArg a)  : d_allocator(a) { ++g_objectCount; }
    TestType_Base(const TestType_Base&)
        : d_allocator(bslma::Default::defaultAllocator()) { ++g_objectCount; }
    TestType_Base(bslmf::MovableRef<TestType_Base> original)
        : d_allocator(bslmf::MovableRefUtil::access(original).d_allocator)
        { ++g_objectCount; }
    ~TestType_Base() { --g_objectCount; }

    // ACCESSORS
    bslma::Allocator *allocator() const { return d_allocator; }
    bool matchAllocator(const AllocArg a) const { return a == d_allocator; }
};

/// Generalized test type. If `ALLOC` is `bsl::allocator` or
/// `bslma::Allocator *`, every constructor can take an optional allocator
/// argument at the end of its argument list.
template <class ALLOC, bool USE_PREFIX_ARG>
class TestType : public TestType_Base<ALLOC>
{

    // PRIVATE TYPES

    /// Non-constructable, non-allocator, dummy type.
    class DummyAllocArg {
        DummyAllocArg();  // Private constructor
    };
    typedef TestType_Base<ALLOC>     Base;

    // Either `LeadingAllocArg` or `TrailingAllocArg` will be an
    // unconstructible dummy type.  If `ALLOC` is void, then both will be
    // unconstructible dummy types.
    typedef typename bsl::conditional<USE_PREFIX_ARG,
                                      typename Base::AllocArg,
                                      DummyAllocArg
                                     >::type LeadingAllocArg;
    typedef typename bsl::conditional<USE_PREFIX_ARG,
                                      DummyAllocArg,
                                      typename Base::AllocArg
                                     >::type TrailingAllocArg;

    /// Array of `FrozenArg`, taking advantage of compiler-generated default
    /// ctor, copy ctor, and assignment.
    class FrozenArgArray {

        // DATA
        FrozenArg d_data[15];  // Item 0 is not used.

      public:
        // MANIPULATORS
        FrozenArg& operator[](std::size_t i) { return d_data[i]; }

        // ACCESSORS
        const FrozenArg& operator[](std::size_t i) const { return d_data[i]; }
    };

    // DATA
    bsltf::CopyMoveTracker d_tracker;
    FrozenArgArray         d_data;

    // PRIVATE MANIPULATORS
    void setData(FrozenArg a01 = FrozenArg(),
                 FrozenArg a02 = FrozenArg(),
                 FrozenArg a03 = FrozenArg(),
                 FrozenArg a04 = FrozenArg(),
                 FrozenArg a05 = FrozenArg(),
                 FrozenArg a06 = FrozenArg(),
                 FrozenArg a07 = FrozenArg(),
                 FrozenArg a08 = FrozenArg(),
                 FrozenArg a09 = FrozenArg(),
                 FrozenArg a10 = FrozenArg(),
                 FrozenArg a11 = FrozenArg(),
                 FrozenArg a12 = FrozenArg(),
                 FrozenArg a13 = FrozenArg(),
                 FrozenArg a14 = FrozenArg())
    {
        d_data[ 1] = a01;
        d_data[ 2] = a02;
        d_data[ 3] = a03;
        d_data[ 4] = a04;
        d_data[ 5] = a05;
        d_data[ 6] = a06;
        d_data[ 7] = a07;
        d_data[ 8] = a08;
        d_data[ 9] = a09;
        d_data[10] = a10;
        d_data[11] = a11;
        d_data[12] = a12;
        d_data[13] = a13;
        d_data[14] = a14;
        d_tracker.resetCopyMoveState();
    }

    static bsltf::ArgumentTypeDefault defaultArg()
        { return bsltf::ArgumentTypeDefault(); }

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestType, bslma::UsesBslmaAllocator,
                                      Base::k_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestType, bslmf::UsesAllocatorArgT,
                                      (USE_PREFIX_ARG &&
                                       ! bsl::is_same<ALLOC, void>::value));

    // CREATORS
    // Non-allocator creators

    TestType(const TestType& original)
        : Base(original)
        , d_tracker(original.d_tracker)
        , d_data(original.d_data) { }

    TestType(bslmf::MovableRef<TestType> original)
        : Base(MoveUtil::move(static_cast<Base&>(MoveUtil::access(original))))
        , d_tracker(MoveUtil::move(MoveUtil::access(original).d_tracker))
        , d_data(MoveUtil::access(original).d_data)
        {}

    explicit TestType(bsltf::ArgumentTypeByValue< 1> a01 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 2> a02 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 3> a03 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 4> a04 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 5> a05 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 6> a06 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 7> a07 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 8> a08 = defaultArg(),
                      bsltf::ArgumentTypeByValue< 9> a09 = defaultArg(),
                      bsltf::ArgumentTypeByValue<10> a10 = defaultArg(),
                      bsltf::ArgumentTypeByValue<11> a11 = defaultArg(),
                      bsltf::ArgumentTypeByValue<12> a12 = defaultArg(),
                      bsltf::ArgumentTypeByValue<13> a13 = defaultArg(),
                      bsltf::ArgumentTypeByValue<14> a14 = defaultArg())
        : Base(), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }


    // Leading-allocator creators
    TestType(bsl::allocator_arg_t  ,
             LeadingAllocArg       alloc,
             const TestType&       original)
        : Base(alloc)
        , d_tracker(original.d_tracker)
        , d_data(original.d_data) { }

    TestType(bsl::allocator_arg_t        ,
             LeadingAllocArg             alloc,
             bslmf::MovableRef<TestType> original)
        : Base(alloc)
        , d_tracker(MoveUtil::move(MoveUtil::access(original).d_tracker))
        , d_data(MoveUtil::access(original).d_data)
        { }

    TestType(bsl::allocator_arg_t,
             LeadingAllocArg  alloc,
             bsltf::ArgumentTypeByValue< 1> a01 = defaultArg(),
             bsltf::ArgumentTypeByValue< 2> a02 = defaultArg(),
             bsltf::ArgumentTypeByValue< 3> a03 = defaultArg(),
             bsltf::ArgumentTypeByValue< 4> a04 = defaultArg(),
             bsltf::ArgumentTypeByValue< 5> a05 = defaultArg(),
             bsltf::ArgumentTypeByValue< 6> a06 = defaultArg(),
             bsltf::ArgumentTypeByValue< 7> a07 = defaultArg(),
             bsltf::ArgumentTypeByValue< 8> a08 = defaultArg(),
             bsltf::ArgumentTypeByValue< 9> a09 = defaultArg(),
             bsltf::ArgumentTypeByValue<10> a10 = defaultArg(),
             bsltf::ArgumentTypeByValue<11> a11 = defaultArg(),
             bsltf::ArgumentTypeByValue<12> a12 = defaultArg(),
             bsltf::ArgumentTypeByValue<13> a13 = defaultArg(),
             bsltf::ArgumentTypeByValue<14> a14 = defaultArg())
    : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }

    // Trailing-allocator creators
    TestType(const TestType& original, TrailingAllocArg alloc)
        : Base(alloc)
        , d_tracker(original.d_tracker)
        , d_data(original.d_data) { }

    TestType(bslmf::MovableRef<TestType> original, TrailingAllocArg alloc)
        : Base(alloc)
        , d_tracker(MoveUtil::move(MoveUtil::access(original).d_tracker))
        , d_data(MoveUtil::access(original).d_data) { }

    explicit TestType(TrailingAllocArg alloc) : Base(alloc), d_tracker() { }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker() { setData(a01); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker() { setData(a01, a02); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker() { setData(a01, a02, a03); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker() { setData(a01, a02, a03, a04); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker() { setData(a01, a02, a03, a04, a05); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
        { setData(a01, a02, a03, a04, a05, a06); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
        { setData(a01, a02, a03, a04, a05, a06, a07); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
        { setData(a01, a02, a03, a04, a05, a06, a07, a08); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
        { setData(a01, a02, a03, a04, a05, a06, a07, a08, a09); }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             bsltf::ArgumentTypeByValue<10> a10,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10);
    }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             bsltf::ArgumentTypeByValue<10> a10,
             bsltf::ArgumentTypeByValue<11> a11,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11);
    }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             bsltf::ArgumentTypeByValue<10> a10,
             bsltf::ArgumentTypeByValue<11> a11,
             bsltf::ArgumentTypeByValue<12> a12,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12);
    }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             bsltf::ArgumentTypeByValue<10> a10,
             bsltf::ArgumentTypeByValue<11> a11,
             bsltf::ArgumentTypeByValue<12> a12,
             bsltf::ArgumentTypeByValue<13> a13,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13);
    }
    TestType(bsltf::ArgumentTypeByValue< 1> a01,
             bsltf::ArgumentTypeByValue< 2> a02,
             bsltf::ArgumentTypeByValue< 3> a03,
             bsltf::ArgumentTypeByValue< 4> a04,
             bsltf::ArgumentTypeByValue< 5> a05,
             bsltf::ArgumentTypeByValue< 6> a06,
             bsltf::ArgumentTypeByValue< 7> a07,
             bsltf::ArgumentTypeByValue< 8> a08,
             bsltf::ArgumentTypeByValue< 9> a09,
             bsltf::ArgumentTypeByValue<10> a10,
             bsltf::ArgumentTypeByValue<11> a11,
             bsltf::ArgumentTypeByValue<12> a12,
             bsltf::ArgumentTypeByValue<13> a13,
             bsltf::ArgumentTypeByValue<14> a14,
             TrailingAllocArg alloc)
        : Base(alloc), d_tracker()
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }

    ~TestType() { }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        d_tracker = rhs.d_tracker;
        d_data    = rhs.d_data;

        return *this;
    }

    TestType& operator=(bslmf::MovableRef<TestType> rhs)
    {
        TestType& rhsLvalue = rhs;

        d_tracker = MoveUtil::move(rhsLvalue.d_tracker);
        d_data    = rhsLvalue.d_data;

        return *this;
    }

    // ACCESSORS
    const FrozenArg& operator[](std::size_t i) const { return d_data[i]; }

    // HIDDEN FRIENDS
    friend CMS::Enum copyMoveState(const TestType& obj)
        { return obj.d_tracker.copyMoveState(); }
};

// The IBM compiler falls over with an out-of-memory error if there too many
// template instantiations.  For this compiler, we limit tests to 5 arguments
// instead of the maximum 14, relying on testing with the other compilers to
// catch any errors with more than 5 arguments.
#ifdef BSLS_PLATFORM_CMP_IBM
# define REPEAT_UP_TO_MAXARGS(MACRO) BSLS_MACROREPEAT(5, MACRO)
#else
# define REPEAT_UP_TO_MAXARGS(MACRO) BSLS_MACROREPEAT(14, MACRO)
#endif

// These macros are intended for use with `BSLS_MACROREPEAT_*`

/// Get const reference to the nth argument in `const` `ArgPack`, `AP`
#define CONST_ARG(n) AP.d_arg##n

/// Get mutable reference to the nth argument in mutable `ArgPack`, `AP`
#define MUTABLE_ARG(n) mAP.d_arg##n

/// Get rvalue reference to the nth argument in mutable `ArgPack`, `AP`
#define RVALUE_ARG(n) MoveUtil::move(mAP.d_arg##n)

/// Assert that the nth argument in `ArgPack` `AP` has been moved from.
#define ASSERT_ARG_MOVED_FROM(n)                         \
    ASSERTV(n, CMS::isMovedFrom(AP.d_arg##n))

/// For metaprogramming: Defines `allocator_type`, but is not AA.
struct NoAlloc {
    typedef void allocator_type;
};

/// Used to test whether its ctor argument was unwrapped.
class WrapTest : public FrozenArg {

    bool d_wasWrapped;

  public:
    explicit WrapTest(bsltf::ArgumentTypeByValue<1> arg)
        : FrozenArg(arg), d_wasWrapped(false) { }

    explicit
    WrapTest(const bslalg::ConstructorProxy<bsltf::ArgumentType<1> >& arg)
        : FrozenArg(arg.object()), d_wasWrapped(true) { }

    bool wasWrapped() const { return d_wasWrapped; }

    friend
    CMS::Enum copyMoveState(const WrapTest& wt) { return wt.copyMoveState(); }
};

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Conditionally pass an allocator to a template member ctor
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a key-value class template consiting of a
// string key paired with a value of template-parameter type.  Since the value
// type might be allocator aware (AA), we want to ensure that our key-value
// class template can pass an allocator to its value-type constructor.
//
// First, we define a simple AA string class that will be our value type for
// testing:
// ```
//  #include <bslma_bslallocator.h>
//  #include <bslma_allocatorutil.h>
//  #include <cstring>

    /// Basic allocator-aware string class
    class String {

        // DATA
        bsl::allocator<char>  d_allocator;
        std::size_t           d_length;
        char                 *d_data;

      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;

        // CREATORS
        String(const char            *str = "",
               const allocator_type&  a = allocator_type());        // IMPLICIT
        String(const String&          original,
               const allocator_type&  a = allocator_type());
        ~String();

        // MANIPULATORS
        String& operator=(const String& rhs);

        // ACCESSORS
        const char* c_str() const { return d_data; }
        allocator_type get_allocator() const { return d_allocator; }
        std::size_t size() const { return d_length; }
    };

    // FREE FUNCTIONS
    bool operator==(const String& a, const String& b);
//! bool operator!=(const String& a, const String& b);
// ```
// Next, we define the constructors, destructor, and equality-comparison
// operators.  For brevity, we've omited the implementation of the assignment
// operator, which is not used in this example:
// ```
    String::String(const char *str, const allocator_type& a)
        : d_allocator(a), d_length(std::strlen(str))
    {
        d_data = static_cast<char *>(
            bslma::AllocatorUtil::allocateBytes(a, d_length + 1));
        std::memcpy(d_data, str, d_length + 1);
    }

    String::String(const String& original, const allocator_type& a)
        : d_allocator(a), d_length(original.d_length)
    {
        d_data = static_cast<char *>(
            bslma::AllocatorUtil::allocateBytes(a, d_length + 1));
        std::memcpy(d_data, original.c_str(), d_length);
        d_data[d_length] = '\0';
    }

    String::~String()
    {
        bslma::AllocatorUtil::deallocateBytes(d_allocator, d_data, d_length+1);
    }

    bool operator==(const String& a, const String& b)
    {
        return (a.size() == b.size() &&
                0 == std::memcmp(a.c_str(), b.c_str(), a.size()));
    }

//! bool operator!=(const String& a, const String& b)
//! {
//!     return ! (a == b);
//! }
// ```
// Now we are ready to define our key-value template.  The data portion of the
// template needs a member for the key and one for the value.  Rather than
// defining the value member as simply a member variable of `TYPE`, we use
// `bslalg::ConstructorProxy` to ensure that we will be able to construct it in
// a uniform way even though we do not know whether or not it is
// allocator-aware:
// ```
//  #include <bslalg_constructorproxy.h>

    /// Key-value pair with string key and arbitrary value type.
    template <class TYPE>
    class KeyValue {

        // DATA
        String                         d_key;
        bslalg::ConstructorProxy<TYPE> d_valueProxy;
// ```
// Next, we declare the creators and manipulators typical of an AA attribute
// class:
// ```
      public:
        typedef bsl::allocator<> allocator_type;

        // CREATORS
        KeyValue(const String&         k,
                 const TYPE&           v,
                 const allocator_type& a = allocator_type());
        KeyValue(const KeyValue&       original,
                 const allocator_type& a = allocator_type());
        ~KeyValue();

        // MANIPULATORS
        KeyValue& operator=(const KeyValue& rhs);
// ```
// Next, we declare the accessessors and, for convenience in this example,
// define them inline.  Note that the `value` accessor extracts the proxied
// object from the `d_valueProxy` member:
// ```
        // ACCESSESSORS
        allocator_type get_allocator() const { return d_key.get_allocator(); }
        const String&  key()   const { return d_key; }
        const TYPE&    value() const { return d_valueProxy.object(); }
    };
// ```
// Next, we define the value constructor, which passes its allocator argument
// to both data members' constructors.  Note that the `d_valueProxy`,
// constructor always expects an allocator argument, even if `TYPE` is not AA:
// ```
    template <class TYPE>
    KeyValue<TYPE>::KeyValue(const String&         k,
                             const TYPE&           v,
                             const allocator_type& a)
        : d_key(k, a), d_valueProxy(v, a)
    {
    }
// ```
// Next, we define the copy constructor and assignment operator.  Since
// `bslalg::ConstructorProxy` is not copyable, we must manually extract the
// proxied object in the assignment operator.  This extraction is not needed in
// the copy constructor because the single-value proxy constructor
// automatically "unwraps" its argument when presented with an instantiation of
// `bslalg::ConstructorProxy`:
// ```
    template <class TYPE>
    KeyValue<TYPE>::KeyValue(const KeyValue&       original,
                             const allocator_type& a)
        : d_key(original.d_key, a)
        , d_valueProxy(original.d_valueProxy, a)  // Automatically unwrapped
    {
    }

    template <class TYPE>
    KeyValue<TYPE>& KeyValue<TYPE>::operator=(const KeyValue& rhs)
    {
        d_key                 = rhs.d_key;
        d_valueProxy.object() = rhs.d_valueProxy.object();
        return *this;
    }
// ```
// Last, we define the destructor, which does nothing explicit (and could
// therefore have been defaulted), because both `String` and `ConstructorProxy`
// clean up after themselves:
// ```
    template <class TYPE>
    KeyValue<TYPE>::~KeyValue()
    {
    }
// ```
// Now we can illustrate the use of our key-value pair by defining a string-int
// pair and constructing it with a test allocator.  Note that the allocator was
// passed to the (`String`) key, as we would expect:
// ```
//  #include <bslma_testallocator.h>

    void usageExample1()
    {
        bslma::TestAllocator ta;

        KeyValue<int> kv1("hello", 2023, &ta);
        ASSERT("hello" == kv1.key());
        ASSERT(2023    == kv1.value());
        ASSERT(&ta     == kv1.get_allocator());
        ASSERT(&ta     == kv1.key().get_allocator());
// ```
// Next, we define a string-string pair and show that the allocator was
// passed to *both* the key and value parts of the pair:
// ```
        KeyValue<String> kv2("March", "Madness", &ta);
        ASSERT("March"   == kv2.key());
        ASSERT("Madness" == kv2.value());
        ASSERT(&ta       == kv2.get_allocator());
        ASSERT(&ta       == kv2.key().get_allocator());
        ASSERT(&ta       == kv2.value().get_allocator());
// ```
// Finally, we declare a `bslalg::ConstructorProxy` of `KeyValue` and show how
// we can pass more than one argument (up to 14) -- in addition to the
// allocator -- to the proxied type's constructor:
// ```
        typedef KeyValue<int> UnitVal;

        bslalg::ConstructorProxy<UnitVal> uvProxy("km", 14, &ta);
        UnitVal& uv = uvProxy.object();
        ASSERT("km" == uv.key());
        ASSERT(14   == uv.value());
        ASSERT(&ta  == uv.get_allocator());
    }
// ```

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // Set default allocator
    bslma::TestAllocator da("default allocator", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. That the usage examples shown in the component-level
        //    documentation compile and run as described.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, changing
        //    `assert` to `ASSERT` and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // UNWRAPPING CONSTRUCTORS
        //   When the non-allocator argument to `ConstructorProxy` is itself an
        //   instaniation of `ConstructorProxy`, it is "unwrapped" before being
        //   passed to the proxied object constructor.
        //
        // Concerns:
        // 1. A `bslalg::ConstructorProxy<TYPE>` instantiation, `Obj`, when
        //    constructed with two arguments of type
        //    `bslalg::ConstructorProxy<T2>`  and `allocator_type`,
        //    respectively, "unwraps" its first argument and passes the non
        //    non-proxied element to the `TYPE` constructor.
        //
        // 2. The value-category (lvalue vs. rvalue) of the argument is
        //    preserved.
        //
        // Plan:
        // 1. Instrument a class `WrapTest` to detect whether it was
        //    constructed with an `bslmf::ArgumentType<T>` or a
        //    `bslalg::ConstructorProxy<bslmf::ArgumentType<1>>`.
        //
        // 2. Using the `WrapTest` class, construct a
        //    bslalg::ConstructorProxy<WrapTest>' object from a
        //    `bslmf::ArgumentType<1>` object.  Verify that the proxied
        //    `WrapTest` was constructed with a bare `bslmf::ArgumentType<1>`
        //    argument.
        //
        // 3. Repeat step 2, passing a
        //    `bslalg::ConstructorProxy<bslmf::ArgumentType<1>>` argument to
        //    the constructor.   erify that the proxied
        //    `WrapTest` was *still* constructed with a bare
        //    `bslmf::ArgumentType<1>` argument (i.e., the proxy layer was
        //    stripped off).
        //
        // 4. Repeat steps 2 and 3 with `const` and non-`const` arguments and
        //    rvalue (movable) arguments.  Verify that the value category was
        //    preserved.
        //
        // TESTING:
        //     ConstructorProxy(ARG01&,  const allocator_type&);
        //     ConstructorProxy(ARG01&&, const allocator_type&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nUNWRAPPING CONSTRUCTORS"
                            "\n=======================\n");

        typedef bslalg::ConstructorProxy<WrapTest> Obj;

        {
            ArgType1 mArg(1); const ArgType1& ARG = mArg;

            WrapTest w(ARG);
            ASSERT(! w.wasWrapped());

            Obj mX(ARG, &da); const Obj& X = mX;
            ASSERT(! X.object().wasWrapped());
            ASSERT(  CMS::isCopiedConstInto(X.object()));
            ASSERT(! CMS::isMovedInto(X.object()));
            ASSERT(! CMS::isMovedFrom(ARG));

            Obj mY(mArg, &da); const Obj& Y = mY;
            ASSERT(! Y.object().wasWrapped());
            ASSERT(  CMS::isCopiedNonconstInto(Y.object()));
            ASSERT(! CMS::isMovedInto(Y.object()));
            ASSERT(! CMS::isMovedFrom(ARG));

            Obj mZ(MoveUtil::move(mArg), &da); const Obj& Z = mZ;
            ASSERT(! Z.object().wasWrapped());
            ASSERT(! CMS::isCopiedInto(Z.object()));
            ASSERT(  CMS::isMovedInto(Z.object()));
            ASSERT(  CMS::isMovedFrom(ARG));
        }

        {
            typedef bslalg::ConstructorProxy<ArgType1> WrappedArg;
            WrappedArg mArg(1, &da); const WrappedArg& ARG = mArg;

            WrapTest w(ARG);
            ASSERT(w.wasWrapped());

            Obj mX(ARG, &da); const Obj& X = mX;
            ASSERT(! X.object().wasWrapped());
            ASSERT(  CMS::isCopiedConstInto(X.object()));
            ASSERT(! CMS::isMovedInto(X.object()));
            ASSERT(! CMS::isMovedFrom(ARG.object()));

            Obj mY(mArg, &da); const Obj& Y = mY;
            ASSERT(! Y.object().wasWrapped());
            ASSERT(  CMS::isCopiedNonconstInto(Y.object()));
            ASSERT(! CMS::isMovedInto(Y.object()));
            ASSERT(! CMS::isMovedFrom(ARG.object()));

            Obj mZ(MoveUtil::move(mArg), &da); const Obj& Z = mZ;
            ASSERT(! Z.object().wasWrapped());
            ASSERT(! CMS::isCopiedInto(Z.object()));
            ASSERT(  CMS::isMovedInto(Z.object()));
            ASSERT(  CMS::isMovedFrom(ARG.object()));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MULTI-ARGUMENT CONSTRUCTORS
        //
        // Concerns:
        // 1. For a `bslalg::ConstructorProxy<TYPE>` instantiation, `Obj`, the
        //    constructor can be invoked with 1 to 14 arguments plus an
        //    allocator that is convertible to `Obj::allocator_type`,
        //    regardless of which AA model `TYPE` conforms to and regardless of
        //    whether it uses the leading or trailing allocator-passing
        //    convention.
        //
        // 2. If `bslma::UsesBslmaAllocator<TYPE>::value` is `true`, then the
        //    allocator is passed to the proxied-object's extended constructor;
        //    otherwise, the (non-extended) constructor is invoked.
        //
        // 3. The argument values are forwarded to the proxied object's
        //    constructor.
        //
        // 4. The argument value categories (lvalue vs rvalue) are preserved
        //    when passed to the proxied object's constructor.  Note that,
        //    in C++03, a modifiable lvalue is forwarded as a `const` lvalue
        //    reference if there are two or more non-allocator arguments.
        //
        // 5. Concerns 1-4 apply when the constructor argument is passed as a
        //    memory resource pointer of type `bslma::Allocator *` or allocator
        //    of type `allocator_type`.
        //
        // 6. Concerns 1-4 apply when the constructor argument is passed as a
        //    null pointer.  The allocator used to construct the object uses
        //    the default memory resource.
        //
        // Plan:
        // 1. Using the `TestType` template, define a list of `TYPE`s,
        //    conforming to each of the five AA models, and each of the
        //    trailing and leading allocator-passing conventions.  For each
        //    `TYPE`, let `Obj` be `bslalg::ConstructorProxy<TYPE>`.  Perform
        //    the following step on each such `Obj`
        //
        //   1. Create a struct (pack) of 14 `bsltf::ArgumentType<N>` objects
        //      to use as arguments to the constructor.  The initial value of
        //      these objects should come from a known list of semi-random
        //      values.  Create a `const` reference to the pack.
        //
        //   2. For *N* in 1 to 14, construct an `Obj`, passing its constructor
        //      *N* arguments of from the pack described step 1, followed by an
        //      allocator argument compatible with `Obj::allocator_type`.
        //      (C-1)
        //
        //   3. Let `P` be the reference returned by the `object` accessor on
        //      the constructed `Obj`.  Verify that, when
        //      `bslma::UsesBslmaAllocator<TYPE>::value` is `true`, the
        //      allocator used by `P` matches the allocator passed to the `Obj`
        //      constructor; otherwise the allocator used by `P` matches the
        //      default allocator.  (C-2)
        //
        //   4. Verify that the value of the arguments captured in `P` match
        //      the values of the arguments passed to the constructor.  (C-3)
        //
        //   5. Perform step 1.2 through 1.4 three times, passing the arguments
        //      by `const` reference, non-const lvalue reference, and rvalue
        //      (movable) reference, respectively.  Verify the expected values
        //      for `P[n].copiedInto()` and `P[n].movedInto`, where `n` is the
        //      captured argument index.  Note that, in C++03, if the number of
        //      arguments is greater than 1, the expected value of
        //      `P[n].copiedInto()` will be `e_COPIED_CONST` for both `const`
        //      and non-`const` lvalue arguments.  (C-4)
        //
        //   6. Perform step 1.5 twice, passing the allocator as type
        //      `bslma::Allocator *` and `allocator_type`.
        //
        //   7. Repeat step 1.5 one more time, passing the allocator a null
        //      pointer.  Verify that the proxied object is constructed as
        //      though the default allocator were passed in.
        //
        // Testing:
        //    ConstructorProxy(ARG01...ARG14, const allocator_type&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMULTI-ARGUMENT CONSTRUCTORS"
                            "\n===========================\n");

        typedef bsl::polymorphic_allocator<int> PmrAllocatorInt;
        typedef bsl::allocator<short>           BslAllocatorShort;
        typedef bslma::Allocator               *LegacyAllocator;

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  // Copying from a non-const lvalue is always detected correctly in C++11
# define EXP_COPIED_NONCONST(R) true
#else
  // Copying from a non-const is detected correctly in C++03 only if `R == 1`
# define EXP_COPIED_NONCONST(R) (R == 1)
#endif

#define CONSTRUCT_WITH_ALLOC(R) {                                            \
            const int N = R;                                                 \
            ArgPack mAP; const ArgPack& AP = mAP;                            \
            Obj mX(BSLS_MACROREPEAT_COMMA(R, CONST_ARG), alloc);             \
            Obj mY(BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG), alloc);           \
            Obj mZ(BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG), alloc);            \
            const Obj::ValueType& PX = mX.object();                          \
            const Obj::ValueType& PY = mY.object();                          \
            const Obj::ValueType& PZ = mZ.object();                          \
            ASSERTV(N, PX.matchAllocator(expAlloc));                         \
            ASSERTV(N, PY.matchAllocator(expAlloc));                         \
            ASSERTV(N, PZ.matchAllocator(expAlloc));                         \
            for (int i = 1; i <= R; ++i) {                                   \
                ASSERTV(N, i, VALUES[i], PX[i], VALUES[i] == PX[i]);         \
                ASSERTV(N, i, VALUES[i], PY[i], VALUES[i] == PY[i]);         \
                ASSERTV(N, i, VALUES[i], PZ[i], VALUES[i] == PZ[i]);         \
                ASSERTV(N, i, CMS::isCopiedConstInto(PX[i]));                \
                ASSERTV(N, i, CMS::isCopiedInto(PX[i]));                     \
                const bool ExpCNC = EXP_COPIED_NONCONST(R);                  \
                ASSERTV(N, i, ExpCNC == CMS::isCopiedNonconstInto(PY[i]));   \
                ASSERTV(N, i, CMS::isMovedInto(PZ[i]));                      \
            }                                                                \
            BSLS_MACROREPEAT_SEP(R, ASSERT_ARG_MOVED_FROM, ; );              \
        }

#define TEST(ALLOC, USE_PREFIX_ARG, EXP_ALLOC) do {                       \
            typedef TestType<ALLOC, USE_PREFIX_ARG>     ValueType;        \
            typedef bslalg::ConstructorProxy<ValueType> Obj;              \
            bslma::Allocator *rsrc = &ta;                                 \
            bslma::Allocator *expAlloc = EXP_ALLOC ? rsrc : &da;          \
            {                                                             \
                bslma::Allocator *alloc = rsrc;                           \
                REPEAT_UP_TO_MAXARGS(CONSTRUCT_WITH_ALLOC)                \
            }                                                             \
            {                                                             \
                Obj::allocator_type alloc(rsrc);                          \
                REPEAT_UP_TO_MAXARGS(CONSTRUCT_WITH_ALLOC)                \
            }                                                             \
            {                                                             \
                bslma::Allocator *alloc = 0; /* null pointer */           \
                expAlloc = &da;                                           \
                REPEAT_UP_TO_MAXARGS(CONSTRUCT_WITH_ALLOC)                \
            }                                                             \
        } while (false)

        //                       use     exp
        //   ALLOC              prefix  alloc
        //   =================  ====== ======
        TEST(void             , false, false);
        TEST(PmrAllocatorInt  , false, true );
        TEST(PmrAllocatorInt  , true , true );
        TEST(BslAllocatorShort, false, true );
        TEST(BslAllocatorShort, true , true );
        TEST(LegacyAllocator  , false, true );
        TEST(LegacyAllocator  , true , true );
        TEST(STLAllocator     , false, false);
        TEST(STLAllocator     , true , false);

#undef CHECK_RESULTS
#undef CONSTRUCT_WITH_ALLOC
#undef TEST

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ALLOCATOR-ONLY CONSTRUCTOR
        //   Test that the constructor proxy invokes the default constructor
        //   (non-AA types) or extended default constructor (AA types) when
        //   invoked with a single argument (which must be an allocator).
        //
        // Concerns:
        // 1. For a `bslalg::ConstructorProxy<TYPE>` instantiation, `Obj`, the
        //    constructor can be invoked with a single argument -- any
        //    allocator that is convertible to `Obj::allocator_type` --
        //    regardless of which AA model conforms to and regardless of
        //    whether it uses the leading or trailing allocator-passing
        //    convention.
        //
        // 2. If `bslma::UsesBslmaAllocator<TYPE>::value` is `true`, then the
        //    allocator is passed to the proxied-object's extended default
        //    constructor; otherwise, the (non-extended) default constructor is
        //    invoked.
        //
        // 3. The `object` method returns a reference to the
        //    (default-constructed) proxied object, preserving the constnesss
        //    of the proxy.
        //
        // 4. The `Obj` destructor invokes the destructor of the proxied
        //    object.
        //
        // Plan:
        // 1. For a list of `TestType` instantations, `TYPE`, conforming to
        //    each of the five AA models, and each of the trailing and leading
        //    allocator-passing conventions, let `Obj` be
        //    `bslalg::ConstructorProxy<TYPE>`, and verify that:
        //
        //   1. `Obj` can be constructed with a single argument of type
        //      compatible with `Obj::allocator_type`.  (C-1)
        //
        //   2. The `object` method returns a reference to the
        //      default-constructed proxy object.  (C-3)
        //
        //   3. If `TYPE` is a pmr-AA, bsl-AA, or legacy-AA type, then the
        //      object's `matchAllocator` accessor will return `true` when
        //      passed the same allocator that was passed to the constructor;
        //      otherwise `matchAllocator` will return `true` when passed a
        //      default allocator.  (C-2)
        //
        //   4. Using the instrumentation provided by `TestType`, verify that
        //      exactly one object is constructed by the `Obj` constructor and
        //      one object is destroyed by the `Obj` destructor.
        //
        // Testing:
        //    ConstructorProxy(const allocator_type&);
        //    ~ConstructorProxy();
        //    TYPE& object();
        //    const TYPE& object() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nALLOCATOR-ONLY CONSTRUCTOR"
                            "\n==========================\n");

        using namespace bslalg;

        typedef bsl::polymorphic_allocator<int> PmrAllocatorInt;
        typedef bsl::allocator<short>           BslAllocatorShort;
        typedef bslma::Allocator               *LegacyAllocator;

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#define TEST(ALLOC, USE_PREFIX_ARG) do {                                      \
        int objCount = g_objectCount;                                         \
        typedef TestType<ALLOC, USE_PREFIX_ARG> ValueType;                    \
        typedef ConstructorProxy<ValueType>     Obj;                          \
        Obj mX(&ta); const Obj& X = mX;                                       \
        ASSERT(objCount + 1 == g_objectCount);                                \
        ValueType&       mV = mX.object();                                    \
        const ValueType& V  = X.object();                                     \
        ASSERT(&V == &mV);                                                    \
        if (bslma::UsesBslmaAllocator<ValueType>::value) {                    \
            ASSERT(V.matchAllocator(&ta));                                    \
        }                                                                     \
        else {                                                                \
            ASSERT(V.matchAllocator(&da));                                    \
        }                                                                     \
    } while (false)

        const int initObjCount = g_objectCount;

        //                       use
        //   ALLOC              prefix
        //   =================  ======
        TEST(void             , false);
        TEST(PmrAllocatorInt  , false);
        TEST(PmrAllocatorInt  , true );
        TEST(BslAllocatorShort, false);
        TEST(BslAllocatorShort, true );
        TEST(LegacyAllocator  , false);
        TEST(LegacyAllocator  , true );
        TEST(STLAllocator     , false);
        TEST(STLAllocator     , true );

        ASSERT(initObjCount == g_objectCount);

#undef TEST

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // NESTED TYPES and TRAITS
        //
        // Concerns:
        // 1. For a `bslalg::ConstructorProxy<TYPE>` instantiation, `Obj`,
        //    `Obj::ValueType` is the same as `TYPE`.
        //
        // 2. If `TYPE` bsl-AA or legacy-AA, then `Obj::allocator_type` is
        //    `bsl::allocator<>`; otherwise, it's
        //    `bsl::polymorphic_allocator<>`, regardless of
        //    `TYPE::allocator_type::value_type`.
        //
        // 3. `bslma::UsesBslmaAllocator<TYPE>::value` is `true`, regardless of
        //    which AA model conforms to and regardless of whether it uses the
        //    leading or trailing allocator-passing convention.
        //
        // 4. `bslmf::UseAllocatorArgT<bslalg::ConstructorProxy<TYPE>>` always
        //    yields `false`.
        //
        // 5. `bsltf::IsBitwiseMoveable<bslalg::ConstructorProxy<TYPE>>`
        //     mirrors `bsltf::IsBitwiseMoveable<TYPE>`.
        //
        // Plan
        // 1. For a list of `TYPE`s conforming to each of the five AA models,
        //    and each of the trailing and leading allocator-passing
        //    conventions, let `Obj` be `bslalg::ConstructorProxy<TYPE>`, and
        //    verify that:
        //
        //   1. `Obj::valueType` is the same as `TYPE`.  (C-1)
        //
        //   2. `Obj::allocator_type` is `bsl::allocator<>` for `TYPE` being
        //      bsl-AA or legacy-AA; otherwise, `Obj::allocator_type` is
        //      bsl::polymorphic_allocator<>'.  (C-2)
        //
        //   3. `bslma::UsesBslmaAllocator<Obj>::value` is `true`.  (C-3)
        //
        //   4. `bslma::AAModel<Obj>` is `AAModelBsl` for `TYPE` being bsl-AA
        //      or legacy-AA; otherwise, `bslma::AAModel<Obj>` is `AAModelPmr`.
        //
        //   5. `bslmf::UsesAllocatorArgT<Obj>::value` is `false`.  (C-4)
        //
        // 2. Verify that `bsltf::IsBitwiseMoveable<Obj>::value` for
        //    non-bitwise moveable `TYPE` (e.g., `TestType<A, U>`) and `true`
        //    for bitwise moveable `TYPE` (e.g., `int`).  (C-5)
        //
        // Testing:
        //     NESTED TYPES
        //     TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nNESTED TYPES and TRAITS"
                            "\n=======================\n");

        using namespace bslalg;

        typedef bsl::polymorphic_allocator<>    PmrAllocator;
        typedef bsl::allocator<>                BslAllocator;
        typedef bslma::Allocator               *LegacyAllocator;

        typedef bsl::polymorphic_allocator<int> PmrAllocatorInt;
        typedef bsl::allocator<short>           BslAllocatorShort;

#define TEST(ALLOC, USE_PREFIX_ARG, EXP_ALLOC_TYPE, EXP_MODEL) do {           \
        typedef TestType<ALLOC, USE_PREFIX_ARG> ValueType;                    \
        typedef ConstructorProxy<ValueType>     Obj;                          \
        ASSERT((bsl::is_same<Obj::ValueType,      ValueType>::value));        \
        ASSERT((bsl::is_same<Obj::allocator_type, EXP_ALLOC_TYPE>::value));   \
        ASSERT(bslma::AAModel<Obj>::value == EXP_MODEL::value);               \
        ASSERT(bslma::UsesBslmaAllocator<Obj>::value);                        \
        ASSERT(! bslmf::UsesAllocatorArgT<Obj>::value);                       \
    } while (false)

        //                       use       exp
        //   ALLOC              prefix  alloc_type     exp AAModel
        //   =================  ====== ============  =================
        TEST(void             , false, PmrAllocator, bslma::AAModelPmr);
        TEST(PmrAllocatorInt  , false, PmrAllocator, bslma::AAModelPmr);
        TEST(PmrAllocatorInt  , true , PmrAllocator, bslma::AAModelPmr);
        TEST(BslAllocatorShort, false, BslAllocator, bslma::AAModelBsl);
        TEST(BslAllocatorShort, true , BslAllocator, bslma::AAModelBsl);
        TEST(LegacyAllocator  , false, BslAllocator, bslma::AAModelBsl);
        TEST(LegacyAllocator  , true , BslAllocator, bslma::AAModelBsl);
        TEST(STLAllocator     , false, PmrAllocator, bslma::AAModelPmr);
        TEST(STLAllocator     , true , PmrAllocator, bslma::AAModelPmr);

#undef TEST

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        // 1. `FrozenArg` captures the value and copy/move state of any
        //    `bsltf::ArgumentType` instance.
        //
        // 2. `bslma::UsesBslmaAllocator<TestType<ALLOC, USE_PREFIX_ARG>>`
        //    yields `true` if `ALLOC` is `bsl::polymorhic_allocator<T>`,
        //    `bsl::allocator<T>`, or `bslma::Allocator *`.
        //
        // 3. `bslmtf::UsesAllocatorArgT<TestType<ALLOC, USE_PREFIX_ARG>>`
        //    yields `true` if `ALLOC` is non-`void` and `USE_PREFIX_ARG` is
        //    `true`.
        //
        // 4. `AAModel<TestType<ALLOC, U> >::value` yields the expected value
        //    depending on `ALLOC`.
        //
        // 5. `TestType<ALLOC, U>::allocator_type` is `ALLOC` if `ALLOC` is
        //    neither `void` nor `bslma::Allocator *`; otherwise
        //    `allocator_type` is not defined.
        //
        // 6. `TestType` can be constructed with 1 to 14 arguments of type
        //    `bsltf::ArgumentType<1>` to `bsltf::ArgumentType<14>`,
        //    respectively.  The value of the arguments is captured in the
        //    `TestType` object.
        //
        // 7. For each argument of `const` lvalue type passed to a `TestType`
        //    constructor, the captured `FrozenArg` reflects that the
        //    argument's `copiedInto()` attribute is `e_COPIED_CONST`.
        //
        // 8. For each argument of mutable lvalue type passed to a `TestType`
        //    constructor, the captured `FrozenArg` reflects that the
        //    argument's `copiedInto()` attribute is `e_COPIED_MUTABLE`.
        //
        // 9. For each argument of rvalue (bslmf::MutableRef) type passed to a
        //    `TestType` constructor, the captured `FrozenArg` reflects that
        //    the argument's `movedInto()` attribute is `e_MOVED`.
        //
        // 10. If no allocator is supplied, the default allocator is used to
        //    construct `TestType`; `matchAllocator` should return `true` when
        //    called with address of the default allocator and `false` when
        //    called with any other allocator except that `matchType` always
        //    returns `true` if `ALLOC` is `void`.
        //
        // 11. For non-`void` `ALLOC`, `TestType<ALLOC, USE_PREFIX_ARG>`, can
        //    be constructed with a trailing allocator argument (if
        //    `USE_PREFIX_ARG` is `false`) or a leading `bsl::allocator_arg`
        //    followed by an allocator argument (if `USE_PREFIX_ARG` is
        //    `true`).  The allocator accessesor should return a copy of the
        //    allocator provided at construction and `matchAllocator` should
        //    return true when given that allocator and `false` for any other
        //    allocator.
        //
        // 12. The `TestType` copy constructor and extended copy constructor
        //    copy the values and record that the new object is in a
        //    copied-into state.  If an allocator is provided, the new object
        //    uses that allocator, otherwise it uses the default allocator (for
        //    AA instantiations).
        //
        // 13. The `TestType` move constructor and extended move constructor
        //    copy the values, record that the new object is in a moved-into
        //    state, and record that the original object is in a moved-from
        //    state after the move.  If an allocator is provided, the new
        //    object uses that allocator, otherwise it uses the allocator of
        //    the moved-from object (for AA instantiations).
        //
        // 14. The `TestType` assignment operators copy the values but not the
        //    allocators, which remain unchanged.  For copy assignment, the lhs
        //    object enters a copied-into state.  For move assignment, the rhs
        //    object enters a moved-into state and the lhs object enters a
        //    moved-from state.
        //
        // Plan:
        // 1. Put a set of `bsltf::ArgumentType` objects into every legal
        //    move/copy state and arbitrary values and construct an `FrozenArg`
        //    for each such object and verify that the value and move/copy
        //    state is captured correctly.  (C-1)
        //
        // 2. For `ALLOC` types `void`, `bsl::polymorphic_allocator`,
        //    `bsl::allocator`, `bslma::Allocator *`, and an standard-compliant
        //    allocator that is none of the preceding, and for `USE_PREFIX_ARG`
        //    values `false` and `true`, verify, for each combination, `TT`, of
        //    `TestType<ALLOC, USE_PREFIX_ARG>`, that
        //    `bslma::UsesBslmaAllocator<TT>`, `bslmtf::UsesAllocatorArgT<TT>`,
        //    `bslma::AAModel<TT>`, and `TT::allocator_type` have the expected
        //    types.  (C-2, C-3, C-4, C-5)
        //
        // 3. For a representative sample of the types in step 2, including all
        //    of the `ALLOC` types and a selection of `USE_PREFIX_ARG` values,
        //    construct objects of type `TestType<ALLOC, USE_PREFIX_ARG>` using
        //   0. to 14 arguments, where each argument is a `const` lvalue.
        //    Verify that the argument values are captured and that their
        //    `copiedInto` attribute is `e_COPIED_CONST`.  (C-6, C-7)
        //
        // 4. Repeat step 3 with non-`const` lvalue arguments.  Verify that the
        //    captured arguments return `e_COPIED_MUTABLE` from their
        //    `copiedInto()` accessesor.  (C-8)
        //
        // 5. Repeat step 3 with rvalue (`MovableRef`) arguments.  Verify that
        //    the captured arguments return `e_MOVED` from their `movedInto()`
        //    accessesor.  (C-9)
        //
        // 6. Verify that `matchAllocator(&da)` returns `true`, where `da` is
        //    the default allocator.  (C-10)
        //
        // 7. For `ALLOC` types other than `void`, repeat step 3 supplying a
        //    trailing allocator (if `USE_PREFIX_ARG` is `false`) or leading
        //    `bsl::allocator_arg` + allocator (if `USE_PREFIX_ARG` is `true`).
        //    Repeat steps 4 and 5 for the objects constructed this way.  Also
        //    verify that the allocator accessor and `matchAllocator` method
        //    return the expected values.  (C-11)
        //
        // 8. Copy construct and move construct new objects from the ones
        //    created in step 3.  Verify that the values and copy/move state of
        //    the objects is as expected.  Repeat using the extended copy and
        //    move constructors and a different allocator.  Verify that the
        //    allocator is as expected.  (C-12, C-13)
        //
        // 9. Invoke the copy and move assignment operators on objects having
        //    different initial values and different allocators.  Verify that
        //    the values were copied or moved and that the allocators remained
        //    unchanged.  Verify that the copy/move state of all objects is as
        //    expected.  (C-14)
        //
        // Testing
        //     TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST APPARATUS"
                            "\n==============\n");

        typedef bsl::polymorphic_allocator<int>  PmrAllocator;
        typedef bsl::allocator<char>             BslAllocator;
        typedef bslma::Allocator                *LegacyAllocator;

        typedef bslma::AAModelNone   None;
        typedef bslma::AAModelPmr    Pmr;
        typedef bslma::AAModelBsl    Bsl;
        typedef bslma::AAModelLegacy Legacy;
        typedef bslma::AAModelStl    Stl;

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);
        bslma::TestAllocator ta2("test allocator 2", veryVeryVeryVerbose);

#define TEST_TRAITS(ALLOC, USE_PREFIX_ARG,                                    \
                    EXP_BSLMA, EXP_PREFIX, EXP_MODEL, EXP_ALLOC_TYPE) do {    \
        typedef TestType<ALLOC, USE_PREFIX_ARG> TT;                           \
        ASSERT(EXP_BSLMA == bslma::UsesBslmaAllocator<TT>::value);            \
        ASSERT(EXP_PREFIX == bslmf::UsesAllocatorArgT<TT>::value);            \
        ASSERT(EXP_MODEL::value == bslma::AAModel<TT>::value);                \
        ASSERT(EXP_ALLOC_TYPE == bslma::HasAllocatorType<TT>::value);         \
        typedef bsl::conditional<EXP_ALLOC_TYPE, TT,                          \
                                 NoAlloc>::type::allocator_type AllocType;    \
        if (EXP_ALLOC_TYPE) ASSERT((bsl::is_same<AllocType, ALLOC>::value));  \
        ASSERT(! bslmf::IsBitwiseMoveable<TT>::value);                        \
    } while (false)

        //                            use     exp    exp     exp     exp
        //          ALLOC            prefix  bslma  prefix  Model alloc_type
        //          ===============  ======  =====  ====== ====== ==========
        TEST_TRAITS(void           , false,  false, false, None  , false);
        TEST_TRAITS(void           , true ,  false, false, None  , false);
        TEST_TRAITS(PmrAllocator   , false,  true , false, Pmr   , true );
        TEST_TRAITS(PmrAllocator   , true ,  true , true , Pmr   , true );
        TEST_TRAITS(BslAllocator   , false,  true , false, Bsl   , true );
        TEST_TRAITS(BslAllocator   , true ,  true , true , Bsl   , true );
        TEST_TRAITS(LegacyAllocator, false,  true , false, Legacy, false);
        TEST_TRAITS(LegacyAllocator, true ,  true , true , Legacy, false);
        TEST_TRAITS(STLAllocator   , false,  false, false, Stl   , true );
        TEST_TRAITS(STLAllocator   , true ,  false, true , Stl   , true );

        if (veryVerbose) printf("Testing ArgumentHolder\n");
        {
            ArgType1 mA1; const ArgType1& A1 = mA1;
            mA1.set(1, CMS::e_COPIED_CONST_INTO);
            FrozenArg h1(A1);
            ASSERTV(h1, 1 == h1);
            ASSERTV(h1.copyMoveState(),   CMS::isCopiedConstInto(h1));
            ASSERTV(h1.copyMoveState(), ! CMS::isMovedInto(h1));

            ArgType2 mA2; const ArgType2& A2 = mA2;
            mA2.set(2, CMS::e_COPIED_NONCONST_INTO);
            FrozenArg h2(A2);
            ASSERTV(h2, 2 == h2);
            ASSERTV(h2.copyMoveState(),   CMS::isCopiedNonconstInto(h2));
            ASSERTV(h2.copyMoveState(), ! CMS::isMovedInto(h2));

            ArgType3 mA3; const ArgType3& A3 = mA3;
            mA3.set(3, CMS::e_MOVED_INTO);
            FrozenArg h3(A3);
            ASSERTV(h3, 3 == h3);
            ASSERTV(h3.copyMoveState(), ! CMS::isCopiedInto(h3));
            ASSERTV(h3.copyMoveState(),   CMS::isMovedInto(h3));

            ArgType4 mA4; const ArgType4& A4 = mA4;
            mA4.set(-1, CMS::e_MOVED_FROM);
            FrozenArg h4(A4);
            ASSERTV(h4, -1 == h4);
            ASSERTV(h4.copyMoveState(), ! CMS::isCopiedInto(h4));
            ASSERTV(h4.copyMoveState(), ! CMS::isMovedInto(h4));

            ArgType5  mA5; const ArgType5& A5 = mA5;
            FrozenArg h5(A5);
            ASSERTV(h5, -1 == h5);
            ASSERTV(h5.copyMoveState(), ! CMS::isCopiedInto(h5));
            ASSERTV(h5.copyMoveState(), ! CMS::isMovedInto(h5));
        }

#define CHECK_RESULTS(R, ALLOC, ALLOC2)                                   \
            BSLS_MACROREPEAT_SEP(R, ASSERT_ARG_MOVED_FROM, ; );           \
            const TT& X = mX, & Y = mY, & Z = mZ;                         \
            ASSERTV(R, X.matchAllocator(ALLOC));                          \
            ASSERTV(R, Y.matchAllocator(ALLOC));                          \
            ASSERTV(R, Z.matchAllocator(ALLOC));                          \
            ASSERTV(R, C.matchAllocator(defaultAlloc));                   \
            ASSERTV(R, EC.matchAllocator(ALLOC2));                        \
            ASSERTV(R, M.matchAllocator(ALLOC));                          \
            ASSERTV(R, EM.matchAllocator(ALLOC2));                        \
            ASSERTV(R, ! CMS::isCopiedInto(X));                           \
            ASSERTV(R, ! CMS::isMovedFrom(Y));                            \
            ASSERTV(R, ! CMS::isMovedInto(Z));                            \
            ASSERTV(R, CMS::isCopiedConstInto(C));                        \
            ASSERTV(R, CMS::isCopiedConstInto(EC));                       \
            ASSERTV(R, CMS::isMovedInto(M));                              \
            ASSERTV(R, CMS::isMovedInto(EM));                             \
            ASSERTV(R, CMS::isMovedFrom(mMF1));                           \
            ASSERTV(R, CMS::isMovedFrom(mMF2));                           \
            for (int i = 1; i <= R; ++i) {                                \
                ASSERTV(R, i, VALUES[i], X[i], VALUES[i] == X[i]);        \
                ASSERTV(R, i, VALUES[i], Y[i], VALUES[i] == Y[i]);        \
                ASSERTV(R, i, VALUES[i], Z[i], VALUES[i] == Z[i]);        \
                ASSERTV(R, i, VALUES[i], C[i], VALUES[i] == C[i]);        \
                ASSERTV(R, i, VALUES[i], EC[i], VALUES[i] == EC[i]);      \
                ASSERTV(R, i, VALUES[i], M[i], VALUES[i] == M[i]);        \
                ASSERTV(R, i, VALUES[i], EM[i], VALUES[i] == EM[i]);      \
                ASSERTV(R, i, CMS::isCopiedConstInto(X[i]));              \
                ASSERTV(R, i, CMS::isCopiedNonconstInto(Y[i]));           \
                ASSERTV(R, i, CMS::isMovedInto(Z[i]));                    \
            }

#define CONSTRUCT_NO_ALLOC(R) {                                           \
            ArgPack mAP; const ArgPack& AP = mAP;                         \
            TT mX(BSLS_MACROREPEAT_COMMA(R, CONST_ARG));                  \
            TT mY(BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG));                \
            TT mZ(BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG));                 \
            TT mMF1(mX), &mMF2 = mMF1;                                    \
            const TT C(mX);                                               \
            const TT& EC = C;                                             \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT& EM = M;                                             \
            CHECK_RESULTS(R, defaultAlloc, defaultAlloc)                  \
        }

#define CONSTRUCT_TRAILING_ALLOC(R) {                                     \
            ArgPack mAP; const ArgPack& AP = mAP;                         \
            TT mX(BSLS_MACROREPEAT_COMMA(R, CONST_ARG), alloc);           \
            TT mY(BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG), alloc);         \
            TT mZ(BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG), alloc);          \
            TT mMF1(mX, alloc), mMF2(mX, alloc);                          \
            const TT C(mX);                                               \
            const TT EC(mX, alloc2);                                      \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT EM(MoveUtil::move(mMF2), alloc2);                    \
            CHECK_RESULTS(R, alloc, alloc2)                               \
        }

#define CONSTRUCT_LEADING_ALLOC(R) {                                      \
            ArgPack mAP; const ArgPack& AP = mAP;                         \
            TT mX(bsl::allocator_arg, alloc,                              \
                  BSLS_MACROREPEAT_COMMA(R, CONST_ARG));                  \
            TT mY(bsl::allocator_arg, alloc,                              \
                  BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG));                \
            TT mZ(bsl::allocator_arg, alloc,                              \
                  BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG));                 \
            TT mMF1(bsl::allocator_arg, alloc, mX);                       \
            TT mMF2(bsl::allocator_arg, alloc, mX);                       \
            const TT C(mX);                                               \
            const TT EC(bsl::allocator_arg, alloc2, mX);                  \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT EM(bsl::allocator_arg,alloc2,MoveUtil::move(mMF2));  \
            CHECK_RESULTS(R, alloc, alloc2)                               \
        }

        if (veryVerbose) printf("Testing TestType<void, false>\n");
        {
            typedef void AllocType;
            typedef TestType<AllocType, false> TT;
            void *defaultAlloc = 0;

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Aways returns true
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_NO_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt2(ARG);
            tt1 = tt2;
            ASSERTV(CMS::isCopiedConstInto(tt1));
            ASSERTV(tt1[1] == ARG);

            // Test move assignment
            TT tt3;
            tt3 = bslmf::MovableRefUtil::move(tt2);
            ASSERTV(CMS::isMovedInto(tt3));
            ASSERTV(CMS::isMovedFrom(tt2));
            ASSERTV(tt3[1] == ARG);
        }

        if (veryVerbose)
            printf("Testing TestType<bslma::Allocator *, false>\n");
        {
            typedef bslma::Allocator           *AllocType;
            typedef TestType<AllocType, false>  TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Default allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_NO_ALLOC)

            TT tt2(alloc);                       // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_TRAILING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(ARG, alloc2);
            tt2 = tt3;
            ASSERTV(CMS::isCopiedConstInto(tt2));
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment
            TT tt4(alloc);
            tt4 = bslmf::MovableRefUtil::move(tt3);
            ASSERTV(CMS::isMovedInto(tt4));
            ASSERTV(CMS::isMovedFrom(tt3));
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

        if (veryVerbose) printf("Testing TestType<PmrAllocator, true>\n");
        {
            typedef PmrAllocator              AllocType;
            typedef TestType<AllocType, true> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Default allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_NO_ALLOC)

            TT tt2(bsl::allocator_arg, alloc);   // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_LEADING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(bsl::allocator_arg, alloc2, ARG);
            tt2 = tt3;
            ASSERTV(CMS::isCopiedConstInto(tt2));
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment
            TT tt4(bsl::allocator_arg, alloc);
            tt4 = bslmf::MovableRefUtil::move(tt3);
            ASSERTV(CMS::isMovedInto(tt4));
            ASSERTV(CMS::isMovedFrom(tt3));
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

        if (veryVerbose) printf("Testing TestType<BslAllocator, false>\n");
        {
            typedef BslAllocator               AllocType;
            typedef TestType<AllocType, false> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                            // Test default ctor
            ASSERTV(tt1.matchAllocator(&da));  // Default allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_NO_ALLOC)

            TT tt2(alloc);                       // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_TRAILING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(ARG, alloc2);
            tt2 = tt3;
            ASSERTV(CMS::isCopiedConstInto(tt2));
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment
            TT tt4(alloc);
            tt4 = bslmf::MovableRefUtil::move(tt3);
            ASSERTV(CMS::isMovedInto(tt4));
            ASSERTV(CMS::isMovedFrom(tt3));
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

        if (veryVerbose) printf("Testing TestType<STLAllocator, true>\n");
        {
            typedef STLAllocator              AllocType;
            typedef TestType<AllocType, true> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Default allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_NO_ALLOC)

            TT tt2(bsl::allocator_arg, alloc);   // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            REPEAT_UP_TO_MAXARGS(CONSTRUCT_LEADING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(bsl::allocator_arg, alloc2, ARG);
            tt2 = tt3;
            ASSERTV(CMS::isCopiedConstInto(tt2));
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment
            TT tt4(bsl::allocator_arg, alloc);
            tt4 = bslmf::MovableRefUtil::move(tt3);
            ASSERTV(CMS::isMovedInto(tt4));
            ASSERTV(CMS::isMovedFrom(tt3));
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

#undef CHECK_RESULTS
#undef CONSTRUCT_NO_ALLOC
#undef CONSTRUCT_LEADING_ALLOC
#undef CONSTRUCT_TRAILING_ALLOC

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
        // 1. Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bsltf::ArgumentType<1> IntWrapper;

        bslma::TestAllocator ta;
        bsl::allocator<char> alloc(&ta);

        // Test proxy of non-AA types
        bslalg::ConstructorProxy<int> iObj1(alloc);
        ASSERT(0 == iObj1.object());
        bslalg::ConstructorProxy<int> iObj2(5, alloc);
        ASSERT(5 == iObj2.object());

        bslalg::ConstructorProxy<IntWrapper> vObj1(alloc);
        ASSERT(IntWrapper() == vObj1.object());
        bslalg::ConstructorProxy<IntWrapper> vObj2(9, alloc);
        ASSERT(9 == int(vObj2.object()));

        // Test trailing `bslma::Allocator *` argument
        {
            typedef TestType<bslma::Allocator *, false> ValueType;
            typedef bslalg::ConstructorProxy<ValueType> Obj;

            Obj v1(alloc);
            ASSERT(&ta == v1.object().allocator());
            ASSERT(-1  == v1.object()[1]);
            ASSERT(! CMS::isCopiedInto(v1.object()[1]));
            ASSERT(! CMS::isMovedInto(v1.object()[1]));

            ArgType1 arg(99); const ArgType1& ARG = arg;

            Obj v2(arg, alloc);
            ASSERT(&ta == v2.object().allocator());
            ASSERT(ARG == v2.object()[1]);
            ASSERT(  CMS::isCopiedNonconstInto(v2.object()[1]));
            ASSERT(! CMS::isMovedInto(v2.object()[1]));

            Obj v3(ARG, alloc);
            ASSERT(&ta == v3.object().allocator());
            ASSERT(ARG == v3.object()[1]);
            ASSERT(  CMS::isCopiedConstInto(v3.object()[1]));
            ASSERT(! CMS::isMovedInto(v3.object()[1]));

            Obj v4(MoveUtil::move(arg), alloc);
            ASSERT(&ta              == v4.object().allocator());
            ASSERT(99               == v4.object()[1]);
            ASSERT(! CMS::isCopiedInto(v4.object()[1]));
            ASSERT(  CMS::isMovedInto(v4.object()[1]));
        }

        // Test leading `bsl::allocator<>` argument
        {
            typedef TestType<bsl::allocator<int>, true> ValueType;
            typedef bslalg::ConstructorProxy<ValueType> Obj;

            Obj v1(alloc);
            ASSERT(alloc == v1.object().get_allocator());
            ASSERT(-1    == v1.object()[1]);
            ASSERT(! CMS::isCopiedInto(v1.object()[1]));
            ASSERT(! CMS::isMovedInto(v1.object()[1]));

            ArgType1 arg1(4);
            ArgType2 arg2(3);

            Obj v2(arg1, MoveUtil::move(arg2), alloc);
            ASSERT(alloc == v2.object().get_allocator());
            ASSERT(4     == v2.object()[1]);
            ASSERT(3     == v2.object()[2]);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(CMS::isCopiedNonconstInto(v2.object()[1]));
#else
            // non-const lvalue arguments are not perfectly forwarded for 2 or
            // more arguments.
            ASSERT(CMS::isCopiedConstInto(v2.object()[1]));
#endif
            ASSERT(! CMS::isMovedInto(v2.object()[1]));
            ASSERT(! CMS::isCopiedInto(v2.object()[2]));
            ASSERT(  CMS::isMovedInto(v2.object()[2]));
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(test, g_objectCount, 0 == g_objectCount);

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
