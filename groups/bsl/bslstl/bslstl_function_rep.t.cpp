// bslstl_function_rep.t.cpp                                          -*-C++-*-

#include "bslstl_function_rep.h"

#include <bsla_maybeunused.h>

#include <bslalg_constructorproxy.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_bsltestutil.h>

#include <bslstl_referencewrapper.h>

#include <bsltf_templatetestfacility.h>

#include <climits>  // 'INT_MIN'
#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -AL01   // Class needs allocator() method
#pragma bde_verify -AP02   // Class needs d_allocator_p member
#pragma bde_verify -AQK01  // Need #include <c-include> for 'symbol'
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -IND01  // Possibly mis-indented line
#pragma bde_verify -MN01   // Class data members must be private

// Doesn't work ??
#pragma bde_verify append dictionary src tbd unspecialized templated
#endif

using std::printf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// This component contains a single class, 'bslstl::Function_Rep', a
// quasi-value-semantic private class holding the representation of a
// 'bsl::function'.  The salient attributes of a 'Function_Rep' are the type
// and value of the target object that it wraps.  Non-salient attributes are
// the allocator and the *invoker* *pointer*, a function pointer supplied by
// the client of this component.  The primitive manipulators are 'installFunc',
// which sets the target object and invoker and 'makeEmpty', which clears them.
//
// Although 'Function_Rep' doesn't have a (public) copy constructor or move
// constructor, it does have copy- and move-initialization functions that
// 'bsl::function' uses to implement its own copy and move operations.  This
// test driver follows the basic structure of an in-memory value-semantic type,
// to the degree that it makes sense.  Because this is a private component not
// intended for public use, a usage example is not necessary.  The main
// perturbations that we must test are:
//
//: o Does the wrapped object fit in the small object buffer?
//: o Is the wrapped object type nothrow move constructible?
//: o Is the wrapped object type bitwise movable?
//: o Does the wrapped object use an allocator?
//
// In the last case, we must test the 'Function_Rep''s allocator is
// propagated to the wrapped object.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit Function_Rep(const allocator_type& allocator);
// [ 2] ~Function_Rep();
//
// MANIPULATORS
// [ 3] void copyInit(const Function_Rep& original);
// [ 2] void installFunc(FUNC *funcPtr, GenericInvoker *invoker_p);
// [ 2] void makeEmpty();
// [ 4] void moveInit(Function_Rep *from);
// [ 5] void swap(Function_Rep& other) noexcept;
// [ 2] TP* target() const noexcept;
// [ 2] TP* targetRaw() const noexcept;
//
// ACCESSORS
// [ 2] allocator_type get_allocator() const noexcept;
// [ 2] GenericInvoker *invoker() const noexcept;
// [ 2] bool isEmpty() const noexcept;
// [ 2] bool isInplace() const noexcept;
// [ 2] const std::type_info& target_type() const noexcept;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

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

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
# define ASSERT_IS_INPLACE(OBJ, EXP) ASSERT((OBJ).isInplace() == EXP)
#else
# define ASSERT_IS_INPLACE(OBJ, EXP) ((void *) 0)
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1800
#define MSVC_2013 1
#else
#define MSVC_2013 0
#endif

namespace {

// Whitebox: Small object optimization buffer size
static const std::size_t k_SMALL_OBJECT_BUFFER_SIZE =
    sizeof(bslstl::Function_SmallObjectOptimization::InplaceBuffer);

typedef bslstl::Function_Rep                     Obj;
typedef bslstl::Function_SmallObjectOptimization Soo;

void testInvoker1()
    // Do-nothing invoker.  Only the address of this function is used.
{
}

void testInvoker2()
    // Do-nothing invoker.  Only the address of this function is used.
{
}

class SimpleFunctor {
    // A simple functor that holds an 'int' and returns it when called.
    int d_value;

  public:
    explicit SimpleFunctor(int v) : d_value(v) { }
    int operator()() const { return d_value; }

    // HIDDEN FRIENDS
    BSLA_MAYBE_UNUSED
    friend bool operator==(const SimpleFunctor& a, const SimpleFunctor& b)
        { return a() == b(); }

    BSLA_MAYBE_UNUSED
    friend bool operator!=(const SimpleFunctor& a, const SimpleFunctor& b)
        { return a() != b(); }
};

                // --------------------
                // Class TrackableValue
                // --------------------

class TrackableValue {
    // This class tracks a value through a series of move and copy operations
    // and has an easily-testable moved-from state.  An instance stores an
    // unsigned integer value and a pair of bits, one indicating if the value
    // was copied, another if it was moved (or neither or both).  When assigned
    // a value at construction, via assignment, or via a mutating operation
    // (such as +=), the 'isMoved' and 'isCopied' bits are cleared, indicating
    // that the new value has been neither moved nor copied.  When assigned a
    // new value via copy construction or copy assignment, the 'isCopied' bit
    // is set and the 'isMoved' bit is cleared.  When assigned a new value via
    // move construction, move assignment, or swap, the 'isCopied' bit is
    // transferred from the original value and the 'isMoved' bit is set.  Thus
    // a value that is copied then moved will have both bits set but a value
    // that is moved then copied has only the copy bit set.  The copy and
    // 'isMoved' bits are not salient attributes of the value and are thus not
    // used for testing equality.  When a 'TrackableValue' is the argument to a
    // move constructor or move-assignment operator, it is given the
    // 'isMovedFrom' value, which is not a bit but a singular state.

    // PRIVATE CONSTANTS
    enum {
        e_NUM_FLAGS       = 2,
        // Value is multiplied by the following value to make room for the
        // flag bits.  Multiply and divide are used to scale the value
        // because shift operations are not guaranteed to preserve sign.  The
        // compiler will almost certainly replace these multiplication and
        // divisions with more efficient arithmetic shift operations.
        e_VALUE_MULTIPLIER = 1 << e_NUM_FLAGS,
        e_FLAGS_MASK       = e_VALUE_MULTIPLIER - 1,
        e_VALUE_MASK       = ~e_FLAGS_MASK,
        e_COPIED_FLAG      = 0x01,
        e_MOVED_FLAG       = 0x02,
        e_MOVED_FROM_VAL   = 0x11111111, // Value assigned to moved-from object

        // The moved from state has a value of e_MOVED_FROM_VAL, with neither
        // flag set.
        e_MOVED_FROM_STATE = e_MOVED_FROM_VAL * e_VALUE_MULTIPLIER
    };

    // PRIVATE DATA
    int d_valueAndFlags;

    TrackableValue& setValue(int  v,
                             bool movedFlag = false,
                             bool copiedFlag = false);
        // Set the value to the specified 'v'.  Optionally specify 'movedFlag'
        // for setting the 'isMoved()' flag and optionally specify 'copiedFlag'
        // for setting the 'isCopied()' flag; otherwise each flag defaults to
        // false.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TrackableValue, bslmf::IsBitwiseMoveable);

    // Constants
    enum {
        e_MIN            = INT_MIN / e_VALUE_MULTIPLIER,
        e_MAX            = INT_MAX / e_VALUE_MULTIPLIER
    };

    // CREATORS
    TrackableValue(int v = 0)                                       // IMPLICIT
        // Create an object.  Set the value to the optionally specified 'v'
        // (default 0) and set 'isMoved()' and 'isCopied()' to false.
        { setValue(v); }

    TrackableValue(const TrackableValue& original)
        // Create an object with the value copied from the specified
        // 'original'.  Set 'isMoved()' to false and 'isCopied()' to true.
        { setValue(original.value(), false, true); }

    TrackableValue(bslmf::MovableRef<TrackableValue> original)
                                                          BSLS_KEYWORD_NOEXCEPT
        // Create an object with the value moved from the specified 'original'.
        // Set 'isMoved()' to true and 'isCopied()' to 'original.isCopied()',
        // then set 'original' to the moved-from state.
        { *this = bslmf::MovableRefUtil::move(original); }

    //! ~TrackableValue() = default;

    // MANIPULATORS
    TrackableValue& operator=(int v)
        // Set value to the specified 'v' and set 'isMoved()' and 'isCopied()'
        // to false; then return '*this'.
        { return setValue(v); }

    TrackableValue& operator=(const TrackableValue& rhs)
        // Set value to the specified 'rhs.value()' and set 'isMoved()' to
        // false and 'isCopied()' to true; then return '*this'.
        { return setValue(rhs.value(), false, true); }

    TrackableValue& operator=(bslmf::MovableRef<TrackableValue> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Move value from the specified 'rhs', set 'isMoved()' to true, and
        // set 'isCopied()' to 'rhs.isCopied()', then assign 'rhs' the value
        // 'e_MOVED_FROM_VAL' and return '*this'.

    void setIsCopiedRaw(bool copiedFlag);
    void setIsMovedRaw(bool movedFlag);
    void setValueRaw(int v);
        // Set the constituent parts of this object to the specified
        // 'copiedFlag', specified 'movedFlag', or specified 'v', without
        // modifying the other parts.  It is up to the caller to ensure that
        // the flags are set consistently.

    void swap(TrackableValue& other);
        // Exchange the values AND 'isCopied()' flags of '*this' and the
        // specified 'other' object, then set the 'isMoved()' flag of both
        // objects to true.

    void resetMoveCopiedFlags()
        // Set 'isMoved()' and 'isCopied() to false.
        { d_valueAndFlags &= e_VALUE_MASK; }

    // ACCESSORS
    bool isCopied()    const { return d_valueAndFlags & e_COPIED_FLAG; }
    bool isMoved()     const { return d_valueAndFlags & e_MOVED_FLAG; }
    bool isMovedFrom() const { return d_valueAndFlags == e_MOVED_FROM_STATE; }
    int  value()       const { return d_valueAndFlags / e_VALUE_MULTIPLIER; }
};

// TrackableValue FREE FUNCTIONS
BSLA_MAYBE_UNUSED
inline bool operator==(const TrackableValue& a, const TrackableValue& b) {
    return a.value() == b.value();
}

BSLA_MAYBE_UNUSED
inline bool operator!=(const TrackableValue& a, const TrackableValue& b) {
    return a.value() != b.value();
}

BSLA_MAYBE_UNUSED
inline void swap(TrackableValue& a, TrackableValue& b) { a.swap(b); }

// IMPLEMENTATION of class TrackableValue
// --------------------------------------

inline
TrackableValue& TrackableValue::setValue(int  v,
                                         bool movedFlag,
                                         bool copiedFlag) {
    ASSERT(e_MIN <= v && v <= e_MAX);
    d_valueAndFlags = v * e_VALUE_MULTIPLIER;
    if (movedFlag)  d_valueAndFlags |= e_MOVED_FLAG;
    if (copiedFlag) d_valueAndFlags |= e_COPIED_FLAG;
    return *this;
}

inline
TrackableValue&
TrackableValue::operator=(bslmf::MovableRef<TrackableValue> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    TrackableValue& rhsRef = bslmf::MovableRefUtil::access(rhs);
    d_valueAndFlags = rhsRef.d_valueAndFlags | e_MOVED_FLAG;
    rhsRef.d_valueAndFlags = e_MOVED_FROM_STATE;
    return *this;
}

void TrackableValue::setIsCopiedRaw(bool copiedFlag) {
    d_valueAndFlags &= (e_VALUE_MASK | e_MOVED_FLAG);
    if (copiedFlag) d_valueAndFlags |= e_COPIED_FLAG;
}

void TrackableValue::setIsMovedRaw(bool movedFlag) {
    d_valueAndFlags &= (e_VALUE_MASK | e_COPIED_FLAG);
    if (movedFlag) d_valueAndFlags |= e_MOVED_FLAG;
}

void TrackableValue::setValueRaw(int v) {
    d_valueAndFlags &= e_FLAGS_MASK;
    d_valueAndFlags |= v * e_VALUE_MULTIPLIER;
}

void TrackableValue::swap(TrackableValue& other) {
    // Don't use std::swap<int> because don't want to #include <algorithm>
    int tmp = d_valueAndFlags;
    d_valueAndFlags = other.d_valueAndFlags;
    other.d_valueAndFlags = tmp;
    d_valueAndFlags       |= e_MOVED_FLAG;
    other.d_valueAndFlags |= e_MOVED_FLAG;
}

                // --------------------------
                // Class template TestFunctor
                // --------------------------

// Define a test functor that can be customized via a set of qualities.

enum FunctorQualities {
    // Bits for encoding the qualities of a functor.

    e_SMALL_FUNCTOR       = 0x01,  // Minimum non-empty size
    e_MEDIUM_FUNCTOR      = 0x02,  // Just fits in small-object buffer
    e_LARGE_FUNCTOR       = 0x03,  // Does not fit in small-object buffer
    e_SIZE_MASK           = 0x03,

    e_HAS_ALLOCATOR       = 0x04,
    e_IS_BITWISE_MOVABLE  = 0x08,
    e_IS_NOTHROW_MOVABLE  = 0x10
};

template <bool HAS_ALLOCATOR>
class TestFunctorMembers;

template <>
class TestFunctorMembers<false> {
    // Base class holding members of a test allocator with no allocator,
    // providing dummy allocator mechanisms for an allocatorless test functor.

  protected:
    struct Alloc {
        // Private non-allocator type
        bsl::true_type operator==(const Alloc&) const
            { return bsl::true_type(); }
        bsl::false_type operator!=(const Alloc&) const
            { return bsl::false_type(); }
    };

    TrackableValue    d_value;

    Alloc privateAllocator() const { return Alloc(); }
    void setPrivateAllocator(const Alloc&) { }
        // These setters/getters are no-ops and exist only to allow generic
        // code to compile.

  public:
    bool verifyAllocator(const bsl::allocator<char>&) const { return true; }
};

template <>
class TestFunctorMembers<true> {
    // Base class holding members of a test functor, including an allocator.

  protected:
    typedef bsl::allocator<char> Alloc;

    bslma::Allocator *d_allocator_p;
    TrackableValue    d_value;

    // Internal getters/setters for derived-class use only.
    Alloc privateAllocator() const { return d_allocator_p; }
    void setPrivateAllocator(const Alloc& a) { d_allocator_p = a.mechanism(); }

  public:
    typedef bsl::allocator<char> allocator_type;

    bslma::Allocator     *allocator()     const { return d_allocator_p; }
    bsl::allocator<char>  get_allocator() const { return d_allocator_p; }
        // Return the current allocator (old and new style, respectively).

    bool verifyAllocator(const bsl::allocator<char>& exp) const
        { return exp == d_allocator_p; }
};

template <bool HAS_ALLOCATOR, int SIZE_QUALITY>
class TestFunctorBase;

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_SMALL_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a minimum-sized stateful functor.
};

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_MEDIUM_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a functor along with padding such
    // that just fits in the small-object buffer.

    typedef TestFunctorMembers<HAS_ALLOCATOR>               Members;
    typedef typename bsls::AlignmentFromType<Members>::Type MembersAlign;

    union {
        MembersAlign d_align;
        char         d_padding[k_SMALL_OBJECT_BUFFER_SIZE - sizeof(Members)];
    };
};

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_LARGE_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a functor along with enough padding
    // such that it does not fit in the small-object buffer.

    typedef TestFunctorMembers<HAS_ALLOCATOR>               Members;
    typedef typename bsls::AlignmentFromType<Members>::Type MembersAlign;

    union {
        MembersAlign d_align;
        char         d_padding[1+k_SMALL_OBJECT_BUFFER_SIZE - sizeof(Members)];
    };
};

template <int QUALITIES>
class TestFunctor
    : public TestFunctorBase<bool(QUALITIES & e_HAS_ALLOCATOR),
                             QUALITIES & e_SIZE_MASK> {

    // PRIVATE CONSTANTS
    enum { k_HAS_ALLOCATOR = bool(QUALITIES & e_HAS_ALLOCATOR) };

    // PRIVATE MEMBER FUNCTIONS
    int encode(int value) const
        // If 'QUALITIES & e_BITWISE_MOVABLE' is false, return an encoding of
        // the specified 'value' such that, it the result is stored in
        // 'd_value', then an accidental bitwise move to a different copy of
        // 'TestFunctor' can be detected; otherwise return 'value' unchanged.
        // This encoding is its own inverse; i.e., 'encode(encode(v)) == v'.
    {
        if (QUALITIES & e_IS_BITWISE_MOVABLE) {
            return value;                                             // RETURN
        }

        std::size_t self = (std::size_t) this;  // Should be uintptr_t
        self /= sizeof(TrackableValue);         // Discard low-order zero bits.
        // Mask off high bits.
        self &= static_cast<std::size_t>(TrackableValue::e_MAX);
        return int(self ^ value);
    }

    typedef typename TestFunctorMembers<k_HAS_ALLOCATOR>::Alloc Alloc;

  public:
    // General, customized functor for testing.  The specified 'QUALITIES'
    // parameter indicates size, allocator usage, and movability and is
    // encoded as the inclusive-or of the appropriate enumerations in
    // 'FunctorQualities'.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
                                      bslma::UsesBslmaAllocator,
                                      k_HAS_ALLOCATOR);

    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
                                      bsl::is_nothrow_move_constructible,
                                      bool(QUALITIES & e_IS_NOTHROW_MOVABLE));

    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
                                    bslmf::IsBitwiseMoveable,
                                    bool(QUALITIES & e_IS_BITWISE_MOVABLE));

    // CREATORS
    explicit TestFunctor(int value = 0, const Alloc& alloc = Alloc())
    {
        this->setPrivateAllocator(alloc);
        this->setValue(value);
    }

    TestFunctor(const TestFunctor& original,
                const Alloc&       alloc = Alloc())
        // Copy construct from the specified 'original' using the
        // optionally specified 'alloc' allocator (if any).
    {
        this->setPrivateAllocator(alloc);
        operator=(original);
    }

    TestFunctor(bslmf::MovableRef<TestFunctor> original)            // IMPLICIT
        // Copy value from 'original', set 'isMoved()' to false and
        // 'isCopied()' to true.
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(bool(QUALITIES &
                                                 e_IS_NOTHROW_MOVABLE))
        // Move construct from the specified 'original' using the allocator
        // from 'original'.
    {
        TestFunctor& originalRef = original;
        this->setPrivateAllocator(originalRef.privateAllocator());
        operator=(bslmf::MovableRefUtil::move(originalRef));
    }

    TestFunctor(bslmf::MovableRef<TestFunctor> original, const Alloc& alloc)
        // Extended move construct from the specified 'original' using the
        // specified 'alloc'.  If 'original.get_allocator() == alloc', the
        // behavior is identical to move construction, otherwise it is
        // identical to extended copy construction.
    {
        TestFunctor& originalRef = original;
        this->setPrivateAllocator(alloc);
        operator=(bslmf::MovableRefUtil::move(originalRef));
    }

    ~TestFunctor() { memset(this, 0xbb, sizeof(*this)); }

    // MANIPULATORS
    TestFunctor& operator=(const TestFunctor& rhs) {
        TrackableValue temp(encode(rhs.value()));
        this->d_value = temp;  // Invoke copy assignment on 'TrackableValue'
        return *this;
    }

    TestFunctor& operator=(bslmf::MovableRef<TestFunctor> rhs) {
        TestFunctor& rhsRef = rhs;
        if (this->privateAllocator() == rhsRef.privateAllocator()) {
            int value = rhsRef.value();
            // Invoke move assignment on 'TrackableValue'
            this->d_value = bslmf::MovableRefUtil::move(rhsRef.d_value);
            // Re-encode value:
            this->d_value.setValueRaw(encode(value));
        }
        else {
            // Copy-assign if different allocators
            operator=(rhsRef);
        }
        return *this;
    }

    void resetMoveCopiedFlags() { this->d_value.resetMoveCopiedFlags(); }

    void setValue(int value) { this->d_value = encode(value); }

    // ACCESSORS
    int operator()()   const { return value(); }
    bool isCopied()    const { return this->d_value.isCopied(); }
    bool isMoved()     const { return this->d_value.isMoved(); }
    bool isMovedFrom() const { return this->d_value.isMovedFrom(); }
    int  value()       const { return encode(this->d_value.value()); }

    // HIDDEN FRIENDS
    friend bool operator==(const TestFunctor& a, const TestFunctor& b)
        { return a.value() == b.value(); }
    friend bool operator!=(const TestFunctor& a, const TestFunctor& b)
        { return a.value() != b.value(); }
};

BSLMF_ASSERT(sizeof(TestFunctor<e_SMALL_FUNCTOR>) <
             k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(TestFunctor<e_MEDIUM_FUNCTOR>) ==
             k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(TestFunctor<e_LARGE_FUNCTOR>) >
             k_SMALL_OBJECT_BUFFER_SIZE);

int simpleFunc(int x)
{
    // Simple function to be used as a target.
    return x + 1;
}

struct SimpleStruct {
    // Structure with a data member and function member, for generating
    // pointer-to-member values.
    int  d_dataMember;
    void functionMember();
};

void SimpleStruct::functionMember() { }

typedef int                (*PtrToFunc_t)(int);
typedef int   SimpleStruct::*PtrToMemData_t;
typedef void (SimpleStruct::*PtrToMemFunc_t)();

template <class TYPE>
struct TargetValueGen {
    // Namespace for methods used to generate values of the specified
    // 'TYPE'.  This primary template works for any 'TYPE' that can be
    // initialized from an integer.

    static int initializer()
        // Return a value that can be used to initialize 'TYPE'.
        { return 99; }
};

template <>
struct TargetValueGen<PtrToFunc_t> {
    // Specialization for pointer-to-function value

    static PtrToFunc_t initializer() { return &simpleFunc; }
};

template <>
struct TargetValueGen<PtrToMemData_t> {
    // Specialization for pointer-to-data member value

    static PtrToMemData_t initializer() { return &SimpleStruct::d_dataMember; }
};

template <>
struct TargetValueGen<PtrToMemFunc_t> {
    // Specialization for pointer-to-data member value

    static PtrToMemFunc_t initializer()
        { return &SimpleStruct::functionMember; }
};

template <class TYPE>
struct TargetValueGen<bslalg::NothrowMovableWrapper<TYPE> > {
    // Specialization for 'NothrowMovableWrapper' instantiations.

    static TYPE initializer()
        { return TYPE(TargetValueGen<TYPE>::initializer()); }
};

template <class TYPE>
struct TargetVerifier {
    // Namespace of methods to verify various attributes of an object of
    // 'TYPE'.  For types that are not specializations of 'TestFunctor', most
    // of the qualities are simply assumed true.

    typedef TYPE            TargType;
    typedef bsl::false_type UsesAlloc;

    static void clearFlags(TargType *) { }
    static void clearFlags(bslalg::NothrowMovableWrapper<TargType> *p)
        { clearFlags(&p->unwrap()); }

    static bool verifyAllocator(const TargType&               ,
                                const bsl::allocator<char>&) { return true; }
    static bool verifyCopied(const TargType&, bool) { return true; }
    static bool verifyMoved(const TargType&, bool) { return true; }
    static bool verifyMovedFrom(const TargType&, bool) { return true; }
};

template <int QUALITIES>
struct TargetVerifier<TestFunctor<QUALITIES> > {
    // Specialization of 'TargetVerifier' for type that are specializations of
    // 'TestFunctor'.

    typedef TestFunctor<QUALITIES>                                  TargType;
    typedef bsl::integral_constant<bool,
                                   bool(QUALITIES|e_HAS_ALLOCATOR)> UsesAlloc;

    static void clearFlags(TargType *obj_p) { obj_p->resetMoveCopiedFlags(); }
    static void clearFlags(bslalg::NothrowMovableWrapper<TargType> *p)
        { clearFlags(&p->unwrap()); }

    static bool verifyAllocator(const TargType&             obj,
                                const bsl::allocator<char>& exp)
        { return obj.verifyAllocator(exp); }

    static bool verifyCopied(const TargType& obj, bool exp)
        { return exp == obj.isCopied(); }
    static bool verifyMoved(const TargType& obj, bool exp)
        { return exp == obj.isMoved(); }
    static bool verifyMovedFrom(const TargType& obj, bool exp)
        { return exp == obj.isMovedFrom(); }
};

// Abbreviations for functor qualities
enum {
    e_SML     = e_SMALL_FUNCTOR     ,
    e_MED     = e_MEDIUM_FUNCTOR    ,
    e_LRG     = e_LARGE_FUNCTOR     ,
    e_ALLOC   = e_HAS_ALLOCATOR     ,
    e_BITWISE = e_IS_BITWISE_MOVABLE,
    e_NTMOVE  = e_IS_NOTHROW_MOVABLE
};

// List of types used for most tests
#if MSVC_2013
// MSVC 2013 miscompiles function templates that return pointers to data
// members.  For this reason, 'bslstl_function_rep' cannot support targets that
// are reference wrappers of pointers to data members.
# define FUNCTION_PTR_TYPES \
    PtrToFunc_t,            \
    PtrToMemFunc_t
#else
# define FUNCTION_PTR_TYPES \
    PtrToFunc_t,            \
    PtrToMemData_t,         \
    PtrToMemFunc_t
#endif

// Generate a list functor types with the specified size and has-allocator
// quality ('e_SMALL_FUNCTOR', 'e_MEDIUM_FUNCTOR' or 'e_LARGE_FUNCTOR',
// possibly ORed with 'e_HAS_ALLOCATOR') and each combination of bitwise
// movable and nothrow move-constructible qualities.
#define GEN_FUNCTOR_TYPES(SIZE_ALLOC)               \
    TestFunctor<SIZE_ALLOC                       >, \
    TestFunctor<SIZE_ALLOC             | e_NTMOVE>, \
    TestFunctor<SIZE_ALLOC | e_BITWISE           >, \
    TestFunctor<SIZE_ALLOC | e_BITWISE | e_NTMOVE>

// A good variety of functor types touching all the corner cases
#define COMMON_FUNCTOR_TYPES                             \
    TestFunctor<e_SML                                 >, \
    TestFunctor<e_SML                       | e_NTMOVE>, \
    TestFunctor<e_MED           | e_BITWISE           >, \
    TestFunctor<e_MED           | e_BITWISE | e_NTMOVE>, \
    TestFunctor<e_MED | e_ALLOC                       >, \
    TestFunctor<e_MED | e_ALLOC             | e_NTMOVE>, \
    TestFunctor<e_SML | e_ALLOC | e_BITWISE           >, \
    TestFunctor<e_SML | e_ALLOC | e_BITWISE | e_NTMOVE>, \
    TestFunctor<e_LRG                                 >, \
    TestFunctor<e_LRG | e_ALLOC | e_BITWISE | e_NTMOVE>

// A few callable types wrapped in 'bslalg::NothrowMovableUtil'.  The small and
// medium ones are the only ones where behavior would change.
#define NTWRAP_T(V) bslalg::NothrowMovableWrapper<V >
#define NTMOVE_FUNCTOR_TYPES                                       \
    NTWRAP_T(PtrToFunc_t                                        ), \
    NTWRAP_T(TestFunctor<e_SML                                 >), \
    NTWRAP_T(TestFunctor<e_MED           | e_BITWISE           >), \
    NTWRAP_T(TestFunctor<e_MED           | e_BITWISE | e_NTMOVE>), \
    NTWRAP_T(TestFunctor<e_MED | e_ALLOC                       >), \
    NTWRAP_T(TestFunctor<e_LRG                                 >), \
    NTWRAP_T(TestFunctor<e_LRG | e_ALLOC             | e_NTMOVE>)

#define SMALL_FUNCTOR_TYPES                              \
    TestFunctor<e_SML                                 >, \
    TestFunctor<e_SML                       | e_NTMOVE>, \
    TestFunctor<e_SML           | e_BITWISE           >, \
    TestFunctor<e_SML           | e_BITWISE | e_NTMOVE>, \
    TestFunctor<e_SML | e_ALLOC                       >, \
    TestFunctor<e_SML | e_ALLOC             | e_NTMOVE>, \
    TestFunctor<e_SML | e_ALLOC | e_BITWISE           >, \
    TestFunctor<e_SML | e_ALLOC | e_BITWISE | e_NTMOVE>

#define MEDIUM_FUNCTOR_TYPES                             \
    TestFunctor<e_MED                                 >, \
    TestFunctor<e_MED                       | e_NTMOVE>, \
    TestFunctor<e_MED           | e_BITWISE           >, \
    TestFunctor<e_MED           | e_BITWISE | e_NTMOVE>, \
    TestFunctor<e_MED | e_ALLOC                       >, \
    TestFunctor<e_MED | e_ALLOC             | e_NTMOVE>, \
    TestFunctor<e_MED | e_ALLOC | e_BITWISE           >, \
    TestFunctor<e_MED | e_ALLOC | e_BITWISE | e_NTMOVE>

#define LARGE_FUNCTOR_TYPES                              \
    TestFunctor<e_LRG                                 >, \
    TestFunctor<e_LRG                       | e_NTMOVE>, \
    TestFunctor<e_LRG           | e_BITWISE           >, \
    TestFunctor<e_LRG           | e_BITWISE | e_NTMOVE>, \
    TestFunctor<e_LRG | e_ALLOC                       >, \
    TestFunctor<e_LRG | e_ALLOC             | e_NTMOVE>, \
    TestFunctor<e_LRG | e_ALLOC | e_BITWISE           >, \
    TestFunctor<e_LRG | e_ALLOC | e_BITWISE | e_NTMOVE>

template <class TYPE>
class TestDriver {
    // Namespace to test 'Function_Rep' for target of specified 'TYPE'.

    typedef bslalg::NothrowMovableUtil NothrowMovableUtil;

    typedef TYPE                                          TargType;
    typedef typename
        NothrowMovableUtil::WrappedType<TargType>::type   NTWrpType;
    typedef typename
        NothrowMovableUtil::UnwrappedType<TargType>::type NTUnwrpType;
    typedef typename bsl::reference_wrapper<TargType>     RefWrap;
    typedef TargetValueGen<TargType>                      ValGen;
    typedef TargetVerifier<NTUnwrpType>                   Verifier;

    template <class TYPE2>
    struct TwoDTests {
        // Namespace for two-demensional tests for targets of specified 'TYPE'
        // and 'TYPE2'.

        typedef TYPE2                                          TargType2;
        typedef typename
            NothrowMovableUtil::WrappedType<TargType2>::type   NTWrpType2;
        typedef typename
            NothrowMovableUtil::UnwrappedType<TargType2>::type NTUnwrpType2;
        typedef typename bsl::reference_wrapper<TargType2>     RefWrap2;
        typedef TargetValueGen<TargType2>                      ValGen2;
        typedef TargetVerifier<NTUnwrpType2>                   Verifier2;

        enum {
            k_EXP_INPLACE2        = Soo::IsInplaceFunc<TargType2>::value,
            k_IS_BITWISE_MOVABLE2 = bslmf::IsBitwiseMoveable<TYPE2>::value
        };

        static void swapImp();
            // Test 'swap' between objects with targets of type 'TYPE' and
            // 'TYPE2'.
    };

    enum {
        k_EXP_INPLACE        = Soo::IsInplaceFunc<TargType>::value,
        k_IS_BITWISE_MOVABLE = bslmf::IsBitwiseMoveable<TYPE>::value
    };

    static void basicManipulatorsImp(bslma::TestAllocator *ta1,
                                     bslma::TestAllocator *ta2);
        // Use the specified 'ta1' to construct a 'bsl::function'
        // object.  Use the specified 'ta2' to construct the object that
        // will become the argument to 'installFunction'.  Run the basic
        // manipulators test with the results.

    static void moveInitImp(bslma::TestAllocator *ta1,
                            bslma::TestAllocator *ta2);

  public:
    static void basicManipulators();
        // Test basic manipulators and accessors.  This function is best suited
        // for use when 'TYPE' is not allocator-aware.

    static void basicManipulatorsWithAlloc();
        // Test basic manipulators and accessors.  This function requires
        // 'TYPE' to be allocator-aware, as it tests 'installFunc' when the
        // input argument uses the same or a different allocator than the
        // eventual target within the 'Function_Rep' object.

    static void copyInit();
        // Test the 'copyInit' manipulator.

    static void moveInit();
        // Test the 'moveInit' manipulator.

    static void swap();
        // Test 'swap' between objects with targets of type 'TYPE' and
        // objects with a number of other target types.
};

template <class TYPE>
void TestDriver<TYPE>::basicManipulatorsImp(bslma::TestAllocator *ta1,
                                            bslma::TestAllocator *ta2)
{
    bslma::TestAllocatorMonitor taM(ta1);

    if (veryVerbose) printf("Test installFunc<TargType>(TargType&)\n");
    {
        Obj mF(ta1); const Obj& F = mF;

        const int expAllocBlocks = k_EXP_INPLACE ? 0 : 1;

        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObj1Proxy(ValGen::initializer(), ta2);
        TargType& callableObj1 = callableObj1Proxy.object();
        Verifier::clearFlags(&callableObj1);
        mF.installFunc(callableObj1, &testInvoker1);
        ASSERT(! F.isEmpty()                     );
        ASSERT(F.invoker()       == &testInvoker1);
        ASSERT(F.target_type()   == typeid(NTUnwrpType));
        ASSERT(F.get_allocator() == ta1          );
        ASSERT_IS_INPLACE(F, k_EXP_INPLACE);
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
        NTUnwrpType *target = F.target<NTUnwrpType>();
        ASSERT(target != 0);
        if (target) {
            ASSERT(*target == TargType(ValGen::initializer()));
            ASSERT((target  == F.targetRaw<NTUnwrpType, k_EXP_INPLACE>()));
            ASSERT(Verifier::verifyAllocator(*target, ta1));
            ASSERT(Verifier::verifyMoved(*target, false));
            ASSERT(Verifier::verifyCopied(*target, true));
            ASSERT(Verifier::verifyMovedFrom(callableObj1, false));
        }
        mF.makeEmpty();
        ASSERT(F.isEmpty()                            );
        ASSERT(F.invoker()             == 0           );
        ASSERT(F.target_type()         == typeid(void));
        ASSERT(F.target<NTUnwrpType>() == 0           );
        ASSERT(F.get_allocator()       == ta1         );
        ASSERT(taM.isInUseSame()                      );

        Proxy     callableObj2Proxy(ValGen::initializer(), ta2);
        TargType& callableObj2 = callableObj2Proxy.object();
        Verifier::clearFlags(&callableObj2);
        mF.installFunc(bslmf::MovableRefUtil::move(callableObj2),
                       &testInvoker1);
        ASSERT(! F.isEmpty());
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
    }
    ASSERT(taM.isInUseSame());  // Any memory allocations returned

    if (veryVerbose) printf("Test installFunc<TargType>(TargType&&)\n");
    {
        // Should we expect a move or copy from the callable object into the
        // target?
        const bool expMove = (ta1 == ta2);// Same allocator.  Expect move.
        const bool expCopy = (ta1 != ta2);// Different allocator.  Expect copy.

        Obj mF(ta1); const Obj& F = mF;

        const int expAllocBlocks = k_EXP_INPLACE ? 0 : 1;

        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObj1Proxy(ValGen::initializer(), ta2);
        TargType& callableObj1 = callableObj1Proxy.object();
        Verifier::clearFlags(&callableObj1);
        mF.installFunc(bslmf::MovableRefUtil::move(callableObj1),
                       &testInvoker1);
        ASSERT(! F.isEmpty()                     );
        ASSERT(F.invoker()       == &testInvoker1);
        ASSERT(F.target_type()   == typeid(NTUnwrpType));
        ASSERT(F.get_allocator() == ta1          );
        ASSERT_IS_INPLACE(F, k_EXP_INPLACE);
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
        NTUnwrpType *target = F.target<NTUnwrpType>();
        ASSERT(target != 0);
        if (target) {
            ASSERT(*target == TargType(ValGen::initializer()));
            ASSERT((target  == F.targetRaw<NTUnwrpType, k_EXP_INPLACE>()));
            ASSERT(Verifier::verifyAllocator(*target, ta1));
            ASSERT(Verifier::verifyMoved(*target, expMove));
            ASSERT(Verifier::verifyCopied(*target, expCopy));
            ASSERT(Verifier::verifyMovedFrom(callableObj1, expMove));
        }
        mF.makeEmpty();
        ASSERT(F.isEmpty()                            );
        ASSERT(F.invoker()             == 0           );
        ASSERT(F.target_type()         == typeid(void));
        ASSERT(F.target<NTUnwrpType>() == 0           );
        ASSERT(F.get_allocator()       == ta1         );
        ASSERT(taM.isInUseSame()                      );

        Proxy     callableObj2Proxy(ValGen::initializer(), ta2);
        TargType& callableObj2 = callableObj2Proxy.object();
        Verifier::clearFlags(&callableObj2);
        mF.installFunc(bslmf::MovableRefUtil::move(callableObj2),
                       &testInvoker1);
        ASSERT(! F.isEmpty());
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
    }
    ASSERT(taM.isInUseSame());  // Any memory allocations returned

    taM.reset();
    if (veryVerbose) printf("Test installFunc<RefWrap>(RefWrap&)\n");
    {
        Obj mF(ta1); const Obj& F = mF;

        const int expAllocBlocks = 0;

        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObj1Proxy(ValGen::initializer(), ta2);
        TargType& callableObj1 = callableObj1Proxy.object();
        Verifier::clearFlags(&callableObj1);
        RefWrap wrappedObj(callableObj1);
        mF.installFunc(wrappedObj, &testInvoker1);
        ASSERT(! F.isEmpty()                       );
        ASSERT(F.invoker()       == &testInvoker1  );
        ASSERT(F.target_type()   == typeid(RefWrap));
        ASSERT(F.get_allocator() == ta1            );
        ASSERT_IS_INPLACE(F, true);
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
        RefWrap *target = F.target<RefWrap>();
        ASSERT(target != 0);
        if (target) {
            TargType& unwrappedTarget = target->get();
            ASSERT(&unwrappedTarget == &callableObj1);
            ASSERT((target == F.targetRaw<RefWrap, true>()));
            ASSERT(Verifier::verifyAllocator(unwrappedTarget, ta2));
            // With 'reference_wrapper', callable object is neither moved nor
            // copied, only the wrapper is moved.
            ASSERT(Verifier::verifyMoved(unwrappedTarget, false));
            ASSERT(Verifier::verifyCopied(unwrappedTarget, false));
            ASSERT(Verifier::verifyMovedFrom(callableObj1, false));
        }
        mF.makeEmpty();
        ASSERT(F.isEmpty()                        );
        ASSERT(F.invoker()         == 0           );
        ASSERT(F.target_type()     == typeid(void));
        ASSERT(F.target<RefWrap>() == 0           );
        ASSERT(F.get_allocator()   == ta1         );
        ASSERT(taM.isInUseSame()                  );
    }
    ASSERT(taM.isInUseSame());  // Any memory allocations returned

    taM.reset();
    if (veryVerbose) printf("Test installFunc<RefWrap>(RefWrap&&)\n");
    {
        Obj mF(ta1); const Obj& F = mF;

        const int expAllocBlocks = 0;

        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObj1Proxy(ValGen::initializer(), ta2);
        TargType& callableObj1 = callableObj1Proxy.object();
        Verifier::clearFlags(&callableObj1);
        RefWrap wrappedObj(callableObj1);
        mF.installFunc(bslmf::MovableRefUtil::move(wrappedObj), &testInvoker1);
        ASSERT(! F.isEmpty()                       );
        ASSERT(F.invoker()       == &testInvoker1  );
        ASSERT(F.target_type()   == typeid(RefWrap));
        ASSERT(F.get_allocator() == ta1            );
        ASSERT_IS_INPLACE(F, true);
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
        RefWrap *target = F.target<RefWrap>();
        ASSERT(target != 0);
        if (target) {
            TargType& unwrappedTarget = target->get();
            ASSERT(&unwrappedTarget == &callableObj1);
            ASSERT((target == F.targetRaw<RefWrap, true>()));
            ASSERT(Verifier::verifyAllocator(unwrappedTarget, ta2));
            // With 'reference_wrapper', callable object is neither moved nor
            // copied, only the wrapper is moved.
            ASSERT(Verifier::verifyMoved(unwrappedTarget, false));
            ASSERT(Verifier::verifyCopied(unwrappedTarget, false));
            ASSERT(Verifier::verifyMovedFrom(callableObj1, false));
        }
        mF.makeEmpty();
        ASSERT(F.isEmpty()                        );
        ASSERT(F.invoker()         == 0           );
        ASSERT(F.target_type()     == typeid(void));
        ASSERT(F.target<RefWrap>() == 0           );
        ASSERT(F.get_allocator()   == ta1         );
        ASSERT(taM.isInUseSame()                  );
    }
    ASSERT(taM.isInUseSame());  // Any memory allocations returned
}

template <class TYPE>
void TestDriver<TYPE>::basicManipulators()
{
    bslma::TestAllocator ta1("Obj alloc", veryVeryVeryVerbose);

    if (veryVerbose) printf("Test empty\n");
    {
        Obj mF1(&ta1); const Obj& F1 = mF1;

        ASSERT(F1.isEmpty()                            );
        ASSERT(F1.invoker()             == 0           );
        ASSERT(F1.target_type()         == typeid(void));
        ASSERT(F1.target<NTUnwrpType>() == 0           );
        ASSERT(F1.get_allocator()       == &ta1        );

        TargType callableObj1(ValGen::initializer());
        mF1.installFunc(callableObj1, 0);
        ASSERT(F1.isEmpty()                            );
        ASSERT(F1.invoker()             == 0           );
        ASSERT(F1.target_type()         == typeid(void));
        ASSERT(F1.target<NTUnwrpType>() == 0           );
        ASSERT(F1.get_allocator()       == &ta1        );

        Obj mF2(&ta1); const Obj& F2 = mF2;

        ASSERT(F2.isEmpty()                            );
        ASSERT(F2.invoker()             == 0           );
        ASSERT(F2.target_type()         == typeid(void));
        ASSERT(F2.target<NTUnwrpType>() == 0           );
        ASSERT(F2.get_allocator()       == &ta1        );

        TargType callableObj2(ValGen::initializer());
        mF2.installFunc(bslmf::MovableRefUtil::move(callableObj2), 0);
        ASSERT(F2.isEmpty()                            );
        ASSERT(F2.invoker()             == 0           );
        ASSERT(F2.target_type()         == typeid(void));
        ASSERT(F2.target<NTUnwrpType>() == 0           );
        ASSERT(F2.get_allocator()       == &ta1        );
    }

    // Test using the same allocator for the 'Function_Rep' object and for the
    // argument to 'installFunc'.
    basicManipulatorsImp(&ta1, &ta1);
}

template <class TYPE>
void TestDriver<TYPE>::basicManipulatorsWithAlloc()
{
    bslma::TestAllocator ta1("Obj alloc", veryVeryVeryVerbose);
    bslma::TestAllocator ta2("Argument alloc", veryVeryVeryVerbose);

    // Test using the same allocator for the 'Function_Rep' object and for the
    // argument to 'installFunc'.
    basicManipulatorsImp(&ta1, &ta1);

    // Test again using different allocators for the 'Function_Rep' object and
    // for the argument to 'installFunc'.
    basicManipulatorsImp(&ta1, &ta2);
}

template <class TYPE>
void TestDriver<TYPE>::copyInit()
{
    bslma::TestAllocator ta1("Source alloc", veryVeryVeryVerbose);
    bslma::TestAllocator ta2("Dest alloc", veryVeryVeryVerbose);

    TargType callableObj(ValGen::initializer());
    Obj      source(&ta1); const Obj& SOURCE = source;
    source.installFunc(callableObj, &testInvoker1);
    if (source.target<NTUnwrpType>()) {
        Verifier::clearFlags(source.target<NTUnwrpType>());
    }

    Obj dest(&ta2);
    dest.copyInit(SOURCE);
    ASSERT(dest.isEmpty()     == SOURCE.isEmpty());
    ASSERT(dest.target_type() == SOURCE.target_type());
    NTUnwrpType *target = dest.target<NTUnwrpType>();
    ASSERT(target);
    if (dest.target<NTUnwrpType>()) {
        ASSERT(*target == *SOURCE.target<NTUnwrpType>());
        ASSERT(Verifier::verifyAllocator(*target, &ta2));
        ASSERT(Verifier::verifyMoved(*target, false));
        ASSERT(Verifier::verifyCopied(*target, true));
        ASSERT(Verifier::verifyMovedFrom(*SOURCE.target<NTUnwrpType>(),false));
    }
    ASSERT(dest.get_allocator() == &ta2);
    ASSERT(ta2.numBlocksInUse() == ta1.numBlocksInUse());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    ASSERT(dest.isInplace() == SOURCE.isInplace());
#endif
}

template <class TYPE>
void TestDriver<TYPE>::moveInitImp(bslma::TestAllocator *ta1,
                                   bslma::TestAllocator *ta2)
{
    // Is the target expected to be moved, copied, or transfered?
    bool expMove = false, expCopy = false, expXfer = false;
    if (ta1 != ta2) {
        expCopy = true;
    }
    else if (k_EXP_INPLACE) {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        expMove = ! k_IS_BITWISE_MOVABLE;  // No moves if bitwise movable
#else
        // TBD: In C++03 compilers, 'destructiveMove' never uses the
        // 'MovableRef' move constructor.  Remove this conditional if that
        // situation is ever corrected.
        expMove = false;
        expCopy = ! k_IS_BITWISE_MOVABLE;
#endif
    }
    else {
        expXfer = true;
    }

    bslma::TestAllocatorMonitor taM(ta2);
    {
        Obj source(ta1);
        Obj dest(ta2);

        const int expAllocBlocks = k_EXP_INPLACE ? 0 : 1;

        // Set target for source
        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObjProxy(ValGen::initializer(), ta1);
        TargType& callableObj = callableObjProxy.object();
        source.installFunc(callableObj, &testInvoker1);
        NTUnwrpType *sourceTarget = source.target<NTUnwrpType>();
        Verifier::clearFlags(sourceTarget);

        // Move to 'dest'
        dest.moveInit(&source);

        if (expXfer) {
            ASSERT(source.isEmpty());
        }
        ASSERT(! dest.isEmpty());
        ASSERT(dest.invoker()       == &testInvoker1);
        ASSERT(dest.target_type()   == typeid(NTUnwrpType));
        ASSERT(dest.get_allocator() == ta2);
        ASSERT_IS_INPLACE(dest, k_EXP_INPLACE);
        ASSERT(taM.numBlocksInUseChange() == expAllocBlocks);
        NTUnwrpType *target = dest.target<NTUnwrpType>();
        ASSERT(target != 0);
        if (target) {
            ASSERT(*target == TargType(ValGen::initializer()));
            ASSERT(Verifier::verifyAllocator(*target, ta2));
            ASSERT(Verifier::verifyMoved(*target, expMove));
            ASSERT(Verifier::verifyCopied(*target, expCopy));
            ASSERT((target == sourceTarget) == expXfer);
        }
    }
    ASSERT(taM.isInUseSame());  // Any memory allocations returned
}

template <class TYPE>
void TestDriver<TYPE>::moveInit()
{
    bslma::TestAllocator ta1("Source alloc", veryVeryVeryVerbose);
    bslma::TestAllocator ta2("Dest alloc", veryVeryVeryVerbose);

    moveInitImp(&ta1, &ta1);  // Same allocator
    moveInitImp(&ta1, &ta2);  // Different allocators
}

template <class TYPE>
template <class TYPE2>
void TestDriver<TYPE>::TwoDTests<TYPE2>::swapImp()
{
    bslma::TestAllocator ta("Test alloc", veryVeryVeryVerbose);

    Obj x1(&ta);
    typedef bslalg::ConstructorProxy<TargType> Proxy1;
    Proxy1    callableObj1Proxy(ValGen::initializer(), &ta);
    TargType& callableObj1 = callableObj1Proxy.object();
    x1.installFunc(callableObj1, &testInvoker1);
    NTUnwrpType *target1Pre = x1.target<NTUnwrpType>();
    Verifier::clearFlags(target1Pre);

    Obj x2(&ta);
    typedef bslalg::ConstructorProxy<TargType2> Proxy2;
    Proxy2     callableObj2Proxy(ValGen2::initializer(), &ta);
    TargType2& callableObj2 = callableObj2Proxy.object();
    x2.installFunc(callableObj2, &testInvoker2);
    NTUnwrpType2 *target2Pre = x2.target<NTUnwrpType2>();
    Verifier2::clearFlags(target2Pre);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    const bool expMove1 = k_EXP_INPLACE  && ! k_IS_BITWISE_MOVABLE;
    const bool expMove2 = k_EXP_INPLACE2 && ! k_IS_BITWISE_MOVABLE2;
    const bool expCopy1 = false;
    const bool expCopy2 = false;
#else
    // TBD: In C++03 compilers, 'destructiveMove' never uses the 'MovableRef'
    // move constructor.  Remove this conditional if that situation is ever
    // corrected.
    const bool expMove1 = false;
    const bool expMove2 = false;
    const bool expCopy1 = k_EXP_INPLACE  && ! k_IS_BITWISE_MOVABLE;
    const bool expCopy2 = k_EXP_INPLACE2 && ! k_IS_BITWISE_MOVABLE2;
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

    const bool expXfer1 = ! k_EXP_INPLACE;
    const bool expXfer2 = ! k_EXP_INPLACE2;

    bslma::TestAllocatorMonitor taM(&ta);
    x1.swap(x2);
    ASSERT(taM.isInUseSame());

    ASSERT(! x1.isEmpty());
    ASSERT(x1.invoker()       == &testInvoker2);
    ASSERT(x1.target_type()   == typeid(NTUnwrpType2));
    ASSERT(x1.get_allocator() == &ta);
    ASSERT_IS_INPLACE(x1, k_EXP_INPLACE2);
    NTUnwrpType2 *target1Post = x1.target<NTUnwrpType2>();
    ASSERT(target1Post != 0);
    if (target1Post) {
        ASSERT(*target1Post == TargType2(ValGen2::initializer()));
        ASSERT(Verifier2::verifyAllocator(*target1Post, &ta));
        ASSERT(Verifier2::verifyMoved(*target1Post, expMove2));
        ASSERT(Verifier2::verifyCopied(*target1Post, expCopy2));
        ASSERT((target1Post == target2Pre) || ! expXfer2);
    }

    ASSERT(! x2.isEmpty());
    ASSERT(x2.invoker()       == &testInvoker1);
    ASSERT(x2.target_type()   == typeid(NTUnwrpType));
    ASSERT(x2.get_allocator() == &ta);
    ASSERT_IS_INPLACE(x2, k_EXP_INPLACE);
    NTUnwrpType *target2Post = x2.target<NTUnwrpType>();
    ASSERT(target2Post != 0);
    if (target2Post) {
        ASSERT(*target2Post == TargType(ValGen::initializer()));
        ASSERT(Verifier::verifyAllocator(*target2Post, &ta));
        ASSERT(Verifier::verifyMoved(*target2Post, expMove1));
        ASSERT(Verifier::verifyCopied(*target2Post, expCopy1));
        ASSERT((target2Post == target1Pre) || ! expXfer1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::swap()
{
    // Test swapping against an empty object.
    {
        bslma::TestAllocator ta("Test alloc", veryVeryVeryVerbose);

        Obj       x1(&ta);
        typedef bslalg::ConstructorProxy<TargType> Proxy;
        Proxy     callableObjProxy(ValGen::initializer(), &ta);
        TargType& callableObj = callableObjProxy.object();
        x1.installFunc(callableObj, &testInvoker1);
        NTUnwrpType *target1Pre = x1.target<NTUnwrpType>();
        Verifier::clearFlags(target1Pre);

        Obj x2(&ta);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        const bool expMove = k_EXP_INPLACE && ! k_IS_BITWISE_MOVABLE;
        const bool expCopy = false;
#else
        // TBD: In C++03 compilers, 'destructiveMove' never uses the
        // 'MovableRef' move constructor.  Remove this conditional if that
        // situation is ever corrected.
        const bool expMove = false;
        const bool expCopy = k_EXP_INPLACE && ! k_IS_BITWISE_MOVABLE;
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

        const bool expXfer = ! k_EXP_INPLACE;

        bslma::TestAllocatorMonitor taM(&ta);
        x1.swap(x2);
        ASSERT(taM.isInUseSame());
        ASSERT(  x1.isEmpty());
        ASSERT(! x2.isEmpty());
        ASSERT(x2.invoker()       == &testInvoker1);
        ASSERT(x2.target_type()   == typeid(NTUnwrpType));
        ASSERT(x2.get_allocator() == &ta);
        ASSERT_IS_INPLACE(x2, k_EXP_INPLACE);
        NTUnwrpType *target2Post = x2.target<NTUnwrpType>();
        ASSERT(target2Post != 0);
        if (target2Post) {
            ASSERT(*target2Post == TargType(ValGen::initializer()));
            ASSERT(Verifier::verifyAllocator(*target2Post, &ta));
            ASSERT(Verifier::verifyMoved(*target2Post, expMove));
            ASSERT(Verifier::verifyCopied(*target2Post, expCopy));
            ASSERT((target2Post == target1Pre) || ! expXfer);
        }

        // Test swap again, but this time 'x1' starts out empty and 'x2' starts
        // out non-empty.
        x1.swap(x2);
        ASSERT(! x1.isEmpty());
        ASSERT(  x2.isEmpty());
        NTUnwrpType *target1Post = x1.target<NTUnwrpType>();
        ASSERT(target1Post != 0);
        if (target1Post) {
            ASSERT(*target1Post == TargType(ValGen::initializer()));
            ASSERT((target1Post == target1Pre) || ! expXfer);
        }
    }

    // Loop through nested list of types, creating a cross-product of two
    // template types.
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
        TwoDTests,
        swapImp,
        FUNCTION_PTR_TYPES,
        COMMON_FUNCTOR_TYPES,
        NTMOVE_FUNCTOR_TYPES);
}

}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

// No usage example for private component

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // TBD: Consider adding negative test cases on methods that have
    // precondition checks.
    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //: 1 Swapping two 'Function_Rep' objects has the same affect as
        //:   generating the same objects but with the 'installFunc' arguments
        //:   to one substituted for the 'installFunc' arguments to the other.
        //:   Note that the allocators must be the same as a precondition to
        //:   'swap'.  This concern applies if either or both 'Function_Rep'
        //:   objects are initially empty.
        //:
        //: 2 Memory consumption, both from allocators and from the global
        //:   heap, is unchanged by the swap operation.
        //:
        //: 3 The above concerns apply for each of the different types of
        //:   wrapped functors.  The wrapped types of the two objects being
        //:   swapped need not be the same nor have the same attributes wrt to
        //:   size, allocator-awareness, bitwise movabiliy, etc.
        //:
        //: 4 The above concerns apply to 'Function_Rep's constructed with
        //:   nothrow wrappers.
        //
        // Plan:
        //: 1 Create two different 'Function_Rep' objects, 'x1' and 'x2', using
        //:   the same allocator, and wrapping callable objects of type 'TYPE'
        //:   and 'TYPE2', respectively.  Call 'x1.swap(x2)', then verify that
        //:   the target and invoker of 'x1' and 'x2' have been exchanged.
        //:   Also verify that if the original target of 'x1' qualifies for the
        //:   small-object optimization, that the new target of 'x2' is in a
        //:   moved-from state; otherwise it was transfered without invoking
        //:   the move constructor (and conversely for the original target of
        //:   'x2' moved to 'x1').  Check that the allocators of both objects
        //:   compare equal to their original values and that the wrapped
        //:   objects' allocators (if any) continue to reflect proper allocator
        //:   propagation.  (Since the allocators of 'x1' and 'x2' were the
        //:   same before the swap, it is unimportant whether the allocators
        //:   are swapped or not.)  (C-1)
        //:
        //: 2 Verify that the amount of memory in use after the swap is the
        //:   same as before the swap.  (C-2)
        //:
        //: 3 Create objects 'x1' and 'x2' using types from a list, producing a
        //:   two-dimensional set of tests.  Ensure that the type lists contain
        //:   representative target types, including some types wrapped in
        //:   'bslalg::NothrowMovableWrapper'.  Repeat steps 1 and 2 for the
        //:   cross product of types in the two lists.  (C-3, 4)
        //
        // Testing
        //  void swap(Function_Rep& other) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        // Swap two empty objects
        {
            bslma::TestAllocator ta;
            Obj                  x1(&ta);
            Obj                  x2(&ta);

            x1.swap(x2);
            ASSERT(x1.isEmpty());
            ASSERT(x2.isEmpty());
            ASSERT(0 == ta.numBlocksInUse());
        }

        // Test 'swap' with a representative list of types
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            swap,
            FUNCTION_PTR_TYPES,
            COMMON_FUNCTOR_TYPES,
            NTMOVE_FUNCTOR_TYPES);

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'moveInit'
        //   The 'moveInit' manipulator is similar to a move constructor but it
        //   is called *after* an object has already been constructed.  The
        //   'Function_Rep' being modified must be empty.
        //
        // Concerns:
        //: 1 Moving from an empty 'Function_Rep' yields an empty
        //:   'Function_Rep' object.
        //:
        //: 2 Moving from a non-empty 'Function_Rep' yields a non-empty
        //:   'Function_Rep' object.  The 'target_type' and 'target' attributes
        //:   of the moved-to 'Function_Rep' match the pre-move attributes
        //:   of the moved-from 'Function_Rep'.  The moved-from 'Function_Rep'
        //:   will be empty after the call.
        //:
        //: 3 The allocator in the destination is the same as the allocator
        //:   specified on construction of the destination, regardless of the
        //:   allocator of the source object.  The allocator (if any) used by
        //:   the moved-to target object matches the allocator of the
        //:   moved-to 'Function_Rep'.
        //:
        //: 4 If the target type qualifies for the small-object optimization,
        //:   then it is moved using the extended move constructor.  If the
        //:   target type is allocator-aware and the source and destination
        //:   allocators are different, this typically means that the target is
        //:   copy-constructed; otherwise it is move-constructed.  Conversely,
        //:   if the target type does not qualify for the small-object
        //:   optimization, then, if the source and destination allocators are
        //:   different, the target is copy-constructed; otherwise it is
        //:   transfered without invoking its move or copy constructor.
        //:
        //: 5 The memory owned by the moved-to 'Function_Rep' after the move is
        //:   the same as the memory owned by the moved-from 'Function_Rep'
        //:   before the move.
        //:
        //: 6 The above concerns apply to targets of type pointer-to-function,
        //:   pointer-to-member function, pointer to member data, and functor
        //:   classes (of various sizes, allocator-aware or not, bitwise
        //:   movable and not, with and without throwing move constructors).
        //:
        //: 7 The above concerns apply if the moved-from 'Function_Rep' is
        //:   holding an object of type 'bslalg::NothrowMovableWrapper'.
        //:   Specifically, if the nothrow wrapper results in an otherwise
        //:   ineligible functor becoming eligible for the small object
        //:   optimization, then the moved-to 'Function_Rep' will also use the
        //:   small object optimization.
        //
        // Plan:
        //: 1 Use 'moveInit' to move an empty 'Function_Rep' into another empty
        //:   'Function_Rep'.  Verify that no attributes of either object
        //:   change.  (C-1)
        //:
        //: 2 Use 'moveInit' to move a non-empty 'Function_Rep' into an empty
        //:   'Function_Rep'.  Verify that the latter object after the call is
        //:   has the same target attributes as the moved-from object did
        //:   before the call and that the moved-from object is now empty.
        //:   (C-2)
        //:
        //: 3 Verify that the allocator in the moved-to object does not change.
        //:   Verify that the allocator (if any) used by the target of the
        //:   moved-to 'Function_Rep' is the same as the allocator used by the
        //:   'Function_Rep' itself. (C-3)
        //:
        //: 4 Construct source and destination objects with the same allocator
        //:   and verify the expected move behavior of the target.  Repeat
        //:   using different allocators and verify the expected behavior.
        //:   (C-4)
        //:
        //: 5 Measure the amount of memory used when constructing the
        //:   moved-from object.  Verify that the same amount of memory is
        //:   used by the moved-to object after the move.  (C-5)
        //:
        //: 6 Use a list-driven approach to repeat steps 2 to 5 with callable
        //:   object types belonging to the categories described in concern
        //:   (C-6).
        //:
        //: 7 Include in the list for step 7 several
        //:   'bslalg::NothrowMovableWrappeer' instantiations, including some
        //:   that make an otherwise ineligible type eligible for the small
        //:   object optimization.  Verify that the source and destination
        //:   objects produce the same result for 'isInplace' in all cases.
        //:   (C-7)
        //
        // Testing:
        //  void moveInit(Function_Rep *from);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'moveInit'"
                            "\n==================\n");

        bslma::TestAllocator ta1("Source alloc", veryVeryVeryVerbose);
        bslma::TestAllocator ta2("Dest alloc", veryVeryVeryVerbose);

        // C-1: Move empty object
        {
            Obj source(&ta1); const Obj& SOURCE = source;
            Obj dest(&ta2);

            ASSERT(SOURCE.isEmpty());
            ASSERT(dest.isEmpty());
            ASSERT(&ta1 == SOURCE.get_allocator());
            ASSERT(&ta2 == dest.get_allocator());

            dest.moveInit(&source);

            ASSERT(SOURCE.isEmpty());
            ASSERT(dest.isEmpty());
            ASSERT(&ta1 == SOURCE.get_allocator());
            ASSERT(&ta2 == dest.get_allocator());
        }

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            moveInit,
            FUNCTION_PTR_TYPES,
            COMMON_FUNCTOR_TYPES,
            NTMOVE_FUNCTOR_TYPES);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'copyInit'
        //   The 'copyInit' manipulator is similar to a copy constructor but it
        //   is called *after* an object has already been constructed.  The
        //   'Function_Rep' being modified must be empty.
        //
        // Concerns:
        //: 1 Copying from an empty 'Function_Rep' yields an empty
        //:   'Function_Rep' object.
        //:
        //: 2 Copying from a non-empty 'Function_Rep' yields a non-empty
        //:   'Function_Rep' object.  The 'target_type' and 'target' attributes
        //:   of the copied-to 'Function_Rep' match the corresponding
        //:   attributes of the copied-from 'Function_Rep'.
        //:
        //: 3 The allocator in the destination is the same as the allocator
        //:   specified on construction of the destination, regardless of the
        //:   allocator of the source object.  The allocator (if any) used by
        //:   the copied-to target object matches the allocator for the
        //:   destination 'Function_Rep'.
        //:
        //: 4 The memory allocated for the copy is the same as the memory
        //:   allocated by the copied-from object.
        //:
        //: 5 The above concerns apply to targets of type pointer-to-function,
        //:   pointer-to-member function, pointer to member data, and functor
        //:   classes (of various sizes, allocator-aware or not, bitwise
        //:   movable and not, with and without throwing move constructors).
        //:
        //: 6 The above concerns apply if the copied-from 'Function_Rep' is
        //:   holding an object of type 'bslalg::NothrowMovableWrapper'.
        //:   Specifically, if the nothrow wrapper results in an otherwise
        //:   ineligible functor becoming eligible for the small object
        //:   optimization, then the copy will also use the small object
        //:   optimization.
        //
        // Plan:
        //: 1 Use 'copyInit' to copy an empty 'Function_Rep' into another empty
        //:   'Function_Rep'.  Verify that no attributes of either object
        //:   change.  (C-1)
        //:
        //: 2 Use 'copyInit' to copy a non-empty 'Function_Rep' into an empty
        //:   'Function_Rep'.  Verify that the latter object after the call is
        //:   a faithful copy of the original.  (C-2)
        //:
        //: 3 Verify that the allocator in the copied-to object does not
        //:   change.  Verify that the allocator (if any) used by the target is
        //:   the same as the allocator used by the copied-to object.  (C-3)
        //:
        //: 4 Measure the amount of memory used when constructing the
        //:   copied-from object.  Verify that the same amount of memory is
        //:   used when constructing the copied-to object.  (C-4)
        //:
        //: 5 Use a list-driven approach to repeat steps 2 to 4 with callable
        //:   object types belonging to the categories described in concern
        //:   (C-5).
        //:
        //: 6 Include in the list for step 5 several
        //:   'bslalg::NothrowMovableWrappeer' instantiations, including some
        //:   that make an otherwise ineligible type eligible for the small
        //:   object optimization.  Verify that the source and destination
        //:   objects produce the same result for 'isInplace' in all cases.
        //:   (C-6)
        //
        // Testing:
        //  void copyInit(const Function_Rep& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'copyInit'"
                            "\n==================\n");

        bslma::TestAllocator ta1("Source alloc", veryVeryVeryVerbose);
        bslma::TestAllocator ta2("Dest alloc", veryVeryVeryVerbose);

        // C-1: Copy empty object
        {
            Obj source(&ta1); const Obj& SOURCE = source;
            Obj dest(&ta2);

            ASSERT(SOURCE.isEmpty());
            ASSERT(dest.isEmpty());
            ASSERT(&ta1 == SOURCE.get_allocator());
            ASSERT(&ta2 == dest.get_allocator());

            dest.copyInit(source);

            ASSERT(SOURCE.isEmpty());
            ASSERT(dest.isEmpty());
            ASSERT(&ta1 == SOURCE.get_allocator());
            ASSERT(&ta2 == dest.get_allocator());
        }

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            copyInit,
            FUNCTION_PTR_TYPES,
            COMMON_FUNCTOR_TYPES,
            NTMOVE_FUNCTOR_TYPES);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND BASIC MANIPULATORS
        //
        // Concerns:
        //: 1 Constructing a 'Function_Rep' with allocator 'a' yields an empty
        //:   object with the correct allocator.  Passing a null invoker to
        //:   'installFunc' leaves the (empty) object unchanged.
        //:
        //: 2 Passing a pointer to a callable object to 'installFunc' yields a
        //:   'Function_Rep' whose target is a copy of the callable object.
        //:
        //: 3 If the callable object type qualifies for the small-object
        //:   optimization (see 'bslstl_function_smallobjectoptimization'),
        //:   then no memory is allocated; otherwise exactly one block is
        //:   allocated (to hold the non-inplace target).  The 'isInplace'
        //:   method will return true if the small-object optimization applies.
        //:
        //: 4 If the callable object uses an allocator, the 'Function_Rep''s
        //:   allocator is passed to the target's constructor.
        //:
        //: 5 The argument to 'installFunc' is moved into the target; no copies
        //:   are made unless the 'Function_Rep''s allocator differs from the
        //:   argument's allocator.
        //:
        //: 6 The 'makeEmpty' method destroys the target and leaves the
        //:   'Function_Rep' empty.
        //:
        //: 7 The destructor destroys the target.
        //:
        //: 8 The above concerns apply to targets of type pointer-to-function,
        //:   pointer-to-member function, pointer to member data, and functor
        //:   classes (of various sizes, allocator-aware or not, bitwise
        //:   movable and not, with and without throwing move constructors).
        //:
        //: 9 If the argument to 'installFunc' is wrapped using
        //:   'bslalg::NothrowMovableWrapper', then the behavior is as though
        //:   it were not wrapped except that small object optimization will
        //:   apply for small functors that have throwing move constructors
        //:   (when otherwise it wouldn't).
        //:
        //: 10 If the argument to 'installFunc' is wrapped in a
        //:   'bsl::reference_wrapper', then the small object optimization will
        //:   always apply.  Only the 'reference_wrapper' will be moved into
        //:   the 'Function_Rep', not the object to which it will refer.
        //:
        //: 11 If the template argument to 'target' is a function (not
        //:   pointer-to-function) type, it always returns null.
        //
        // Plan:
        //: 1 Construct a 'Function_Rep' with a test allocator.  Verify that
        //:   the accessors all return the expected value: 'get_allocator'
        //:   returns the allocator, 'isEmpty' returns 'true', 'invoker'
        //:   returns null, 'target_type' returns 'typeid(void)', and 'target'
        //:   returns null.  Create a callable object to pass to 'installFunc',
        //:   but use a null invoker pointer.  Verify that the 'Function_Rep'
        //:   is still empty.  (C-1)
        //:
        //: 2 Create a callable object.  If the callable object is
        //:   allocator-aware, supply a test allocator to its constructor.
        //:
        //: 3 Using an no-op invoker function (just to get a unique invoker
        //:   pointer), pass the address of the callable object from step 2 to
        //:   'installFunc'.  Verify that the object is no longer empty and
        //:   that the accessors now refer to the new target.  (C-2)
        //:
        //: 4 Use a list-driven approach to repeat steps 1 to 3 with callable
        //:   object types belonging to the categories described in concern
        //:   (C-8).  If the object type is allocator-aware, test the case
        //:   where the allocator supplied to the 'Function_Rep' created in
        //:   step 1 is the same as the allocator supplied to the callable
        //:   object created in step 2 as well as the case where they are
        //:   different.  Verify the expected memory allocation and result of
        //:   'isInplace' (C-3), target allocator (C-4), and number of expected
        //:   moves and copies (C-5).
        //:
        //: 5 Call 'clear' on the 'Function_Rep' object and test that the
        //:   target was destroyed and memory was freed.  Call 'installFunc'
        //:   again, then let the 'Function_Rep' go out of scope (invoking its
        //:   destructor), again verifying verify that the target was destroyed
        //:   and memory was returned to the test allocator.  (C-6, 7)
        //:
        //: 6 Add functors wrapped in 'bslalg::NothrowMovableWrapper' to the
        //:   list in step 4.  (C-9)
        //:
        //: 7 For each type, 'X', in step 4, also test
        //:   'bsl::reference_wrapper<X>'.  (C-10)
        //:
        //: 8 Invoke 'target' with a function type template argument and verify
        //:   that it compiles and returns a null pointer.
        //
        // Testing:
        //  explicit Function_Rep(const allocator_type& allocator);
        //  ~Function_Rep();
        //  void installFunc(FUNC *funcPtr, GenericInvoker *invoker_p);
        //  void makeEmpty();
        //  TP* target() const noexcept;
        //  TP* targetRaw() const noexcept;
        //  allocator_type get_allocator() const noexcept;
        //  GenericInvoker *invoker() const noexcept;
        //  bool isEmpty() const noexcept;
        //  bool isInplace() const noexcept;
        //  const std::type_info& target_type() const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR AND BASIC MANIPULATORS"
                            "\n==================================\n");

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            basicManipulators,
            FUNCTION_PTR_TYPES);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            basicManipulators,
            GEN_FUNCTOR_TYPES(e_SMALL_FUNCTOR),
            GEN_FUNCTOR_TYPES(e_MEDIUM_FUNCTOR),
            GEN_FUNCTOR_TYPES(e_LARGE_FUNCTOR));

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            basicManipulatorsWithAlloc,
            GEN_FUNCTOR_TYPES(e_SMALL_FUNCTOR  | e_HAS_ALLOCATOR),
            GEN_FUNCTOR_TYPES(e_MEDIUM_FUNCTOR | e_HAS_ALLOCATOR),
            GEN_FUNCTOR_TYPES(e_LARGE_FUNCTOR  | e_HAS_ALLOCATOR));

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
            TestDriver,
            basicManipulators,
            NTMOVE_FUNCTOR_TYPES);

        // Test 'target' with function type, installed with a movable
        // reference of a pointer-to-function object.
        {
            bslma::TestAllocator ta;
            Obj                  rep(&ta);
            ASSERT(rep.isEmpty());
            int (*callable)(int) = &simpleFunc;
            rep.installFunc(bslmf::MovableRefUtil::move(callable),
                            &testInvoker1);
            ASSERT(! rep.isEmpty());
            ASSERT(0 != rep.target<int (*)(int)>());
            ASSERT(0 == rep.target<int (int)>());
        }

        // Test 'target' with a function type, installed with an lvalue of
        // pointer-to-function type.
        {
            bslma::TestAllocator ta;
            Obj                  rep(&ta);
            ASSERT(rep.isEmpty());
            int (*callable)(int) = &simpleFunc;
            rep.installFunc(callable, &testInvoker1);
            ASSERT(! rep.isEmpty());
            ASSERT(0 != rep.target<int (*)(int)>());
            ASSERT(0 == rep.target<int (int)>());
        }

#ifndef BSLS_PLATFORM_CMP_IBM
        // Test 'target' with a function type, installed with an lvalue of
        // function type.
        {
            bslma::TestAllocator ta;
            Obj                  rep(&ta);
            ASSERT(rep.isEmpty());
            rep.installFunc(simpleFunc, &testInvoker1);
            ASSERT(! rep.isEmpty());
            ASSERT(0 != rep.target<int (*)(int)>());
            ASSERT(0 == rep.target<int (int)>());
        }
#endif

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Construct a 'Function_Rep'.  Verify that it is empty.
        //:
        //: 2 Use 'installFunc' to set the target to a simple Functor.
        //:   Verify that it is no longer empty and that the target was set.
        //:
        //: 3 Use 'makeEmpty' to return the 'Function_Rep' to the empty state.
        //
        // Testing:
        //  BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta;

        Obj r1(&ta); const Obj& R1 = r1;
        ASSERT(R1.isEmpty());
        ASSERT(&ta == R1.get_allocator());

        SimpleFunctor f(4); SimpleFunctor& F = f;
        r1.installFunc(f, testInvoker1);
        ASSERT(! R1.isEmpty());
        ASSERT(&ta == R1.get_allocator());
        ASSERT(typeid(f) == R1.target_type());
        ASSERT(F == *R1.target<SimpleFunctor>());
        ASSERT(&testInvoker1 == R1.invoker());

        r1.makeEmpty();
        ASSERT(R1.isEmpty());
        ASSERT(&ta == R1.get_allocator());

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
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
