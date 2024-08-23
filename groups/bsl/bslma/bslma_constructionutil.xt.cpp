// bslma_constructionutil.t.cpp                                       -*-C++-*-

#include <bslma_constructionutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>

#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <cstdio>      // 'std::printf'
#include <cstdlib>     // 'std::atoi'

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function must have contract
#pragma bde_verify -MN01   // Class data members must be private
#pragma bde_verify -AP02   // Class needs d_allocator_p member
#pragma bde_verify -AL01   // Class needs allocator() method
#pragma bde_verify -MA02   // Allocator not passed to member
#pragma bde_verify -GA01:  // Global variable must use non-default allocator
#pragma bde_verify -FABC01 // Function not in alphanumeric order
#pragma bde_verify -FD03   // Parameter is not documented in function contract
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#pragma bde_verify -IND03  // Function parameter names should align vertically

#pragma bde_verify append dictionary src tbd  // Doesn't work ??
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//@bdetdsplit PARTS (syntax version 1.0.0)
//@
//@# This test driver will be split into multiple parts for faster compilation
//@# using bde_xt_cpp_splitter.py from the bde-tools repo.  Each line below
//@# controls which test cases from this file will be included in one (or more)
//@# standalone test drivers.  Specific contents of each part can be further
//@# controlled by //@bdetdsplit comments throughout this file, for which full
//@# documentation can be found by running:
//@#    bde_xt_cpp_splitter --help usage-guide
//@
//@  CASES: 1, 8
//@  CASES: 2.SLICES, 3
//@  CASES: 4.SLICES, 5
//@  CASES: 6.SLICES, 7
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides primitive operations to construct an object,
// abstracting the fact that the object's constructors may or may not take an
// optional allocator argument.  These primitives allow one to write
// parameterized code (e.g., containers) that constructs objects in a manner
// that is independent of whether an allocator can be passed to the constructor
// those objects and is also independent of object's allocator-passing
// convention.
//
// The allocator argument to the facilities in this component can have one of
// the following five types:
//
//: 1 A non-allocator expressed as a non-class object other than a pointer to
//:   'bslma::Allocator'
//: 2 A pointer to class (derived from) 'bslma::Allocator'
//: 3 An instantiation of 'bsl::allocator'
//: 4 An instantiation of 'bsl::polymorphic_allocator'
//: 5 An STL allocator object of class type meeting the requirements of a C++11
//:   allocator other than an instantiation of 'bsl::allocator' or
//:   'bsl::polymorphic_allocator'
//
// The destination object being constructed will have type 'T', belonging to
// one of the following allocator-awareness (AA) models:
//
//: 1 Non-AA
//: 2 legacy-AA: taking an 'bslma::Allocator *' as a constructor parameter
//: 3 bsl-AA: taking an instantiation of 'bsl::allocator' as a constructor
//:   parameter
//: 4 pmr-AA: taking an instantiation of 'bsl::polymorphic_allocator' as a
//:   constructor parameter
//: 5 stl-AA: taking an allocator parameter of type 'T::allocator_type', where
//:   'T::allocator_type' is not an instantiation of 'bsl::allocator' or
//:   'bsl::polymorphic_allocator'
//
// If the type being constructed is non-AA, the allocator is ignored,
// regardless of its category.  If a non-allocator is passed to a
// 'ConstructionUtil' method, then no allocator is passed to the object
// constructor regardless of the type's AA model.  Otherwise, the allocator
// must be compatible with the type being constructed, as shown in this table:
//
//:                            |                AA MODEL
//: ALLOCATOR TYPE             | non-AA | legacy-AA | bsl-AA | pmr-AA | stl-AA
//: --------------------------------------------------------------------------
//: non-allocator              | YES    | YES       | YES    | YES    | YES
//: bslma::Allocator *         | YES    | YES       | YES    | YES    | NO
//: bsl::allocator             | YES    | YES       | YES    | YES    | NO
//: bsl::polymorphic_allocator | YES    | NO        | NO     | YES    | NO
//: STL allocator              | YES    | NO        | NO     | NO     | YES
//
// We must test all 17 of the valid combinations above.  Furthermore, any AA
// type my conform to one of the following allocator-passing conventions:
//
//: 1 Trailing-allocator Convention: The allocator is passed as the last
//:   argument to the constructor.
//: 2 Leading-allocator Convention: 'bsl::allocator_arg' is passed as the first
//:   argument to the constructor and the allocator is passed as the second
//:   argument.
//
// Thus, the test cases must ensure that all 29 combinations of allocator
// types, AA model, and allocator-passing convensions. Note that there are no
// allocator-passing convensions for non-AA types, hence 29 rather than 34
// combinations.
//
// Other concerns of this component are the proper detection of traits (e.g.,
// 'bslmf::IsBitwiseMoveable') and the correct selection of the implementation.
// Some of these concerns are addressed by the compilation (detecting the wrong
// traits will lead to compilation failure) and some others are addressed by
// runtime detection of values after evaluation.
//-----------------------------------------------------------------------------
// [ 3] void construct(TYPE *, const ALLOCATOR&);
// [ 4] void construct(TYPE *, const ALLOCATOR&, ARGS&&...);
// [ 7] void destructiveMove(TYPE *target, const ALLOCATOR&, TYPE *src)
// [ 5] TYPE make<TYPE>(const ALLOCATOR&);
// [ 6] TYPE make<TYPE>(const ALLOCATOR&, ARGS&&...);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 2] TEST APPARATUS
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define PP(X) printf(#X " = %p\n", (void*)(X));
                                          // Print ptr identifier and value.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

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

typedef bslma::ConstructionUtil Util;
typedef bslma::DestructionUtil  DestructionUtil;
typedef bslmf::MovableRefUtil   MoveUtil;
using bslmf::MovableRef;

// const char k_GARBAGE        = 92;    // Fill value for raw buffer
// const int  k_DESTROYED      = 91;    // "value" of destructed object
// const int  k_MOVED_FROM_VAL = 0x01d; // Value of a moved-from object

//=============================================================================
//                             CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// VALUES[0] is not used
const int VALUES[] = {
    0, 20, 1, 23, 44, 66, 176, 878, 8, 912, 102, 111, 333, 712, 1414
};

class CopyMoveTracker {
    // Type that tracks whether it has been copied into, moved into, or moved
    // from.

  public:
    enum State {
        e_NOT_COPIED_OR_MOVED, // not copied into, moved into, or moved from
        e_CONST_COPIED_INTO,   // copied into from a const reference
        e_MUTABLE_COPIED_INTO, // copied into from a non-const reference
        e_MOVED_INTO,          // moved into
        e_MOVED_FROM           // moved from
    };

  private:
    State d_state;

  public:
    // CREATORS
    CopyMoveTracker() : d_state(e_NOT_COPIED_OR_MOVED) { }

    CopyMoveTracker(CopyMoveTracker      &) : d_state(e_MUTABLE_COPIED_INTO) {}
    CopyMoveTracker(CopyMoveTracker const&) : d_state(e_CONST_COPIED_INTO)   {}
    CopyMoveTracker(MovableRef<CopyMoveTracker> original)
        : d_state(e_MOVED_INTO)
        { MoveUtil::access(original).d_state = e_MOVED_FROM; }

    //! ~CopyMoveTracker() = default;

    // MANIPULATORS
    CopyMoveTracker& operator=(CopyMoveTracker      & rhs)
        { if (&rhs != this) d_state = e_MUTABLE_COPIED_INTO; return *this; }

    CopyMoveTracker& operator=(CopyMoveTracker const& rhs)
        { if (&rhs != this) d_state = e_CONST_COPIED_INTO; return *this; }

    CopyMoveTracker& operator=(MovableRef<CopyMoveTracker> rhs)
    {
        CopyMoveTracker& rhsLvalue = rhs;
        if (&rhsLvalue != this) {
            d_state           = e_MOVED_INTO;
            rhsLvalue.d_state = e_MOVED_FROM;
        }
        return *this;
    }

    void set(State state) { d_state = state; }
    void reset() { d_state = e_NOT_COPIED_OR_MOVED; }

    // ACCESSORS
    State state() const { return d_state; }
    bool isNotCopiedOrMoved()  const
        { return e_NOT_COPIED_OR_MOVED == d_state; }
    bool isConstCopiedInto()   const
        { return e_CONST_COPIED_INTO   == d_state; }
    bool isMutableCopiedInto() const
        { return e_MUTABLE_COPIED_INTO == d_state; }
    bool isCopiedInto() const
        { return isConstCopiedInto() || isMutableCopiedInto(); }
    bool isMovedInto()  const { return e_MOVED_INTO == d_state; }
    bool isMovedFrom()  const { return e_MOVED_FROM == d_state; }
};

template <int N>
class ArgumentType {
    // This class template wraps an integer value and provides implicit
    // conversion to, and explicit conversion from, 'int', while keeping track
    // of copy and move operations.  Its main purpose is for following an
    // argument through a forwarding interface.  Each value of the template
    // parameter 'N' yields a unique type, enabling this test driver to
    // distingusih them when calling through a function template interface,
    // thereby avoiding ambiguities or accidental switching of arguments in the
    // implementation of test-class methods and constructors or in calls to
    // methods and constructors of a class template under test.

    // DATA
    int             d_data;     // attribute value
    CopyMoveTracker d_copyMove;

  public:
    // CREATORS
    ArgumentType() : d_data(-1) { }

    explicit ArgumentType(int value) : d_data(value) { }

    ArgumentType(ArgumentType      & original)
        : d_data(original.d_data), d_copyMove(original.d_copyMove) { }
    ArgumentType(ArgumentType const& original)
        : d_data(original.d_data), d_copyMove(original.d_copyMove) { }

    ArgumentType(MovableRef<ArgumentType> original)
        { operator=(MoveUtil::move(original)); }

    ~ArgumentType()
    {
        volatile int *data_p = &d_data;
        *data_p = int(0xDEAD);
    }

    // MANIPULATORS
    ArgumentType& operator=(ArgumentType      & rhs)
        { d_data = rhs.d_data; d_copyMove = rhs.d_copyMove; return *this; }
    ArgumentType& operator=(ArgumentType const& rhs)
        { d_data = rhs.d_data; d_copyMove = rhs.d_copyMove; return *this; }
    ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs)
    {
        ArgumentType& rhsLvalue = rhs;
        if (this != &rhsLvalue) {
            d_data           = rhsLvalue.d_data;
            rhsLvalue.d_data = -1;
            d_copyMove       = MoveUtil::move(rhsLvalue.d_copyMove);
        }
        return *this;
    }
    ArgumentType& operator=(int rhs) { set(rhs); return *this; }

    void reset() { d_data = -1; d_copyMove.reset(); }
    void set(int                    value,
             CopyMoveTracker::State cms =
                                        CopyMoveTracker::e_NOT_COPIED_OR_MOVED)
        { BSLS_ASSERT(value >= -1); d_data = value; d_copyMove.set(cms); }

    // ACCESSORS
    operator int()                         const { return d_data; }
    CopyMoveTracker::State copyMoveState() const { return d_copyMove.state(); }

    bool isNotCopiedOrMoved()  const
        { return d_copyMove.isNotCopiedOrMoved(); }
    bool isCopiedInto()        const { return d_copyMove.isCopiedInto(); }
    bool isConstCopiedInto()   const { return d_copyMove.isConstCopiedInto(); }
    bool isMutableCopiedInto() const
        { return d_copyMove.isMutableCopiedInto(); }
    bool isMovedInto()         const { return d_copyMove.isMovedInto(); }
    bool isMovedFrom()         const { return d_copyMove.isMovedFrom(); }
};

typedef ArgumentType< 1> ArgType1;
typedef ArgumentType< 2> ArgType2;
typedef ArgumentType< 3> ArgType3;
typedef ArgumentType< 4> ArgType4;
typedef ArgumentType< 5> ArgType5;
typedef ArgumentType< 6> ArgType6;
typedef ArgumentType< 7> ArgType7;
typedef ArgumentType< 8> ArgType8;
typedef ArgumentType< 9> ArgType9;
typedef ArgumentType<10> ArgType10;
typedef ArgumentType<11> ArgType11;
typedef ArgumentType<12> ArgType12;
typedef ArgumentType<13> ArgType13;
typedef ArgumentType<14> ArgType14;

struct ArgPack {
    // Packet of 14 'ArgumentType' values having initial values
    // 'VALUES[1]' through 'VALUES[14]'.

    // PUBLIC DATA
    ArgumentType< 1> d_arg1;
    ArgumentType< 2> d_arg2;
    ArgumentType< 3> d_arg3;
    ArgumentType< 4> d_arg4;
    ArgumentType< 5> d_arg5;
    ArgumentType< 6> d_arg6;
    ArgumentType< 7> d_arg7;
    ArgumentType< 8> d_arg8;
    ArgumentType< 9> d_arg9;
    ArgumentType<10> d_arg10;
    ArgumentType<11> d_arg11;
    ArgumentType<12> d_arg12;
    ArgumentType<13> d_arg13;
    ArgumentType<14> d_arg14;

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

template <int N>
class ByValueParam : public ArgumentType<N> {
    // A pass-by-value parameter of this class can be initialized with an
    // argument of type 'ArgumentType<N>', in the process recording whether
    // the argument was moved or copied.  Using 'ArgumentType<N>' parameter
    // directly does not work becuase, in C++03, initializing a pass-by-value
    // parameter of type 'ArgumentType<N>' from a 'MovableRef' results in an
    // ambiguous conversion sequence (i.e., 'MovableRef::operator T&'
    // vs. 'ArgumentType(MovableRef)' are equally good conversions).  This
    // class eliminates the ambiguity because 'MovableRef<AT>' -> 'AT&' ->
    // 'ByValueParam' requires *two* user-defined conversions and is therefore
    // eliminated during overload resolution.

    typedef ArgumentType<N> Base;

  public:
    ByValueParam() : Base() { }
    ByValueParam(Base const& original) : Base(original) { }
    ByValueParam(Base      & original) : Base(original) { }
    ByValueParam(bslmf::MovableRef<Base> original)
        : Base(MoveUtil::move(original)) { }
};

class FrozenArg {
    // Class to hold the value and state of any 'ArgumentType' object. Note
    // that this class does *not* track its own copy and move state; the state
    // is frozen at the value acquired from the original construction from
    // 'ArgumentType<N>'.

    int                    d_value;
    CopyMoveTracker::State d_copyMoveState;

  public:
    FrozenArg()
        : d_value(-1)
        , d_copyMoveState(CopyMoveTracker::e_NOT_COPIED_OR_MOVED) { }

    template <int N>
    FrozenArg(const ArgumentType<N>& a) { *this = a; }              // IMPLICIT

    //! FrozenArg(const FrozenArg&) = default;
    //! ~FrozenArg() = default;

    // MANIPULATORS
    //! FrozenArg& operator=(const FrozenArg& rhs) = default;

    template <int N>
    FrozenArg& operator=(const ArgumentType<N>& a)
    {
        d_value         = a;
        d_copyMoveState = a.copyMoveState();

        return *this;
    }

    // ACCESSORS
    operator int()                         const { return d_value; }

    CopyMoveTracker::State copyMoveState() const { return d_copyMoveState; }

    bool isNotCopiedOrMoved()  const
        { return CopyMoveTracker::e_NOT_COPIED_OR_MOVED == d_copyMoveState; }
    bool isConstCopiedInto()   const
        { return CopyMoveTracker::e_CONST_COPIED_INTO == d_copyMoveState; }
    bool isMutableCopiedInto() const
        { return CopyMoveTracker::e_MUTABLE_COPIED_INTO == d_copyMoveState; }
    bool isCopiedInto() const
        { return isConstCopiedInto() || isMutableCopiedInto(); }
    bool isMovedInto()  const
        { return CopyMoveTracker::e_MOVED_INTO == d_copyMoveState; }
    bool isMovedFrom()  const
        { return CopyMoveTracker::e_MOVED_FROM == d_copyMoveState; }
};

class TestTypeStats {
    // Empty class to keep track of construction and destruction of 'TestType'
    // objects.  Used as a base class to 'TestType', this class's constructors
    // and destructor keep track of live and total 'TestType' counts and the
    // address of the most-recently-destroyed 'TestType' object.

    // Private CLASS DATA
    static int             s_currentCount;  // count of live objects
    static int             s_totalCount;    // count of total constructor calls
    static TestTypeStats *s_lastDestroyed;  // address of last destroyed object

  public:
    // CLASS METHODS
    static int currentCount() { return s_currentCount; }
        // Return the number of currently live (constructed and not destroyed)
        // 'TestType' objects.

    static int totalCount() { return s_totalCount; }
        // Return the number of 'TestType' objects ever constructed.

    static TestTypeStats *lastDestroyed() { return s_lastDestroyed; }
        // Return the address of the most 'TestType' object for which the
        // destructor was invoked.

    // CREATORS
    TestTypeStats()                     { ++s_currentCount; ++s_totalCount; }
    TestTypeStats(const TestTypeStats&) { ++s_currentCount; ++s_totalCount; }
    ~TestTypeStats() { --s_currentCount; s_lastDestroyed = this; }
};

int            TestTypeStats::s_currentCount  = 0;
int            TestTypeStats::s_totalCount    = 0;
TestTypeStats *TestTypeStats::s_lastDestroyed = 0;

template <class ALLOC>
class TestType_Base : public TestTypeStats {
    // Base class for the 'TestType` template holding the allocator and
    // providing allocator-related typedefs and accessors.  Each instantiation
    // has a protected 'AllocArg' type used to pass an allocator to the
    // derived-class constructors and a 'k_USES_BSLMA_ALLOCATOR' constant that
    // is 'true' for AA types and false for non-AA types.  The primary template
    // is used for bsl-AA, pmr-AA, and STL-AA instantiations; specialization
    // 'TestType_Base<void>' is provided for non-AA instantiations and
    // specialization 'TestType_Base<bslma::Allocator *>' is provided for
    // legacy-AA instantiations.

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
    TestType_Base()                     : d_allocator()  { }
    explicit TestType_Base(AllocArg a)  : d_allocator(a) { }
    TestType_Base(const TestType_Base&) : TestTypeStats(), d_allocator()  { }
    TestType_Base(bslmf::MovableRef<TestType_Base> original)
        : d_allocator(bslmf::MovableRefUtil::access(original).d_allocator) { }
    //! ~TestType_Base() = default;

    // ACCESSORS
    allocator_type get_allocator() const { return d_allocator; }

    bool matchAllocator(AllocArg a) const { return a == d_allocator; }
    bool matchAllocator(bslma::Allocator *a) const
        // Return 'true' if the specified 'a' matches the allocator.  The
        // second overload relies on the fact that most STL allocators used for
        // testing at Bloomberg can be constructed (not necessarily converted)
        // from 'bslma::Allocator *'; if not, a call to the second overload
        // will fail to compile.
    {
        allocator_type alloc(a);
        return alloc == d_allocator;
    }
    bool sameAllocator(const TestType_Base& other) const
        { return d_allocator == other.d_allocator; }
};

template <>
class TestType_Base<void> : public TestTypeStats {
    // Specialization of base class for derived classes not using an allocator.

    // NOT IMPLEMENTED
    TestType_Base& operator=(const TestType_Base&) BSLS_KEYWORD_DELETED;

  protected:
    // PROTECTED CONSTANTS
    enum { k_USES_BSLMA_ALLOCATOR = false };

    // PROTECTED TYPES
    class AllocArg { AllocArg(); };  // Non-constructable, non-allocator type

  public:
    // CREATORS
    TestType_Base() : TestTypeStats() { }
    TestType_Base(const TestType_Base&) : TestTypeStats() { }
    //! ~TestType_Base() = default;

    // ACCESSORS
    template <class ANY_ALLOC>
    bool matchAllocator(const ANY_ALLOC&) const { return true; }
    bool sameAllocator(const TestType_Base&) const { return true; }
};

template <>
class TestType_Base<bslma::Allocator *> : public TestTypeStats {
    // Specialization of base class for 'bslma::Allocator *'

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
    TestType_Base() : d_allocator(bslma::Default::defaultAllocator()) { }
    explicit TestType_Base(AllocArg a)  : d_allocator(a)              { }
    TestType_Base(const TestType_Base&)
        : TestTypeStats(), d_allocator(bslma::Default::defaultAllocator()) { }
    TestType_Base(bslmf::MovableRef<TestType_Base> original)
        : d_allocator(bslmf::MovableRefUtil::access(original).d_allocator) { }
    //! ~TestType_Base() = default;

    // ACCESSORS
    bslma::Allocator *allocator() const { return d_allocator; }
    bool matchAllocator(const AllocArg a) const { return a == d_allocator; }
    bool sameAllocator(const TestType_Base& other) const
        { return d_allocator == other.d_allocator; }
};

template <class ALLOC, bool USE_PREFIX_ARG>
class TestType : public TestType_Base<ALLOC>
{
    // Generalized test type. If 'ALLOC' is 'bsl::allocator' or
    // 'bslma::Allocator *', every constructor can take an optional allocator
    // argument at the end of its argument list.

    // PRIVATE TYPES
    class DummyAllocArg {
        // Non-constructable, non-allocator, dummy type.
        DummyAllocArg();  // Private constructor
    };
    typedef TestType_Base<ALLOC>     Base;

    // Either 'LeadingAllocArg' or 'TrailingAllocArg' will be an
    // unconstructible dummy type.  If 'ALLOC' is void, then both will be
    // unconstructible dummy types.
    typedef typename bsl::conditional<USE_PREFIX_ARG,
                                      typename Base::AllocArg,
                                      DummyAllocArg
                                     >::type LeadingAllocArg;
    typedef typename bsl::conditional<USE_PREFIX_ARG,
                                      DummyAllocArg,
                                      typename Base::AllocArg
                                     >::type TrailingAllocArg;

    class FrozenArgArray {
        // Array of 'FrozenArg', taking advantage of compiler-generated default
        // ctor, copy ctor, and assignment.

        // DATA
        FrozenArg d_data[15];  // Item 0 is not used.

      public:
        // MANIPULATORS
        FrozenArg& operator[](std::size_t i) { return d_data[i]; }

        // ACCESSORS
        const FrozenArg& operator[](std::size_t i) const { return d_data[i]; }
    };

    // DATA
    FrozenArgArray  d_data;
    CopyMoveTracker d_copyMove;  // Track copies/moves

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
        d_copyMove.reset();
    }

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
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(TestType& original)
        : Base(original)
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(bslmf::MovableRef<TestType> original)
        : Base(MoveUtil::move(static_cast<Base&>(MoveUtil::access(original))))
        , d_data(MoveUtil::access(original).d_data)
        , d_copyMove(MoveUtil::move(MoveUtil::access(original).d_copyMove))
        {}

    explicit TestType(ByValueParam< 1> a01 = ByValueParam< 1>(),
                      ByValueParam< 2> a02 = ByValueParam< 2>(),
                      ByValueParam< 3> a03 = ByValueParam< 3>(),
                      ByValueParam< 4> a04 = ByValueParam< 4>(),
                      ByValueParam< 5> a05 = ByValueParam< 5>(),
                      ByValueParam< 6> a06 = ByValueParam< 6>(),
                      ByValueParam< 7> a07 = ByValueParam< 7>(),
                      ByValueParam< 8> a08 = ByValueParam< 8>(),
                      ByValueParam< 9> a09 = ByValueParam< 9>(),
                      ByValueParam<10> a10 = ByValueParam<10>(),
                      ByValueParam<11> a11 = ByValueParam<11>(),
                      ByValueParam<12> a12 = ByValueParam<12>(),
                      ByValueParam<13> a13 = ByValueParam<13>(),
                      ByValueParam<14> a14 = ByValueParam<14>())
    : Base() {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }

    // Leading-allocator creators
    TestType(bsl::allocator_arg_t  ,
             LeadingAllocArg       alloc,
             const TestType&       original)
        : Base(alloc)
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(bsl::allocator_arg_t  ,
             LeadingAllocArg alloc,
             TestType&       original)
        : Base(alloc)
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(bsl::allocator_arg_t        ,
             LeadingAllocArg             alloc,
             bslmf::MovableRef<TestType> original)
        : Base(alloc)
        { operator=(MoveUtil::move(original)); }

    TestType(bsl::allocator_arg_t    ,
             LeadingAllocArg         alloc,
             ByValueParam< 1> a01 = ByValueParam< 1>(),
             ByValueParam< 2> a02 = ByValueParam< 2>(),
             ByValueParam< 3> a03 = ByValueParam< 3>(),
             ByValueParam< 4> a04 = ByValueParam< 4>(),
             ByValueParam< 5> a05 = ByValueParam< 5>(),
             ByValueParam< 6> a06 = ByValueParam< 6>(),
             ByValueParam< 7> a07 = ByValueParam< 7>(),
             ByValueParam< 8> a08 = ByValueParam< 8>(),
             ByValueParam< 9> a09 = ByValueParam< 9>(),
             ByValueParam<10> a10 = ByValueParam<10>(),
             ByValueParam<11> a11 = ByValueParam<11>(),
             ByValueParam<12> a12 = ByValueParam<12>(),
             ByValueParam<13> a13 = ByValueParam<13>(),
             ByValueParam<14> a14 = ByValueParam<14>())
    : Base(alloc)  {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }

    // Trailing-allocator creators
    TestType(const TestType& original, TrailingAllocArg alloc)
        : Base(alloc)
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(TestType& original, TrailingAllocArg alloc)
        : Base(alloc)
        , d_data(original.d_data)
        , d_copyMove(original.d_copyMove) { }

    TestType(bslmf::MovableRef<TestType> original, TrailingAllocArg alloc)
        : Base(alloc)
        { operator=(MoveUtil::move(original)); }

    explicit TestType(TrailingAllocArg alloc) : Base(alloc) { }
    TestType(ByValueParam< 1> a01,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04, a05); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04, a05, a06); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04, a05, a06, a07); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04, a05, a06, a07, a08); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             TrailingAllocArg alloc)
        : Base(alloc) { setData(a01, a02, a03, a04, a05, a06, a07, a08, a09); }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             ByValueParam<10> a10,
             TrailingAllocArg alloc)
        : Base(alloc)
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10);
    }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             ByValueParam<10> a10,
             ByValueParam<11> a11,
             TrailingAllocArg alloc)
        : Base(alloc)
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11);
    }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             ByValueParam<10> a10,
             ByValueParam<11> a11,
             ByValueParam<12> a12,
             TrailingAllocArg alloc)
        : Base(alloc)
    {
        setData(a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12);
    }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             ByValueParam<10> a10,
             ByValueParam<11> a11,
             ByValueParam<12> a12,
             ByValueParam<13> a13,
             TrailingAllocArg alloc)
        : Base(alloc)
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13);
    }
    TestType(ByValueParam< 1> a01,
             ByValueParam< 2> a02,
             ByValueParam< 3> a03,
             ByValueParam< 4> a04,
             ByValueParam< 5> a05,
             ByValueParam< 6> a06,
             ByValueParam< 7> a07,
             ByValueParam< 8> a08,
             ByValueParam< 9> a09,
             ByValueParam<10> a10,
             ByValueParam<11> a11,
             ByValueParam<12> a12,
             ByValueParam<13> a13,
             ByValueParam<14> a14,
             TrailingAllocArg alloc)
        : Base(alloc)
    {
        setData(a01, a02, a03, a04, a05, a06, a07,
                a08, a09, a10, a11, a12, a13, a14);
    }

    ~TestType() { }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        d_data     = rhs.d_data;
        d_copyMove = rhs.d_copyMove;

        return *this;
    }

    TestType& operator=(TestType& rhs)
    {
        d_data     = rhs.d_data;
        d_copyMove = rhs.d_copyMove;

        return *this;
    }

    TestType& operator=(bslmf::MovableRef<TestType> rhs)
    {
        TestType& rhsLvalue = rhs;
        if (this->sameAllocator(rhs)) {
            // Real move
            d_data              = rhsLvalue.d_data;
            d_copyMove      = MoveUtil::move(rhsLvalue.d_copyMove);
        }
        else {
            // Degenerate to a copy
            operator=(rhsLvalue);
        }

        return *this;
    }

    // ACCESSORS
    const FrozenArg& operator[](std::size_t i) const { return d_data[i]; }

    CopyMoveTracker::State copyMoveState() const { return d_copyMove.state(); }

    bool isNotCopiedOrMoved()  const
        { return d_copyMove.isNotCopiedOrMoved(); }
    bool isCopiedInto()        const { return d_copyMove.isCopiedInto(); }
    bool isConstCopiedInto()   const { return d_copyMove.isConstCopiedInto(); }
    bool isMutableCopiedInto() const
        { return d_copyMove.isMutableCopiedInto(); }
    bool isMovedInto()         const { return d_copyMove.isMovedInto(); }
    bool isMovedFrom()         const { return d_copyMove.isMovedFrom(); }
};

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

template <class TYPE>
class Wrapper {
    // Class to test copy elision when initializing a a member subobject using
    // 'ConstructioUtil::make'.

    TYPE d_member;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(Wrapper, bslma::UsesBslmaAllocator);

    explicit Wrapper(const int& arg, bslma::Allocator *alloc = 0)
        : d_member(Util::make<TYPE>(alloc, ArgumentType<1>(arg))) { }

    Wrapper(const Wrapper& original, bslma::Allocator *alloc = 0)
        : d_member(Util::make<TYPE>(alloc, original.d_member)) { }

    const TYPE& unwrap() const { return d_member; }
    bool isNotCopiedOrMoved() const { return d_member.isNotCopiedOrMoved(); }
    bool matchAllocator(bslma::Allocator *a)
        { return d_member.matchAllocator(a); }
    const FrozenArg& operator[](std::size_t i) const { return d_member[i]; }
};

template <class TYPE>
class Derived : public TYPE {
    // Derived class to test copy elision when initializing base class
    // subobject using 'ConstructionUtil::make'.

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(Derived, bslma::UsesBslmaAllocator);

    template <class ARG>
    Derived(const ARG& arg, bslma::Allocator *alloc)
        : TYPE(Util::make<TYPE>(alloc, arg)) { }
};

#endif  // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

class BitwiseMovableTestType : public TestType<bslma::Allocator *, false> {
    // A tracking test type that is bitwise movable.

    typedef TestType<bslma::Allocator *, false> Base;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMovableTestType,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMovableTestType,
                                   bslmf::IsBitwiseMoveable);

    BitwiseMovableTestType() : Base(0) { }

    BitwiseMovableTestType(ArgumentType<1>  arg1,
                           ArgumentType<2>  arg2,
                           bslma::Allocator *alloc)
        : Base(arg1, arg2, alloc) { }
};

// These macros are intended for use with 'BSLS_MACROREPEAT_*'
#define CONST_ARG(n) AP.d_arg##n
    // Get const reference to the nth argument in 'const' 'ArgPack', 'AP'
#define MUTABLE_ARG(n) mAP.d_arg##n
    // Get mutable reference to the nth argument in mutable 'ArgPack', 'AP'
#define RVALUE_ARG(n) MoveUtil::move(mAP.d_arg##n)
    // Get rvalue reference to the nth argument in mutable 'ArgPack', 'AP'
#define ASSERT_ARG_MOVED_FROM(n)                         \
    ASSERTV(n, AP.d_arg##n.isMovedFrom())
    // Assert that the nth argument in 'ArgPack' 'AP' has been moved from.

struct NoAlloc {
    // For metaprogramming: Defines 'allocator_type', but is not AA.
    typedef void allocator_type;
};

typedef void *NonAllocator;

typedef bslma::Allocator *LegacyAllocator;  //@bdetdsplit FOR 3..6, 2.FIRST

template <class TYPE = char>
class PmrAllocator {
    // An allocator that behaves like 'bsl::polymorphic_allocator' without
    // depending on the 'bslma_polymorphic_allocator' higher-level component.

    enum { k_TYPE_ALIGNMENT = bsls::AlignmentFromType<TYPE>::VALUE };

    bsl::memory_resource *d_resource;

    // NOT IMPLEMENTED
    PmrAllocator& operator=(const PmrAllocator&) BSLS_KEYWORD_DELETED;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(PmrAllocator, bslma::IsStdAllocator);

    typedef TYPE value_type;

    template <class ANY_TYPE>
    struct rebind {
        typedef PmrAllocator<ANY_TYPE> other;
    };

    PmrAllocator() : d_resource(bslma::Default::defaultAllocator()) { }

    PmrAllocator(bsl::memory_resource *r) : d_resource(r) { }       // IMPLICIT

    PmrAllocator(const PmrAllocator& original)
        : d_resource(original.resource()) { }

    template <class T2>
    PmrAllocator(const PmrAllocator<T2>& original)
        : d_resource(original.resource()) { }

    //! ~PmrAllocator() = default;

    BSLS_ANNOTATION_NODISCARD TYPE *allocate(std::size_t n) {
        return static_cast<TYPE *>(d_resource->allocate(n * sizeof(TYPE),
                                                        k_TYPE_ALIGNMENT));
    }

    void deallocate(TYPE *p, std::size_t n)
        { d_resource->deallocate(p, n * sizeof(TYPE), k_TYPE_ALIGNMENT); }

    bsl::memory_resource *resource() const { return d_resource; }

    friend bool operator==(const PmrAllocator& a, const PmrAllocator& b)
        { return a.d_resource == b.d_resource; }
    friend bool operator!=(const PmrAllocator& a, const PmrAllocator& b)
        { return a.d_resource != b.d_resource; }
};

template <class TYPE = char>
class BslAllocator : public PmrAllocator<TYPE> {
    // An allocator that behaves like 'bsl::allocator' without depending on
    // the 'bslma_bslallocator' higher-level component.

    typedef PmrAllocator<TYPE> Base;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(BslAllocator, bslma::IsStdAllocator);

    typedef TYPE value_type;

    template <class ANY_TYPE>
    struct rebind {
        typedef BslAllocator<ANY_TYPE> other;
    };

    BslAllocator() { }

    BslAllocator(bslma::Allocator *mechanism)                       // IMPLICIT
        : Base(bslma::Default::allocator(mechanism)) { }

    BslAllocator(const BslAllocator& original) : Base(original) { }
    template <class T2>
    BslAllocator(const BslAllocator<T2>& original)                  // IMPLICIT
        : Base(original.resource()) { }

    //! ~BslAllocator() = default;

    bslma::Allocator *mechanism() const {
        return static_cast<bslma::Allocator *>(this->resource());
    }
};

template <class TYPE = char>
class STLAllocator {
    // A Standard-compliant allocator

    bslma::Allocator *d_mechanism;

    // NOT IMPLEMENTED
    STLAllocator& operator=(const STLAllocator&) BSLS_KEYWORD_DELETED;

    template <class T2> friend class STLAllocator;

  public:
    typedef TYPE value_type;

    BSLMF_NESTED_TRAIT_DECLARATION(STLAllocator, bslma::IsStdAllocator);

    template <class ANY_TYPE>
    struct rebind {
        typedef STLAllocator<ANY_TYPE> other;
    };

    STLAllocator() : d_mechanism(bslma::Default::defaultAllocator()) { }

    explicit STLAllocator(bslma::Allocator *m) : d_mechanism(m) { }

    STLAllocator(const STLAllocator& original)
        : d_mechanism(original.d_mechanism) { }

    template <class T2>
    STLAllocator(const STLAllocator<T2>& original)
        : d_mechanism(original.d_mechanism) { }

    //! ~STLAllocator() = default;

    BSLS_ANNOTATION_NODISCARD TYPE *allocate(std::size_t n);
    void deallocate(TYPE *p, std::size_t n);
        // These functions are not used, so are not defined.

    friend bool operator==(const STLAllocator& a, const STLAllocator& b)
        { return a.d_mechanism == b.d_mechanism; }
    friend bool operator!=(const STLAllocator& a, const STLAllocator& b)
        { return a.d_mechanism != b.d_mechanism; }
};

class CopyOnlyTestType : public TestType<BslAllocator<>, false> {
    // Simplified 'TestType', but lacking a dedicated move constructor (moves
    // degenerate to copies).

    typedef TestType<BslAllocator<>, false> Base;

  public:
    explicit CopyOnlyTestType(const BslAllocator<>& a = BslAllocator<>())
        : Base(a) { }

    explicit CopyOnlyTestType(int   i,
                              const BslAllocator<>& a = BslAllocator<>())
        : Base(ArgType1(i), a) { }

    CopyOnlyTestType(const CopyOnlyTestType& original,
                     const BslAllocator<>&   a = BslAllocator<>())
        : Base(original, a) { }
};

} // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//@bdetdsplit FOR 8 BEGIN
// Mimic 'bsl::allocator' for usage example
namespace bsl {
namespace {  // unnamed namespace within 'bsl' namespace

template <class TYPE = char>
class allocator : public BslAllocator<TYPE>
{
    typedef BslAllocator<TYPE> Base;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(allocator, bslma::IsStdAllocator);

    template <class TYPE2>
    struct rebind {
        typedef allocator<TYPE2> other;
    };

    allocator() { }
    allocator(bslma::Allocator *m) : Base(m) { }                    // IMPLICIT
    template <class TYPE2>
    allocator(const allocator<TYPE2>& original) : Base(original) {} // IMPLICIT
    //! allocator(const allocator&);
    //! ~allocator();
};

}  // close unnamed namespace within 'bsl' namespace
}  // close bsl namespace

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslma::ConstructionUtil' to Implement a Container
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the intended use of 'bslma::ConstructionUtil' to
// implement a simple container class that uses an instance of 'bsl::allocator'
// for memory management.
//
// First, because allocation and construction are done in two separate steps,
// we need to define a proctor type that will deallocate the allocated memory
// in case the constructor throws an exception:
//..
//  #include <bslma_bslallocator.h>

    template <class TYPE>
    class MyContainerProctor {
        // This class implements a proctor to release memory allocated during
        // the construction of a 'MyContainer' object if the constructor for
        // the container's data element throws an exception.  Such a proctor
        // should be 'release'd once the element is safely constructed.

        // DATA
        bsl::allocator<TYPE>  d_allocator;
        TYPE                 *d_address_p;    // proctored memory

      private:
        // NOT IMPLEMENTED
        MyContainerProctor(const MyContainerProctor&);             // = delete
        MyContainerProctor& operator=(const MyContainerProctor&);  // = delete

      public:
        // CREATORS
        MyContainerProctor(const bsl::allocator<TYPE> allocator, TYPE *address)
            // Create a proctor that conditionally manages the memory at the
            // specified 'address', and that uses the specified 'allocator' to
            // deallocate the block of memory (if not released -- see
            // 'release') upon destruction.  The behavior is undefined unless
            // 'allocator' is non-zero and supplied the memory at 'address'.
        : d_allocator(allocator)
        , d_address_p(address)
        {
        }

        ~MyContainerProctor()
            // Destroy this proctor, and deallocate the block of memory it
            // manages (if any) by invoking the 'deallocate' method of the
            // allocator that was supplied at construction of this proctor.  If
            // no memory is currently being managed, this method has no effect.
        {
            if (d_address_p) {
                d_allocator.deallocate(d_address_p, 1);
            }
        }

        // MANIPULATORS
        void release()
            // Release from management the block of memory currently managed by
            // this proctor.  If no memory is currently being managed, this
            // method has no effect.
        {
            d_address_p = 0;
        }
    };
//..
// Then, we create a container class that holds a single element and uses
// 'bsl::allocator' to supply memory:
//..
//  #include <bslma_constructionutil.h>

    template <class TYPE>
    class MyContainer {
        // This class provides a container that always holds exactly one
        // element, dynamically allocated using the specified 'bslma'
        // allocator.

        // DATA
        bsl::allocator<TYPE>  d_allocator;
        TYPE                 *d_value_p;

        TYPE *createElement();
        TYPE *createElement(const TYPE& value);
            // Return the address of a new element that was allocated from this
            // container's allocator and initialized with the optionally
            // specified 'value', or default-initialized if 'value' is not
            // specified.  If 'TYPE' is AA, this container's allocator is used
            // to construct the new element.

      public:
        typedef bsl::allocator<TYPE>  allocator_type;

        // CREATORS
        explicit
        MyContainer(const allocator_type& allocator = allocator_type())
            // Create a container with a default-constructed element.
            // Optionally specify a 'allocator' used to supply memory.
            : d_allocator(allocator), d_value_p(createElement()) { }

        explicit
        MyContainer(const TYPE&           value,
                    const allocator_type& allocator = allocator_type())
            // Create a container having an element constructed from the
            // specified 'value'.  Optionally specify an 'allocator' to supply
            // memory both for the container and for the contained element.
            : d_allocator(allocator), d_value_p(createElement(value)) { }

        MyContainer(const MyContainer&    original,
                    const allocator_type& allocator = allocator_type())
            // Create a container having the same value as the specified
            // 'original' object.  Optionally specify a 'allocator' used
            // to supply memory.  If 'allocator' is 0, the currently
            // installed default allocator is used.
            : d_allocator(allocator)
            , d_value_p(createElement(*original.d_value_p)) { }

        ~MyContainer();
            // Destroy this object.

        // MANIPULATORS
        MyContainer& operator=(const TYPE& rhs);
        MyContainer& operator=(const MyContainer& rhs);
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.

        TYPE& front()
            // Return a non-'const' reference to the element contained in this
            // object.
        {
            return *d_value_p;
        }

        // ACCESSORS
        const TYPE& front() const
            // Return a 'const' reference to the element contained in this
            // object.
        {
            return *d_value_p;
        }

        allocator_type get_allocator() const
            // Return the allocator used by this object to supply memory.
        {
            return d_allocator;
        }

        // etc.
    };
//..
// Next, we implement the private 'createElement' members that allocate memory
// and construct a 'TYPE' object in the allocated memory.  We perform the
// allocation using the 'allocate' method of 'bsl::allocator' and the
// construction using the 'construct' method of 'ConstructionUtil' that
// provides the correct semantics for passing the allocator to the constructed
// object when appropriate:
//..
    template <class TYPE>
    TYPE *MyContainer<TYPE>::createElement()
    {
        TYPE *value_p = d_allocator.allocate(1);
        MyContainerProctor<TYPE> proctor(d_allocator, value_p);

        // Call 'construct' passing the allocator but no constructor
        // arguments.

        bslma::ConstructionUtil::construct(value_p, d_allocator);
        proctor.release();

        return value_p;
    }

    template <class TYPE>
    TYPE *MyContainer<TYPE>::createElement(const TYPE& value)
    {
        TYPE *value_p = d_allocator.allocate(1);
        MyContainerProctor<TYPE> proctor(d_allocator, value_p);

        // Call 'construct' passing the allocator and 'value' arguments.

        bslma::ConstructionUtil::construct(value_p, d_allocator, value);
        proctor.release();

        return value_p;
    }
//..
// Now, the destructor destroys the object and deallocates the memory used to
// hold the element using the allocator:
//..
    template <class TYPE>
    MyContainer<TYPE>::~MyContainer()
    {
        d_value_p->~TYPE();
        d_allocator.deallocate(d_value_p, 1);
    }
//..
// Next, the assignment operator needs to assign the value without modifying
// the allocator.
//..
    template <class TYPE>
    MyContainer<TYPE>& MyContainer<TYPE>::operator=(const TYPE& rhs)
    {
        if (&rhs != d_value_p) {
            *d_value_p = rhs;
        }
        return *this;
    }

    template <class TYPE>
    MyContainer<TYPE>& MyContainer<TYPE>::operator=(const MyContainer& rhs)
    {
        return operator=(*rhs.d_value_p);
    }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
    int usageExample1()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<int>     C1(123, &testAlloc);
        ASSERT(C1.get_allocator() == &testAlloc);
        ASSERT(C1.front()         == 123);

        MyContainer<int> C2(C1);
        ASSERT(C2.get_allocator() == bslma::Default::defaultAllocator());
        ASSERT(C2.front()         == 123);

        return 0;
    }
//..
//
///Example 2: 'bslma' Allocator Propagation
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates that 'MyContainer' does indeed propagate the
// allocator to its contained element.
//
// First, we create a representative element class, 'MyType'.  Unlike the
// 'MyContainer' template, 'MyType' allocates memory using the
// 'bslma::Allocator *' (legacy) allocator model instead of the
// 'bsl::allocator' (bsl) allocator model:
//..
//  #include <bslma_default.h>

    class MyType {

        // DATA
        bslma::Allocator *d_allocator_p;
        int               d_value;
        // ...

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit MyType(bslma::Allocator *basicAllocator = 0)
            // Create a 'MyType' object having the default value.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.
            : d_allocator_p(bslma::Default::allocator(basicAllocator))
            , d_value()
        {
            // ...
        }

        explicit MyType(int               value,
                        bslma::Allocator *basicAllocator = 0)
            // Create a 'MyType' object having the specified 'value'.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.
            : d_allocator_p(bslma::Default::allocator(basicAllocator))
            , d_value(value)
        {
            // ...
        }

        MyType(const MyType& original, bslma::Allocator *basicAllocator = 0)
            // Create a 'MyType' object having the same value as the specified
            // 'original' object.  Optionally specify a 'basicAllocator' used
            // to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
        : d_allocator_p(bslma::Default::allocator(basicAllocator))
        , d_value(original.value())
        {
            // ...
        }

        // ...

        // ACCESSORS
        bslma::Allocator *allocator() const
            // Return the allocator used by this object to supply memory.
        {
            return d_allocator_p;
        }

        int value() const
            // Return the value of this object.
        {
            return d_value;
        }

        // ...
    };
//..
// Finally, we instantiate 'MyContainer' using 'MyType' and verify that, when
// we provide an allocator to the constructor of the container, the same
// allocator is passed to the constructor of the contained element.  Because
// the container and the element implement different allocator models, the
// invocation of 'bslma::ConstructionUtil::construct' automatically adapts the
// 'bsl::allocator' held by the container to a 'bslma::Allocator' pointer
// expected by the element.  We also verify that, when the container is
// copy-constructed without supplying an allocator, the copy uses the default
// allocator, not the allocator from the original object.  Moreover, we verify
// that the element stored in the copy also uses the default allocator:
//..
    void usageExample2()
    {
        bslma::TestAllocator testAlloc;
        bslma::TestAllocator testAlloc2;

        MyContainer<MyType>  C1(&testAlloc);  // extended default constructor
        ASSERT(C1.get_allocator()     == &testAlloc);
        ASSERT(C1.front().allocator() == &testAlloc);
        ASSERT(C1.front().value()     == 0);

        MyContainer<MyType>  C2(MyType(22), &testAlloc);  // value constructor
        ASSERT(C2.get_allocator()     == &testAlloc);
        ASSERT(C2.front().allocator() == &testAlloc);
        ASSERT(C2.front().value()     == 22);

        MyContainer<MyType> C3(C2);
        ASSERT(C3.get_allocator()     != C2.get_allocator());
        ASSERT(C3.get_allocator()     == bslma::Default::defaultAllocator());
        ASSERT(C3.front().allocator() != C1.front().allocator());
        ASSERT(C3.front().allocator() == bslma::Default::defaultAllocator());
        ASSERT(C3.front().value()     == 22);

        MyContainer<MyType> C4(C2, &testAlloc2);
        ASSERT(C4.get_allocator()     == &testAlloc2);
        ASSERT(C4.front().allocator() == &testAlloc2);
        ASSERT(C4.front().value()     == 22);
    }
//..
//
#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
///Example 3: Constructing into Non-heap Memory
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'bslma::ConstructionUtil::make' to
// implement a simple wrapper class that contains a single item that might or
// might not use the 'bslma' allocator protocol.
//
// First, we define a wrapper class that holds an object and a functor.  The
// functor (known as the *listener*) is called each time the wrapped object is
// changes.  We store the object directly as a member variable, instead of
// using an uninitialized buffer, to avoid a separate construction step:
//..
    template <class TYPE, class FUNC>
    class MyTriggeredWrapper {
        // This class is a wrapper around an object of the specified 'TYPE'
        // that triggers a call to an object, called the "listener", of the
        // specified 'FUNC' invocable type whenever the wrapped object is
        // changed.

        // DATA
        TYPE d_value;
        FUNC d_listener;

      public:
        typedef bsl::allocator<> allocator_type;

        // CREATORS
        explicit
        MyTriggeredWrapper(const FUNC&           f,
                           const allocator_type& allocator = allocator_type());
        MyTriggeredWrapper(const TYPE&           v,
                           const FUNC&           f,
                           const allocator_type& allocator = allocator_type());
            // Create an object with the specified 'f' as the listener to be
            // called when a change is triggered.  Optionally specify 'v' as
            // the wrapped value; otherwise the wrapped value is default
            // constructed.  Optionally specify 'allocator' to supply
            // memory; otherwise the current default allocator is used.  If
            // 'TYPE' is not allocator aware, 'allocator' is ignored.

        MyTriggeredWrapper(const MyTriggeredWrapper&  original,
                           const allocator_type& allocator = allocator_type());
            // Create a copy of the specified 'original'.  Optionally specify
            // 'allocator' to supply memory; otherwise the current
            // default allocator is used.

        ~MyTriggeredWrapper()
            // Destroy the wrapped object and listener.
        {
        }

        // MANIPULATORS
        MyTriggeredWrapper& operator=(const TYPE& rhs);
        MyTriggeredWrapper& operator=(const MyTriggeredWrapper& rhs);
            // Assign to the wrapped value the value of the specified 'rhs',
            // invoke the listener with the new value, and return a reference
            // providing modifiable access to this object.  Note that the
            // listener itself is not assigned.

        void setValue(const TYPE& value);
            // Set the wrapped value to the specified 'value' and invoke the
            // listener with the new value.

        // ACCESSORS
        const TYPE& value() const
            // Return a reference providing read-only access to the wrapped
            // value.
        {
            return d_value;
        }

        const FUNC& listener() const
            // Return a reference providing read-only access to the listener.
        {
            return d_listener;
        }
    };
//..
// Next, we define the constructors such that they initialize 'd_value' using
// the specified allocator if and only if 'TYPE' accepts an allocator.  The
// 'bslma::ConstructionUtil::make' family of functions encapsulate all of the
// metaprogramming that detects whether 'TYPE' uses an allocator and, if so,
// which construction protocol it uses (allocator at the front or at the back
// of the argument list), making all three constructors straightforward:
//..
    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
                                              const FUNC&           f,
                                              const allocator_type& allocator)
    : d_value(bslma::ConstructionUtil::make<TYPE>(allocator))
    , d_listener(f)
    {
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
                                              const TYPE&           v,
                                              const FUNC&           f,
                                              const allocator_type& allocator)
    : d_value(bslma::ConstructionUtil::make<TYPE>(allocator, v))
    , d_listener(f)
    {
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>::MyTriggeredWrapper(
                                           const MyTriggeredWrapper& other,
                                           const allocator_type&     allocator)
    : d_value(bslma::ConstructionUtil::make<TYPE>(allocator, other.value()))
    , d_listener(other.d_listener)
    {
    }
//..
// Note that, for 'd_value' to be constructed with the correct allocator, the
// compiler must construct the result returned from 'make' directly into the
// 'd_value' variable, an optimization known prior to C++17 as "copy elision".
// This optimization is required by the C++17 standard and is optional in
// pre-2017 standards, but is implemented in all of the C++11 compilers for
// which this component is expected to be used at Bloomberg.
//
// Next, we implement the assignment operators, which simply call 'setValue':
//..
    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>&
    MyTriggeredWrapper<TYPE, FUNC>::operator=(const TYPE& rhs)
    {
        setValue(rhs);
        return *this;
    }

    template <class TYPE, class FUNC>
    MyTriggeredWrapper<TYPE, FUNC>&
    MyTriggeredWrapper<TYPE, FUNC>::operator=(const MyTriggeredWrapper& rhs)
    {
        setValue(rhs.value());
        return *this;
    }
//..
// Then, we implement 'setValue', which calls the listener after modifying the
// value:
//..
    template <class TYPE, class FUNC>
    void MyTriggeredWrapper<TYPE, FUNC>::setValue(const TYPE& value)
    {
        d_value = value;
        d_listener(d_value);
    }
//..
// Finally, we check our work by creating a listener for 'MyContainer<int>'
// that stores its last-seen value in a known location and a wrapper around
// 'MyContainer<int>' to test it:
//..
    int lastSeen = 0;
    void myListener(const MyContainer<int>& c)
    {
        lastSeen = c.front();
    }

    void usageExample3()
    {
        bslma::TestAllocator testAlloc;
        MyTriggeredWrapper<MyContainer<int>,
                           void (*)(const MyContainer<int>&)>
                             wrappedContainer(myListener, &testAlloc);
        ASSERT(&testAlloc == wrappedContainer.value().get_allocator());

        wrappedContainer = MyContainer<int>(99);

        ASSERT(99 == lastSeen);
    }
//..
#else  // if ! defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
    void usageExample3() { }
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

}  // close unnamed namespace
//@bdetdsplit FOR 8 END

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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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
        usageExample1();
        usageExample2();
        usageExample3();
      } break;
      case 7: {
        // 'destructiveMove'
        //   Test calls to 'destructiveMove' and verify that the first argument
        //   is move-constructed and the last argument is destroyed.  If the
        //   type being moved is bitwise-movable, no constructor or destructor
        //   is actually called.
        //
        // Concerns:
        //: 1 If the type being moved is not bitwise-movable, the
        //:   move-constructor is invoked to move the object then the
        //:   destructor is invoked to destroy the original.
        //:
        //: 2 If the type being moved is bitwise-movable, the original is
        //:   bitwise-copied to the destination; no constructors or destructors
        //:   are invoked.
        //:
        //: 3 If the type being moved is allocator-aware, the supplied
        //:   allocator is used for the target object.  If the supplied
        //:   allocator is different than the source-object's allocator, the
        //:   behavior is undefined, but no negative testing is possible at
        //:   this time.
        //:
        //: 4 For AA types that do not have a dedicated move constructor (i.e.,
        //:   the copy constructor acts as the move constructor), the target
        //:   object is in a copied-to state, rather than a moved-to state, but
        //:   the allocator is still the same as the source object because of
        //:   concern 3.
        //
        // Plan:
        //: 1 Using several instantiations of 'TestType', construct an object
        //:   in one buffer than use 'destructiveMove' to relocate it to
        //:   another buffer.  Verify that the target object is a in a
        //:   move-constructed state, that the constructor was invoked exactly
        //:   once, that the most-recently-destroyed object matches the source
        //:   object, and that the total number of objects did not change as a
        //:   result of the 'destructiveMove'.  (C-1)
        //:
        //: 2 Using a bitwise-movable variation of 'TestType', construct an
        //:   object in one buffer than use 'destructiveMove' to relocate it to
        //:   another buffer.  Verify that the target object is *not* in a
        //:   moved-from state (because the move constructor was not invoked),
        //:   that no constructors were invoked, that the
        //:   most-recently-destroyed object did not change (because no
        //:   destructors were invoked) and that the total number of objects
        //:   did not change as a result of the 'destructiveMove'.  (C-2)
        //:
        //: 3 In steps 1 and 2, if the type under test is AA, supply the same
        //:   allocator to the 'destructiveMove' call as was supplied to the
        //:   source object's constructor.  Verify that, after calling
        //:   'destructiveMove', the target object uses the supplied allocator.
        //:   (C-3)
        //:
        //: 4 Using an AA type having a copy constructor but no separate move
        //:   constructor, verify that 'destructiveMove' leaves the target
        //:   object in a copied-to state having the supplied allocator.
        //:   (C-4)
        //
        // Testing:
        //     void destructiveMove(TYPE *target, const ALLOCATOR&, TYPE *src)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'destructiveMove'"
                            "\n=================\n");

        bslma::TestAllocator ta;
        BslAllocator<>       bslAlloc(&ta);
        const int            nonAlloc = -1;

        ArgumentType<1> arg1(VALUES[1]);
        ArgumentType<2> arg2(VALUES[2]);

        // Test with simple 'int' values
        {
            int obj1 = 99;
            int obj2 = 0;
            Util::destructiveMove(&obj2, nonAlloc, &obj1);
            ASSERTV(obj2, 99 == obj2);
        }

        // Test with non-AA 'TestType'
        {
            typedef TestType<void, false>  TT;

            bsls::ObjectBuffer<TT> b1, b2;
            TT& source = b1.object();
            TT& target = b2.object();
            (void) TT();  // Force constructor/destructor invocation
            ASSERT(&source != TestTypeStats::lastDestroyed());

            ::new (&source) TT(arg1, arg2);
            int initObjCount   = TestTypeStats::currentCount();
            int initTotalCount = TestTypeStats::totalCount();
            ASSERT(VALUES[1] == source[1]);
            ASSERT(VALUES[2] == source[2]);

            Util::destructiveMove(&target, nonAlloc, &source);

            ASSERT(initObjCount       == TestTypeStats::currentCount());
            ASSERT(initTotalCount + 1 == TestTypeStats::totalCount());
            ASSERT(VALUES[1] == target[1]);
            ASSERT(VALUES[2] == target[2]);
            ASSERT(target.isMovedInto());
            ASSERT(&source == TestTypeStats::lastDestroyed());
            target.~TT();
        }

        // Test with AA 'TestType'
        {
            typedef TestType<BslAllocator<>, true>  TT;

            bsls::ObjectBuffer<TT> b1, b2;
            TT& source = b1.object();
            TT& target = b2.object();
            (void) TT();  // Force constructor/destructor invocation
            ASSERT(&source != TestTypeStats::lastDestroyed());

            ::new (&source) TT(bsl::allocator_arg, bslAlloc, arg1, arg2);
            int initObjCount   = TestTypeStats::currentCount();
            int initTotalCount = TestTypeStats::totalCount();
            ASSERT(bslAlloc  == source.get_allocator());
            ASSERT(VALUES[1] == source[1]);
            ASSERT(VALUES[2] == source[2]);

            Util::destructiveMove(&target, bslAlloc, &source);

            ASSERT(initObjCount       == TestTypeStats::currentCount());
            ASSERT(initTotalCount + 1 == TestTypeStats::totalCount());
            ASSERT(bslAlloc  == target.get_allocator());
            ASSERT(VALUES[1] == target[1]);
            ASSERT(VALUES[2] == target[2]);
            ASSERT(target.isMovedInto());
            ASSERT(&source == TestTypeStats::lastDestroyed());
            target.~TT();
        }

        // Test with AA 'BitwiseMovableTestType'
        {
            typedef BitwiseMovableTestType TT;

            bsls::ObjectBuffer<TT> b1, b2;
            TT& source = b1.object();
            TT& target = b2.object();
            (void) TT();  // Force constructor/destructor invocation
            void *initLastDestroyed = TestTypeStats::lastDestroyed();
            ASSERT(&source != initLastDestroyed);

            ::new (&source) TT(arg1, arg2, &ta);
            int initObjCount   = TestTypeStats::currentCount();
            int initTotalCount = TestTypeStats::totalCount();
            ASSERT(&ta       == source.allocator());
            ASSERT(VALUES[1] == source[1]);
            ASSERT(VALUES[2] == source[2]);

            Util::destructiveMove(&target, bslAlloc, &source);

            ASSERT(initObjCount   == TestTypeStats::currentCount());
            ASSERT(initTotalCount == TestTypeStats::totalCount());
            ASSERT(&ta       == target.allocator());
            ASSERT(VALUES[1] == target[1]);
            ASSERT(VALUES[2] == target[2]);
            ASSERT(! target.isMovedInto());
            ASSERT(initLastDestroyed == TestTypeStats::lastDestroyed());
            target.~TT();
        }

        // Test with AA 'TestType' for which move degenerates to copy
        {
            typedef CopyOnlyTestType TT;

            bsls::ObjectBuffer<TT> b1, b2;
            TT& source = b1.object();
            TT& target = b2.object();
            (void) TT(0);  // Force constructor/destructor invocation
            ASSERT(&source != TestTypeStats::lastDestroyed());

            ::new (&source) TT(VALUES[1], &ta);
            int initObjCount   = TestTypeStats::currentCount();
            int initTotalCount = TestTypeStats::totalCount();
            ASSERT(source.matchAllocator(&ta));
            ASSERT(source[1] == VALUES[1]);

            Util::destructiveMove(&target, &ta, &source);

            ASSERT(initObjCount       == TestTypeStats::currentCount());
            ASSERT(initTotalCount + 1 == TestTypeStats::totalCount());
            ASSERT(target.matchAllocator(&ta));
            ASSERT(target[1] == VALUES[1]);
            ASSERT(target.isCopiedInto());
            ASSERT(&source == TestTypeStats::lastDestroyed());
            target.~TT();
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MULTI-ARGUMENT 'make'
        //   Test calls to 'bslma::ConstructorUtil::make<TYPE>' having two or
        //   more arguments where the first argument is the allocator (or
        //   a non-allocator), and the remaining arguments are additional
        //   arguments to the 'TYPE' constructor.  Verify that the call returns
        //   a correctly constructed 'TYPE' object.
        //
        // Concerns:
        //: 1 'bslma::ConstructorUtil::make<TYPE>' can be invoked with an
        //:   allocator (or a non-allocator scalar), and 1 to 14 additional
        //:   'TYPE' constructor arguments, regardless of which AA model 'TYPE'
        //:   conforms to and regardless of whether it uses the leading or
        //:   trailing allocator-passing convention.  The call returns an
        //:   object of 'TYPE'.
        //:
        //: 2 If 'TYPE' is allocator-aware (AA) and the allocator type is
        //:   compatible with 'TYPE', then the allocator argument is passed to
        //:   'TYPE''s extended constructor; otherwise, 'TYPE''s non-extended
        //:   constructor is invoked.
        //:
        //: 3 The argument values are forwarded to 'TYPE''s constructor.
        //:
        //: 4 The argument value categories (lvalue vs rvalue) are preserved
        //:   when forwarded to 'TYPE''s constructor.  Note that, in C++03,
        //:   only the first argument is perfectly forwarded; for the remaining
        //:   arguments, a modifiable lvalue is forwarded as a 'const' lvalue
        //:   reference.
        //:
        //: 5 Initializing a 'TYPE' object from a call to 'make' --
        //:   specifically by direct initialization, copy initialization,
        //:   base-class initialization, member initialization, and
        //:   initialization of a pass-by-value argument -- invokes the 'TYPE'
        //:   constructor only once and does not invoke its move or copy
        //:   constructor.  Note that the 'make' method is available only on
        //:   platforms that reliably perform such copy elision.
        //
        // Plan:
        //: 1 Let 'TYPE' an element in a list of 'TestType' instantiations,
        //:   'TYPE<A, C>', where 'A' is 'void', 'bslma::Allocator *',
        //:   'BslAllocator', 'PmrAllocator', and 'StlAllocator', and where 'C'
        //:   is 'true' and 'false'.  For each such 'TYPE', and an allocator
        //:   'q' of type 'A2':
        //:
        //:   1 Create a struct (pack) of 14 'bsltf::ArgumentType<N>' objects
        //:     to use as arguments to the constructor.  The initial value of
        //:     these objects should come from a known list of semi-random
        //:     values.  Create a 'const' reference to the pack.
        //:
        //:   2 For *N* in 1 to 14, call 'make<TYPE>', passing each compatible
        //:     allocator type and *N* arguments from the pack described step
        //:     1.  Capture the returned object in a local variable using
        //:     direct initialization.  (C-1)
        //:
        //:   3 Verify that, when 'TYPE' is AA, the allocator used by the
        //:     returned object matches the allocator passed to 'make';
        //:     otherwise the allocator matches the default allocator.  (C-2)
        //:
        //:   4 Verify that the value of the arguments captured in the return
        //:     value match the values of the arguments passed to the
        //:     constructor.  (C-3)
        //:
        //:   5 Perform step 1.2 through 1.4 three times, passing the arguments
        //:     by 'const' reference, non-const lvalue reference, and rvalue
        //:     (movable) reference, respectively.  Verify the expected
        //:     copy/move states for the captured values in the returned object
        //:     and the original arguments passed to 'construct'. Note that, in
        //:     C++03, the expected copy/move state for the second and
        //:     subsequent captured values is 'e_CONST_COPIED_INTO' for both
        //:     'const' and non-'const' lvalue arguments.  (C-4)
        //:
        //: 2 Initialize objects of a specific 'TestType' instantiation via
        //:   direct initialization, copy initialization, base-class
        //:   initialization, member initialization, and initialization of a
        //:   pass-by-value argument from a call to 'make<TestType<...>>'.
        //:   Verify that there was only one call to the 'TestType' constructor
        //:   and that the initialized object is not in a copied-into or
        //:   moved-into state.  (C-5)
        //
        // Testing:
        //    TYPE make<TYPE>(const ALLOCATOR&, ARGS&&...);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMULTI-ARGUMENT 'make'"
                            "\n=====================\n");
#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  // Copying from a non-const lvalue is always detected correctly in C++11
# define EXP_COPIED_MUTABLE(TEST_OBJ, N) (TEST_OBJ)[(N)].isMutableCopiedInto()
#else
  // Copying from a non-const is detected correctly in C++03 only if 'N == 1'
# define EXP_COPIED_MUTABLE(TEST_OBJ, N)                      \
        (N == 1 ? (TEST_OBJ)[(N)].isMutableCopiedInto() :     \
                  (TEST_OBJ)[(N)].isConstCopiedInto())
#endif

#define MAKE_WITH_ALLOC(R) {                                                  \
        const int N = R;                                                      \
        const int objCount   = TestTypeStats::currentCount();                 \
        const int totalCount = TestTypeStats::totalCount();                   \
        ArgPack mAP; const ArgPack& AP = mAP;                                 \
        const ValueType X(Util::make<ValueType>(alloc,                        \
                                    BSLS_MACROREPEAT_COMMA(R, CONST_ARG)));   \
        const ValueType Y(Util::make<ValueType>(alloc,                        \
                                    BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG))); \
        const ValueType Z(Util::make<ValueType>(alloc,                        \
                                    BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG)));  \
        ASSERT(objCount + 3   == TestTypeStats::currentCount());              \
        ASSERT(totalCount + 3 == TestTypeStats::totalCount());                \
        ASSERTV(N, X.matchAllocator(expAlloc));                               \
        ASSERTV(N, Y.matchAllocator(expAlloc));                               \
        ASSERTV(N, Z.matchAllocator(expAlloc));                               \
        for (int i = 1; i <= R; ++i) {                                        \
            ASSERTV(N, i, VALUES[i], X[i], VALUES[i] == X[i]);                \
            ASSERTV(N, i, VALUES[i], Y[i], VALUES[i] == Y[i]);                \
            ASSERTV(N, i, VALUES[i], Z[i], VALUES[i] == Z[i]);                \
            ASSERTV(N, i, X[i].isConstCopiedInto());                          \
            ASSERTV(N, i, EXP_COPIED_MUTABLE(Y,i));                           \
            ASSERTV(N, i, Z[i].isMovedInto());                                \
        }                                                                     \
        BSLS_MACROREPEAT_SEP(R, ASSERT_ARG_MOVED_FROM, ; );                   \
      }

#define TEST_IMP(ALLOC_TYPE, USE_PREFIX_ARG, ALLOC_ARG_T, EXP_PROPAGATE) do { \
        ALLOC_ARG_T alloc(&ta);                                               \
        typedef TestType<ALLOC_TYPE, USE_PREFIX_ARG> ValueType;               \
        bslma::Allocator *expAlloc = EXP_PROPAGATE ? &ta : &da;             \
        BSLS_MACROREPEAT(14, MAKE_WITH_ALLOC)                                 \
      } while (false)

#define TEST(ALLOC_TYPE, ALLOC_ARG_T, EXP_PROPAGATE) do {              \
        TEST_IMP(ALLOC_TYPE, false, ALLOC_ARG_T, EXP_PROPAGATE);       \
        TEST_IMP(ALLOC_TYPE, true , ALLOC_ARG_T, EXP_PROPAGATE);       \
      } while (false)

        const int initObjCount = TestTypeStats::currentCount();

        //                                                Expect
        //   Allocator parameter   Allocator argument   propagation
        //   ===================   ===================  ===========
//@bdetdsplit CODE SLICING BEGIN
        TEST(void               ,  NonAllocator       , false      );
        TEST(void               ,  PmrAllocator<short>, true       );
        TEST(void               ,  BslAllocator<int>  , true       );
        TEST(void               ,  LegacyAllocator    , true       );
        TEST(void               ,  STLAllocator<short>, true       );

//@bdetdsplit CODE SLICING BREAK
        TEST(PmrAllocator<int>  ,  NonAllocator       , false      );
        TEST(PmrAllocator<int>  ,  PmrAllocator<short>, true       );
        TEST(PmrAllocator<int>  ,  BslAllocator<int>  , true       );
        TEST(PmrAllocator<int>  ,  LegacyAllocator    , true       );
        TEST(BslAllocator<short>,  NonAllocator       , false      );
//@bdetdsplit CODE SLICING BREAK
        TEST(BslAllocator<short>,  BslAllocator<int>  , true       );
        TEST(BslAllocator<short>,  LegacyAllocator    , true       );
        TEST(LegacyAllocator    ,  NonAllocator       , false      );
        TEST(LegacyAllocator    ,  BslAllocator<int>  , true       );
        TEST(LegacyAllocator    ,  LegacyAllocator    , true       );
//@bdetdsplit CODE SLICING BREAK
        TEST(STLAllocator<char> ,  NonAllocator       , false      );
        TEST(STLAllocator<char> ,  PmrAllocator<short>, false      );
        TEST(STLAllocator<char> ,  BslAllocator<int>  , false      );
        TEST(STLAllocator<char> ,  LegacyAllocator    , false      );
        TEST(STLAllocator<char> ,  STLAllocator<short>, true       );

        ///// Negative tests.  These should fail to compile.
//      TEST(PmrAllocator<int>  ,  STLAllocator<>     , false      );
//      TEST(BslAllocator<short>,  PmrAllocator<int>  , false      );
//      TEST(BslAllocator<short>,  STLAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  PmrAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  STLAllocator<int>  , false      );

        // Test that copy elision happens with direct initialization
        {
            typedef TestType<BslAllocator<>, false> TT;

            const int             totalCount = TestTypeStats::totalCount();
            const ArgumentType<1> arg(VALUES[1]);
            TT                    obj(Util::make<TT>(&ta, arg));
            ASSERT(totalCount + 1 == TestTypeStats::totalCount());
            ASSERT(&ta == obj.get_allocator());
            ASSERT(arg == obj[1]);
            ASSERT(obj.isNotCopiedOrMoved());
        }

        // Test that copy elision happens with copy initialization
        {
            typedef TestType<BslAllocator<>, false> TT;

            const int             totalCount = TestTypeStats::totalCount();
            const ArgumentType<1> arg(VALUES[1]);
            TT                    obj = Util::make<TT>(&ta, arg);
            ASSERT(totalCount + 1 == TestTypeStats::totalCount());
            ASSERT(&ta == obj.get_allocator());
            ASSERT(arg == obj[1]);
            ASSERT(obj.isNotCopiedOrMoved());
        }

        // Test that copy elision happens with base-class initialization
#define SUPPORT_BASE_CLASS_COPY_ELISION 0
#if SUPPORT_BASE_CLASS_COPY_ELISION
        // It does not appear that mandatory base-class copy elision is
        // a thing or, if it is, it is not implemented consistently even by
        // C++ 17 compilers.
        {
            typedef Derived<TestType<bslma::Allocator *, false> >
                                                               DerivedTestType;

            const int       totalCount = TestTypeStats::totalCount();
            DerivedTestType obj(ArgType1(VALUES[1]), &ta);
            ASSERTV(totalCount, TestTypeStats::totalCount(),
                    totalCount + 1 == TestTypeStats::totalCount());
            ASSERT(obj.matchAllocator(&ta));
            ASSERT(VALUES[1] == obj[1]);
            ASSERT(obj.isNotCopiedOrMoved());
        }
#endif // SUPPORT_BASE_CLASS_COPY_ELISION

        // Test that copy elision happens with member initialization
        {
            typedef Wrapper<TestType<bslma::Allocator *, false> >
                                                               WrapperTestType;

            const int       totalCount = TestTypeStats::totalCount();
            WrapperTestType obj(VALUES[1], &ta);
            ASSERTV(totalCount, TestTypeStats::totalCount(),
                    totalCount + 1 == TestTypeStats::totalCount());
            ASSERT(obj.matchAllocator(&ta));
            ASSERT(VALUES[1] == obj[1]);
            ASSERT(obj.isNotCopiedOrMoved());
        }
//@bdetdsplit CODE SLICING END

        ASSERT(initObjCount == TestTypeStats::currentCount());
#undef CHECK_RESULTS
#undef MAKE_WITH_ALLOC
#undef TEST
#undef TEST_IMP
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ALLOCATOR-ONLY 'make'
        //   Test calls to 'bslma::ConstructorUtil::make<TYPE>' having a single
        //   argument, which is the allocator (or a non-allocator).  Verify
        //   that the call returns a default-constructed 'TYPE' object (non-AA
        //   types) or extended default-constructed 'TYPE' object (AA types).
        //
        // Concerns:
        //: 1 If 'TYPE' is a non-AA type, 'make' can be called by passing,
        //:   as the allocator (only) argument, any allocator or a
        //:   non-allocator scalar; a default-constructed 'TYPE' object is
        //:   returned.
        //:
        //: 2 If 'bslma::UsesBslmaAllocator<TYPE>::value' is 'true',
        //:   'make' can be called by passing, as the allocator argument,
        //:   a non-allocator, 'bslma::Allocator' pointer, or an allocator
        //:   object having a 'mechanism' accessor that returns a
        //:   'bslma::Allocator *'; an extended-default-constructed 'TYPE'
        //:   object using the specified allocator is returned.
        //:
        //: 3 If 'TYPE::allocator_type' exists and is a class type, 'make'
        //:   can be called by passing, as the allocator argument, any
        //:   allocator that is convertible to 'TYPE::allocator_type'; an
        //:   extended-default-constructed 'TYPE' object using the specified
        //:   allocator is returned.
        //:
        //: 4 If a non-allocator is passed to 'make' a default-constructed
        //:   'TYPE' object is returned.
        //:
        //: 5 If 'TYPE' is an AA type and a compatible allocator is passed to
        //:   'make', then the allocator is passed to the extended default
        //:   constructor for 'TYPE', regardless of whether 'TYPE' uses the
        //:   leading or trailing allocator-passing convention.
        //
        // Plan:
        //: 1 Let 'TYPE' an element in a list of 'TestType' instantiations,
        //:   'TYPE<A, C>', where 'A' is 'void', 'bslma::Allocator *',
        //:   'BslAllocator', 'PmrAllocator', and 'StlAllocator', and where 'C'
        //:   is 'true' and 'false'.  For each such 'TYPE', and an allocator
        //:   'q' of type 'A2', verify that:
        //:
        //:   1 If 'A' is 'none' (i.e., 'TestType<A, C>' is not AA),
        //:     the 'make<TYPE>(q)' call is valid for all allocator types,
        //:     'A2' and invokes the default constructor of 'TYPE'.  (C-1)
        //:
        //:   2 If 'A' is 'bslma::Allocator *', the 'make<TYPE>(q)' call is
        //:     valid if 'A2' is a non-allocator, 'bslma::Allocator *', or
        //:     'BslAllocator'.  (C-2)
        //:
        //:   3 If 'A' is one of the other allocator types, the 'make<TYPE>(q)'
        //:     call is valid if 'A2' is a non-allocator or is convertible to
        //:     'A'.  (C-3)
        //:
        //:   4 If 'A2' is a non-allocator, then the default constructor for
        //:     'TYPE' is invoked.  (C-4)
        //:
        //:   5 If 'A2' is an allocator type, then the extended default
        //:     constructor for 'TYPE' is invoked.  (C-5)
        //:
        //:   6 Using the instrumentation provided by 'TestType', verify that
        //:     exactly one object is constructed by the 'construct' method.
        //
        // Testing:
        //    TYPE make<TYPE>(const ALLOCATOR&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nALLOCATOR-ONLY 'make'"
                            "\n=====================\n");

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#define TEST_IMP(ALLOC_TYPE, USE_PREFIX_ARG, ALLOC_ARG_T, EXP_PROPAGATE) do { \
        const int objCount   = TestTypeStats::currentCount();                 \
        const int totalCount = TestTypeStats::totalCount();                   \
        ALLOC_ARG_T allocator(&ta);                                           \
        typedef TestType<ALLOC_TYPE, USE_PREFIX_ARG> ValueType;               \
        ValueType mX(Util::make<ValueType>(allocator));                       \
        const ValueType& X = mX;                                              \
        ASSERT(objCount + 1   == TestTypeStats::currentCount());              \
        ASSERT(totalCount + 1 == TestTypeStats::totalCount());                \
        ASSERT(-1 == X[1]);                                                   \
        bslma::Allocator *expAlloc = EXP_PROPAGATE ? &ta : &da;             \
        ASSERT(X.matchAllocator(expAlloc));                                   \
      } while (false)

#define TEST(ALLOC_TYPE, ALLOC_ARG_T, EXP_PROPAGATE) do {              \
        TEST_IMP(ALLOC_TYPE, false, ALLOC_ARG_T, EXP_PROPAGATE);       \
        TEST_IMP(ALLOC_TYPE, true , ALLOC_ARG_T, EXP_PROPAGATE);       \
      } while (false)

        const int initObjCount = TestTypeStats::currentCount();

        //                                                Expect
        //   Allocator parameter   Allocator argument   propagation
        //   ===================   ===================  ===========
        TEST(void               ,  NonAllocator       , false      );
        TEST(void               ,  PmrAllocator<short>, true       );
        TEST(void               ,  BslAllocator<int>  , true       );
        TEST(void               ,  LegacyAllocator    , true       );
        TEST(void               ,  STLAllocator<short>, true       );

        TEST(PmrAllocator<int>  ,  NonAllocator       , false      );
        TEST(PmrAllocator<int>  ,  PmrAllocator<short>, true       );
        TEST(PmrAllocator<int>  ,  BslAllocator<int>  , true       );
        TEST(PmrAllocator<int>  ,  LegacyAllocator    , true       );
        TEST(BslAllocator<short>,  NonAllocator       , false      );
        TEST(BslAllocator<short>,  BslAllocator<int>  , true       );
        TEST(BslAllocator<short>,  LegacyAllocator    , true       );
        TEST(LegacyAllocator    ,  NonAllocator       , false      );
        TEST(LegacyAllocator    ,  BslAllocator<int>  , true       );
        TEST(LegacyAllocator    ,  LegacyAllocator    , true       );
        TEST(STLAllocator<char> ,  NonAllocator       , false      );
        TEST(STLAllocator<char> ,  PmrAllocator<short>, false      );
        TEST(STLAllocator<char> ,  BslAllocator<int>  , false      );
        TEST(STLAllocator<char> ,  LegacyAllocator    , false      );
        TEST(STLAllocator<char> ,  STLAllocator<short>, true       );

        ///// Negative tests.  These should fail to compile.
//      TEST(PmrAllocator<int>  ,  STLAllocator<>     , false      );
//      TEST(BslAllocator<short>,  PmrAllocator<int>  , false      );
//      TEST(BslAllocator<short>,  STLAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  PmrAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  STLAllocator<int>  , false      );

        ASSERT(initObjCount == TestTypeStats::currentCount());

#undef TEST
#undef TEST_IMP

#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTI-ARGUMENT 'construct'
        //   Test calls to 'bslma::ConstructorUtil::construct' having three or
        //   more arguments where the first argument is the address of the
        //   object being constructed, the second argument is the allocator (or
        //   a non-allocator), and the remaining arguments are additional
        //   arguments to the type of object being constructed.
        //
        // Concerns:
        //: 1 'bslma::ConstructorUtil::construct' can be invoked with the
        //:   address of an object of type 'TYPE' to be constructed, an
        //:   allocator (or a non-allocator scalar), and 1 to 14 additional
        //:   'TYPE' constructor arguments, regardless of which AA model 'TYPE'
        //:   conforms to and regardless of whether it uses the leading or
        //:   trailing allocator-passing convention.
        //:
        //: 2 If 'TYPE' is allocator-aware (AA) and the allocator type is
        //:   compatible with 'TYPE', then the allocator argument is passed to
        //:   'TYPE''s extended constructor; otherwise, 'TYPE''s non-extended
        //:   constructor is invoked.
        //:
        //: 3 The argument values are forwarded to 'TYPE''s constructor.
        //:
        //: 4 The argument value categories (lvalue vs rvalue) are preserved
        //:   when forwarded to 'TYPE''s constructor.  Note that, in C++03,
        //:   only the first argument is perfectly forwarded; for the remaining
        //:   arguments, a modifiable lvalue is forwarded as a 'const' lvalue
        //:   reference.
        //
        // Plan:
        //: 1 Let 'TYPE' an element in a list of 'TestType' instantiations,
        //:   'TYPE<A, C>', where 'A' is 'void', 'bslma::Allocator *',
        //:   'BslAllocator', 'PmrAllocator', and 'StlAllocator', and where 'C'
        //:   is 'true' and 'false'.  For a pointer 'p' to each such 'TYPE',
        //:   and an allocator 'q' of type 'A2':
        //:
        //:   1 Create a struct (pack) of 14 'bsltf::ArgumentType<N>' objects
        //:     to use as arguments to the constructor.  The initial value of
        //:     these objects should come from a known list of semi-random
        //:     values.  Create a 'const' reference to the pack.
        //:
        //:   2 For *N* in 1 to 14, use 'construct' to initialize '*p', passing
        //:     each compatible allocator type and *N* arguments from the pack
        //:     described step 1.  (C-1)
        //:
        //:   3 Verify that, when 'TYPE' is AA, the allocator used by '*p'
        //:     matches the allocator passed to 'construct'; otherwise the
        //:     allocator used by '*p' matches the default allocator.  (C-2)
        //:
        //:   4 Verify that the value of the arguments captured in '*p' match
        //:     the values of the arguments passed to the constructor.  (C-3)
        //:
        //:   5 Perform step 1.2 through 1.4 three times, passing the arguments
        //:     by 'const' reference, non-const lvalue reference, and rvalue
        //:     (movable) reference, respectively.  Verify the expected
        //:     copy/move states for the captured values in '*p' and the
        //:     original arguments passed to 'construct'. Note that, in C++03,
        //:     the expected copy/move state for the second and subsequent
        //:     captured values is 'e_CONST_COPIED_INTO' for both 'const' and
        //:     non-'const' lvalue arguments.  (C-4)
        //
        // Testing:
        //    void construct(TYPE *, const ALLOCATOR&, ARGS&&...);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMULTI-ARGUMENT 'construct'"
                            "\n==========================\n");

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  // Copying from a non-const lvalue is always detected correctly in C++11
# define EXP_COPIED_MUTABLE(TEST_OBJ, N) (TEST_OBJ)[(N)].isMutableCopiedInto()
#else
  // Copying from a non-const is detected correctly in C++03 only if 'N == 1'
# define EXP_COPIED_MUTABLE(TEST_OBJ, N)                      \
        (N == 1 ? (TEST_OBJ)[(N)].isMutableCopiedInto() :     \
                  (TEST_OBJ)[(N)].isConstCopiedInto())
#endif

#define CONSTRUCT_WITH_ALLOC(R) {                                             \
        const int N = R;                                                      \
        const int objCount   = TestTypeStats::currentCount();                 \
        const int totalCount = TestTypeStats::totalCount();                   \
        ArgPack mAP; const ArgPack& AP = mAP;                                 \
        bsls::ObjectBuffer<ValueType> bX, bY, bZ;                             \
        ValueType& mX = bX.object(), &mY = bY.object(), &mZ = bZ.object();    \
        const ValueType& X = mX, &Y = mY, &Z = mZ;                            \
        Util::construct(&mX, alloc, BSLS_MACROREPEAT_COMMA(R, CONST_ARG));    \
        Util::construct(&mY, alloc, BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG));  \
        Util::construct(&mZ, alloc, BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG));   \
        ASSERT(objCount + 3   == TestTypeStats::currentCount());              \
        ASSERT(totalCount + 3 == TestTypeStats::totalCount());                \
        ASSERTV(N, X.matchAllocator(expAlloc));                               \
        ASSERTV(N, Y.matchAllocator(expAlloc));                               \
        ASSERTV(N, Z.matchAllocator(expAlloc));                               \
        for (int i = 1; i <= R; ++i) {                                        \
            ASSERTV(N, i, VALUES[i], X[i], VALUES[i] == X[i]);                \
            ASSERTV(N, i, VALUES[i], Y[i], VALUES[i] == Y[i]);                \
            ASSERTV(N, i, VALUES[i], Z[i], VALUES[i] == Z[i]);                \
            ASSERTV(N, i, X[i].isConstCopiedInto());                          \
            ASSERTV(N, i, EXP_COPIED_MUTABLE(Y,i));                           \
            ASSERTV(N, i, Z[i].isMovedInto());                                \
        }                                                                     \
        BSLS_MACROREPEAT_SEP(R, ASSERT_ARG_MOVED_FROM, ; );                   \
        mX.~ValueType();                                                      \
        mY.~ValueType();                                                      \
        mZ.~ValueType();                                                      \
      }

#define TEST_IMP(ALLOC_TYPE, USE_PREFIX_ARG, ALLOC_ARG_T, EXP_PROPAGATE) do { \
        ALLOC_ARG_T alloc(&ta);                                               \
        typedef TestType<ALLOC_TYPE, USE_PREFIX_ARG> ValueType;               \
        bslma::Allocator *expAlloc = EXP_PROPAGATE ? &ta : &da;               \
        BSLS_MACROREPEAT(14, CONSTRUCT_WITH_ALLOC)                            \
      } while (false)

#define TEST(ALLOC_TYPE, ALLOC_ARG_T, EXP_PROPAGATE) do {                     \
        TEST_IMP(ALLOC_TYPE, false, ALLOC_ARG_T, EXP_PROPAGATE);              \
        TEST_IMP(ALLOC_TYPE, true , ALLOC_ARG_T, EXP_PROPAGATE);              \
      } while (false)

        const int initObjCount = TestTypeStats::currentCount();

        //                                                Expect
        //   Allocator parameter   Allocator argument   propagation
        //   ===================   ===================  ===========
//@bdetdsplit CODE SLICING BEGIN
        TEST(void               ,  NonAllocator       , false      );
        TEST(void               ,  PmrAllocator<short>, true       );
        TEST(void               ,  BslAllocator<int>  , true       );
        TEST(void               ,  LegacyAllocator    , true       );
        TEST(void               ,  STLAllocator<short>, true       );

//@bdetdsplit CODE SLICING BREAK
        TEST(PmrAllocator<int>  ,  NonAllocator       , false      );
        TEST(PmrAllocator<int>  ,  PmrAllocator<short>, true       );
        TEST(PmrAllocator<int>  ,  BslAllocator<int>  , true       );
        TEST(PmrAllocator<int>  ,  LegacyAllocator    , true       );
//@bdetdsplit CODE SLICING BREAK
        TEST(BslAllocator<short>,  BslAllocator<int>  , true       );
        TEST(BslAllocator<short>,  LegacyAllocator    , true       );
        TEST(LegacyAllocator    ,  NonAllocator       , false      );
        TEST(LegacyAllocator    ,  BslAllocator<int>  , true       );
//@bdetdsplit CODE SLICING BREAK
        TEST(STLAllocator<char> ,  NonAllocator       , false      );
        TEST(STLAllocator<char> ,  PmrAllocator<short>, false      );
        TEST(STLAllocator<char> ,  BslAllocator<int>  , false      );
        TEST(STLAllocator<char> ,  LegacyAllocator    , false      );
        TEST(STLAllocator<char> ,  STLAllocator<short>, true       );
//@bdetdsplit CODE SLICING END

        ///// Negative tests.  These should fail to compile.
//      TEST(PmrAllocator<int>  ,  STLAllocator<>     , false      );
//      TEST(BslAllocator<short>,  PmrAllocator<int>  , false      );
//      TEST(BslAllocator<short>,  STLAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  PmrAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  STLAllocator<int>  , false      );

        ASSERT(initObjCount == TestTypeStats::currentCount());

#undef CHECK_RESULTS
#undef CONSTRUCT_WITH_ALLOC
#undef TEST
#undef TEST_IMP
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATOR-ONLY 'construct'
        //   Test two-argument calls to 'bslma::ConstructorUtil::construct',
        //   where the first argument is the address of the object being
        //   constructed and the second argument is the allocator (or a
        //   non-allocator). Verify that the call invokes the object's default
        //   constructor (non-AA types) or extended default constructor (AA
        //   types).
        //
        // Concerns:
        //: 1 If 'TYPE' is a non-AA type, 'construct' can be called by passing,
        //:   as the allocator (second) argument, any allocator or a
        //:   non-allocator scalar; the default constructor for 'TYPE' is
        //:   invoked.
        //:
        //: 2 If 'bslma::UsesBslmaAllocator<TYPE>::value' is 'true',
        //:   'construct' can be called by passing, as the allocator argument,
        //:   a non-allocator, 'bslma::Allocator' pointer, or an allocator
        //:   object having a 'mechanism' accessor that returns a
        //:   'bslma::Allocator *'.
        //:
        //: 3 If 'TYPE::allocator_type' exists and is a class type, 'construct'
        //:   can be called by passing, as the allocator argument, any
        //:   allocator that is convertible to 'TYPE::allocator_type'.
        //:
        //: 4 If a non-allocator is passed to 'construct', the default
        //:   constructor for 'TYPE' is invoked.
        //:
        //: 5 If 'TYPE' is an AA type and a compatible allocator is passed to
        //:   'construct', then the allocator is passed to the extended default
        //:   constructor for 'TYPE', regardless of whether 'TYPE' uses the
        //:   leading or trailing allocator-passing convention.
        //
        // Plan:
        //: 1 Create a 'TestType<A, C>' class template where 'A' is the desired
        //:   allocator type (or 'void' for none) and 'C' is the desired
        //:   allocator-passing convention ('false' for trailing allocator,
        //:   'true' for leading allocator).
        //:
        //: 2 Create a set of STL-compatible allocator templates,
        //:   'BslAllocator', 'PmrAllocator', and 'STLAllocator'.  Note that
        //:   'BslAllocator' and 'PmrAllocator' mimic 'bsl::allocator' and
        //:   'bsl::polymorphic_allocator' but avoid creating a cyclic
        //:   dependency on 'bslma_bslallocator' or
        //:   'bslma_polymorphicallocator'.
        //:
        //: 3 Let 'TYPE' an element in a list of 'TestType' instantiations,
        //:   'TYPE<A, C>', where 'A' is 'void', 'bslma::Allocator *',
        //:   'BslAllocator', 'PmrAllocator', and 'StlAllocator', and where 'C'
        //:   is 'true' and 'false'.  For a pointer 'p' to each such 'TYPE',
        //:   and an allocator 'q' of type 'A2', verify that:
        //:
        //:   1 If 'A' is 'none' (i.e., 'TestType<A, C>' is not AA),
        //:     the 'construct(p, q)' call is valid for all allocator types,
        //:     'A2' and invokes the default constructor of 'TYPE'.  (C-1)
        //:
        //:   2 If 'A' is 'bslma::Allocator *', the 'construct(p, q)' call is
        //:     valid if 'A2' is a non-allocator, 'bslma::Allocator *', or
        //:     'BslAllocator'.  (C-2)
        //:
        //:   3 If 'A' is one of the other allocator types, then 'construct(p,
        //:     q)' call is valid if 'A2' is a non-allocator or is convertible
        //:     to 'A'.  (C-3)
        //:
        //:   4 If 'A2' is a non-allocator, then the default constructor for
        //:     'TYPE' is invoked.  (C-4)
        //:
        //:   5 If 'A2' is an allocator type, then the extended default
        //:     constructor for 'TYPE' is invoked.  (C-5)
        //:
        //:   6 Using the instrumentation provided by 'TestType', verify that
        //:     exactly one object is constructed by the 'construct' method.
        //
        // Testing:
        //    void construct(TYPE *, const ALLOCATOR&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nALLOCATOR-ONLY 'construct'"
                            "\n==========================\n");

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

#define TEST_IMP(ALLOC_TYPE, USE_PREFIX_ARG, ALLOC_ARG_T, EXP_PROPAGATE) do { \
        const int objCount   = TestTypeStats::currentCount();                 \
        const int totalCount = TestTypeStats::totalCount();                   \
        ALLOC_ARG_T allocator(&ta);                                           \
        typedef TestType<ALLOC_TYPE, USE_PREFIX_ARG> ValueType;               \
        bsls::ObjectBuffer<ValueType> buf;                                    \
        ValueType& mX = buf.object(); const ValueType& X = mX;                \
        Util::construct(&mX, allocator);                                      \
        ASSERT(objCount + 1   == TestTypeStats::currentCount());              \
        ASSERT(totalCount + 1 == TestTypeStats::totalCount());                \
        ASSERT(-1 == X[1]);                                                   \
        bslma::Allocator *expAlloc = EXP_PROPAGATE ? &ta : &da;             \
        ASSERT(X.matchAllocator(expAlloc));                                   \
        mX.~ValueType();                                                      \
      } while (false)

#define TEST(ALLOC_TYPE, ALLOC_ARG_T, EXP_PROPAGATE) do {              \
        TEST_IMP(ALLOC_TYPE, false, ALLOC_ARG_T, EXP_PROPAGATE);       \
        TEST_IMP(ALLOC_TYPE, true , ALLOC_ARG_T, EXP_PROPAGATE);       \
      } while (false)

        const int initObjCount = TestTypeStats::currentCount();

        //                                                Expect
        //   Allocator parameter   Allocator argument   propagation
        //   ===================   ===================  ===========
        TEST(void               ,  NonAllocator       , false      );
        TEST(void               ,  PmrAllocator<short>, true       );
        TEST(void               ,  BslAllocator<int>  , true       );
        TEST(void               ,  LegacyAllocator    , true       );
        TEST(void               ,  STLAllocator<short>, true       );

        TEST(PmrAllocator<int>  ,  NonAllocator       , false      );
        TEST(PmrAllocator<int>  ,  PmrAllocator<short>, true       );
        TEST(PmrAllocator<int>  ,  BslAllocator<int>  , true       );
        TEST(PmrAllocator<int>  ,  LegacyAllocator    , true       );
        TEST(BslAllocator<short>,  NonAllocator       , false      );
        TEST(BslAllocator<short>,  BslAllocator<int>  , true       );
        TEST(BslAllocator<short>,  LegacyAllocator    , true       );
        TEST(LegacyAllocator    ,  NonAllocator       , false      );
        TEST(LegacyAllocator    ,  BslAllocator<int>  , true       );
        TEST(LegacyAllocator    ,  LegacyAllocator    , true       );
        TEST(STLAllocator<char> ,  NonAllocator       , false      );
        TEST(STLAllocator<char> ,  PmrAllocator<short>, false      );
        TEST(STLAllocator<char> ,  BslAllocator<int>  , false      );
        TEST(STLAllocator<char> ,  LegacyAllocator    , false      );
        TEST(STLAllocator<char> ,  STLAllocator<short>, true       );

        ///// Negative tests.  These should fail to compile.
//      TEST(PmrAllocator<int>  ,  STLAllocator<>     , false      );
//      TEST(BslAllocator<short>,  PmrAllocator<int>  , false      );
//      TEST(BslAllocator<short>,  STLAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  PmrAllocator<int>  , false      );
//      TEST(LegacyAllocator    ,  STLAllocator<int>  , false      );

        ASSERT(initObjCount == TestTypeStats::currentCount());
#undef TEST
#undef TEST_IMP
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 'FrozenArg' captures the value and copy/move state of any
        //:   'bsltf::ArgumentType' instance.
        //:
        //: 2 'bslma::UsesBslmaAllocator<TestType<ALLOC, USE_PREFIX_ARG>>'
        //:   yields 'true' if 'ALLOC' is 'PmrAllocator<T>', 'BslAllocator<T>',
        //:   or 'bslma::Allocator *'.
        //:
        //: 3 'bslmtf::UsesAllocatorArgT<TestType<ALLOC, USE_PREFIX_ARG>>'
        //:   yields 'true' if 'ALLOC' is non-'void' and 'USE_PREFIX_ARG' is
        //:   'true'.
        //:
        //: 4 'TestType<ALLOC, U>::allocator_type' is not defined if 'ALLOC' is
        //:   'void' or 'bslma::Allocator *'; otherwise 'allocator_type' is
        //:   'ALLOC'.
        //:
        //: 5 'TestType' can be constructed with 1 to 14 arguments of type
        //:   'bsltf::ArgumentType<1>' to 'bsltf::ArgumentType<14>',
        //:   respectively.  The values of the arguments is captured in the
        //:   'TestType' object.
        //:
        //: 6 For each argument of 'const' lvalue type passed to a 'TestType'
        //:   constructor, the captured 'FrozenArg' reflects that the
        //:   argument was copied from a 'const' lvalue.
        //:
        //: 7 For each argument of mutable lvalue type passed to a 'TestType'
        //:   constructor, the captured 'FrozenArg' reflects that the
        //:   argument was copied from a non-'const' lvalue.
        //:
        //: 8 For each argument of rvalue ('bslmf::MutableRef') type passed to
        //:   a 'TestType' constructor, the captured 'FrozenArg' reflects that
        //:   the argument was moved from an xvalue.
        //:
        //: 9 If no allocator is supplied, the default allocator is used to
        //:   construct 'TestType' (for AA instantiations).
        //:
        //: 10 For non-'void' 'ALLOC', 'TestType<ALLOC, USE_PREFIX_ARG>', can
        //:   be constructed with a trailing allocator argument (if
        //:   'USE_PREFIX_ARG' is 'false') or a leading 'bsl::allocator_arg'
        //:   followed by an allocator argument (if 'USE_PREFIX_ARG' is
        //:   'true').  The allocator accessor should return a copy of the
        //:   allocator provided at construction and 'matchAllocator' should
        //:   return true when given that allocator and 'false' for any other
        //:   allocator.
        //:
        //: 11 The 'TestType' copy constructor and extended copy constructor
        //:   copy the values and record that the new object is in a
        //:   copied-into state.  If an allocator is provided, the new object
        //:   uses that allocator, otherwise it uses the default allocator (for
        //:   AA instantiations).
        //:
        //: 12 The 'TestType' move constructor and extended move constructor
        //:   copy the values, record that the new object is in a moved-into
        //:   state, and record that the original object is in a moved-from
        //:   state after the move.  If an allocator is provided, the new
        //:   object uses that allocator, otherwise it uses the allocator of
        //:   the moved-from object (for AA instantiations).
        //:
        //: 13 The 'TestType' assignment operators copy the values but not the
        //:   allocators, which remain unchanged.  For copy assignment, the lhs
        //:   object enters a copied-into state.  For move assignment, the rhs
        //:   object enters a moved-into state and the lhs object enters a
        //:   moved-from state.
        //
        // Plan:
        //: 1 Put a set of 'bsltf::ArgumentType' objects into every legal
        //:   move/copy state and arbitrary values and construct an 'FrozenArg'
        //:   for each such object and verify that the value and move/copy
        //:   state is captured correctly.  (C-1)
        //:
        //: 2 For 'ALLOC' types 'void', 'PmrAllocator', 'BslAllocator',
        //:   'bslma::Allocator *', and a standard-compliant allocator that is
        //:   none of the preceding, and for 'USE_PREFIX_ARG' values 'false'
        //:   and 'true', verify, for each combination, 'TT', of type
        //:   'TestType<ALLOC, USE_PREFIX_ARG>', that
        //:   'bslma::UsesBslmaAllocator<TT>', 'bslmtf::UsesAllocatorArgT<TT>',
        //:   and 'TT::allocator_type' have the expected types.  (C-2, C-3,
        //:   C-4)
        //:
        //: 3 For a representative sample of the types in step 2, including all
        //:   of the 'ALLOC' types and a selection of 'USE_PREFIX_ARG' values,
        //:   construct objects of type 'TestType<ALLOC, USE_PREFIX_ARG>' using
        //:   0 to 14 arguments, where each argument is a 'const' lvalue.
        //:   Verify that the argument values are captured and that their
        //:   'copiedInto' attribute is 'e_COPIED_CONST'.  (C-5, C-6)
        //:
        //: 4 Repeat step 3 with non-'const' lvalue arguments.  Verify that the
        //:   captured arguments return 'e_COPIED_MUTABLE' from their
        //:   'copiedInto()' accessesor.  (C-7)
        //:
        //: 5 Repeat step 3 with rvalue ('MovableRef') arguments.  Verify that
        //:   the captured arguments return 'e_MOVED' from their 'movedInto()'
        //:   accessor.  (C-8)
        //:
        //: 6 Perform steps 3-5 without passing an allocator to the 'TestType'
        //:   constructor.  Verify that 'matchAllocator(&da)' returns 'true',
        //:   where 'da' is the default allocator.  (C-9)
        //:
        //: 7 For 'ALLOC' types other than 'void', repeat steps 3-5 supplying a
        //:   trailing allocator (if 'USE_PREFIX_ARG' is 'false') or leading
        //:   'bsl::allocator_arg' + allocator (if 'USE_PREFIX_ARG' is 'true').
        //:   Verify that the allocator accessor and 'matchAllocator' method
        //:   return the expected values.  (C-10)
        //:
        //: 8 Copy construct and move construct new objects from the ones
        //:   created in steps 3-5.  Verify that the values and copy/move state
        //:   of the objects is as expected.  Repeat using the extended copy
        //:   and move constructors and a different allocator.  Verify that the
        //:   allocator is as expected.  (C-11, C-12)
        //:
        //: 9 Invoke the copy and move assignment operators on objects having
        //:   different initial values and different allocators.  Verify that
        //:   the values were copied or moved and that the allocators remained
        //:   unchanged.  Verify that the copy/move state of all objects is as
        //:   expected.  (C-13)
        //
        // Testing:
        //     TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST APPARATUS"
                            "\n==============\n");

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);
        bslma::TestAllocator ta2("test allocator 2", veryVeryVeryVerbose);
#define CHECK_RESULTS(R, ALLOC, ALLOC2)                                       \
            BSLS_MACROREPEAT_SEP(R, ASSERT_ARG_MOVED_FROM, ; );               \
            ASSERTV(R, X.matchAllocator(ALLOC));                              \
            ASSERTV(R, Y.matchAllocator(ALLOC));                              \
            ASSERTV(R, Z.matchAllocator(ALLOC));                              \
            ASSERTV(R, C.matchAllocator(defaultAlloc));                       \
            ASSERTV(R, EC.matchAllocator(ALLOC2));                            \
            ASSERTV(R, M.matchAllocator(ALLOC));                              \
            ASSERTV(R, EM1.matchAllocator(ALLOC ));                           \
            ASSERTV(R, EM2.matchAllocator(ALLOC2));                           \
            ASSERTV(R, X.isNotCopiedOrMoved());                               \
            ASSERTV(R, Y.isNotCopiedOrMoved());                               \
            ASSERTV(R, Z.isNotCopiedOrMoved());                               \
            ASSERTV(R, C.isConstCopiedInto());                                \
            ASSERTV(R, EC.isMutableCopiedInto());                             \
            ASSERTV(R, M.isMovedInto());                                      \
            ASSERTV(R, EM1.isMovedInto());                                    \
            ASSERTV(R, EM2.isMutableCopiedInto());                            \
            ASSERTV(R, mMF1.isMovedFrom());                                   \
            ASSERTV(R, mMF2.isMovedFrom());                                   \
            ASSERTV(R, ! mMF3.isMovedFrom());                                 \
            for (int i = 1; i <= R; ++i) {                                    \
                ASSERTV(R, i, VALUES[i], X[i], VALUES[i] == X[i]);            \
                ASSERTV(R, i, VALUES[i], Y[i], VALUES[i] == Y[i]);            \
                ASSERTV(R, i, VALUES[i], Z[i], VALUES[i] == Z[i]);            \
                ASSERTV(R, i, VALUES[i], C[i], VALUES[i] == C[i]);            \
                ASSERTV(R, i, VALUES[i], EC[i], VALUES[i] == EC[i]);          \
                ASSERTV(R, i, VALUES[i], M[i], VALUES[i] == M[i]);            \
                ASSERTV(R, i, VALUES[i], EM1[i], VALUES[i] == EM1[i]);        \
                ASSERTV(R, i, VALUES[i], EM2[i], VALUES[i] == EM2[i]);        \
                ASSERTV(R, i, X[i].isConstCopiedInto());                      \
                ASSERTV(R, i, Y[i].isMutableCopiedInto());                    \
                ASSERTV(R, i, Z[i].isMovedInto());                            \
            }

#define CONSTRUCT_NO_ALLOC(R) {                                           \
            ArgPack mAP; const ArgPack& AP = mAP;                         \
            TT mX(BSLS_MACROREPEAT_COMMA(R, CONST_ARG));                  \
            TT mY(BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG));                \
            TT mZ(BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG));                 \
            const TT& X = mX, & Y = mY, & Z = mZ;                         \
            TT mMF1(X), &mMF2 = mMF1, mMF3(X);                            \
            const TT C(X);                                                \
            const TT EC(mX);                                              \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT& EM1 = M;                                            \
            const TT& EM2 = EC;                                           \
            CHECK_RESULTS(R, defaultAlloc, defaultAlloc)                  \
        }

#define CONSTRUCT_TRAILING_ALLOC(R) {                                     \
            ArgPack mAP; const ArgPack& AP = mAP;                         \
            TT mX(BSLS_MACROREPEAT_COMMA(R, CONST_ARG), alloc);           \
            TT mY(BSLS_MACROREPEAT_COMMA(R, MUTABLE_ARG), alloc);         \
            TT mZ(BSLS_MACROREPEAT_COMMA(R, RVALUE_ARG), alloc);          \
            const TT& X = mX, & Y = mY, & Z = mZ;                         \
            TT mMF1(X, alloc), mMF2(X, alloc), mMF3(X, alloc);            \
            const TT C(X);                                                \
            const TT EC(mX, alloc2);                                      \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT EM1(MoveUtil::move(mMF2), alloc );                   \
            const TT EM2(MoveUtil::move(mMF3), alloc2);                   \
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
            const TT& X = mX, & Y = mY, & Z = mZ;                         \
            TT mMF1(bsl::allocator_arg, alloc, X);                        \
            TT mMF2(bsl::allocator_arg, alloc, X);                        \
            TT mMF3(bsl::allocator_arg, alloc, X);                        \
            const TT C(X);                                                \
            const TT EC(bsl::allocator_arg, alloc2, mX);                  \
            const TT M(MoveUtil::move(mMF1));                             \
            const TT EM1(bsl::allocator_arg,alloc ,MoveUtil::move(mMF2)); \
            const TT EM2(bsl::allocator_arg,alloc2,MoveUtil::move(mMF3)); \
            CHECK_RESULTS(R, alloc, alloc2)                               \
        }

//@bdetdsplit CODE SLICING BEGIN FIRST
#define TEST_TRAITS(ALLOC, USE_PREFIX_ARG,                                    \
                    EXP_BSLMA, EXP_PREFIX, EXP_ALLOC_TYPE) do {               \
        typedef TestType<ALLOC, USE_PREFIX_ARG> TT;                           \
        ASSERT(EXP_BSLMA == bslma::UsesBslmaAllocator<TT>::value);            \
        ASSERT(EXP_PREFIX == bslmf::UsesAllocatorArgT<TT>::value);            \
        ASSERT(EXP_ALLOC_TYPE == bslma::HasAllocatorType<TT>::value);         \
        typedef bsl::conditional<EXP_ALLOC_TYPE, TT,                          \
                                 NoAlloc>::type::allocator_type AllocType;    \
        if (EXP_ALLOC_TYPE) ASSERT((bsl::is_same<AllocType, ALLOC>::value));  \
        ASSERT(! bslmf::IsBitwiseMoveable<TT>::value);                        \
      } while (false)
        //                            use     exp    exp      exp
        //          ALLOC            prefix  bslma  prefix alloc_type
        //          ===============  ======  =====  ====== ==========
        TEST_TRAITS(void           , false,  false, false,  false);
        TEST_TRAITS(void           , true ,  false, false,  false);
        TEST_TRAITS(PmrAllocator<> , false,  true , false,  true );
        TEST_TRAITS(PmrAllocator<> , true ,  true , true ,  true );
        TEST_TRAITS(BslAllocator<> , false,  true , false,  true );
        TEST_TRAITS(BslAllocator<> , true ,  true , true ,  true );
        TEST_TRAITS(LegacyAllocator, false,  true , false,  false);
        TEST_TRAITS(LegacyAllocator, true ,  true , true ,  false);
        TEST_TRAITS(STLAllocator<> , false,  false, false,  true );
        TEST_TRAITS(STLAllocator<> , true ,  false, true ,  true );
#undef TEST_TRAITS

        if (veryVerbose) printf("Testing FrozenArg\n");
        {
#define TEST_FROZEN_ARG(V, CM_STATE) do {                                 \
                const int                    val     = (V);               \
                const CopyMoveTracker::State cmState = (CM_STATE);        \
                ArgumentType<L_> mA; const ArgumentType<L_>& A = mA;      \
                mA.set(val, cmState);                                     \
                FrozenArg h(A);                                           \
                ASSERTV(h, val == h);                                     \
                ASSERTV(h.copyMoveState(), cmState == h.copyMoveState()); \
                ASSERTV(A.copyMoveState(), cmState == A.copyMoveState()); \
        } while (false)

            FrozenArg arg;
            ASSERTV(arg                , -1 == arg);
            ASSERTV(arg.copyMoveState(), arg.isNotCopiedOrMoved());

            TEST_FROZEN_ARG( 0, CopyMoveTracker::e_NOT_COPIED_OR_MOVED);
            TEST_FROZEN_ARG( 1, CopyMoveTracker::e_CONST_COPIED_INTO);
            TEST_FROZEN_ARG( 2, CopyMoveTracker::e_MUTABLE_COPIED_INTO);
            TEST_FROZEN_ARG( 3, CopyMoveTracker::e_MOVED_INTO);
            TEST_FROZEN_ARG(-1, CopyMoveTracker::e_MOVED_FROM);

#undef TEST_FROZEN_ARG
        }

        if (veryVerbose) printf("Testing TestType<void, false>\n");
        {
            typedef void AllocType;
            typedef TestType<AllocType, false> TT;
            void *defaultAlloc = 0;

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Aways returns true
            BSLS_MACROREPEAT(14, CONSTRUCT_NO_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt2(ARG);
            ASSERTV(! tt1.isCopiedInto());
            tt1 = tt2;
            ASSERTV(  tt1.isCopiedInto());
            ASSERTV(tt1[1] == ARG);

            // Test move assignment
            TT tt3;
            tt3 = MoveUtil::move(tt2);
            ASSERTV(tt3.isMovedInto());
            ASSERTV(tt2.isMovedFrom());
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
            BSLS_MACROREPEAT(14, CONSTRUCT_NO_ALLOC)

            TT tt2(alloc);                       // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_TRAILING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(ARG, alloc2);
            ASSERTV(! tt2.isCopiedInto());
            tt2 = tt3;
            ASSERTV(  tt2.isCopiedInto());
            ASSERTV(! tt2.isConstCopiedInto());
            ASSERTV(  tt2.isMutableCopiedInto());
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with same allocator
            TT tt4(alloc);
            tt4 = MoveUtil::move(tt2);
            ASSERTV(tt4.isMovedInto());
            ASSERTV(tt2.isMovedFrom());
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with different allocator
            TT tt5(alloc);
            tt5 = MoveUtil::move(tt3);
            ASSERTV(tt5.isMutableCopiedInto());
            ASSERTV(! tt3.isMovedFrom());
            ASSERTV(tt5[1] == ARG);
            ASSERTV(tt5.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

        if (veryVerbose) printf("Testing TestType<PmrAllocator, true>\n");
        {
            typedef PmrAllocator<>            AllocType;
            typedef TestType<AllocType, true> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Default allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_NO_ALLOC)

            TT tt2(bsl::allocator_arg, alloc);   // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_LEADING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(bsl::allocator_arg, alloc2, ARG); const TT& TT3 = tt3;
            ASSERTV(! tt2.isCopiedInto());
            tt2 = TT3;
            ASSERTV(  tt2.isCopiedInto());
            ASSERTV(  tt2.isConstCopiedInto());
            ASSERTV(! tt2.isMutableCopiedInto());
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with same allocator
            TT tt4(bsl::allocator_arg, alloc);
            tt4 = MoveUtil::move(tt2);
            ASSERTV(tt4.isMovedInto());
            ASSERTV(tt2.isMovedFrom());
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with different allocator
            TT tt5(bsl::allocator_arg, alloc);
            tt5 = MoveUtil::move(tt3);
            ASSERTV(tt5.isMutableCopiedInto());
            ASSERTV(! tt3.isMovedFrom());
            ASSERTV(tt5[1] == ARG);
            ASSERTV(tt5.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

//@bdetdsplit CODE SLICING BREAK
        if (veryVerbose) printf("Testing TestType<BslAllocator<>, false>\n");
        {
            typedef BslAllocator<>             AllocType;
            typedef TestType<AllocType, false> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                              // Test default ctor
            ASSERTV(tt1.matchAllocator(&da));  // Default allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_NO_ALLOC)

            TT tt2(alloc);                       // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_TRAILING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(ARG, alloc2);
            ASSERTV(! tt2.isCopiedInto());
            tt2 = tt3;
            ASSERTV(tt2.isCopiedInto());
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with same allocator
            TT tt4(alloc);
            tt4 = MoveUtil::move(tt2);
            ASSERTV(tt4.isMovedInto());
            ASSERTV(tt2.isMovedFrom());
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with different allocator
            TT tt5(alloc);
            tt5 = MoveUtil::move(tt3);
            ASSERTV(tt5.isMutableCopiedInto());
            ASSERTV(! tt3.isMovedFrom());
            ASSERTV(tt5[1] == ARG);
            ASSERTV(tt5.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }

        if (veryVerbose) printf("Testing TestType<STLAllocator, true>\n");
        {
            typedef STLAllocator<>            AllocType;
            typedef TestType<AllocType, true> TT;
            AllocType alloc(&ta);
            AllocType alloc2(&ta2);
            AllocType defaultAlloc(&da);

            TT tt1;                                     // Test default ctor
            ASSERTV(tt1.matchAllocator(defaultAlloc));  // Default allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_NO_ALLOC)

            TT tt2(bsl::allocator_arg, alloc);   // Test extended default ctor
            ASSERTV(tt2.matchAllocator(alloc));  // Specified allocator
            BSLS_MACROREPEAT(14, CONSTRUCT_LEADING_ALLOC)

            // Test copy assignment
            const ArgType1 ARG(VALUES[1]);
            TT tt3(bsl::allocator_arg, alloc2, ARG);
            ASSERTV(! tt2.isCopiedInto());
            tt2 = tt3;
            ASSERTV(  tt2.isCopiedInto());
            ASSERTV(! tt2.isConstCopiedInto());
            ASSERTV(  tt2.isMutableCopiedInto());
            ASSERTV(tt2[1] == ARG);
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with same allocator
            TT tt4(bsl::allocator_arg, alloc);
            tt4 = MoveUtil::move(tt2);
            ASSERTV(tt4.isMovedInto());
            ASSERTV(tt2.isMovedFrom());
            ASSERTV(tt4[1] == ARG);
            ASSERTV(tt4.matchAllocator(alloc));
            ASSERTV(tt2.matchAllocator(alloc));

            // Test move assignment with different allocator
            TT tt5(bsl::allocator_arg, alloc);
            tt5 = MoveUtil::move(tt3);
            ASSERTV(tt5.isMutableCopiedInto());
            ASSERTV(! tt3.isMovedFrom());
            ASSERTV(tt5[1] == ARG);
            ASSERTV(tt5.matchAllocator(alloc));
            ASSERTV(tt3.matchAllocator(alloc2));
        }
//@bdetdsplit CODE SLICING END
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
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta;
        BslAllocator<>       bslAlloc(&ta);
        int                  dummy;
        int                 *nonAlloc = &dummy;

        ArgumentType<1> arg1(10);
        ArgumentType<2> arg2(20);

        if (veryVerbose) printf("Testing 'construct' and 'make'\n");

        // Construct a non-AA type ('int')
        {
            typedef int                    TT;
            typedef bsls::ObjectBuffer<TT> ObjBuf;

            ObjBuf b0;
            TT&    obj0 = b0.object();
            Util::construct(&obj0, nonAlloc, 100);
            ASSERT(100 == obj0);

            ObjBuf b1;
            TT&    obj1 = b1.object();
            Util::construct(&obj1, &ta, 11);
            ASSERT(11 == obj1);

            ObjBuf b2;
            TT&    obj2 = b2.object();
            Util::construct(&obj2, bslAlloc, 22);
            ASSERT(22 == obj2);

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
            TT obj3(Util::make<TT>(bslAlloc, 33));
            ASSERT(33 == obj3);
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
        }

        // Construct legacy AA type that uses trailing allocator argument
        {
            typedef TestType<bslma::Allocator *, false> TT;
            typedef bsls::ObjectBuffer<TT>              ObjBuf;

            ObjBuf b0;
            TT&    obj0 = b0.object();
            Util::construct(&obj0, nonAlloc, arg1, arg2);
            ASSERT(&da == obj0.allocator());
            ASSERT(10 == obj0[1]);
            ASSERT(20 == obj0[2]);
            obj0.~TT();

            ObjBuf b1;
            TT&    obj1 = b1.object();
            Util::construct(&obj1, &ta, arg1, arg2);
            ASSERT(&ta == obj1.allocator());
            ASSERT(10 == obj1[1]);
            ASSERT(20 == obj1[2]);
            obj1.~TT();

            ObjBuf b2;
            TT&    obj2 = b2.object();
            Util::construct(&obj2, bslAlloc, arg1, arg2);
            ASSERT(&ta == obj2.allocator());
            ASSERT(10 == obj2[1]);
            ASSERT(20 == obj2[2]);
            obj2.~TT();

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
            TT obj3(Util::make<TT>(bslAlloc, arg1, arg2));
            ASSERT(&ta == obj3.allocator());
            ASSERT(10 == obj3[1]);
            ASSERT(20 == obj3[2]);
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
        }

        // Construct bsl-AA type that uses leading allocator argument
        {
            typedef TestType<BslAllocator<>, true> TT;
            typedef bsls::ObjectBuffer<TT>         ObjBuf;

            ObjBuf b0;
            TT&    obj0 = b0.object();
            Util::construct(&obj0, nonAlloc, arg1, arg2);
            ASSERT(&da == obj0.get_allocator());
            ASSERT(10 == obj0[1]);
            ASSERT(20 == obj0[2]);
            obj0.~TT();

            ObjBuf b1;
            TT&    obj1 = b1.object();
            Util::construct(&obj1, &ta, arg1, arg2);
            ASSERT(&ta == obj1.get_allocator());
            ASSERT(10 == obj1[1]);
            ASSERT(20 == obj1[2]);
            obj1.~TT();

            ObjBuf b2;
            TT&    obj2 = b2.object();
            Util::construct(&obj2, bslAlloc, arg1, arg2);
            ASSERT(&ta == obj2.get_allocator());
            ASSERT(10 == obj2[1]);
            ASSERT(20 == obj2[2]);
            obj2.~TT();

#if defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
            TT obj3(Util::make<TT>(bslAlloc, arg1, arg2));
            ASSERT(&ta == obj3.get_allocator());
            ASSERT(10 == obj3[1]);
            ASSERT(20 == obj3[2]);
#endif // defined(BSLS_COMPILERFEATURES_GUARANTEED_COPY_ELISION)
        }

        if (veryVerbose) printf("Testing 'destructiveMove'");

        {
            typedef TestType<void, false>  TT;

            bsls::ObjectBuffer<TT> b1, b2;
            TT& obj1 = b1.object();
            TT& obj2 = b2.object();

            ::new (&obj1) TT(arg1, arg2);
            int initObjCount   = TestTypeStats::currentCount();
            int initTotalCount = TestTypeStats::totalCount();
            ASSERT(10 == obj1[1]);
            ASSERT(20 == obj1[2]);

            Util::destructiveMove(&obj2, nonAlloc, &obj1);
            ASSERT(initObjCount       == TestTypeStats::currentCount());
            ASSERT(initTotalCount + 1 == TestTypeStats::totalCount());
            ASSERT(10 == obj2[1]);
            ASSERT(20 == obj2[2]);
            ASSERT(obj2.isMovedInto());
            ASSERT(&obj1 == TestTypeStats::lastDestroyed());
            obj2.~TT();
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(test, TestTypeStats::currentCount(),
            0 == TestTypeStats::currentCount());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
