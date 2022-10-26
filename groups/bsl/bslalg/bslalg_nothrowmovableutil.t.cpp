// bslalg_nothrowmovableutil.t.cpp                                    -*-C++-*-
#include <bslalg_nothrowmovableutil.h>

#include <bslalg_constructorproxy.h>

#include <bslma_defaultallocatorguard.h>
#include <bsla_maybeunused.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component  under test provides a utility struct,
// 'bslalg::NothrowMovableUtil', that provides a namespace for static functions
// 'wrap' and 'unwrap' with a uniform interface such that unwrapping an object
// that is not wrapped or wrapping an object that is already wrapped are
// no-ops.  This utility struct also provides type traits for determining
// whether a type is wrapped and for deducing the type of the wrapped and
// unwrapped object.  The tests will verify that the 'wrap' and 'unwrap'
// utility behave as expected and that they preserve the constness and the
// value type category of the wrapped object.  Additionally, the tests will
// verify that traits correctly determine whether the type is a wrapped or
// unwrapped type, and that the deduced wrapped and unwrapped type are correct.
//-----------------------------------------------------------------------------
//
// TRAITS
// [ 2] IsWrapped
// [ 2] WrappedType
// [ 2] UnwrappedType
//
// STATIC METHODS
// [ 3] wrap(TYPE&)
// [ 3] wrap(TYPE const&)
// [ 3] wrap(BSLMF_MOVABLEREF_DEDUCE(TYPE))
// [ 3] wrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE)
// [ 4] unwrap(TYPE&)
// [ 4] unwrap(TYPE const&)
// [ 4] unwrap(BSLMF_MOVABLEREF_DEDUCE(TYPE))
// [ 4] unwrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE))
//
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

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

typedef void (*FunctionPtrType)();
typedef void(FunctionType)();

typedef bslalg::NothrowMovableWrapper<int>             WrappedInt;
typedef bslalg::NothrowMovableWrapper<const int>       WrappedConstInt;
typedef bslalg::NothrowMovableWrapper<FunctionPtrType> WrappedFunctionPtrType;

#define BSLALG_NOTHROWMOVABLEUTIL_TEST_TYPES                                  \
    int, const int, FunctionPtrType, FunctionType, WrappedInt,                \
        WrappedConstInt, WrappedFunctionPtrType

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
    mutable int d_valueAndFlags;

    TrackableValue& setValue(int  v,
                             bool movedFlag  = false,
                             bool copiedFlag = false);
        // Set the value to the specified 'v'. Set the 'isMoved()' flag to the
        // optionally-specified 'movedFlag' bit and the 'isCopied()' flag to
        // the optionally-specified 'copiedFlag' bit.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TrackableValue, bslmf::IsBitwiseMoveable);

    // Constants
    enum {
        e_MIN = INT_MIN / e_VALUE_MULTIPLIER,
        e_MAX = INT_MAX / e_VALUE_MULTIPLIER
    };

    // CREATORS
    TrackableValue(int v = 0)  // IMPLICIT
        // Set value to the optionally-specified 'v' and set 'isMoved()' and
        // 'isCopied()' to false.
    {
        setValue(v);
    }

    TrackableValue(const TrackableValue& original)
        // Copy value from the specified 'original', set 'isMoved()' to false
        // and
        //  'isCopied()' to 'true'.
    {
        setValue(original.value(), false, true);
    }

    TrackableValue(bslmf::MovableRef<TrackableValue> original)
        // Move value from the specified 'original', set 'isMoved()' to true,
        // and set 'isCopied()' to 'original.isCopied()', then set 'original'
        // to the moved-from state.
    {
        *this = bslmf::MovableRefUtil::move(original);
    }

    //! ~TrackableValue() = default;

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
        // Move value from the specified 'rhs', set 'isMoved()' to true, and
        // set 'isCopied()' to 'rhs.isCopied()', then assign 'rhs' the value
        // 'e_MOVED_FROM_VAL' and return '*this'.

    void setIsCopiedRaw(bool);
    void setIsMovedRaw(bool);
    void setValueRaw(int);
        // Set the constituent parts of this object without modifying the other
        // parts.  It is up to the caller to ensure that the flags are set
        // consistently.

    void swap(TrackableValue& other);
        // Exchange the values AND 'isCopied()' flags of '*this' and specified
        // 'other', then set the 'isMoved()' flag of both to true.

    // ACCESSORS
    bool isCopied() const { return d_valueAndFlags & e_COPIED_FLAG; }
    bool isMoved() const { return d_valueAndFlags & e_MOVED_FLAG; }
    bool isMovedFrom() const { return d_valueAndFlags == e_MOVED_FROM_STATE; }
        // Return the state of this object

    void resetMoveCopiedFlags() const
        // Set 'isMoved()' and 'isCopied() to false.
    {
        d_valueAndFlags &= e_VALUE_MASK;
    }

    int value() const { return d_valueAndFlags / e_VALUE_MULTIPLIER; }
        // Return the state of this object
};
// IMPLEMENTATION OF TrackableValue

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

void TrackableValue::swap(TrackableValue& other)
{
    // Don't use std::swap<int> because don't want to #include <algorithm>
    int tmp               = d_valueAndFlags;
    d_valueAndFlags       = other.d_valueAndFlags;
    other.d_valueAndFlags = tmp;
    d_valueAndFlags |= e_MOVED_FLAG;
    other.d_valueAndFlags |= e_MOVED_FLAG;
}

// ACCESSORS

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

    void setValue(int v) { d_trackable = v; }
    void resetMoveCopiedFlags() const
        // Set 'isMoved()' and 'isCopied() to false.
    {
        d_trackable.resetMoveCopiedFlags();
    }

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

TrackableValue VTV(6);
int            VI(6);


//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace {

///Example 1
///- - - - -
// In this example, we define a class template, 'CountedType<TYPE>', a wrapper
// around 'TYPE' that counts the count member along with the single value
// member:
//..
template <class TYPE>
class CountedType {
    // PUBLIC CLASS DATA
    static int s_count;

    // PUBLIC DATA
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
    CountedType(const TYPE& val);  // IMPLICIT
        // Construct 'CountedType' from the specified 'val'.

    CountedType(const CountedType& original);  // IMPLICIT
        // Copy construct '*this' from the specified 'original' object.

    CountedType(bslmf::MovableRef<CountedType> original);  // IMPLICIT
        // Move construct '*this' from 'original'.  If an exception is thrown,
        // by the constructor for 'TYPE' 'original' is unchanged.

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
// Next, we implement the value constructor and copy constructor, which simply
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
// exception thrown by 'TYPE''s move constructor would leave 'original' in a
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
    SomeType(int v = 0)
    : d_value(v)
    {
    }  // IMPLICIT
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
//  void main()
//  {
//      CountedType<SomeType> obj1(1);
//      CountedType<SomeType> obj2(bslmf::MovableRefUtil::move(obj1));
//      assert(1 == obj1.value().value());  // Copied, not moved from
//      assert(1 == obj2.value().value());
//..
// For the purpose of this example, we can be sure that 'SomeThing' will not
// throw on move, at least not in our application.  In order to obtain the
// expected move optimization, we next wrap our 'SomeType in a
// 'bslalg::NothrowMovableWrapper':
//..
//      CountedType<bslalg::NothrowMovableWrapper<SomeType> >
//          obj3(SomeType(3));
//      CountedType<bslalg::NothrowMovableWrapper<SomeType> >
//          obj4(bslmf::MovableRefUtil::move(obj3));
//      assert(-1 == obj3.value().unwrap().value());  // moved from
//      assert(3 == obj4.value().unwrap().value());
//  }
//..
// Note that, in the last two lines of 'main', we must call 'unwrap' in order
// to access the 'SomeType' object inside of the 'NothrowMovableWrapper'.  This
// is one situation where it would be attractive to have an overloadable
// "operator dot" so that both 'CountedThing' and 'NothrowMovableWrapper' could
// be transparent proxy types.  C++ does not have overloadable operator dot,
// but we can create a 'CountedType' that is more intelligent about the
// existance of 'NothrowMovableWrapper' and automatically unwraps values for
// the user's convenience.
//
// Rather than starting from scratch, we'll build our new counted type,
// 'CountedType2' on 'CountedType'.  We start be defining a single data member
// of type 'CountedType':
//..
template <class TYPE>
class CountedType2 {
    CountedType<TYPE> d_data;
        //..
        // Next, for convenience, we add a public data type, 'ValueType' for
        // the value stored within 'CountedType2'.  However, rather than
        // defining 'ValueType' as simply 'TYPE', we want to know if it is an
        // instantiation of 'NothrowMovableWrapper<TP>'.  If it is, we want a
        // type that represents the unwrapped 'TP' rather than the full 'TYPE'.
        // For this type transformation, we turn to the type traits defined in
        // 'bslalg::NothrowMovableUtil':"
        //..
  public:
    // TYPES
    typedef typename bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type
        ValueType;
        //..
        // Note that the 'UnwrappedType' metafunction has no affect of 'TYPE'
        // is not wrapped.
        //
        // Next, we declare (and define) the class functions, constructors, and
        // destructor, simply forwarding to the corresponding 'CountedType'
        // function, constructor, or destructor:
        //..
        // CLASS FUNCTIONS
    static int count() { return CountedType<TYPE>::count(); }

    // CREATORS
    CountedType2(const TYPE& val)  // IMPLICIT
    : d_data(val)
    {
    }
    CountedType2(const CountedType2& original)
    : d_data(original.d_data)
    {
    }
    CountedType2(bslmf::MovableRef<CountedType2> original)  // IMPLICIT
    : d_data(bslmf::MovableRefUtil::move(
          bslmf::MovableRefUtil::access(original).d_data))
    {
    }
    //..
    // Finally, we implement the 'value()' members such that the returned
    // values do not need to be unwrapped.  As in the case of the
    // 'UnwrappedType' metafunction, the 'unwrap()' function in
    // 'NothrowMovableUtil' handles both wrapped and unwrapped arguments,
    // unwrapping the latter and returning an unmodified reference to the
    // former:
    //..
    // MANIPULATORS
    ValueType& value()
    {
        return bslalg::NothrowMovableUtil::unwrap(d_data.value());
        // Alternatively: 'return d_data.value();'
    }

    // ACCESSORS
    const ValueType& value() const
    {
        return bslalg::NothrowMovableUtil::unwrap(d_data.value());
        // Alternatively: 'return d_data.value();'
    }
};
//..
// Note the alternative code for these members: A 'NothrowMovableWrapper<TP>'
// object is implicitly convertible to 'TP&', so if 'TYPE' is a
// 'NothrowMovableWrapper', the simple return statement will implicitly unwrap
// it.
//
// Using a similar example for 'CountedType2' as we used for 'CountedType', we
// see that the usage of 'CountedType2' with and without
// 'NothrowMovableWrapper' is the same:
//..
void usageExample()
{
    CountedType2<SomeType> obj1(1);
    CountedType2<SomeType> obj2(bslmf::MovableRefUtil::move(obj1));
    ASSERT(1 == obj1.value().value());  // Copied, not moved from
    ASSERT(1 == obj2.value().value());

    CountedType2<bslalg::NothrowMovableWrapper<SomeType> > obj3(SomeType(3));
    CountedType2<bslalg::NothrowMovableWrapper<SomeType> > obj4(
                                            bslmf::MovableRefUtil::move(obj3));
    ASSERT(-1 == obj3.value().value());  // moved from
    ASSERT(3 == obj4.value().value());   // No need to call 'unwrap'
}

}  // close unnamed namespace

#define TEST_IS_WRAPPED(TYPE, isWrapped)                                      \
    ASSERT((bslalg::NothrowMovableUtil::IsWrapped<TYPE>::value) ==            \
           isWrapped);

#define TEST_UNWRAPPED_TYPE(TYPE, Expected)                                   \
    ASSERT(                                                                   \
        (bsl::is_same<bslalg::NothrowMovableUtil::UnwrappedType<TYPE>::type,  \
                      Expected>::value));

#define TEST_WRAPPED_TYPE(TYPE, Expected)                                     \
    ASSERT(                                                                   \
        (bsl::is_same<bslalg::NothrowMovableUtil::WrappedType<TYPE>::type,     \
                      Expected>::value));

template <class TYPE,
          bool USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class AllocatorTestUtil {
    // This class provided test utilities that have different behaviour
    // depending on whether 'TYPE is allocator-aware or not.  The primary
    // template is for allocator-aware types.

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
    // specified 'obj' is the specified 'allocator'; otherwise return 'true'.
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

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TEST_TYPE>
class TestDriver {
    // This class template provides a namespace for testing the
    // 'bslalg::NothrowMovableUtil' when used with 'TEST_TYPE'.

  public:
    //  TYPES
    enum ValueCategory {
        e_LVALUE_REF,
        e_CONST_LVALUE_REF,
        e_MOVABLE_REF,
        e_CONST_MOVABLE_REF
    };

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslalg::NothrowMovableUtil Util;
        // abbreviation for utility struct

    typedef typename Util::UnwrappedType<TEST_TYPE>::type ValueType;
        // Unwrapped type

    typedef typename bsl::remove_cv<ValueType>::type NonConstValueType;
        // non const version of ValueType.  This is needed for
        // checkValueCategory overloads.

    typedef typename Util::WrappedType<TEST_TYPE>::type WrappedType;
        // Wrapped type

  public:
    static ValueCategory checkValueCategory(NonConstValueType&);
    static ValueCategory checkValueCategory(const NonConstValueType&);
    static ValueCategory checkValueCategory(
                            BloombergLP::bslmf::MovableRef<NonConstValueType>);
    static ValueCategory checkValueCategory(
                      BloombergLP::bslmf::MovableRef<const NonConstValueType>);
        // Helper functions to determine the value category of an expression.
        // Returns the 'ValueCategory' value corresponding to the value
        // category and the constness of the argument.

    static void testCase3();
        // TESTING 'wrap' METHOD

    static void testCase4();
        // TESTING 'unwrap' METHOD
};

template <class TYPE>
typename TestDriver<TYPE>::ValueCategory TestDriver<TYPE>::checkValueCategory(
                                                            NonConstValueType&)
{
    return e_LVALUE_REF;
}

template <class TYPE>
typename TestDriver<TYPE>::ValueCategory TestDriver<TYPE>::checkValueCategory(
                                                      const NonConstValueType&)
{
    return e_CONST_LVALUE_REF;
}

template <class TYPE>
typename TestDriver<TYPE>::ValueCategory TestDriver<TYPE>::checkValueCategory(
                             BloombergLP::bslmf::MovableRef<NonConstValueType>)
{
    return e_MOVABLE_REF;
}

template <class TYPE>
typename TestDriver<TYPE>::ValueCategory TestDriver<TYPE>::checkValueCategory(
                       BloombergLP::bslmf::MovableRef<const NonConstValueType>)
{
    return e_CONST_MOVABLE_REF;
}

template <class TYPE>
void TestDriver<TYPE>::testCase4()
{
    // --------------------------------------------------------------------
    // TESTING 'unwrap' METHOD
    //
    // Concerns:
    //: 1 Invoking 'unwrap' method with an argument of an unwrapped type
    //:   returns a reference to the argument.
    //:
    //: 2 Invoking 'unwrap' method with an argument of a wrapped type returns
    //:   a reference to the wrapped object.
    //:
    //: 3 That the value category of the returned reference matches the value
    //:   category of the argument.
    //:
    //: 4 That the returned reference is a const reference if the argument is
    //:   a reference to a const unwrapped object or a reference to a wrapper
    //:   of const type.  Otherwise, the returned reference is a reference to
    //:   a modifiable object.
    //
    // Plan:
    //: 1 Using a matrix of all value categories, invoke unwrap and
    //:   verify the returned reference has the correct value category and
    //:   constness using 'checkValueCategory' helper function.  [C-3][C-4]
    //:
    //: 2 In step 1, check the address of the returned object matches the
    //:   address of the unwrap argument if the argument is of an unwrapped
    //:   type, or the address of the argument's wrapped 'ValueType' object
    //:   otherwise.  [C-1], [C-2]
    //
    // Testing:
    //   unwrap(TYPE&);
    //   unwrap(TYPE const&);
    //   unwrap(BSLMF_MOVABLEREF_DEDUCE(TYPE));
    //   unwrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE));
    // --------------------------------------------------------------------

    typedef bslalg::ConstructorProxy<TYPE> TypeWithAllocator;
    bslma::TestAllocator                   da("default", veryVeryVeryVerbose);
    bslma::TestAllocator                   oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard           dag(&da);
    if (veryVerbose)
        printf("\tCalling unwrap with a non const lvalue ref.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        TYPE&             x = xBuffer.object();
        const ValueType&  val = x;
        ValueType&        unwrappedX = Util::unwrap(x);

        ASSERT(checkValueCategory(Util::unwrap(x)) ==
               (bsl::is_const<ValueType>::value ? e_CONST_LVALUE_REF
                                                : e_LVALUE_REF));
        ASSERT(BSLS_UTIL_ADDRESSOF(unwrappedX) == BSLS_UTIL_ADDRESSOF(val));
    }
    if (veryVerbose)
        printf("\tCalling unwrap with a const lvalue ref.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        const TYPE&       x = xBuffer.object();
        const ValueType&  val = x;

        const ValueType& unwrappedX = Util::unwrap(x);

        ASSERT(checkValueCategory(Util::unwrap(x)) == e_CONST_LVALUE_REF);
        ASSERT(BSLS_UTIL_ADDRESSOF(unwrappedX) == BSLS_UTIL_ADDRESSOF(val));
    }
    if (veryVerbose)
        printf("\tCalling unwrap with a non const movable ref.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        TYPE&             x   = xBuffer.object();
        const ValueType&  val = x;

        BloombergLP::bslmf::MovableRef<ValueType> unwrappedX =
            Util::unwrap(MoveUtil::move(x));

        ASSERT(checkValueCategory(Util::unwrap(MoveUtil::move(x))) ==
               (bsl::is_const<ValueType>::value ? e_CONST_MOVABLE_REF
                                                : e_MOVABLE_REF));
        ASSERT(BSLS_UTIL_ADDRESSOF(MoveUtil::access(unwrappedX)) ==
               BSLS_UTIL_ADDRESSOF(val));
    }
    if (veryVerbose)
        printf("\tCalling unwrap with a const movable ref.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        const TYPE&       x   = xBuffer.object();
        const ValueType&  val = x;

        BloombergLP::bslmf::MovableRef<const ValueType> unwrappedX =
            Util::unwrap(MoveUtil::move(x));

        ASSERT(checkValueCategory(Util::unwrap(MoveUtil::move(x))) ==
               e_CONST_MOVABLE_REF);
        ASSERT(BSLS_UTIL_ADDRESSOF(MoveUtil::access(unwrappedX)) ==
               BSLS_UTIL_ADDRESSOF(val));
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase3()
{
    // --------------------------------------------------------------------
    // TESTING 'wrap' METHOD
    //
    // Concerns:
    //: 1 Invoking 'wrap' method with an argument of an unwrapped type returns
    //:   a wrapper object containing a copy of the argument.
    //:
    //: 2 Invoking 'wrap' method with an argument of a wrapped type returns a
    //:   copy of the same object.
    //:
    //: 3 Wrapping an object of 'const TYPE' yields a
    //:   'NothrowMovableWrapper<const TYPE>'.
    //:
    //: 4 The argument is moved from if it is a movable ref to a non-const
    //:   object having a move constructor
    //
    // Plan:
    //: 1 Create an object of 'TYPE' from a 'ValueType' object.  Using the
    //:   'wrap' method, created a Wrapper object.  Verify that the Wrapper
    //:   object contains a copy of the original ValueType object.  [C-1][C-2]
    //:
    //: 2 Repeat step 1 using a const object of 'TYPE'.  [C-3]
    //:
    //: 3 Repeat step 1 invoking wrap by moving from the original object.
    //:   Verify the object inside the returned wrapper has been created by
    //:   move construction if the original object is a non const object
    //:   of a type with a move constructor.  [C-4]
    //
    // Testing:
    //   wrap(TYPE& );
    //   wrap(TYPE const&);
    //   wrap(BSLMF_MOVABLEREF_DEDUCE(TYPE));
    //   wrap(BSLMF_MOVABLEREF_DEDUCE(const TYPE));
    // --------------------------------------------------------------------

    typedef bslalg::ConstructorProxy<TYPE> TypeWithAllocator;
    bslma::TestAllocator                   da("default", veryVeryVeryVerbose);
    bslma::TestAllocator                   oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard           dag(&da);
    if (veryVerbose)
        printf("\tWrapping a non const object.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        TYPE&             x = xBuffer.object();
        ValueType         exp(3);

        const WrappedType& wrappedX = Util::wrap(x);
        // using const reference to get lifetime extension.

        ASSERT(wrappedX.unwrap() == exp);
        ASSERT(wrappedX.unwrap().isCopied());
        ASSERT(!wrappedX.unwrap().isMoved());
        ASSERT(checkAllocator(wrappedX.unwrap(), &da));
    }
    if (veryVerbose)
        printf("\tWrapping a const object.\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        const TYPE&       x = xBuffer.object();
        ValueType         exp(3);

        const WrappedType& wrappedX = Util::wrap(x);
        // using const reference to get lifetime extension.

        ASSERT(wrappedX.unwrap() == exp);
        ASSERT(wrappedX.unwrap().isCopied());
        ASSERT(!wrappedX.unwrap().isMoved());
        ASSERT(checkAllocator(wrappedX.unwrap(), &da));
    }
    if (veryVerbose)
        printf("\tWrapping a non const movable ref .\n");
    {
        TypeWithAllocator xBuffer(ValueType(3), &oa);
        TYPE&             x = xBuffer.object();
        ValueType         exp(3);
        ValueType&        val = x;
        val.resetMoveCopiedFlags();
        const WrappedType& wrappedX = Util::wrap(MoveUtil::move(x));
        // using const reference to get lifetime extension.

        // If TYPE is const, wrap will perform a copy.  Otherwise, it will do a
        // move
        ASSERT(wrappedX.unwrap() == exp);
        ASSERT(wrappedX.unwrap().isCopied() ==
               bsl::is_const<ValueType>::value);
        ASSERT(wrappedX.unwrap().isMoved() != bsl::is_const<ValueType>::value);
        ASSERT(checkAllocator(wrappedX.unwrap(),
                              (bsl::is_const<ValueType>::value ? &da : &oa)));
    }
    if (veryVerbose)
        printf("\tWrapping a const movable ref .\n");
    {
        const TYPE       x(ValueType(3));
        ValueType        exp(3);
        const ValueType& val = x;
        val.resetMoveCopiedFlags();
        const WrappedType& wrappedX = Util::wrap(MoveUtil::move(x));
        // using const reference to get lifetime extension.

        ASSERT(wrappedX.unwrap() == exp);
        ASSERT(wrappedX.unwrap().isCopied());
        ASSERT(!wrappedX.unwrap().isMoved());
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
      case 5: {
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
        // Testing:
        //    USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");

        usageExample();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'unwrap' METHOD
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'unwrap' METHOD"
                   "\n=======================\n");

        RUN_EACH_TYPE(
            TestDriver,
            testCase4,
            TrackableValue,
            TrackableValueWithAlloc,
            BloombergLP::bslalg::NothrowMovableWrapper<TrackableValue>,
            BloombergLP::bslalg::NothrowMovableWrapper<
                TrackableValueWithAlloc>,
            BloombergLP::bslalg::NothrowMovableWrapper<const TrackableValue>,
            BloombergLP::bslalg::NothrowMovableWrapper<
                const TrackableValueWithAlloc>);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'wrap' METHOD
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'wrap' METHOD"
                   "\n=====================\n");

        RUN_EACH_TYPE(
            TestDriver,
            testCase3,
            TrackableValue,
            TrackableValueWithAlloc,
            BloombergLP::bslalg::NothrowMovableWrapper<TrackableValue>,
            BloombergLP::bslalg::NothrowMovableWrapper<
                TrackableValueWithAlloc>,
            BloombergLP::bslalg::NothrowMovableWrapper<const TrackableValue>,
            BloombergLP::bslalg::NothrowMovableWrapper<
                const TrackableValueWithAlloc>);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'NothrowMovableUtil' TRAITS
        //
        //  This test checks the correct behaviour of 'NothrowMovableUtil'
        //  traits.
        //
        // Concerns:
        //: 1 'IsWrapped<TYPE>' trait is derived from 'bsl::true_type' if and
        //:   only if 'TYPE' is a specialization of 'NothrowMovableWrapper'.
        //:
        //: 2 'WrappedType<TYPE>::type' is 'NothrowMovableWrapper<TYPE>' if
        //:   'TYPE' is not a specialization of 'NothrowMovableWrapper', and
        //:   'TYPE' otherwise.
        //:
        //: 3 'UnWrappedType<TYPE>::type' is 'TYPE' if 'TYPE' is not a
        //:   specialization of 'NothrowMovableWrapper' and 'TYPE is not a
        //:   function, 'TYPE*' if TYPE is a function, and 'TYPE::ValueType'
        //:   otherwise.
        //
        // Plan:
        //: 1 Using a 'TYPE' which is not a specialization of
        //:   'NothrowMovableWrapper' check that 'IsWrapped<TYPE>' trait is
        //:   derived from 'bsl::false_type'.  [C-1]
        //:
        //: 2 In step 1, check that 'WrappedType<TYPE>::type' is
        //:   'NothrowMovableWrapper<TYPE>'.  [C-2]
        //:
        //: 3 In step 1, check that 'UnwrappedType<TYPE>::type' is 'TYPE'.
        //:   [C-3]
        //:
        //: 4 In step 3, using a function type 'TYPE', verify that
        //:   'UnwrappedType<TYPE>::type' is 'TYPE*'
        //:
        //: 5 Using a 'NothrowMovableWrapper<TYPE> as test type, check that
        //:   'IsWrapped<TYPE>' trait is derived from 'bsl::true_type'.  [C-1]
        //:
        //: 6 In step 5, check that 'WrappedType<TYPE>::type' is 'TYPE'.
        //:   [C-2]
        //:
        //: 7 In step 6, check that 'UnwrappedType<TYPE>::type' is
        //:   'TYPE::ValueType'.  [C-3]
        //
        // Testing:
        //  IsWrapped
        //  UnwrappedType
        //  WrappedType
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nTESTING 'NothrowMovableUtil' TRAITS"
                   "\n===================================\n");
        if (veryVerbose)
            printf("\tTesting isWrapped.\n");
        {
            TEST_IS_WRAPPED( int,                       false );
            TEST_IS_WRAPPED( const int,                 false );
            TEST_IS_WRAPPED( FunctionPtrType,           false );
            TEST_IS_WRAPPED( FunctionType,              false );
            TEST_IS_WRAPPED( WrappedInt,                true  );
            TEST_IS_WRAPPED( WrappedConstInt,           true  );
            TEST_IS_WRAPPED( WrappedFunctionPtrType,    true  );
        }

        if (veryVerbose)
                    printf("\tTesting UnwrappedType.\n");
        {

            TEST_UNWRAPPED_TYPE( int,                       int              );
            TEST_UNWRAPPED_TYPE( const int,                 const int        );
            TEST_UNWRAPPED_TYPE( FunctionPtrType,           FunctionPtrType  );
            TEST_UNWRAPPED_TYPE( FunctionType,              FunctionPtrType  );
            TEST_UNWRAPPED_TYPE( WrappedInt,                int              );
            TEST_UNWRAPPED_TYPE( WrappedConstInt,           const int        );
            TEST_UNWRAPPED_TYPE( WrappedFunctionPtrType,    FunctionPtrType  );
        }

        if (veryVerbose)
                    printf("\tTesting WrappedType.\n");
        {

            TEST_WRAPPED_TYPE( int,                   WrappedInt             );
            TEST_WRAPPED_TYPE( const int,             WrappedConstInt        );
            TEST_WRAPPED_TYPE( FunctionPtrType,       WrappedFunctionPtrType );
            TEST_WRAPPED_TYPE( FunctionType,          WrappedFunctionPtrType );
            TEST_WRAPPED_TYPE( WrappedInt,            WrappedInt             );
            TEST_WRAPPED_TYPE( WrappedConstInt,       WrappedConstInt        );
            TEST_WRAPPED_TYPE( WrappedFunctionPtrType,WrappedFunctionPtrType );
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the basic 'NothrowMovableUtil' functionality works as
        // intended.
        //
        // Plan:
        //: 1 Wrap and unwrap an object.
        //:
        //: 2 Apply each of the traits.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        bslalg::NothrowMovableUtil::WrappedType<int>::type intWrap =
            bslalg::NothrowMovableUtil::wrap(VI);

        ASSERT(intWrap.unwrap() == VI);

        bslalg::NothrowMovableUtil::WrappedType<TrackableValue>::type
            trackableValueWrap = bslalg::NothrowMovableUtil::wrap(VTV);

        ASSERT(trackableValueWrap.unwrap() == VTV);

        ASSERT(!(bslalg::NothrowMovableUtil_Traits<int>::IsWrapped::value));
        ASSERT((
            bsl::is_same<bslalg::NothrowMovableUtil_Traits<int>::UnwrappedType,
                         int>::value));
        ASSERT(
            (bsl::is_same<bslalg::NothrowMovableUtil_Traits<int>::WrappedType,
                          WrappedInt>::value));

        ASSERT(
            (bslalg::NothrowMovableUtil_Traits<WrappedInt>::IsWrapped::value));
        ASSERT((bsl::is_same<
                bslalg::NothrowMovableUtil_Traits<WrappedInt>::UnwrappedType,
                int>::value));
        ASSERT((bsl::is_same<
                bslalg::NothrowMovableUtil_Traits<WrappedInt>::WrappedType,
                WrappedInt>::value));

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
