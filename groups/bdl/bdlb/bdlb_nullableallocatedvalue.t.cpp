// bdlb_nullableallocatedvalue.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullableallocatedvalue.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>

#include <bslstl_span.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bsl_algorithm.h>  // 'swap'
#include <bsl_climits.h>
#include <bsl_cstdlib.h>    // 'atoi', 'abs'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <stdint.h>    // uintptr_t

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD
//
// Primary Manipulators and Basic Accessors
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Primary Manipulators:
//
// Basic Accessors:
//
//-----------------------------------------------------------------------------
// CREATORS
// [02] NullableAllocatedValue();
// [02] NullableAllocatedValue(const allocator<char>&);
// [06] NullableAllocatedValue(const nullopt_t&);
// [06] NullableAllocatedValue(const nullopt_t&, const allocator<char>&);
// [06] NullableAllocatedValue(const TYPE&);
// [06] NullableAllocatedValue(const TYPE&, const allocator<char>&);
// [05] NullableAllocatedValue(const NAV&);
// [05] NullableAllocatedValue(const NAV&, const allocator<char>&);
//
// MANIPULATORS
// [02] bool has_value() const;
// [02] bool isNull() const;
// [02] BOOTSTRAP: TYPE& makeValue(const TYPE&);
// [02] TYPE& makeValue();
// [02] void reset() const;
// [02] TYPE& value();
// [02] TYPE value_or(const ANY_TYPE &) const;
// [02] explicit operator bool() const;
// [02] const TYPE& operator* () const;
// [02] const TYPE* operator->() const;
// [02] TYPE& operator* ();
// [02] TYPE* operator->();
// [03] bsl::ostream& operator<<(bsl::ostream&, const b_NV<T>&);
// [07] operator=(const b_NV<TYPE>&);
// [07] operator=(const bsl::nullopt_t&);
// [07] operator=(const TYPE&);
// [02] ~NullableAllocatedValue();
// [08] STREAM& bdexStreamIn(STREAM& stream, int version);
// [13] emplace(ARGS args...);
// [13] emplace(initializer_list, ARGS args...);
//
// ACCESSORS
// [02] const TYPE& value() const;
// [02] bslma::Allocator *allocator() const;
// [02] bsl::allocator<char> get_allocator() const;
// [03] bsl::ostream& print(bsl::ostream&, int, int) const;
// [08] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [08] int maxSupportedBdexVersion() const;
// [08] int maxSupportedBdexVersion(int versionSelector) const;
//
// FREE OPERATORS
// [ 04] bool operator==(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator==(const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator==(const b_NV<TYPE>&, const TYPE&);
// [ 04] bool operator!=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator!=(const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator!=(const b_NV<TYPE>&, const TYPE&);
// [ 04] bool operator<( const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator<( const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator<( const b_NV<TYPE>&, const TYPE&);
// [ 04] bool operator<=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator<=(const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator<=(const b_NV<TYPE>&, const TYPE&);
// [ 04] bool operator>( const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator>( const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator>( const b_NV<TYPE>&, const TYPE&);
// [ 04] bool operator>=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 04] bool operator>=(const TYPE&,       const b_NV<TYPE>&);
// [ 04] bool operator>=(const b_NV<TYPE>&, const TYPE&);
// [ 09] void swap(bdlb::NullableAllocatedValue<TYPE>& other);
// [ 09] void swap(bdlb::NullableAllocatedValue<TYPE>& a, b);
// [ 11] bool operator==(const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator!=(const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator< (const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator<=(const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator>=(const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator> (const NullableAllocatedValue&, bsl::nullopt_t);
// [ 11] bool operator==(bsl::nullopt_t, const NullableAllocatedValue&);
// [ 11] bool operator!=(bsl::nullopt_t, const NullableAllocatedValue&);
// [ 11] bool operator< (bsl::nullopt_t, const NullableAllocatedValue&);
// [ 11] bool operator<=(bsl::nullopt_t, const NullableAllocatedValue&);
// [ 11] bool operator>=(bsl::nullopt_t, const NullableAllocatedValue&);
// [ 11] bool operator> (bsl::nullopt_t, const NullableAllocatedValue&);
// [ 12] bool operator==(const NullableAllocatedValue&, const optional&);
// [ 12] bool operator!=(const NullableAllocatedValue&, const optional&);
// [ 12] bool operator< (const NullableAllocatedValue&, const optional&);
// [ 12] bool operator<=(const NullableAllocatedValue&, const optional&);
// [ 12] bool operator>=(const NullableAllocatedValue&, const optional&);
// [ 12] bool operator> (const NullableAllocatedValue&, const optional&);
// [ 12] bool operator==(const optional&, const NullableAllocatedValue&);
// [ 12] bool operator!=(const optional&, const NullableAllocatedValue&);
// [ 12] bool operator< (const optional&, const NullableAllocatedValue&);
// [ 12] bool operator<=(const optional&, const NullableAllocatedValue&);
// [ 12] bool operator>=(const optional&, const NullableAllocatedValue&);
// [ 12] bool operator> (const optional&, const NullableAllocatedValue&);
//
// DEPRECATED FUNCTIONS
//  [14] const TYPE *addressOr(const TYPE *address) const;
//  [14] TYPE& makeValueInplace(ARGS&&... args);
//  [14] TYPE valueOr(const TYPE& otherValue) const;
//  [14] const TYPE *valueOrNull() const;
// TRAITS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] INCOMPLETE CLASS SUPPORT
// [15] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

typedef bdlb::NullableAllocatedValue<int> NullableInt;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableInt>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableInt>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableInt>::value);

typedef bdlb::NullableAllocatedValue<bsl::string> NullableString;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableString>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableString>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableString>::value);

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate use of this component.
//
// Suppose we want to create a linked list of nodes that contain integers:
//..
    struct LinkedListNode {
        int                                          d_value;
        bdlb::NullableAllocatedValue<LinkedListNode> d_next;
    };
//..
// Note that 'bdlb::NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdlb::NullableValue' requires that the template parameter 'TYPE' be
// a complete type when the class is instantiated.
//
// We can now traverse a linked list and add a new value at the end using the
// following code:
//..
    void addValueAtEnd(LinkedListNode *node, int value)
        // Add the specified 'value' to the end of the list that contains the
        // specified 'node'.
    {
        while (!node->d_next.isNull()) {
            node = &node->d_next.value();
        }

        node->d_next.makeValue();
        node = &node->d_next.value();
        node->d_value = value;
    }
//..

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class SwappableSmall {
    // 'SwappableSmall', used for testing 'swap', does not take an allocator.
  public:
    typedef int DataType;
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableSmall, bslmf::IsBitwiseMoveable);

  private:

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'Swappable' objects created
                                      // since last reset

    // DATA
    DataType d_value;

    // FRIENDS
    friend void swap(SwappableSmall&, SwappableSmall&);

  private:
    // NOT IMPLEMENTED
    void swap(SwappableSmall&);
        // Not implemented.

  public:
    // CLASS METHODS
    static int numObjectsCreated()
        // Return the number of objects created since the last call to 'reset'.
    {
        return s_numObjectsCreated;
    }

    static void reset()
        // Reset the counters to zero.
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
        // Return 'true' if 'swap' has been called since the last call to
        // 'reset', and 'false' otherwise.
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit SwappableSmall(DataType v)
        // Construct an object with the specified 'v'
    : d_value(v)
    {
        ++SwappableSmall::s_numObjectsCreated;
    }

    SwappableSmall(const SwappableSmall& original)
        // Construct an object with the specified 'original'
    : d_value(original.d_value)
    {
        ++SwappableSmall::s_numObjectsCreated;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // MANIPULATORS
    SwappableSmall& operator=(const SwappableSmall& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif

    // ACCESSORS
    DataType value() const
        // Return the value held by this object
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const SwappableSmall& lhs, const SwappableSmall& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' contain the same value,
    // and 'false' otherwise.
{
    return lhs.value() == rhs.value();
}

bool operator!=(const SwappableSmall& lhs, const SwappableSmall& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' do not contain the same
    // value, and 'false' otherwise.
{
    return !(lhs == rhs);
}

// CLASS DATA
bool SwappableSmall::s_swapCalledFlag    = false;
int  SwappableSmall::s_numObjectsCreated = 0;

void swap(SwappableSmall& a, SwappableSmall& b)
    // Swap the values of the specified 'a' and 'b'.
{
    SwappableSmall::s_swapCalledFlag = true;

    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

class SwappableLarge {
    // 'SwappableLarge', used for testing 'swap', does not take an allocator.
    // This class is large enough so that it does't take advantage of the
    // small-object optimization in NullableAllocatedValue.
  public:
    typedef bsl::pair<uintptr_t, uintptr_t> DataType;
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableLarge, bslmf::IsBitwiseMoveable);

  private:

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'Swappable' objects created
                                      // since last reset

    // DATA
    DataType d_value;

    // FRIENDS
    friend void swap(SwappableLarge&, SwappableLarge&);

  private:
    // NOT IMPLEMENTED
    void swap(SwappableLarge&);
        // Not implemented.

  public:
    // CLASS METHODS
    static int numObjectsCreated()
        // Return the number of objects created since the last call to 'reset'.
    {
        return s_numObjectsCreated;
    }

    static void reset()
        // Reset the counters to zero.
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
        // Return 'true' if 'swap' has been called since the last call to
        // 'reset', and 'false' otherwise.
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit SwappableLarge(DataType v)
        // Construct an object with the specified 'v'
    : d_value(v)
    {
        ++SwappableLarge::s_numObjectsCreated;
    }

    SwappableLarge(const SwappableLarge& original)
        // Construct an object with the specified 'original'
    : d_value(original.d_value)
    {
        ++SwappableLarge::s_numObjectsCreated;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // MANIPULATORS
    SwappableLarge& operator=(const SwappableLarge& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif

    // ACCESSORS
    DataType value() const
        // Return the value held by this object
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const SwappableLarge& lhs, const SwappableLarge& rhs)
{
    return lhs.value() == rhs.value();
}

bool operator!=(const SwappableLarge& lhs, const SwappableLarge& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool SwappableLarge::s_swapCalledFlag    = false;
int  SwappableLarge::s_numObjectsCreated = 0;

void swap(SwappableLarge& a, SwappableLarge& b)
{
    SwappableLarge::s_swapCalledFlag = true;

    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}


class SwappableWithAllocator {
    // 'SwappableWithAllocator', used for testing 'swap', takes an allocator.

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'SwappableWithAllocator'
                                      // objects created since last reset

    // DATA
    bslma::Allocator *d_allocator_p;  // held, not owned
    int               d_value;
    bsl::string       d_string;

    // FRIENDS
    friend void swap(SwappableWithAllocator&, SwappableWithAllocator&);

  private:
    // NOT IMPLEMENTED
    void swap(SwappableWithAllocator&);
        // Not implemented.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableWithAllocator,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int numObjectsCreated()
        // Return the number of objects created since the last call to 'reset'.
    {
        return s_numObjectsCreated;
    }

    static void reset()
        // Reset the counters to zero.
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
        // Return 'true' if 'swap' has been called since the last call to
        // 'reset', and 'false' otherwise.
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit
    SwappableWithAllocator(int v, bslma::Allocator *basicAllocator = 0)
        // Construct an object with the specified 'v' and the optionally
        // specified 'basicAllocator'.
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_value(v)
    , d_string(bsl::abs(v), 'x', d_allocator_p)
    {
        ++SwappableWithAllocator::s_numObjectsCreated;
    }

    SwappableWithAllocator(const SwappableWithAllocator&  original,
                           bslma::Allocator              *basicAllocator = 0)
        // Construct an object with the specified 'original' and the optionally
        // specified 'basicAllocator'.
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_value(original.d_value)
    , d_string(original.d_string, d_allocator_p)
    {
        ++SwappableWithAllocator::s_numObjectsCreated;
    }

    ~SwappableWithAllocator()
        // Destroy the object.
    {
        BSLS_ASSERT_OPT(allocator() == d_string.get_allocator().mechanism());
    }

    // MANIPULATORS
    SwappableWithAllocator& operator=(const SwappableWithAllocator& rhs)
        // Exchange the contents of the object with the specified 'rhs'
    {
        d_value  = rhs.d_value;
        d_string = rhs.d_string;

        return *this;
    }

    // ACCESSORS
    bslma::Allocator *allocator() const
        // Return the pointer used to construct this object
    {
        return d_allocator_p;
    }

    const bsl::string& string() const
        // Return a reference offering non-modifiable access to the string
        // managed by this object.
    {
        return d_string;
    }

    int value() const
        // Return the value held by this object
    {
        return d_value;
    }
};

struct ConstructorThatThrows {
    ConstructorThatThrows()
        // Construct an object with default values.
    : d_i(1), d_l(2), d_d(3.0)
    {
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    explicit ConstructorThatThrows(std::initializer_list<int> il)
        // Construct an object with the values from the specified 'il'.
    {
        std::initializer_list<int>::const_iterator it = il.begin();
        d_i = *it++;
        d_l = *it++;
        d_d = *it++;
    }

    ConstructorThatThrows(std::initializer_list<int> il, double d)
        // Construct an object with the values from the specified 'il' and 'd'.
    {
        std::initializer_list<int>::const_iterator it = il.begin();
        d_i = *it++;
        d_l = *it++;
        d_d = d;
    }
#endif

    ConstructorThatThrows(int, long, double)
        // Throw an exception upon construction.
    {
        throw std::runtime_error("ConstructorThatThrows");
    }

    int    d_i;
    long   d_l;
    double d_d;
    };

// FREE OPERATORS
bool operator==(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return lhs.value() == rhs.value() && lhs.string() == rhs.string();
}

bool operator!=(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool SwappableWithAllocator::s_swapCalledFlag    = false;
int  SwappableWithAllocator::s_numObjectsCreated = 0;

void swap(SwappableWithAllocator& a, SwappableWithAllocator& b)
{
    SwappableWithAllocator::s_swapCalledFlag = true;

    if (a.allocator() == b.allocator()) {
        bslalg::SwapUtil::swap(&a.d_value, &b.d_value);

        a.d_string.swap(b.d_string);

        return;                                                       // RETURN
    }

    SwappableWithAllocator futureA(b, a.allocator());
    SwappableWithAllocator futureB(a, b.allocator());

    bslalg::SwapUtil::swap(&a.d_value, &futureA.d_value);
    bslalg::SwapUtil::swap(&b.d_value, &futureB.d_value);

    a.d_string.swap(futureA.d_string);
    b.d_string.swap(futureB.d_string);
}

class Incomplete;  // an incomplete class


void testSwappableLarge (bool verbose,
                         bool veryVerbose,
                         bool veryVeryVerbose,
                         bool veryVeryVeryVerbose)
    // Test the swap functionality for an object that does not fit into the
    // small-object optimization.  Use the specified 'verbose', 'veryVerbose',
    // 'veryVeryVerbose' and 'veryVeryVeryVerbose' to control the amount of
    // information that is output.
{
    typedef SwappableLarge   Swap_t;
    typedef Swap_t::DataType Data_t;
    (void) veryVeryVerbose;

    if (verbose) cout << "Testing w/large a type." << endl;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) cout << "\tSanity test 'SwappableLarge' type." << endl;
    {
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(!Swap_t::swapCalled());

        Swap_t obj1(Data_t(1, 2));
        Swap_t obj2(Data_t(3, 4));
        ASSERT(2 == Swap_t::numObjectsCreated());

        const Swap_t Zobj1(obj1);
        const Swap_t Zobj2(obj2);
        ASSERT(4 == Swap_t::numObjectsCreated());

        ASSERT(obj1 == Zobj1);
        ASSERT(obj2 == Zobj2);

        ASSERT(!Swap_t::swapCalled());
        swap(obj1, obj2);
        ASSERT( Swap_t::swapCalled());
        ASSERT(4 == Swap_t::numObjectsCreated());

        ASSERT(obj2 == Zobj1);
        ASSERT(obj1 == Zobj2);

        Swap_t::reset();
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
    }

    typedef bdlb::NullableAllocatedValue<Swap_t> Obj;

    if (veryVerbose) cout << "\tSwap two null objects." << endl;
    {
        // Swap of two null objects is a no-op.

        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;
        ASSERT(X.isNull());
        ASSERT(Y.isNull());

        // member 'swap'

        Swap_t::reset();

        mX.swap(mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(X.isNull());
        ASSERT(Y.isNull());

        // free 'swap'

        Swap_t::reset();

        swap(mX, mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(X.isNull());
        ASSERT(Y.isNull());
    }

    if (veryVerbose) cout << "\tSwap null with non-null." << endl;
    {
        // Swap of null and non-null moves the value from one object to the
        // other without calling swap for the value type.  For objects that
        // cannot take advantage of the small-object optimization, this just
        // involves swapping pointers; no values are created/destroyed/swapped.

        const Swap_t VV(Data_t(10, 20));

        // 'swap' member called on non-null object.
        {
            Obj mX(VV);  const Obj& X = mX;
            Obj mY;      const Obj& Y = mY;
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());

            // member 'swap'

            Swap_t::reset();

            mX.swap(mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(0 == Swap_t::numObjectsCreated());
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());

            // free 'swap'

            Swap_t::reset();

            swap(mX, mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(0 == Swap_t::numObjectsCreated());
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());
        }

        // 'swap' member called on null object.
        {
            Obj mX;      const Obj& X = mX;
            Obj mY(VV);  const Obj& Y = mY;
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());

            // member 'swap'

            Swap_t::reset();

            mX.swap(mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(0 == Swap_t::numObjectsCreated());
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());

            // free 'swap'

            Swap_t::reset();

            swap(mX, mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(0 == Swap_t::numObjectsCreated());
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());
        }
    }

    if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
    {
        // For objects that cannot take advantage of the small-object
        // optimization, swapping involves swapping pointers; no values are
        // created/destroyed/swapped.

        const Swap_t UU(Data_t(10, 20));
        const Swap_t VV(Data_t(20, 30));

        Obj mX(UU);  const Obj& X = mX;
        Obj mY(VV);  const Obj& Y = mY;
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( UU == X.value());
        ASSERT( VV == Y.value());

        // member 'swap'

        Swap_t::reset();

        mX.swap(mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( VV == X.value());
        ASSERT( UU == Y.value());

        // free 'swap'

        Swap_t::reset();

        swap(mX, mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( UU == X.value());
        ASSERT( VV == Y.value());
    }
}

void testSwappableSmall (bool verbose,
                         bool veryVerbose,
                         bool veryVeryVerbose,
                         bool veryVeryVeryVerbose)
    // Test the swap functionality for an object that fits into the
    // small-object optimization.  Use the specified 'verbose', 'veryVerbose',
    // 'veryVeryVerbose' and 'veryVeryVeryVerbose' to control the amount of
    // information that is output.
{
    typedef SwappableSmall  Swap_t;
    (void) veryVeryVerbose;

    if (verbose) cout << "Testing w/ a small type." << endl;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) cout << "\tSanity test 'SwappableSmall' type." << endl;
    {
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(!Swap_t::swapCalled());

        Swap_t obj1(1);
        Swap_t obj2(2);
        ASSERT(2 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());

        const Swap_t Zobj1(obj1);
        const Swap_t Zobj2(obj2);
        ASSERT(4 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());

        ASSERT(obj1 == Zobj1);
        ASSERT(obj2 == Zobj2);

        ASSERT(!Swap_t::swapCalled());
        swap(obj1, obj2);
        ASSERT( Swap_t::swapCalled());
        ASSERT(4 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());

        ASSERT(obj2 == Zobj1);
        ASSERT(obj1 == Zobj2);

        Swap_t::reset();
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());
    }

    typedef bdlb::NullableAllocatedValue<Swap_t> Obj;

    if (veryVerbose) cout << "\tSwap two null objects." << endl;
    {
        // Swap of two null objects is a no-op.

        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;
        ASSERT(X.isNull());
        ASSERT(Y.isNull());
        ASSERT(0 == da.numAllocations());

        // member 'swap'

        Swap_t::reset();

        mX.swap(mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());
        ASSERT(X.isNull());
        ASSERT(Y.isNull());

        // free 'swap'

        Swap_t::reset();

        swap(mX, mY);
        ASSERT(!Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());
        ASSERT(X.isNull());
        ASSERT(Y.isNull());
    }

    if (veryVerbose) cout << "\tSwap null with non-null." << endl;
    {
        // Swap of null and non-null moves the value from one object to the
        // other without calling swap for the value type.  For objects that
        // can take advantage of the small-object optimization, this means that
        // a new value is created, and the old value is destroyed; no calls to
        // 'swap' are made.

        const Swap_t VV(10);

        // 'swap' member called on non-null object.
        {
            Obj mX(VV);  const Obj& X = mX;
            Obj mY;      const Obj& Y = mY;
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());
            ASSERT(0 == da.numAllocations());

            // member 'swap'

            Swap_t::reset();

            mX.swap(mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(1 == Swap_t::numObjectsCreated());
            ASSERT(0 == da.numAllocations());
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());

            // free 'swap'

            Swap_t::reset();

            swap(mX, mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(1 == Swap_t::numObjectsCreated());
            ASSERT(0 == da.numAllocations());
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());
        }

        // 'swap' member called on null object.
        {
            Obj mX;      const Obj& X = mX;
            Obj mY(VV);  const Obj& Y = mY;
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());
            ASSERT(0 == da.numAllocations());

            // member 'swap'

            Swap_t::reset();

            mX.swap(mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(1 == Swap_t::numObjectsCreated());
            ASSERT(0 == da.numAllocations());
            ASSERT(!X.isNull());
            ASSERT( Y.isNull());
            ASSERT( VV == X.value());

            // free 'swap'

            Swap_t::reset();

            swap(mX, mY);
            ASSERT(!Swap_t::swapCalled());
            ASSERT(1 == Swap_t::numObjectsCreated());
            ASSERT(0 == da.numAllocations());
            ASSERT( X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == Y.value());
        }
    }

    if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
    {
        // Swap of two non-null objects calls swap for the value type.

        const Swap_t UU(10);
        const Swap_t VV(20);

        Obj mX(UU);  const Obj& X = mX;
        Obj mY(VV);  const Obj& Y = mY;
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( UU == X.value());
        ASSERT( VV == Y.value());
        ASSERT(0 == da.numAllocations());

        // member 'swap'

        Swap_t::reset();

        mX.swap(mY);
        ASSERT( Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( VV == X.value());
        ASSERT( UU == Y.value());

        // free 'swap'

        Swap_t::reset();

        swap(mX, mY);
        ASSERT( Swap_t::swapCalled());
        ASSERT(0 == Swap_t::numObjectsCreated());
        ASSERT(0 == da.numAllocations());
        ASSERT(!X.isNull());
        ASSERT(!Y.isNull());
        ASSERT( UU == X.value());
        ASSERT( VV == Y.value());
    }
}

void testSwappableWithAllocator (bool verbose,
                                 bool veryVerbose,
                                 bool veryVeryVerbose,
                                 bool veryVeryVeryVerbose)
    // Test the swap functionality for an object that uses an allocator.  Use
    // the specified 'verbose', 'veryVerbose', 'veryVeryVerbose' and
    // 'veryVeryVeryVerbose' to control the amount of information that is
    // output.
{
    (void) veryVeryVerbose;

    if (verbose) cout << "Testing w/type taking an allocator." << endl;

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        if (veryVerbose) cout
            << "\tSanity test 'SwappableWithAllocator' type." << endl;
        {
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(!SwappableWithAllocator::swapCalled());

            SwappableWithAllocator obj1(1, &scratch);
            SwappableWithAllocator obj2(2, &scratch);
            ASSERT(2 == SwappableWithAllocator::numObjectsCreated());

            const SwappableWithAllocator Zobj1(obj1, &scratch);
            const SwappableWithAllocator Zobj2(obj2, &scratch);
            ASSERT(4 == SwappableWithAllocator::numObjectsCreated());

            ASSERT(obj1 == Zobj1);
            ASSERT(obj2 == Zobj2);

            ASSERT(!SwappableWithAllocator::swapCalled());
            swap(obj1, obj2);
            ASSERT( SwappableWithAllocator::swapCalled());
            ASSERT(4 == SwappableWithAllocator::numObjectsCreated());

            ASSERT(obj2 == Zobj1);
            ASSERT(obj1 == Zobj2);

            SwappableWithAllocator::reset();
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
        }

        typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

        if (veryVerbose) cout << "\tSwap two null objects." << endl;
        {
            // Swap of two null objects is a no-op.

            Obj mX(&oa);  const Obj& X = mX;
            Obj mY(&oa);  const Obj& Y = mY;
            ASSERT(X.isNull());
            ASSERT(Y.isNull());

            // member 'swap'

            SwappableWithAllocator::reset();

            mX.swap(mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(X.isNull());
            ASSERT(Y.isNull());

            // free 'swap'

            SwappableWithAllocator::reset();

            swap(mX, mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(X.isNull());
            ASSERT(Y.isNull());
        }

        if (veryVerbose) cout << "\tSwap null with non-null." << endl;
        {
            // Swap of null and non-null moves the value from one object to
            // the other without calling swap for the value type.

            const SwappableWithAllocator VV(88, &scratch);

            // 'swap' member called on non-null object.
            {
                Obj mX(VV, &oa);  const Obj& X = mX;
                Obj mY(&oa);      const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT( Y.isNull());
                ASSERT( VV == X.value());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT( X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT( Y.isNull());
                ASSERT( VV == X.value());
            }

            // 'swap' member called on null object.
            {
                Obj mX(&oa);      const Obj& X = mX;
                Obj mY(VV, &oa);  const Obj& Y = mY;
                ASSERT( X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == Y.value());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT( Y.isNull());
                ASSERT( VV == X.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT( X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == Y.value());
            }
        }

        if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
        {
            // Swap of two non-null objects calls swap for the value type.

            const SwappableWithAllocator UU(88, &scratch);
            const SwappableWithAllocator VV(99, &scratch);

            Obj mX(UU, &oa);  const Obj& X = mX;
            Obj mY(VV, &oa);  const Obj& Y = mY;
            ASSERT(!X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( UU == X.value());
            ASSERT( VV == Y.value());

            // member 'swap'

            SwappableWithAllocator::reset();

            mX.swap(mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(!X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == X.value());
            ASSERT( UU == Y.value());

            // free 'swap'

            SwappableWithAllocator::reset();

            swap(mX, mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(!X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( UU == X.value());
            ASSERT( VV == Y.value());
        }
    }

    if (verbose) cout << "Testing free 'swap' w/different allocators."
                      << endl;
    {
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
        bslma::TestAllocator za("different", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

        if (veryVerbose) cout << "\tSwap two null objects." << endl;
        {
            // Swap of two null objects is a no-op.

            Obj mX(&oa);  const Obj& X = mX;
            Obj mY(&za);  const Obj& Y = mY;
            ASSERT(X.isNull());
            ASSERT(Y.isNull());

            // free 'swap'

            SwappableWithAllocator::reset();

            swap(mX, mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(X.isNull());
            ASSERT(Y.isNull());
        }

        if (veryVerbose) cout << "\tSwap null with non-null." << endl;
        {
            // Swap of null and non-null moves the value from one object to
            // the other without calling swap for the value type.

            const SwappableWithAllocator VV(88, &scratch);

            // non-null object as first argument
            {
                Obj mX(VV, &oa);  const Obj& X = mX;
                Obj mY(&za);      const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT( Y.isNull());
                ASSERT( VV == X.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(1 == SwappableWithAllocator::numObjectsCreated());
                ASSERT( X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == Y.value());
            }

            // null object as first argument
            {
                Obj mX(&oa);      const Obj& X = mX;
                Obj mY(VV, &za);  const Obj& Y = mY;
                ASSERT( X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(1 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT( Y.isNull());
                ASSERT( VV == X.value());
            }
        }

        if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
        {
            // Swap of two non-null objects calls swap for the value type.

            const SwappableWithAllocator UU(88, &scratch);
            const SwappableWithAllocator VV(99, &scratch);

            Obj mX(UU, &oa);  const Obj& X = mX;
            Obj mY(VV, &za);  const Obj& Y = mY;
            ASSERT(!X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( UU == X.value());
            ASSERT( VV == Y.value());

            // free 'swap'

            SwappableWithAllocator::reset();

            swap(mX, mY);
            ASSERT(!SwappableWithAllocator::swapCalled());
            ASSERT(2 == SwappableWithAllocator::numObjectsCreated());
            ASSERT(!X.isNull());
            ASSERT(!Y.isNull());
            ASSERT( VV == X.value());
            ASSERT( UU == Y.value());
        }
    }
}

template <class TYPE, size_t SIZE>
void comparisonTest(bsl::span<TYPE, SIZE> values,
                    bool                  verbose,
                    bool                  veryVerbose,
                    bool                  veryVeryVerbose)
    // Test the comparison operators for NullableAllocatedValue<TYPE>.  Uses
    // the specified 'values' as test cases.  Uses the specified 'verbose',
    // 'veryVerbose', and 'veryVeryVerbose' flags to control the output.
{
    typedef typename bsl::remove_const<TYPE>::type ValueType;
    typedef bdlb::NullableAllocatedValue<ValueType> Obj;

    // Comparison between two non-null values.
    for (size_t ii = 0; ii < values.size(); ++ii) {
        const ValueType RU = values[ii];
        const Obj       U(RU);
        ASSERT(!U.isNull());

        if (veryVerbose) { T_ P_(ii) P(U) }

        for (size_t jj = 0; jj < values.size(); ++jj) {
            const ValueType RV = values[jj];
            const Obj       V(RV);
            ASSERT(!V.isNull());

            if (veryVeryVerbose) { T_ T_ P_(jj) P(V) }

            // lhs is nullable, but non-null, rhs is raw value

            ASSERTV(U, RV, (ii == jj) == (U == RV));
            ASSERTV(U, RV, (ii != jj) == (U != RV));

            ASSERTV(U, RV, (ii <  jj) == (U <  RV));
            ASSERTV(U, RV, (ii <= jj) == (U <= RV));
            ASSERTV(U, RV, (ii >  jj) == (U >  RV));
            ASSERTV(U, RV, (ii >= jj) == (U >= RV));

            // lhs is raw value, rhs is nullable, but non-null

            ASSERTV(RU, V, (ii == jj) == (RU == V));
            ASSERTV(RU, V, (ii != jj) == (RU != V));

            ASSERTV(RU, V, (ii <  jj) == (RU <  V));
            ASSERTV(RU, V, (ii <= jj) == (RU <= V));
            ASSERTV(RU, V, (ii >  jj) == (RU >  V));
            ASSERTV(RU, V, (ii >= jj) == (RU >= V));

            // both sides are nullable, but non-null

            ASSERTV(U, V, (ii == jj) == (U == V));
            ASSERTV(U, V, (ii != jj) == (U != V));

            ASSERTV(U, V, (ii <  jj) == (U <  V));
            ASSERTV(U, V, (ii <= jj) == (U <= V));
            ASSERTV(U, V, (ii >  jj) == (U >  V));
            ASSERTV(U, V, (ii >= jj) == (U >= V));
        }
    }

    // Comparison between null nullable values and raw value.
    {
        const Obj N;
        ASSERT(N.isNull());

        if (verbose) cout <<
            "\tComparison between null values and raw values" <<
            endl;

        for (size_t ii = 0; ii < values.size(); ++ii) {
            const ValueType RV = values[ii];    // Raw Value

            ASSERT(!(N  == RV));
            ASSERT(!(RV == N ));

            ASSERT(  N  != RV );
            ASSERT(  RV != N  );

            ASSERT(  N  <  RV );
            ASSERT(!(RV <  N ));

            ASSERT(  N  <= RV );
            ASSERT(!(RV <= N ));

            ASSERT(!(N  >  RV));
            ASSERT(  RV >  N  );

            ASSERT(!(N  >= RV));
            ASSERT(  RV >= N  );
        }
    }

    // Comparison between null nullable values and non-null value.
    {
        const Obj N;
        ASSERT(N.isNull());

        if (verbose) cout <<
              "\tComparison between null values and non-null values" <<
              endl;

        for (size_t ii = 0; ii < values.size(); ++ii) {
            const Obj NV(values[ii]);
            ASSERT(!NV.isNull());

            if (veryVerbose) { T_ P_(ii) P_(NV) }

            ASSERT(!(N  == NV));
            ASSERT(!(NV == N ));

            ASSERT(  N  != NV );
            ASSERT(  NV != N  );

            ASSERT(  N  <  NV );
            ASSERT(!(NV <  N ));

            ASSERT(  N  <= NV );
            ASSERT(!(NV <= N ));

            ASSERT(!(N  >  NV));
            ASSERT(  NV >  N  );

            ASSERT(!(N  >= NV));
            ASSERT(  NV >= N  );
        }
    }

    // Comparison between two null values.
    {
        const Obj LN, RN;
        ASSERT(LN.isNull());
        ASSERT(RN.isNull());

        ASSERT(  LN == RN );
        ASSERT(!(LN != RN));

        ASSERT(!(LN <  RN));
        ASSERT(  LN <= RN );

        ASSERT(!(LN >  RN));
        ASSERT(  LN >= RN);
    }
}



template <class TYPE>
void breathingTest(const TYPE& VA,
                   const TYPE& VB,
                   const TYPE& VC,
                   bool        verbose,
                   bool        veryVerbose)
    // Exercises basic functionality, but tests nothing.  Uses the specified
    // 'VA', 'VB', and 'VC' as test values.  Uses the specified 'verbose' and
    // 'veryVerbose' flags to control the output.
{
    typedef TYPE                                    ValueType;
    typedef bdlb::NullableAllocatedValue<ValueType> Obj;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                         "\t\t{ x1:VA }" << endl;
    Obj mX1(VA);  const Obj& X1 = mX1;
    if (veryVerbose) { cout << '\t'; P(X1); }

    if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
    ASSERT(!X1.isNull());
    ASSERT(VA == X1.value());

    if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
    ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                         "\t\t{ x1:VA x2:VA }" << endl;
    Obj mX2(X1);  const Obj& X2 = mX2;
    if (veryVerbose) { cout << '\t'; P(X2); }

    if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
    ASSERT(!X2.isNull());
    ASSERT(VA == X2.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2." << endl;
    ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
    ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                         "\t\t{ x1:VA x2:VA x3:U }" << endl;
    Obj mX3;  const Obj& X3 = mX3;
    if (veryVerbose) { cout << '\t'; P(X3); }

    if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
    ASSERT(X3.isNull());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
    ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
    ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
    ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                         "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
    Obj mX4(X3);  const Obj& X4 = mX4;
    if (veryVerbose) { cout << '\t'; P(X4); }

    if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
    ASSERT(X4.isNull());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x4 <op> x1, x2, x3, x4." << endl;
    ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
    ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
    ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
    ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                         "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
    mX3.makeValue(VB);
    if (veryVerbose) { cout << '\t'; P(X3); }

    if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
    ASSERT(!X3.isNull());
    ASSERT(VB == X3.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3, x4." << endl;
    ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
    ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
    ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
    ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 6. Change x1 using 'reset'."
                         "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
    mX1.reset();
    if (veryVerbose) { cout << '\t'; P(X1); }

    if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
    ASSERT(X1.isNull());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
    ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
    ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
    ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
    ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                         "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
    mX1.makeValue(VC);
    if (veryVerbose) { cout << '\t'; P(X1); }

    if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
    ASSERT(!X1.isNull());
    ASSERT(VC == X1.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
    ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
    ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
    ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
    ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 8. Assign x2 = x1."
                         "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
    mX2 = X1;
    if (veryVerbose) { cout << '\t'; P(X2); }

    if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
    ASSERT(!X2.isNull());
    ASSERT(VC == X2.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
    ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
    ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
    ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
    ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 9. Assign x2 = x3."
                         "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
    mX2 = X3;
    if (veryVerbose) { cout << '\t'; P(X2); }

    if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
    ASSERT(!X2.isNull());
    ASSERT(VB == X2.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3, x4." << endl;
    ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
    ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
    ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
    ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) cout << "\n 10. Assign x1 = x1."
                         "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
    mX1 = X1;
    if (veryVerbose) { cout << '\t'; P(X1); }

    if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
    ASSERT(!X1.isNull());
    ASSERT(VC == X1.value());

    if (veryVerbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3, x4." << endl;
    ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
    ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
    ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
    ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    bslma::TestAllocator *ALLOC = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        LinkedListNode node;
        node.d_value = 3;
        ASSERT(node.d_next.isNull());

        addValueAtEnd(&node, 5);
        ASSERT(!node.d_next.isNull());
        ASSERT(node.d_next.value().d_next.isNull());
        ASSERT(3 == node.d_value);
        ASSERT(5 == node.d_next.value().d_value);

        addValueAtEnd(&node, 53);
        ASSERT(!node.d_next.isNull());
        ASSERT(!node.d_next.value().d_next.isNull());
        ASSERT(node.d_next.value().d_next.value().d_next.isNull());
        ASSERT( 3 == node.d_value);
        ASSERT( 5 == node.d_next.value().d_value);
        ASSERT(53 == node.d_next.value().d_next.value().d_value);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // DEPRECATED FUNCTIONS BROUGHT OVER FROM 'NullableValue'
        //   Extracted from component header file.
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
        //   const TYPE *addressOr(const TYPE *address) const;
        //   TYPE& makeValueInplace(ARGS&&... args);
        //   TYPE valueOr(const TYPE& otherValue) const;
        //   const TYPE *valueOrNull() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
           << "DEPRECATED FUNCTIONS BROUGHT OVER FROM 'NullableValue'" << endl
           << "======================================================" << endl;

        bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other",     veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj       mX(bsl::nullopt);       const Obj& X = mX;
            Obj       mY(123);                const Obj& Y = mY;
            Obj       mZ(456, &oa);           const Obj& Z = mZ;
            ValueType v(789);

            ASSERT(!X.has_value());
            ASSERT(X.addressOr(&v) == &v);
            ASSERT(X.valueOr(v)    == v);
            ASSERT(X.valueOrNull() == NULL);
            ASSERT(X.get_allocator().mechanism() == &da);

            mX.makeValueInplace(4);
            ASSERT( X.has_value());
            ASSERTV(X.value(), 4 == X);

            ASSERT( Y.has_value());
            const ValueType *ptrY = &Y.value();
            ASSERT(Y.addressOr(&v) == ptrY);
            ASSERT(Y.valueOr(v)    == ValueType(123));
            ASSERT(Y.valueOrNull() == ptrY);
            ASSERT(Y.get_allocator().mechanism() == &da);

            ASSERT( Z.has_value());
            const ValueType *ptrZ = &Z.value();
            ASSERT(Z.addressOr(&v) == ptrZ);
            ASSERT(Z.valueOr(v)    == ValueType(456));
            ASSERT(Z.valueOrNull() == ptrZ);
            ASSERT(Z.get_allocator().mechanism() == &oa);
        }

        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const char *source = "0123456789ABCDEF";
            Obj         mX(bsl::nullopt);       const Obj& X = mX;
            Obj         mY(source);             const Obj& Y = mY;
            Obj         mZ("456", &oa);         const Obj& Z = mZ;
            ValueType   v("789");

            ASSERT(!X.has_value());
            ASSERT(X.addressOr(&v) == &v);
            ASSERT(X.valueOr(v)    == v);
            ASSERT(X.valueOrNull() == NULL);
            ASSERT(X.get_allocator().mechanism() == &da);

            ASSERT(!X.has_value());
            mX.makeValueInplace(source, 3, 4);   // (char *, offset, count)
            ASSERT( X.has_value());
            ASSERTV(X.value(), "3456" == X);

            ASSERT( Y.has_value());
            const ValueType *ptrY = &Y.value();
            ASSERT(Y.addressOr(&v) == ptrY);
            ASSERT(Y.valueOr(v)    == ValueType(source));
            ASSERT(Y.valueOrNull() == ptrY);
            ASSERT(Y.get_allocator().mechanism() == &da);

            ASSERT( Z.has_value());
            const ValueType *ptrZ = &Z.value();
            ASSERT(Z.addressOr(&v) == ptrZ);
            ASSERT(Z.valueOr(v)    == ValueType("456"));
            ASSERT(Z.valueOrNull() == ptrZ);
            ASSERT(Z.get_allocator().mechanism() == &oa);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'emplace'
        //   Extracted from component header file.
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
        //   emplace(ARGS args...);
        //   emplace(initializer_list, ARGS args...);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'emplace'" << endl
                          << "=================" << endl;

        bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const char *source = "0123456789ABCDEF";
            Obj         mX(bsl::nullopt);       const Obj& X = mX;

            ASSERT(!X.has_value());
            mX.emplace(source, 3, 4);   // ValueType (char *, offset, count)
            ASSERT( X.has_value());
            ASSERTV(X.value(), "3456" == X);

            mX.emplace(5U, 'J');   // ValueType (size_t, char)
            ASSERT( X.has_value());
            ASSERTV(X.value(), "JJJJJ" == X);
        }
        ASSERT(da.numAllocations() == da.numDeallocations());

        {
            typedef ConstructorThatThrows                   ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj         mX(bsl::nullopt);       const Obj& X = mX;
            ASSERT(!X.has_value());
            mX.emplace();   // ValueType ()
            ASSERT( X.has_value());
            ASSERT(1   == X->d_i);
            ASSERT(2   == X->d_l);
            ASSERT(3   == X->d_d);

            ASSERT(1 == da.numAllocations() - da.numDeallocations());
            try {
                mX.emplace(23, 46L, 69.0);
                ASSERT(false);
            }
            catch (const std::runtime_error &) {
            }

            ASSERT(!X.has_value());
            ASSERT(0 == da.numAllocations() - da.numDeallocations());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            std::initializer_list<int> il = {5, 6, 7};
            mX.emplace(il);
            ASSERT( X.has_value());
            ASSERT(5   == X->d_i);
            ASSERT(6   == X->d_l);
            ASSERT(7   == X->d_d);

            mX.reset();
            ASSERT(!X.has_value());
            mX.emplace(il, 34.0);
            ASSERT( X.has_value());
            ASSERT(5    == X->d_i);
            ASSERT(6    == X->d_l);
            ASSERT(34.0 == X->d_d);
#endif
        }
        ASSERT(da.numAllocations() == da.numDeallocations());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::optional' COMPARISONS
        //  Compare a 'bsl::optional' and a 'NullableAllocatedValue', either
        //  of which can be empty,
        //
        // Concerns:
        //: 1 That an empty 'bsl::optional' orders as a non-empty
        //:   'NullableAllocatedValue'
        //: o That an empty 'bsl::optional' orders as before a non-empty
        //:   'NullableAllocatedValue'
        //: o That an empty 'NullableAllocatedValue' orders as before a
        //:   non-empty 'bsl::optional'
        //: o That an non-empty 'NullableAllocatedValue' orders as with a
        //:   non-empty 'bsl::optional' according to the contained values.
        //:
        //: 2 If the compiler supports 'noexcept', then none of the comparisons
        //:   with 'bsl::nullopt' can throw an exception.
        //
        // Plan:
        //: 1  Conduct the regular test using 'int'.
        //
        // Testing:
        //   bool operator==(const NullableAllocatedValue&, const optional&);
        //   bool operator!=(const NullableAllocatedValue&, const optional&);
        //   bool operator< (const NullableAllocatedValue&, const optional&);
        //   bool operator<=(const NullableAllocatedValue&, const optional&);
        //   bool operator>=(const NullableAllocatedValue&, const optional&);
        //   bool operator> (const NullableAllocatedValue&, const optional&);
        //   bool operator==(const optional&, const NullableAllocatedValue&);
        //   bool operator!=(const optional&, const NullableAllocatedValue&);
        //   bool operator< (const optional&, const NullableAllocatedValue&);
        //   bool operator<=(const optional&, const NullableAllocatedValue&);
        //   bool operator>=(const optional&, const NullableAllocatedValue&);
        //   bool operator> (const optional&, const NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bsl::optional' COMPARISONS"
                             "\n===================================" << endl;

        if (verbose) cout << "\tfor 'NullableAllocatedValue<int>" << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;
            typedef bsl::optional<ValueType>                Optional;

            Obj mX(bsl::nullopt);       const Obj& X = mX;
            Obj mY(10);                 const Obj& Y = mY;

            // Sanity checks
            ASSERT(!X.has_value());
            ASSERT( Y.has_value());

            Optional oA(bsl::nullopt);  const Optional &A = oA;
            Optional oB(5);             const Optional &B = oB;
            Optional oC(10);            const Optional &C = oC;
            Optional oD(15);            const Optional &D = oD;

            // Sanity checks
            ASSERT(!A.has_value());
            ASSERT( B.has_value());
            ASSERT( C.has_value());
            ASSERT( D.has_value());

            // Comparisons between two empty objects
            ASSERT( (X == A) );
            ASSERT(!(X != A) );
            ASSERT(!(X <  A) );
            ASSERT( (X <= A) );
            ASSERT( (X >= A) );
            ASSERT(!(X >  A) );

            ASSERT( (A == X) );
            ASSERT(!(A != X) );
            ASSERT(!(A <  X) );
            ASSERT( (A <= X) );
            ASSERT( (A >= X) );
            ASSERT(!(A >  X) );

            // Comparisons between empty Obj and non-empty Optional
            ASSERT(!(X == B) );
            ASSERT( (X != B) );
            ASSERT( (X <  B) );
            ASSERT( (X <= B) );
            ASSERT(!(X >= B) );
            ASSERT(!(X >  B) );

            ASSERT(!(B == X) );
            ASSERT( (B != X) );
            ASSERT(!(B <  X) );
            ASSERT(!(B <= X) );
            ASSERT( (B >= X) );
            ASSERT( (B >  X) );


            // Comparisons between non-empty Obj and empty Optional
            ASSERT(!(Y == A) );
            ASSERT( (Y != A) );
            ASSERT(!(Y <  A) );
            ASSERT(!(Y <= A) );
            ASSERT( (Y >= A) );
            ASSERT( (Y >  A) );

            ASSERT(!(A == Y) );
            ASSERT( (A != Y) );
            ASSERT( (A <  Y) );
            ASSERT( (A <= Y) );
            ASSERT(!(A >= Y) );
            ASSERT(!(A >  Y) );


            // Comparisons between non-empty Obj and non-empty Optional
            ASSERT(!(Y == B) );
            ASSERT( (Y == C) );
            ASSERT(!(Y == D) );
            ASSERT( (Y != B) );
            ASSERT(!(Y != C) );
            ASSERT( (Y != D) );
            ASSERT(!(Y <  B) );
            ASSERT(!(Y <  C) );
            ASSERT( (Y <  D) );
            ASSERT(!(Y <= B) );
            ASSERT( (Y <= C) );
            ASSERT( (Y <= D) );
            ASSERT( (Y >  B) );
            ASSERT(!(Y >  C) );
            ASSERT(!(Y >  D) );
            ASSERT( (Y >= B) );
            ASSERT( (Y >= C) );
            ASSERT(!(Y >= D) );

            ASSERT(!(B == Y) );
            ASSERT( (C == Y) );
            ASSERT(!(D == Y) );
            ASSERT( (B != Y) );
            ASSERT(!(C != Y) );
            ASSERT( (D != Y) );
            ASSERT( (B <  Y) );
            ASSERT(!(C <  Y) );
            ASSERT(!(D <  Y) );
            ASSERT( (B <= Y) );
            ASSERT( (C <= Y) );
            ASSERT(!(D <= Y) );
            ASSERT(!(B >  Y) );
            ASSERT(!(C >  Y) );
            ASSERT( (D >  Y) );
            ASSERT(!(B >= Y) );
            ASSERT( (C >= Y) );
            ASSERT( (D >= Y) );
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::nullopt_t' COMPARISONS
        //  The type 'bsl::nullopt_t' is not a type suitable for arbitrary
        //  user-created objects, but a proxy for conversion from the literal
        //  value 'bsl::nullopt'.  As such, all testing concerns will be
        //  phrased in terms of the 'bsl::nullopt' literal, rather than
        //  'bsl::nullopt_t'.
        //
        // Concerns:
        //: 1 That 'bsl::nullopt' orders as a null NullableAllocatedValue
        //: o 'bsl::nullopt' orders before any non-null 'NullableAllocatedValue
        //: o 'bsl::nullopt' compares equal to a null NullableAllocatedValue
        //:
        //: 2 If the compiler supports 'noexcept', then none of the comparisons
        //:   with 'bsl::nullopt' can throw an exception.
        //
        // Plan:
        //: 1  Conduct the regular test using 'int' and 'bsl::string'.
        //
        // Testing:
        //   bool operator==(const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator!=(const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator< (const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator<=(const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator>=(const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator> (const NullableAllocatedValue&, bsl::nullopt_t);
        //   bool operator==(bsl::nullopt_t, const NullableAllocatedValue&);
        //   bool operator!=(bsl::nullopt_t, const NullableAllocatedValue&);
        //   bool operator< (bsl::nullopt_t, const NullableAllocatedValue&);
        //   bool operator<=(bsl::nullopt_t, const NullableAllocatedValue&);
        //   bool operator>=(bsl::nullopt_t, const NullableAllocatedValue&);
        //   bool operator> (bsl::nullopt_t, const NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bsl::nullopt_t' COMPARISONS"
                             "\n====================================" << endl;

        if (verbose) cout << "\tfor 'NullableAllocatedValue<int>" << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj mX(bsl::nullopt);          const Obj& X = mX;
            Obj mY(0);                     const Obj& Y = mY;

            // Sanity checks
            ASSERT(!X.has_value());
            ASSERT( Y.has_value());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERT(noexcept(X == bsl::nullopt));
            ASSERT(noexcept(X != bsl::nullopt));
            ASSERT(noexcept(X <  bsl::nullopt));
            ASSERT(noexcept(X <= bsl::nullopt));
            ASSERT(noexcept(X >= bsl::nullopt));
            ASSERT(noexcept(X >  bsl::nullopt));

            ASSERT(noexcept(bsl::nullopt == X));
            ASSERT(noexcept(bsl::nullopt != X));
            ASSERT(noexcept(bsl::nullopt <  X));
            ASSERT(noexcept(bsl::nullopt <= X));
            ASSERT(noexcept(bsl::nullopt >= X));
            ASSERT(noexcept(bsl::nullopt >  X));
#endif


            ASSERT( (X == bsl::nullopt) );
            ASSERT(!(X != bsl::nullopt) );
            ASSERT(!(X <  bsl::nullopt) );
            ASSERT( (X <= bsl::nullopt) );
            ASSERT( (X >= bsl::nullopt) );
            ASSERT(!(X >  bsl::nullopt) );

            ASSERT( (bsl::nullopt == X) );
            ASSERT(!(bsl::nullopt != X) );
            ASSERT(!(bsl::nullopt <  X) );
            ASSERT( (bsl::nullopt <= X) );
            ASSERT( (bsl::nullopt >= X) );
            ASSERT(!(bsl::nullopt >  X) );

            ASSERT(!(Y == bsl::nullopt) );
            ASSERT( (Y != bsl::nullopt) );
            ASSERT(!(Y <  bsl::nullopt) );
            ASSERT(!(Y <= bsl::nullopt) );
            ASSERT( (Y >= bsl::nullopt) );
            ASSERT( (Y >  bsl::nullopt) );

            ASSERT(!(bsl::nullopt == Y) );
            ASSERT( (bsl::nullopt != Y) );
            ASSERT( (bsl::nullopt <  Y) );
            ASSERT( (bsl::nullopt <= Y) );
            ASSERT(!(bsl::nullopt >= Y) );
            ASSERT(!(bsl::nullopt >  Y) );
        }

        if (verbose) cout << "\tfor 'NullableAllocatedValue<string>" << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj        mX(bsl::nullopt);
            const Obj& X = mX;
            Obj        mY("Long string literal: no short string optimization");
            const Obj& Y = mY;

            ASSERT(!X.has_value());
            ASSERT( Y.has_value());

            ASSERT( (X == bsl::nullopt) );
            ASSERT(!(X != bsl::nullopt) );
            ASSERT(!(X <  bsl::nullopt) );
            ASSERT( (X <= bsl::nullopt) );
            ASSERT( (X >= bsl::nullopt) );
            ASSERT(!(X >  bsl::nullopt) );

            ASSERT( (bsl::nullopt == X) );
            ASSERT(!(bsl::nullopt != X) );
            ASSERT(!(bsl::nullopt <  X) );
            ASSERT( (bsl::nullopt <= X) );
            ASSERT( (bsl::nullopt >= X) );
            ASSERT(!(bsl::nullopt >  X) );


            ASSERT(!(Y == bsl::nullopt) );
            ASSERT( (Y != bsl::nullopt) );
            ASSERT(!(Y <  bsl::nullopt) );
            ASSERT(!(Y <= bsl::nullopt) );
            ASSERT( (Y >= bsl::nullopt) );
            ASSERT( (Y >  bsl::nullopt) );

            ASSERT(!(bsl::nullopt == Y) );
            ASSERT( (bsl::nullopt != Y) );
            ASSERT( (bsl::nullopt <  Y) );
            ASSERT( (bsl::nullopt <= Y) );
            ASSERT(!(bsl::nullopt >= Y) );
            ASSERT(!(bsl::nullopt >  Y) );
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INCOMPLETE CLASS SUPPORT
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 NullableAllocatedValue can be instantiated with an incomplete
        //:   class, and the resulting class is not incomplete.
        //
        // Plan:
        //: 1 Define a specialization of NullableAllocatedValue containing an
        //:   incomplete type, and check to see that it is a complete type.
        //:   (C-1)
        //
        // Testing:
        //   INCOMPLETE CLASS SUPPORT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INCOMPLETE CLASS SUPPORT" << endl
                          << "================================" << endl;

        typedef bdlb::NullableAllocatedValue<Incomplete> NAV_Incomplete;
        typedef bdlb::NullableAllocatedValue<int>        NAV_Int;

        // this will fail, because 'Incomplete' has no size
        // ASSERT(sizeof(Incomplete) > 0);

        ASSERT(sizeof(NAV_Incomplete) == sizeof(NAV_Int));
        ASSERT(bslmf::IsBitwiseMoveable<NAV_Incomplete>::value);
        ASSERT(bslma::UsesBslmaAllocator<NAV_Incomplete>::value);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //: 1 Swap of two null objects leaves both objects null.
        //:
        //: 2 Swap of a null object and a non-null object, or of two non-null
        //:   objects, swaps the underlying pointers to the out-of-place
        //:   objects (i.e., no temporary objects are incurred to effect the
        //:   swap).
        //
        // Plan:
        //: 1 Create a class, 'Swappable', with a 'swap' free function
        //:   instrumented to track swap calls and creators instrumented to
        //:   track the number of 'Swappable' objects created.  Instantiate
        //:   'bdlb::NullableAllocatedValue' with 'Swappable' and execute
        //:   operations needed to verify the concerns.  (C-1..2)
        //
        // Testing:
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& other);
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        testSwappableSmall(verbose,
                           veryVerbose,
                           veryVeryVerbose,
                           veryVeryVeryVerbose);
        testSwappableLarge(verbose,
                           veryVerbose,
                           veryVeryVerbose,
                           veryVeryVeryVerbose);
        testSwappableWithAllocator(verbose,
                                   veryVerbose,
                                   veryVeryVerbose,
                                   veryVeryVeryVerbose);

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // STREAMING
        //
        // Concerns:
        //: 1 An object that has been streamed out can be streamed back in, and
        //:   results in the same value.
        //
        // Plan:
        //: 1 Create an object, stream it out, and stream back into a new
        //:   object. Ensure that the new object is equal to the original, and
        //:   that the streaming operation consumed all the data in the stream.
        //:   (C-1)
        //
        // Testing:
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   int maxSupportedBdexVersion() const;
        //   int maxSupportedBdexVersion(int versionSelector) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STREAMING" << endl
                          << "=========" << endl;

        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20140601;

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 Any value must be assignable to an object having any initial
        //:   value without affecting the rhs operand value.  Also, any object
        //:   must be assignable to itself.
        //
        // Plan:
        //: 1  Use 'bsl::string' and 'int' for 'TYPE'.
        //:
        //: 2 Specify a set of unique values.  Construct and initialize all
        //:   combinations (u, v) in the cross product.  Copy construct a
        //:   control w from v, assign v to u, and assert that w == u and
        //:   w == v.  Then test aliasing by copy constructing a control w from
        //:   each u, assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   operator=(const b_NV<TYPE>&);
        //   operator=(const bsl::nullopt_t&);
        //   operator=(const TYPE&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = NAV." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], ALLOC);  const Obj& V = mV;

                    Obj mW(V, ALLOC);  const Obj& W = mW;

                    mU = V;

                    ASSERTV(U, W, U == W);
                    ASSERTV(V, W, V == W);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;
                Obj mW(U,     ALLOC);  const Obj& W = mW;

                mU = U;

                ASSERTV(U, W, U == W);
            }
        }

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], ALLOC);  const Obj& V = mV;

                    Obj mW(V, ALLOC);  const Obj& W = mW;

                    mU = V;

                    ASSERTV(U, W, U == W);
                    ASSERTV(V, W, V == W);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;
                Obj mW(U,     ALLOC);  const Obj& W = mW;

                mU = U;

                ASSERTV(U, W, U == W);
            }
        }

        if (verbose) cout << "\nTesting assignment u = V." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int       NUM_VALUES = 3;
            const ValueType vals[NUM_VALUES] = {"123", "456", "789"};
            Obj             mX[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) {}
                mX[i].makeValue(vals[i]);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(U, ALLOC);

                    mV = vals[j];

                    ASSERTV(i, j, mV, mX[j], mV == mX[j]);
                }
            }
        }

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int       NUM_VALUES = 3;
            const ValueType vals[NUM_VALUES] = {123, 456, 789};
            Obj             mX[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) {}
                mX[i].makeValue(vals[i]);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(U, ALLOC);

                    mV = vals[j];

                    ASSERTV(i, j, mV, mX[j], mV == mX[j]);
                }
            }
        }

        if (verbose) cout << "\nTesting assignment u = bsl::nullopt." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj mX0 = ValueType("123");
            Obj mX1 = ValueType("456");

            ASSERT( mX0.has_value());
            ASSERT( mX1.has_value());

            mX0 = bsl::nullopt;
            ASSERT(!mX0.has_value());
            ASSERT( mX1.has_value());

            mX1 = bsl::nullopt;
            ASSERT(!mX0.has_value());
            ASSERT(!mX1.has_value());
        }

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            Obj mX0 = ValueType(123);
            Obj mX1 = ValueType(456);

            ASSERT( mX0.has_value());
            ASSERT( mX1.has_value());

            mX0 = bsl::nullopt;
            ASSERT(!mX0.has_value());
            ASSERT( mX1.has_value());

            mX1 = bsl::nullopt;
            ASSERT(!mX0.has_value());
            ASSERT(!mX1.has_value());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OTHER CONSTRUCTORS
        //
        // Concerns:
        //: 1 NullableAllocateValue should be constructible from a 'nullopt_t'
        //:   and a 'TYPE' argument, both with and without an allocator.
        //
        // Plan:
        //: 1 Conduct the test using 'int' (does not use allocator) and
        //:   'bsl::string' (uses allocator) for 'TYPE'. Verify that the memory
        //:   for the objects is allocated as expected.
        //
        // Testing:
        //   NullableAllocatedValue(const nullopt_t&);
        //   NullableAllocatedValue(const nullopt_t&, const allocator<char>&);
        //   NullableAllocatedValue(const TYPE&);
        //   NullableAllocatedValue(const TYPE&, const allocator<char>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING OTHER CONSTRUCTORS"
                          << "\n==========================" << endl;

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
        bsl::allocator<char> alloc(&oa);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            {
                Obj mX(bsl::nullopt);  const Obj& X = mX;
                ASSERT(!X.has_value());
                ASSERT(X.allocator() == &da);

                Obj mY(bsl::nullopt, alloc);  const Obj& Y = mY;
                ASSERT(!Y.has_value());
                ASSERT(Y.allocator() == &oa);
            }

            ASSERT(0 == da.numAllocations());
            ASSERT(0 == oa.numAllocations());

            {
                const ValueType value = 3;

                Obj mX(value);  const Obj& X = mX;
                ASSERT( X.has_value());
                ASSERT(X.allocator() == &da);
                ASSERT(value == X.value());

                Obj mY(value, alloc);  const Obj& Y = mY;
                ASSERT( Y.has_value());
                ASSERT(Y.allocator() == &oa);
                ASSERT(value == Y.value());
            }

            ASSERT(0 == da.numAllocations());
            ASSERT(0 == oa.numAllocations());
        }

        if (verbose) cout << "\nUsing bdlb::NullableAllocatedValue<"
                          << "bsl::string>." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            {
                Obj mX(bsl::nullopt);  const Obj& X = mX;
                ASSERT(!X.has_value());
                ASSERT(X.allocator() == &da);

                Obj mY(bsl::nullopt, alloc);  const Obj& Y = mY;
                ASSERT(!Y.has_value());
                ASSERT(Y.allocator() == &oa);
            }

            ASSERT(0 == da.numAllocations());
            ASSERT(0 == oa.numAllocations());

            {
                const ValueType value = "123"; // short string

                Obj mX(value);  const Obj& X = mX;
                ASSERT( X.has_value());
                ASSERT(X.allocator() == &da);
                ASSERT(value == X.value());

                Obj mY(value, alloc);  const Obj& Y = mY;
                ASSERT( Y.has_value());
                ASSERT(Y.allocator() == &oa);
                ASSERT(value == Y.value());
            }

            ASSERT(1 == da.numAllocations());
            ASSERT(1 == oa.numAllocations());
            ASSERT(da.numDeallocations() == da.numAllocations());
            ASSERT(oa.numDeallocations() == oa.numAllocations());

            {
                const ValueType value = "1234567890ABCDF" // long string
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                ASSERT(value.size() >= 40);

                Obj mX(value);  const Obj& X = mX;
                ASSERT( X.has_value());
                ASSERT(X.allocator() == &da);
                ASSERT(value == X.value());

                Obj mY(value, alloc);  const Obj& Y = mY;
                ASSERT( Y.has_value());
                ASSERT(Y.allocator() == &oa);
                ASSERT(value == Y.value());
            }

            // 1 allocation for the creation of 'value' - uses 'da'
            // 2 allocations for the creation of the 'NullableAllocatedValue';
            // one for the string object, and one for the string data.
            ASSERT(4 == da.numAllocations());
            ASSERT(3 == oa.numAllocations());
            ASSERT(da.numDeallocations() == da.numAllocations());
            ASSERT(oa.numDeallocations() == oa.numAllocations());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 Any value must be copy constructible without affecting the
        //:   argument.
        //
        // Plan:
        //: 1 Conduct the test using 'int' (does not use allocator) and
        //:   'bsl::string' (uses allocator) for 'TYPE'.
        //:
        //: 2 Specify a set whose elements have substantial and varied
        //:   differences in value.  For each element in S, construct and
        //:   initialize identical objects W and X using tested methods.  Then
        //:   copy construct Y from X and use the equality operator to assert
        //:   that both X and Y have the same value as W.
        //
        // Testing:
        //   NullableAllocatedValue(const NAV&);
        //   NullableAllocatedValue(const NAV&, const allocator<char>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

        if (verbose) cout << "\nUsing bdlb::NullableAllocatedValue<"
                          << "bsl::string>." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X, ALLOC);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 That all 6 comparison operators work properly.  A null values is
        //:   always not equal to, and less than, any non-null value.  Two null
        //:   values are equal.
        //
        // Plan:
        //: 1 Use 'int' for 'TYPE'.
        //:
        //: 2 Create an array 'values' of 7 non-null nullable objects, none
        //:   equal to each other, sorted in value from lowest to highest.
        //:
        //: 3 Try comparisons of two non-null nullable objects.
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create 'const int UR = values[ii]' and non-null
        //:     'const Obj U(UV)'.
        //:
        //:   o Within that loop, iterate 'jj' through the indexes of 'values',
        //:     and in that loop create 'const int VR = values[jj]' and
        //:     non-null 'const Obj V(VR)'.
        //:
        //:   o Try every possible comparison of 'UR' and 'V', with 'UR' on the
        //:     lhs and 'V' on the rhs, whose results should match the boolean
        //:     results of comparing 'ii' and 'jj'.
        //:
        //:   o Try every possible comparison of 'U' and 'VR', with 'U' on the
        //:     lhs and 'VR' on the rhs, whose results should match the boolean
        //:     results of comparing 'ii' and 'jj'.
        //:
        //:   o Try every possible comparison of 'U' and 'V', whose results
        //:     should match the boolean results of comparing 'ii' and 'jj'.
        //:
        //: 4 Try comparisons between a null nullable value and non-nullable
        //:   'int'.
        //:   o Create a null nullable value 'N'.
        //:
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create 'const int RV(values[ii])'.
        //:
        //:   o In the loop, do all possible comparisons of 'N' and 'RV', and
        //:     'N' should always be less than 'RV', regardless of the value of
        //:     'RV'.
        //:
        //: 5 Try comparisons of one null object and one non-null object.
        //:   o Create a null nullable value 'N'.
        //:
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create non-null 'const Obj NV(values[ii])'.
        //:
        //:   o In the loop, do all possible comparisons of 'N' and 'NV', and
        //:     'N' should always be less than 'NV', regardless of the value of
        //:     'NV'.
        //:
        //: 6 Try comparisons of two null nullable objects.
        //:   o Create two null nullable objects 'LN' and 'RN'
        //:
        //:   o Try every possible comparison between them, they are always
        //:     equal, not greater, not less.
        //
        // TESTING:
        //   bool operator==(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator==(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator==(const b_NV<TYPE>&, const TYPE&);
        //   bool operator!=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator!=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator!=(const b_NV<TYPE>&, const TYPE&);
        //   bool operator<( const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator<( const TYPE&,       const b_NV<TYPE>&);
        //   bool operator<( const b_NV<TYPE>&, const TYPE&);
        //   bool operator<=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator<=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator<=(const b_NV<TYPE>&, const TYPE&);
        //   bool operator>( const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator>( const TYPE&,       const b_NV<TYPE>&);
        //   bool operator>( const b_NV<TYPE>&, const TYPE&);
        //   bool operator>=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator>=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator>=(const b_NV<TYPE>&, const TYPE&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING COMPARISON OPERATORS\n"
                             "============================\n";

        {
            typedef int ValueType;

            const ValueType values[] =
                             { INT_MIN, -1000, -123, 0, +123, +1000, INT_MAX };
            enum { k_NUM_VALUES = sizeof values / sizeof *values };

            comparisonTest(bsl::span<const ValueType, k_NUM_VALUES>(values),
                                        verbose, veryVerbose, veryVeryVerbose);

      }

      {
          typedef bsl::string ValueType;

          const ValueType values[] = { "a", "ab", "abc", "def" };
          enum { k_NUM_VALUES = sizeof values / sizeof *values };

          comparisonTest(bsl::span<const ValueType, k_NUM_VALUES>(values),
                                        verbose, veryVerbose, veryVeryVerbose);

      }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //: 1 The print method and output (<<) operator work as expected.
        //
        // Plan:
        //: 1 Conduct the test using 'int' for 'TYPE'.
        //:
        //:  For a set of values, check that the 'print' function and the
        //:  output (<<) operator work as expected. (C-1)
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const b_NV<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRINT METHOD AND OUTPUT (<<) OPERATOR"
                          << "\n============================================="
                          << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        const ValueType VALUE1          = 123;
        const char      NULL_RESULT[]   = "NULL";
        const char      VALUE1_RESULT[] = "123";

        if (verbose) cout << "\nTesting 'print' Method." << endl;
        {
            {
                Obj               mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting Output (<<) Operator." << endl;
        {
            {
                Obj               mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS & BASIC ACCESSORS
        //   Verify that the primary manipulators and basic accessors work as
        //   expected.
        //
        // Concerns:
        //: 1 The default constructor creates a null object.
        //:
        //: 2 'makeValue()' sets the value to the default value for 'TYPE'.
        //:
        //: 3 'makeValue(const TYPE&)' sets the value appropriately.
        //
        // Plan:
        //: 1 Conduct the test using 'int' (does not use allocator) and
        //:   'bsl::string' (uses allocator) for 'TYPE'.
        //:
        //: 2 First, verify the default constructor by testing that the
        //:   resulting object is null.
        //:
        //: 3 Next, verify that the 'makeValue' function works by making a
        //:   value equal to the value passed into 'makeValue'.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   NullableAllocatedValue();
        //   NullableAllocatedValue(const allocator<char>&);
        //   ~NullableAllocatedValue();
        //   TYPE& makeValue();
        //   BOOTSTRAP: TYPE& makeValue(const TYPE&);
        //   bool has_value() const;
        //   bool isNull() const;
        //   void reset() const;
        //   const TYPE& value() const;
        //   TYPE value_or(const ANY_TYPE &) const;
        //   TYPE& value();
        //   bslma::Allocator *allocator() const;
        //   bsl::allocator<char> get_allocator() const;
        //   explicit operator bool() const;
        //   const TYPE& operator* () const;
        //   const TYPE* operator->() const;
        //   TYPE& operator* ();
        //   TYPE* operator->();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS & BASIC ACCESSORS"
                          << "\n=============================================="
                          << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>'."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX;  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT( X.isNull());
                ASSERT(!X.has_value());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT( X.has_value());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(3);  // set some random value
                mX.makeValue();   // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT( X.has_value());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT( X.has_value());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;
                const ValueType VALUE2 = 456;

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT( X.has_value());
                ASSERTV(X.value(), VALUE2 == X.value());
            }

            if (veryVerbose) cout << "\tTesting 'reset'." << endl;
            {
                Obj mX0;
                Obj mX1(ValueType(123));

                ASSERT( mX0.isNull());
                ASSERT(!mX0.has_value());
                ASSERT(!mX1.isNull());
                ASSERT( mX1.has_value());
                mX0.reset();
                mX1.reset();
                ASSERT( mX0.isNull());
                ASSERT(!mX0.has_value());
                ASSERT( mX1.isNull());
                ASSERT(!mX1.has_value());
            }

            if (veryVerbose) cout << "\tTesting operator bool." << endl;
            {
                Obj mX;                  const Obj& X = mX;
                Obj mY(ValueType(123));  const Obj& Y = mY;
//                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT( X.isNull());
                if (X) {
                    ASSERTV(X, false);
                }
                else {
                    ASSERTV(X, true);
                }

//                if (veryVeryVerbose) { T_ T_ P(Y) }
                ASSERT(!Y.isNull());
                if (Y) {
                    ASSERTV(Y, true);
                }
                else {
                    ASSERTV(Y, false);
                }
            }

            if (veryVerbose) cout << "\tTesting value_or." << endl;
            {
                Obj mX;                  const Obj& X = mX;
                Obj mY(ValueType(123));  const Obj& Y = mY;

                ASSERT( X.isNull());
                ASSERT(!Y.isNull());

                ValueType fromX = X.value_or(456LL);
                ValueType fromY = Y.value_or(456LL);

                ASSERT(fromX == 456);
                ASSERT(fromY == 123);
            }

            if (veryVerbose) cout << "\tTesting operator * and ->" << endl;
            {
                Obj mY(ValueType(123));  const Obj& Y = mY;
                ASSERT(!Y.isNull());

                ASSERT(123 == *Y);
                ASSERT(123 == *mY);
            }
        }

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<"
                          << "bsl::string>'." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX(ALLOC);  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
                ASSERT(X.allocator() == ALLOC);
                ASSERT(X.get_allocator().mechanism() == ALLOC);
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue("3");  // set some random value
                mX.makeValue();     // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE1 == X.value());
                ASSERT(X.allocator() == ALLOC);
                ASSERT(X.get_allocator().mechanism() == ALLOC);
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";
                const ValueType VALUE2 = "456";

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE2 == X.value());
            }

            if (veryVerbose) cout << "\tTesting 'reset'." << endl;
            {
                Obj mX0;
                Obj mX1(ValueType("123"));

                ASSERT( mX0.isNull());
                ASSERT(!mX1.isNull());
                mX0.reset();
                mX1.reset();
                ASSERT( mX0.isNull());
                ASSERT( mX1.isNull());
            }

            if (veryVerbose) cout << "\tTesting operator bool." << endl;
            {
                Obj mX;                    const Obj& X = mX;
                Obj mY(ValueType("123"));  const Obj& Y = mY;

//                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT( X.isNull());
                if (X) {
                    ASSERTV(X, false);
                }
                else {
                    ASSERTV(X, true);
                }

//                if (veryVeryVerbose) { T_ T_ P(Y) }
                ASSERT(!Y.isNull());
                if (Y) {
                    ASSERTV(Y, true);
                }
                else {
                    ASSERTV(Y, false);
                }
            }

            if (veryVerbose) cout << "\tTesting value_or." << endl;
            {
                Obj mX;                    const Obj& X = mX;
                Obj mY(ValueType("123"));  const Obj& Y = mY;

                ASSERT( X.isNull());
                ASSERT(!Y.isNull());

                ValueType fromX = X.value_or("456");
                ValueType fromY = Y.value_or("456");

                ASSERT(fromX == "456");
                ASSERT(fromY == "123");
            }

            if (veryVerbose) cout << "\tTesting operator * and ->" << endl;
            {
                Obj mY(ValueType("123"));  const Obj& Y = mY;
                ASSERT(!Y.isNull());

                ASSERT("123" == *Y);
                ASSERT("123" == *mY);
                ASSERT(3U ==  Y->size());
                ASSERT(3U == mY->size());
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //:
        // Plan:
        //:  1 Create four test objects using the default, value, and copy
        //:  constructors.  Exercise the basic value-semantic methods and the
        //:  equality operators using the test objects.  Invoke the primary
        //:  manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //:  operator without [9] and with [10] aliasing.  Use the basic
        //:  accessors to verify the expected results.  Display object values
        //:  frequently in verbose mode.  Note that 'VA', 'VB', and 'VC' denote
        //:  unique, but otherwise arbitrary, object values, while 'U' denotes
        //:  the valid, but "unknown", default object value.
        //:
        //:  2 Create an object x1 (init. to VA)    { x1:VA                  }
        //:  3 Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //:  4 Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //:  5 Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //:  6 Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //:  7 Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //:  8 Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //:  9 Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //: 10 Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //: 11 Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        {
            typedef bsl::string ValueType;

            // some values
            const ValueType VA("The");
            const ValueType VB("Breathing");
            const ValueType VC("Test");

            breathingTest(VA, VB, VC, verbose, veryVerbose);
        }

        {
            typedef int ValueType;

            // possible values
            const ValueType VA(123);
            const ValueType VB(234);
            const ValueType VC(345);

            breathingTest(VA, VB, VC, verbose, veryVerbose);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
