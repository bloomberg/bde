// bslalg_nothrowmovablewrapper.t.cpp                                 -*-C++-*-
#include <bslalg_nothrowmovablewrapper.h>

#include <bsla_maybeunused.h>

#include <bslalg_constructorproxy.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a wrapper class holding an object of 'TYPE', and
// providing no other functionality other than returning the wrapped object.
// The use of this class communicates to specific clients (see
// 'bslstl_function') that the wrapped object should be treated as-if it has a
// 'noexcept' move constructor, even in C++03, where 'noexcept' does not exist.
// The tests will verify that all of the constructors correctly construct the
// wrapped object and that all of the manipulators and accessors correctly give
// access to the wrapped object.  Special consideration should be given to
// ensure that allocator-aware types are handled correctly and that
// non-allocator-aware types are treated as such.  The tests also verify that
// the wrapper has the correct traits, based on the wrapped type.
//
// ----------------------------------------------------------------------------
// TRAITS
// [ 3] bsl::is_nothrow_move_constructible
// [ 3] bslma::UsesBslmaAllocator
// [ 3] bslma::UsesAllocatorArgT
// [ 3] bslma::IsBitwiseMoveable
//
// TYPEDEFS
// [ 3] allocator_type
// [ 3] ValueType
//
// CREATORS
// [ 4] NothrowMovableWrapper();
// [ 4] NothrowMovableWrapper(bsl::allocator_arg_t,
//                            const allocator_type&
//                            allocator);
// [ 5] NothrowMovableWrapper(const TYPE& val);
// [ 5] NothrowMovableWrapper(bsl::allocator_arg_t,
//                            const allocator_type& allocator,
//                            const TYPE&           val);
// [ 5] NothrowMovableWrapper(bslmf::MovableRef<TYPE> val);
// [ 5] NothrowMovableWrapper(bsl::allocator_arg_t    ,
//                            const allocator_type&   allocator,
//                            bslmf::MovableRef<TYPE> val);
// [ 6] NothrowMovableWrapper(const NothrowMovableWrapper& original);
// [ 6] NothrowMovableWrapper(bsl::allocator_arg_t      ,
//                            const allocator_type&     alloc,
//                            const NothrowMovableWrapper& original);
// [ 6] NothrowMovableWrapper(bslmf::MovableRef<NothrowMovableWrapper>
//                            original)
//                            BSLS_KEYWORD_NOEXCEPT;
// [ 6] NothrowMovableWrapper(bsl::allocator_arg_t                  ,
//                            const allocator_type&                 alloc,
//                            bslmf::MovableRef<NothrowMovableWrapper>
//                            original);
// [ 8] ~NothrowMovableWrapper();
//
// MANIPULATORS
// [ 7] ValueType& unwrap();
// [ 7] operator ValueType&();
//
// ACCESSORS
// [ 7] ValueType const& unwrap() const;
// [ 7] operator ValueType const&() const;
// [ 7] allocator_type get_allocator() const;
//
// ----------------------------------------------------------------------------
// [ 9] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [ 5] 'bsl::Function_NothrowWrapper' compatibility

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

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                VERBOSITY
//-----------------------------------------------------------------------------

static int verbose             = 0;
static int veryVerbose         = 0;
static int veryVeryVerbose     = 0;
static int veryVeryVeryVerbose = 0;  // For test allocators

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslmf::MovableRefUtil MoveUtil;

int simpleFunction(int i)
    // A simple function that can be stored in no-throw wrapper.
{
    return i;
}

class TrackableValue {
    // This class tracks a value through a series of move and copy operations
    // and has an easily-testable moved-from state.  An instance stores an
    // unsigned integer value and a pair of bits, one indicating if the value
    // was copied, another if it was moved (or neither or both).  When assigned
    // a value at construction, via assignment, or via a mutating operation
    // (such as +=), the move and copy bits are cleared, indicating that the
    // new value has been neither moved nor copied.  When assigned a new value
    // via copy construction or copy assignment, the copy bit is set and the
    // move bit is cleared.  When assigned a new value via move construction,
    // move assignment, or swap, the copy bit is transferred from the original
    // value and the move bit is set.  Thus a value that is copied then moved
    // will have both bits set but a value that is moved then copy has only the
    // copy bit set.  The copy and move bits are not salient attributes of the
    // value and are thus not used for testing equality.

    // PRIVATE CONSTANTS
    enum {
        e_NUM_FLAGS = 2,
        // Value is multiplied by the following value to make room for the
        // flag bits.  Multiply and divide are used to scale the value because
        // shift operations are not guaranteed to preserve sign.  The compiler
        // will almost certainly replace these multiplication and divisions
        // with more efficient arithmetic shift operations.
        e_VALUE_MULTIPLIER = 1 << e_NUM_FLAGS,
        e_FLAGS_MASK       = e_VALUE_MULTIPLIER - 1,
        e_VALUE_MASK       = ~e_FLAGS_MASK,
        e_COPIED_FLAG      = 0x01,
        e_MOVED_FLAG       = 0x02,
        e_MOVED_FROM_VAL = 0x11111111,  // Value assigned to moved-from object

        // The moved from state has a value of e_MOVED_FROM_VAL, with neither
        // flag set.
        e_MOVED_FROM_STATE = e_MOVED_FROM_VAL * e_VALUE_MULTIPLIER
    };

    // PRIVATE DATA
    int d_valueAndFlags;

    TrackableValue& setValue(int  v,
                             bool movedFlag  = false,
                             bool copiedFlag = false);
        // Set the value to the specified 'v'.  Set the 'isMoved()' flag to
        // the optionally-specified 'movedFlag' bit and the 'isCopied()' flag
        // to the optionally-specified 'copiedFlag' bit.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TrackableValue, bslmf::IsBitwiseMoveable);

    // Constants
    enum {
        e_MIN = INT_MIN / e_VALUE_MULTIPLIER,
        e_MAX = INT_MAX / e_VALUE_MULTIPLIER
    };

    // PUBLIC CLASS DATA
    static int s_numDestructorCalls;
        // number of times the destructor of 'TrackableValue' has been invoked.

    // CREATORS
    TrackableValue(int v = 0)                                       // IMPLICIT
        // Set value to the specified 'v' and set 'isMoved()' and 'isCopied()'
        // to false.
    {
        setValue(v);
    }

    TrackableValue(const TrackableValue& other)
        // Copy value from the specified 'other', set 'isMoved()' to false and
        // 'isCopied()' to true.
    {
        setValue(other.value(), false, true);
    }

    TrackableValue(bslmf::MovableRef<TrackableValue> other)         // IMPLICIT
        // Move value from the specified 'other', set 'isMoved()' to true, and
        // set 'isCopied()' to 'other.isCopied()', then set 'other' to the
        // moved-from state.
    {
        *this = bslmf::MovableRefUtil::move(other);
    }


    ~TrackableValue()
        // We count the number of destructor invocations
    {
        s_numDestructorCalls++;
    };

    // MANIPULATORS
    TrackableValue& operator=(int v)
        // Set value to the specified 'v' and set 'isMoved()' and 'isCopied()'
        // to false; then return '*this'.
    {
        return setValue(v);
    }

    TrackableValue& operator=(const TrackableValue& rhs)
        // Set value to the specified 'rhs.value()' and set 'isMoved()' to
        // false and 'isCopied()' to true; then return '*this'.
    {
        return setValue(rhs.value(), false, true);
    }

    TrackableValue& operator=(bslmf::MovableRef<TrackableValue> rhs);
        // Move value from 'rhs', set 'isMoved()' to true, and set 'isCopied()'
        // to 'other.isCopied()', then assign 'rhs' the value
        // 'e_MOVED_FROM_VAL' and return '*this'.

    void resetMoveCopiedFlags()
        // Set 'isMoved()' and 'isCopied() to false.
    {
        d_valueAndFlags &= e_VALUE_MASK;
    }

    void setIsCopiedRaw(bool);
    void setIsMovedRaw(bool);
    void setValueRaw(int);
        // Set the constituent parts of this object without modifying the other
        // parts.  It is up to the caller to ensure that the flags are set
        // consistently.

    void swap(TrackableValue& other);
        // Exchange the values AND 'isCopied()' flags of '*this' and the
        // specified 'other', then set the 'isMoved()' flag of both to true.

    // ACCESSORS
    bool isCopied() const { return d_valueAndFlags & e_COPIED_FLAG; }
    bool isMoved() const { return d_valueAndFlags & e_MOVED_FLAG; }
    bool isMovedFrom() const { return d_valueAndFlags == e_MOVED_FROM_STATE; }
    int  value() const { return d_valueAndFlags / e_VALUE_MULTIPLIER; }

};
// IMPLEMENTATION OF TrackableValue

// PUBLIC CLASS DATA
int TrackableValue::s_numDestructorCalls = 0;

// MANIPULATORS
inline
TrackableValue& TrackableValue::setValue(int  v,
                                         bool movedFlag,
                                         bool copiedFlag)
{
    ASSERT(e_MIN <= v && v <= e_MAX);
    d_valueAndFlags = v * e_VALUE_MULTIPLIER;
    if (movedFlag)
        d_valueAndFlags |= e_MOVED_FLAG;
    if (copiedFlag)
        d_valueAndFlags |= e_COPIED_FLAG;
    return *this;
}

inline
TrackableValue& TrackableValue::operator=(
                                       bslmf::MovableRef<TrackableValue> rhs)
{
    TrackableValue& otherRef = bslmf::MovableRefUtil::access(rhs);
    d_valueAndFlags          = otherRef.d_valueAndFlags | e_MOVED_FLAG;
    otherRef.d_valueAndFlags = e_MOVED_FROM_STATE;
    return *this;
}

void TrackableValue::setIsCopiedRaw(bool copiedFlag)
{
    d_valueAndFlags &= (e_VALUE_MASK | e_MOVED_FLAG);
    if (copiedFlag)
        d_valueAndFlags |= e_COPIED_FLAG;
}

void TrackableValue::setIsMovedRaw(bool movedFlag)
{
    d_valueAndFlags &= (e_VALUE_MASK | e_COPIED_FLAG);
    if (movedFlag)
        d_valueAndFlags |= e_MOVED_FLAG;
}

void TrackableValue::setValueRaw(int v)
{
    d_valueAndFlags &= e_FLAGS_MASK;
    d_valueAndFlags |= v * e_VALUE_MULTIPLIER;
}

void TrackableValue::swap(TrackableValue& other)
{
    // Don't use std::swap<int> because don't want to #include <algorithm>
    int tmp               = d_valueAndFlags;
    d_valueAndFlags       = other.d_valueAndFlags;
    other.d_valueAndFlags = tmp;
    d_valueAndFlags |= e_MOVED_FLAG;
    other.d_valueAndFlags |= e_MOVED_FLAG;
}

// TrackableValue FREE FUNCTIONS
BSLA_MAYBE_UNUSED
inline
bool operator==(const TrackableValue& a, const TrackableValue& b)
{
    return a.value() == b.value();
}

BSLA_MAYBE_UNUSED
inline
bool operator!=(const TrackableValue& a, const TrackableValue& b)
{
    return a.value() != b.value();
}

BSLA_MAYBE_UNUSED
inline
void swap(TrackableValue& a, TrackableValue& b)
{
    a.swap(b);
}

// End implementation of TrackableValue

class TrackableValueWithAlloc {
    // Trackable value with allocator.
    // - Uses allocator
    // - Not bitwise moveable
    // - potentially-throwing move constructor

    TrackableValue       d_trackable;
    bsl::allocator<char> d_alloc;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(TrackableValueWithAlloc,
                                   bslma::UsesBslmaAllocator);

    typedef bsl::allocator<char> allocator_type;

    explicit TrackableValueWithAlloc(int v = 0)
    : d_trackable(v)
    , d_alloc()
    {
    }

    explicit TrackableValueWithAlloc(const allocator_type& alloc)
    : d_trackable(0)
    , d_alloc(alloc)
    {
    }

    TrackableValueWithAlloc(int v, const allocator_type& alloc)
    : d_trackable(v)
    , d_alloc(alloc)
    {
    }

    TrackableValueWithAlloc(
                       const TrackableValueWithAlloc& original,
                       const allocator_type&          alloc = allocator_type())
    : d_trackable(original.d_trackable)
    , d_alloc(alloc)
    {
    }

    // Move constructor propagates allocator
    TrackableValueWithAlloc(
        bslmf::MovableRef<TrackableValueWithAlloc> original)
    : d_trackable(bslmf::MovableRefUtil::move(
          bslmf::MovableRefUtil::access(original).d_trackable))
    , d_alloc(bslmf::MovableRefUtil::access(original).d_alloc)
    {
    }

    TrackableValueWithAlloc(
        bslmf::MovableRef<TrackableValueWithAlloc> original,
        const allocator_type&                      alloc)
    : d_trackable(bslmf::MovableRefUtil::move(
          bslmf::MovableRefUtil::access(original).d_trackable))
    , d_alloc(alloc)
    {
    }

    ~TrackableValueWithAlloc() { memset((void *)this, 0xbb, sizeof(*this)); }

    TrackableValueWithAlloc& operator=(const TrackableValueWithAlloc& rhs)
    {
        d_trackable = rhs.d_trackable;
        return *this;
    }

    TrackableValueWithAlloc& operator=(
                                bslmf::MovableRef<TrackableValueWithAlloc> rhs)
    {
        TrackableValueWithAlloc& rhsRef = rhs;
        d_trackable = bslmf::MovableRefUtil::move(rhsRef.d_trackable);

        return *this;
    }

    void resetMoveCopiedFlags()
        // Set 'isMoved()' and 'isCopied() to false.
    {
        d_trackable.resetMoveCopiedFlags();
    }

    void setValue(int v) { d_trackable = v; }

    // ACCESSORS
    bslma::Allocator *allocator() const { return d_alloc.mechanism(); }
    allocator_type    get_allocator() const { return d_alloc; }

    bool isCopied() const { return d_trackable.isCopied(); }
    bool isMoved() const { return d_trackable.isMoved(); }
    bool isMovedFrom() const { return d_trackable.isMovedFrom(); }
    int  value() const { return d_trackable.value(); }

};

BSLA_MAYBE_UNUSED inline bool operator==(const TrackableValueWithAlloc& a,
                                    const TrackableValueWithAlloc& b)
{
    return a.value() == b.value();
}

BSLA_MAYBE_UNUSED inline bool operator!=(const TrackableValueWithAlloc& a,
                                    const TrackableValueWithAlloc& b)
{
    return a.value() != b.value();
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace {

///Example 1
///- - - - -
// In this example, we define a class template, 'CountedType<TYPE>', that is
// little more than just a wrapper around 'TYPE' that counts the count member
// along with the single value member:
//..
template <class TYPE>
class CountedType {
    // CLASS DATA
    static int s_count;

    // DATA
    TYPE d_value;
        //..
        // Because of externally-imposed requirements, the move constructor for
        // 'CountedType' must provide the strong guarantee; i.e., if the move
        // constructor of 'TYPE' throws an exception, then the moved-from
        // 'CountedType' object must be left unchanged.  To support this
        // requirement, we next define a private static function,
        // 'MoveIfNoexcept', similar to the standard 'std::move_if_noexcept',
        // that returns a movable reference if its argument is no-throw move
        // constructible and a const lvalue reference otherwise:
        //..
        // PRIVATE CLASS FUNCTIONS
    template <class TP>
    static typename bsl::conditional<
        bsl::is_nothrow_move_constructible<TP>::value,
        bslmf::MovableRef<TP>,
        const TP&>::type
    MoveIfNoexcept(TP&);
        //..
        // We next finish out the class definition with a constructor, copy
        // constructor, move constructor, destructor, and member functions to
        // retrieve the count and value:
        //..
  public:
    // CLASS FUNCTIONS
    static int count() { return s_count; }

    // CREATORS
    CountedType(const TYPE& val);                                   // IMPLICIT
        // Construct 'CountedType' from the specified 'val'.

    CountedType(const CountedType& original);
        // Copy construct '*this' from the specified 'original' object.

    CountedType(bslmf::MovableRef<CountedType> original);           // IMPLICIT
        // Move construct '*this' from the specified 'original'.  If an
        // exception is thrown, by the constructor for 'TYPE' 'original' is
        // unchanged.

    ~CountedType() { --s_count; }
        // Destroy this object.

    // MANIPULATORS
    TYPE& value() { return d_value; }

    // ACCESSORS
    const TYPE& value() const { return d_value; }
};
//..
// Next, we implement 'MoveIfNoexcept', which calls 'move' on its argument,
// allowing it to convert back to an lvalue if the return type is an lvalue
// reference:
//..
template <class TYPE>
template <class TP>
inline
typename bsl::conditional<bsl::is_nothrow_move_constructible<TP>::value,
                          bslmf::MovableRef<TP>,
                          const TP&>::type
CountedType<TYPE>::MoveIfNoexcept(TP& x)
{
    return bslmf::MovableRefUtil::move(x);
}
//..
// Next, we implement the value constructor and move constructor, which simply
// copy their argument into the 'd_value' data members and increment the count:
//..
template <class TYPE>
CountedType<TYPE>::CountedType(const TYPE& val)
: d_value(val)
{
    ++s_count;
}

template <class TYPE>
CountedType<TYPE>::CountedType(const CountedType& original)
: d_value(original.d_value)
{
    ++s_count;
}
//..
// We're now ready implement the move constructor.  Logically, we would simply
// move the value from 'original' into the 'd_value' member of '*this', but an
// exception thrown 'TYPE''s move constructor would leave 'original' in a
// (valid but) unspecified state, violating the strong guarantee.  Instead, we
// move the value only if we know that the move will succeed; otherwise, we
// copy it.  This behavior is facilitated by the 'MoveIfNoexcept' function
// defined above:
//..
template <class TYPE>
CountedType<TYPE>::CountedType(bslmf::MovableRef<CountedType> original)
: d_value(MoveIfNoexcept(bslmf::MovableRefUtil::access(original).d_value))
{
    ++s_count;
}
//..
// Finally, we define the 's_count' member to complete the class
// implementation:
//..
template <class TYPE>
int CountedType<TYPE>::s_count = 0;
//..
// To test the 'CountedType' class template, assume a simple client type,
// 'SomeType' that makes it easy to detect if it was move constructed.
// 'SomeType' holds an 'int' value which is set to -1 when it is moved from, as
// shown here:
//..
class SomeType {
    int d_value;

  public:
    SomeType(int v = 0)                                             // IMPLICIT
    : d_value(v)
    {
    }
    SomeType(const SomeType& original)
    : d_value(original.d_value)
    {
    }
    SomeType(bslmf::MovableRef<SomeType> original)
    : d_value(bslmf::MovableRefUtil::access(original).d_value)
    {
        bslmf::MovableRefUtil::access(original).d_value = -1;
    }

    int value() const { return d_value; }
};
//..
// Notice that 'SomeType' neglected to declare its move constructor as
// 'noexcept'.  This might be an oversight or it could be an old class that
// predates both 'noexcept' and the 'bsl::is_nothrow_move_constructible' trait.
// It is even be possible that the move constructor might throw (though, of
// course, it doesn't in this simplified example).  Regardless, the effect is
// that move-constructing a 'CountedType<SomeType>' will result in the move
// constructor actually performing a copy:
//..
void usageExample1()
{
    CountedType<SomeType> obj1(1);
    CountedType<SomeType> obj2(bslmf::MovableRefUtil::move(obj1));
    ASSERT(1 == obj1.value().value());  // Copied, not moved from
    ASSERT(1 == obj2.value().value());
    //..
    // For the purpose of this example, we can be sure that 'SomeThing' will
    // not throw on move, at least not in our applcation.  In order to obtain
    // the expected move optimiztion, we next wrap our 'SomeType in a
    // 'bslalg::NothrowMovableWrapper':
    //..
    CountedType<bslalg::NothrowMovableWrapper<SomeType> > obj3(SomeType(3));
    CountedType<bslalg::NothrowMovableWrapper<SomeType> > obj4(
                                            bslmf::MovableRefUtil::move(obj3));
    ASSERT(-1 == obj3.value().unwrap().value());  // moved from
    ASSERT(3 == obj4.value().unwrap().value());
}
}  // close unnamed namespace

template <class TYPE,
          bool  USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class AllocatorTestUtil {
    // This class provided test utilities that have different behaviour
    // depending on whether 'TYPE is allocator-aware or not.  The main template
    // is for allocator-aware types.

  public:
    static bool checkAllocator(const TYPE&                 obj,
                               const bsl::allocator<char>& expected);
        // Check if, for the specified 'obj', 'obj.get_allocator()' returns the
        // specified 'expected' allocator.

    static bool hasSameAllocator(const TYPE& obj, const TYPE& other);
        // Check if, for the specified 'obj' and specified 'other',
        // 'obj.allocator() == other.allocator()';
};

template <class TYPE>
class AllocatorTestUtil<TYPE, false> {
    // This class provided test utilities that have different behaviour
    // depending on whether 'TYPE is allocator-aware or not.  This
    // specialization is for non allocator-aware types.

  public:
    static bool checkAllocator(const TYPE&, const bsl::allocator<char>&);
        // return 'true'.

    static bool hasSameAllocator(const TYPE&, const TYPE&);
        // return 'true'.
};

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool AllocatorTestUtil<TYPE, USES_BSLMA_ALLOC>::checkAllocator(
                                          const TYPE&                 obj,
                                          const bsl::allocator<char>& expected)
{
    return (expected == obj.get_allocator());
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool AllocatorTestUtil<TYPE, USES_BSLMA_ALLOC>::hasSameAllocator(
                                                         const TYPE& obj,
                                                         const TYPE& other)
{
    return (obj.allocator() == other.allocator());
}

template <class TYPE>
inline bool
AllocatorTestUtil<TYPE, false>::checkAllocator(const TYPE&,
                                       const bsl::allocator<char>&)
{
    return true;
}

template <class TYPE>
inline bool
AllocatorTestUtil<TYPE, false>::hasSameAllocator(const TYPE&, const TYPE&)
{
    return true;
}

template <class TYPE>
bool checkAllocator(const TYPE& obj, const bsl::allocator<char>& allocator)
    // If 'TYPE' is allocator-aware, return whether the allocator of the
    // specified 'obj' is specified 'allocator'; otherwise return 'true'.
{
    return AllocatorTestUtil<TYPE>::checkAllocator(obj, allocator);
}

template <class TYPE>
bool hasSameAllocator(const TYPE& obj1, const TYPE& obj2)
    // If 'TYPE' is allocator-aware, return whether the allocator of the
    // specified 'obj1' matches that of specified 'obj2'; otherwise return
    // 'true'.
{
    return AllocatorTestUtil<TYPE>::hasSameAllocator(obj1, obj2);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#define BSLALG_NOTHROWMOVABLEWRAPPER_TEST_TYPES                               \
    TrackableValue, TrackableValueWithAlloc

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TEST_TYPE>
class TestDriver {
    // This class template provides a namespace for testing the
    // 'bslalg::NothrowMovableWrapper' type.

  private:
    // PRIVATE TYPES
    typedef TEST_TYPE ValueType;
        // ValueType under test.

    typedef bslalg::NothrowMovableWrapper<ValueType> Obj;
        // Type under test.

    typedef bslalg::ConstructorProxy<Obj> ObjWithAllocator;
        // Wrapper for 'Obj' whose constructor takes an allocator.

    typedef bslalg::ConstructorProxy<ValueType> ValWithAllocator;
        // Wrapper for 'ValueType' whose constructor takes an allocator.

  public:
    static void testCase7();
        // ACCESSORS AND MANIPULATORS

    static void testCase6();
        // COPY AND MOVE CONSTRUCTORS

    static void testCase5();
        // CONSTRUCTION FROM 'TYPE'

    static void testCase4();
        // DEFAULT CONSTRUCTION

    static void testCase3();
        // TRAITS AND TYPEDEFS
};

template <class TYPE>
void TestDriver<TYPE>::testCase7()
{
    // --------------------------------------------------------------------
    //  ACCESSORS AND MANIPULATORS
    //
    //  This test checks the functionality of accessors and manipulators.
    //
    // Concerns:
    //: 1 That the 'unwrap' method returns a reference to the wrapped object.
    //:   If invoked on a const wrapper, the returned reference is const
    //:   qualified.
    //:
    //: 2 That the conversion operators return the same thing as the 'unwrap'
    //:   method
    //:
    //: 3 That the 'get_allocator' method returns the allocator used to
    //:   created the wrapped object, if the wrapped object type is
    //:   allocator-aware.
    //:
    // Plan:
    //: 1 Create a wrapper object of 'TYPE'.  Using the 'unwrap' method, check
    //:   that the returned reference matches the wrapped object.  [C-1]
    //:
    //: 2 In step 1, call the 'unwrap' method through a const reference to
    //:   the wrapper object.  [C-1]
    //:
    //: 3 Repeat step 1 using the conversion operator.  [C-2]
    //:
    //: 4 Repeat step 2 using the conversion operator.  [C-2]
    //:
    //: 5 If 'TYPE' is allocator-aware, verify that 'get_allocator' return the
    //:   allocator specified at wrapped construction time.  [C-3]
    //
    // Testing:
    //   ValueType& unwrap();
    //   operator ValueType&();
    //   ValueType const& unwrap() const;
    //   operator ValueType const&() const;
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    {
        ValueType        val(3);
        ObjWithAllocator objBuffer(val, &oa);
        Obj&             mX = objBuffer.object();
        const Obj&       x  = mX;

        ASSERT(checkAllocator(x, &oa));
        ASSERT(checkAllocator(mX, &oa));

        ASSERT(x.unwrap() == val);
        ASSERT(mX.unwrap() == val);

        ValueType val2(6);
        mX.unwrap() = val2;

        ASSERT(checkAllocator(x, &oa));
        ASSERT(checkAllocator(mX, &oa));

        ASSERT(x.unwrap() == val2);
        ASSERT(mX.unwrap() == val2);

        ValueType val3(mX);
        ASSERT(val3 == val2);

        ValueType val4(x);
        ASSERT(val4 == val2);
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase6()
{
    // --------------------------------------------------------------------
    //  COPY AND MOVE CONSTRUCTORS
    //   This test will verify that 'NothrowMovableWrapper<TYPE>' object
    //   constructed from an object of 'NothrowMovableWrapper' is as expected.
    //
    // Concerns:
    //: 1 'NothrowMovableWrapper<TYPE>' object constructed from an object of
    //:   'NothrowMovableWrapper', has the value of the original object.
    //:
    //: 2 If the argument to the constructor is an lvalue or a movable ref to
    //:   a const object, the 'ValueType' object is copy constructed.  If
    //:   the argument to the constructor is a movable ref to a non const
    //:   object, the 'ValueType' object is move constructed.
    //:
    //: 3 If 'TYPE' is allocator-aware and no allocator was provided at
    //:   construction time, the 'ValueType' object will use the default
    //:   allocator if the original was copied from, and the allocator of the
    //:   original object if the original object was moved from.
    //:
    //: 4 If 'TYPE' is allocator-aware and an allocator was provided at
    //:   construction time, the 'ValueType' object will use the provided
    //:   allocator.
    //
    // Plan:
    //: 1 Construct an object of 'NothrowMovableWrapper<TYPE>' using an lvalue
    //:   of 'NothrowMovableWrapper<TYPE>' as the source object.  Verify that
    //:   the value of the object wrapped in the resulting
    //:   'NothrowMovableWrapper' compares equal to the object wrapped in the
    //:   source object.  [C-1]
    //:
    //: 2 In Step 1, Verify that the 'ValueType' object in the resulting
    //:   'NothrowMovableWrapper<TYPE>' has been created by copy construction.
    //:   [C-2]
    //:
    //: 3 In step 1, If 'TYPE' is allocator-aware, verify that the 'ValueType'
    //:   object of the 'NothrowMovableWrapper<TYPE>' has been constructed
    //:   using the default allocator.  [C-3]
    //:
    //: 4 Repeat step 1 using an rvalue as the source object.  Verify that the
    //:   'ValueType' object in the resulting 'NothrowMovableWrapper<TYPE>'
    //:   has been created by move construction.  If 'TYPE' is allocator-aware,
    //:   verify that the 'ValueType' object of the
    //:   'NothrowMovableWrapper<TYPE>' has been constructed using the
    //:   allocator of the original object.[C-1][C-2][C-3]
    //:
    //: 5 Repeat step 1 using a const rvalue as the source object.  Verify that
    //:   the 'ValueType' object in the resulting 'NothrowMovableWrapper<TYPE>'
    //:   has been created by copy construction.  If 'TYPE' is allocator-aware,
    //:   verify that the 'ValueType' object of the
    //:   'NothrowMovableWrapper<TYPE>' has been constructed using the default
    //:   allocator.[C-1][C-2][C-3]
    //:
    //: 6 If 'TYPE' is allocator-aware, repeat step 1 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created by copy
    //:   construction and with the provided allocator.  [C-1][C-2][C-4]
    //:
    //: 7 If 'TYPE' is allocator-aware, repeat step 4 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created with the
    //:   provided allocator.  [C-1][C-2][C-4]
    //:
    //: 8 If 'TYPE' is allocator-aware, repeat step 5 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created with the
    //:   provided allocator.  [C-1][C-2][C-4]
    //
    // Testing:
    //   NothrowMovableWrapper(const NothrowMovableWrapper& original);
    //   NothrowMovableWrapper(bsl::allocator_arg_t         ,
    //                         const allocator_type&        alloc,
    //                         const NothrowMovableWrapper& original);
    //   NothrowMovableWrapper(bslmf::MovableRef<NothrowMovableWrapper>
    //   original);
    //   NothrowMovableWrapper(bsl::allocator_arg_t                  ,
    //                         const allocator_type&                 alloc,
    //                         bslmf::MovableRef<NothrowMovableWrapper>
    //                         original);
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
        printf("\tNon allocator-extended construction from "
               "'NothrowMovableWrapper'.\n");
    {
        ValueType        val(3);
        ObjWithAllocator sourceBuffer(val, &oa);

        sourceBuffer.object().unwrap().resetMoveCopiedFlags();

        Obj& source  = sourceBuffer.object();
        Obj  x(source);
        ASSERT(x.unwrap() == val);
        ASSERT(checkAllocator(x, &da));
        ASSERT(!x.unwrap().isMoved());
        ASSERT(x.unwrap().isCopied());

        Obj x2(MoveUtil::move(source));
        ASSERT(x2.unwrap() == val);
        ASSERT(checkAllocator(x2, &oa));
        ASSERT(x2.unwrap().isMoved());
        ASSERT(!x2.unwrap().isCopied());
    }
    {
        ValueType        val(3);
        ObjWithAllocator sourceBuffer(val, &oa);

        sourceBuffer.object().unwrap().resetMoveCopiedFlags();

        const Obj& SOURCE  = sourceBuffer.object();
        Obj x(MoveUtil::move(SOURCE));
        ASSERT(x.unwrap() == val);
        ASSERT(checkAllocator(x, &da));
        ASSERT(!x.unwrap().isMoved());
        ASSERT(x.unwrap().isCopied());
    }
    if (veryVerbose)
        printf("\tallocator-extended construction from "
               "'NothrowMovableWrapper'.\n");
    {
        ValueType        val(3);
        ObjWithAllocator sourceBuffer(val, &oa);
        sourceBuffer.object().unwrap().resetMoveCopiedFlags();

        Obj&             source  = sourceBuffer.object();
        ObjWithAllocator xBuffer(source, &ta);
        Obj&             x = xBuffer.object();
        ASSERT(x.unwrap() == val);
        ASSERT(checkAllocator(x, &ta));
        ASSERT(!x.unwrap().isMoved());
        ASSERT(x.unwrap().isCopied());

        ObjWithAllocator x2Buffer(MoveUtil::move(source), &ta);
        Obj&             x2 = x2Buffer.object();
        ASSERT(x2.unwrap() == val);
        ASSERT(checkAllocator(x2, &ta));
        ASSERT(x2.unwrap().isMoved());
        ASSERT(!x2.unwrap().isCopied());
    }
    {
        ValueType        val(3);
        ObjWithAllocator sourceBuffer(val, &oa);
        sourceBuffer.object().unwrap().resetMoveCopiedFlags();

        const Obj&       SOURCE = sourceBuffer.object();
        ObjWithAllocator xBuffer(MoveUtil::move(SOURCE), &ta);
        Obj&             x = xBuffer.object();
        ASSERT(x.unwrap() == val);
        ASSERT(checkAllocator(x, &ta));
        ASSERT(!x.unwrap().isMoved());
        ASSERT(x.unwrap().isCopied());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase5()
{
    // --------------------------------------------------------------------
    //  CONSTRUCTION FROM 'TYPE'
    //   This test will verify that 'NothrowMovableWrapper<TYPE>' constructed
    //   from an object of 'TYPE' is as expected.
    //
    // Concerns:
    //: 1 'NothrowMovableWrapper<TYPE>' object constructed from an object of
    //:   'TYPE', has the value of the original 'TYPE' object.
    //:
    //: 2 If the argument to the constructor is an lvalue or a movable ref to
    //:   a const object, the 'ValueType' object is copy constructed.  If
    //:   the argument to the constructor is a movable ref to a non const
    //:   object, the 'ValueType' object is move constructed.
    //:
    //: 3 If 'TYPE' is allocator-aware and no allocator was provided at
    //:   construction time, the 'ValueType' object will use the default
    //:   allocator if the original was copied from, and the allocator of the
    //:   original object if the original object was moved from.
    //:
    //: 4 If 'TYPE' is allocator-aware and allocator was provided at
    //:   construction time, the 'ValueType' object will use the provided
    //:   allocator.
    //:
    //: 5 'bsl::Function_NothrowWrapper<TYPE>' is a synonym for
    //:   'bslstl::NothrowMovableWrapper<TYPE>' when constructed with one
    //:   argument of type (convertible to) 'TYPE'.
    //
    // Plan:
    //: 1 Construct an object of 'NothrowMovableWrapper<TYPE>' from an lvalue
    //:   of 'TYPE'.  Verify that the values wrapped in the resulting wrapper
    //:   compares equal to the source object [C-1]
    //:
    //: 2 In Step 1, verify that the 'ValueType' object in the resulting
    //:   'NothrowMovableWrapper<TYPE>' has been created by copy construction.
    //:   [C-2]
    //:
    //: 3 In step 1, If 'TYPE' is allocator-aware, verify that the 'ValueType'
    //:   object of the 'NothrowMovableWrapper<TYPE>' has been constructed
    //:   using the default allocator.  [C-3]
    //:
    //: 4 Repeat step 1 using an rvalue as the source object.  Verify that the
    //:   'ValueType' object in the resulting 'NothrowMovableWrapper<TYPE>'
    //:   has been created by move construction. If 'TYPE' is allocator-aware,
    //:   verify that the 'ValueType' object of the
    //:   'NothrowMovableWrapper<TYPE>' has been constructed using the
    //:   allocator of the original object.[C-1][C-2][C-3]
    //:
    //: 5 Repeat step 1 using a const rvalue as the source object.  Verify that
    //:   the 'ValueType' object in the resulting 'NothrowMovableWrapper<TYPE>'
    //:   has been created by copy construction.  If 'TYPE' is allocator-aware,
    //:   verify that the 'ValueType' object of the
    //:   'NothrowMovableWrapper<TYPE>' has been constructed using the default
    //:   allocator.[C-1][C-2][C-3]
    //:
    //: 6 If 'TYPE' is allocator-aware, repeat step 1 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created by copy
    //:   construction and with the provided allocator.  [C-1][C-2][C-4]
    //:
    //: 7 If 'TYPE' is allocator-aware, repeat step 4 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created with the
    //:   provided allocator.  [C-1][C-2][C-4]
    //:
    //: 8 If 'TYPE' is allocator-aware, repeat step 5 using the allocator
    //:   extended constructor.  Verify that the 'ValueType' object in the
    //:   resulting 'NothrowMovableWrapper<TYPE>' has been created with the
    //:   provided allocator.  [C-1][C-2][C-4]
    //:
    //: 9 Repeat step one with 'bsl::Function_NothrowWrapper' instead of
    //:   'bslstl::NothrowMovableWrapper'.  [C-5]
    //
    // Testing:
    //   NothrowMovableWrapper(const TYPE& val);
    //   NothrowMovableWrapper(bsl::allocator_arg_t,
    //                         const allocator_type& allocator,
    //                         const TYPE&           val);
    //   NothrowMovableWrapper(bslmf::MovableRef<TYPE> val)
    //                         BSLS_KEYWORD_NOEXCEPT;
    //   NothrowMovableWrapper(bsl::allocator_arg_t    ,
    //                         const allocator_type&   allocator,
    //                         bslmf::MovableRef<TYPE> val);
    //   'bsl::Function_NothrowWrapper' compatibility
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
        printf("\tNon allocator-extended construction from 'TYPE'.\n");
    {
        ValWithAllocator sourceBuf(3, &oa);
        ValWithAllocator sourceCopyBuf(3, &oa);
        ValueType&       source = sourceBuf.object();
        ValueType&       sourceCopy = sourceCopyBuf.object();

        ValueType expected(source);
        Obj       x(source);
        ASSERT(expected == x.unwrap());
        ASSERT(hasSameAllocator(expected, x.unwrap()));
        ASSERT(expected.isMoved() == x.unwrap().isMoved());
        ASSERT(expected.isCopied() == x.unwrap().isCopied());

        ValueType expected2(MoveUtil::move(source));
        Obj       x2(MoveUtil::move(sourceCopy));
        ASSERT(expected2 == x2.unwrap());
        ASSERT(hasSameAllocator(expected2, x2.unwrap()));
        ASSERT(expected2.isMoved() == x2.unwrap().isMoved());
        ASSERT(expected2.isCopied() == x2.unwrap().isCopied());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ValueType expected3(source);
        bsl::Function_NothrowWrapper<ValueType> x3(source);
        ASSERT((bsl::is_same<Obj,
                             bsl::Function_NothrowWrapper<ValueType> >::value));
        ASSERT(expected3 == x3.unwrap());
        ASSERT(hasSameAllocator(expected3, x3.unwrap()));
        ASSERT(expected3.isMoved() == x3.unwrap().isMoved());
        ASSERT(expected3.isCopied() == x3.unwrap().isCopied());
#endif
    }
    {
        ValWithAllocator sourceBuf(3, &oa);
        ValWithAllocator sourceCopyBuf(3, &oa);
        ValueType&       source = sourceBuf.object();
        const ValueType& SOURCE = source;

        ValueType expected(MoveUtil::move(SOURCE));
        Obj       x(MoveUtil::move(SOURCE));

        ASSERT(expected == x.unwrap());
        ASSERT(hasSameAllocator(expected, x.unwrap()));
        ASSERT(expected.isMoved() == x.unwrap().isMoved());
        ASSERT(expected.isCopied() == x.unwrap().isCopied());
    }
    if (veryVerbose)
        printf("\tallocator-extended construction from 'TYPE'.\n");
    {
        ValWithAllocator sourceBuf(3, &oa);
        ValWithAllocator sourceCopyBuf(3, &oa);
        ValueType&       source = sourceBuf.object();
        ValueType&       sourceCopy = sourceCopyBuf.object();

        ValWithAllocator expectedBuffer(source, &ta);
        ValueType&       expected = expectedBuffer.object();
        ObjWithAllocator xBuffer(source, &ta);
        Obj&             x = xBuffer.object();
        ASSERT(expected == x.unwrap());
        ASSERT(hasSameAllocator(expected, x.unwrap()));
        ASSERT(expected.isMoved() == x.unwrap().isMoved());
        ASSERT(expected.isCopied() == x.unwrap().isCopied());

        ValWithAllocator expected2Buffer(MoveUtil::move(source), &ta);
        ValueType&       expected2 = expected2Buffer.object();
        ObjWithAllocator x2Buffer(MoveUtil::move(sourceCopy), &ta);
        Obj&             x2 = x2Buffer.object();
        ASSERT(expected2 == x2.unwrap());
        ASSERT(hasSameAllocator(expected2, x2.unwrap()));
        ASSERT(expected2.isMoved() == x2.unwrap().isMoved());
        ASSERT(expected2.isCopied() == x2.unwrap().isCopied());
    }
    {
        ValWithAllocator sourceBuf(3, &oa);
        ValWithAllocator sourceCopyBuf(3, &oa);
        ValueType&       source = sourceBuf.object();

        const ValueType SOURCE = source;

        ValWithAllocator expectedBuffer(MoveUtil::move(SOURCE), &ta);
        ValueType&       expected = expectedBuffer.object();
        ObjWithAllocator xBuffer(MoveUtil::move(SOURCE), &ta);
        Obj&             x = xBuffer.object();
        ASSERT(expected == x.unwrap());
        ASSERT(hasSameAllocator(expected, x.unwrap()));
        ASSERT(expected.isMoved() == x.unwrap().isMoved());
        ASSERT(expected.isCopied() == x.unwrap().isCopied());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4()
{
    // --------------------------------------------------------------------
    //  DEFAULT CONSTRUCTION
    //   This test will verify that default constructed
    //   'NothrowMovableWrapper' contains a default constructed 'ValueType'
    //   object.
    //
    // Concerns:
    //: 1 Default constructed 'NothrowMovableWrapper' contains a default
    //:   constructed 'ValueType' object.
    //:
    //: 2 If 'TYPE' is allocator-aware and no allocator was provided at
    //:   construction time, the 'ValueType' object will use the default
    //:   allocator.
    //:
    //: 3 If 'TYPE' is allocator-aware and allocator was provided at
    //:   construction time, the 'ValueType' object will use the provided
    //:   allocator.
    //
    // Plan:
    //: 1 Default construct an object of 'TYPE' and a
    //:   'NothrowMovableWrapper<TYPE>' object.  Verify that the values of the
    //:   two objects match.  [C-1]
    //:
    //: 2 If 'TYPE' is allocator-aware, verify that the 'ValueType' object of
    //:   the 'NothrowMovableWrapper<TYPE>' has been constructed using the
    //:   default allocator.  [C-2]
    //:
    //: 3 If 'TYPE' is allocator-aware, repeat step 1 using the allocator
    //:   extended default constructor.  [C-1]
    //:
    //: 4 In step 3, verify that the 'ValueType' object of the
    //:   'NothrowMovableWrapper<TYPE>' has been constructed using the
    //:   provided allocator.  [C-3]
    //
    // Testing:
    //  NothrowMovableWrapper();
    //  NothrowMovableWrapper(bsl::allocator_arg_t  ,
    //                        const allocator_type& allocator);
    // --------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
        printf("\tNon allocator-extended default construction.\n");
    {
        TYPE val = TYPE();
        Obj  x;
        ASSERT(val == x.unwrap());
        ASSERT(hasSameAllocator(val, x.unwrap()));
    }
    if (veryVerbose)
        printf("\tallocator-extended default construction.\n");
    {
        ValWithAllocator valBuffer(&oa);
        ValueType&       val = valBuffer.object();
        ObjWithAllocator objBuffer(&oa);
        Obj&             x = objBuffer.object();
        ASSERT(val == x.unwrap());
        ASSERT(hasSameAllocator(val, x.unwrap()));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase3()
{
    // --------------------------------------------------------------------
    // TRAITS AND TYPEDEFS
    //
    //  This test checks the nested 'NothrowMovableWrapper' traits  and
    //  pubic typedefs.
    //
    // Concerns:
    //: 1 If and only if 'TYPE' is allocator-aware,
    //:   'NothrowMovableWrapper<TYPE>' is allocator-aware and satisfies
    //:   'UsesAllocatorArgT' trait.
    //:
    //: 2 If and only if 'TYPE' is 'IsBitwiseMoveable',
    //:   'NothrowMovableWrapper<TYPE>' is 'IsBitwiseMoveable'.
    //:
    //: 3 'NothrowMovableWrapper<TYPE>::ValueType' is 'TYPE'.
    //:
    //: 4 If and only if 'TYPE' is allocator-aware,
    //:   'NothrowMovableWrapper<TYPE>::allocator_type' is
    //:   'bsl::allocator<char>'
    //:
    //: 5 'NothrowMovableWrapper<TYPE>' is always no-throw move constructible.
    //
    // Plan:
    //: 1 Verify that 'UsesBslmaAllocator' trait for 'TYPE' and
    //:   'NothrowMovableWrapper<TYPE>' has the same value.  Check for 'TYPE1'
    //:   where the trait is true and 'TYPE2' where the trait is false. [C-1]
    //:
    //: 2 Verify that 'UsesAllocatorArgT' trait for
    //:   'NothrowMovableWrapper<TYPE>' has the same value as
    //:   'UsesBslmaAllocator' trait for 'TYPE'.  Check for 'TYPE1' where the
    //:   trait is true and 'TYPE2' where the trait is false.  [C-1]
    //:
    //: 3 Verify that 'IsBitwiseMoveable' trait for 'TYPE' and
    //:   'NothrowMovableWrapper<TYPE>' has the same value.  Check for 'TYPE1'
    //:   where the trait is true and 'TYPE2' where the trait is false.  [C-2]
    //:
    //: 4 Verify that 'NothrowMovableWrapper<TYPE>::ValueType' is 'TYPE'.
    //:   [C-3]
    //:
    //: 5 Verify that 'NothrowMovableWrapper<TYPE>::allocator_type' is
    //:   'bsl::allocator<char>' if 'TYPE' is allocator aware.  [C-4]
    //:
    //: 6 Verify that 'bsl::is_nothrow_move_constructible' trait for
    //:   'NothrowMovableWrapper<TYPE>' is derived from 'bsl::true_type'.
    //:   Check for 'TYPE1' where the trait is true and 'TYPE2' where the trait
    //:   is false.  [C-5]
    //
    // Testing:
    //   bsl::is_nothrow_move_constructible<NothrowMovableWrapper>
    //   BloombergLP::bslma::UsesBslmaAllocator<NothrowMovableWrapper>
    //   BloombergLP::bslmf::UsesAllocatorArgT<NothrowMovableWrapper>
    //   BloombergLP::bslmf::IsBitwiseMoveable<NothrowMovableWrapper>
    //   typedef TYPE ValueType;
    //   typedef ...  allocator_type;
    // --------------------------------------------------------------------
    {
        ASSERT(BloombergLP::bslma::UsesBslmaAllocator<Obj>::value ==
               BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value);
        ASSERT(BloombergLP::bslmf::UsesAllocatorArgT<Obj>::value ==
               BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value);
        ASSERT(BloombergLP::bslmf::IsBitwiseMoveable<Obj>::value ==
               BloombergLP::bslmf::IsBitwiseMoveable<ValueType>::value);

        ASSERT((bsl::is_same<typename Obj::ValueType, ValueType>::value));
        ASSERT((BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value &&
                bsl::is_same<typename Obj::allocator_type,
                             bsl::allocator<char> >::value) ||
               !(BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value));

        ASSERT((bsl::is_nothrow_move_constructible<Obj>::value));
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example in the component documentation compiles
        //:   and runs correctly.
        //
        // Plan:
        //: 1 Copy the usage example directly into the test driver.
        //
        // Testing
        //    USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");

        usageExample1();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // DESTRUCTOR
        //
        //  This test checks that the destructor of the wrapped object is
        //  invoked when 'NothrowMovableWrapper<TYPE>' object is destroyed.
        //
        // Concerns:
        //: 1 Destroying a 'NothrowMovableWrapper<TYPE>' object invokes the
        //:   destructor of the wrapped object.
        //
        // Plan:
        //: 1 Create a 'NothrowMovableWrapper<TYPE>' object of a 'TYPE' which
        //:   check for destructor invocation.  Check the destructor of 'TYPE'
        //:   is invoked when 'NothrowMovableWrapper<TYPE>' object is
        //:   destroyed.
        //
        // Testing:
        //   ~NothrowMovableWrapper<TYPE>()
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nDESTRUCTOR"
                   "\n==========\n");

        int numDestructorCalls = TrackableValue::s_numDestructorCalls;
        {
            bslalg::NothrowMovableWrapper<TrackableValue> source;
        }
        ASSERT(numDestructorCalls+1 == TrackableValue::s_numDestructorCalls);

      } break;
      case 7: {

        // --------------------------------------------------------------------
        //  ACCESSORS AND MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nACCESSORS AND MANIPULATORS"
                   "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLALG_NOTHROWMOVABLEWRAPPER_TEST_TYPES);
      } break;
      case 6: {

        // --------------------------------------------------------------------
        //  COPY AND MOVE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCOPY AND MOVE CONSTRUCTORS"
                   "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLALG_NOTHROWMOVABLEWRAPPER_TEST_TYPES);
      } break;
      case 5: {

        // --------------------------------------------------------------------
        //  CONSTRUCTION FROM 'TYPE'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCONSTRUCTION FROM 'TYPE'"
                   "\n========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase5,
                      BSLALG_NOTHROWMOVABLEWRAPPER_TEST_TYPES);
      } break;
      case 4: {

        // --------------------------------------------------------------------
        //  DEFAULT CONSTRUCTION
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nDEFAULT CONSTRUCTION"
                   "\n====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        //  TRAITS AND TYPEDEFS
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTRAITS AND TYPEDEFS"
                   "\n===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        //
        //  This test checks the 'noexcept' specification of move constructor.
        //
        // Concerns:
        //: 1 The 'noexcept' specification has been applied to the move
        //:   constructor
        //
        // Plan:
        //: 1 Apply the unary 'noexcept' operator to move constructor
        //
        // Testing:
        //   CONCERN: move constructor is 'noexcept'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'noexcept' SPECIFICATION"
                   "\n========================\n");

        bslalg::NothrowMovableWrapper<TrackableValue> source;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                   bslalg::NothrowMovableWrapper<TrackableValue>(
                       MoveUtil::move(source))));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the basic 'NothrowMovableWrapper' functionality works as
        // intended.
        //
        // Plan:
        //: 1 Exercise basic methods of 'NothrowMovableWrapper'
        //
        // Testing:
        //   BREATHING TEST
        //
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        ASSERT(!bsl::is_nothrow_move_constructible<TrackableValue>::value);
        ASSERT(bsl::is_nothrow_move_constructible<
               bslalg::NothrowMovableWrapper<TrackableValue> >::value);

        bslalg::NothrowMovableWrapper<TrackableValue>        ntmw1;
        const bslalg::NothrowMovableWrapper<TrackableValue>& NTMW1 = ntmw1;
        ASSERT(0 == NTMW1.unwrap().value());

        ntmw1.unwrap() = 4;
        ASSERT(4 == NTMW1.unwrap().value());

        bslalg::NothrowMovableWrapper<TrackableValue>        ntmw2(ntmw1);
        const bslalg::NothrowMovableWrapper<TrackableValue>& NTMW2 = ntmw2;
        ASSERT(4 == NTMW1.unwrap().value());
        ASSERT(4 == NTMW2.unwrap().value());
        ASSERT(!NTMW1.unwrap().isMovedFrom());
        ASSERT(!NTMW2.unwrap().isMoved());
        ASSERT(NTMW2.unwrap().isCopied());

        bslalg::NothrowMovableWrapper<TrackableValue> ntmw3(
                                           bslmf::MovableRefUtil::move(ntmw1));

        const bslalg::NothrowMovableWrapper<TrackableValue>& NTMW3 = ntmw3;
        ASSERT(4 == NTMW3.unwrap().value());
        ASSERT(NTMW1.unwrap().isMovedFrom());
        ASSERT(NTMW3.unwrap().isMoved());
        ASSERT(!NTMW3.unwrap().isCopied());

        bslalg::NothrowMovableWrapper<int (*)(int)> funcWrap1(&simpleFunction);

        int (*&pfunc1)(int) = funcWrap1.unwrap();
        ASSERT(pfunc1 == &simpleFunction);
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE '%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

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
