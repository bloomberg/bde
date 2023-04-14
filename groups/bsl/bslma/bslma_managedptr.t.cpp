// bslma_managedptr.t.cpp                                             -*-C++-*-
#include <bslma_managedptr.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorproctor.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <utility>      // 'std::move'
#include <cstring>      // 'std::strlen', 'std::strncpy'

#include <stddef.h>     // 'size_t'
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

// These bde_verify warning suppressions should be eliminated in due course.
// BDE_VERIFY pragma: -BW01  // bdewrap recommendation
// BDE_VERIFY pragma: -FD01  // Function needs contract


#if defined BSLS_PLATFORM_CMP_SUN
# define BSLSTL_MANAGEDPTR_LIMIT_TESTING_COMPLEXITY 1
// Some compilers struggle for resources when trying to compile all of the
// template instantiations required for this test driver.  We will perform
// slightly less thorough testing on those compilers, testing sufficient to
// rely on the more thorough testing offered by platforms that are not as
// constrained.
#endif

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bslma_managedptr' component provides a small number of classes that
// combine to provide a common solution to the problem of managing and
// transferring ownership of a dynamically allocated object.  It further
// contains a number of private classes to supply important implementation
// details, while the test driver introduces a reasonable amount of test
// machinery in order to carefully observe the correct handling of callbacks.
// We choose to test each class in turn, starting with the test machinery,
// according to their internal levelization in the component implementation.
//
// [ 2]   Test machinery
// [ 3]   imp. class bslma::ManagedPtr_Ref
// [4-5]  (tested classes migrated to their own components)
// [4-13] class bslma::ManagedPtr
// [14]   class bslma::ManagedPtrNilDeleter   [DEPRECATED]
// [15]   class bslma::ManagedPtrNoOpDeleter
//
// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition of
// that macro immediately above the test, to easily enable compiling that test
// while in development.  Below is the list of all macros that control the
// availability of these tests:
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_FROM_DERIVED_TYPE_LVALUE
//  #define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_WITH_EXTRA_PTR
//-----------------------------------------------------------------------------
//                         bslma::ManagedPtr
//                         -----------------
// We test 'bslma::ManagedPtr' incrementally, increasing the functionality that
// can be relied upon in later tests.  Starting with the default constructor,
// we will demonstrate that any valid state can be attained with the methods
// 'load' and 'loadAlias'.  In turn, this will be used to prove the correct
// behavior of the basic accessors, which are assumed correct but minimally
// relied up for the initial tests.  This allows us to test the remaining
// constructors, the move-semantic operations, and finally any other operations
// of the class.  Negative testing will ensure that all expected assertions are
// present and correct in the implementation, and a number of compile-fail
// tests will ensure that attempts to construct invalid managed pointers are
// caught early by the compiler, ideally with a helpful error diagnostic.
//-----------------------------------------------------------------------------
// [ 4] ManagedPtr();
// [ 4] ManagedPtr(bsl::nullptr_t, bsl::nullptr_t = 0);
// [ 8] ManagedPtr(OTHER *ptr);
// [ 9] ManagedPtr(ManagedPtr& original);
// [ 9] ManagedPtr(ManagedPtr&& original);
// [ 9] ManagedPtr(MovableRef<ManagedPtr<OTHER>> original);
// [ 9] ManagedPtr(ManagedPtr_Ref<TYPE> ref);
// [ 6] ManagedPtr(ManagedPtr<OTHER>& alias, TYPE *ptr);
// [ 6] ManagedPtr(ManagedPtr<OTHER>&& alias, TYPE *ptr);
// [ 8] ManagedPtr(OTHER *ptr, FACTORY *factory);
// [  ] ManagedPtr(bsl::nullptr_t, FACTORY *factory);
// [ 8] ManagedPtr(TYPE *ptr, void *cookie, DeleterFunc deleter);
// [ 8] ManagedPtr(OTHER *ptr, void *cookie, DeleterFunc deleter);
// [  ] ManagedPtr(OTHER *, void *, void (*)(OTHER_BASE *, void *));
// [ 8] ManagedPtr(OTHER *, COOKIE *, void (*)(OTHER_BASE *, COOKIE_BASE *));
// [ 5] ~ManagedPtr();
// [ 9] operator ManagedPtr_Ref<BDE_OTHER_TYPE>();
// [ 5] void load(TYPE *ptr);
// [ 5] void load(TYPE *ptr, FACTORY *factory);
// [ 5] void load(TYPE *ptr, void *cookie, DeleterFunc deleter);
// [ 5] void load(nullptr_t=0, void *cookie=0, DeleterFunc deleter=0);
// [ 5] void load(TYPE *ptr, bsl::nullptr_t, void (*del)(BASE *, void *));
// [ 5] void load(TYPE *, FACTORY *, void(*)(TYPE_BASE *, FACTORY_BASE *))
// [ 6] void loadAlias(ManagedPtr<OTHER>& alias, TYPE *ptr);
// [13] void reset();
// [11] void swap(ManagedPtr& rhs);
// [12] ManagedPtr& operator=(ManagedPtr& rhs);
// [12] ManagedPtr& operator=(ManagedPtr&& rhs);
// [12] ManagedPtr& operator=(MovableRef<ManagedPtr<OTHER>> rhs);
// [12] ManagedPtr& operator=(ManagedPtr_Ref<ELEMENT_TYPE> ref);
// [12] ManagedPtr& operator=(ManagedPtr<OTHER>&& rhs);
// [13] void clear();
// [13] bsl::pair<TYPE *, ManagedPtrDeleter> release();
// [  ] TARGET_TYPE *release(ManagedPtrDeleter *deleter);
// [ 7] operator BoolType() const;
// [ 7] TYPE& operator*() const;
// [ 7] TYPE *operator->() const;
// [ 7] TYPE *get() const;
// [ 7] TYPE *ptr() const;
// [ 7] const ManagedPtrDeleter& deleter() const;
//-----------------------------------------------------------------------------
// [ 8] ManagedPtr(TYPE *ptr, void *cookie, void(*deleter)(TYPE*, void*));
// [ 8] ManagedPtr(OTHER *, bsl::nullptr_t, void(*)(BASE *, void *));
//
//                       bslma::ManagedPtrUtil
//                       ---------------------
//-----------------------------------------------------------------------------
// [14] void noOpDeleter(void *, void *);
// [18] ManagedPtr allocateManaged(Allocator *alloc, ARGS&&... args);
// [18] ManagedPtr makeManaged(ARGS&&... args);
//-----------------------------------------------------------------------------
//
//                 bslma::ManagedPtrNilDeleter<TYPE>
//                 ---------------------------------
//-----------------------------------------------------------------------------
// [15] void deleter(void *, void *);
//-----------------------------------------------------------------------------
//
//                       bslma::ManagedPtr_Ref
//                       ---------------------
//-----------------------------------------------------------------------------
// [ 3] ManagedPtr_Ref(ManagedPtr_Members *base, TARGET_TYPE *target);
// [ 3] ManagedPtr_Ref(const bslma::ManagedPtr_Ref& original);
// [ 3] ~ManagedPtr_Ref();
// [ 3] ManagedPtr_Ref& operator=(const bslma::ManagedPtr_Ref&);
// [ 3] ManagedPtr_Members *base() const;
// [ 3] TARGET_TYPE *target() const;
//-----------------------------------------------------------------------------
//
//                         Test Machinery
//                         --------------
//-----------------------------------------------------------------------------
// [ 2] class MyTestObject
// [ 2] class MyDerivedObject
// [ 2] class MySecondDerivedObject
// [ 2] class MyDerivedDerivedObject
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST MACHINERY
// [ 7] (implicit) bool operator!() const;  // via operator BoolType()
// [19] USAGE EXAMPLE
// [20] CASTING EXAMPLES
// [-1] VERIFYING FAILURES TO COMPILE

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

#define ASSERT  BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define Q       BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P       BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_      BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_      BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_      BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR)     BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR)     BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)          BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)          BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
namespace {

typedef bslma::ManagedPtrUtil Util;

bool g_verbose;
bool g_veryVerbose;
bool g_veryVeryVerbose;
bool g_veryVeryVeryVerbose;

class MyTestObject;
class MyDerivedObject;

// BDE_VERIFY pragma: push     // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -CC01    // C-style casts are used for readability
// BDE_VERIFY pragma: -FABC01  // Functions ordered for expository purpose

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// The following types are the primary test types used within the policy driven
// generative testing framework.  They are also used, undocumented, in some of
// the usage examples.

                           // ==================
                           // class MyTestObject
                           // ==================

class MyTestObject {
    // This test-class serves three purposes.  It provides a base class for the
    // test classes in this test driver, so that derived -> base conversions
    // can be tested.  It also signals when its destructor is run by
    // incrementing an externally managed counter, supplied when each object is
    // created.  Finally, it exposes an internal data structure that can be use
    // to demonstrate the 'bslma::ManagedPtr' aliasing facility.

    // DATA
    volatile int *d_deleteCounter_p;
    mutable int   d_value[2];

  public:
    // CREATORS
    explicit MyTestObject(int *counter);
        // Create a 'MyTestObject' using the specified 'counter' to record when
        // this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    // MyTestObject(const MyTestObject& other) = default;
    // MyTestObject operator=(const MyTestObject& rhs) = default;

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const;
        // Return the address of the counter used to track when this object's
        // destructor is run.

    int *valuePtr(int index = 0) const;
        // Return the address of the value associated with the optionally
        // specified 'index', and the address of the first such object if no
        // 'index' is specified.
};

                           // =====================
                           // class MyDerivedObject
                           // =====================

class MyDerivedObject : public MyTestObject {
    // This test-class has the same destructor-counting behavior as
    // 'MyTestObject', but offers a derived class in order to test correct
    // behavior when handling derived->base conversions.

  public:
    // CREATORS
    explicit MyDerivedObject(int *counter);
        // Create a 'MyDerivedObject' using the specified 'counter' to record
        // when this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    // MyDerivedObject(const MyDerivedObject& other);
    // MyDerivedObject operator=(const MyDerivedObject& rhs);

    ~MyDerivedObject();
        // Increment the stored reference to a counter by 100, then destroy
        // this object.
};

                           // ===========================
                           // class MySecondDerivedObject
                           // ===========================

class MySecondDerivedObject : public MyTestObject {
    // This test-class has the same destructor-counting behavior as
    // 'MyTestObject', but offers a second, distinct, derived class in order to
    // test correct behavior when handling derived->base conversions.

  public:
    // CREATORS
    explicit MySecondDerivedObject(int *counter);
        // Create a 'MySecondDerivedObject' using the specified 'counter' to
        // record when this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    // MySecondDerivedObject(const MySecondDerivedObject& other);
    // MySecondDerivedObject operator=(const MySecondDerivedObject& rhs);

    ~MySecondDerivedObject();
        // Increment the stored reference to a counter by 10000, then destroy
        // this object.
};

                           // ============================
                           // class MyDerivedDerivedObject
                           // ============================

class MyDerivedDerivedObject : public MyDerivedObject {
    // This test-class has the same destructor-counting behavior as
    // 'MyTestObject', but offers a second, distinct, derived class in order to
    // test correct behavior when handling derived->base conversions.

  public:
    // CREATORS
    explicit MyDerivedDerivedObject(int *counter);
        // Create a 'MyDerivedDerivedObject' using the specified 'counter' to
        // record when this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    // MyDerivedDerivedObject(const MyDerivedDerivedObject& other);
    // MyDerivedDerivedObject operator=(const MyDerivedDerivedObject& rhs);

    ~MyDerivedDerivedObject();
        // Increment the stored reference to a counter by 1000000, then destroy
        // this object.
};

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLES {

///Example 1: Implementing a protocol
/// - - - - - - - - - - - - - - - - -
// We demonstrate using 'bslma::ManagedPtr' to configure and return a managed
// object implementing an abstract protocol.
//
// First we define our protocol, 'Shape', a type of object that knows how to
// compute its 'area'.  Note that for expository reasons only, we do *not* give
// 'Shape' a virtual destructor.
//..
    struct Shape {
        virtual double area() const = 0;
            // Return the 'area' of this shape.
    };
//..
// Then we define a couple of classes that implement the 'Shape' protocol, a
// 'Circle' and a 'Square'.
//..
    class Circle : public Shape {
      private:
        // DATA
        double d_radius;

      public:
        // CREATORS
        explicit Circle(double radius);
            // Create a 'Circle' object having the specified 'radius'.

        // ACCESSORS
        virtual double area() const;
            // Return the area of this Circle, given by the formula pi*r*r.
    };

    class Square : public Shape {
      private:
        // DATA
        double d_sideLength;

      public:
        // CREATORS
        explicit Square(double side);
            // Create a 'Square' having sides with length of the specified
            // 'side'.

        // ACCESSORS
        virtual double area() const;
            // Return the area of this Square, given by the formula side*side
    };
//..
// Next we implement the methods for 'Circle' and 'Square'.
//..
    Circle::Circle(double radius)
    : d_radius(radius)
    {
    }

    double Circle::area() const
    {
        return 3.141592653589793238462 * d_radius * d_radius;
    }

    Square::Square(double side)
    : d_sideLength(side)
    {
    }

    double Square::area() const
    {
        return d_sideLength * d_sideLength;
    }
//..
// Then we define an enumeration that lists each implementation of the 'Shape'
// protocol.
//..
    struct Shapes {
        enum VALUES { SHAPE_CIRCLE, SHAPE_SQUARE };
    };
//..
// Now we can define a function that will return a 'Circle' object or a
// 'Square' object according to the specified 'kind' parameter, and having its
// 'dimension' specified by the caller.
//..
    bslma::ManagedPtr<Shape> makeShape(Shapes::VALUES kind, double dimension)
    {
        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        bslma::ManagedPtr<Shape> result;
        switch (kind) {
          case Shapes::SHAPE_CIRCLE: {
            Circle *circ = new(*alloc)Circle(dimension);
            result.load(circ);
          } break;
          case Shapes::SHAPE_SQUARE: {
            Square *sqr = new(*alloc)Square(dimension);
            result.load(sqr);
          } break;
        }
        return result;
    }
//..
// Then, we can use our function to create shapes of different kinds, and check
// that they report the correct area.  Note that we are using a radius of '1.0'
// for the 'Circle' and integral side-length for the 'Square' to support an
// accurate 'operator==' with floating-point quantities.  Also note that,
// despite the destructor for 'Shape' being non-virtual, the correct destructor
// for the appropriate concrete 'Shape' type is called.  This is because the
// destructor is captured when the 'bslma::ManagedPtr' constructor is called,
// and has access to the complete type of each shape object.
//..
    void testShapes()
    {
        bslma::ManagedPtr<Shape> shape = makeShape(Shapes::SHAPE_CIRCLE, 1.0);
        ASSERT(0 != shape);
        ASSERT(3.141592653589793238462 == shape->area());

        shape = makeShape(Shapes::SHAPE_SQUARE, 2.0);
        ASSERT(0 != shape);
        ASSERT(4.0 == shape->area());
    }
//..
// Next, we observe that as we are creating objects dynamically, we should pass
// an allocator to the 'makeShape' function, rather than simply accepting the
// default allocator each time.  Note that when we do this, we pass the user's
// allocator to the 'bslma::ManagedPtr' object as the "factory".
//..
    bslma::ManagedPtr<Shape> makeShape(Shapes::VALUES    kind,
                                       double            dimension,
                                       bslma::Allocator *allocator)
    {
        bslma::Allocator *alloc = bslma::Default::allocator(allocator);
        bslma::ManagedPtr<Shape> result;
        switch (kind) {
          case Shapes::SHAPE_CIRCLE: {
            Circle *circ = new(*alloc)Circle(dimension);
            result.load(circ, alloc);
          } break;
          case Shapes::SHAPE_SQUARE: {
            Square *sqr = new(*alloc)Square(dimension);
            result.load(sqr, alloc);
          } break;
        }
        return result;
    }
//..
// Finally we repeat the earlier test, additionally passing a test allocator:
//..
    void testShapesToo()
    {
        bslma::TestAllocator ta("object");

        bslma::ManagedPtr<Shape> shape =
                                     makeShape(Shapes::SHAPE_CIRCLE, 1.0, &ta);
        ASSERT(0 != shape);
        ASSERT(3.141592653589793238462 == shape->area());

        shape = makeShape(Shapes::SHAPE_SQUARE, 3.0, &ta);
        ASSERT(0 != shape);
        ASSERT(9.0 == shape->area());
    }
//..
//
///Example 2: Aliasing
///- - - - - - - - - -
// Suppose that we wish to give access to an item in a temporary array via a
// pointer which we'll call the "finger".  The finger is the only pointer to
// the array or any part of the array, but the entire array must be valid until
// the finger is destroyed, at which time the entire array must be deleted.  We
// handle this situation by first creating a managed pointer to the entire
// array, then creating an alias of that pointer for the finger.  The finger
// takes ownership of the array instance, and when the finger is destroyed, it
// is the array's address, rather than the finger, that is passed to the
// deleter.
//
// First, let's say our array stores data acquired from a ticker plant
// accessible by a global 'getQuote' function:
//..
    struct Ticker {

        static double getQuote() // From ticker plant. Simulated here
        {
            static const double QUOTES[] = {
            7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
            };
            static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
            static int index = 0;

            double ret = QUOTES[index];
            index = (index + 1) % NUM_QUOTES;
            return ret;
        }
    };
//..
// Then, we want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns a
// managed pointer to the desired value:
//..
    const double END_QUOTE = -1;

    bslma::ManagedPtr<double>
    getFirstQuoteLargerThan(double threshold, bslma::Allocator *allocator)
    {
        ASSERT( END_QUOTE < 0 && 0 <= threshold );
//..
// Next, we allocate our array with extra room to mark the beginning and end
// with a special 'END_QUOTE' value:
//..
        const int MAX_QUOTES = 100;
        int numBytes = (MAX_QUOTES + 2) * sizeof(double);
        double *quotes = (double*) allocator->allocate(numBytes);
        quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then, we create a managed pointer to the entire array:
//..
        bslma::ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Next, we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
        double *finger = 0;

        for (int i = 1; i <= MAX_QUOTES; ++i) {
            double quote = Ticker::getQuote();
            quotes[i] = quote;
            if (!finger && quote > threshold) {
                finger = &quotes[i];
            }
        }
//..
// Now, we use the alias constructor to create a managed pointer that points to
// the desired value (the finger) but manages the entire array:
//..
        return bslma::ManagedPtr<double>(managedQuotes, finger);
    }
//..
// Then, our main program calls 'getFirstQuoteLargerThan' like this:
//..
    int aliasExample()
    {
        bslma::TestAllocator ta;
        bslma::ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
        ASSERT(*result > 16.00);
        ASSERT(1 == ta.numBlocksInUse());
        if (g_verbose) printf("Found quote: %g\n", *result);
//..
// Next, we also print the preceding 5 quotes in last-to-first order:
//..
        if (g_verbose) printf("Preceded by:");
        int i;
        for (i = -1; i >= -5; --i) {
            double quote = result.get()[i];
            if (END_QUOTE == quote) {
                break;
            }
            ASSERT(quote < *result);
            if (g_verbose) printf(" %g", quote);
        }
        if (g_verbose) printf("\n");
//..
// Then, to move the finger, e.g., to the last position printed, one must be
// careful to retain the ownership of the entire array.  Using the statement
// 'result.load(result.get()-i)' would be an error, because it would first
// compute the pointer value 'result.get()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
        result.loadAlias(result, result.get()-i);
//..
// Finally, if we reset the result pointer, the entire array is deallocated:
//..
        result.clear();
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());

        return 0;
    }
//..
//
///Example 3: Dynamic Objects and Factories
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to track the number of objects currently managed by
// 'bslma::ManagedPtr' objects.
//
// First we define a factory type that holds an allocator and a usage-counter.
// Note that such a type cannot sensibly be copied, as the notion 'count'
// becomes confused.
//..
    class CountedFactory {
        // DATA
        int               d_count;
        bslma::Allocator *d_allocator;

      private:
        // NOT IMPLEMENTED
        CountedFactory(const CountedFactory&);
        CountedFactory& operator=(const CountedFactory&);

      public:
        // CREATORS
        explicit CountedFactory(bslma::Allocator *basicAllocator = 0);
            // Create a 'CountedFactory' object which uses the optionally
            // specified 'basicAllocator' to supply memory, and the default
            // allocator otherwise.

        ~CountedFactory();
            // Destroy this object.
//..
// Next, we provide the 'createObject' and 'deleteObject' functions that are
// standard for factory objects.  Note that the 'deleteObject' function
// signature has the form required by 'bslma::ManagedPtr' for a factory.
//..
        // MANIPULATORS
        template <class TYPE>
        TYPE *createObject();
            // Return a pointer to a newly allocated object of type 'TYPE'
            // created using its default constructor.  Memory for the object is
            // supplied by the allocator supplied to this factory's
            // constructor, and the count of valid object is incremented.

        template <class TYPE>
        void deleteObject(const TYPE *target);
            // Destroy the object pointed to by the specified 'target' and
            // reclaim the memory.  Decrement the count of currently valid
            // objects.
//..
// Then, we round out the class with the ability to query the 'count' of
// currently allocated objects.
//..
        // ACCESSORS
        int count() const;
            // Return the number of currently valid objects allocated by this
            // factory.
    };
//..
// Next, we define the operations declared by the class.
//..
    CountedFactory::CountedFactory(bslma::Allocator *basicAllocator)
    : d_count(0)
    , d_allocator(bslma::Default::allocator(basicAllocator))
    {
    }

    CountedFactory::~CountedFactory()
    {
        ASSERT(0 == d_count);
    }

    template <class TYPE>
    TYPE *CountedFactory::createObject()
    {
        TYPE *result = new(*d_allocator)TYPE;
        ++d_count;
        return result;
    }

    template <class TYPE>
    void CountedFactory::deleteObject(const TYPE *object)
    {
        d_allocator->deleteObject(object);
        --d_count;
    }

    inline
    int CountedFactory::count() const
    {
        return d_count;
    }
//..
// Then, we can create a test function to illustrate how such a factory would
// be used with 'bslma::ManagedPtr'.
//..
    void testCountedFactory()
    {
//..
// Next, we declare a test allocator, and an object of our 'CountedFactory'
// type using that allocator.
//..
        bslma::TestAllocator ta;
        CountedFactory cf(&ta);
//..
// Then, we open a new local scope and declare an array of managed pointers.
// We need a local scope in order to observe the behavior of the destructors at
// end of the scope, and use an array as an easy way to count more than one
// object.
//..
        {
            bslma::ManagedPtr<int> pData[4];
//..
// Next, we load each managed pointer in the array with a new 'int' using our
// factory 'cf' and assert that the factory 'count' is correct after each new
// 'int' is created.
//..
            int i = 0;
            while (i != 4) {
                pData[i++].load(cf.createObject<int>(), &cf);
                ASSERT(cf.count() == i);
            }
//..
// Then, we 'clear' the contents of a single managed pointer in the array, and
// assert that the factory 'count' is appropriately reduced.
//..
            pData[1].clear();
            ASSERT(3 == cf.count());
//..
// Next, we 'load' a managed pointer with another new 'int' value, again using
// 'cf' as the factory, and assert that the 'count' of valid objects remains
// the same (destroy one object and add another).
//..
            pData[2].load(cf.createObject<int>(), &cf);
            ASSERT(3 == cf.count());
        }
//..
// Finally, we allow the array of managed pointers to go out of scope and
// confirm that when all managed objects are destroyed, the factory 'count'
// falls to zero, and does not overshoot.
//..
        ASSERT(0 == cf.count());
    }
//..
//
///Example 5: Inplace Object Creation
/// - - - - - - - - - - - - - - - - -
// Suppose we want to allocate memory for an object, construct it in place, and
// obtain a managed pointer referring to this object.  This can be done in one
// step using two free functions provided in 'bslma::ManagedPtrUtil'.
//
// First, we create a simple class clearly showing the features of these
// functions.  Note that this class does not define the
// 'bslma::UsesBslmaAllocator' trait.  It is done intentionally for
// illustration purposes only, and definitely is *not* *recommended* in
// production code.  The class has an elided interface (i.e., copy constructor
// and copy-assignment operator are not included for brevity):
//..
    class String {
        // Simple class that stores a copy of a null-terminated C-style string.

      private:
        // DATA
        char             *d_str_p;    // stored value (owned)
        bslma::Allocator *d_alloc_p;  // allocator to allocate any dynamic
                                      // memory (held, not owned)

      public:
        // CREATORS
        String(const char *str, bslma::Allocator *alloc)
            // Create an object having the same value as the specified 'str'
            // using the specified 'alloc' to supply memory.
        : d_alloc_p(alloc)
        {
            ASSERT(str);
            ASSERT(alloc);

            std::size_t length = std::strlen(str);

            d_str_p = static_cast<char *>(d_alloc_p->allocate(length + 1));
            std::strncpy(d_str_p, str, length + 1);
        }

        ~String()
            // Destroy this object.
        {
            d_alloc_p->deallocate(d_str_p);
        }

        // ACCESSORS
        bslma::Allocator *allocator() const
            // Return a pointer providing modifiable access to the allocator
            // associated with this 'String'.
        {
            return d_alloc_p;
        }
    };
//..
// Next, we create a code fragment that will construct a managed 'String'
// object using the default allocator to supply memory:
//..
    void testInplaceCreation()
    {
//..
// Suppose we want to have a different allocator supply memory allocated by the
// object:
//..
        bslma::TestAllocator ta;
        bsls::Types::Int64   testBytesInUse = ta.numBytesInUse();

        ASSERT(0 == testBytesInUse);

        bslma::TestAllocator         da;
        bslma::DefaultAllocatorGuard dag(&da);
        bsls::Types::Int64           defaultBytesInUse = da.numBytesInUse();

        ASSERT(0 == defaultBytesInUse);
//..
// Then, create a string to copy:
//..
        const char *STR        = "Test string";
        const int   STR_LENGTH = static_cast<int>(std::strlen(STR));
//..
// Next, dynamically create an object and obtain the managed pointer referring
// to it using the 'bslma::ManagedPtrUtil::makeManaged' function:
//..
        {
            bslma::ManagedPtr<String> stringManagedPtr =
                          bslma::ManagedPtrUtil::makeManaged<String>(STR, &ta);
//..
// Note that memory for the object itself is supplied by the default allocator,
// while memory for the copy of the passed string is supplied by another
// allocator:
//:
            ASSERT(static_cast<int>(sizeof(String)) <= da.numBytesInUse());
            ASSERT(&ta == stringManagedPtr->allocator());
            ASSERT(STR_LENGTH + 1 == ta.numBytesInUse());
        }
//..
// Then, make sure that all allocated memory is successfully released after
// managed pointer destruction:
//..
        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
//..
// If you want to use an allocator other than the default allocator, then the
// 'allocateManaged' function should be used instead:
//..
        bslma::TestAllocator oa;
        bsls::Types::Int64   objectBytesInUse = oa.numBytesInUse();
        ASSERT(0 == objectBytesInUse);

        {
            bslma::ManagedPtr<String> stringManagedPtr =
                 bslma::ManagedPtrUtil::allocateManaged<String>(&oa, STR, &ta);

            ASSERT(static_cast<int>(sizeof(String)) <= oa.numBytesInUse());
            ASSERT(&ta == stringManagedPtr->allocator());
            ASSERT(STR_LENGTH + 1 == ta.numBytesInUse());
            ASSERT(0 == da.numBytesInUse());
        }

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == oa.numBytesInUse());
    }
//..
// Next, let's look at a more common scenario where the object's type uses
// 'bslma' allocators.  In that case 'allocateManaged' implicitly passes the
// supplied allocator to the object's constructor as an extra argument in the
// final position.
//
// The second example class almost completely repeats the first one, except
// that it explicitly defines the 'bslma::UsesBslmaAllocator' trait:
//..
    class StringAlloc {
        // Simple class that stores a copy of a null-terminated C-style string
        // and explicitly claims to use 'bslma' allocators.

      private:
        // DATA
        char             *d_str_p;    // stored value (owned)
        bslma::Allocator *d_alloc_p;  // allocator to allocate any dynamic
                                      // memory (held, not owned)

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(StringAlloc, bslma::UsesBslmaAllocator);

        // CREATORS
        StringAlloc(const char *str, bslma::Allocator *basicAllocator = 0)
            // Create an object having the same value as the specified 'str'.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.
        : d_alloc_p(bslma::Default::allocator(basicAllocator))
        {
            ASSERT(str);

            std::size_t length = std::strlen(str);

            d_str_p = static_cast<char *>(d_alloc_p->allocate(length + 1));
            std::strncpy(d_str_p, str, length + 1);
        }

        ~StringAlloc()
            // Destroy this object.
        {
            d_alloc_p->deallocate(d_str_p);
        }

        // ACCESSORS
        bslma::Allocator *allocator() const
            // Return a pointer providing modifiable access to the allocator
            // associated with this 'StringAlloc'.
        {
            return d_alloc_p;
        }
    };
//..
// Then, let's create two managed objects using both 'makeManaged' and
// 'allocateManaged':
//..
    void testUsesAllocatorInplaceCreation()
    {
        bslma::TestAllocator ta;
        bsls::Types::Int64   testBytesInUse = ta.numBytesInUse();

        ASSERT(0 == testBytesInUse);

        bslma::TestAllocator         da;
        bslma::DefaultAllocatorGuard dag(&da);
        bsls::Types::Int64           defaultBytesInUse = da.numBytesInUse();

        ASSERT(0 == defaultBytesInUse);

        const char *STR        = "Test string";
        const int   STR_LENGTH = static_cast<int>(std::strlen(STR));

//..
// Note that we need to explicitly supply the allocator's address to
// 'makeManaged' to be passed to the object's constructor:
//..
        {
            bslma::ManagedPtr<StringAlloc> stringManagedPtr =
                     bslma::ManagedPtrUtil::makeManaged<StringAlloc>(STR, &ta);

            ASSERT(static_cast<int>(sizeof(String)) <= da.numBytesInUse());
            ASSERT(&ta == stringManagedPtr->allocator());
            ASSERT(STR_LENGTH + 1 == ta.numBytesInUse());
        }

//..
// But the supplied allocator is implicitly passed to the constructor by
// 'allocateManaged':
//
        {
            bslma::ManagedPtr<StringAlloc> stringManagedPtr =
                 bslma::ManagedPtrUtil::allocateManaged<StringAlloc>(&ta, STR);

            ASSERT(static_cast<int>(sizeof(String)) + STR_LENGTH + 1 <=
                                                           ta.numBytesInUse());
            ASSERT(&ta == stringManagedPtr->allocator());
            ASSERT(0 == da.numBytesInUse());
        }
//..
// Finally, make sure that all allocated memory is successfully released after
// the managed pointers (and the objects they manage) are destroyed:
//..
        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
    }
//..

}  // close namespace USAGE_EXAMPLES

//=============================================================================
//                              CASTING EXAMPLE
//-----------------------------------------------------------------------------
namespace TYPE_CASTING_TEST_NAMESPACE {

typedef MyTestObject    A;
typedef MyDerivedObject B;

///Example 4: Type Casting
///- - - - - - - - - - - -
// 'bslma::ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way that native pointers can.
//
///Implicit Conversion
/// -  -  -  -  -  - -
// As with native pointers, a pointer of the type 'B' that is publicly derived
// from the type 'A', can be directly assigned a 'bslma::ManagedPtr' of 'A'.
//
// First, consider the following code snippets:
//..
    void implicitCastingExample()
    {
//..
// If the statements:
//..
        bslma::TestAllocator localDefaultTa;
        bslma::TestAllocator localTa;

        bslma::DefaultAllocatorGuard guard(&localDefaultTa);

        int numdels = 0;

        {
            B *b_p = 0;
            A *a_p = b_p;   (void)a_p;
//..
// are legal expressions, then the statements
//..
            bslma::ManagedPtr<A> a_mp1;
            bslma::ManagedPtr<B> b_mp1;

            ASSERT(!a_mp1 && !b_mp1);

            a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
            ASSERT(!a_mp1 && !b_mp1);

            B *b_p2 = new B(&numdels);
            bslma::ManagedPtr<B> b_mp2(b_p2);    // default allocator
            ASSERT(!a_mp1 && b_mp2);

            a_mp1 = b_mp2;      // conversion assignment of non-nil ptr to nil
            ASSERT(a_mp1 && !b_mp2);

            B *b_p3 = new (localTa) B(&numdels);
            bslma::ManagedPtr<B> b_mp3(b_p3, &localTa);
            ASSERT(a_mp1 && b_mp3);

            a_mp1 = b_mp3;      // conversion assignment of non-nil to non-nil
            ASSERT(a_mp1 && !b_mp3);

            a_mp1 = b_mp3;      // conversion assignment of nil to non-nil
            ASSERT(!a_mp1 && !b_mp3);

            // constructor conversion init with nil
            bslma::ManagedPtr<A> a_mp4(b_mp3, b_mp3.get());
            ASSERT(!a_mp4 && !b_mp3);

            // constructor conversion init with non-nil
            B *p_b5 = new (localTa) B(&numdels);
            bslma::ManagedPtr<B> b_mp5(p_b5, &localTa);
            bslma::ManagedPtr<A> a_mp5(b_mp5, b_mp5.get());
            ASSERT(a_mp5 && !b_mp5);
            ASSERT(a_mp5.get() == p_b5);

            // constructor conversion init with non-nil
            B *p_b6 = new (localTa) B(&numdels);
            bslma::ManagedPtr<B> b_mp6(p_b6, &localTa);
            bslma::ManagedPtr<A> a_mp6(b_mp6);
            ASSERT(a_mp6 && !b_mp6);
            ASSERT(a_mp6.get() == p_b6);

            struct S {
                int d_i[10];
            };

#if 0
            S *pS = new (localTa) S;
            bslma::ManagedPtr<S> s_mp1(pS, &localTa);

            for (int i = 0; 10 > i; ++i) {
                pS->d_i[i] = i;
            }

            bslma::ManagedPtr<int> i_mp1(s_mp1, s_mp1->d_i + 4);
            ASSERT(4 == *i_mp1);
#endif

            ASSERT(200 == numdels);
        }

        ASSERT(400 == numdels);
    } // implicitCastingExample()
//..
//
///Explicit Conversion
/// -  -  -  -  -  - -
// Through "aliasing", a managed pointer of any type can be explicitly
// converted to a managed pointer of any other type using any legal cast
// expression.  For example, to static-cast a managed pointer of type A to a
// shared pointer of type B, one can simply do the following:
//..
    void explicitCastingExample()
    {
        bslma::ManagedPtr<A> a_mp;
        bslma::ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.get()));
//..
// or even use the less safe "C"-style casts:
//..
        // bslma::ManagedPtr<A> a_mp;
        bslma::ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp.get()));

    } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
    void processPolymorphicObject(bslma::ManagedPtr<A>  aPtr,
                                  bool                 *castSucceeded)
    {
        bslma::ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.get()));
        if (bPtr) {
            ASSERT(!aPtr);
            *castSucceeded = true;
        }
        else {
            ASSERT(aPtr);
            *castSucceeded = false;
        }
    }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed object will be destroyed correctly regardless of how it
// is cast.

}  // close namespace TYPE_CASTING_TEST_NAMESPACE

// BDE_VERIFY pragma: pop

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

//=============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

typedef MyTestObject                          Test;
typedef MyDerivedObject                       Derived;
typedef MySecondDerivedObject                 SecondDerived;
typedef MyDerivedDerivedObject                 DerivedDerived;

typedef bslma::ManagedPtr<MyTestObject>           Obj;
typedef bslma::ManagedPtr<const MyTestObject>     CObj;
typedef bslma::ManagedPtr<MyDerivedObject>        DObj;
typedef bslma::ManagedPtr<MySecondDerivedObject>  D2Obj;
typedef bslma::ManagedPtr<MyDerivedDerivedObject> DDObj;
typedef bslma::ManagedPtr<void>                   VObj;

//=============================================================================
//                      HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------
// The 'bsls_IsPolymorphic' trait does not work correctly on the following two
// platforms, which causes 'bslma::DeleterHelper' to dispatch to an
// implementation that cannot compile.
#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC) || \
    defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE
#endif

struct Base {
    explicit Base(int *deleteCount)
    : d_count_p(deleteCount)
    {
    }

    ~Base() { ++*d_count_p; }
        // Increment the held counter, and destroy this object

    int *d_count_p;
};

#if defined BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE
struct Base1 : virtual Base {
    explicit Base1(int *deleteCount = 0)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base1() { *d_count_p += 9; }
        // Increment the held counter by '9', and destroy this object

    char d_padding;
};

struct Base2 : virtual Base {
    explicit Base2(int *deleteCount = 0)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base2() { *d_count_p += 99; }
        // Increment the held counter by '99', and destroy this object

    char d_padding;
};

struct Composite : Base1, Base2 {
    explicit Composite(int *deleteCount)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Composite() { *d_count_p += 891; }
        // Increment the held counter by '891', and destroy this object

    char d_padding;
};
#else
struct Base1 : Base {
    explicit Base1(int *deleteCount)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base1() { *d_count_p += 9; }
        // Increment the held counter by '9', and destroy this object

    char d_padding;
};

struct Base2 : Base {
    explicit Base2(int *deleteCount)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base2() { *d_count_p += 99; }
        // Increment the held counter by '99', and destroy this object

    char d_padding;
};

struct Composite : Base1, Base2 {
    explicit Composite(int *deleteCount)
    : Base1(deleteCount)
    , Base2(deleteCount)
    , d_count_p(deleteCount)
    {
    }

    ~Composite() { *d_count_p += 890; }
        // Increment the held counter by '890', and destroy this object

    int *d_count_p;
};
#endif

// The next three types are used for more general testing of types using
// multiple inheritance, and vtables.  They specifically support test case 11,
// and to not currently need to support the policy-driven generative testing
// framework.

class BaseInt1 {
  public:
    int d_data;  // public data as we want to inspect object layouts

    BaseInt1()
    : d_data(1)
    {
    }

    virtual int data()  const { return d_data; }
    virtual int data1() const { return d_data; }
};

class BaseInt2 {
  public:
    int d_data;  // public data as we want to inspect object layouts

    BaseInt2()
    : d_data(2)
    {
    }

    virtual int data()  const { return d_data; }
    virtual int data2() const { return d_data; }
};

class CompositeInt3 : public BaseInt1, public BaseInt2 {
  public:
    int d_data;  // public data as we want to inspect object layouts

    CompositeInt3()
    : d_data(3)
    {
    }

    virtual int data()  const { return d_data; }
    virtual int data1() const { return d_data * d_data; }
    virtual int data2() const { return d_data + d_data; }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CountedStackDeleter
{
    // DATA
    volatile int *d_deleteCounter_p;

  private:
    // NOT IMPLEMENTED
    CountedStackDeleter(const CountedStackDeleter&); //=delete;
    CountedStackDeleter& operator=(const CountedStackDeleter&); //=delete;

  public:
    // CREATORS
    explicit CountedStackDeleter(int *counter) : d_deleteCounter_p(counter) {}
        // Create a 'CountedStackDeleter' using the specified 'counter' to
        // record when this object is invoked as a deleter.

    //! ~CountedStackDeleter();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const { return d_deleteCounter_p; }
        // Return the address of the counter used to track when this object is
        // invoked as a deleter.

    void deleteObject(void *) const
        // Increment the stored reference to a counter to indicate that this
        // method has been called.
    {
        ++*d_deleteCounter_p;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct IncrementIntFactory
{
    void destroy(int *object)
    {
        ASSERT(object);
        ++*object;
    }
};

void incrementIntDeleter(int *ptr, IncrementIntFactory *factory)
{
    ASSERT(ptr);
    ASSERT(factory);

    factory->destroy(ptr);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The two deleters defined below do not use the factory (or even object)
// argument to perform their bookkeeping.  They are typically used to test
// overloads taking 'NULL' factories.
int g_deleteCount = 0;

static void countedNilDelete(void *, void*)
{
//    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template<class TARGET_TYPE>
static void templateNilDelete(TARGET_TYPE *, void*)
{
//    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class ELEMENT_TYPE>
class AllocatorDeleter
{
    // This class provides a 'bslma::ManagedPtr' deleter that does *not* derive
    // from 'bslma::Allocator'.

  public:
    static void deleter(ELEMENT_TYPE *ptr, bslma::Allocator *alloc)
    {
        BSLS_ASSERT_SAFE(0 != ptr);
        BSLS_ASSERT_SAFE(0 != alloc);

        alloc->deleteObject(ptr);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct OverloadTest {
    // This struct provides a small overload set taking managed pointer values
    // with similar looking (potentially related) types, to be sure there are
    // no unexpected ambiguities or conversions.

    static int invoke(bslma::ManagedPtr<void>)     { return 0; }
    static int invoke(bslma::ManagedPtr<int>)      { return 1; }
    static int invoke(bslma::ManagedPtr<const int>){ return 2; }
        // Return an integer code reporting which specific overload was called.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                           // ------------------
                           // class MyTestObject
                           // ------------------

// CREATORS
MyTestObject::MyTestObject(int *counter)
: d_deleteCounter_p(counter)
, d_value()
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile int *MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

inline
int *MyTestObject::valuePtr(int index) const
{
    BSLS_ASSERT_SAFE(2 > index);

    return d_value + index;
}

                           // ---------------------
                           // class MyDerivedObject
                           // ---------------------

// CREATORS
inline
MyDerivedObject::MyDerivedObject(int *counter)
: MyTestObject(counter)
{
}

inline
MyDerivedObject::~MyDerivedObject()
{
    (*deleteCounter()) += 99; // +1 from base -> 100
}

                           // ---------------------------
                           // class MySecondDerivedObject
                           // ---------------------------

// CREATORS
inline
MySecondDerivedObject::MySecondDerivedObject(int *counter)
: MyTestObject(counter)
{
}

inline
MySecondDerivedObject::~MySecondDerivedObject()
{
    (*deleteCounter()) += 9999;  // +1 from base -> 10000
}

                           // ----------------------------
                           // class MyDerivedDerivedObject
                           // ----------------------------

// CREATORS
inline
MyDerivedDerivedObject::MyDerivedDerivedObject(int* counter)
    : MyDerivedObject(counter)
{
}

inline
MyDerivedDerivedObject::~MyDerivedDerivedObject()
{
    (*deleteCounter()) += 999900;  // +100 from base -> 1000000
}
}  // close unnamed namespace

//=============================================================================
//                              CREATORS TEST
//=============================================================================

namespace CREATORS_TEST_NAMESPACE {

#undef SS
struct SS {
    // DATA
    char  d_buf[100];
    int  *d_numDeletes_p;

    // CREATORS
    explicit SS(int *numDeletes)
    {
        d_numDeletes_p = numDeletes;
    }

    ~SS()
        // Increment the held counter, and destroy this object.
    {
        ++*d_numDeletes_p;
    }
};

typedef bslma::ManagedPtr<SS>   SSObj;
typedef bslma::ManagedPtr<char> ChObj;

}  // close namespace CREATORS_TEST_NAMESPACE

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void doNothingDeleter(void *object, void *)
{
    ASSERT(object);
}

#if 0
static void myTestDeleter(Test *object, bslma::TestAllocator *allocator)
{
    allocator->deleteObject(object);
    if (g_verbose) {
        printf("myTestDeleter called\n");
    }
}
#else
static void myTestDeleter(void *object, void *allocator)
    // Destroy the specified 'object' using the specified 'allocator'.  The
    // behavior is undefined unless 'object' points to a 'Test' and 'allocator'
    // points to a 'bslma::TestAllocator'.  Note that the parameters are
    // type-erased for use as a managed pointer deleter.
{
    static_cast<bslma::TestAllocator *>(allocator)->deleteObject(
                                                  static_cast<Test *>(object));
    if (g_verbose) {
        printf("myTestDeleter called\n");
    }
}
#endif

static bslma::ManagedPtr<MyDerivedObject>
returnDerivedPtr(int *numDels, bslma::TestAllocator *allocator)
{
    MyDerivedObject *p = new (*allocator) MyDerivedObject(numDels);

    bslma::ManagedPtr<MyDerivedObject> ret(p, allocator);
    return ret;
}

static bslma::ManagedPtr<MyTestObject>
returnManagedPtr(int *numDels, bslma::TestAllocator *allocator)
{
    MyTestObject *p = new (*allocator) MyTestObject(numDels);

    bslma::ManagedPtr<MyTestObject> ret(p, allocator);
    return ret;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
static void
consumeManagedPtr(bslma::ManagedPtr<MyTestObject>,
                  int *numDels,
                  int  checkValue)
{
    ASSERTV(checkValue, *numDels, checkValue == *numDels);
}
#endif //defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

static void
consumeManagedPtrCR(const bslma::ManagedPtr<MyTestObject>&,
                    int *numDels,
                    int  checkValue)
{
    ASSERTV(checkValue, *numDels, checkValue == *numDels);
}

static bslma::ManagedPtr<MySecondDerivedObject>
returnSecondDerivedPtr(int *numDels, bslma::TestAllocator *allocator)
{
    MySecondDerivedObject *p = new (*allocator) MySecondDerivedObject(numDels);

    bslma::ManagedPtr<MySecondDerivedObject> ret(p, allocator);
    return ret;
}

template <class TYPE>
void validateManagedState(unsigned int                     LINE,
                          const bslma::ManagedPtr<TYPE>&   obj,
                          const void                      *ptr,
                          const bslma::ManagedPtrDeleter&  del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   TYPE& operator*() const;
    //   TYPE *operator->() const;
    //   TYPE *ptr() const;
    //   const bslma::ManagedPtrDeleter& deleter() const;

    bslma::TestAllocatorMonitor gam(dynamic_cast<bslma::TestAllocator*>
                                          (bslma::Default::globalAllocator()));
    bslma::TestAllocatorMonitor dam(dynamic_cast<bslma::TestAllocator*>
                                         (bslma::Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERTV(LINE, false == static_cast<bool>(obj));
        ASSERTV(LINE, !obj);
        ASSERTV(LINE, 0 == obj.operator->());
        ASSERTV(LINE, 0 == obj.get());
        ASSERTV(LINE, 0 == obj.ptr());

#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) printf("\tNegative testing\n");

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(*obj);
            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == static_cast<bool>(obj));
        ASSERT(false == !obj);

        TYPE *arrow = obj.operator->();
        ASSERTV(LINE, ptr, arrow,   ptr == arrow);

        TYPE *objPtr = obj.ptr();
        ASSERTV(LINE, ptr, objPtr,  ptr == objPtr);

        TYPE *objPtr2 = obj.get();
        ASSERTV(LINE, ptr, objPtr2, ptr == objPtr2);

        TYPE& target = *obj;
        ASSERTV(LINE, &target, ptr, &target == ptr);

        const bslma::ManagedPtrDeleter& objDel = obj.deleter();
        ASSERTV(LINE, del, objDel, del == objDel);
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}

void validateManagedState(unsigned int                     LINE,
                          const bslma::ManagedPtr<void>&   obj,
                          void                            *ptr,
                          const bslma::ManagedPtrDeleter&  del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   void operator*() const;
    //   void *operator->() const;
    //   void *ptr() const;
    //   const bslma::ManagedPtrDeleter& deleter() const;

    bslma::TestAllocatorMonitor gam(dynamic_cast<bslma::TestAllocator*>
                                          (bslma::Default::globalAllocator()));
    bslma::TestAllocatorMonitor dam(dynamic_cast<bslma::TestAllocator*>
                                         (bslma::Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERTV(LINE, false == static_cast<bool>(obj));
        ASSERTV(LINE, !obj);
        ASSERTV(LINE, 0 == obj.operator->());
        ASSERTV(LINE, 0 == obj.get());
        ASSERTV(LINE, 0 == obj.ptr());
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) printf("\tNegative testing\n");

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == static_cast<bool>(obj));
        ASSERT(false == !obj);

        void *arrow = obj.operator->();
        ASSERTV(LINE, ptr, arrow,   ptr == arrow);

        void *objPtr = obj.ptr();
        ASSERTV(LINE, ptr, objPtr,  ptr == objPtr);

        void *objPtr2 = obj.get();
        ASSERTV(LINE, ptr, objPtr2, ptr == objPtr2);

        const bslma::ManagedPtrDeleter& objDel = obj.deleter();
        ASSERTV(LINE, del, objDel, del == objDel);

#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
        *obj;
#endif
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}

void validateManagedState(unsigned int                          LINE,
                          const bslma::ManagedPtr<const void>&  obj,
                          const void                           *ptr,
                          const bslma::ManagedPtrDeleter&       del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   void operator*() const;
    //   const void *operator->() const;
    //   const void *ptr() const;
    //   const bslma::ManagedPtrDeleter& deleter() const;

    bslma::TestAllocatorMonitor gam(dynamic_cast<bslma::TestAllocator*>
                                          (bslma::Default::globalAllocator()));
    bslma::TestAllocatorMonitor dam(dynamic_cast<bslma::TestAllocator*>
                                         (bslma::Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERTV(LINE, false == static_cast<bool>(obj));
        ASSERTV(LINE, !obj);
        ASSERTV(LINE, 0 == obj.operator->());
        ASSERTV(LINE, 0 == obj.get());
        ASSERTV(LINE, 0 == obj.ptr());
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) printf("\tNegative testing\n");

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == static_cast<bool>(obj));
        ASSERT(false == !obj);

        const void *arrow = obj.operator->();
        ASSERTV(LINE, ptr, arrow, ptr == arrow);

        const void *objPtr = obj.ptr();
        ASSERTV(LINE, ptr, objPtr, ptr == objPtr);

        const void *objPtr2 = obj.get();
        ASSERTV(LINE, ptr, objPtr2, ptr == objPtr2);

        const bslma::ManagedPtrDeleter& objDel = obj.deleter();
        ASSERTV(LINE, del, objDel, del == objDel);

#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
        *obj;
#endif
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}


// 'debugprint' support for 'bsl' types

namespace BloombergLP {
namespace bslma {

void debugprint(const ManagedPtrDeleter& obj)
{
    printf("ManagedPtrDeleter[");
    printf("object: ");     bsls::debugprint(obj.object());
    printf(", factory: ");  bsls::debugprint(obj.factory());
#if defined(BSLS_PLATFORM_CMP_MSVC)
    void *d = obj.deleter();
    printf(", deleter: ");  bsls::debugprint(d);
#else
    printf(", deleter: ");  bsls::debugprint(obj.deleter());
#endif
    printf("]");
}

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                      'load' and constructor TESTING SUPPORT
//-----------------------------------------------------------------------------
// The following functions load a 'bslma::ManagedPtr' into a defined final
// state assuming that it is passed in with an initial state known to the
// calling function.  None of the following functions have their own test case,
// as they are vital implementation details of testing the constructors, and of
// testing the 'load' functions that, in turn, are later used to test the basic
// accessors.  However, these functions are very thoroughly exercised in the
// various 'load' and constructor test cases, in particular by taking an empty
// 'bslma::ManagedPtr' and taking it to the known state expected of each of
// these functions.  Similarly, we will test each transition from every
// possible initial state through each of these functions to validate all
// 'load' state transitions.  Essentially, these are implementation details of
// the 'load' test cases that may be deemed validated by that test case, and so
// safely relied on for all later test cases.
//
// Note that we are generating a very large test-space of test cases to ensure
// that all valid type-deductions and conversions are syntax-checked through
// the various overload sets for 'load' and 'bslma::ManagedPtr' constructors.
// Attempts to manually test the valid combinations demonstrated that the only
// way to be truly confident with this complex set of constraints was to
// exhaustively generate every testable combination.  These test tables are
// generated in a way to support automated tested of functions with varying
// signatures though a single, simple framework.
//
// Each function below has the same signature so that they can be used to
// populate a test table supporting table-driven testing techniques.  This will
// enable exhaustive testing of the state space and transitions of holding
// various kinds of 'bslma::ManagedPtr' objects.
//
// Each function performs the same set of operations in turn:
//: 1 Copy the initial values stored in passed pointers to compute the side-
//:   effects expected of calling 'load', typically observed as a consequence
//:   of destroying any held managed object.
//:
//: 2 'load' the specified pointer 'o' into the new defined state.
//:
//: 3 Set the new value for 'deleteDelta' for when this new state of 'o' is
//:   destroyed.
//:
//: 4 confirm the act of 'load'ing ran the expected deleters by comparing
//:   new state of 'deleteCount' with the computed value in (1).
//:
//: 5 confirm that each (defined) attribute of 'o' is in the expected state.
//
// The state combinations that need testing are invoking load with 0, 1, 2 or 3
// arguments.  Each combination should be tested for 'bslma::ManagedPtr'
// parameterized with
//: o 'MyTestObject'
//: o 'const MyTestObject'
//: o 'MyDerivedObject'
//: o 'void'
//: o 'const void'
//
// Additionally, there are a small number of corner cases where the base class
// does not have a virtual destructor, especially when the most derived object
// is using multiple inheritance.  These additional states are tested with
// smaller test tables for the 'Base', 'Base1', 'Base2' and 'Composite'
// hierarchy.  A basic set of these tests are also added to the test table for
// 'bslma::ManagedPtr<const void>'.
//
// The first pointer argument should be tested with a pointer of each of the
// following types:
//: o '0' literal
//: o 'MyTestObject *'
//: o 'const MyTestObject *'
//: o 'MyDerivedObject *'
//: o 'void *'
//: o 'const void *'
//
// When no 'factory' argument is passed, each function should behave as if the
// default allocator were passing in that place.
//
// The second factory argument should be tested with:
//: o '0' literal
//: o 'bslma::Allocator *'
//: o 'ta' to test the specific 'bslma::TestAllocator' derived factory type
//: 0 SOME OTHER FACTORY TYPE NOT DERIVED FROM BSLMA_TESTALLOCATOR [NOT TESTED]
//
// The 'deleter' argument will be tested with each of:
//: o '0' literal
//
// The 'line' and 'index' parameters describe the source line number at the
// call site, and any index into a function table to identify the specific
// invocation of this test function.  The 'useDefault' argument must be set to
// 'true' if the function allocates memory from the default allocator.  This is
// then used by the calling harness to know if it can check the default
// allocator's memory usage.
//
// The following chart describes the complete set of test scenarios, labeled
// with their corresponding function:
//:        Object  Code     Value
//:        ------  ----     -----
//:             -  (none)   use default (if any)
//:             0  Onull    null pointer literal
//:          base  Obase    pointer to allocated MyTestObject
//:    const base  OCbase   pointer to allocated 'const MyTestObject'
//:       derived  Oderiv   pointer to allocated 'MyDerivedObject'
//: const derived  OCderiv  pointer to allocated 'const MyDerivedObject'
//:
//:       Factory  Code     Value
//:       -------  ----     -----
//:             -  (none)   use default (if any)
//:             0  Fnull    null pointer literal
//:         bslma  Fbsl     'bslma::TestAllocator' cast to 'bslma::Allocator *'
//:          Test  Ftst     'bslma::TestAllocator' factory
//:       default  Fdflt    default allocator, passed as 'bslma::Allocator *'
//:
//: [No const factory support by default, but can 'deleter' support this?]
//: [Probably only through the deprecated interface, and no code can do this
//: yet?]
//:
//: Scenarios to consider:
//: "V(V*, V*)" can hide many casting opportunities inside the function body.
//: This implies we may have many scenarios to test inside this one case, or
//: we may want to pick the most representative single case.  In fact, two
//: cases dominate our analysis, "V(bslma::Allocator, Base)", and "V(actual,
//: actual)".  The former can be explicitly coded up as a non-template
//: function.  The latter is already implemented as
//: 'bslma::ManagedPtr_FactoryDeleter'.  Note that there is a third category of
//: deleter, where the deleter function acts only on the 'object' parameter and
//: ignores the 'factory'.  This is an important case, as we must support '0'
//: literals and null pointers for factories based on existing code.  We can
//: test this case with a deleter that assumes the object was allocated using
//: the default allocator.  This gives us our 3 test scenarios for "V(V*,V*)".
//: The "V(V*,B*)" and "V(V*,D*)" cases could be tricky if the first "V*"
//: parameter is thought to be a type-erased factory that is cast back
//: internally.  We believe there are such cases in existing code, so must be
//: supported - we cannot assume the initial "V*" factory argument is ignored
//: by the deleter.  Here we will test just two forms, 'D' ignoring the factory
//: argument and using the default allocator to destroy the 'object', and 'B'
//: which destroys the 'object' by casting the 'factory' to
//: 'bslma::Allocator*'.
//:
//:      Deleter  Code   Value
//:      -------  ----   -----
//:            -  (none) use default (if any)
//:            0  Dnull  [ALL SUCH OVERLOADS ARE COMPILE-FAIL TEST CASES]
//:    V(V*, V*)  Dzero  a pointer variable with value '0'.
//:    V(V*, V*)  DvvF
//:    V(V*, V*)  DvvT
//:    V(V*, V*)  DvvD
//:    V(V*, B*)  DvbD
//:    V(V*, B*)  DvbB
//:    V(V*, D*)  DvdD
//:    V(V*, D*)  DvdB
//:    V(B*, V*)  Dbv
//:    V(B*, B*)  Dbb
//:    V(B*, D*)  Dbd
//:    V(T*, V*)  Dtv
//:    V(T*, B*)  Dtb
//:    V(T*, D*)  Dtd
//:
//: Deleter codes used above:
//:     V(X*, Y*) is a function type, returning 'void' taking arguments of type
//:              'X*' and 'Y*'.
//:
//: Possible values of X:
//: o V void
//: o B bslma::Allocator
//: o T bslma::TestAllocator
//:
//: Possible values of Y:
//: o V void
//: o B MyTestClass
//: o D MyDerivedClass

//X doLoad

//X doLoadOnull
//X doLoadObase
//X doLoadOCbase
//X doLoadOderiv
//X doLoadOCderiv

//- doLoadOnullFbsl    [COMPILE-FAIL], but might permit
//- doLoadOnullFtst    [COMPILE-FAIL], but might permit
//X doLoadObaseFbsl
//X doLoadObaseFtst
//X doLoadOCbaseFbsl
//X doLoadOCbaseFtst
//X doLoadOderivFbsl
//X doLoadOderivFtst
//X doLoadOCderivFbsl
//X doLoadOCderivFtst

//X doLoadObaseFbslDzero
//X doLoadObaseFtstDzero
//X doLoadOderivFbslDzero
//X doLoadOderivFtstDzero

// TBD Can we store a 'void *' object with a knowledgeable factory?
//     (I think we require a knowledgeable deleter to handle such type erasure)

// WOULD THESE SERVE ANY PURPOSE, OR PURELY DELETER TESTS?
// Patterns that would form compile-fails
//   O*Fnull
//   O*Fdflt
//   O*FVtest
//   O*FVdflt
//   O*F*Dnull

// These stems match the above pattern, but are interesting when testing
// deleters

//: doLoadOnullFdflt
//: doLoadObaseFdflt
//: doLoadOCbaseFdflt
//: doLoadOderivFdflt
//: doLoadOCderivFdflt

//: doLoadOnullFnull
//: doLoadObaseFnull
//: doLoadOCbaseFnull
//: doLoadOderivFnull
//: doLoadOCderivFnull

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01 // Functions ordered logically, for easier audit.

namespace {

template <class POINTER_TYPE>
struct TestLoadArgs {
    // This struct holds the set of arguments that will be passed into a policy
    // based test function.  It collects all information for the range of tests
    // and expectations to be set up on entry, and reported on exit.

    int d_deleteCount;          // Delete counter, whose address will be passed
                                // to test object constructors.
    int d_deleteDelta;          // Expected change in delete counter when a new
                                // value is 'load'ed or destructor run.
    bool d_useDefault;          // Set to true if the test uses the default
                                // allocator.
    bslma::TestAllocator *d_ta; // pointer to a test allocator whose lifetime
                                // will outlast the function call.
    unsigned int d_config;      // Valid values are 0-3.
                                // The low-bit represents whether to pass a
                                // null for 'object', the second bit whether to
                                // pass a null for 'factory'
    bslma::ManagedPtr<POINTER_TYPE> *d_p; // pointer to the long-lived managed
                                         // pointer on which to execute tests
};

template <class POINTER_TYPE>
void validateTestLoadArgs(int                               callLine,
                          int                               testLine,
                          const TestLoadArgs<POINTER_TYPE> *args)
{
    // Assert pre-conditions that are appropriate for every call using 'args'.
    ASSERTV(callLine, testLine, args->d_deleteCount, 0 == args->d_deleteCount);
    ASSERTV(callLine, testLine, args->d_p,           0 != args->d_p);
    ASSERTV(callLine, testLine, args->d_ta,          0 != args->d_ta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             ToVoid metafunction
// The 'ToVoid' metafunction supports tests that need to use a 'void' pointer
// representing a pointer to the test object, while also retaining the correct
// cv-qualification.
template <class TYPE>
struct ToVoid {
    typedef void type;
};

template <class TYPE>
struct ToVoid<const TYPE> {
    typedef const void type;
};

template <class TYPE>
struct ToVoid<volatile TYPE> {
    typedef volatile void type;
};

template <class TYPE>
struct ToVoid<const volatile TYPE> {
    typedef const volatile void type;
};

//=============================================================================
//                          Target Object policies
// A Target Object policy consist of two members:
//: 1 A typedef, 'ObjectType', that reports the type of object to create
//: 2 An enum value 'DELETE_DELTA' reporting the expected change in
//:   'deleteCount' when the created object is destroyed.
//
// Note that the dynamic type of the object used in the test might be quite
// different to the static type of the created object described by this policy,
// notably for tests of 'bslma::ManagedPtr<void>'.
//
// List of available policies:
struct Obase;   // construct a base-class object
struct OCbase;  // construct a 'const' base object
struct Oderiv;  // Construct a derived-class object
struct OCderiv; // Construct a const derived object
struct Ob1;     // Construct a left-base class object
struct Ob2;     // Construct a right-base object
struct Ocomp;   // Construct a complete object, derived from two base classes

// Policy implementations
struct Obase {
    typedef MyTestObject ObjectType;

    enum { DELETE_DELTA = 1 };
};

struct OCbase {
    typedef const MyTestObject ObjectType;

    enum { DELETE_DELTA = 1 };
};

struct Oderiv {
    typedef MyDerivedObject ObjectType;

    enum { DELETE_DELTA = 100 };
};

struct OCderiv {
    typedef const MyDerivedObject ObjectType;

    enum { DELETE_DELTA = 100 };
};

struct Ob1 {
    typedef Base1 ObjectType;

    enum { DELETE_DELTA = 10 };
};

struct Ob2 {
    typedef Base2 ObjectType;

    enum { DELETE_DELTA = 100 };
};

struct Ocomp {
    typedef Composite ObjectType;

    enum { DELETE_DELTA = 1000 };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             Factory Policies
// List of available policies:
// ---------------------------
// The factory policy has a type alias named 'FactoryType' that gives the
// static type of the factory supported by the policy - the dynamic type may
// well be a class derived from the static type.
//
// The 'factory' function is supplied with a test allocator, and returns a
// pointer to the preferred allocator to use, which may be the supplied
// allocator cast to the 'FactoryType', or might substitute some other
// allocator entirely, such as the default allocator.
//
// Two constants further describe how policy might be applied: 'USE_DEFAULT'
// indicates that use of the factory will also imply use of the default
// allocator.
//
// TBD 'DELETER_USES_FACTORY' is important and needs better doc!
struct Fbsl;    // factory is a 'bsl' allocator'
struct Ftst;    // factory is a test allocator
struct Fdflt;   // factory is the default allocator

// Policy implementations
struct Fbsl {
    typedef bslma::Allocator FactoryType;

    static FactoryType *factory(bslma::TestAllocator *f)
        // Return the specified factory 'f'.
    {
        return f;
    }

    enum { USE_DEFAULT = false };
    enum { DELETER_USES_FACTORY = true};
};

struct Ftst {
    typedef bslma::TestAllocator FactoryType;

    static FactoryType *factory(bslma::TestAllocator *f)
        // Return the specified factory 'f'.
    {
        return f;
    }

    enum { USE_DEFAULT = false };
    enum { DELETER_USES_FACTORY = true};
};

struct Fdflt {
    typedef bslma::Allocator FactoryType;

    static FactoryType *factory(bslma::TestAllocator *)
        // Return the default allocator.
    {
        return bslma::Default::defaultAllocator();
    }

    enum { USE_DEFAULT = true };
    enum { DELETER_USES_FACTORY = false};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             Deleter Policies
// Deleter policies are class templates that co-ordinate an Object Policy with
// a Factory Policy to produce the correct destruction behavior for testing.
//
// A valid policy will have three type aliases, an enumerated constant, and two
// static function members:
//
//: typedef ... ObjectType :  Object type of the OBJECT policy
//: typedef ... FactoryType:  Factory type of the FACTORY policy
//: typedef ... DeleterType:  type of the deleter function 'doDelete'
//
//: enum DELETER_USES_FACTORY : enumeration from the FACTORY policy
//
// DeleterType *deleter()
//    Return the address of the 'doDelete' function
//
// void doDelete(...)
//    The 'doDelete' function has a signature described by the type alias
//    'DeleterType' and actually destroys the dynamically allocated object
//    supplied in the first argument, using either the factory supplied as the
//    second argument, or the default allocator, according to the value of the
//    'DELETER_USES_FACTORY' constant.
//
//  The implementation of the policies is essentially identical, differing only
//  in the type of the 'doDelete' function.  The intent of the policy is to
//  validate testing with different deleter function signatures as they plug
//  into 'bslma::ManagedPtr' objects.
//
//  There are 4 variations of policy that we must test:
//    Passing the Object pointer, and the Factory pointer, with the correct
//    type, passing each with the type erased as a 'void *', and all the
//    permutations of such typing,
//
// The policy named are encoded as:
//  'D' for Deleter po
//  'Obj' or 'Void' to indicate if the object type is preserved in the deleter
//  'Fac' or 'Void' to indicate if the factory type is preserved in the deleter
//
// Note that use of any policy other than that for passing all parameters to
// the deleter function as 'void *' is deprecated.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template<class OBJECT_POLICY, class FACTORY_POLICY> struct DObjFac;
template<class OBJECT_POLICY, class FACTORY_POLICY> struct DObjVoid;
template<class OBJECT_POLICY, class FACTORY_POLICY> struct DVoidFac;
#endif // BDE_OMIT_INTERNAL_DEPRECATED
template<class OBJECT_POLICY, class FACTORY_POLICY> struct DVoidVoid;

// Policy implementations
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template<class OBJECT_POLICY, class FACTORY_POLICY>
struct DObjFac {
    // This class implements the deleter policy for a deleter function
    // 'doDelete' that explicitly passes both the object type and the factory
    // type.

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    typedef void DeleterType(ObjectType*, FactoryType *);

    enum {DELETER_USES_FACTORY = FACTORY_POLICY::DELETER_USES_FACTORY};

    static DeleterType *deleter()
        // Return the address of the 'doDelete' static member of this class.
    {
        return &doDelete;
    }

    static void doDelete(ObjectType *object, FactoryType *factory)
        // Destroy the specified 'object' and reclaim its memory.  If
        // 'DELETER_USES_FACTORY' is 'true', destroy the 'object' using the
        // specified 'factory', otherwise destroy the 'object' using the
        // default allocator as the factory.
    {
        if (DELETER_USES_FACTORY) {
            factory->deleteObject(object);
        }
        else {
            // Use the default allocator as the deleter, ignore the passed
            // 'factory' pointer
            bslma::Allocator *pDa = bslma::Default::defaultAllocator();
            pDa->deleteObject(object);
        }
    }

};

template<class OBJECT_POLICY, class FACTORY_POLICY>
struct DObjVoid {
    // This class implements the deleter policy for a deleter function
    // 'doDelete' that explicitly passes both the object type, but requires
    // explicitly casting the factory type back from 'void *', where the
    // original factory type is known through the (template parameter)
    // 'FACTORY_POLICY'.

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    typedef void DeleterType(ObjectType*, void *);

    enum {DELETER_USES_FACTORY = FACTORY_POLICY::DELETER_USES_FACTORY};

    static DeleterType *deleter()
        // Return the address of the 'doDelete' static member of this class.
    {
        return &doDelete;
    }

    static void doDelete(ObjectType *object, void *factory)
        // Destroy the specified 'object' and reclaim its memory.  If
        // 'DELETER_USES_FACTORY' is 'true', destroy the 'object' using the
        // specified 'factory', otherwise destroy the 'object' using the
        // default allocator as the factory.  The behavior is undefined unless
        // 'factory' points to an object of type 'FactoryType'.
    {
        if (DELETER_USES_FACTORY) {
            FactoryType *fac = reinterpret_cast<FactoryType *>(factory);
            fac->deleteObject(object);
        }
        else {
            // Use the default allocator as the deleter, ignore the passed
            // 'factory' pointer
            bslma::Allocator *pDa = bslma::Default::defaultAllocator();
            pDa->deleteObject(object);
        }
    }
};

template<class OBJECT_POLICY, class FACTORY_POLICY>
struct DVoidFac {
    // This class implements the deleter policy for a deleter function
    // 'doDelete' that explicitly passes both the factory type, but requires
    // explicitly casting the object type back from 'void *', where the
    // original object type is known through the (template parameter)
    // 'OBJECT_POLICY'.

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    typedef typename ToVoid<ObjectType>::type VoidType;

    typedef void DeleterType(VoidType *, FactoryType *);

    enum {DELETER_USES_FACTORY = FACTORY_POLICY::DELETER_USES_FACTORY};

    static DeleterType *deleter()
        // Return the address of the 'doDelete' static member of this class.
    {
        return &doDelete;
    }

    static void doDelete(VoidType *object, FactoryType *factory)
        // Destroy the specified 'object' and reclaim its memory.  If
        // 'DELETER_USES_FACTORY' is 'true', destroy the 'object' using the
        // specified 'factory', otherwise destroy the 'object' using the
        // default allocator as the factory.  The behavior is undefined unless
        // 'object' points to an object of type 'ObjectType'.

    {
        ObjectType *obj = reinterpret_cast<ObjectType *>(object);
        if (DELETER_USES_FACTORY) {
            factory->deleteObject(obj);
        }
        else {
            // Use the default allocator as the deleter, ignore the passed
            // 'factory' pointer
            bslma::Allocator *pDa = bslma::Default::defaultAllocator();
            pDa->deleteObject(obj);
        }
    }
};
#endif // BDE_OMIT_INTERNAL_DEPRECATED

template<class OBJECT_POLICY, class FACTORY_POLICY>
struct DVoidVoid {
    // This class implements the deleter policy for a deleter function
    // 'doDelete' that requires explicitly casting both the object type and
    // factory type back from 'void *', where the original object type is known
    // through the (template parameter) 'OBJECT_POLICY' and the original
    // factory type is known through the (template parameter) 'FACTORY_POLICY'.

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    typedef void DeleterType(void*, void *);

    enum {DELETER_USES_FACTORY = FACTORY_POLICY::DELETER_USES_FACTORY};

    static DeleterType *deleter()
        // Return the address of the 'doDelete' static member of this class.
    {
        return &doDelete;
    }

    static void doDelete(void *object, void *factory)
        // Destroy the specified 'object' and reclaim its memory.  If
        // 'DELETER_USES_FACTORY' is 'true', destroy the 'object' using the
        // specified 'factory', otherwise destroy the 'object' using the
        // default allocator as the factory.  The behavior is undefined unless
        // 'object' points to an object of type 'ObjectType' and 'factory'
        // points to an object of type 'FactoryType'.

    {
        ObjectType *obj = reinterpret_cast<ObjectType *>(object);
        if (DELETER_USES_FACTORY) {
            FactoryType *fac = reinterpret_cast<FactoryType *>(factory);
            fac->deleteObject(obj);
        }
        else {
            // Use the default allocator as the deleter, ignore the passed
            // 'factory' pointer
            bslma::Allocator *pDa = bslma::Default::defaultAllocator();
            pDa->deleteObject(obj);
        }
    }
};

//=============================================================================
//                      POLICY BASED TEST FUNCTIONS
//=============================================================================
// The following set of functions use the policies defined in the previous
// section to construct a set of tests that will exhaustively cover the
// potential type-space of valid combinations of type for each set of arguments
// to 'bslma::ManagedPtr::load'.  Each is a function template, taking at least
// a single type parameter describing the type 'bslma::ManagedPtr' should be
// instantiated for.  Most function templates will take additional type
// arguments describing different policies that are used to define the
// functionality of that test.
//
// This decomposition into 11 test policies and 10 test functions allows us to
// generate over 200 distinct test functions that in turn may be specified for
// each of the 5 types we instantiate 'bslma::ManagedPtr' with for testings.
// Note that not all 200 tests are valid for each of the 5 types, and indeed
// many will not compile if instantiated.
//
// In order to sequentially test each state and permutation of state changes we
// generate large test tables for each of our 5 test types taking the address
// of each valid test function that can be instantiated.  For completeness and
// ease of auditing, we list all combinations of function and policy for each
// of the 5 test types, and comment out only those we believe must be disabled.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct TestCtorArgs {
    // This struct holds the set of arguments that will be passed into a policy
    // based test function.  It collects all information for the range of tests
    // and expectations to be set up on entry, and reported on exit.

    bool d_useDefault;  // Set to true if the test uses the default allocator
    unsigned int d_config; // Valid values are 0-3.  The low-bit represents
                           // whether to pass a null for 'object', the second
                           // bit whether to pass a null for 'factory'.
};

struct TestUtil {
    template <class TARGET_TYPE>
    static void *stripPointerType(TARGET_TYPE *ptr);
        // Return the specified 'ptr' safely cast to a 'void *'.  Note that
        // this function will cast away constness, which may yield undefined
        // behavior in later code if an attempt is made to modify the original
        // object that is being pointed to.
};

template <class TARGET_TYPE>
void *TestUtil::stripPointerType(TARGET_TYPE *ptr)
{
    return const_cast<void*>(static_cast<const void*>(ptr));
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// No policies needed for 'load' of empty managed pointers

template <class POINTER_TYPE>
void doConstruct(int callLine, int testLine, int index, TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 1 > args->d_config);

    bslma::ManagedPtr<POINTER_TYPE> testObject;

    POINTER_TYPE *ptr = testObject.get();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);
}

template <class POINTER_TYPE>
void
doConstructOnull(int callLine, int testLine, int index, TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 1 > args->d_config);

    bslma::ManagedPtr<POINTER_TYPE> testObject(0);

    POINTER_TYPE *ptr = testObject.get();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);
}

template <class POINTER_TYPE>
void doConstructOnullFnull(int           callLine,
                           int           testLine,
                           int           index,
                           TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 1 > args->d_config);

    bslma::ManagedPtr<POINTER_TYPE> testObject(0, 0);

    POINTER_TYPE *ptr = testObject.get();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);
}

template <class POINTER_TYPE>
void doConstructOnullFnullDnull(int           callLine,
                                int           testLine,
                                int           index,
                                TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 1 > args->d_config);

    bslma::ManagedPtr<POINTER_TYPE> testObject(0, 0, 0);

    POINTER_TYPE *ptr = testObject.get();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A simple object policy governs loading a single argument
template<class POINTER_TYPE, class OBJECT_POLICY>
void
doConstructObject(int callLine, int testLine, int index, TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 2 > args->d_config);

    typedef typename OBJECT_POLICY::ObjectType ObjectType;

    const bool nullObject  = args->d_config & 1;

    const int expectedCount = nullObject ? 0 : OBJECT_POLICY::DELETE_DELTA;
    int deleteCount = 0;
    ObjectType *pO  = 0;
    if (nullObject) {
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO);

        const bslma::ManagedPtrDeleter del;

        validateManagedState(L_, testObject, 0, del);
    }
    else {
        pO = new ObjectType(&deleteCount);
        args->d_useDefault = false;

        bslma::ManagedPtr<POINTER_TYPE> testObject(pO);

        typedef bslma::ManagedPtr_DefaultDeleter<ObjectType> DeleterClass;
        const bslma::ManagedPtrDeleter del(TestUtil::stripPointerType(pO),
                                           0,
                                           &DeleterClass::deleter);

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }

    ASSERTV(callLine, testLine, index, expectedCount, deleteCount,
            expectedCount == deleteCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bslma::ManagedPtr' object using a factory.
// We now require separate policies for Object and Factory types

template<class POINTER_TYPE, class FACTORY_POLICY>
void
doConstructOnullFactory(int callLine, int testLine, int, TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 1 > args->d_config);

    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    bslma::TestAllocator ta("Test ConstructOnull", g_veryVeryVeryVerbose);
    FactoryType *pAlloc = FACTORY_POLICY::factory(&ta);

    const bslma::ManagedPtrDeleter del;

    bslma::ManagedPtr<POINTER_TYPE> testObject(0, pAlloc);
    validateManagedState(L_, testObject, 0, del);
}

template<class POINTER_TYPE, class OBJECT_POLICY, class FACTORY_POLICY>
void
doConstructObjectFactory(int callLine, int testLine, int, TestCtorArgs *args)
{
    BSLMF_ASSERT(FACTORY_POLICY::DELETER_USES_FACTORY);

    ASSERTV(callLine, testLine, args->d_config, 4 > args->d_config);

    const bool nullObject  = args->d_config & 1;
    const bool nullFactory = args->d_config & 2;

    // Given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject && nullFactory;

    // If we are negative-testing, we will create and destroy any target object
    // entirely within this function, so must track with a local counter
    // instead of the 'args' counter.

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    bslma::TestAllocator ta("Test ConstructObject", g_veryVeryVeryVerbose);

    FactoryType *pAlloc = FACTORY_POLICY::factory(&ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    // Load the 'bslma::ManagedPtr' and check that the previous state is
    // correctly cleared.
    if (!negativeTesting) {
        const bslma::ManagedPtrDeleter del;

        ObjectType  *pO = 0;
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF);

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }
    else {
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) printf("\tNegative testing null factory pointer\n");

        int deleteCount = 0;
        ObjectType  *pO = nullObject
                          ? 0
                          : new(*pAlloc)ObjectType(&deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }

        bsls::AssertTestHandlerGuard guard;

        ASSERT_SAFE_FAIL_RAW(
                           bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF));

        pAlloc->deleteObject(pO);

        ASSERTV(deleteCount, OBJECT_POLICY::DELETE_DELTA == deleteCount);
#else
    if (g_verbose) printf("\tNegative testing disabled due to lack of "
                           "exception support\n");
#endif
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bslma::ManagedPtr' object using both a
// factory and a deleter.
//
// First we perform negative testing when the 'deleter' argument is equal to a
// null pointer.  Note that passing a null pointer literal will produce a
// compile time error in this case, so we store the null in a variable of the
// desired function-pointer type.

template<class POINTER_TYPE, class OBJECT_POLICY, class FACTORY_POLICY>
void doConstructObjectFactoryDzero(int           callLine,
                                   int           testLine,
                                   int,
                                   TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 4 > args->d_config);

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    void (*nullFn)(void *, void*) = 0;

    // given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject;

    // If we are negative-testing, we will create and destroy any target object
    // entirely within this function, so must track with a local counter
    // instead of the 'args' counter.
    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    bslma::TestAllocator ta("Test ConstructObjectDzero",
                            g_veryVeryVeryVerbose);

    FactoryType *pAlloc = FACTORY_POLICY::factory(&ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    if (!negativeTesting) {
        ObjectType *pO = 0;
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF, nullFn);

        const bslma::ManagedPtrDeleter del;

        validateManagedState(L_, testObject, 0, del);
    }
    else {
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) printf("\tNegative testing null factory pointer\n");

        int deleteCount = 0;
        ObjectType *pO = new(*pAlloc)ObjectType(&deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
        const int expectedCount = OBJECT_POLICY::DELETE_DELTA;

        bsls::AssertTestHandlerGuard guard;

        ASSERT_SAFE_FAIL_RAW(
                   bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF, nullFn));
        ASSERT_SAFE_FAIL_RAW(
                   bslma::ManagedPtr<POINTER_TYPE> testObject(pO,  0, nullFn));

        pAlloc->deleteObject(pO);
        ASSERTV(expectedCount, deleteCount, expectedCount == deleteCount);
#else
        if (g_verbose) printf("\tNegative testing disabled due to lack of "
                               "exception support\n");
#endif
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Next we supply the actual deleter argument, which now requires three
// separate policies.  Note that the 'deleter' policy is in turn parameterized
// on the types it expects to see, which may be different to (but compatible
// with) the actual 'object' and 'factory' policies used in a given test.

template<class POINTER_TYPE,
         class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
void doConstructObjectFactoryDeleter(int           callLine,
                                     int           testLine,
                                     int           index,
                                     TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 4 > args->d_config);

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    if (nullFactory && FACTORY_POLICY::DELETER_USES_FACTORY) {
        // It is perfectly well defined to pass a null pointer as the factory
        // if it is not going to be used by the deleter.  We cannot assert this
        // condition in the 'bslma::ManagedPtr' component, so simply exit from
        // this test case, rather than try negative testing strategies.  Note
        // that some factory/deleter policies do not actually use the factory
        // argument when running the deleter.  These must be allowed to
        // continue through the rest of this test.
        return;                                                       // RETURN
    }

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    bslma::TestAllocator ta("Test ConstructObjectDeleter",
                            g_veryVeryVeryVerbose);

    FactoryType *pAlloc = FACTORY_POLICY::factory(&ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    DeleterType *deleter = DELETER_POLICY::deleter();

    const int expectedCount = nullObject
                              ? 0
                              : OBJECT_POLICY::DELETE_DELTA;

    int deleteCount = 0;
    ObjectType *pO  = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
    }

    {
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF, deleter);

        const bslma::ManagedPtrDeleter del(TestUtil::stripPointerType(pO),
                                           pF,
                 reinterpret_cast<bslma::ManagedPtrDeleter::Deleter>(deleter));

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }

    ASSERTV(callLine, testLine, index, expectedCount, deleteCount,
            expectedCount == deleteCount);
}

// Next we supply the actual deleter argument, which now requires three
// separate policies.  Note that the 'deleter' policy is in turn parameterized
// on the types it expects to see, which may be different to (but compatible
// with) the actual 'object' and 'factory' policies used in a given test.

template<class POINTER_TYPE,
         class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
void doConstructObjectFactoryDeleter2(int           callLine,
                                      int           testLine,
                                      int           index,
                                      TestCtorArgs *args)
{
    ASSERTV(callLine, testLine, args->d_config, 8 > args->d_config);

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;
    bool voidFactory = args->d_config & 4;

    if (nullFactory && FACTORY_POLICY::DELETER_USES_FACTORY) {
        // It is perfectly well defined to pass a null pointer as the factory
        // if it is not going to be used by the deleter.  We cannot assert this
        // condition in the 'bslma::ManagedPtr' component, so simply exit from
        // this test case, rather than try negative testing strategies.  Note
        // that some factory/deleter policies do not actually use the factory
        // argument when running the deleter.  These must be allowed to
        // continue through the rest of this test.
        return;                                                       // RETURN
    }

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    bslma::TestAllocator ta("Test ConstructObjectDeleter2",
                            g_veryVeryVeryVerbose);

    FactoryType *pAlloc = FACTORY_POLICY::factory(&ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    DeleterType *deleter = DELETER_POLICY::deleter();

    const int expectedCount = nullObject
                              ? 0
                              : OBJECT_POLICY::DELETE_DELTA;

    int deleteCount = 0;
    ObjectType *pO  = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
    }

    if (!voidFactory)
    {
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO, pF, deleter);

        const bslma::ManagedPtrDeleter del(TestUtil::stripPointerType(pO),
                                           pF,
                                           deleter);

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }
    else{
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO,
                                                   static_cast<void *>(pF),
                                                   deleter);

        const bslma::ManagedPtrDeleter del(TestUtil::stripPointerType(pO),
                                           pF,
                                           deleter);

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }

    ASSERTV(callLine, testLine, index, expectedCount, deleteCount,
            expectedCount == deleteCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Finally we test the small set of policies that combine to allow passing a
// null pointer literal as the factory.  This requires a deleter that will not
// use the factory pointer.
template<class POINTER_TYPE, class OBJECT_POLICY, class DELETER_POLICY>
void doConstructObjectFnullDeleter(int           callLine,
                                   int           testLine,
                                   int           index,
                                   TestCtorArgs *args)
{
    BSLMF_ASSERT(!DELETER_POLICY::DELETER_USES_FACTORY);

    ASSERTV(callLine, testLine, args->d_config, 4 > args->d_config);

    bool nullObject  = args->d_config & 1;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    const int expectedCount = nullObject
                              ? 0
                              : OBJECT_POLICY::DELETE_DELTA;

    int deleteCount = 0;
    ObjectType *pO  = 0;
    if (!nullObject) {
        bslma::Allocator *pA = bslma::Default::defaultAllocator();
        pO = new(*pA)ObjectType(&deleteCount);
        args->d_useDefault  = true;
    }

    DeleterType *deleter = DELETER_POLICY::deleter();
    {
        bslma::ManagedPtr<POINTER_TYPE> testObject(pO, 0, deleter);

        const bslma::ManagedPtrDeleter del(TestUtil::stripPointerType(pO),
                                           0,
                 reinterpret_cast<bslma::ManagedPtrDeleter::Deleter>(deleter));

        POINTER_TYPE *pTarget = pO;  // implicit cast-to-base etc.
        validateManagedState(L_, testObject, pTarget, del);
    }

    ASSERTV(callLine, testLine, index, expectedCount, deleteCount,
            expectedCount == deleteCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// No policies needed for 'load' of empty managed pointers

template <class POINTER_TYPE>
void doLoad(int                         callLine,
            int                         testLine,
            int                         index,
            TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load();
    args->d_deleteDelta = 0;

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template <class POINTER_TYPE>
void doLoadOnull(int                         callLine,
                 int                         testLine,
                 int                         index,
                 TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load(0);
    args->d_deleteDelta = 0;

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template <class POINTER_TYPE>
void doLoadOnullFnull(int                         callLine,
                      int                         testLine,
                      int                         index,
                      TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load(0, 0);
    args->d_deleteDelta = 0;

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template <class POINTER_TYPE>
void doLoadOnullFnullDnull(int                         callLine,
                           int                         testLine,
                           int                         index,
                           TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load(0, 0, 0);
    args->d_deleteDelta = 0;

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A simple object policy governs loading a single argument

template<class POINTER_TYPE, class OBJECT_POLICY>
void doLoadObject(int                         callLine,
                  int                         testLine,
                  int                         index,
                  TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const bool nullObject  = args->d_config & 1;

    const int expectedCount = args->d_deleteDelta;

    typedef typename OBJECT_POLICY::ObjectType ObjectType;

    ObjectType *pO = 0;
    if (nullObject) {
        args->d_p->load(pO);
        args->d_deleteDelta = 0;
    }
    else {
        bslma::Allocator& da = *bslma::Default::defaultAllocator();
        pO = new(da)ObjectType(&args->d_deleteCount);
        args->d_useDefault = true;

        args->d_p->load(pO);
        args->d_deleteDelta = OBJECT_POLICY::DELETE_DELTA;
    }

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);

    // If we are feeling brave, verify that 'd_p.deleter' has the expected
    // 'object', 'factory' and 'deleter'
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bslma::ManagedPtr' object using a factory.
// We now require separate policies for Object and Factory types

template<class POINTER_TYPE, class FACTORY_POLICY>
void doLoadOnullFactory(int                         callLine,
                        int                         testLine,
                        int                         index,
                        TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FACTORY_POLICY::factory(args->d_ta);

    if (FACTORY_POLICY::USE_DEFAULT) {
        args->d_useDefault = true;
    }

    args->d_p->load(0, pAlloc);
    args->d_deleteDelta = 0;

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template<class POINTER_TYPE, class OBJECT_POLICY, class FACTORY_POLICY>
void doLoadObjectFactory(int                         callLine,
                         int                         testLine,
                         int                         index,
                         TestLoadArgs<POINTER_TYPE> *args)
{
    BSLMF_ASSERT(FACTORY_POLICY::DELETER_USES_FACTORY);

    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const bool nullObject  = args->d_config & 1;
    const bool nullFactory = args->d_config & 2;

    const int expectedCount = args->d_deleteDelta;

    // Given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject && nullFactory;

#if !defined(BDE_BUILD_TARGET_EXC)
    if (negativeTesting) {
        if (g_veryVeryVerbose) printf(
     "\t\t\t\t\tNegative testing disabled due to lack of exception support\n");
        return;                                                       // RETURN
    }
#endif

    if (g_veryVeryVerbose) {
        printf("\t\t\t\t\tPerforming ");
        if (negativeTesting) printf("(negative) ");
        printf("test for 'load((");
        printf(nullObject ? "0," : "obj,");
        printf(nullFactory ? " 0)'\n" : " factory)'\n");
    }

    // If we are negative-testing, we will create and destroy any target object
    // entirely within this function, so must track with a local counter
    // instead of the 'args' counter.
    int deleteCount = 0;

    int *counter = negativeTesting ? &deleteCount : &args->d_deleteCount;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FACTORY_POLICY::factory(args->d_ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    ObjectType  *pO = nullObject ? 0 : new(*pAlloc)ObjectType(counter);

    if (FACTORY_POLICY::USE_DEFAULT) {
        args->d_useDefault = true;
    }

    // Load the 'bslma::ManagedPtr' and check that the previous state is
    // correctly cleared.
    if (!negativeTesting) {
        args->d_p->load(pO, pF);
        args->d_deleteDelta = nullObject ? 0 : OBJECT_POLICY::DELETE_DELTA;

        ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
                expectedCount == args->d_deleteCount);

        POINTER_TYPE *ptr = args->d_p->ptr();
        ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);
    }
    else {
        bsls::AssertTestHandlerGuard guard;

        ASSERT_SAFE_FAIL(args->d_p->load(pO, pF));

        pAlloc->deleteObject(pO);

        ASSERTV(deleteCount, OBJECT_POLICY::DELETE_DELTA == deleteCount);
    }

    // If we are feeling brave, verify that 'p.deleter' has the expected
    // 'object', 'factory' and 'deleter'
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bslma::ManagedPtr' object using both a
// factory and a deleter.
//
// First we perform negative testing when the 'deleter' argument is equal to a
// null pointer.  Note that passing a null pointer literal will produce a
// compile time error in this case, so we store the null in a variable of the
// desired function-pointer type.

template<class POINTER_TYPE, class OBJECT_POLICY, class FACTORY_POLICY>
void doLoadObjectFactoryDzero(int                         callLine,
                              int                         testLine,
                              int                         index,
                              TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    void (*nullFn)(void *, void*) = 0;

    const int expectedCount = args->d_deleteDelta;

    // Given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject;

#if !defined(BDE_BUILD_TARGET_EXC)
    if (negativeTesting) {
        if (g_veryVeryVerbose) printf(
     "\t\t\t\t\tNegative testing disabled due to lack of exception support\n");
        return;                                                       // RETURN
    }
#endif

    if (g_veryVeryVerbose) {
        printf("\t\t\t\t\tPerforming ");
        if (negativeTesting) printf("(negative) ");
        printf("test for 'load((");
        printf(nullObject ? "0," : "obj,");
        printf(nullFactory ? " 0" : " factory");
        printf(", nullFn)'\n");
    }

    // If we are negative-testing, we will create and destroy any target object
    // entirely within this function, so must track with a local counter
    // instead of the 'args' counter.
    int deleteCount = 0;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FACTORY_POLICY::factory(args->d_ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    ObjectType *pO = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
    }

    if (!negativeTesting) {
        args->d_p->load(pO, pF, nullFn);
        args->d_deleteDelta = 0;

        ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
                expectedCount == args->d_deleteCount);

        POINTER_TYPE *ptr = args->d_p->ptr();
        ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);
    }
    else {
        bsls::AssertTestHandlerGuard guard;

        ASSERT_SAFE_FAIL(args->d_p->load(pO, pF, nullFn));
        ASSERT_SAFE_FAIL(args->d_p->load(pO,  0, nullFn));

        pAlloc->deleteObject(pO);
        ASSERTV(deleteCount, OBJECT_POLICY::DELETE_DELTA == deleteCount);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Next we supply the actual deleter argument, which now requires three
// separate policies.  Note that the 'deleter' policy is in turn parameterized
// on the types it expects to see, which may be different to (but compatible
// with) the actual 'object' and 'factory' policies used in a given test.

template<class POINTER_TYPE,
         class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
void doLoadObjectFactoryDeleter(int                         callLine,
                                int                         testLine,
                                int                         index,
                                TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    if (nullFactory && FACTORY_POLICY::DELETER_USES_FACTORY) {
        // It is perfectly well defined to pass a null pointer as the factory
        // if it is not going to be used by the deleter.  We cannot assert this
        // condition in the 'bslma::ManagedPtr' component, so simply exit from
        // this test case, rather than try negative testing strategies.  Note
        // that some factory/deleter policies do not actually use the factory
        // argument when running the deleter.  These must be allowed to
        // continue through the rest of this test.
        return;                                                       // RETURN
    }

    const int expectedCount = args->d_deleteDelta;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FACTORY_POLICY::factory(args->d_ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    ObjectType *pO = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&args->d_deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
        args->d_deleteDelta = OBJECT_POLICY::DELETE_DELTA;
    }

    DeleterType *deleter = DELETER_POLICY::deleter();
    args->d_p->load(pO, pF, deleter);

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Next we supply the actual deleter argument, which now requires three
// separate policies.  Note that the 'deleter' policy is in turn parameterized
// on the types it expects to see, which may be different to (but compatible
// with) the actual 'object' and 'factory' policies used in a given test.

template<class POINTER_TYPE,
         class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
void doLoadObjectFactoryDeleter2(int                         callLine,
                                 int                         testLine,
                                 int                         index,
                                 TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;
    bool voidFactory = args->d_config & 4;

    if (nullFactory && FACTORY_POLICY::DELETER_USES_FACTORY) {
        // It is perfectly well defined to pass a null pointer as the factory
        // if it is not going to be used by the deleter.  We cannot assert this
        // condition in the 'bslma::ManagedPtr' component, so simply exit from
        // this test case, rather than try negative testing strategies.  Note
        // that some factory/deleter policies do not actually use the factory
        // argument when running the deleter.  These must be allowed to
        // continue through the rest of this test.
        return;                                                       // RETURN
    }

    const int expectedCount = args->d_deleteDelta;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename FACTORY_POLICY::FactoryType FactoryType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FACTORY_POLICY::factory(args->d_ta);
    FactoryType *pF = nullFactory ? 0 : pAlloc;

    ObjectType *pO = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&args->d_deleteCount);
        if (FACTORY_POLICY::USE_DEFAULT) {
            args->d_useDefault = true;
        }
        args->d_deleteDelta = OBJECT_POLICY::DELETE_DELTA;
    }

    DeleterType *deleter = DELETER_POLICY::deleter();
    if (!voidFactory) {
        args->d_p->load(pO, pF, deleter);
    }
    else {
        args->d_p->load(pO, static_cast<void *>(pF), deleter);
    }

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Finally we test the small set of policies that combine to allow passing a
// null pointer literal as the factory.  This requires a deleter that will not
// use the factory pointer.
template<class POINTER_TYPE, class OBJECT_POLICY, class DELETER_POLICY>
void doLoadObjectFnullDeleter(int                         callLine,
                              int                         testLine,
                              int                         index,
                              TestLoadArgs<POINTER_TYPE> *args)
{
    BSLMF_ASSERT(!DELETER_POLICY::DELETER_USES_FACTORY);

    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;

    const int expectedCount = args->d_deleteDelta;

    typedef typename  OBJECT_POLICY::ObjectType  ObjectType;
    typedef typename DELETER_POLICY::DeleterType DeleterType;

    ObjectType *pO = 0;
    if (!nullObject) {
        bslma::Allocator *pA = bslma::Default::defaultAllocator();
        pO = new(*pA)ObjectType(&args->d_deleteCount);
        args->d_useDefault  = true;
        args->d_deleteDelta = OBJECT_POLICY::DELETE_DELTA;
    }

    DeleterType *deleter = DELETER_POLICY::deleter();
    args->d_p->load(pO, 0, deleter);

    ASSERTV(callLine, testLine, index, expectedCount, args->d_deleteCount,
            expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    ASSERTV(callLine, testLine, index, pO, ptr, pO == ptr);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Once all the testing policies are composed into arrays of test functions, we
// need some driver functions to iterate over each valid combination (stored in
// separate test tables) and check that the behavior transitions correctly in
// each case.
struct NullPolicy {
    // This class is a tag-type that represents passing a null pointer value as
    // an argument, whose position is determined relative to other policies
    // supplied to a 'TestPolicy' constructor,
};

template<class TARGET>
struct TestPolicy {
    // PUBLIC TYPES
    typedef void TestLoadFn(int, int, int, TestLoadArgs<TARGET> *);
    typedef void TestCtorFn(int, int, int, TestCtorArgs *);

  private:
    // DATA
    TestLoadFn  *d_testLoad;
    TestCtorFn  *d_testCtor;
    int          d_configs;

  public:
    // CREATORS
    TestPolicy();
    explicit TestPolicy(NullPolicy);
    TestPolicy(NullPolicy, NullPolicy);
    TestPolicy(NullPolicy, NullPolicy, NullPolicy);

    template<class OBJECT_POLICY>
    explicit TestPolicy(OBJECT_POLICY);

    template<class FACTORY_POLICY>
    TestPolicy(NullPolicy, FACTORY_POLICY);

    template<class OBJECT_POLICY, class FACTORY_POLICY>
    TestPolicy(OBJECT_POLICY, FACTORY_POLICY);

    template<class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
    TestPolicy(OBJECT_POLICY, FACTORY_POLICY, DELETER_POLICY);

    template<class OBJECT_POLICY, class FACTORY_POLICY,
             class DELETER_OBJECT_POLICY, class DELETER_FACTORY_POLICY>
    TestPolicy(OBJECT_POLICY,
               FACTORY_POLICY,
               DVoidVoid<DELETER_OBJECT_POLICY, DELETER_FACTORY_POLICY>);

    template<class OBJECT_POLICY, class DELETER_POLICY>
    TestPolicy(OBJECT_POLICY, NullPolicy, DELETER_POLICY);

    template<class OBJECT_POLICY,
             class DELETER_OBJECT_POLICY,
             class DELETER_FACTORY_POLICY>
    TestPolicy(OBJECT_POLICY, NullPolicy, DVoidVoid<DELETER_OBJECT_POLICY,
                                                    DELETER_FACTORY_POLICY>);

    template<class OBJECT_POLICY, class FACTORY_POLICY>
    TestPolicy(OBJECT_POLICY, FACTORY_POLICY, NullPolicy);
        // Create a 'TestPolicy' object configured to run tests according to
        // the passed policy parameters.  Note that policy parameters are all
        // stateless tag-types, and the configuration is entirely a matter of
        // tag-dispatch used to encode type information.

    // ACCESSORS
    unsigned int configs() const { return d_configs; }

    void testCtor(int           LINE_1,
                  int           LINE_2,
                  int           loopIndex,
                  TestCtorArgs *config) const;
        // Run the test of the 'bslma::ManagedPtr' constructor indicated by the
        // specified 'config', called from the specified 'LINE_1' (which was,
        // in turn, called from the specified 'LINE_2') by the specified
        // 'loopIndex'th iteration of the test loop.

    void testLoad(int                   LINE_1,
                  int                   LINE_2,
                  int                   loopIndex,
                  TestLoadArgs<TARGET> *config) const;
        // Run the test of 'bslma::ManagedPtr::load' indicated by the specified
        // 'config', called from the specified 'LINE_1' (which was, in turn,
        // called from the specified 'LINE_2') by the specified 'loopIndex'th
        // iteration of the test loop.
};

// CREATORS
template<class TARGET>
inline
TestPolicy<TARGET>::TestPolicy()
: d_testLoad(&doLoad     <TARGET>)
, d_testCtor(&doConstruct<TARGET>)
, d_configs(1)
{
}

template<class TARGET>
inline
TestPolicy<TARGET>::TestPolicy(NullPolicy)
: d_testLoad(&doLoadOnull     <TARGET>)
, d_testCtor(&doConstructOnull<TARGET>)
, d_configs(1)
{
}

template<class TARGET>
inline
TestPolicy<TARGET>::TestPolicy(NullPolicy, NullPolicy)
: d_testLoad(&doLoadOnullFnull     <TARGET>)
, d_testCtor(&doConstructOnullFnull<TARGET>)
, d_configs(1)
{
}

template<class TARGET>
inline
TestPolicy<TARGET>::TestPolicy(NullPolicy, NullPolicy, NullPolicy)
: d_testLoad(&doLoadOnullFnullDnull     <TARGET>)
, d_testCtor(&doConstructOnullFnullDnull<TARGET>)
, d_configs(1)
{
}

template<class TARGET>
template<class OBJECT_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(OBJECT_POLICY)
: d_testLoad(&doLoadObject     <TARGET, OBJECT_POLICY>)
, d_testCtor(&doConstructObject<TARGET, OBJECT_POLICY>)
, d_configs(2)
{
}

template<class TARGET>
template<class FACTORY_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(NullPolicy, FACTORY_POLICY)
: d_testLoad(&doLoadOnullFactory     <TARGET, FACTORY_POLICY>)
, d_testCtor(&doConstructOnullFactory<TARGET, FACTORY_POLICY>)
, d_configs(1)
{
}

template<class TARGET>
template<class OBJECT_POLICY, class FACTORY_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(OBJECT_POLICY, FACTORY_POLICY)
: d_testLoad(&doLoadObjectFactory     <TARGET, OBJECT_POLICY, FACTORY_POLICY>)
, d_testCtor(&doConstructObjectFactory<TARGET, OBJECT_POLICY, FACTORY_POLICY>)
, d_configs(4)
{
}

template<class TARGET>
template<class OBJECT_POLICY, class FACTORY_POLICY, class DELETER_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(OBJECT_POLICY, FACTORY_POLICY, DELETER_POLICY)
: d_testLoad(&doLoadObjectFactoryDeleter<TARGET,
                                         OBJECT_POLICY,
                                         FACTORY_POLICY,
                                         DELETER_POLICY>)
, d_testCtor(&doConstructObjectFactoryDeleter<TARGET,
                                              OBJECT_POLICY,
                                              FACTORY_POLICY,
                                              DELETER_POLICY>)
, d_configs(4)
{
}

template<class TARGET>
template<class OBJECT_POLICY,
         class FACTORY_POLICY,
         class DELETER_OBJECT_POLICY,
         class DELETER_FACTORY_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(
                      OBJECT_POLICY,
                      FACTORY_POLICY,
                      DVoidVoid<DELETER_OBJECT_POLICY, DELETER_FACTORY_POLICY>)
: d_testLoad(&doLoadObjectFactoryDeleter2<TARGET,
                                          OBJECT_POLICY,
                                          FACTORY_POLICY,
                                          DVoidVoid<DELETER_OBJECT_POLICY,
                                                    DELETER_FACTORY_POLICY> >)
, d_testCtor(&doConstructObjectFactoryDeleter2<
                                           TARGET,
                                           OBJECT_POLICY,
                                           FACTORY_POLICY,
                                           DVoidVoid<DELETER_OBJECT_POLICY,
                                                     DELETER_FACTORY_POLICY> >)
, d_configs(8)
{
}

template<class TARGET>
template<class OBJECT_POLICY, class DELETER_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(OBJECT_POLICY, NullPolicy, DELETER_POLICY)
: d_testLoad(&doLoadObjectFnullDeleter<TARGET, OBJECT_POLICY, DELETER_POLICY>)
, d_testCtor(
         &doConstructObjectFnullDeleter<TARGET, OBJECT_POLICY, DELETER_POLICY>)
, d_configs(2)
{
}

template<class TARGET>
template<class OBJECT_POLICY,
         class DELETER_OBJECT_POLICY,
         class DELETER_FACTORY_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(
                      OBJECT_POLICY,
                      NullPolicy,
                      DVoidVoid<DELETER_OBJECT_POLICY, DELETER_FACTORY_POLICY>)
: d_testLoad(&doLoadObjectFnullDeleter<TARGET,
                                       OBJECT_POLICY,
                                       DVoidVoid<DELETER_OBJECT_POLICY,
                                                 DELETER_FACTORY_POLICY> >)
, d_testCtor(
            &doConstructObjectFnullDeleter<TARGET,
                                           OBJECT_POLICY,
                                           DVoidVoid<DELETER_OBJECT_POLICY,
                                                     DELETER_FACTORY_POLICY> >)
, d_configs(2)
{
}

template<class TARGET>
template<class OBJECT_POLICY, class FACTORY_POLICY>
inline
TestPolicy<TARGET>::TestPolicy(OBJECT_POLICY, FACTORY_POLICY, NullPolicy)
: d_testLoad(&doLoadObjectFactoryDzero<TARGET, OBJECT_POLICY, FACTORY_POLICY>)
, d_testCtor(
         &doConstructObjectFactoryDzero<TARGET, OBJECT_POLICY, FACTORY_POLICY>)
, d_configs(4)
{
}

// ACCESSORS
template<class TARGET>
inline
void
TestPolicy<TARGET>::testCtor(int           LINE_1,
                             int           LINE_2,
                             int           loopIndex,
                             TestCtorArgs *config) const
{
    BSLS_ASSERT_SAFE(config);

    d_testCtor(LINE_1, LINE_2, loopIndex, config);
}

template<class TARGET>
inline
void TestPolicy<TARGET>::testLoad(int                   LINE_1,
                                  int                   LINE_2,
                                  int                   loopIndex,
                                  TestLoadArgs<TARGET> *config) const
{
    BSLS_ASSERT_SAFE(config);

    d_testLoad(LINE_1, LINE_2, loopIndex, config);
}


template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testConstructors(
                   int                            callLine,
                   const TestPolicy<TEST_TARGET>(&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function iterates all viable variations of test functions composed
    // of the policies above, to verify that all 'bslma::ManagedPtr::load'
    // behave according to contract.  First, we call 'load' on an empty managed
    // pointer using a test function from the passed array, confirming that the
    // managed pointer takes up the correct state.  Then we allow that pointer
    // to go out of scope, and confirm that any managed object is destroyed
    // using the correct deleter.  Next we repeat the test, setting up the
    // same, now well-known, state of the managed pointer, and replace it with
    // a second call to load (by a second iterator over the array of test
    // functions).  We confirm that the original state and managed object (if
    // any) are destroyed correctly, and that the expected new state has been
    // established correctly.  Finally, we allow this pointer to leave scope
    // and confirm that all managed objects are destroyed correctly and all
    // allocated memory has been reclaimed.  At each stage, we perform negative
    // testing where appropriate, and check that no memory is being allocated
    // other than by the object allocator, or the default allocator only for
    // those test functions that return a state indicating that they used the
    // default allocator.

    bslma::TestAllocator *ga = dynamic_cast<bslma::TestAllocator *>
                                           (bslma::Default::globalAllocator());

    bslma::TestAllocator *da = dynamic_cast<bslma::TestAllocator *>
                                          (bslma::Default::defaultAllocator());

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned config = 0; config != TEST_ARRAY[i].configs(); ++config)
        {
            TestCtorArgs args = { false, config };

            bslma::TestAllocatorMonitor gam(ga);
            bslma::TestAllocatorMonitor dam(da);

            args.d_useDefault = false;

            TEST_ARRAY[i].testCtor(callLine, L_, i, &args);

            ASSERTV(L_, i, gam.isInUseSame());
            ASSERTV(L_, i, gam.isMaxSame());

            ASSERTV(L_, i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(L_, i, dam.isMaxSame());
            }
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testLoadOps(int                             callLine,
                 const TestPolicy<TEST_TARGET> (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function iterates all viable variations of test functions composed
    // of the policies above, to verify that all 'bslma::ManagedPtr::load'
    // behave according to contract.  First, we call 'load' on an empty managed
    // pointer using a test function from the passed array, confirming that the
    // managed pointer takes up the correct state.  Then we allow that pointer
    // to go out of scope, and confirm that any managed object is destroyed
    // using the correct deleter.  Next we repeat the test, setting up the
    // same, now well-known, state of the managed pointer, and replace it with
    // a second call to load (by a second iterator over the array of test
    // functions).  We confirm that the original state and managed object (if
    // any) are destroyed correctly, and that the expected new state has been
    // established correctly.  Finally, we allow this pointer to leave scope
    // and confirm that all managed objects are destroyed correctly and all
    // allocated memory has been reclaimed.  At each stage, we perform negative
    // testing where appropriate, and check that no memory is being allocated
    // other than by the object allocator, or the default allocator only for
    // those test functions that return a state indicating that they used the
    // default allocator.
    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    // gcc insists on warning on empty aggregate initialization when trying to
    // force zero-initialization.  This is a bad warning as the code functions
    // correctly (and idiomatically) as intended, and the rewrite is distinctly
    // inferior, creating and copying a temporary object that we hope the
    // optimizer will eliminate.
//    TestLoadArgs<TEST_TARGET> args = {};
    TestLoadArgs<TEST_TARGET> args = TestLoadArgs<TEST_TARGET>();

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        if (g_veryVerbose) printf(
               "\tTesting 'load' into object constructed by function no. %d\n",
                                                                            i);

        for (unsigned configI = 0; configI != TEST_ARRAY[i].configs();
                                                                   ++configI) {
            if (g_veryVerbose) printf("\t\tTesting config %d\n", configI);

            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i].testLoad(callLine, L_, i, &args);
            }
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }

            for (int j = 0; j != TEST_ARRAY_SIZE; ++j) {
                if (g_veryVeryVerbose) {
                    printf("\t\t\tInner-loop - testing function no. %d\n", j);
                }

                for (unsigned configJ = 0; configJ != TEST_ARRAY[j].configs();
                                                                   ++configJ) {
                    if (g_veryVeryVerbose) {
                        printf("\t\t\t\tInner-loop - config %d\n", configJ);
                    }

                    bslma::TestAllocatorMonitor dam2(&da);

                    bslma::TestAllocator ta("TestLoad 2",
                                            g_veryVeryVeryVerbose);

                    TestPointer p;
                    ASSERT(0 == p.get());

                    args.d_p  = &p;
                    args.d_ta = &ta;
                    args.d_config = configI;

                    args.d_deleteCount = 0;
                    args.d_deleteDelta = 0;
                    args.d_useDefault  = false;
                    TEST_ARRAY[i].testLoad(callLine, L_, i, &args);

                    args.d_config = configJ;
                    args.d_deleteCount = 0;
                    TEST_ARRAY[j].testLoad(callLine, L_, j, &args);

                    // Clear 'deleteCount' before 'p' is destroyed.
                    args.d_deleteCount = 0;

                    ASSERTV(i, gam.isInUseSame());
                    ASSERTV(i, gam.isMaxSame());

                    if (!args.d_useDefault) {
                        ASSERTV(i, dam2.isInUseSame());
                        ASSERTV(i, dam2.isMaxSame());
                    }
                }
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, dam.isInUseSame());
            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The 'AliasTestTypeN' metafunctions suggest a type to use when testing the
// ManagedPtr alias facility, given a managed pointer owning an object of the
// parameterized 'TYPE'.

template <class TYPE>
struct AliasTestType1 {
    typedef MyDerivedObject type;
};

template <class TYPE>
struct AliasTestType2 {
    typedef MySecondDerivedObject type;
};

template <>
struct AliasTestType1<Base> {
    typedef Base2 type;
};

template <>
struct AliasTestType2<Base> {
    typedef Composite type;
};

template <>
struct AliasTestType1<Base2> {
    typedef Base2 type;
};

template <>
struct AliasTestType2<Base2> {
    typedef Composite type;
};

template <class TYPE>
struct AliasTestType2<const TYPE> : AliasTestType2<TYPE> {};

template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testLoadAliasOps1(
                  int                             callLine,
                  const TestPolicy<TEST_TARGET> (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function validates the simple scenario of calling 'loadAlias' to
    // create a simple aliased managed pointer, and confirming that pointer
    // destroyed its managed object with the correct deleter and reclaims all
    // memory when destroyed by leaving scope.
    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned configI = 0; configI != TEST_ARRAY[i].configs();
                                                                   ++configI) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            // gcc insists on warning on empty aggregate initialization when
            // trying to force zero-initialization.  This is a bad warning as
            // the code functions correctly (and idiomatically) as intended,
            // and the rewrite is distinctly inferior, creating and copying a
            // temporary object that we hope the optimizer will eliminate.
//            TestLoadArgs<TEST_TARGET> args = {};
            TestLoadArgs<TEST_TARGET> args = TestLoadArgs<TEST_TARGET>();
            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i].testLoad(callLine, L_, i, &args);

                // All operations from here are effectively 'move' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_veryVerbose) printf(
                                         "\tNegative testing null pointers\n");

                TestPointer pAlias;
                if (0 == p.get()) {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias.loadAlias(p, &aliasTarget));
                    ASSERT_SAFE_PASS(pAlias.loadAlias(p, 0));

                    ASSERTV(p.get(),      0 == p.get());
                    ASSERTV(pAlias.get(), 0 == pAlias.get());
                }
                else {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias.loadAlias(p, 0));
                    ASSERT_SAFE_PASS(pAlias.loadAlias(p, &aliasTarget));

                    ASSERTV(p.get(),      0 == p.get());
                    ASSERTV(pAlias.get(), &aliasTarget == pAlias.get());
                }
#else
                TestPointer pAlias;
                TEST_TARGET *pTarget = 0 == p.get() ? 0 : &aliasTarget;

                pAlias.loadAlias(p, pTarget);

                ASSERTV(p.get(), 0 == p.get());
                ASSERTV(pTarget, pAlias.get(), pTarget == pAlias.get());
#endif

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testLoadAliasOps2(
                   int                            callLine,
                   const TestPolicy<TEST_TARGET>(&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This scenario tests the correct state change for following a 'loadAlias'
    // call with another 'loadAlias' call.  It will also test derived* -> base*
    // conversions for the aliased pointer, and non-'const *' -> 'const *'.
    // The test process is to take an empty 'bslma::ManagedPtr' object and
    // 'load' a known state into it using a well-known test function.  Then we
    // "alias" this pointer by calling 'loadAlias' on another (empty) managed
    // pointer object, and check that the new aliased state has been created
    // correctly, without allocating any memory, and that the original managed
    // pointer object is now empty.  Next we establish another well-known
    // managed pointer value, and call 'loadAlias' again on the pointer in the
    // existing aliased state.  We again confirm that the aliased state is
    // transferred without allocating any memory, but also that the object
    // managed by the original 'bslma::ManagedPtr' object has now been
    // destroyed as expected.  Finally we let this final managed pointer object
    // leave scope and confirm that all managed objects have been destroyed, as
    // expected, and that all memory has been reclaimed.  At each step, we
    // further implement negative testing if a null pointer may be passed, and
    // that passing a null pointer would yield (negatively testable) undefined
    // behavior.

    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    // gcc insists on warning on empty aggregate initialization when trying to
    // force zero-initialization.  This is a bad warning as the code functions
    // correctly (and idiomatically) as intended, and the rewrite is distinctly
    // inferior, creating and copying a temporary object that we hope the
    // optimizer will eliminate.
//    TestLoadArgs<TEST_TARGET> args = {};
    TestLoadArgs<TEST_TARGET> args = TestLoadArgs<TEST_TARGET>();

    int aliasDeleterCount1 = 0;
    int aliasDeleterCount2 = 0;
    typename AliasTestType1<TEST_TARGET>::type alias1(&aliasDeleterCount1);
    typename AliasTestType2<TEST_TARGET>::type alias2(&aliasDeleterCount2);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned configI = 0; configI != TEST_ARRAY[i].configs();
                                                                   ++configI) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i].testLoad(callLine, L_, i, &args);

                // All operations from here are effectively 'mode' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_veryVerbose) printf(
                                         "\tNegative testing null pointers\n");

                // Declare variables so that the lifetime extends to the end of
                // the loop.  Otherwise, the 'ta' monitor tests will flag the
                // 'pAlias2' destructor for freeing the original object.
                TestPointer pAlias1;
                TestPointer pAlias2;

                if (0 == p.get()) {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias1.loadAlias(p, &alias1));
                    ASSERT_SAFE_PASS(pAlias1.loadAlias(p, 0));

                    ASSERTV(p.get(),       0 == p.get());
                    ASSERTV(pAlias1.get(), 0 == pAlias1.get());
                }
                else {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias1.loadAlias(p, 0));
                    ASSERT_SAFE_PASS(pAlias1.loadAlias(p, &alias1));

                    ASSERTV(p.get(), 0 == p.get());
                    ASSERTV(&alias1, pAlias1.get(), &alias1 == pAlias1.get());

                    ASSERT_SAFE_FAIL(pAlias2.loadAlias(pAlias1, 0));
                    ASSERT_SAFE_PASS(pAlias2.loadAlias(pAlias1, &alias2));

                    ASSERTV(pAlias1.get(), 0 == pAlias1.get());
                    ASSERTV(&alias2, pAlias2.get(), &alias2 == pAlias2.get());
                }
#else
                TestPointer pAlias1;
                TEST_TARGET *pTarget = 0 == p.get() ? 0 : &alias1;

                pAlias1.loadAlias(p, pTarget);

                ASSERTV(p.get(), 0 == p.get());
                ASSERTV(pTarget, pAlias1.get(), pTarget == pAlias1.get());
#endif

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testLoadAliasOps3(
                   int                            callLine,
                   const TestPolicy<TEST_TARGET>(&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function tests the correct interaction of 'load' and 'loadAlias'.
    // Initially, an empty 'bslma::ManagedPtr' object is loaded into a well
    // defined non-empty state using a well-known test loader.  This state is
    // then transferred to a second empty pointer through a 'loadAlias' call,
    // and we validate that no memory is allocated for this operation, and the
    // state is correctly transferred.  Next we replace this aliased state with
    // another well-known state using 'load' again.  We test that the initial
    // state is correctly destroyed, and the new state is in place without any
    // aliasing.  Then we allow this final state to be destroyed, and confirm
    // that all managed objects have been correctly disposed of.
    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (int j = 0; j != TEST_ARRAY_SIZE; ++j) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            // gcc insists on warning on empty aggregate initialization when
            // trying to force zero-initialization.  This is a bad warning as
            // the code functions correctly (and idiomatically) as intended,
            // and the rewrite is distinctly inferior, creating and copying a
            // temporary object that we hope the optimizer will eliminate.
//            TestLoadArgs<TEST_TARGET> args = {};
            TestLoadArgs<TEST_TARGET> args = TestLoadArgs<TEST_TARGET>();
            args.d_useDefault = false;
            args.d_config = 0;  // We need only test a fully defined pointer,
                                // there are no concerns about null arguments.
            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i].testLoad(callLine, L_, i, &args);
                if (0 == p.get()) {
                    // We have no interest in tests that create a null pointer,
                    // this scenario is negative tested in testLoadAliasOps1.
                    continue;
                }

                // All operations from here are effectively 'move' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

                TestPointer pAlias;
                pAlias.loadAlias(p, &aliasTarget);

                ASSERTV(p.get(),      0 == p.get());
                ASSERTV(pAlias.get(), &aliasTarget == pAlias.get());

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());

                // Next we load a fresh state into the pointer to verify the
                // final concern for 'load'; that it correctly destroys an
                // aliased state while acquire the new value.
                args.d_p  = &pAlias;

                // The test function itself asserts correct destructor count
                // for this transition, and that the 'pAlias' has the correct
                // final state.
                TEST_ARRAY[j].testLoad(callLine, L_, j, &args);

                ASSERTV(i, gam.isInUseSame());
                ASSERTV(i, gam.isMaxSame());

                if (!args.d_useDefault) {
                    ASSERTV(i, dam.isInUseSame());
                    ASSERTV(i, dam.isMaxSame());
                }

                // Nothing further to assert, but reset 'deleteCount' to verify
                // destroying final objects outside the loop.
                args.d_deleteCount = 0;
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

#if 0
template<class TEST_TARGET, size_t TEST_ARRAY_SIZE>
void testConstructors(int callLine,
                      const TestCtorFn(&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function iterates all viable variations of test functions composed
    // of the policies above, to verify that all 'bslma::ManagedPtr::load'
    // behave according to contract.  First, we call 'load' on an empty managed
    // pointer using a test function from the passed array, confirming that the
    // managed pointer takes up the correct state.  Then we allow that pointer
    // to go out of scope, and confirm that any managed object is destroyed
    // using the correct deleter.  Next we repeat the test, setting up the
    // same, now well-known, state of the managed pointer, and replace it with
    // a second call to load (by a second iterator over the array of test
    // functions).  We confirm that the original state and managed object (if
    // any) are destroyed correctly, and that the expected new state has been
    // established correctly.  Finally, we allow this pointer to leave scope
    // and confirm that all managed objects are destroyed correctly and all
    // allocated memory has been reclaimed.  At each stage, we perform negative
    // testing where appropriate, and check that no memory is being allocated
    // other than by the object allocator, or the default allocator only for
    // those test functions that return a state indicating that they used the
    // default allocator.
    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator *ga = dynamic_cast<bslma::TestAllocator *>
                                           (bslma::Default::globalAllocator());

    bslma::TestAllocator *da = dynamic_cast<bslma::TestAllocator *>
                                          (bslma::Default::defaultAllocator());

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned config = 0; config != TEST_ARRAY[i].configs(); ++config)
        {
            TestCtorArgs args = { false, config};

            bslma::TestAllocatorMonitor gam(ga);
            bslma::TestAllocatorMonitor dam(da);

            args.d_useDefault = false;

            TEST_ARRAY[i](callLine, L_, i, &args);

            ASSERTV(L_, i, gam.isInUseSame());
            ASSERTV(L_, i, gam.isMaxSame());

            ASSERTV(L_, i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(L_, i, dam.isMaxSame());
            }
        }
    }
}
#endif
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template<class  TEST_TARGET,
         class  TEST_FUNCTION_TYPE,
         size_t TEST_ARRAY_SIZE>
void testLoadAliasOps1(int                        callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function validates the simple scenario of calling 'loadAlias' to
    // create a simple aliased managed pointer, and confirming that pointer
    // destroyed its managed object with the correct deleter and reclaims all
    // memory when destroyed by leaving scope.
    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned configI = 0; configI != TEST_ARRAY[i].configs();
                                                                   ++configI) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            TestLoadArgs<TEST_TARGET> args = {};
            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);

                // All operations from here are effectively 'mode' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_veryVerbose)
                                  printf("\tNegative testing null pointers\n");

                TestPointer pAlias;
                if (0 == p.get()) {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias.loadAlias(p, &aliasTarget));
                    ASSERT_SAFE_PASS(pAlias.loadAlias(p, 0));

                    ASSERTV(p.get(),      0 == p.get());
                    ASSERTV(pAlias.get(), 0 == pAlias.get());
                }
                else {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias.loadAlias(p, 0));
                    ASSERT_SAFE_PASS(pAlias.loadAlias(p, &aliasTarget));

                    ASSERTV(p.get(),      0 == p.get());
                    ASSERTV(pAlias.get(), &aliasTarget == pAlias.get());
                }
#else
                TestPointer pAlias;
                TEST_TARGET pTarget = 0 == p.get() ? 0 : &aliasTarget;

                pAlias.loadAlias(p, pTarget);

                ASSERTV(p.get(),  0 == p.get());
                ASSERTV(pTarget, pAlias.get(), pTarget == pAlias.get());
#endif

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

template<class  TEST_TARGET,
         class  TEST_FUNCTION_TYPE,
         size_t TEST_ARRAY_SIZE>
void testLoadAliasOps2(int                        callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This scenario tests the correct state change for following a 'loadAlias'
    // call with another 'loadAlias' call.  It will also test derived* -> base*
    // conversions for the aliased pointer, and non-'const *' -> 'const *'.
    // The test process is to take an empty 'bslma::ManagedPtr' object and
    // 'load' a known state into it using a well-known test function.  Then we
    // "alias" this pointer by calling 'loadAlias' on another (empty) managed
    // pointer object, and check that the new aliased state has been created
    // correctly, without allocating any memory, and that the original managed
    // pointer object is now empty.  Next we establish another well-known
    // managed pointer value, and call 'loadAlias' again on the pointer in the
    // existing aliased state.  We again confirm that the aliased state is
    // transferred without allocating any memory, but also that the object
    // managed by the original 'bslma::ManagedPtr' object has now been
    // destroyed as expected.  Finally we let this final managed pointer object
    // leave scope and confirm that all managed objects have been destroyed, as
    // expected, and that all memory has been reclaimed.  At each step, we
    // further implement negative testing if a null pointer may be passed, and
    // that passing a null pointer would yield (negatively testable) undefined
    // behavior.

    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    TestLoadArgs<TEST_TARGET> args = {};

    int aliasDeleterCount1 = 0;
    int aliasDeleterCount2 = 0;
    typename AliasTestType1<TEST_TARGET>::type alias1(&aliasDeleterCount1);
    typename AliasTestType2<TEST_TARGET>::type alias2(&aliasDeleterCount2);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (unsigned configI = 0; configI != TEST_ARRAY[i].configs();
                                                                   ++configI) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            args.d_useDefault = false;
            args.d_config     = configI;

            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);

                // All operations from here are effectively 'mode' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_veryVerbose)
                                  printf("\tNegative testing null pointers\n");

                // Declare variables so that the lifetime extends to the end
                // of the loop.  Otherwise, the 'ta' monitor tests will flag
                // the 'pAlias2' destructor for freeing the original object.
                TestPointer pAlias1;
                TestPointer pAlias2;

                if (0 == p.get()) {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias1.loadAlias(p, &alias1));
                    ASSERT_SAFE_PASS(pAlias1.loadAlias(p, 0));

                    ASSERTV(p.get(),       0 == p.get());
                    ASSERTV(pAlias1.get(), 0 == pAlias1.get());
                }
                else {
                    bsls::AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL(pAlias1.loadAlias(p, 0));
                    ASSERT_SAFE_PASS(pAlias1.loadAlias(p, &alias1));

                    ASSERTV(p.get(), 0 == p.get());
                    ASSERTV(&alias1, pAlias1.get(), &alias1 == pAlias1.get());

                    ASSERT_SAFE_FAIL(pAlias2.loadAlias(pAlias1, 0));
                    ASSERT_SAFE_PASS(pAlias2.loadAlias(pAlias1, &alias2));

                    ASSERTV(pAlias1.get(), 0 == pAlias1.get());
                    ASSERTV(&alias2, pAlias2.get(), &alias2 == pAlias2.get());
                }
#else
                TestPointer pAlias1;
                TEST_TARGET pTarget = 0 == p.get() ? 0 : &alias1;

                pAlias1.loadAlias(p, pTarget);

                ASSERTV(p.get(), 0 == p.get());
                ASSERTV(pTarget, pAlias1.get(), pTarget == pAlias1.get());
#endif

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

template<class  TEST_TARGET,
         class  TEST_FUNCTION_TYPE,
         size_t TEST_ARRAY_SIZE>
void testLoadAliasOps3(int                        callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function tests the correct interaction of 'load' and 'loadAlias'.
    // Initially, an empty 'bslma::ManagedPtr' object is loaded into a well
    // defined non-empty state using a well-known test loader.  This state is
    // then transferred to a second empty pointer through a 'loadAlias' call,
    // and we validate that no memory is allocated for this operation, and the
    // state is correctly transferred.  Next we replace this aliased state with
    // another well-known state using 'load' again.  We test that the initial
    // state is correctly destroyed, and the new state is in place without any
    // aliasing.  Then we allow this final state to be destroyed, and confirm
    // that all managed objects have been correctly disposed of.

    typedef bslma::ManagedPtr<TEST_TARGET> TestPointer;

    bslma::TestAllocator& ga = dynamic_cast<bslma::TestAllocator&>
                                          (*bslma::Default::globalAllocator());

    bslma::TestAllocator& da = dynamic_cast<bslma::TestAllocator&>
                                         (*bslma::Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for (int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for (int j = 0; j != TEST_ARRAY_SIZE; ++j) {
            bslma::TestAllocatorMonitor gam(&ga);
            bslma::TestAllocatorMonitor dam(&da);

            TestLoadArgs<TEST_TARGET> args = {};
            args.d_useDefault = false;
            args.d_config = 0;  // We need only test a fully defined pointer;
                                // there are no concerns about null arguments.
            {
                bslma::TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.get());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);
                if (0 == p.get()) {
                    // We have no interest in tests that create a null pointer;
                    // this scenario is negative-tested in 'testLoadAliasOps1'.
                    continue;
                }

                // All operations from here are effectively 'move' operations.
                // Check that no more memory is allocated or freed.
                bslma::TestAllocatorMonitor gam2(&ga);
                bslma::TestAllocatorMonitor dam2(&da);
                bslma::TestAllocatorMonitor tam2(&ta);

                TestPointer pAlias;
                pAlias.loadAlias(p, &aliasTarget);

                ASSERTV(p.get(),      0 == p.get());
                ASSERTV(pAlias.get(), &aliasTarget == pAlias.get());

                // Assert that no memory was allocated or freed.
                ASSERTV(i, tam2.isInUseSame());
                ASSERTV(i, tam2.isMaxSame());
                ASSERTV(i, dam2.isInUseSame());
                ASSERTV(i, dam2.isMaxSame());
                ASSERTV(i, gam2.isInUseSame());
                ASSERTV(i, gam2.isMaxSame());

                // Next we load a fresh state into the pointer to verify the
                // final concern for 'load'; that it correctly destroys an
                // aliased state while acquire the new value.
                args.d_p  = &pAlias;

                // The test function itself asserts correct destructor count
                // for this transition, and that the 'pAlias' has the correct
                // final state.
                TEST_ARRAY[j](callLine, L_, j, &args);

                ASSERTV(i, gam.isInUseSame());
                ASSERTV(i, gam.isMaxSame());

                if (!args.d_useDefault) {
                    ASSERTV(i, dam.isInUseSame());
                    ASSERTV(i, dam.isMaxSame());
                }

                // Nothing further to assert, but reset 'deleteCount' to
                // verify destroying final objects outside the loop.
                args.d_deleteCount = 0;
            }

            // Validate the final deleter run when 'p' is destroyed.
            ASSERTV(args.d_deleteCount,   args.d_deleteDelta,
                    args.d_deleteCount == args.d_deleteDelta);

            ASSERTV(i, gam.isInUseSame());
            ASSERTV(i, gam.isMaxSame());

            ASSERTV(i, dam.isInUseSame());
            if (!args.d_useDefault) {
                ASSERTV(i, dam.isMaxSame());
            }
        }
    }
}

// BDE_VERIFY pragma: pop  // end of auditable test functionality

// BDE_VERIFY pragma: push   // Test tables need long lines to read in 2D
// BDE_VERIFY pragma: -LL01  // Line longer than 79 chars

//=============================================================================
// This is the test table for iterating constructor and load functions for
// 'bslma::ManagedPtr<MyTestObject>'.  The same test table is created for each
// of the main 5 tested pointer types, and then the invalid functions are
// commented out, to audit that they have intentionally been reviewed and
// rejected.  This allows us to compare the different test tables if a
// discrepancy occurs in the future.
//
// In particular, this case does not support construction from pointers to
// 'const' objects.

static const TestPolicy<MyTestObject> TEST_POLICY_BASE_ARRAY[] = {
    // default test
    TestPolicy<MyTestObject>(),

    // single object-pointer tests
    TestPolicy<MyTestObject>( NullPolicy() ),
    TestPolicy<MyTestObject>( NullPolicy(), NullPolicy() ),
    TestPolicy<MyTestObject>( NullPolicy(), NullPolicy(), NullPolicy() ),

    TestPolicy<MyTestObject>( Obase() ),
    TestPolicy<MyTestObject>( Oderiv() ),
    //TestPolicy<MyTestObject>( OCbase() ),
    //TestPolicy<MyTestObject>( OCderiv() ),

    // factory tests

    TestPolicy<MyTestObject>( NullPolicy(), Ftst() ),
    TestPolicy<MyTestObject>( NullPolicy(), Fbsl() ),
    TestPolicy<MyTestObject>( Obase(),      Ftst() ),
    TestPolicy<MyTestObject>( Obase(),      Fbsl() ),
    TestPolicy<MyTestObject>( Oderiv(),     Ftst() ),
    TestPolicy<MyTestObject>( Oderiv(),     Fbsl() ),
    //TestPolicy<MyTestObject>( OCbase(),   Ftst() ),
    //TestPolicy<MyTestObject>( OCbase(),   Fbsl() ),
    //TestPolicy<MyTestObject>( OCderiv(),  Ftst() ),
    //TestPolicy<MyTestObject>( OCderiv(),  Fbsl() ),
    // deleter tests

    // First test the non-deprecated interface, using the policy 'DVoidVoid'.

    // MyTestObject
    TestPolicy<MyTestObject>( Obase(), Ftst(), DVoidVoid< Obase,   Ftst >() ),
    TestPolicy<MyTestObject>( Obase(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<MyTestObject>( Obase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyTestObject
    //TestPolicy<MyTestObject>( OCbase(), Ftst(), DVoidVoid< OCbase, Ftst >() ),
    //TestPolicy<MyTestObject>( OCbase(), Fbsl(), DVoidVoid< OCbase, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCbase(), Ftst(), DVoidVoid< OCbase, Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<MyTestObject>( Obase(),  Fdflt(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<MyTestObject>( Obase(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that type-erase the
    // 'object' type, but have a strongly typed 'factory' argument.  Such
    // deleters are generated by the 'DVoidFac' policy..

    // MyTestObject
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DVoidFac< Obase,   Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyTestObject
    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),
    //TestPolicy<MyTestObject>( OCbase(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Ftst >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Ftst >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DVoidFac<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyTestObject>( Obase(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<MyTestObject>( Obase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<Oderiv,  Fdflt>() ),
    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),

    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT SUPPORTED FOR
    // TYPE-ERASURE THROUGH DELETER
    //TestPolicy<MyTestObject>( OCbase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // Now we test deleters that are strongly typed for the 'object' parameter,
    // but type-erase the 'factory'.

    // MyTestObject
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjVoid< Obase,   Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyTestObject
    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),
    //TestPolicy<MyTestObject>( OCbase(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Ftst >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Ftst >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<MyTestObject>( Obase(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    // Finally we test the most generic combination of generic object type, a
    // factory, and a deleter taking two arguments compatible with pointers to
    // the invoking 'object' and 'factory' types.

    // MyTestObject
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjFac< Obase,   Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Obase(),   Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyTestObject
    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Ftst >() ),
    //TestPolicy<MyTestObject>( OCbase(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Ftst >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Ftst >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DObjFac<Obase,   Fdflt>() ),
    TestPolicy<MyTestObject>( Obase(),   Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjFac<Oderiv,  Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjFac<Obase,   Fdflt>() ),

    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyTestObject>( Obase(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<MyTestObject>( Obase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCbase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjFac<Oderiv,  Fdflt>() ),
    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),

    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( Oderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyTestObject>( OCderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    TestPolicy<MyTestObject>( Obase(),   Ftst(), NullPolicy() ),
    TestPolicy<MyTestObject>( Obase(),   Fbsl(), NullPolicy() ),
    TestPolicy<MyTestObject>( Oderiv(),  Ftst(), NullPolicy() ),
    TestPolicy<MyTestObject>( Oderiv(),  Fbsl(), NullPolicy() ),
    //TestPolicy<MyTestObject>( OCbase(),  Ftst(), NullPolicy() ),
    //TestPolicy<MyTestObject>( OCbase(),  Fbsl(), NullPolicy() ),
    //TestPolicy<MyTestObject>( OCderiv(), Ftst(), NullPolicy() ),
    //TestPolicy<MyTestObject>( OCderiv(), Fbsl(), NullPolicy() )
};

//-----------------------------------------------------------------------------
// This is the test table for iterating constructor and load functions for
// 'bslma::ManagedPtr<MyTestObject>'.  The same test table is created for each
// of the main 5 tested pointer types, and then the invalid functions are
// commented out, to audit that they have intentionally been reviewed and
// rejected.  This allows us to compare the different test tables if a
// discrepancy occurs in the future.
static const TestPolicy<const MyTestObject> TEST_POLICY_CONST_BASE_ARRAY[] = {
    // default test
    TestPolicy<const MyTestObject>(),

    // single object-pointer tests
    TestPolicy<const MyTestObject>( NullPolicy() ),

    TestPolicy<const MyTestObject>( Obase() ),
    TestPolicy<const MyTestObject>( Oderiv() ),
    TestPolicy<const MyTestObject>( OCbase() ),
    TestPolicy<const MyTestObject>( OCderiv() ),

    // factory tests
    TestPolicy<const MyTestObject>( NullPolicy(), NullPolicy() ),

    TestPolicy<const MyTestObject>( NullPolicy(), Ftst() ),
    TestPolicy<const MyTestObject>( NullPolicy(), Fbsl() ),
    TestPolicy<const MyTestObject>( Obase(),      Ftst() ),
    TestPolicy<const MyTestObject>( Obase(),      Fbsl() ),
    TestPolicy<const MyTestObject>( Oderiv(),     Ftst() ),
    TestPolicy<const MyTestObject>( Oderiv(),     Fbsl() ),
    TestPolicy<const MyTestObject>( OCbase(),     Ftst() ),
    TestPolicy<const MyTestObject>( OCbase(),     Fbsl() ),
    TestPolicy<const MyTestObject>( OCderiv(),    Ftst() ),
    TestPolicy<const MyTestObject>( OCderiv(),    Fbsl() ),

    // deleter tests
    TestPolicy<const MyTestObject>( NullPolicy(), NullPolicy(), NullPolicy() ),

    // First test the non-deprecated interface, using the policy 'DVoidVoid'.

    // MyTestObject
    TestPolicy<const MyTestObject>( Obase(), Ftst(), DVoidVoid< Obase,   Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyTestObject
    TestPolicy<const MyTestObject>( OCbase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( OCbase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCbase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const MyTestObject>( Obase(),  Fdflt(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that type-erase the
    // 'object' type, but have a strongly typed 'factory' argument.  Such
    // deleters are generated by the 'DVoidFac' policy..

    // MyTestObject
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DVoidFac< Obase,   Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyTestObject
    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( OCbase(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DVoidFac<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<Oderiv,  Fdflt>() ),
    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),

    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT SUPPORTED FOR
    // TYPE-ERASURE THROUGH DELETER
    //TestPolicy<const MyTestObject>( OCbase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // Now we test deleters that are strongly typed for the 'object' parameter,
    // but type-erase the 'factory'.

    // MyTestObject
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjVoid< Obase,   Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyTestObject
    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( OCbase(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    // Finally we test the most generic combination of generic object type, a
    // factory, and a deleter taking two arguments compatible with pointers to
    // the invoking 'object' and 'factory' types.

    // MyTestObject
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjFac< Obase,   Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Obase(),   Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyTestObject
    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<const MyTestObject>( OCbase(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DObjFac<Obase,   Fdflt>() ),
    TestPolicy<const MyTestObject>( Obase(),   Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCbase(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjFac<Oderiv,  Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjFac<Obase,   Fdflt>() ),

    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<const MyTestObject>( Obase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const MyTestObject>( OCbase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjFac<Oderiv,  Fdflt>() ),
    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),

    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<const MyTestObject>( Oderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<const MyTestObject>( OCderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    TestPolicy<const MyTestObject>( Obase(),   Ftst(), NullPolicy() ),
    TestPolicy<const MyTestObject>( Obase(),   Fbsl(), NullPolicy() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Ftst(), NullPolicy() ),
    TestPolicy<const MyTestObject>( Oderiv(),  Fbsl(), NullPolicy() ),
    TestPolicy<const MyTestObject>( OCbase(),  Ftst(), NullPolicy() ),
    TestPolicy<const MyTestObject>( OCbase(),  Fbsl(), NullPolicy() ),
    TestPolicy<const MyTestObject>( OCderiv(), Ftst(), NullPolicy() ),
    TestPolicy<const MyTestObject>( OCderiv(), Fbsl(), NullPolicy() )
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This is the test table for iterating constructor and load functions for
// 'bslma::ManagedPtr<MyTestObject>'.  The same test table is created for each
// of the main 5 tested pointer types, and then the invalid functions are
// commented out, to audit that they have intentionally been reviewed and
// rejected.  This allows us to compare the different test tables if a
// discrepancy occurs in the future.
//
// In particular, this case does not support construction from pointers to
// 'const' objects, or from pointers to base objects.
static const TestPolicy<MyDerivedObject> TEST_POLICY_DERIVED_ARRAY[] = {
    // default test
    TestPolicy<MyDerivedObject>(),

    // single object-pointer tests
    TestPolicy<MyDerivedObject>( NullPolicy() ),
    TestPolicy<MyDerivedObject>( NullPolicy(), NullPolicy(), NullPolicy() ),
    TestPolicy<MyDerivedObject>( NullPolicy(), NullPolicy() ),

    //TestPolicy<MyDerivedObject>( Obase() ),
    TestPolicy<MyDerivedObject>( Oderiv() ),
    //TestPolicy<MyDerivedObject>( OCbase() ),
    //TestPolicy<MyDerivedObject>( OCderiv() ),

    // factory tests

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Fbsl() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl() ),
    // deleter tests

    // First test the non-deprecated interface, using the policy 'DVoidVoid'.

    // MyDerivedObject
    //TestPolicy<MyDerivedObject>( Obase(), Ftst(), DVoidVoid< Obase,   Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    //TestPolicy<MyDerivedObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCbase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCbase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCbase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Fbsl(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    //TestPolicy<MyDerivedObject>( Obase(),  Fdflt(), DVoidVoid<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Fdflt(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Fdflt(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that type-erase the
    // 'object' type, but have a strongly typed 'factory' argument.  Such
    // deleters are generated by the 'DVoidFac' policy..

    // MyDerivedObject
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DVoidFac< Obase,   Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Ftst >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DVoidFac<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DVoidFac<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidFac<Oderiv,  Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),

    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT SUPPORTED FOR
    // TYPE-ERASURE THROUGH DELETER
    //TestPolicy<MyDerivedObject>( OCbase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // Now we test deleters that are strongly typed for the 'object' parameter,
    // but type-erase the 'factory'.

    // MyDerivedObject
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjVoid< Obase,   Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Ftst >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DObjVoid<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),


    // Finally we test the most generic combination of generic object type, a
    // factory, and a deleter taking two arguments compatible with pointers to
    // the invoking 'object' and 'factory' types.

    // MyDerivedObject
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjFac< Obase,   Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DObjFac< Obase,   Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjFac< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Ftst >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Ftst >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), DObjFac< OCbase,  Fbsl >() ),


    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjFac<Oderiv,  Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjFac<Obase,   Fdflt>() ),

    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Obase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCbase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjFac<Oderiv,  Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),

    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( Oderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    //TestPolicy<MyDerivedObject>( Obase(),   Ftst(), NullPolicy() ),
    //TestPolicy<MyDerivedObject>( Obase(),   Fbsl(), NullPolicy() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Ftst(), NullPolicy() ),
    TestPolicy<MyDerivedObject>( Oderiv(),  Fbsl(), NullPolicy() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Ftst(), NullPolicy() ),
    //TestPolicy<MyDerivedObject>( OCbase(),  Fbsl(), NullPolicy() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Ftst(), NullPolicy() ),
    //TestPolicy<MyDerivedObject>( OCderiv(), Fbsl(), NullPolicy() )
};

//=============================================================================
// This is the test table for iterating constructor and load functions for
// 'bslma::ManagedPtr<MyTestObject>'.  The same test table is created for each
// of the main 5 tested pointer types, and then the invalid functions are
// commented out, to audit that they have intentionally been reviewed and
// rejected.  This allows us to compare the different test tables if a
// discrepancy occurs in the future.
//
// In particular, this case does not support construction from pointers to
// 'const' objects.
static const TestPolicy<void> TEST_POLICY_VOID_ARRAY[] = {
    // default test
    TestPolicy<void>(),

    // single object-pointer tests
    TestPolicy<void>( NullPolicy() ),

    TestPolicy<void>( Obase() ),
    TestPolicy<void>( Oderiv() ),
    //TestPolicy<void>( OCbase() ),
    //TestPolicy<void>( OCderiv() ),

    TestPolicy<void>( Ob1() ),
    TestPolicy<void>( Ob2() ),
    TestPolicy<void>( Ocomp() ),

    // factory tests
    TestPolicy<void>( NullPolicy(), NullPolicy() ),

    TestPolicy<void>( Obase(),   Ftst() ),
    TestPolicy<void>( Obase(),   Fbsl() ),
    TestPolicy<void>( Oderiv(),  Ftst() ),
    TestPolicy<void>( Oderiv(),  Fbsl() ),
    //TestPolicy<void>( OCbase(),  Ftst() ),
    //TestPolicy<void>( OCbase(),  Fbsl() ),
    //TestPolicy<void>( OCderiv(), Ftst() ),
    //TestPolicy<void>( OCderiv(), Fbsl() ),

    TestPolicy<void>( Ob1(),   Ftst() ),
    TestPolicy<void>( Ob1(),   Fbsl() ),
    TestPolicy<void>( Ob2(),   Ftst() ),
    TestPolicy<void>( Ob2(),   Fbsl() ),
    TestPolicy<void>( Ocomp(), Ftst() ),
    TestPolicy<void>( Ocomp(), Fbsl() ),

    TestPolicy<void>( Ocomp(), Ftst(), DVoidVoid< Ocomp, Ftst >() ),
    TestPolicy<void>( Ocomp(), Fbsl(), DVoidVoid< Ocomp, Fbsl >() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    TestPolicy<void>( Ocomp(), Ftst(), DObjFac< Ocomp,   Ftst >() ),
    TestPolicy<void>( Ocomp(), Fbsl(), DObjFac< Ocomp,   Fbsl >() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // deleter tests
    TestPolicy<void>( NullPolicy(), NullPolicy(), NullPolicy() ),

    // First test the non-deprecated interface, using the policy 'DVoidVoid'.

    // void
    TestPolicy<void>( Obase(), Ftst(), DVoidVoid< Obase,   Ftst >() ),
    TestPolicy<void>( Obase(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<void>( Obase(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Obase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<void>( Obase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<void>( Obase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const void
    //TestPolicy<void>( OCbase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    //TestPolicy<void>( OCbase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCbase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Ftst >() ),
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< Obase,   Ftst >() ),

    TestPolicy<void>( Oderiv(), Fbsl(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<void>( Oderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    //TestPolicy<void>( OCderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<void>( Obase(),  Fdflt(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(), Fdflt(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(), Fdflt(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<void>( Obase(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(), NullPolicy(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that type-erase the
    // 'object' type, but have a strongly typed 'factory' argument.  Such
    // deleters are generated by the 'DVoidFac' policy..

    // void
    TestPolicy<void>( Obase(),   Ftst(), DVoidFac< Obase,   Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Obase(),   Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const void
    //TestPolicy<void>( OCbase(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),
    //TestPolicy<void>( OCbase(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCbase(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< Obase,   Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidFac< OCderiv, Ftst >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidFac< OCbase,  Ftst >() ),

    //TestPolicy<void>( OCderiv(), Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCderiv(), Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<void>( Obase(),   Fdflt(), DVoidFac<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(),   Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DVoidFac<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DVoidFac<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<void>( Obase(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<void>( Obase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( Oderiv(), NullPolicy(), DVoidFac<Oderiv,  Fdflt>() ),
    //TestPolicy<void>( Oderiv(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),

    //TestPolicy<void>( Oderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( Oderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT SUPPORTED FOR
    // TYPE-ERASURE THROUGH DELETER
    //TestPolicy<void>( OCbase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // Now we test deleters that are strongly typed for the 'object' parameter,
    // but type-erase the 'factory'.

    // void
    TestPolicy<void>( Obase(),   Ftst(), DObjVoid< Obase,   Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Obase(),   Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const void
    //TestPolicy<void>( OCbase(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),
    //TestPolicy<void>( OCbase(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCbase(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< Obase,   Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<void>( OCderiv(), Ftst(), DObjVoid< OCderiv, Ftst >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DObjVoid< OCbase,  Ftst >() ),

    //TestPolicy<void>( OCderiv(), Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCderiv(), Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<void>( Obase(),   Fdflt(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(),   Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<void>( Obase(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(), NullPolicy(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    // Finally we test the most generic combination of generic object type, a
    // factory, and a deleter taking two arguments compatible with pointers to
    // the invoking 'object' and 'factory' types.

    // void
    TestPolicy<void>( Obase(),   Ftst(), DObjFac< Obase,   Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Obase(),   Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<void>( Obase(),   Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<void>( Obase(),   Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const void
    //TestPolicy<void>( OCbase(),  Ftst(), DObjFac< OCbase,  Ftst >() ),
    //TestPolicy<void>( OCbase(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCbase(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< Obase,   Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<void>( Oderiv(),  Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<void>( Oderiv(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    //TestPolicy<void>( OCderiv(), Ftst(), DObjFac< OCderiv, Ftst >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DObjFac< OCbase,  Ftst >() ),

    //TestPolicy<void>( OCderiv(), Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    //TestPolicy<void>( OCderiv(), Ftst(), DObjFac< OCderiv, Fbsl >() ),
    //TestPolicy<void>( OCderiv(), Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<void>( Obase(),   Fdflt(), DObjFac<Obase,   Fdflt>() ),
    TestPolicy<void>( Obase(),   Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DObjFac<Oderiv,  Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DObjFac<Obase,   Fdflt>() ),

    TestPolicy<void>( Oderiv(),  Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<void>( Oderiv(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<void>( Obase(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<void>( Obase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCbase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( Oderiv(), NullPolicy(), DObjFac<Oderiv,  Fdflt>() ),
    //TestPolicy<void>( Oderiv(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),

    //TestPolicy<void>( Oderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( Oderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<void>( OCderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<void>( OCderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    TestPolicy<void>( Obase(),   Ftst(), NullPolicy() ),
    TestPolicy<void>( Obase(),   Fbsl(), NullPolicy() ),
    TestPolicy<void>( Oderiv(),  Ftst(), NullPolicy() ),
    TestPolicy<void>( Oderiv(),  Fbsl(), NullPolicy() ),
    //TestPolicy<void>( OCbase(),  Ftst(), NullPolicy() ),
    //TestPolicy<void>( OCbase(),  Fbsl(), NullPolicy() ),
    //TestPolicy<void>( OCderiv(), Ftst(), NullPolicy() ),
    //TestPolicy<void>( OCderiv(), Fbsl(), NullPolicy() )
};

//-----------------------------------------------------------------------------
// This is the test table for iterating constructor and load functions for
// 'bslma::ManagedPtr<MyTestObject>'.  The same test table is created for each
// of the main 5 tested pointer types, and then the invalid functions are
// commented out, to audit that they have intentionally been reviewed and
// rejected.  This allows us to compare the different test tables if a
// discrepancy occurs in the future.
static const TestPolicy<const void> TEST_POLICY_CONST_VOID_ARRAY[] = {
    // default test
    TestPolicy<const void>(),

    // single object-pointer tests
    TestPolicy<const void>( NullPolicy() ),

    TestPolicy<const void>( Obase() ),
    TestPolicy<const void>( Oderiv() ),
    TestPolicy<const void>( OCbase() ),
    TestPolicy<const void>( OCderiv() ),

    TestPolicy<const void>( Ob1() ),
    TestPolicy<const void>( Ob2() ),
    TestPolicy<const void>( Ocomp() ),

    // factory tests
    TestPolicy<const void>( NullPolicy(), NullPolicy() ),

    TestPolicy<const void>( Obase(),   Ftst() ),
    TestPolicy<const void>( Obase(),   Fbsl() ),
    TestPolicy<const void>( Oderiv(),  Ftst() ),
    TestPolicy<const void>( Oderiv(),  Fbsl() ),
    TestPolicy<const void>( OCbase(),  Ftst() ),
    TestPolicy<const void>( OCbase(),  Fbsl() ),
    TestPolicy<const void>( OCderiv(), Ftst() ),
    TestPolicy<const void>( OCderiv(), Fbsl() ),

    TestPolicy<const void>( Ob1(),     Ftst() ),
    TestPolicy<const void>( Ob1(),     Fbsl() ),
    TestPolicy<const void>( Ob2(),     Ftst() ),
    TestPolicy<const void>( Ob2(),     Fbsl() ),
    TestPolicy<const void>( Ocomp(),   Ftst() ),
    TestPolicy<const void>( Ocomp(),   Fbsl() ),

    TestPolicy<const void>( Ocomp(), Ftst(), DVoidVoid< Ocomp,   Ftst >() ),
    TestPolicy<const void>( Ocomp(), Fbsl(), DVoidVoid< Ocomp,   Fbsl >() ),
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    TestPolicy<const void>( Ocomp(), Ftst(), DObjFac< Ocomp,   Ftst >() ),
    TestPolicy<const void>( Ocomp(), Fbsl(), DObjFac< Ocomp,   Fbsl >() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

#if 0
    TestPolicy<const void>( Ocomp(), Ftst(), DVoidVoid< Ob1,     Fbsl >() ),
    TestPolicy<const void>( Ocomp(), Ftst(), DVoidVoid< Ob2,     Fbsl >() ),
    TestPolicy<const void>( Ocomp(), Ftst(), DObjFac< Ob1,     Fbsl >() ),
    TestPolicy<const void>( Ocomp(), Ftst(), DObjFac< Ob2,     Fbsl >() ),
#endif

    // deleter tests
    TestPolicy<const void>( NullPolicy(), NullPolicy(), NullPolicy() ),

    // First test the non-deprecated interface, using the policy 'DVoidVoid'.

    // void
    TestPolicy<const void>( Obase(), Ftst(), DVoidVoid< Obase,   Ftst >() ),
    TestPolicy<const void>( Obase(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<const void>( Obase(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Obase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<const void>( Obase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Obase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const void
    TestPolicy<const void>( OCbase(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),
    TestPolicy<const void>( OCbase(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCbase(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Ftst >() ),
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< Obase,   Ftst >() ),

    TestPolicy<const void>( Oderiv(), Fbsl(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(), Fbsl(), DVoidVoid< Obase,   Fbsl >() ),

    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<const void>( Oderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Ftst >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Ftst >() ),

    TestPolicy<const void>( OCderiv(), Fbsl(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Fbsl(), DVoidVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCderiv(), Ftst(), DVoidVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const void>( Obase(),  Fdflt(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(),  Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(), Fdflt(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(), Fdflt(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), Fdflt(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), Fdflt(), DVoidVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<const void>( Obase(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidVoid<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), NullPolicy(), DVoidVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), NullPolicy(), DVoidVoid<OCbase,  Fdflt>() ),

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that type-erase the
    // 'object' type, but have a strongly typed 'factory' argument.  Such
    // deleters are generated by the 'DVoidFac' policy..

    // void
    TestPolicy<const void>( Obase(),   Ftst(), DVoidFac< Obase,   Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Obase(),   Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const void
    TestPolicy<const void>( OCbase(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),
    TestPolicy<const void>( OCbase(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCbase(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< Obase,   Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DVoidFac< Obase,   Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidFac< OCderiv, Ftst >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidFac< OCbase,  Ftst >() ),

    TestPolicy<const void>( OCderiv(), Fbsl(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Fbsl(), DVoidFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCderiv(), Ftst(), DVoidFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DVoidFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const void>( Obase(),   Fdflt(), DVoidFac<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(),   Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DVoidFac<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DVoidFac<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), Fdflt(), DVoidFac<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), Fdflt(), DVoidFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<const void>( Obase(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),
    //TestPolicy<const void>( Obase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidFac<Oderiv,  Fdflt>() ),
    //TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidFac<Obase,   Fdflt>() ),

    //TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<const void>( Oderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT SUPPORTED FOR
    // TYPE-ERASURE THROUGH DELETER
    //TestPolicy<const void>( OCbase(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    //TestPolicy<const void>( OCderiv(), NullPolicy(), DVoidFac<OCderiv, Fdflt>() ),
    //TestPolicy<const void>( OCderiv(), NullPolicy(), DVoidFac<OCbase,  Fdflt>() ),

    // Now we test deleters that are strongly typed for the 'object' parameter,
    // but type-erase the 'factory'.

    // void
    TestPolicy<const void>( Obase(),   Ftst(), DObjVoid< Obase,   Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Obase(),   Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const void
    TestPolicy<const void>( OCbase(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),
    TestPolicy<const void>( OCbase(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCbase(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< Obase,   Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjVoid< Obase,   Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const void>( OCderiv(), Ftst(), DObjVoid< OCderiv, Ftst >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DObjVoid< OCbase,  Ftst >() ),

    TestPolicy<const void>( OCderiv(), Fbsl(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Fbsl(), DObjVoid< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCderiv(), Ftst(), DObjVoid< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DObjVoid< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const void>( Obase(),   Fdflt(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(),   Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), Fdflt(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), Fdflt(), DObjVoid<OCbase,  Fdflt>() ),

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    TestPolicy<const void>( Obase(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(), NullPolicy(), DObjVoid<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(), NullPolicy(), DObjVoid<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), NullPolicy(), DObjVoid<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), NullPolicy(), DObjVoid<OCbase,  Fdflt>() ),

    // Finally we test the most generic combination of generic object type, a
    // factory, and a deleter taking two arguments compatible with pointers to
    // the invoking 'object' and 'factory' types.

    // void
    TestPolicy<const void>( Obase(),   Ftst(), DObjFac< Obase,   Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Obase(),   Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<const void>( Obase(),   Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Obase(),   Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const void
    TestPolicy<const void>( OCbase(),  Ftst(), DObjFac< OCbase,  Ftst >() ),
    TestPolicy<const void>( OCbase(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCbase(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // MyDerivedObject
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< Obase,   Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjFac< Obase,   Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< Oderiv,  Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< Obase,   Fbsl >() ),

    // ... plus safe const-conversions
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // const MyDerivedObject
    TestPolicy<const void>( OCderiv(), Ftst(), DObjFac< OCderiv, Ftst >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DObjFac< OCbase,  Ftst >() ),

    TestPolicy<const void>( OCderiv(), Fbsl(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Fbsl(), DObjFac< OCbase,  Fbsl >() ),

    TestPolicy<const void>( OCderiv(), Ftst(), DObjFac< OCderiv, Fbsl >() ),
    TestPolicy<const void>( OCderiv(), Ftst(), DObjFac< OCbase,  Fbsl >() ),

    // Also test a deleter that does not use the 'factory' argument.  These
    // tests must also validate passing a null pointer lvalue as the 'factory'
    // argument.
    TestPolicy<const void>( Obase(),   Fdflt(), DObjFac<Obase,   Fdflt>() ),
    TestPolicy<const void>( Obase(),   Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCbase(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjFac<Oderiv,  Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjFac<Obase,   Fdflt>() ),

    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<const void>( Oderiv(),  Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    TestPolicy<const void>( OCderiv(), Fdflt(), DObjFac<OCderiv, Fdflt>() ),
    TestPolicy<const void>( OCderiv(), Fdflt(), DObjFac<OCbase,  Fdflt>() ),

    // DESIGN NOTE: NULL POINTER LITERALS CAN BE USED ONLY WITH DELETERS THAT
    // TYPE-ERASE THE FACTORY.

    // Also, verify null pointer literal can be used for the factory argument
    // in each case.
    //TestPolicy<const void>( Obase(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),
    //TestPolicy<const void>( Obase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const void>( OCbase(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const void>( Oderiv(), NullPolicy(), DObjFac<Oderiv,  Fdflt>() ),
    //TestPolicy<const void>( Oderiv(), NullPolicy(), DObjFac<Obase,   Fdflt>() ),

    //TestPolicy<const void>( Oderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<const void>( Oderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),

    //TestPolicy<const void>( OCderiv(), NullPolicy(), DObjFac<OCderiv, Fdflt>() ),
    //TestPolicy<const void>( OCderiv(), NullPolicy(), DObjFac<OCbase,  Fdflt>() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    TestPolicy<const void>( Obase(),   Ftst(), NullPolicy() ),
    TestPolicy<const void>( Obase(),   Fbsl(), NullPolicy() ),
    TestPolicy<const void>( Oderiv(),  Ftst(), NullPolicy() ),
    TestPolicy<const void>( Oderiv(),  Fbsl(), NullPolicy() ),
    TestPolicy<const void>( OCbase(),  Ftst(), NullPolicy() ),
    TestPolicy<const void>( OCbase(),  Fbsl(), NullPolicy() ),
    TestPolicy<const void>( OCderiv(), Ftst(), NullPolicy() ),
    TestPolicy<const void>( OCderiv(), Fbsl(), NullPolicy() )
};

// BDE_VERIFY pragma: pop  // end of test tables

//=============================================================================
// Here we add additional test cases for the deliberately awkward 'composite'
// case, which does not use a virtual destructor.  Note that we cannot test
// this on the couple of platforms where we get the wrong answer from the
// 'bslmf::IsPolymorphic' type trait, as the workaround makes the 'Base' class
// ambiguous.
#if defined(BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE)
static const TestPolicy<Base> TEST_POLICY_BASE0_ARRAY[] = {
    // default test
    TestPolicy<Base>(),

    // single object-pointer tests
    TestPolicy<Base>( NullPolicy() ),

    TestPolicy<Base>( Ob1() ),
    TestPolicy<Base>( Ob2() ),
    TestPolicy<Base>( Ocomp() ),

    // factory tests
    TestPolicy<Base>( NullPolicy(), NullPolicy() ),

    TestPolicy<Base>( Ob1(),   Ftst() ),
    TestPolicy<Base>( Ob1(),   Fbsl() ),
    TestPolicy<Base>( Ob2(),   Ftst() ),
    TestPolicy<Base>( Ob2(),   Fbsl() ),
    TestPolicy<Base>( Ocomp(), Ftst() ),
    TestPolicy<Base>( Ocomp(), Fbsl() ),

    // deleter tests
    TestPolicy<Base>( Ocomp(), Ftst(), DVoidVoid< Ocomp,   Ftst >() ),
    TestPolicy<Base>( Ocomp(), Fbsl(), DVoidVoid< Ocomp,   Fbsl >() ),
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    TestPolicy<Base>( Ocomp(), Ftst(), DObjFac< Ocomp,   Ftst >() ),
    TestPolicy<Base>( Ocomp(), Fbsl(), DObjFac< Ocomp,   Fbsl >() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED
};
#endif

// This is the important test case for composites, where 'Base2' is the
// *second* base class from 'Composite', which uses multiple inheritance.  Note
// that this test is equally important for platforms where
// 'bslmf::IsPolymorphic' is giving the wrong answer - that is why we have a
// second workaround version of 'Composite' available for this test.
static const TestPolicy<Base2> TEST_POLICY_BASE2_ARRAY[] = {
    // default test
    TestPolicy<Base2>(),

    // single object-pointer tests
    TestPolicy<Base2>( NullPolicy() ),

    TestPolicy<Base2>( Ob2() ),
    TestPolicy<Base2>( Ocomp() ),

    // factory tests
    TestPolicy<Base2>( NullPolicy(), NullPolicy() ),

    TestPolicy<Base2>( Ob2(),   Ftst() ),
    TestPolicy<Base2>( Ob2(),   Fbsl() ),
    TestPolicy<Base2>( Ocomp(), Ftst() ),
    TestPolicy<Base2>( Ocomp(), Fbsl() ),

    // deleter tests
    TestPolicy<Base2>( Ocomp(), Ftst(), DVoidVoid< Ocomp,   Ftst >() ),
    TestPolicy<Base2>( Ocomp(), Fbsl(), DVoidVoid< Ocomp,   Fbsl >() ),
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    TestPolicy<Base2>( Ocomp(), Ftst(), DObjFac< Ocomp,   Ftst >() ),
    TestPolicy<Base2>( Ocomp(), Fbsl(), DObjFac< Ocomp,   Fbsl >() ),
#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

struct MoveState {
  public:
    // TYPES
    enum Enum {
        // Enumeration of move state.

        e_NOT_MOVED,  // The type was not involved in a move operation.

        e_MOVED,      // The type was involved in a move operation.

        e_UNKNOWN     // The type does not expose move-state information.
    };
};

                         // ============================
                         // class AllocArgumentType<int>
                         // ============================

template <int N>
class AllocArgumentType {
    // This class template declares a separate type for each template parameter
    // value 'N', 'AllocArgumentType<N>', that wraps an integer value
    // and provides implicit conversion to and from 'int'.  The wrapped integer
    // will be dynamically allocated using the supplied allocator, or the
    // default allocator if none is supplied.  Its main purpose is that having
    // separate types for testing enables distinguishing them when calling
    // through a function template interface, thereby avoiding ambiguities or
    // accidental switching of arguments in the implementation of in-place
    // constructors.  It further tests that allocators are propagated
    // correctly, or not, as required.

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil MoveUtil;

    // DATA
    bslma::Allocator *d_allocator_p;        // allocator used to supply memory
                                            // (held, not owned)

    int              *d_data_p;             // pointer to the data value

    MoveState::Enum   d_movedFrom;          // moved-from state

    MoveState::Enum   d_movedInto;          // moved-from state

  public:
    // CREATORS
    explicit AllocArgumentType(bslma::Allocator *basicAllocator =  0);
        // Create an 'AllocArgumentType' object having the (default) attribute
        // value -1.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Note that the default constructor does not
        // allocate memory.

    explicit AllocArgumentType(int               value,
                               bslma::Allocator *basicAllocator =  0);
        // Create an 'AllocArgumentType' object having the specified 'value'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used. The behavior is undefined unless 'value >= 0'.

    AllocArgumentType(const AllocArgumentType&  original,
                      bslma::Allocator         *basicAllocator = 0);
        // Create an 'AllocArgumentType' object having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that no memory is allocated if
        // 'original' refers to a default-constructed object.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType> original);
        // Create an 'AllocArgumentType' object having the same value as the
        // specified 'original'.  Note that no memory is allocated if
        // 'original' refers to a default-constructed object.

    AllocArgumentType(bslmf::MovableRef<AllocArgumentType>  original,
                      bslma::Allocator                     *basicAllocator);
        // Create an 'AllocArgumentType' object having the same value as the
        // specified 'original' using the specified 'basicAllocator' to supply
        // memory.  Note that no memory is allocated if 'original' refers to a
        // default-constructed object.

    ~AllocArgumentType();
        // Destroy this object.

    // MANIPULATORS
    AllocArgumentType& operator=(const AllocArgumentType& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    AllocArgumentType&
    operator=(BloombergLP::bslmf::MovableRef<AllocArgumentType> rhs);
        // Assign to this object the value of the specified 'rhs' object.  Note
        // that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    operator int() const;
        // Return the value of this test argument object.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.
};

// FREE FUNCTIONS
template <int N>
MoveState::Enum getMovedFrom(const AllocArgumentType<N>& object);
    // Return the move-from state of the specified 'object'.

template <int N>
MoveState::Enum getMovedInto(const AllocArgumentType<N>& object);
    // Return the move-into state of the specified 'object'.

                        // --------------------------
                        // class AllocArgumentType<N>
                        // --------------------------

// CREATORS
template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
    // Note that the default constructor does not allocate.  This is done to
    // correctly count allocations when not the whole set of arguments is
    // passed to emplacable test types.
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(int               value,
                                        bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
    BSLS_ASSERT_SAFE(value >= 0);

    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = value;
}


template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                                      const AllocArgumentType&  original,
                                      bslma::Allocator         *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_data_p(0)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
    if (original.d_data_p) {
        d_data_p = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        *d_data_p = int(original);
    }
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                                 bslmf::MovableRef<AllocArgumentType> original)
: d_allocator_p(MoveUtil::access(original).d_allocator_p)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_MOVED)
{
    AllocArgumentType& lvalue = original;

    d_data_p = lvalue.d_data_p;

    lvalue.d_data_p = 0;
    lvalue.d_movedInto = MoveState::e_NOT_MOVED;
    lvalue.d_movedFrom = MoveState::e_MOVED;
}

template <int N>
inline
AllocArgumentType<N>::AllocArgumentType(
                          bslmf::MovableRef<AllocArgumentType>  original,
                          bslma::Allocator                     *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_MOVED)
{
    AllocArgumentType& lvalue = original;

    if (d_allocator_p == lvalue.d_allocator_p) {
        d_data_p = lvalue.d_data_p;
        lvalue.d_data_p = 0;
    } else {
        if (lvalue.d_data_p) {
            d_data_p  = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            *d_data_p = int(lvalue);

            lvalue.d_allocator_p->deallocate(lvalue.d_data_p);
            lvalue.d_data_p = 0;
        } else {
            d_data_p = 0;
        }
    }

    lvalue.d_movedInto = MoveState::e_NOT_MOVED;
    lvalue.d_movedFrom = MoveState::e_MOVED;
}

template <int N>
inline
AllocArgumentType<N>::~AllocArgumentType()
{
    if (d_data_p) {
        d_allocator_p->deallocate(d_data_p);
    }
}

// MANIPULATORS
template <int N>
inline
AllocArgumentType<N>&
AllocArgumentType<N>::operator=(const AllocArgumentType& rhs)
{
    if (this != &rhs) {
        int *newValue = 0;
        if (rhs.d_data_p) {
            newValue = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            *newValue = int(rhs);
        }
        if (d_data_p) {
            d_allocator_p->deallocate(d_data_p);
        }
        d_data_p = newValue;

        d_movedInto = MoveState::e_NOT_MOVED;
        d_movedFrom = MoveState::e_NOT_MOVED;
    }
    return *this;
}

template <int N>
inline
AllocArgumentType<N>& AllocArgumentType<N>::operator=(
                         BloombergLP::bslmf::MovableRef<AllocArgumentType> rhs)
{
    AllocArgumentType& lvalue = rhs;

    if (this != &lvalue) {
        if (d_allocator_p == lvalue.d_allocator_p) {
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = lvalue.d_data_p;
            lvalue.d_data_p = 0;
        }
        else {
            int *newValue = 0;
            if (lvalue.d_data_p) {
                newValue = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
                *newValue = int(lvalue);
                *lvalue.d_data_p = -1;
            }
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = newValue;
        }

        d_movedInto = MoveState::e_MOVED;
        d_movedFrom = MoveState::e_NOT_MOVED;

        lvalue.d_movedInto = MoveState::e_NOT_MOVED;
        lvalue.d_movedFrom = MoveState::e_MOVED;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
AllocArgumentType<N>::operator int() const
{
    return d_data_p ? *d_data_p : -1;
}

template <int N>
inline
bslma::Allocator * AllocArgumentType<N>::allocator() const
{
    return d_allocator_p;
}

template <int N>
inline
MoveState::Enum AllocArgumentType<N>::movedFrom() const
{
    return d_movedFrom;
}

template <int N>
inline
MoveState::Enum AllocArgumentType<N>::movedInto() const
{
    return d_movedInto;
}

                                  // Aspects

// FREE FUNCTIONS
template <int N>
inline
MoveState::Enum getMovedFrom(const AllocArgumentType<N>& object)
{
    return object.movedFrom();
}

template <int N>
inline
MoveState::Enum getMovedInto(const AllocArgumentType<N>& object)
{
    return object.movedInto();
}

                         // =============================
                         // class AllocEmplacableTestType
                         // =============================

template <bool USES_BSLMA_ALLOC>
class AllocEmplacableTestType {
    // This class provides a test object used to check that the arguments
    // passed for creating an object with an in-place representation are of the
    // correct types and values.  Note that this class declares the
    // 'UsesBslmaAllocator' trait if the template parameter 'USES_BSLMA_ALLOC'
    // is 'true', and does not declare the trait otherwise.

  public:
    // TYPEDEFS
    typedef AllocArgumentType< 1> ArgType01;
    typedef AllocArgumentType< 2> ArgType02;
    typedef AllocArgumentType< 3> ArgType03;
    typedef AllocArgumentType< 4> ArgType04;
    typedef AllocArgumentType< 5> ArgType05;
    typedef AllocArgumentType< 6> ArgType06;
    typedef AllocArgumentType< 7> ArgType07;
    typedef AllocArgumentType< 8> ArgType08;
    typedef AllocArgumentType< 9> ArgType09;
    typedef AllocArgumentType<10> ArgType10;
    typedef AllocArgumentType<11> ArgType11;
    typedef AllocArgumentType<12> ArgType12;
    typedef AllocArgumentType<13> ArgType13;
    typedef AllocArgumentType<14> ArgType14;

    typedef bslmf::MovableRefUtil MoveUtil;

  private:
    // DATA
    ArgType01         d_arg01;        // value  1
    ArgType02         d_arg02;        // value  2
    ArgType03         d_arg03;        // value  3
    ArgType04         d_arg04;        // value  4
    ArgType05         d_arg05;        // value  5
    ArgType06         d_arg06;        // value  6
    ArgType07         d_arg07;        // value  7
    ArgType08         d_arg08;        // value  8
    ArgType09         d_arg09;        // value  9
    ArgType10         d_arg10;        // value 10
    ArgType11         d_arg11;        // value 11
    ArgType12         d_arg12;        // value 12
    ArgType13         d_arg13;        // value 13
    ArgType14         d_arg14;        // value 14
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    AllocEmplacableTestType(bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            ArgType13         arg13,
                            bslma::Allocator *basicAllocator = 0);
    AllocEmplacableTestType(ArgType01         arg01,
                            ArgType02         arg02,
                            ArgType03         arg03,
                            ArgType04         arg04,
                            ArgType05         arg05,
                            ArgType06         arg06,
                            ArgType07         arg07,
                            ArgType08         arg08,
                            ArgType09         arg09,
                            ArgType10         arg10,
                            ArgType11         arg11,
                            ArgType12         arg12,
                            ArgType13         arg13,
                            ArgType14         arg14,
                            bslma::Allocator *basicAllocator = 0);
        // Create an 'AllocEmplacableTestType' by initializing corresponding
        // attributes with the specified 'arg01'..'arg14', and initializing any
        // remaining attributes with their default value (-1).  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    AllocEmplacableTestType(
                           const AllocEmplacableTestType&  original,
                           bslma::Allocator               *basicAllocator = 0);
        // Create an allocating, in-place test object having the same value as
        // the specified 'original'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~AllocEmplacableTestType();
        // Destroy this object.

    // MANIPULATORS
    //! AllocEmplacableTestType& operator=(
    //                           const AllocEmplacableTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used to supply memory for this object.

    const ArgType01& arg01() const;
    const ArgType02& arg02() const;
    const ArgType03& arg03() const;
    const ArgType04& arg04() const;
    const ArgType05& arg05() const;
    const ArgType06& arg06() const;
    const ArgType07& arg07() const;
    const ArgType08& arg08() const;
    const ArgType09& arg09() const;
    const ArgType10& arg10() const;
    const ArgType11& arg11() const;
    const ArgType12& arg12() const;
    const ArgType13& arg13() const;
    const ArgType14& arg14() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.

    bool isEqual(const AllocEmplacableTestType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'AllocEmplacableTestType'
        // objects have the same value if each of their corresponding
        // attributes have the same value.
};

// FREE OPERATORS
template <bool USES_BSLMA_ALLOC>
bool operator==(const AllocEmplacableTestType<USES_BSLMA_ALLOC>& lhs,
                const AllocEmplacableTestType<USES_BSLMA_ALLOC>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocEmplacableTestType' objects
    // have the same value if each of their corresponding attributes have the
    // same value.

template <bool USES_BSLMA_ALLOC>
bool operator!=(const AllocEmplacableTestType<USES_BSLMA_ALLOC>& lhs,
                const AllocEmplacableTestType<USES_BSLMA_ALLOC>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocEmplacableTestType'
    // objects do not have the same value if any of their corresponding
    // attributes do not have the same value.

                        // -----------------------------
                        // class AllocEmplacableTestType
                        // -----------------------------

// CREATORS
template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              bslma::Allocator *basicAllocator)
: d_arg01(bslma::Default::allocator(basicAllocator))
, d_arg02(bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              ArgType10         arg10,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              ArgType10         arg10,
                                              ArgType11         arg11,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              ArgType10         arg10,
                                              ArgType11         arg11,
                                              ArgType12         arg12,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              ArgType10         arg10,
                                              ArgType11         arg11,
                                              ArgType12         arg12,
                                              ArgType13         arg13,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(MoveUtil::move(arg13), bslma::Default::allocator(basicAllocator))
, d_arg14(bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                              ArgType01         arg01,
                                              ArgType02         arg02,
                                              ArgType03         arg03,
                                              ArgType04         arg04,
                                              ArgType05         arg05,
                                              ArgType06         arg06,
                                              ArgType07         arg07,
                                              ArgType08         arg08,
                                              ArgType09         arg09,
                                              ArgType10         arg10,
                                              ArgType11         arg11,
                                              ArgType12         arg12,
                                              ArgType13         arg13,
                                              ArgType14         arg14,
                                              bslma::Allocator *basicAllocator)
: d_arg01(MoveUtil::move(arg01), bslma::Default::allocator(basicAllocator))
, d_arg02(MoveUtil::move(arg02), bslma::Default::allocator(basicAllocator))
, d_arg03(MoveUtil::move(arg03), bslma::Default::allocator(basicAllocator))
, d_arg04(MoveUtil::move(arg04), bslma::Default::allocator(basicAllocator))
, d_arg05(MoveUtil::move(arg05), bslma::Default::allocator(basicAllocator))
, d_arg06(MoveUtil::move(arg06), bslma::Default::allocator(basicAllocator))
, d_arg07(MoveUtil::move(arg07), bslma::Default::allocator(basicAllocator))
, d_arg08(MoveUtil::move(arg08), bslma::Default::allocator(basicAllocator))
, d_arg09(MoveUtil::move(arg09), bslma::Default::allocator(basicAllocator))
, d_arg10(MoveUtil::move(arg10), bslma::Default::allocator(basicAllocator))
, d_arg11(MoveUtil::move(arg11), bslma::Default::allocator(basicAllocator))
, d_arg12(MoveUtil::move(arg12), bslma::Default::allocator(basicAllocator))
, d_arg13(MoveUtil::move(arg13), bslma::Default::allocator(basicAllocator))
, d_arg14(MoveUtil::move(arg14), bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::AllocEmplacableTestType(
                                const AllocEmplacableTestType&  original,
                                bslma::Allocator               *basicAllocator)
: d_arg01(original.d_arg01, bslma::Default::allocator(basicAllocator))
, d_arg02(original.d_arg02, bslma::Default::allocator(basicAllocator))
, d_arg03(original.d_arg03, bslma::Default::allocator(basicAllocator))
, d_arg04(original.d_arg04, bslma::Default::allocator(basicAllocator))
, d_arg05(original.d_arg05, bslma::Default::allocator(basicAllocator))
, d_arg06(original.d_arg06, bslma::Default::allocator(basicAllocator))
, d_arg07(original.d_arg07, bslma::Default::allocator(basicAllocator))
, d_arg08(original.d_arg08, bslma::Default::allocator(basicAllocator))
, d_arg09(original.d_arg09, bslma::Default::allocator(basicAllocator))
, d_arg10(original.d_arg10, bslma::Default::allocator(basicAllocator))
, d_arg11(original.d_arg11, bslma::Default::allocator(basicAllocator))
, d_arg12(original.d_arg12, bslma::Default::allocator(basicAllocator))
, d_arg13(original.d_arg13, bslma::Default::allocator(basicAllocator))
, d_arg14(original.d_arg14, bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
}

template <bool USES_BSLMA_ALLOC>
AllocEmplacableTestType<USES_BSLMA_ALLOC>::~AllocEmplacableTestType()
{
    BSLS_ASSERT(d_arg01.allocator() == d_arg02.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg03.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg04.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg05.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg06.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg07.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg08.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg09.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg10.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg11.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg12.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg13.allocator());
    BSLS_ASSERT(d_arg01.allocator() == d_arg14.allocator());
}

// ACCESSORS
template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType01&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg01() const
{
    return d_arg01;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType02&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg02() const
{
    return d_arg02;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType03&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg03() const
{
    return d_arg03;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType04&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg04() const
{
    return d_arg04;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType05&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg05() const
{
    return d_arg05;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType06&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg06() const
{
    return d_arg06;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType07&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg07() const
{
    return d_arg07;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType08&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg08() const
{
    return d_arg08;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType09&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg09() const
{
    return d_arg09;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType10&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg10() const
{
    return d_arg10;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType11&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg11() const
{
    return d_arg11;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType12&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg12() const
{
    return d_arg12;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType13&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg13() const
{
    return d_arg13;
}

template <bool USES_BSLMA_ALLOC>
inline
const typename AllocEmplacableTestType<USES_BSLMA_ALLOC>::ArgType14&
AllocEmplacableTestType<USES_BSLMA_ALLOC>::arg14() const
{
    return d_arg14;
}

template <bool USES_BSLMA_ALLOC>
inline
bslma::Allocator *AllocEmplacableTestType<USES_BSLMA_ALLOC>::allocator() const
{
    return d_allocator_p;
}

template <bool USES_BSLMA_ALLOC>
inline
bool AllocEmplacableTestType<USES_BSLMA_ALLOC>::isEqual(
                    const AllocEmplacableTestType<USES_BSLMA_ALLOC>& rhs) const
{
    return d_arg01 == rhs.d_arg01
        && d_arg02 == rhs.d_arg02
        && d_arg03 == rhs.d_arg03
        && d_arg04 == rhs.d_arg04
        && d_arg05 == rhs.d_arg05
        && d_arg06 == rhs.d_arg06
        && d_arg07 == rhs.d_arg07
        && d_arg08 == rhs.d_arg08
        && d_arg09 == rhs.d_arg09
        && d_arg10 == rhs.d_arg10
        && d_arg11 == rhs.d_arg11
        && d_arg12 == rhs.d_arg12
        && d_arg13 == rhs.d_arg13
        && d_arg14 == rhs.d_arg14;
}

// FREE OPERATORS
template <bool USES_BSLMA_ALLOC>
inline
bool operator==(const AllocEmplacableTestType<USES_BSLMA_ALLOC>& lhs,
                const AllocEmplacableTestType<USES_BSLMA_ALLOC>& rhs)
{
    return lhs.isEqual(rhs);
}

template <bool USES_BSLMA_ALLOC>
inline
bool operator!=(const AllocEmplacableTestType<USES_BSLMA_ALLOC>& lhs,
                const AllocEmplacableTestType<USES_BSLMA_ALLOC>& rhs)
{
    return !lhs.isEqual(rhs);
}

} // close unnamed namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

// We need to test 'makeManaged' and 'allocateManaged' functions for classes
// that define 'bslma::UsesBslmaAllocator' trait and for those that do not.  To
// avoid code duplication test class is created as a template and different
// specializations have different trait values.

template <> struct UsesBslmaAllocator<AllocEmplacableTestType<true> > :
                                                                 bsl::true_type
{};

template <> struct UsesBslmaAllocator<AllocEmplacableTestType<false> > :
                                                                bsl::false_type
{};
}  // close namespace bslma
}  // close enterprise namespace

namespace {

                         // =====================
                         // class NoAllocTestType
                         // =====================

class NoAllocTestType {
    // This class provides a simple test object that does not use allocators.
    // It is used for testing 'makeManaged' and 'allocateManaged'.

  private:
    // DATA
    char  d_char;   // char  value
    int   d_int;    // int   value
    float d_float;  // float value

  public:
    // CREATORS
    NoAllocTestType();
    NoAllocTestType(char c);
    NoAllocTestType(char c, int i);
    NoAllocTestType(char c, int i, float f);
        // Create a 'NoAllocTestType' object by initializing corresponding
        // attributes with the specified 'c', 'i', and 'f' and initializing any
        // remaining attributes with their default value (0).

    NoAllocTestType(const NoAllocTestType& original);
        // Create an object having the same value as the specified 'original'.

    ~NoAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    //! NoAllocTestType& operator=(const NoAllocTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    char  getChar()  const;
    int   getInt()   const;
    float getFloat() const;
        // Return the value of the respective argument that was passed to the
        // constructor of this object.
};


                        // ---------------------
                        // class NoAllocTestType
                        // ---------------------

// CREATORS
NoAllocTestType::NoAllocTestType()
: d_char(0)
, d_int(0)
, d_float(0)
{
}

NoAllocTestType::NoAllocTestType(char c)
: d_char(c)
, d_int(0)
, d_float(0)
{
}

NoAllocTestType::NoAllocTestType(char c, int i)
: d_char(c)
, d_int(i)
, d_float(0)
{
}

NoAllocTestType::NoAllocTestType(char c, int i, float f)
: d_char(c)
, d_int(i)
, d_float(f)
{
}

NoAllocTestType::~NoAllocTestType()
{
}

// ACCESSORS
inline
char NoAllocTestType::getChar() const
{
    return d_char;
}

inline
int NoAllocTestType::getInt() const
{
    return d_int;
}

inline
float NoAllocTestType::getFloat() const
{
    return d_float;
}

                         // =======================
                         // class AllocOnlyTestType
                         // =======================

class AllocOnlyTestType {
    // This class provides a simple test object that accepts only allocator
    // pointers in its constructor.  It is used for testing 'makeManaged' and
    // 'allocateManaged'.

  private:
    // DATA
    bslma::Allocator *d_alloc1_p;  // first  value (held, not owned)
    bslma::Allocator *d_alloc2_p;  // second value (held, not owned)
    bslma::Allocator *d_alloc3_p;  // third  value (held, not owned)

  public:
    // CREATORS
    AllocOnlyTestType();
    AllocOnlyTestType(bslma::Allocator *alloc1);
    AllocOnlyTestType(bslma::Allocator *alloc1, bslma::Allocator *alloc2);
    AllocOnlyTestType(bslma::Allocator *alloc1,
                      bslma::Allocator *alloc2,
                      bslma::Allocator *alloc3);
        // Create an 'AllocOnlyTestType' by initializing corresponding
        // attributes with the specified 'alloc1', 'alloc2', 'alloc3', and
        // initializing any remaining attributes with their default value (0).

    AllocOnlyTestType(const AllocOnlyTestType& original);
        // Create an object having the same value as the specified 'original'.

    ~AllocOnlyTestType();
        // Destroy this object.

    // MANIPULATORS
    //! AllocOnlyTestType& operator=(const AllocOnlyTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    bslma::Allocator *alloc1() const;
    bslma::Allocator *alloc2() const;
    bslma::Allocator *alloc3() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.
};

                        // -----------------------
                        // class AllocOnlyTestType
                        // -----------------------

// CREATORS
AllocOnlyTestType::AllocOnlyTestType()
: d_alloc1_p(0)
, d_alloc2_p(0)
, d_alloc3_p(0)
{
}

AllocOnlyTestType::AllocOnlyTestType(bslma::Allocator *alloc1)
: d_alloc1_p(alloc1)
, d_alloc2_p(0)
, d_alloc3_p(0)
{
}

AllocOnlyTestType::AllocOnlyTestType(bslma::Allocator *alloc1,
                                     bslma::Allocator *alloc2)
: d_alloc1_p(alloc1)
, d_alloc2_p(alloc2)
, d_alloc3_p(0)
{
}

AllocOnlyTestType::AllocOnlyTestType(bslma::Allocator *alloc1,
                                     bslma::Allocator *alloc2,
                                     bslma::Allocator *alloc3)
: d_alloc1_p(alloc1)
, d_alloc2_p(alloc2)
, d_alloc3_p(alloc3)
{
}

AllocOnlyTestType::~AllocOnlyTestType()
{
}

// ACCESSORS
inline
bslma::Allocator *AllocOnlyTestType::alloc1() const
{
    return d_alloc1_p;
}

inline
bslma::Allocator *AllocOnlyTestType::alloc2() const
{
    return d_alloc2_p;
}

inline
bslma::Allocator *AllocOnlyTestType::alloc3() const
{
    return d_alloc3_p;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

struct Harness {
    // This 'struct' provides a namespace for utility functions that test
    // 'makeManaged' and 'allocateManaged'.

    // TYPES
    typedef AllocArgumentType< 1> Arg01;
    typedef AllocArgumentType< 2> Arg02;
    typedef AllocArgumentType< 3> Arg03;
    typedef AllocArgumentType< 4> Arg04;
    typedef AllocArgumentType< 5> Arg05;
    typedef AllocArgumentType< 6> Arg06;
    typedef AllocArgumentType< 7> Arg07;
    typedef AllocArgumentType< 8> Arg08;
    typedef AllocArgumentType< 9> Arg09;
    typedef AllocArgumentType<10> Arg10;
    typedef AllocArgumentType<11> Arg11;
    typedef AllocArgumentType<12> Arg12;
    typedef AllocArgumentType<13> Arg13;
    typedef AllocArgumentType<14> Arg14;

    typedef bslmf::MovableRefUtil          MoveUtil;
    typedef AllocEmplacableTestType<true>  UsingType;
    typedef AllocEmplacableTestType<false> NotUsingType;

    // CLASS METHODS
    template <class TYPE, int N_ARGS,
              int N01, int N02, int N03, int N04, int N05, int N06, int N07,
              int N08, int N09, int N10, int N11, int N12, int N13, int N14>
    static void prepareObject(TYPE   *target,
                              Arg01&  A01,
                              Arg02&  A02,
                              Arg03&  A03,
                              Arg04&  A04,
                              Arg05&  A05,
                              Arg06&  A06,
                              Arg07&  A07,
                              Arg08&  A08,
                              Arg09&  A09,
                              Arg10&  A10,
                              Arg11&  A11,
                              Arg12&  A12,
                              Arg13&  A13,
                              Arg14&  A14);
        // Prepare control object to compare with the results of 'makeManaged'
        // and 'allocateManaged'.

    template <class TYPE, int N_ARGS,
              int N01, int N02, int N03, int N04, int N05, int N06, int N07,
              int N08, int N09, int N10, int N11, int N12, int N13, int N14>
    static void testCase18_ConcreteTypeRun();
        // Verify that 'makeManaged' and 'allocateManaged' pass arguments to
        // the (template parameter) TYPE constructor correctly.

    template <int N_ARGS, int N01, int N02, int N03, int N04, int N05, int N06,
              int N07,    int N08, int N09, int N10, int N11, int N12, int N13,
              int N14>
    static void testCase18_ArgumentsTest();
        // Verify that 'makeManaged' and 'allocateManaged' pass arguments
        // to the constructor correctly.

    static void testCase18_NoAllocTestSingleCheck(int   numParameters,
                                                  char  c,
                                                  int   i,
                                                  float f);
        // Verify the behavior of 'makeManaged' and 'allocateManaged' accepting
        // the specified 'numParameters' (the specified 'c', 'i', and 'f'
        // accordingly) for the type that does not accept an allocator.

    static void testCase18_NoAllocTest();
        // Verify the behavior of 'makeManaged' and 'allocateManaged' for the
        // type that does not accept an allocator.

    static void testCase18_AllocOnlyTest();
        // Verify the behavior of 'makeManaged' for the type whose constructors
        // accept only allocator pointers.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type);
        // Return the specified 't' moved.

    template <class T>
    static const T&             testArg(T& t, bsl::false_type);
        // Return a reference providing non-modifiable access to the specified
        // 't'.
};

static bslma::TestAllocator g_argAlloc("test arguments allocator",
                                       g_veryVeryVeryVerbose);

static const Harness::Arg01 VA01(1,    &g_argAlloc);
static const Harness::Arg02 VA02(20,   &g_argAlloc);
static const Harness::Arg03 VA03(23,   &g_argAlloc);
static const Harness::Arg04 VA04(44,   &g_argAlloc);
static const Harness::Arg05 VA05(66,   &g_argAlloc);
static const Harness::Arg06 VA06(176,  &g_argAlloc);
static const Harness::Arg07 VA07(878,  &g_argAlloc);
static const Harness::Arg08 VA08(8,    &g_argAlloc);
static const Harness::Arg09 VA09(912,  &g_argAlloc);
static const Harness::Arg10 VA10(102,  &g_argAlloc);
static const Harness::Arg11 VA11(111,  &g_argAlloc);
static const Harness::Arg12 VA12(333,  &g_argAlloc);
static const Harness::Arg13 VA13(712,  &g_argAlloc);
static const Harness::Arg14 VA14(1414, &g_argAlloc);

// Inline methods are defined before the remaining class methods.

template <class T>
inline
bslmf::MovableRef<T> Harness::testArg(T& t, bsl::true_type)
{
    return MoveUtil::move(t);
}

template <class T>
inline
const T& Harness::testArg(T& t, bsl::false_type)
{
    return  t;
}

// Remaining class methods for 'Harness'.

template <class TYPE, int N_ARGS,
          int N01, int N02, int N03, int N04, int N05, int N06, int N07,
          int N08, int N09, int N10, int N11, int N12, int N13, int N14>
void Harness::prepareObject(TYPE   *target,
                            Arg01&  A01,
                            Arg02&  A02,
                            Arg03&  A03,
                            Arg04&  A04,
                            Arg05&  A05,
                            Arg06&  A06,
                            Arg07&  A07,
                            Arg08&  A08,
                            Arg09&  A09,
                            Arg10&  A10,
                            Arg11&  A11,
                            Arg12&  A12,
                            Arg13&  A13,
                            Arg14&  A14)
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT_OPT(da);

    DECLARE_BOOL_CONSTANT(MOVE_01, 1 == N01);
    DECLARE_BOOL_CONSTANT(MOVE_02, 1 == N02);
    DECLARE_BOOL_CONSTANT(MOVE_03, 1 == N03);
    DECLARE_BOOL_CONSTANT(MOVE_04, 1 == N04);
    DECLARE_BOOL_CONSTANT(MOVE_05, 1 == N05);
    DECLARE_BOOL_CONSTANT(MOVE_06, 1 == N06);
    DECLARE_BOOL_CONSTANT(MOVE_07, 1 == N07);
    DECLARE_BOOL_CONSTANT(MOVE_08, 1 == N08);
    DECLARE_BOOL_CONSTANT(MOVE_09, 1 == N09);
    DECLARE_BOOL_CONSTANT(MOVE_10, 1 == N10);
    DECLARE_BOOL_CONSTANT(MOVE_11, 1 == N11);
    DECLARE_BOOL_CONSTANT(MOVE_12, 1 == N12);
    DECLARE_BOOL_CONSTANT(MOVE_13, 1 == N13);
    DECLARE_BOOL_CONSTANT(MOVE_14, 1 == N14);

    switch (N_ARGS) {
      case 0: {
        new (target) TYPE();
      } break;
      case 1: {
        new (target) TYPE(testArg(A01, MOVE_01));
      } break;
      case 2: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02));
      } break;
      case 3: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03));
      } break;
      case 4: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04));
      } break;
      case 5: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05));
      } break;
      case 6: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06));
      } break;
      case 7: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07));
      } break;
      case 8: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08));
      } break;
      case 9: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09));
      } break;
      case 10: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09),
                          testArg(A10, MOVE_10));
      } break;
      case 11: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09),
                          testArg(A10, MOVE_10),
                          testArg(A11, MOVE_11));
      } break;
      case 12: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09),
                          testArg(A10, MOVE_10),
                          testArg(A11, MOVE_11),
                          testArg(A12, MOVE_12));
      } break;
      case 13: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09),
                          testArg(A10, MOVE_10),
                          testArg(A11, MOVE_11),
                          testArg(A12, MOVE_12),
                          testArg(A13, MOVE_13));
      } break;
      case 14: {
        new (target) TYPE(testArg(A01, MOVE_01),
                          testArg(A02, MOVE_02),
                          testArg(A03, MOVE_03),
                          testArg(A04, MOVE_04),
                          testArg(A05, MOVE_05),
                          testArg(A06, MOVE_06),
                          testArg(A07, MOVE_07),
                          testArg(A08, MOVE_08),
                          testArg(A09, MOVE_09),
                          testArg(A10, MOVE_10),
                          testArg(A11, MOVE_11),
                          testArg(A12, MOVE_12),
                          testArg(A13, MOVE_13),
                          testArg(A14, MOVE_14));
      } break;
      default: {
        ASSERTV("Unsupported # of arguments", N_ARGS, false);
      } break;
    }

    const TYPE& EXP = *target;

    ASSERTV(VA01, EXP.arg01(), VA01 == EXP.arg01() || 2 == N01);
    ASSERTV(VA02, EXP.arg02(), VA02 == EXP.arg02() || 2 == N02);
    ASSERTV(VA03, EXP.arg03(), VA03 == EXP.arg03() || 2 == N03);
    ASSERTV(VA04, EXP.arg04(), VA04 == EXP.arg04() || 2 == N04);
    ASSERTV(VA05, EXP.arg05(), VA05 == EXP.arg05() || 2 == N05);
    ASSERTV(VA06, EXP.arg06(), VA06 == EXP.arg06() || 2 == N06);
    ASSERTV(VA07, EXP.arg07(), VA07 == EXP.arg07() || 2 == N07);
    ASSERTV(VA08, EXP.arg08(), VA08 == EXP.arg08() || 2 == N08);
    ASSERTV(VA09, EXP.arg09(), VA09 == EXP.arg09() || 2 == N09);
    ASSERTV(VA10, EXP.arg10(), VA10 == EXP.arg10() || 2 == N10);
    ASSERTV(VA11, EXP.arg11(), VA11 == EXP.arg11() || 2 == N11);
    ASSERTV(VA12, EXP.arg12(), VA12 == EXP.arg12() || 2 == N12);
    ASSERTV(VA13, EXP.arg13(), VA13 == EXP.arg13() || 2 == N13);
    ASSERTV(VA14, EXP.arg14(), VA14 == EXP.arg14() || 2 == N14);

    ASSERTV(da, EXP.arg01().allocator(), da == EXP.arg01().allocator());
    ASSERTV(da, EXP.arg02().allocator(), da == EXP.arg02().allocator());
    ASSERTV(da, EXP.arg03().allocator(), da == EXP.arg03().allocator());
    ASSERTV(da, EXP.arg04().allocator(), da == EXP.arg04().allocator());
    ASSERTV(da, EXP.arg05().allocator(), da == EXP.arg05().allocator());
    ASSERTV(da, EXP.arg06().allocator(), da == EXP.arg06().allocator());
    ASSERTV(da, EXP.arg07().allocator(), da == EXP.arg07().allocator());
    ASSERTV(da, EXP.arg08().allocator(), da == EXP.arg08().allocator());
    ASSERTV(da, EXP.arg09().allocator(), da == EXP.arg09().allocator());
    ASSERTV(da, EXP.arg10().allocator(), da == EXP.arg10().allocator());
    ASSERTV(da, EXP.arg11().allocator(), da == EXP.arg11().allocator());
    ASSERTV(da, EXP.arg12().allocator(), da == EXP.arg12().allocator());
    ASSERTV(da, EXP.arg13().allocator(), da == EXP.arg13().allocator());
    ASSERTV(da, EXP.arg14().allocator(), da == EXP.arg14().allocator());
}

template <class TYPE, int N_ARGS,
          int N01, int N02, int N03, int N04, int N05, int N06, int N07,
          int N08, int N09, int N10, int N11, int N12, int N13, int N14>
void Harness::testCase18_ConcreteTypeRun()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT_OPT(da);

    DECLARE_BOOL_CONSTANT(MOVE_01, 1 == N01);
    DECLARE_BOOL_CONSTANT(MOVE_02, 1 == N02);
    DECLARE_BOOL_CONSTANT(MOVE_03, 1 == N03);
    DECLARE_BOOL_CONSTANT(MOVE_04, 1 == N04);
    DECLARE_BOOL_CONSTANT(MOVE_05, 1 == N05);
    DECLARE_BOOL_CONSTANT(MOVE_06, 1 == N06);
    DECLARE_BOOL_CONSTANT(MOVE_07, 1 == N07);
    DECLARE_BOOL_CONSTANT(MOVE_08, 1 == N08);
    DECLARE_BOOL_CONSTANT(MOVE_09, 1 == N09);
    DECLARE_BOOL_CONSTANT(MOVE_10, 1 == N10);
    DECLARE_BOOL_CONSTANT(MOVE_11, 1 == N11);
    DECLARE_BOOL_CONSTANT(MOVE_12, 1 == N12);
    DECLARE_BOOL_CONSTANT(MOVE_13, 1 == N13);
    DECLARE_BOOL_CONSTANT(MOVE_14, 1 == N14);

    // First prepare a test object to compare against the managed objects
    // constructed by 'makeManaged' and 'allocateManaged'.  In particular, we
    // want to pay attention to the moved-from state of each argument, and the
    // final value of the whole test object.

    // 14 arguments for expected object

    Arg01 A01 = VA01;
    Arg02 A02 = VA02;
    Arg03 A03 = VA03;
    Arg04 A04 = VA04;
    Arg05 A05 = VA05;
    Arg06 A06 = VA06;
    Arg07 A07 = VA07;
    Arg08 A08 = VA08;
    Arg09 A09 = VA09;
    Arg10 A10 = VA10;
    Arg11 A11 = VA11;
    Arg12 A12 = VA12;
    Arg13 A13 = VA13;
    Arg14 A14 = VA14;

    typedef typename bsl::remove_cv<TYPE>::type UNQUALIFIED_TYPE;

    bsls::ObjectBuffer<UNQUALIFIED_TYPE> buffer;
    prepareObject<UNQUALIFIED_TYPE, N_ARGS,
                  N01, N02, N03, N04, N05, N06, N07,
                  N08, N09, N10, N11, N12, N13, N14>(
                       buffer.address(),
                       A01, A02, A03, A04, A05, A06, A07,
                       A08, A09, A10, A11, A12, A13, A14);

    bslma::DestructorProctor<UNQUALIFIED_TYPE> proctor(buffer.address());

    const UNQUALIFIED_TYPE& EXP = buffer.object();

    // Here starts the actual test case.

    // When we move an "arg" object, no new allocation occurs, but when we copy
    // an "arg" object an additional allocation will occur.  We need to account
    // for the additional allocations within the test blocks below, but we also
    // need to account for the deallocations *outside* those blocks.  Also,
    // remember the allocation for the footprint of the managed object when
    // accounting for total allocations and deallocations.

    bsls::Types::Int64 nArgCopies = 0;

    switch (N_ARGS) {
      case 14: if (0 == N14) { ++nArgCopies; }  // fall-through
      case 13: if (0 == N13) { ++nArgCopies; }  // fall-through
      case 12: if (0 == N12) { ++nArgCopies; }  // fall-through
      case 11: if (0 == N11) { ++nArgCopies; }  // fall-through
      case 10: if (0 == N10) { ++nArgCopies; }  // fall-through
      case  9: if (0 == N09) { ++nArgCopies; }  // fall-through
      case  8: if (0 == N08) { ++nArgCopies; }  // fall-through
      case  7: if (0 == N07) { ++nArgCopies; }  // fall-through
      case  6: if (0 == N06) { ++nArgCopies; }  // fall-through
      case  5: if (0 == N05) { ++nArgCopies; }  // fall-through
      case  4: if (0 == N04) { ++nArgCopies; }  // fall-through
      case  3: if (0 == N03) { ++nArgCopies; }  // fall-through
      case  2: if (0 == N02) { ++nArgCopies; }  // fall-through
      case  1: if (0 == N01) { ++nArgCopies; }  // fall-through
      case  0: break;                           // silence warnings
    }

    bsls::Types::Int64 nArgMoves = 0;

    switch (N_ARGS) {
      case 14: if (1 == N14) { ++nArgMoves; }  // fall-through
      case 13: if (1 == N13) { ++nArgMoves; }  // fall-through
      case 12: if (1 == N12) { ++nArgMoves; }  // fall-through
      case 11: if (1 == N11) { ++nArgMoves; }  // fall-through
      case 10: if (1 == N10) { ++nArgMoves; }  // fall-through
      case  9: if (1 == N09) { ++nArgMoves; }  // fall-through
      case  8: if (1 == N08) { ++nArgMoves; }  // fall-through
      case  7: if (1 == N07) { ++nArgMoves; }  // fall-through
      case  6: if (1 == N06) { ++nArgMoves; }  // fall-through
      case  5: if (1 == N05) { ++nArgMoves; }  // fall-through
      case  4: if (1 == N04) { ++nArgMoves; }  // fall-through
      case  3: if (1 == N03) { ++nArgMoves; }  // fall-through
      case  2: if (1 == N02) { ++nArgMoves; }  // fall-through
      case  1: if (1 == N01) { ++nArgMoves; }  // fall-through
      case  0: break;                          // silence warnings
    }

    bslma::TestAllocatorMonitor dam(da);

    {
        // Arguments for functions.

        Arg01 B01[3] = { VA01, VA01, VA01 };
        Arg02 B02[3] = { VA02, VA02, VA02 };
        Arg03 B03[3] = { VA03, VA03, VA03 };
        Arg04 B04[3] = { VA04, VA04, VA04 };
        Arg05 B05[3] = { VA05, VA05, VA05 };
        Arg06 B06[3] = { VA06, VA06, VA06 };
        Arg07 B07[3] = { VA07, VA07, VA07 };
        Arg08 B08[3] = { VA08, VA08, VA08 };
        Arg09 B09[3] = { VA09, VA09, VA09 };
        Arg10 B10[3] = { VA10, VA10, VA10 };
        Arg11 B11[3] = { VA11, VA11, VA11 };
        Arg12 B12[3] = { VA12, VA12, VA12 };
        Arg13 B13[3] = { VA13, VA13, VA13 };
        Arg14 B14[3] = { VA14, VA14, VA14 };

        bsls::Types::Int64 numAllocationsDA   = da->numAllocations();
        bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();

        bsls::Types::Int64 EXPECTED_DA_NUM_ALLOCATIONS   = 0;
        bsls::Types::Int64 EXPECTED_DA_NUM_DEALLOCATIONS = 0;

        // Testing 'makeManaged'.
        {
            bslma::ManagedPtr<TYPE> mX;

            switch (N_ARGS) {
              case 0: {
                mX = Util::makeManaged<TYPE>();
              } break;
              case 1: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01));
              } break;
              case 2: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02));
              } break;
              case 3: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03));
              } break;
              case 4: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04));
              } break;
              case 5: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05));
              } break;
              case 6: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06));
              } break;
              case 7: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07));
              } break;
              case 8: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08));
              } break;
              case 9: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09));
              } break;
              case 10: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09),
                                             testArg(B10[0], MOVE_10));
              } break;
              case 11: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09),
                                             testArg(B10[0], MOVE_10),
                                             testArg(B11[0], MOVE_11));
              } break;
              case 12: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09),
                                             testArg(B10[0], MOVE_10),
                                             testArg(B11[0], MOVE_11),
                                             testArg(B12[0], MOVE_12));
              } break;
              case 13: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09),
                                             testArg(B10[0], MOVE_10),
                                             testArg(B11[0], MOVE_11),
                                             testArg(B12[0], MOVE_12),
                                             testArg(B13[0], MOVE_13));
              } break;
              case 14: {
                mX = Util::makeManaged<TYPE>(testArg(B01[0], MOVE_01),
                                             testArg(B02[0], MOVE_02),
                                             testArg(B03[0], MOVE_03),
                                             testArg(B04[0], MOVE_04),
                                             testArg(B05[0], MOVE_05),
                                             testArg(B06[0], MOVE_06),
                                             testArg(B07[0], MOVE_07),
                                             testArg(B08[0], MOVE_08),
                                             testArg(B09[0], MOVE_09),
                                             testArg(B10[0], MOVE_10),
                                             testArg(B11[0], MOVE_11),
                                             testArg(B12[0], MOVE_12),
                                             testArg(B13[0], MOVE_13),
                                             testArg(B14[0], MOVE_14));
              } break;
              default: {
                ASSERTV("Unsupported # of arguments", N_ARGS, false);
              } break;
            }

            // Verify memory allocation and deallocation.

            EXPECTED_DA_NUM_ALLOCATIONS =
                         numAllocationsDA  // previous value
                       + 1                 // block for object itself
                       + nArgCopies;       // blocks for copied object's fields
            ASSERTV(EXPECTED_DA_NUM_ALLOCATIONS,   da->numAllocations(),
                    EXPECTED_DA_NUM_ALLOCATIONS == da->numAllocations());

            ASSERTV(numDeallocationsDA,   da->numDeallocations(),
                    numDeallocationsDA == da->numDeallocations());

            // Verify the value of the resulting object.

            ASSERTV(EXP == *mX);

            // Verify that the allocator wasn't passed as the last argument to
            // the managed object's constructor.

            ASSERTV(mX->allocator(), 0 == mX->allocator());

            ASSERT(A01.movedFrom() == B01[0].movedFrom());
            ASSERT(A02.movedFrom() == B02[0].movedFrom());
            ASSERT(A03.movedFrom() == B03[0].movedFrom());
            ASSERT(A04.movedFrom() == B04[0].movedFrom());
            ASSERT(A05.movedFrom() == B05[0].movedFrom());
            ASSERT(A06.movedFrom() == B06[0].movedFrom());
            ASSERT(A07.movedFrom() == B07[0].movedFrom());
            ASSERT(A08.movedFrom() == B08[0].movedFrom());
            ASSERT(A09.movedFrom() == B09[0].movedFrom());
            ASSERT(A10.movedFrom() == B10[0].movedFrom());
            ASSERT(A11.movedFrom() == B11[0].movedFrom());
            ASSERT(A12.movedFrom() == B12[0].movedFrom());
            ASSERT(A13.movedFrom() == B13[0].movedFrom());
            ASSERT(A14.movedFrom() == B14[0].movedFrom());
        }

        // Verify memory deallocation.

        EXPECTED_DA_NUM_DEALLOCATIONS =
                       numDeallocationsDA  // previous value
                     + 1                   // block for object itself
                     + nArgCopies          // blocks for copied object's fields
                     + nArgMoves;          // blocks for moved object's fields

        ASSERTV(EXPECTED_DA_NUM_DEALLOCATIONS,   da->numDeallocations(),
                EXPECTED_DA_NUM_DEALLOCATIONS == da->numDeallocations());

        // Testing 'allocateManaged(&sa, ...)'.

        // Renew the current values.

        numAllocationsDA   = da->numAllocations();
        numDeallocationsDA = da->numDeallocations();

        bslma::TestAllocator sa("supplied", g_veryVeryVeryVerbose);

        bsls::Types::Int64 EXPECTED_SA_NUM_ALLOCATIONS   = 0;
        bsls::Types::Int64 EXPECTED_SA_NUM_DEALLOCATIONS = 0;

        bsls::Types::Int64 numAllocationsSA   = sa.numAllocations();
        bsls::Types::Int64 numDeallocationsSA = sa.numDeallocations();

        {
            bslma::ManagedPtr<TYPE> mX;

            switch (N_ARGS) {
              case 0: {
                mX = Util::allocateManaged<TYPE>(&sa);
              } break;
              case 1: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01));
              } break;
              case 2: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02));
              } break;
              case 3: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03));
              } break;
              case 4: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04));
              } break;
              case 5: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05));
              } break;
              case 6: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06));
              } break;
              case 7: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07));
              } break;
              case 8: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08));
              } break;
              case 9: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09));
              } break;
              case 10: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09),
                                                 testArg(B10[1], MOVE_10));
              } break;
              case 11: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09),
                                                 testArg(B10[1], MOVE_10),
                                                 testArg(B11[1], MOVE_11));
              } break;
              case 12: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09),
                                                 testArg(B10[1], MOVE_10),
                                                 testArg(B11[1], MOVE_11),
                                                 testArg(B12[1], MOVE_12));
              } break;
              case 13: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09),
                                                 testArg(B10[1], MOVE_10),
                                                 testArg(B11[1], MOVE_11),
                                                 testArg(B12[1], MOVE_12),
                                                 testArg(B13[1], MOVE_13));
              } break;
              case 14: {
                mX = Util::allocateManaged<TYPE>(&sa,
                                                 testArg(B01[1], MOVE_01),
                                                 testArg(B02[1], MOVE_02),
                                                 testArg(B03[1], MOVE_03),
                                                 testArg(B04[1], MOVE_04),
                                                 testArg(B05[1], MOVE_05),
                                                 testArg(B06[1], MOVE_06),
                                                 testArg(B07[1], MOVE_07),
                                                 testArg(B08[1], MOVE_08),
                                                 testArg(B09[1], MOVE_09),
                                                 testArg(B10[1], MOVE_10),
                                                 testArg(B11[1], MOVE_11),
                                                 testArg(B12[1], MOVE_12),
                                                 testArg(B13[1], MOVE_13),
                                                 testArg(B14[1], MOVE_14));
              } break;
              default: {
                ASSERTV("Unsupported # of arguments", N_ARGS, false);
              } break;
            }

            // Verify memory allocation and deallocation.

            if (bslma::UsesBslmaAllocator<TYPE>::value) {
                // The supplied allocator ('sa') is used by the managed object
                // in this case.  Since 'sa != da', a temporary object is
                // created for each argument, whether copied or moved.  This
                // incurs an allocation from 'sa' for each argument, but an
                // allocation from 'da' on only those arguments that are copied
                // because the allocation for the moved arguments ('B01[1]',
                // 'B02[1]', etc.) occurred when they were constructed (just
                // above the test block for 'makeManaged').

                EXPECTED_DA_NUM_ALLOCATIONS =
                              numAllocationsDA  // previous value
                            + nArgCopies;       // blocks for temporary objects

                EXPECTED_DA_NUM_DEALLOCATIONS =
                       numDeallocationsDA  // previous value
                     + nArgCopies          // blocks for temporary objects
                     + nArgMoves;          // blocks for moved object's fields

                EXPECTED_SA_NUM_ALLOCATIONS =
                         numAllocationsSA  // previous value
                       + 1                 // block for object itself
                       + nArgCopies        // blocks for copied object's fields
                       + nArgMoves;        // blocks for moved object's fields
            }
            else {
                EXPECTED_DA_NUM_ALLOCATIONS =
                         numAllocationsDA  // previous value
                       + nArgCopies;       // blocks for copied object's fields

                EXPECTED_DA_NUM_DEALLOCATIONS =
                                         numDeallocationsDA;  // previous value

                EXPECTED_SA_NUM_ALLOCATIONS = 1;  // block for object itself
            }

            ASSERTV(EXPECTED_DA_NUM_ALLOCATIONS,     da->numAllocations(),
                    EXPECTED_DA_NUM_ALLOCATIONS   == da->numAllocations());
            ASSERTV(EXPECTED_DA_NUM_DEALLOCATIONS,   da->numDeallocations(),
                    EXPECTED_DA_NUM_DEALLOCATIONS == da->numDeallocations());

            ASSERTV(EXPECTED_SA_NUM_ALLOCATIONS,     sa.numAllocations(),
                    EXPECTED_SA_NUM_ALLOCATIONS   == sa.numAllocations());
            ASSERTV(numDeallocationsSA,              sa.numDeallocations(),
                    numDeallocationsSA            == sa.numDeallocations());

            // Verify the value of the resulting object.

            ASSERTV(EXP == *mX);

            // Verify that the allocator was passed as the last argument to the
            // managed object's constructor only when expected.

            bslma::Allocator *EXP_ALLOC_PTR = 0;
            if (bslma::UsesBslmaAllocator<TYPE>::value) {
                EXP_ALLOC_PTR = &sa;
            }

            ASSERTV(EXP_ALLOC_PTR,   mX->allocator(),
                    EXP_ALLOC_PTR == mX->allocator());

            ASSERT(A01.movedFrom() == B01[1].movedFrom());
            ASSERT(A02.movedFrom() == B02[1].movedFrom());
            ASSERT(A03.movedFrom() == B03[1].movedFrom());
            ASSERT(A04.movedFrom() == B04[1].movedFrom());
            ASSERT(A05.movedFrom() == B05[1].movedFrom());
            ASSERT(A06.movedFrom() == B06[1].movedFrom());
            ASSERT(A07.movedFrom() == B07[1].movedFrom());
            ASSERT(A08.movedFrom() == B08[1].movedFrom());
            ASSERT(A09.movedFrom() == B09[1].movedFrom());
            ASSERT(A10.movedFrom() == B10[1].movedFrom());
            ASSERT(A11.movedFrom() == B11[1].movedFrom());
            ASSERT(A12.movedFrom() == B12[1].movedFrom());
            ASSERT(A13.movedFrom() == B13[1].movedFrom());
            ASSERT(A14.movedFrom() == B14[1].movedFrom());

            // Renew the current values.

            numDeallocationsDA = da->numDeallocations();
            numDeallocationsSA = sa.numDeallocations();
        }

        // Verify memory deallocation.

        if (bslma::UsesBslmaAllocator<TYPE>::value) {
            EXPECTED_DA_NUM_DEALLOCATIONS =
                                         numDeallocationsDA;  // previous value

            EXPECTED_SA_NUM_DEALLOCATIONS =
                       numDeallocationsSA  // previous value
                     + 1                   // block for object itself
                     + nArgCopies          // blocks for copied object's fields
                     + nArgMoves;          // blocks for moved object's fields
        }
        else {
            EXPECTED_DA_NUM_DEALLOCATIONS =
                       numDeallocationsDA  // previous value
                     + nArgCopies          // blocks for copied object's fields
                     + nArgMoves;          // blocks for moved object's fields

            EXPECTED_SA_NUM_DEALLOCATIONS = 1;  // block for object itself
        }

        ASSERTV(EXPECTED_DA_NUM_DEALLOCATIONS,   da->numDeallocations(),
                EXPECTED_DA_NUM_DEALLOCATIONS == da->numDeallocations());

        ASSERTV(EXPECTED_SA_NUM_DEALLOCATIONS,   sa.numDeallocations(),
                EXPECTED_SA_NUM_DEALLOCATIONS == sa.numDeallocations());

        // Testing 'allocateManaged(0, ...)'.

        // Renew the current values.

        numAllocationsDA   = da->numAllocations();
        numDeallocationsDA = da->numDeallocations();

        {
            bslma::ManagedPtr<TYPE> mX;

            switch (N_ARGS) {
              case 0: {
                mX = Util::allocateManaged<TYPE>(0);
              } break;
              case 1: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01));
              } break;
              case 2: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02));
              } break;
              case 3: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03));
              } break;
              case 4: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04));
              } break;
              case 5: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05));
              } break;
              case 6: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06));
              } break;
              case 7: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07));
              } break;
              case 8: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08));
              } break;
              case 9: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09));
              } break;
              case 10: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09),
                                                 testArg(B10[2], MOVE_10));
              } break;
              case 11: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09),
                                                 testArg(B10[2], MOVE_10),
                                                 testArg(B11[2], MOVE_11));
              } break;
              case 12: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09),
                                                 testArg(B10[2], MOVE_10),
                                                 testArg(B11[2], MOVE_11),
                                                 testArg(B12[2], MOVE_12));
              } break;
              case 13: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09),
                                                 testArg(B10[2], MOVE_10),
                                                 testArg(B11[2], MOVE_11),
                                                 testArg(B12[2], MOVE_12),
                                                 testArg(B13[2], MOVE_13));
              } break;
              case 14: {
                mX = Util::allocateManaged<TYPE>(0,
                                                 testArg(B01[2], MOVE_01),
                                                 testArg(B02[2], MOVE_02),
                                                 testArg(B03[2], MOVE_03),
                                                 testArg(B04[2], MOVE_04),
                                                 testArg(B05[2], MOVE_05),
                                                 testArg(B06[2], MOVE_06),
                                                 testArg(B07[2], MOVE_07),
                                                 testArg(B08[2], MOVE_08),
                                                 testArg(B09[2], MOVE_09),
                                                 testArg(B10[2], MOVE_10),
                                                 testArg(B11[2], MOVE_11),
                                                 testArg(B12[2], MOVE_12),
                                                 testArg(B13[2], MOVE_13),
                                                 testArg(B14[2], MOVE_14));
              } break;
              default: {
                ASSERTV("Unsupported # of arguments", N_ARGS, false);
              } break;
            }

            // Verify memory allocation and deallocation.

            EXPECTED_DA_NUM_ALLOCATIONS =
                         numAllocationsDA  // previous value
                       + 1                 // block for object itself
                       + nArgCopies;       // blocks for copied object's fields

            EXPECTED_DA_NUM_DEALLOCATIONS =
                      numDeallocationsDA;  // previous value

            ASSERTV(EXPECTED_DA_NUM_ALLOCATIONS,     da->numAllocations(),
                    EXPECTED_DA_NUM_ALLOCATIONS   == da->numAllocations());
            ASSERTV(EXPECTED_DA_NUM_DEALLOCATIONS,   da->numDeallocations(),
                    EXPECTED_DA_NUM_DEALLOCATIONS == da->numDeallocations());

            // Verify the value of the resulting object.

            ASSERTV(EXP == *mX);

            // Verify that the allocator was passed as the last argument to the
            // managed object's constructor only when expected.

            bslma::Allocator *EXP_ALLOC_PTR = 0;
            if (bslma::UsesBslmaAllocator<TYPE>::value) {
                EXP_ALLOC_PTR = da;
            }

            ASSERTV(EXP_ALLOC_PTR,   mX->allocator(),
                    EXP_ALLOC_PTR == mX->allocator());

            ASSERT(A01.movedFrom() == B01[2].movedFrom());
            ASSERT(A02.movedFrom() == B02[2].movedFrom());
            ASSERT(A03.movedFrom() == B03[2].movedFrom());
            ASSERT(A04.movedFrom() == B04[2].movedFrom());
            ASSERT(A05.movedFrom() == B05[2].movedFrom());
            ASSERT(A06.movedFrom() == B06[2].movedFrom());
            ASSERT(A07.movedFrom() == B07[2].movedFrom());
            ASSERT(A08.movedFrom() == B08[2].movedFrom());
            ASSERT(A09.movedFrom() == B09[2].movedFrom());
            ASSERT(A10.movedFrom() == B10[2].movedFrom());
            ASSERT(A11.movedFrom() == B11[2].movedFrom());
            ASSERT(A12.movedFrom() == B12[2].movedFrom());
            ASSERT(A13.movedFrom() == B13[2].movedFrom());
            ASSERT(A14.movedFrom() == B14[2].movedFrom());

            // Renew the current value.

            numDeallocationsDA = da->numDeallocations();
        }

        // Verify memory deallocation.

        EXPECTED_DA_NUM_DEALLOCATIONS =
                       numDeallocationsDA  // previous value
                     + 1                   // block for object itself
                     + nArgCopies          // blocks for copied object's fields
                     + nArgMoves;          // blocks for moved object's fields

        ASSERTV(EXPECTED_DA_NUM_DEALLOCATIONS,   da->numDeallocations(),
                EXPECTED_DA_NUM_DEALLOCATIONS == da->numDeallocations());
    }
    ASSERT(dam.isInUseSame());
}

template <int N_ARGS,
          int N01, int N02, int N03, int N04, int N05, int N06, int N07,
          int N08, int N09, int N10, int N11, int N12, int N13, int N14>
void Harness::testCase18_ArgumentsTest()
{
    typedef       UsingType      UT;
    typedef const UsingType     CUT;

    typedef       NotUsingType  NUT;
    typedef const NotUsingType CNUT;

    testCase18_ConcreteTypeRun<  UT, N_ARGS,
                                     N01, N02, N03, N04, N05, N06, N07,
                                     N08, N09, N10, N11, N12, N13, N14>();
    testCase18_ConcreteTypeRun< CUT, N_ARGS,
                                     N01, N02, N03, N04, N05, N06, N07,
                                     N08, N09, N10, N11, N12, N13, N14>();

    testCase18_ConcreteTypeRun< NUT, N_ARGS,
                                     N01, N02, N03, N04, N05, N06, N07,
                                     N08, N09, N10, N11, N12, N13, N14>();
    testCase18_ConcreteTypeRun<CNUT, N_ARGS,
                                     N01, N02, N03, N04, N05, N06, N07,
                                     N08, N09, N10, N11, N12, N13, N14>();
}

void Harness::testCase18_NoAllocTestSingleCheck(int   numParameters,
                                                char  c,
                                                int   i,
                                                float f)
{
    bslma::TestAllocator *da =
        dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT_OPT(da);

    // Testing 'makeManaged'.

    {
        bsls::Types::Int64 numAllocationsDA   = da->numAllocations();
        bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();
        bsls::Types::Int64 numBytesInUseDA    = da->numBytesInUse();

        const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA   =
                                                        numAllocationsDA + 1;
        const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA =
                                                        numDeallocationsDA + 1;
        const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA    =
                                     numBytesInUseDA + sizeof(NoAllocTestType);

        {
            bslma::ManagedPtr<NoAllocTestType> mX;

            switch (numParameters) {
              case 0: {
                mX = Util::makeManaged<NoAllocTestType>();
              } break;
              case 1: {
                mX = Util::makeManaged<NoAllocTestType>(c);
              } break;
              case 2: {
                mX = Util::makeManaged<NoAllocTestType>(c, i);
              } break;
              case 3: {
                mX = Util::makeManaged<NoAllocTestType>(c, i, f);
              } break;
              default: {
                ASSERTV("Unsupported # of parameters", numParameters, false);
              }
            }

            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_ALLOCATIONS_DA,      da->numAllocations(),
                    EXP_NUM_ALLOCATIONS_DA    == da->numAllocations());
            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_BYTESINUSE_DA,       da->numAllocations(),
                    EXP_NUM_BYTESINUSE_DA     == da->numBytesInUse());

            ASSERTV(numParameters, c, i, f, c == mX->getChar());
            ASSERTV(numParameters, c, i, f, i == mX->getInt());
            ASSERTV(numParameters, c, i, f, f == mX->getFloat());
        }

        ASSERTV(numParameters, c, i, f,
                EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());
    }

    // Testing 'allocateManaged(&sa, ...)'.

    {
        bslma::TestAllocator sa("supplied", g_veryVeryVeryVerbose);

        bsls::Types::Int64 numAllocationsDA   = da->numAllocations();
        bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();
        bsls::Types::Int64 numBytesInUseDA    = da->numBytesInUse();

        bsls::Types::Int64 numAllocationsSA   = sa.numAllocations();
        bsls::Types::Int64 numDeallocationsSA = sa.numDeallocations();
        bsls::Types::Int64 numBytesInUseSA    = sa.numBytesInUse();

        const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA   = numAllocationsDA;
        const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA = numDeallocationsDA;
        const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA    = numBytesInUseDA;

        const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_SA   =
                                                        numAllocationsSA + 1;
        const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_SA =
                                                        numDeallocationsSA + 1;
        const bsls::Types::Int64 EXP_NUM_BYTESINUSE_SA    =
                                     numBytesInUseSA + sizeof(NoAllocTestType);

        {
            bslma::ManagedPtr<NoAllocTestType> mX;

            switch (numParameters) {
              case 0: {
                mX = Util::allocateManaged<NoAllocTestType>(&sa);
              } break;
              case 1: {
                mX = Util::allocateManaged<NoAllocTestType>(&sa, c);
              } break;
              case 2: {
                mX = Util::allocateManaged<NoAllocTestType>(&sa, c, i);
              } break;
              case 3: {
                mX = Util::allocateManaged<NoAllocTestType>(&sa, c, i, f);
              } break;
              default: {
                ASSERTV("Unsupported # of parameters", numParameters, false);
              }
            }

            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_ALLOCATIONS_DA,      da->numAllocations(),
                    EXP_NUM_ALLOCATIONS_DA    == da->numAllocations());
            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_BYTESINUSE_DA,       da->numAllocations(),
                    EXP_NUM_BYTESINUSE_DA     == da->numBytesInUse());

            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_ALLOCATIONS_SA,      sa.numAllocations(),
                    EXP_NUM_ALLOCATIONS_SA    == sa.numAllocations());
            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_BYTESINUSE_SA,       sa.numAllocations(),
                    EXP_NUM_BYTESINUSE_SA     == sa.numBytesInUse());

            ASSERTV(numParameters, c, i, f, c == mX->getChar());
            ASSERTV(numParameters, c, i, f, i == mX->getInt());
            ASSERTV(numParameters, c, i, f, f == mX->getFloat());
        }

        ASSERTV(numParameters, c, i, f,
                EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());
        ASSERTV(numParameters, c, i, f,
                EXP_NUM_DEALLOCATIONS_SA,   sa.numDeallocations(),
                EXP_NUM_DEALLOCATIONS_SA == sa.numAllocations());
    }

    // Testing 'allocateManaged(0, ...)'.

    {
        bsls::Types::Int64 numAllocationsDA   = da->numAllocations();
        bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();
        bsls::Types::Int64 numBytesInUseDA    = da->numBytesInUse();

        const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA   =
                                                        numAllocationsDA + 1;
        const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA =
                                                        numDeallocationsDA + 1;
        const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA    =
                                     numBytesInUseDA + sizeof(NoAllocTestType);

        {
            bslma::ManagedPtr<NoAllocTestType> mX;

            switch (numParameters) {
              case 0: {
                mX = Util::allocateManaged<NoAllocTestType>(0);
              } break;
              case 1: {
                mX = Util::allocateManaged<NoAllocTestType>(0, c);
              } break;
              case 2: {
                mX = Util::allocateManaged<NoAllocTestType>(0, c, i);
              } break;
              case 3: {
                mX = Util::allocateManaged<NoAllocTestType>(0, c, i, f);
              } break;
              default: {
                ASSERTV("Unsupported # of parameters", numParameters, false);
              }
            }

            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_ALLOCATIONS_DA,      da->numAllocations(),
                    EXP_NUM_ALLOCATIONS_DA    == da->numAllocations());
            ASSERTV(numParameters, c, i, f,
                    EXP_NUM_BYTESINUSE_DA,       da->numAllocations(),
                    EXP_NUM_BYTESINUSE_DA     == da->numBytesInUse());

            ASSERTV(numParameters, c, i, f, c == mX->getChar());
            ASSERTV(numParameters, c, i, f, i == mX->getInt());
            ASSERTV(numParameters, c, i, f, f == mX->getFloat());
        }

        ASSERTV(numParameters, c, i, f,
                EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());
    }
}

void Harness::testCase18_NoAllocTest()
{
    const char   CHAR_CYCLE_LIMIT = 10;
    const int     INT_CYCLE_LIMIT = 10;
    const float FLOAT_CYCLE_LIMIT = 10;

    for (char c = 0; c < CHAR_CYCLE_LIMIT; ++c) {
        for (int i = 0; i < INT_CYCLE_LIMIT; ++i) {
            for (float f = 0; f < FLOAT_CYCLE_LIMIT; ++f) {
                testCase18_NoAllocTestSingleCheck(3, c, i, f);
            }

            testCase18_NoAllocTestSingleCheck(2, c, i, 0);
        }

        testCase18_NoAllocTestSingleCheck(1, c, 0, 0);
    }

    testCase18_NoAllocTestSingleCheck(0, 0, 0, 0);
}

void Harness::testCase18_AllocOnlyTest()
{
    bslma::TestAllocator *da =
        dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT_OPT(da);

    bslma::TestAllocator aa1("arg1", g_veryVeryVeryVerbose);
    bslma::TestAllocator aa2("arg2", g_veryVeryVeryVerbose);
    bslma::TestAllocator aa3("arg3", g_veryVeryVeryVerbose);

    // Testing 'makeManaged'.

    for (int i = 0; i < 4; ++i) {
        bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();

        {
            bslma::ManagedPtr<AllocOnlyTestType> mX;

            bslma::TestAllocator *EXP1 = 0;
            bslma::TestAllocator *EXP2 = 0;
            bslma::TestAllocator *EXP3 = 0;

            bsls::Types::Int64 numAllocationsDA = da->numAllocations();
            bsls::Types::Int64 numBytesInUseDA  = da->numBytesInUse();

            switch (i) {
              case 0: {
                mX   = Util::makeManaged<AllocOnlyTestType>();
              } break;
              case 1: {
                mX   = Util::makeManaged<AllocOnlyTestType>(&aa1);
                EXP1 = &aa1;
              } break;
              case 2: {
                mX   = Util::makeManaged<AllocOnlyTestType>(&aa1, &aa2);
                EXP1 = &aa1;
                EXP2 = &aa2;
              } break;
              case 3: {
                mX   = Util::makeManaged<AllocOnlyTestType>(&aa1, &aa2, &aa3);
                EXP1 = &aa1;
                EXP2 = &aa2;
                EXP3 = &aa3;
              } break;
              default: {
                ASSERTV("Unsupported # of arguments", i, false);
              } break;
            }

            const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA =
                                                          numAllocationsDA + 1;
            const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA =
                                   numBytesInUseDA + sizeof(AllocOnlyTestType);

            ASSERTV(i, EXP_NUM_ALLOCATIONS_DA,     da->numAllocations(),
                       EXP_NUM_ALLOCATIONS_DA   == da->numAllocations());
            ASSERTV(i, EXP_NUM_BYTESINUSE_DA,      da->numBytesInUse(),
                       EXP_NUM_BYTESINUSE_DA    == da->numBytesInUse());

            ASSERTV(i, EXP1, mX->alloc1(), EXP1 == mX->alloc1());
            ASSERTV(i, EXP2, mX->alloc2(), EXP2 == mX->alloc2());
            ASSERTV(i, EXP3, mX->alloc3(), EXP3 == mX->alloc3());
        }

        const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA =
                                                        numDeallocationsDA + 1;
        ASSERTV(i, EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                   EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());
    }

    // Testing 'allocateManaged(&sa, ...)'.

    // Since 'true == bslma::UsesBslmaAllocator<AllocOnlyTestType>::value', the
    // first argument passed to 'allocateManaged' (i.e., the allocator) is
    // passed as the *last* argument to the 'AllocOnlyTestType' constructor.

    {
        bslma::TestAllocator sa("supplied", g_veryVeryVeryVerbose);

        for (int i = 0; i < 3; ++i) {
            bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();
            bsls::Types::Int64 numDeallocationsSA = sa.numDeallocations();

            {
                bslma::ManagedPtr<AllocOnlyTestType> mX;

                bslma::TestAllocator *EXP1 = 0;
                bslma::TestAllocator *EXP2 = 0;
                bslma::TestAllocator *EXP3 = 0;

                bsls::Types::Int64 numAllocationsDA = da->numAllocations();
                bsls::Types::Int64 numBytesInUseDA  = da->numBytesInUse();

                bsls::Types::Int64 numAllocationsSA = sa.numAllocations();
                bsls::Types::Int64 numBytesInUseSA  = sa.numBytesInUse();

                switch (i) {
                  case 0: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(&sa);
                    EXP1 = &sa;
                  } break;
                  case 1: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(&sa, &aa1);
                    EXP1 = &aa1;
                    EXP2 = &sa;
                  } break;
                  case 2: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(
                                                              &sa, &aa1, &aa2);
                    EXP1 = &aa1;
                    EXP2 = &aa2;
                    EXP3 = &sa;
                  } break;
                  default: {
                    ASSERTV("Unsupported # of arguments", i, false);
                  } break;
                }

                const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA =
                                                              numAllocationsDA;
                const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA =
                                                              numBytesInUseDA;

                ASSERTV(i, EXP_NUM_ALLOCATIONS_DA,     da->numAllocations(),
                           EXP_NUM_ALLOCATIONS_DA   == da->numAllocations());
                ASSERTV(i, EXP_NUM_BYTESINUSE_DA,      da->numBytesInUse(),
                           EXP_NUM_BYTESINUSE_DA    == da->numBytesInUse());

                const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_SA =
                                                          numAllocationsSA + 1;
                const bsls::Types::Int64 EXP_NUM_BYTESINUSE_SA =
                                   numBytesInUseSA + sizeof(AllocOnlyTestType);

                ASSERTV(i, EXP_NUM_ALLOCATIONS_SA,     sa.numAllocations(),
                           EXP_NUM_ALLOCATIONS_SA   == sa.numAllocations());
                ASSERTV(i, EXP_NUM_BYTESINUSE_SA,      sa.numBytesInUse(),
                           EXP_NUM_BYTESINUSE_SA    == sa.numBytesInUse());

                ASSERTV(i, EXP1, mX->alloc1(), EXP1 == mX->alloc1());
                ASSERTV(i, EXP2, mX->alloc2(), EXP2 == mX->alloc2());
                ASSERTV(i, EXP3, mX->alloc3(), EXP3 == mX->alloc3());
            }

            const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA =
                                                            numDeallocationsDA;
            ASSERTV(i, EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                       EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());

            const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_SA =
                                                        numDeallocationsSA + 1;
            ASSERTV(i, EXP_NUM_DEALLOCATIONS_SA,   sa.numDeallocations(),
                       EXP_NUM_DEALLOCATIONS_SA == sa.numAllocations());
        }
    }

    // Testing 'allocateManaged(0, ...)'.

    {
        for (int i = 0; i < 3; ++i) {
            bsls::Types::Int64 numDeallocationsDA = da->numDeallocations();

            {
                bslma::ManagedPtr<AllocOnlyTestType> mX;

                bslma::TestAllocator *EXP1 = 0;
                bslma::TestAllocator *EXP2 = 0;
                bslma::TestAllocator *EXP3 = 0;

                bsls::Types::Int64 numAllocationsDA = da->numAllocations();
                bsls::Types::Int64 numBytesInUseDA  = da->numBytesInUse();

                switch (i) {
                  case 0: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(0);
                    EXP1 = da;
                  } break;
                  case 1: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(0, &aa1);
                    EXP1 = &aa1;
                    EXP2 = da;
                  } break;
                  case 2: {
                    mX   = Util::allocateManaged<AllocOnlyTestType>(
                                                                0, &aa1, &aa2);
                    EXP1 = &aa1;
                    EXP2 = &aa2;
                    EXP3 = da;
                  } break;
                  default: {
                    ASSERTV("Unsupported # of arguments", i, false);
                  } break;
                }

                const bsls::Types::Int64 EXP_NUM_ALLOCATIONS_DA =
                                                          numAllocationsDA + 1;
                const bsls::Types::Int64 EXP_NUM_BYTESINUSE_DA =
                                   numBytesInUseDA + sizeof(AllocOnlyTestType);

                ASSERTV(i, EXP_NUM_ALLOCATIONS_DA,     da->numAllocations(),
                           EXP_NUM_ALLOCATIONS_DA   == da->numAllocations());
                ASSERTV(i, EXP_NUM_BYTESINUSE_DA,      da->numBytesInUse(),
                           EXP_NUM_BYTESINUSE_DA    == da->numBytesInUse());

                ASSERTV(i, EXP1, mX->alloc1(), EXP1 == mX->alloc1());
                ASSERTV(i, EXP2, mX->alloc2(), EXP2 == mX->alloc2());
                ASSERTV(i, EXP3, mX->alloc3(), EXP3 == mX->alloc3());
            }

            const bsls::Types::Int64 EXP_NUM_DEALLOCATIONS_DA =
                                                        numDeallocationsDA + 1;
            ASSERTV(i, EXP_NUM_DEALLOCATIONS_DA,   da->numDeallocations(),
                       EXP_NUM_DEALLOCATIONS_DA == da->numAllocations());
        }
    }
}

}  // close unnamed namespace

//=============================================================================
//                              DRQS 30670366
//-----------------------------------------------------------------------------
namespace DRQS_30670366_NAMESPACE {

void testDeleter(int *expectedCookieValue, void *cookie)
{
    ASSERT(expectedCookieValue == cookie);
}

}  // close namespace DRQS_30670366_NAMESPACE

//=============================================================================
//                              TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

                   g_verbose = verbose;
               g_veryVerbose = veryVerbose;
           g_veryVeryVerbose = veryVeryVerbose;
       g_veryVeryVeryVerbose = veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&da));

    // Confirm no static initialization locked the default allocator
    ASSERT(&da == bslma::Default::defaultAllocator());

    switch (test) { case 0:
      case 20: {
        // --------------------------------------------------------------------
        // TESTING CASTING EXAMPLES
        //
        // Concerns
        //   Test casting of managed pointers, both when the pointer is null
        //   and when it is not.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   CASTING EXAMPLES
        // --------------------------------------------------------------------

        using namespace TYPE_CASTING_TEST_NAMESPACE;

        if (verbose) printf("\nTESTING CASTING EXAMPLES"
                            "\n========================\n");

        int numdels = 0;

        {
            implicitCastingExample();
            explicitCastingExample();

            bool castSucceeded;

            bslma::TestAllocator ta("object", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor tam(&ta);

            processPolymorphicObject(returnManagedPtr(&numdels, &ta),
                                     &castSucceeded);
            ASSERT(!castSucceeded);
            processPolymorphicObject(
                         bslma::ManagedPtr<A>(returnDerivedPtr(&numdels, &ta)),
                        &castSucceeded);
            ASSERT(castSucceeded);
            processPolymorphicObject(
                   bslma::ManagedPtr<A>(returnSecondDerivedPtr(&numdels, &ta)),
                  &castSucceeded);
            ASSERT(!castSucceeded);

            returnManagedPtr(&numdels, &ta);
            returnDerivedPtr(&numdels, &ta);
            returnSecondDerivedPtr(&numdels, &ta);
        }

        ASSERTV(numdels, 20202 == numdels);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        USAGE_EXAMPLES::testShapes();                        // Example 1
        USAGE_EXAMPLES::testShapesToo();                     // Example 1
        USAGE_EXAMPLES::aliasExample();                      // Example 2
        USAGE_EXAMPLES::testCountedFactory();                // Example 3
        USAGE_EXAMPLES::testInplaceCreation();               // Example 5
        USAGE_EXAMPLES::testUsesAllocatorInplaceCreation();  // Example 5

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'makeManaged' and 'allocateManaged'
        //
        // Concerns:
        //: 1 'makeManaged' and 'allocateManaged' return a 'ManagedPtr'
        //:   pointing to a new object of the specified 'T' type, where 'T'
        //:   must be a complete type.
        //:
        //: 2 There is only one allocation to create the managed object (for
        //:   the object's footprint), unless the managed object performs
        //:   additional allocations.
        //:
        //: 3 The appropriate constructor for 'T' is called, forwarding all
        //:   of the supplied arguments to that constructor, in the supplied
        //:   order.
        //:
        //: 4 'makeManaged' unconditionally passes only supplied arguments to
        //:   the constructor of the specified 'T' type without propagating the
        //:   default allocator regardless of whether 'T' uses 'bslma'
        //:   allocators.
        //:
        //: 5 'allocateManaged' passes the supplied allocator as an extra
        //:   argument in the final position if 'T' uses 'bslma' allocators.
        //:
        //: 6 If 0 is supplied to 'allocateManaged' as the allocator then the
        //:   default allocator is used.
        //
        // Plan:
        //: 1 Use 'makeManaged' and 'allocateManaged' to call 15 different
        //:   constructors of the special test type 'AllocEmplacableTestType'
        //:   and supply them with the appropriate arguments.
        //:
        //: 2 Verify that the obtained managed object is initialized using the
        //:   arguments supplied, comparing it with the control object created
        //:   via an explicit constructor call.
        //:
        //: 3 Verify that the memory for the managed object is allocated from
        //:   the appropriate allocator (default for 'makeManaged' and supplied
        //:   for 'allocateManaged').
        //:
        //: 4 Verify the number of memory allocations.  (C-2..3)
        //:
        //: 5 Verify that the default allocator is not propagated by
        //:   'makeManaged' to the constructor of the managed object under any
        //:   circumstances.  (C-4)
        //:
        //: 6 Verify that the supplied allocator is propagated by
        //:   'allocateManaged' to the constructor of the managed object if its
        //:   type uses 'bslma' allocators and is not propagated otherwise
        //:   Also verify that the default allocator is used if 0 is supplied
        //:   as the allocator.  (C-5..6)
        //:
        //: 7 Let the managed pointer go out of scope and verify that all
        //:   allocated memory is successfully released.  (C-1)
        //
        // Testing:
        //   ManagedPtr makeManaged(ARGS&&... args);
        //   ManagedPtr allocateManaged(Allocator *alloc, ARGS&&... args);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'makeManaged' and 'allocateManaged'"
                            "\n===========================================\n");

        // Verifying arguments forwarding.

#if !defined(BSL_DO_NOT_TEST_MOVE_FORWARDING)

        Harness::testCase18_ArgumentsTest<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        // 1 argument

        Harness::testCase18_ArgumentsTest<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        // 2 arguments

        Harness::testCase18_ArgumentsTest<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        // 3 arguments

        Harness::testCase18_ArgumentsTest<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

# if !defined(BSLSTL_MANAGEDPTR_LIMIT_TESTING_COMPLEXITY)

        // 4 arguments

        Harness::testCase18_ArgumentsTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        // 5 arguments

        Harness::testCase18_ArgumentsTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        // 6 arguments

        Harness::testCase18_ArgumentsTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        // 7 arguments

        Harness::testCase18_ArgumentsTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        // 8 arguments

        Harness::testCase18_ArgumentsTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        // 9 arguments

        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        // 10 arguments

        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        // 11 arguments

        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        // 12 arguments

        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        Harness::testCase18_ArgumentsTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        // 13 arguments

        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        Harness::testCase18_ArgumentsTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

# else // BSLSTL_MANAGEDPTR_LIMIT_TESTING_COMPLEXITY)

        // 4 arguments

        Harness::testCase18_ArgumentsTest<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        // 5 arguments

        Harness::testCase18_ArgumentsTest<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        // 6 arguments

        Harness::testCase18_ArgumentsTest<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        // 7 arguments

        Harness::testCase18_ArgumentsTest<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        // 8 arguments

        Harness::testCase18_ArgumentsTest<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        // 9 arguments

        Harness::testCase18_ArgumentsTest<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        // 10 arguments

        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        // 11 arguments

        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        // 12 arguments

        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        // 13 arguments

        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

# endif // BSLSTL_MANAGEDPTR_LIMIT_TESTING_COMPLEXITY)

        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        Harness::testCase18_ArgumentsTest<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();

#else // BSL_DO_NOT_TEST_MOVE_FORWARDING

        Harness::testCase18_ArgumentsTest< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        Harness::testCase18_ArgumentsTest<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        Harness::testCase18_ArgumentsTest<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        Harness::testCase18_ArgumentsTest<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        Harness::testCase18_ArgumentsTest<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();

#endif // BSL_DO_NOT_TEST_MOVE_FORWARDING

        // Check the behavior for the class that does not take an allocator.

        Harness::testCase18_NoAllocTest();

        // Check the behavior for the class that takes only allocators.

        Harness::testCase18_AllocOnlyTest();

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // Concerns:
        // TBD:
        // This test case recreates an issue discovered in client code when
        // building a big with the C++11 development branch.  The plan is to
        // fold this kind of testing into another more relevant test case
        // rather than having a separate one.
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        bslma::ManagedPtr<int> r;
        bslma::ManagedPtr<int> s;
        r = bslmf::MovableRefUtil::move_if_noexcept(s);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // DRQS 30670366
        //
        // Concerns
        //   Swapping a cookie of type 'void *' and a deletion functor of type
        //   'void deleter(DERIVED_TYPE *, void *)' supplies the correct cookie
        //   to the deletion functor.  Note that this test for deprecated
        //   functionality will become redundant and ultimately vanish, as the
        //   deprecated functionality is removed.
        //
        // Plan:
        //   Replicated the .
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        using namespace DRQS_30670366_NAMESPACE;
        if (verbose) printf("\nDRQS 30670366"
                            "\n=============\n");

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        {
            int cookie = 100;
            bslma::ManagedPtr<int> test(&cookie,
                                        static_cast<void *>(&cookie),
                                        &testDeleter);
        }
        {
            int cookie = 100;
            bslma::ManagedPtr<int> test;
            test.load(&cookie, static_cast<void *>(&cookie), &testDeleter);
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'ManagedPtrNilDeleter<TYPE>'
        //
        // Concerns:
        //: 1 The 'deleter' method can be used as a deleter policy by
        //:   'bslma::ManagedPtr'.
        //:
        //: 2 When invoked, 'bslma::ManagedPtrNilDeleter<T>::deleter' has no
        //:   effect.
        //:
        //: 3 No memory is allocated from the global or default allocators.
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   void deleter(void *, void *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'ManagedPtrNilDeleter<TYPE>'"
                            "\n====================================\n");

        if (verbose) printf("\tConfirm the deleter does not destroy the "
                             "passed object\n");

        int deleteCount = 0;
        MyTestObject t(&deleteCount);
        bslma::ManagedPtrNilDeleter<MyTestObject>::deleter(&t, 0);
        ASSERTV(deleteCount, 0 == deleteCount);

        if (verbose) printf("\tConfirm the deleter can be registered with "
                             "a managed pointer\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        int x;
        int y;
        {
            bslma::ManagedPtr<int> p(
                                   &x,
                                    0,
                                   &bslma::ManagedPtrNilDeleter<int>::deleter);
            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());

            p.load(&y, 0, &bslma::ManagedPtrNilDeleter<int>::deleter);
            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());
        }
        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'ManagedPtrUtil'
        //
        // Concerns:
        //: 1 The 'noOpDeleter' method can be used as a deleter policy by
        //:   'bslma::ManagedPtr'.
        //:
        //: 2 When invoked, 'bslma::ManagedPtrUtil::noOpDeleter' has no effect.
        //:
        //: 3 No memory is allocated from the global or default allocators.
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   void noOpDeleter(void *, void *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'ManagedPtrUtil'"
                            "\n========================\n");

        if (verbose) printf("\tConfirm the deleter does not destroy the "
                            "passed object\n");

        int deleteCount = 0;
        MyTestObject t(&deleteCount);
        bslma::ManagedPtrUtil::noOpDeleter(&t, 0);
        ASSERTV(deleteCount, 0 == deleteCount);

        if (verbose) printf("\tConfirm the deleter can be registered with "
                            "a managed pointer\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        int x;
        int y;
        {
            bslma::ManagedPtr<int> p(&x,
                                     0,
                                     &bslma::ManagedPtrUtil::noOpDeleter);
            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());

            p.load(&y, 0, bslma::ManagedPtrUtil::noOpDeleter);
            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());
        }
        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'clear', 'reset', AND 'release'
        //
        // Concerns:
        //: 1 'reset' destroys the managed object (if any) and re-initializes
        //:   the managed pointer to an unset state.
        //:
        //: 2 'reset' destroys any managed object using the stored 'deleter'.
        //:
        //: 3 same concerns as 1 and 2 but with 'clear'.
        //
        //   That release works properly.
        //   Release gives up ownership of resources without running deleters
        //
        //   Test each function behaves correctly given one of the following
        //     kinds of managed pointer objects:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        // Plan:
        //   TBD...
        //
        // Testing:
        //   void clear();
        //   bsl::pair<TYPE *, ManagedPtrDeleter> release();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'clear', 'reset', AND 'release'"
                            "\n=======================================\n");

        using namespace CREATORS_TEST_NAMESPACE;

        int numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(0 == numDeletes);
            o.clear();
            ASSERTV(numDeletes, 1 == numDeletes);

            ASSERT(!o && !o.get());
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(0 == numDeletes);
            o.reset();
            ASSERTV(numDeletes, 1 == numDeletes);

            ASSERT(!o && !o.get());
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            Test *p;
            {
                p = new MyTestObject(&numDeletes);
                Obj o(p);

                ASSERT(p == o.release().first);
                ASSERT(0 == numDeletes);

                ASSERT(!o && !o.get());
            }

            ASSERT(0 == numDeletes);
            delete p;
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        // testing 'release().second'
        numDeletes = 0;
        {
            Test *p;
            {
                p =  new MyTestObject(&numDeletes);
                Obj o(p);

                bslma::ManagedPtrDeleter d(o.deleter());
                bslma::ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            delete p;
        }
        ASSERTV(numDeletes, 1 == numDeletes);

#if 0
        // testing 'deleter' accessor and 'release().second'
        numDeletes = 0;
        {
            Test *p;
            {
                p =  new (da) MyTestObject(&numDeletes);
                Obj o(p);

                bslma::ManagedPtrDeleter d(o.deleter());
                bslma::ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            da.deleteObject(p);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        {
            bsls::Types::Int64 numDeallocations = da.numDeallocations();
            numDeletes = 0;
            {
                SS *p = new (da) SS(&numDeletes);
                std::strcpy(p->d_buf, "Woof meow");

                SSObj s(p);

                // testing * and -> references
                ASSERT(0 == std::strcmp(&(*s).d_buf[5], "meow"));
                ASSERT(0 == std::strcmp(&s->d_buf[5],   "meow"));
            }
            ASSERT(da.numDeallocations() == numDeallocations + 1);
        }
#endif

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //
        // Concerns:
        //   Test swap function and all assignments operators.
        //
        //   (AJM concerns, not yet confirmed to be tested)
        //
        //   assign clears the pointer being assigned from
        //   self-assignment safe
        //   assign destroys held pointer, does not merely swap
        //   assign-with-null
        //   assign with aliased pointer
        //   assign from pointer with factory/deleter
        //   assign to pointer with factory/deleter/aliased-pointer
        //   assign from a compatible managed pointer type
        //      (e.g., ptr-to-derived, to ptr-to-base, ptr to ptr-to-const)
        //   any managed pointer can be assigned to 'bslma::ManagedPtr<void>'
        //   assign to/from an empty managed pointer, each of the cases above
        //   assigning incompatible pointers should fail to compile (hand test)
        //
        //   REFORMULATION
        //   want to be sure assignment works correctly for all combinations of
        //   assigning from and to a managed pointer with each of the following
        //   states.  Similarly, want to swap with each possible combination of
        //   each of the following states:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     simple with null factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  In addition, assignment supports the following that 'swap' does not
        //  assignment from temporary/rvalue must be supported
        //  assignment from 'compatible' managed pointer must be supported
        //    i.e., where raw pointers would be convertible under assignment
        //
        //: X No 'bslma::ManagedPtr' method should allocate any memory.
        // Plan:
        //   TBD...
        //
        //   Test the functions in the order in which they are declared in
        //   the ManagedPtr class.
        //
        // Testing:
        //   ManagedPtr& operator=(ManagedPtr& rhs);
        //   ManagedPtr& operator=(ManagedPtr&& rhs);
        //   ManagedPtr& operator=(MovableRef<ManagedPtr<OTHER>> rhs);
        //   ManagedPtr& operator=(ManagedPtr_Ref<ELEMENT_TYPE> ref);
        //   ManagedPtr& operator=(ManagedPtr<OTHER>&& rhs);
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS"
                            "\n============================\n");

        int numDeletes = 0;
        {
            Obj o;
            Obj o2;
            ASSERT(!o);
            ASSERT(!o2);

            o = o2;

            ASSERT(!o);
            ASSERT(!o2);
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            Obj o;
            Obj o2;
            ASSERT(!o);
            ASSERT(!o2);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o);
            ASSERT(!o2);
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            Obj o;
            ASSERT(!o);

            o = 0;

            ASSERT(!o);
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2;

            o = o2;

            ASSERT(!o);
            ASSERT(!o2);
            ASSERTV(numDeletes, 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2;

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o);
            ASSERT(!o2);
            ASSERTV(numDeletes, 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);

            Obj o(p);

            o = 0;

            ASSERT(!o);
            ASSERTV(numDeletes, 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);

            Obj o;
            Obj o2(p);

            o = o2;

            ASSERT(!o2);
            ASSERTV(numDeletes, 0 == numDeletes);

            ASSERT(o.get() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);

            Obj o;
            Obj o2(p);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes, 0 == numDeletes);

            ASSERT(o.get() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        int numDeletes2 = 0;
        {
            Test *p  = new MyTestObject(&numDeletes);
            Test *p2 = new MyTestObject(&numDeletes2);

            Obj o(p);
            Obj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);

            ASSERT(o.get() == p2);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes2);

        numDeletes  = 0;
        numDeletes2 = 0;
        {
            Test *p  = new MyTestObject(&numDeletes);
            Test *p2 = new MyTestObject(&numDeletes2);

            Obj o(p);
            Obj o2(p2);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);

            ASSERT(o.get() == p2);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes2);

        numDeletes = 0;
        numDeletes2 = 0;
        {
            Test    *p  = new MyTestObject(&numDeletes);
            Derived *p2 = new MyDerivedObject(&numDeletes2);

            Obj o(p);
            DObj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);

            ASSERT(o.get() == p2);
        }
        ASSERT(1 == numDeletes);
        ASSERT(100 == numDeletes2);

        numDeletes  = 0;
        numDeletes2 = 0;
        {
            Test    *p  = new MyTestObject(&numDeletes);
            Derived *p2 = new MyDerivedObject(&numDeletes2);

            Obj o(p);
            DObj o2(p2);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);

            ASSERT(o.get() == p2);
        }
        ASSERT(  1 == numDeletes);
        ASSERT(100 == numDeletes2);

        numDeletes = 0;
        numDeletes2 = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            Obj   o(p);

            o = returnDerivedPtr(&numDeletes2, &da);

            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);
        }
        ASSERT(1 == numDeletes);
        ASSERT(100 == numDeletes2);

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            Obj   o(p);

            o = DObj();

            ASSERTV(numDeletes, 1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        numDeletes2 = 0;
        int numDeletes3 = 0;
        {
            // Assign to moved-from ManagedPtr

            Test           *p  = new MyTestObject(&numDeletes);
            Derived        *p2 = new MyDerivedObject(&numDeletes2);
            DerivedDerived *p3 = new MyDerivedDerivedObject(&numDeletes3);

            Obj    o(p);
            DObj  o2(p2);
            DDObj o3(p3);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);
            ASSERTV(numDeletes3, 0 == numDeletes3);
            ASSERT(o.get() == p2);

            o2 = o3;

            ASSERT(!o3);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);
            ASSERTV(numDeletes3, 0 == numDeletes3);
            ASSERT(o2.get() == p3);
        }
        ASSERT(      1 == numDeletes);
        ASSERT(    100 == numDeletes2);
        ASSERT(1000000 == numDeletes3);

        numDeletes  = 0;
        numDeletes2 = 0;
        numDeletes3 = 0;
        {
            // Move assign to moved-from ManagedPtr

            Test           *p  = new MyTestObject(&numDeletes);
            Derived        *p2 = new MyDerivedObject(&numDeletes2);
            DerivedDerived *p3 = new MyDerivedDerivedObject(&numDeletes3);

            Obj    o(p);
            DObj  o2(p2);
            DDObj o3(p3);

            o = bslmf::MovableRefUtil::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);
            ASSERTV(numDeletes3, 0 == numDeletes3);
            ASSERT(o.get() == p2);

            o2 = bslmf::MovableRefUtil::move(o3);

            ASSERT(!o3);
            ASSERTV(numDeletes,  1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);
            ASSERTV(numDeletes3, 0 == numDeletes3);
            ASSERT(o2.get() == p3);
        }
        ASSERT(      1 == numDeletes);
        ASSERT(    100 == numDeletes2);
        ASSERT(1000000 == numDeletes3);

        numDeletes = 0;
        {
            // Explicitly test move assignment from the same 'ManagedPtr'.

            {
                Test *p = new MyTestObject(&numDeletes);
                Obj o(p);

                o = bslmf::MovableRefUtil::move(o);

                ASSERT(o.get() == p);
            }
            ASSERTV(numDeletes, 1 == numDeletes);
        }

        numDeletes = 0;
        {
            // Test creation of a ref from the same type of 'ManagedPtr', then
            // assignment to another 'ManagedPtr'.

            Obj o2;
            {
                Test *p = new MyTestObject(&numDeletes);
                Obj o(p);

                bslma::ManagedPtr_Ref<Test> r = o;
                o2 = r;

                ASSERT(o2.get() == p);
            }
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2;

            bslma::ManagedPtr_Ref<Test> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);

            bslma::ManagedPtr_Ref<Test> r2 = o;
            o2 = r2;
            ASSERT(!o2);
            ASSERT(!o);

            ASSERTV(numDeletes, 1 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);

            DObj o(p);
            Obj  o2;

            bslma::ManagedPtr_Ref<Test> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        if (verbose) printf(
                "\tTest bslma::ManagedPtr<Base>::operator=" \
                "(ManagedPtr<Derived>&& rhs)\n");

        numDeletes = 0;
        numDeletes2 = 0;
        {
            Test     *p = new MyTestObject(&numDeletes);
            Derived *p2 = new MyDerivedObject(&numDeletes2);

            Obj  o(p);
            DObj o2(p2);

            o = std::move(o2);

            ASSERT(!o2);
            ASSERTV(numDeletes, 1 == numDeletes);
            ASSERTV(numDeletes2, 0 == numDeletes2);

            ASSERT(o.get() == p2);
        }
        ASSERT(1 == numDeletes);
        ASSERT(100 == numDeletes2);

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);

            DObj o(p);
            Obj  o2 = bslmf::MovableRefUtil::move(o);

            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);

            Obj o2 = DObj(p);

            ASSERT(o2);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Obj o2 = returnDerivedPtr(&numDeletes, &da);

            ASSERT(o2);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Obj o2 = DObj();

            ASSERT(!o2);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 0 == numDeletes);
#endif //defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_FROM_DERIVED_TYPE_LVALUE
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_FROM_DERIVED_TYPE_LVALUE)
        // Note: on MSVC this will compile unless the /permissive- flag is
        // specified to disable Microsoft C++ extensions and force
        // conformance to the standard.
        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);

            DObj o(p);
            Obj  o2 = o;

            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);
#endif


//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE)
            {
                bslma::ManagedPtr<int> x;
                bslma::ManagedPtr<double> y;
                y = x;  // This should fail to compile.
            }
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //   Test swap functions.
        //
        //   (AJM concerns, not yet confirmed to be tested)
        //
        //   assign clears the pointer being assigned from
        //   self-assignment safe
        //   assign destroys held pointer, does not merely swap
        //   assign-with-null
        //   assign with aliased pointer
        //   assign from pointer with factory/deleter
        //   assign to pointer with factory/deleter/aliased-pointer
        //   assign from a compatible managed pointer type
        //      (e.g., ptr-to-derived, to ptr-to-base, ptr to ptr-to-const)
        //   any managed pointer can be assigned to 'bslma::ManagedPtr<void>'
        //   assign to/from an empty managed pointer, each of the cases above
        //   assigning incompatible pointers should fail to compile (hand test)
        //
        //   swap with self changes nothing
        //   swap two simple pointer exchanged pointer values
        //   swap two aliased pointer exchanges aliases as well as pointers
        //   swap a simple managed pointer with an empty managed pointer
        //   swap a simple managed pointer with an aliased managed pointer
        //   swap an aliased managed pointer with an empty managed pointer
        //
        //   REFORMULATION
        //   want to be sure assignment works correctly for all combinations of
        //   assigning from and to a managed pointer with each of the following
        //   states.  Similarly, want to swap with each possible combination of
        //   each of the following states:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     simple with null factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //: X No 'bslma::ManagedPtr' method should allocate any memory.
        // Plan:
        //   TBD...
        //
        //   Test the functions in the order in which they are declared in
        //   the ManagedPtr class.
        //
        // Testing:
        //   void swap(ManagedPtr& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SWAP"
                            "\n============\n");

        using namespace CREATORS_TEST_NAMESPACE;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                       "\tTest bslma::ManagedPtr::swap(bslma::ManagedPtr&)\n");

        int numDeletes = 0;
        {
            Test *p  = new MyTestObject(&numDeletes);
            Test *p2 = new MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2(p2);

            o.swap(o2);

            ASSERT(o.get()  == p2);
            ASSERT(o2.get() == p);
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 2 == numDeletes);

        if (verbose) printf("\t\tswap with empty managed pointer\n");

        numDeletes = 0;
        {
            Test *p =  new MyTestObject(&numDeletes);
            Obj o(p);
            Obj o2;

            o.swap(o2);

            ASSERT(!o.get());
            ASSERT(o2.get() == p);
            ASSERTV(numDeletes, 0 == numDeletes);

            o.swap(o2);

            ASSERT(o.get() == p);
            ASSERT(!o2.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\t\tswap deleters\n");

        numDeletes = 0;
        {
            bslma::TestAllocator ta1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator ta2("object2", veryVeryVeryVerbose);

            Test *p  = new (ta1) MyTestObject(&numDeletes);
            Test *p2 = new (ta2) MyTestObject(&numDeletes);

            Obj o(p, &ta1);
            Obj o2(p2, &ta2);

            o.swap(o2);

            ASSERT(o.get()  == p2);
            ASSERT(o2.get() == p);

            ASSERT(&ta2 == o.deleter().factory());
            ASSERT(&ta1 == o2.deleter().factory());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 2 == numDeletes);

        if (verbose) printf("\t\tswap aliases\n");

        numDeletes = 0;
        {
            bslma::TestAllocator ta1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator ta2("object2", veryVeryVeryVerbose);

            int *p3 = new (ta2) int;
            *p3 = 42;

            Test *p =  new (ta1) MyTestObject(&numDeletes);
            MyDerivedObject d2(&numDeletes);

            bslma::ManagedPtr<int> o3(p3, &ta2);
            Obj o(p, &ta1);
            Obj o2(o3, &d2);

            o.swap(o2);

            ASSERT( o.get() == &d2);
            ASSERT(o2.get() ==   p);

            ASSERT(  p3 ==  o.deleter().object());
            ASSERT(   p == o2.deleter().object());
            ASSERT(&ta2 ==  o.deleter().factory());
            ASSERT(&ta1 == o2.deleter().factory());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 101 == numDeletes);

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES)
            {
                // confirm that the various implicit conversions in this
                // component do not accidentally introduce a dangerous 'swap'.
                bslma::ManagedPtr<int> x;
                bslma::ManagedPtr<double> y;
                x.swap(y);  // should not compile
                y.swap(x);  // should not compile

                bslma::ManagedPtr<MyTestObject> b;
                bslma::ManagedPtr<MyDerivedObject> d;
                b.swap(d);  // should not compile
                d.swap(b);  // should not compile

                using std::swap;
                swap(x, y);  // should not compile
                swap(b, d);  // should not compile
            }
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // UNUSED TEST CASE NUMBER
        //
        // Concerns:
        //  None, this test case is available to be recycled.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\nUNUSED TEST CASE NUMBER"
                            "\n=======================\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MOVE-CONSTRUCTION
        //
        // Concerns:
        //: 1 No constructor nor conversion operator allocates any memory from
        //:   the default or global allocators.
        //:
        //: 2 Each constructor takes ownership of the passed managed object.
        //:
        //: 3 Move construction for lvalues directly invokes the copy/move-
        //:   constructor (or constructor template) to take ownership away from
        //:   the source managed pointer object.
        //:
        //: 4 Move semantics for temporary objects (rvalues) are supported
        //:   through an implicit conversion to 'bslma::ManagedPtr_Ref', and
        //:   the single argument (implicit) constructor taking ownership from
        //:   such a managed reference.
        //:
        //: 5 const-qualified objects cannot be moved from (compile-fail test).
        //:
        //: 6 Both lvalue and rvalue objects of 'bslma::ManagedPtr' types can
        //:   implicitly convert to a 'bslma::ManagedPtr_Ref' of any compatible
        //:   type, i.e., where a pointer to the specified '_Ref' type may be
        //:   converted from a pointer to the specified 'Managed' type.
        //:
        //: 7 A 'bslma::ManagedPtr' object is left in an empty state after
        //:   being supplied as the source to a move operation.
        //:
        //: 8 A 'bslma::ManagedPtr' object holding an object of a most-derived
        //:   class can correctly move to a 'bslma::ManagedPtr' holding one of
        //:   its non-leftmost base classes.
        //
        // Plan:
        //   First we test the conversion operator, including compile-fail test
        //   for incompatible types
        //
        //   Next we test construction from a 'bslma::ManagedPtr_Ref' object
        //
        //   Finally we test the tricky combinations of invoking the (lvalue)
        //   move constructor, including with rvalues, and values of different
        //   target types.
        //
        // Testing:
        //   ManagedPtr(ManagedPtr& original);
        //   ManagedPtr(ManagedPtr&& original);
        //   ManagedPtr(MovableRef<ManagedPtr<OTHER>> original);
        //   ManagedPtr(ManagedPtr_Ref<TYPE> ref);
        //   operator ManagedPtr_Ref<BDE_OTHER_TYPE>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE-CONSTRUCTION"
                            "\n=========================\n");

        using namespace CREATORS_TEST_NAMESPACE;

        bslma::TestAllocator ta("object", veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose)
                    printf("\tTest operator bslma::ManagedPtr_Ref<OTHER>()\n");

        ASSERTV(g_deleteCount, 0 == g_deleteCount);
        {
            bslma::TestAllocatorMonitor gam(&globalAllocator);
            bslma::TestAllocatorMonitor dam(&da);

            int numDeletes = 0;

            {
                Test x(&numDeletes);
                Obj  o(&x, 0, countedNilDelete);

                bslma::ManagedPtr_Ref<Test> r = o;
                // Check no memory is allocated/released and no deleters run
                ASSERTV(g_deleteCount, 0 == g_deleteCount);
                ASSERT(0 == numDeletes);

                // check the pointer reference an object with the correct data
                ASSERT(&x == r.base()->pointer());
                ASSERT(&x == r.base()->deleter().object());
                ASSERT(0 == r.base()->deleter().factory());
                ASSERT(&countedNilDelete == r.base()->deleter().deleter());

                // finally, check the address of the pointed-to object lies
                // within 'o', as we cannot directly query the address of the
                // private member
                const void *p1 = &o;
                const void *p2 = reinterpret_cast<const unsigned char *>(p1) +
                                                                     sizeof(o);
                const void *pRef = r.base();
                ASSERTV(p1, pRef, p2, p1 <= pRef && pRef < p2);
            }
            ASSERTV(g_deleteCount, 1 == g_deleteCount);
            ASSERTV(numDeletes,    1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

            g_deleteCount = 0;
            numDeletes    = 0;
            {
                // To test conversion from an rvalue, we must bind the
                // temporary to a function argument in order to prolong the
                // lifetime of the temporary until after testing is complete.
                // We must bind the temporary to a 'bslma::ManagedPtr_Ref' and
                // not a whole 'bslma::ManagedPtr' because we are testing an
                // implementation detail of that move-constructor that would be
                // invoked.
                struct Local {
                    static void test(void                        *px,
                                     bslma::ManagedPtr_Ref<Test>  r)
                    {
                        ASSERTV(g_deleteCount, 0 == g_deleteCount);

                        ASSERT(px == r.base()->pointer());
                        ASSERT(px == r.base()->deleter().object());
                        ASSERT( 0 == r.base()->deleter().factory());
                        ASSERT(&countedNilDelete ==
                                                r.base()->deleter().deleter());
                    }
                };

                Test x(&numDeletes);
                Local::test( &x, (Obj(&x, 0, countedNilDelete)));
            }
            ASSERTV(g_deleteCount, 1 == g_deleteCount);
            ASSERTV(numDeletes,    1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST)
            {
                Test x(&numDeletes);
                const Obj o(&x, 0, countedNilDelete);

                bslma::ManagedPtr_Ref<Test> r = o;   // should not compile
                ASSERTV(g_deleteCount, 0 == g_deleteCount);
                ASSERT(0 == numDeletes);
            }
#endif
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tbslma::ManagedPtr(bslma::ManagedPtr& donor)\n");

        {
            bslma::TestAllocatorMonitor gam(&globalAllocator);
            bslma::TestAllocatorMonitor dam(&da);

            g_deleteCount  = 0;
            int numDeletes = 0;
            {
                Test x(&numDeletes);
                Obj  o(&x, 0, countedNilDelete);
                ASSERT(&x == o.get());

                Obj o2(o);
                ASSERT( 0 ==  o.get());
                ASSERT(&x == o2.get());
                ASSERT(&x == o2.deleter().object());
                ASSERT( 0 == o2.deleter().factory());
                ASSERT(&countedNilDelete == o2.deleter().deleter());
            }

            ASSERTV(g_deleteCount, 1 == g_deleteCount);
            ASSERTV(numDeletes,    1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

            g_deleteCount = 0;
            numDeletes    = 0;
            {
                Test x(&numDeletes);
                Obj  o = Obj(&x, 0, countedNilDelete);
                ASSERT(&x == o.get());

                Obj o2(o);
                ASSERT( 0 ==  o.get());
                ASSERT(&x == o2.get());
                ASSERT(&x == o2.deleter().object());
                ASSERT( 0 == o2.deleter().factory());
                ASSERT(&countedNilDelete == o2.deleter().deleter());
            }

            ASSERTV(g_deleteCount, 1 == g_deleteCount);
            ASSERTV(numDeletes,    1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST)
            {
                Test x(&numDeletes);
                const Obj  o(&x, 0, countedNilDelete);
                ASSERT(&X == o.get());

                Obj o2(o);  // should not compile
                ASSERT(!"The preceding line should not have compiled");
            }
#endif
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tManagedPtr(MovableRef<ManagedPtr> donor)\n");

        {
            bslma::TestAllocatorMonitor gam(&globalAllocator);
            bslma::TestAllocatorMonitor dam(&da);

            g_deleteCount  = 0;
            int numDeletes = 0;
            {
                Test x(&numDeletes);
                Obj  o(&x, 0, countedNilDelete);
                ASSERT(&x == o.get());

                Obj o2(bslmf::MovableRefUtil::move(o));
                ASSERT( 0 ==  o.get());
                ASSERT(&x == o2.get());
                ASSERT(&x == o2.deleter().object());
                ASSERT( 0 == o2.deleter().factory());
                ASSERT(&countedNilDelete == o2.deleter().deleter());
            }

            ASSERTV(g_deleteCount, 1 == g_deleteCount);
            ASSERTV(numDeletes,    1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

            g_deleteCount = 0;
            numDeletes    = 0;
            {
                Obj o;
                ASSERT(0 == o.get());

                Obj o2(bslmf::MovableRefUtil::move(o));
                ASSERT(0 == o.get());
                ASSERT(0 == o2.get());
            }
            ASSERTV(g_deleteCount, 0 == g_deleteCount);
            ASSERTV(numDeletes,    0 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
        "\tTest bslma::ManagedPtr(bslma::ManagedPtr_Ref<ELEMENT_TYPE> ref)\n");

        int numDeletes = 0;
        {
            // This cast tests both a cast while creating the ref, and the
            // constructor from a ref.

            Derived *p = new MyDerivedObject(&numDeletes);
            DObj o(p);

            ASSERT(o);
            ASSERT(o.get() == p);

            bslma::ManagedPtr_Ref<Test> r = o;
            ASSERT(o);
            Obj o2(r);

            ASSERT(!o && !o.get());
            ASSERT(0 == numDeletes);

            ASSERT(o2.get() == p);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.get() == p);

            Obj o(d);
            ASSERT(o.get() == p);
            ASSERT(0 == d.get());
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);

            DObj d(p);
            ASSERT(d.get() == p);

            Obj o(bslmf::MovableRefUtil::move(d));
            ASSERT(o.get() == p);
            ASSERT(0 == d.get());
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Obj o(returnDerivedPtr(&numDeletes, &da));
            ASSERT(o.get());
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Obj o((DObj())); // Avoid vexing parse.
            ASSERT(!o && !o.get());
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 0 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);

            DObj d(p);
            ASSERT(d.get() == p);

            consumeManagedPtrCR(bslmf::MovableRefUtil::move(d),
                                &numDeletes, 0);

            ASSERT(0 == d.get());
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

        numDeletes = 0;
        {
            Obj o(bslmf::MovableRefUtil::move(
                returnDerivedPtr(&numDeletes, &da)));

            ASSERT(o.get());
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);

            DObj d(p);
            ASSERT(d.get() == p);

            consumeManagedPtr(bslmf::MovableRefUtil::move(d),
                              &numDeletes, 0);

            ASSERT(0 == d.get());
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            consumeManagedPtr(returnDerivedPtr(&numDeletes, &da),
                              &numDeletes, 0);
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);

            consumeManagedPtr(DObj(p), &numDeletes, 0);
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            consumeManagedPtr(DObj(), &numDeletes, 0);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 0 == numDeletes);

        numDeletes = 0;
        {
            consumeManagedPtrCR(returnDerivedPtr(&numDeletes, &da),
                                &numDeletes, 0);
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);

            consumeManagedPtrCR(DObj(p), &numDeletes, 0);
            ASSERT(100 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            consumeManagedPtrCR(DObj(), &numDeletes, 0);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 0 == numDeletes);
#endif //defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_FROM_DERIVED_TYPE_LVALUE
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_FROM_DERIVED_TYPE_LVALUE)
        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.get() == p);

            consumeManagedPtr(d, &numDeletes, 0);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            Derived* p = new MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.get() == p);

            consumeManagedPtrCR(d, &numDeletes, 0);
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_WITH_EXTRA_PTR
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_MOVE_WITH_EXTRA_PTR)
        {
            Derived* dp = new MyDerivedObject(&numDeletes);

            DObj dmp(dp);

            void *vp = 0;

            Obj  test1(bslmf::MovableRefUtil::move(dmp), vp);
            DObj test2(bslmf::MovableRefUtil::move(dmp), vp);
        }
#endif

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tTesting moving to non-leftmost base\n");
        {
            CompositeInt3 derived;

            bslma::ManagedPtr<CompositeInt3> pD(
                                          &derived,
                                          0,
                                          &bslma::ManagedPtrUtil::noOpDeleter);

            int testVal  = pD->data();
            int testVal2 = pD->data2();
            ASSERTV(testVal,    3 == testVal);  // pD->data()
            ASSERTV(testVal2,   6 == testVal2); // pD->data2()
            ASSERTV(pD->d_data, 3 == pD->d_data);

            bslma::ManagedPtr<BaseInt2> pB(pD);  // cannot use '=' form
            ASSERT(0 == pD.get());

            testVal  = pB->data();
            testVal2 = pB->data2();
            ASSERTV(testVal,    3 == testVal);  // pB->data()
            ASSERTV(testVal2,   6 == testVal2); // pB->data2()
            ASSERTV(pB->d_data, 2 == pB->d_data);

            // After testing construction, test assignment
            bslma::ManagedPtr<CompositeInt3> pD2(
                                          &derived,
                                          0,
                                          &bslma::ManagedPtrUtil::noOpDeleter);
            // sanity checks only
            testVal  = pD2->data();
            testVal2 = pD2->data2();
            ASSERTV(testVal,     3 == testVal);  // pD2->data()
            ASSERTV(testVal2,    6 == testVal2); // pD2->data2()
            ASSERTV(pD2->d_data, 3 == pD2->d_data);

            pB = pD2;
            ASSERT(0 == pD2.get());

            testVal  = pB->data();
            testVal2 = pB->data2();
            ASSERTV(testVal,    3 == testVal);  // pB->data()
            ASSERTV(testVal2,   6 == testVal2); // pB->data2()
            ASSERTV(pB->d_data, 2 == pB->d_data);

            // After testing construction/assignment, move construction
            bslma::ManagedPtr<CompositeInt3> pD3(
                                          &derived,
                                          0,
                                          &bslma::ManagedPtrUtil::noOpDeleter);
            // sanity checks only
            testVal  = pD3->data();
            testVal2 = pD3->data2();
            ASSERTV(testVal,     3 == testVal);  // pD2->data()
            ASSERTV(testVal2,    6 == testVal2); // pD2->data2()
            ASSERTV(pD3->d_data, 3 == pD3->d_data);

            bslma::ManagedPtr<BaseInt2> pB2(
                     bslmf::MovableRefUtil::move(pD3));  // cannot use '=' form
            ASSERT(0 == pD3.get());

            testVal  = pB2->data();
            testVal2 = pB2->data2();
            ASSERTV(testVal,    3 == testVal);  // pB2->data()
            ASSERTV(testVal2,   6 == testVal2); // pB2->data2()
            ASSERTV(pB2->d_data, 2 == pB2->d_data);

            // After testing move construction, test move assignment
            bslma::ManagedPtr<CompositeInt3> pD4(
                                          &derived,
                                          0,
                                          &bslma::ManagedPtrUtil::noOpDeleter);
            // sanity checks only
            testVal  = pD4->data();
            testVal2 = pD4->data2();
            ASSERTV(testVal,     3 == testVal);  // pD2->data()
            ASSERTV(testVal2,    6 == testVal2); // pD2->data2()
            ASSERTV(pD4->d_data, 3 == pD4->d_data);

            pB2 = bslmf::MovableRefUtil::move(pD4);
            ASSERT(0 == pD4.get());

            testVal  = pB2->data();
            testVal2 = pB2->data2();
            ASSERTV(testVal,    3 == testVal);  // pB->data()
            ASSERTV(testVal2,   6 == testVal2); // pB->data2()
            ASSERTV(pB2->d_data, 2 == pB2->d_data);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// examples to demonstrate:
        // Moving from lvalues:
        //   derived->base
        //   no-cv -> 'const'
        //   anything -> 'void'
        //
        // Moving from rvalues:
        //   as above, plus...
        //   rvalue of same type

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
        {
            int x;
            bslma::ManagedPtr<int> i_ptr(&x, 0, &countedNilDelete);
            bslma::ManagedPtr<double> d_ptr(i_ptr);

            struct Local_factory {
                static bslma::ManagedPtr<double> exec()
                {
                    return bslma::ManagedPtr<double>();
                }
            };

            bslma::ManagedPtr<long> l_ptr(Local_factory::exec());
        }

        // Additional failures to demonstrate
        //   base -> derived type (a likely user error)
        //   'const' -> non-'const'
        //   'void' -> anything but 'void'
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CREATORS WITH FACTORY OR DELETER
        //
        // Concerns:
        //: 1 No constructor allocates any memory from the default or global
        //:   allocators.
        //:
        //: 2 Each constructor takes ownership of the passed managed object.
        //:
        //: 3 Each constructor establishes the supplied 'deleter', unless the
        //:   specified managed object has a null pointer value.
        //:
        //: 4 Each constructor ASSERTs in appropriate build modes when passed
        //:   a null pointer for the deleter, but a non-null pointer to the
        //:   managed object.
        //:
        //: 5 It must be possible to pass a null-pointer constant for the
        //:   'factory' argument when the specified 'deleter' will use only the
        //:   managed pointer value.
        //
        //   Exercise each declared constructors of ManagedPtr (other than
        //   those already tested in an earlier test case; those constructors
        //   that implement move semantics; and the constructor that enables
        //   aliasing).  Note that the primary accessor, 'ptr', cannot be
        //   considered to be validated until after testing the alias support,
        //   see test case 11.
        //
        // Plan:
        //   TBD...
        //
        //   Go through the constructors in the order in which they are
        //   declared in the ManagedPtr class and exercise all of them.
        //
        //   Remember to pass '0' as a null-pointer literal to all arguments
        //   that accept pointers (with negative testing if that is out of
        //   contract).
        //
        // Testing:
        //   ManagedPtr(OTHER *ptr);
        //   ManagedPtr(OTHER *ptr, FACTORY *factory);
        //   ManagedPtr(TYPE *ptr, void *cookie, DeleterFunc deleter);
        //   ManagedPtr(TYPE *ptr, void *cookie, void(*deleter)(TYPE*, void*));
        //   ManagedPtr(OTHER *ptr, void *cookie, DeleterFunc deleter);
        //   ManagedPtr(OTHER *, bsl::nullptr_t, void(*)(BASE *, void *));
        //   ManagedPtr(OTHER *, COOKIE *, void (*)(OTHER_BASE*, COOKIE_BASE*))
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CREATORS WITH FACTORY OR DELETER"
                            "\n========================================\n");

        {
            if (veryVerbose)
                     printf("Testing bslma::ManagedPtr<MyTestObject> ctors\n");

            testConstructors(L_, TEST_POLICY_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
               printf("Testing bslma::ManagedPtr<const MyTestObject> ctors\n");

            testConstructors(L_, TEST_POLICY_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                  printf("Testing bslma::ManagedPtr<MyDerivedObject> ctors\n");

            testConstructors(L_, TEST_POLICY_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                            printf("Testing bslma::ManagedPtr<void>::ctors\n");

            testConstructors(L_, TEST_POLICY_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                       printf("Testing bslma::ManagedPtr<const void> ctors\n");

            testConstructors(L_, TEST_POLICY_CONST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE)
        {
            if (veryVerbose) printf("Testing bslma::ManagedPtr<Base> ctors\n");

            testConstructors(L_, TEST_POLICY_BASE0_ARRAY);
        }
#endif
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                            printf("Testing bslma::ManagedPtr<Base2> ctors\n");

            testConstructors(L_, TEST_POLICY_BASE2_ARRAY);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        using namespace CREATORS_TEST_NAMESPACE;

        bslma::TestAllocator ta("object", veryVeryVeryVerbose);

        if (verbose) printf("\tTest valid pointer passed to void*\n");

        int numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tconst-qualified int\n");

            bslma::TestAllocatorMonitor dam(&da);
            {
                bslma::TestAllocatorMonitor dam2(&da);

                const int *p = new const int(0);
                bslma::ManagedPtr<const int> o(p);

                ASSERT(o.get() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(!dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tint -> const int conversion\n");

            bslma::TestAllocatorMonitor dam(&da);
            {
                bslma::TestAllocatorMonitor dam2(&da);

                int *p = new int;
                bslma::ManagedPtr<const int> o(p);

                ASSERT(o.get() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(!dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose) printf("\tTest bslma::ManagedPtr(ELEMENT_TYPE *ptr,"
                             " bsl::nullptr_t,"
                             " void(*)(ELEMENT_TYPE *, void*));\n");

        numDeletes = 0;
        ASSERTV(g_deleteCount, 0 == g_deleteCount);
        {
            bslma::TestAllocatorMonitor tam(&ta);

            MyTestObject obj(&numDeletes);
            Obj o(&obj, 0, &templateNilDelete<MyTestObject>);
        }
        ASSERTV(numDeletes,    1 == numDeletes);
        ASSERTV(g_deleteCount, 1 == g_deleteCount);
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
        // This segment of the test case examines the quality of compiler
        // diagnostics when trying to create a 'bslma::ManagedPtr' object with
        // a pointer that it not convertible to a pointer of the type that the
        // smart pointer is managing.

        if (verbose) printf("\tTesting compiler diagnostics*\n");

        // distinct, unrelated types
        numDeletes = 0;
        {
            double *p = new (da) double;
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        // const-conversion
        numDeletes = 0;
        {
            const MyTestObject *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            const MyTestObject *p = new (da) MyTestObject(&numDeletes);
            VObj o(p);

            ASSERT(o.get() == p);
        }
        ASSERTV(numDeletes, 1 == numDeletes);
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY)
        {
            int i = 0;
            bslma::ManagedPtr<int> x(&i, 0);
            bslma::ManagedPtr<int> y( 0, 0);

            bslma::Allocator *pNullAlloc = 0;
            bslma::ManagedPtr<int> z(0, pNullAlloc);
        }
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY)
        {
            int *i = 0;
            bslma::ManagedPtr<const int> x(&i, 0);
            bslma::ManagedPtr<int> y( 0, 0);  // allow this?

            bslma::Allocator *pNullAlloc = 0;
            bslma::ManagedPtr<const int> z(0, pNullAlloc);  // allow this?
        }
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER)
        {
            int *i = 0;
            bslma::ManagedPtr<const int> x(i, 0, 0);
            bslma::ManagedPtr<const int> y(0, 0, 0);  // allow this?

            // These are currently runtime (UB) failures, rather than
            // compile-time errors.
            bslma::Allocator *pNullAlloc = 0;
            bslma::ManagedPtr<const int> z( i, pNullAlloc, 0);
            bslma::ManagedPtr<const int> zz(0, pNullAlloc, 0);  // allow this?
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   That all accessors work properly.  The 'ptr' and 'get' accessors
        //   have already been substantially tested in previous tests.
        //   The unspecified bool conversion evaluates as expected in all
        //     circumstances: if/while/for, (implied) operator!
        //   All accessors work on 'const'- qualified objects
        //   All accessors can be called for 'bslma::ManagedPtr<void>'
        //   All accessors return expected values when a 'bslma::ManagedPtr'
        //     has been aliased
        //   'operator*' should assert in SAFE builds for empty pointers
        //   'deleter' should assert in SAFE builds for empty pointers
        //   'operator*' should be well-formed, but not callable for
        //     'bslma::ManagedPtr<void>'.
        //
        //: X No 'bslma::ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   Test each accessor for the expected value on each of the following
        //   cases:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     simple with null factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  For 'bslma::ManagedPtr<void>', test syntax of 'operator*' in an
        //    unevaluated context, such as a 'typeid' expression.
        //
        //  Test that illegal expressions cannot compile in compile-fail tests,
        //  guarded by '#ifdef's, where necessary.
        //
        // Testing:
        //   operator BoolType() const;
        //   TYPE& operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   TYPE *get() const;
        //   const ManagedPtrDeleter& deleter() const;
        //   (implicit) bool operator!() const;  // via operator BoolType()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        bslma::TestAllocator ta("object", veryVeryVeryVerbose);

        if (verbose) printf("\tTest accessors on empty object\n");

        int numDeletes = 0;
        {
            const Obj o;
            const bslma::ManagedPtrDeleter del;

            validateManagedState(L_, o, 0, del);
        }

        ASSERTV(numDeletes, 0 == numDeletes);
        {
            const VObj o;
            const bslma::ManagedPtrDeleter del;

            validateManagedState(L_, o, 0, del);
            // The following 'typeid' fails on Unix compilers, but should be an
            // unevaluated operand, and so safely invokable.
            //typeid(*o); // should parse, even if it cannot be called
        }

        ASSERTV(numDeletes, 0 == numDeletes);
        {
            const bslma::ManagedPtr<const void> o(0);
            const bslma::ManagedPtrDeleter del;

            validateManagedState(L_, o, 0, del);
            // The following 'typeid' fails on Unix compilers, but should be an
            // unevaluated operand, and so safely invokable.
            //typeid(*o); // should parse, even if it cannot be called
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tTest accessors on simple object\n");

        typedef bslma::ManagedPtr_FactoryDeleter<Test, bslma::Allocator>
                                                        TestcaseFactoryDeleter;
        ASSERTV(numDeletes, 0 == numDeletes);
        {
            Obj o;
            Test *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &da,
                                             &TestcaseFactoryDeleter::deleter);

            validateManagedState(L_, o, p, del);

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
        }
        ASSERTV(numDeletes, 101 == numDeletes);

        numDeletes = 0;
        {
            VObj o;
            Test *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &da,
                                             &TestcaseFactoryDeleter::deleter);

            validateManagedState(L_, o, p, del);

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
        }
        ASSERTV(numDeletes, 101 == numDeletes);

        numDeletes = 0;
        {
            bslma::ManagedPtr<const void> o;
            Test *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &da,
                                             &TestcaseFactoryDeleter::deleter);

            validateManagedState(L_, o, p, del);

            bslma::ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
        }
        ASSERTV(numDeletes, 101 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                        "\tTest accessors on simple object using a factory\n");

        bslma::TestAllocatorMonitor tam(&ta);
        numDeletes = 0;
        {
            Obj o;
            Test *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &ta,
                                             &TestcaseFactoryDeleter::deleter);

            bslma::TestAllocatorMonitor tam2(&ta);

            validateManagedState(L_, o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        ASSERTV(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        numDeletes = 0;
        {
            VObj o;
            Test *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &ta,
                                             &TestcaseFactoryDeleter::deleter);

            bslma::TestAllocatorMonitor tam2(&ta);

            validateManagedState(L_, o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        ASSERTV(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        numDeletes = 0;
        {
            bslma::ManagedPtr<const void> o;
            Test *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bslma::ManagedPtrDeleter del(
                                              p,
                                             &ta,
                                             &TestcaseFactoryDeleter::deleter);

            bslma::TestAllocatorMonitor tam2(&ta);

            validateManagedState(L_, o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            bslma::ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        ASSERTV(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
         "\tTest accessors on simple object using a deleter but no factory\n");

        g_deleteCount = 0;
        numDeletes    = 0;
        {
            Obj o;
            Test obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bslma::ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(L_, o, &obj, del);

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes,  100 == numDeletes);
            ASSERTV(g_deleteCount, 0 == g_deleteCount);
        }
        ASSERTV(numDeletes,  101 == numDeletes);
        ASSERTV(g_deleteCount, 1 == g_deleteCount);

        g_deleteCount = 0;
        numDeletes    = 0;
        {
            VObj o;
            Test obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bslma::ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(L_, o, &obj, del);

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes,  100 == numDeletes);
            ASSERTV(g_deleteCount, 0 == g_deleteCount);
        }
        ASSERTV(numDeletes,  101 == numDeletes);
        ASSERTV(g_deleteCount, 1 == g_deleteCount);

        g_deleteCount = 0;
        numDeletes    = 0;
        {
            bslma::ManagedPtr<const void> o;
            Test obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bslma::ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(L_, o, &obj, del);

            bslma::ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(L_, oD, &d, del);
            }
            ASSERTV(numDeletes,  100 == numDeletes);
            ASSERTV(g_deleteCount, 0 == g_deleteCount);
        }
        ASSERTV(numDeletes,  101 == numDeletes);
        ASSERTV(g_deleteCount, 1 == g_deleteCount);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// TBD Double check that the non-deprecated form is tested, otherwise correct
// this test to use non-deprecated functions.

        if (verbose) printf(
       "\tTest accessors on simple object using both a factory and deleter\n");

        // Declare a local variable of the correct deleter type to avoid
        // repeated casting when trying to pass 'incrementIntDeleter' as a
        // deleter.

        bslma::ManagedPtrDeleter::Deleter fn_incrementIntDeleter_p =
                           reinterpret_cast<bslma::ManagedPtrDeleter::Deleter>(
                                                         &incrementIntDeleter);
        numDeletes = 0;
        {
            bslma::ManagedPtr<int> o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bslma::ManagedPtrDeleter del(&numDeletes,
                                               &factory,
                                                fn_incrementIntDeleter_p);

            validateManagedState(L_, o, &numDeletes, del);

            bslma::ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(L_, o2, &i2, del);
            }
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            VObj o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bslma::ManagedPtrDeleter del(&numDeletes,
                                               &factory,
                                                fn_incrementIntDeleter_p);

            validateManagedState(L_, o, &numDeletes, del);

            bslma::ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(L_, o2, &i2, del);
            }
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            bslma::ManagedPtr<const void> o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bslma::ManagedPtrDeleter del(&numDeletes,
                                               &factory,
                                                fn_incrementIntDeleter_p);

            validateManagedState(L_, o, &numDeletes, del);

            bslma::ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(L_, o2, &i2, del);
            }
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
            {
                int x;
                VObj p(&x);
                *p;

                bslma::ManagedPtr<const void> p2(&x);
                *p2;
            }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ALIAS SUPPORT
        //
        // Concerns:
        //   managed pointer can hold an alias
        //
        //   'ptr' returns the alias pointer, and not the managed pointer
        //
        //   correct deleter is run when an aliased pointer is destroyed
        //
        //   appropriate object is cleared/deleters run when assigning to/from
        //   an aliased managed pointer
        //
        //   a managed pointer can alias itself
        //
        //   alias type need not be the same as the managed type (often isn't)
        //
        //   aliasing a null pointer clears the managed pointer, releasing any
        //   previously held object
        //
        //: X No 'bslma::ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   Alias an existing state:
        //     Run through the function table for test case 'load'
        //     Test 1:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //         negative test alias with a null pointer value
        //         negative test if aliased managed pointer is empty
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 2:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //       call 'loadAlias' again on a third empty managed pointer
        //       Check new aliased state, and first aliased managed pointer
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 3: (to be written)
        //       Create an alias
        //       Check aliased state, and original managed pointer
        //       run another 'load' function and check alias destroys correctly
        //       destroy 'load'ed managed pointer, validating results
        //
        // Testing:
        //   ManagedPtr(ManagedPtr<OTHER>& alias, TYPE *ptr);
        //   ManagedPtr(ManagedPtr<OTHER>&& alias, TYPE *ptr);
        //   void loadAlias(ManagedPtr<OTHER>& alias, TYPE *ptr);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALIAS SUPPORT"
                            "\n=====================\n");

        if (veryVerbose)
                printf("Testing bslma::ManagedPtr<MyTestObject>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_BASE_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_BASE_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) printf(
                 "Testing bslma::ManagedPtr<const MyTestObject>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_CONST_BASE_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_CONST_BASE_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) printf(
                    "Testing bslma::ManagedPtr<MyDerivedObject>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_DERIVED_ARRAY);
            //testLoadAliasOps2(L_, TEST_POLICY_DERIVED_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
                        printf("Testing bslma::ManagedPtr<void>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_VOID_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_VOID_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
                  printf("Testing bslma::ManagedPtr<const void>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_CONST_VOID_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_CONST_VOID_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_CONST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE)
        if (veryVerbose)
                        printf("Testing bslma::ManagedPtr<Base>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_BASE0_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_BASE0_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_BASE0_ARRAY);
        }
#endif

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
                       printf("Testing bslma::ManagedPtr<Base2>::loadAlias\n");
        {

            testLoadAliasOps1(L_, TEST_POLICY_BASE2_ARRAY);
            testLoadAliasOps2(L_, TEST_POLICY_BASE2_ARRAY);
            testLoadAliasOps3(L_, TEST_POLICY_BASE2_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose)
            printf("\tManagedPtr(ManagedPtr<OTHER>& alias, TYPE *ptr)\n");

        int numDeletes = 0;
        {
            SS *p = new SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");

            SSObj s(p);
            ChObj c(s, &p->d_buf[5]);

            ASSERT(0 == s.get());
            ASSERT(0 == std::strcmp(c.get(), "meow"));
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose)
            printf("\tManagedPtr(ManagedPtr<OTHER>&& alias, TYPE *ptr)\n");

        numDeletes = 0;
        {
            SS *p = new SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");

            SSObj s(p);
            ChObj c(bslmf::MovableRefUtil::move(s), &p->d_buf[5]);

            ASSERT(0 == s.get());
            ASSERT(0 == std::strcmp(c.get(), "meow"));
            ASSERT(0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose)
            printf("\tvoid loadAlias(ManagedPtr<OTHER>& alias, TYPE *ptr)\n");

        bsls::Types::Int64 numDeallocations = da.numDeallocations();
        numDeletes = 0;
        {
            SS *p = new SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");
            char *pc = static_cast<char *>(da.allocate(5));
            std::strcpy(pc, "Werf");

            SSObj s(p);
            ChObj c(pc, &da);

            ASSERT(da.numDeallocations() == numDeallocations);
            c.loadAlias(s, &p->d_buf[5]);
            ASSERT(da.numDeallocations() == numDeallocations + 1);

            ASSERT(0 == s.get());
            ASSERT(0 == std::strcmp(c.get(), "meow"));
        }
        ASSERT(da.numDeallocations() == numDeallocations + 1);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'load' OVERLOADS
        //
        // Concerns:
        //: 1 Calling 'load' on an empty managed pointer assigns ownership of
        //:   the pointer passed as the argument.
        //:
        //: 2 Calling 'load' on a 'bslma::ManagedPtr' that owns a non-null
        //:   pointer destroys the referenced object, and takes ownership of
        //:   the new one.
        //:
        //: 3 Calling load with a null pointer, or no argument, causes a
        //:   'bslma::ManagedPtr' object to destroy any managed object, without
        //:   owning a new one.
        //:
        //: 4 'bslma::ManagedPtr<void>' can load a pointer to any other type,
        //:   owning the pointer and deducing a deleter that will correctly
        //:   destroy the pointed-to object.
        //:
        //: 5 'bslma::ManagedPtr<void>' can load a true 'void *' pointer only
        //:   if an appropriate factory or deleter function is also passed.
        //:   The single argument 'load(void *) should fail to compile.
        //:
        //: 6 'bslma::ManagedPtr<const T>' can be loaded with a 'T *' pointer
        //:   (cv-qualification conversion).
        //:
        //: 7 'bslma::ManagedPtr<base>' can be loaded with a 'derived *'
        //:   pointer and the deleter will destroy the 'derived' type, even if
        //:   the 'base' destructor is not virtual.
        //:
        //: 8 When 'bslma::ManagedPtr' is passed a single 'FACTORY *' argument,
        //:   the implicit deleter-function will destroy the pointed-to object
        //:   using the FACTORY::deleteObject (non-static) method.
        //:
        //: 9 'bslma::Allocator' serves as a valid FACTORY type.
        //:
        //:10 A custom type offering just the 'deleteObject' (non-virtual)
        //:   member function serves as a valid FACTORY type.
        //:
        //:11 A 'bslma::ManagedPtr' points to the same object as the pointer
        //:   passed to 'load'.  Note that this includes null pointers.
        //:
        //:12 Destroying a 'bslma::ManagedPtr' destroys any owned object using
        //:   the deleter mechanism supplied by 'load'.
        //:
        //:13 Destroying a bslma::ManagedPtr that does not own a pointer has
        //:   no observable effect.
        //:
        //:14 No 'bslma::ManagedPtr' method should allocate any memory.
        //:
        //:15 Defensive checks assert in safe build modes when passing null
        //:   pointers as arguments for factory or deleters, unless target
        //:   pointer is also null.
        //
        // Plan:
        //   take an empty pointer, and call each overload of load.
        //      confirm pointer is initially null
        //      confirm new pointer value is stored by 'ptr'
        //      confirm destructor destroys target object
        //      be sure to pass both '0' and valid pointer values to each
        //          potential overload
        //   Write a pair of nested loops
        //     For each iteration, first create a default-constructed
        //         'bslma::ManagedPtr'
        //     Then call a load function (testing each overload by the first
        //         loop)
        //     Then, in inner loop, load a second pointer and verify first
        //         target is destroyed
        //     Then verify the new target is destroyed when test object goes
        //         out of scope.
        //
        //   Test a number of scenarios in a consistent way.
        //   The 5 scenarios are:  (TestTarget)
        //      MyTestObject
        //      const MyTestObject
        //      MyDerivedObject
        //      void
        //      const void
        //
        //   For each, create a table of test-functions that use encoded names:
        //      Object  - function supplies an object to 'load'
        //      Factory - function supplies a factory to 'load'
        //      Fnull   - function passes a null pointer literal as factory
        //      Deleter - function supplies a deleter to 'load'
        //      Dzero   - function calls 'load' with a 0-value pointer variable
        //                and *not* a null-pointer literal.
        //
        //   Codes that may be passed as 'Object' policies are:
        //      Obase
        //      OCbase
        //      Oderiv
        //      OCDeriv
        //
        //   Codes for specific factories are:
        //      Fbsl   factory is cast to base 'bslma::Allocator'
        //      Ftst   factory is cast to specific 'bslma::TestAllocator' type
        //      Fdflt  factory argument is ignored and default allocator used
        //
        //   Each test-function taking an 'Object' parameter will call 'load'
        //   with both an allocate object, and a pointer variable holding a
        //   null-pointer value.
        //
        //   Each test-function taking a 'Factory' parameter will call 'load'
        //   with both an allocate object, and a pointer variable holding a
        //   null-pointer value.
        //
        //   Each of the four combinations of valid/null pointer for the
        //   factory/object arguments will be tested for test-functions taking
        //   both parameters.  The combination will be negatively tested using
        //   the 'bsls_asserttest' facility if that is appropriate.
        //
        //   These functions are assembled using further policy-functions that
        //   will create and supply objects, deleters and factories of types
        //   that are specified as template type parameters.  This allows us to
        //   compose test cases with the full set of conversion scenarios that
        //   may be needed.
        //
        //   The test function will take a default-constructed managed pointer
        //   object, call the 'load' test function, establish that the expected
        //   results of 'load' are evident, and then let the managed pointer
        //   fall out of scope, and check that the destructor destroys any
        //   managed object appropriately.
        //
        //   Then an inner-loop will again create an empty managed pointer
        //   object and 'load' it using the test function.  It will then call
        //   'load' again with the next function in the test table, and verify
        //   that the original managed object (if any) is destroyed correctly
        //   and that the new managed object is held as expected.  Then this
        //   managed pointer will fall out of scope, and we test again that any
        //   held managed object is destroyed correctly.
        //
        //   Currently well tested:
        //     const MyObjectType
        //     const void  [audit the disabled tests before moving on]
        //
        //   Note that base/derived are a conventional pair of polymorphic
        //   classes with virtual destructors.  This class should work equally
        //   well for base/derived classes that are not polymorphic, but we
        //   do not currently test that.
        //
        // Testing:
        //   void load(nullptr_t=0, void *cookie=0, DeleterFunc deleter=0);
        //   void load(TYPE *ptr);
        //   void load(TYPE *ptr, FACTORY *factory);
        //   void load(TYPE *ptr, void *cookie, DeleterFunc deleter);
        //   void load(TYPE *ptr, bsl::nullptr_t, void (*del)(BASE *, void *));
        //   void load(TYPE *, FACTORY *, void(*)(TYPE_BASE *, FACTORY_BASE *))
        //   ~ManagedPtr();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'load' OVERLOADS"
                            "\n========================\n");

        {
            if (verbose) printf(
                            "Testing bslma::ManagedPtr<MyTestObject>::load\n");

            testLoadOps(L_, TEST_POLICY_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (verbose) printf(
                      "Testing bslma::ManagedPtr<const MyTestObject>::load\n");

            testLoadOps(L_, TEST_POLICY_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (verbose) printf(
                         "Testing bslma::ManagedPtr<MyDerivedObject>::load\n");

            testLoadOps(L_, TEST_POLICY_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (verbose) printf("Testing bslma::ManagedPtr<void>::load\n");

            testLoadOps(L_, TEST_POLICY_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (verbose) printf(
                              "Testing bslma::ManagedPtr<const void>::load\n");

            testLoadOps(L_, TEST_POLICY_CONST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(BSLMA_MANAGEDPTR_TESTVIRTUALINHERITANCE)
        {
            if (verbose) printf("Testing bslma::ManagedPtr<Base>::load\n");

            testLoadOps(L_, TEST_POLICY_BASE0_ARRAY);
        }
#endif
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (verbose) printf("Testing bslma::ManagedPtr<Base2>::load\n");

            testLoadOps(L_, TEST_POLICY_BASE2_ARRAY);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE)
        {
            int i = 0;
            bslma::ManagedPtr<double> x;
            x.load(&i);

            const double d = 0.0;
            x.load(&d);

            void *v = 0;
            x.load(v);
        }
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY)
        {
            int i = 0;
            bslma::ManagedPtr<int> x;
            x.load(&i, 0);
            x.load( 0, 0); // We may consider allowing this

            void *v = 0;
            x.load(v, 0);
            x.load(0, v); // We may consider allowing this

            bslma::Allocator *pNullAlloc = 0;
            x.load(0, pNullAlloc); // We may consider allowing this

            MyDerivedObject *pd = 0;
            bslma::ManagedPtr<MyDerivedObject> md;
            md.load(pd, 0);
        }
#endif

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER
#if defined(BSLMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER)
        {
            int *i = 0;
            bslma::ManagedPtr<int> x;
            x.load(i, 0, 0);
            x.load(0, 0, 0); // We may consider allowing this

            bslma::Allocator *pNullAlloc = 0;
            x.load(i, pNullAlloc, 0);
            x.load(0, pNullAlloc, 0);  // We may consider allowing this
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY CREATORS
        //   Note that we will not deem the destructor to be completely tested
        //   until the next test case, which tests the range of management
        //   strategies a 'bslma::ManagedPtr' may hold.
        //
        // Concerns:
        //: 1 A default constructed 'bslma::ManagedPtr' does not own a pointer.
        //:
        //: 2 A default constructed 'bslma::ManagedPtr' does not allocate any
        //:   memory.
        //:
        //: 3 A 'bslma::ManagedPtr' takes ownership of a pointer passed as a
        //:   single argument to its constructor, and destroys the pointed-to
        //:   object in its destructor using the default allocator.  It does
        //:   not allocate any memory.
        //:
        //: 4 A 'bslma::ManagedPtr<base>' object created by passing a
        //:   'derived *' pointer calls the 'derived' destructor when
        //:   destroying the managed object, regardless of whether the 'base'
        //:   destructor is declared as 'virtual'.  No memory is allocated by
        //:   'bslma::ManagedPtr'.
        //:
        //: 5 A 'bslma::ManagedPtr<void>' object created by passing a
        //:   'derived*' pointer calls the 'derived' destructor when destroying
        //:   the managed object.  No memory is allocated by
        //:   'bslma::ManagedPtr'.
        //:
        //: 6 A 'bslma::ManagedPtr' taking ownership of a null pointer passed
        //:   as a single argument is equivalent to default construction; it
        //:   does not allocate any memory.
        //
        // Plan:
        //    TBD
        //
        // Testing:
        //   ManagedPtr();
        //   ManagedPtr(bsl::nullptr_t, bsl::nullptr_t = 0);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY CREATORS"
                            "\n========================\n");

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) printf("\tTest default constructor\n");

        int numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tBasic test object\n");

            bslma::TestAllocatorMonitor dam(&da);
            Obj o;

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tvoid type\n");

            bslma::TestAllocatorMonitor dam(&da);
            bslma::ManagedPtr<void> o;

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tconst-qualified int\n");

            bslma::TestAllocatorMonitor dam(&da);
            bslma::ManagedPtr<const int> o;

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tTest constructing with a null pointer\n");

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tBasic test object\n");

            bslma::TestAllocatorMonitor dam(&da);
            Obj o(0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tvoid type\n");

            bslma::TestAllocatorMonitor dam(&da);
            VObj o(0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tconst-qualified int\n");

            bslma::TestAllocatorMonitor dam(&da);
            bslma::ManagedPtr<const int> o(0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tTest constructing with two null pointers\n");

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tBasic test object\n");

            bslma::TestAllocatorMonitor dam(&da);
            Obj o(0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tvoid type\n");

            bslma::TestAllocatorMonitor dam(&da);
            VObj o(0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tconst-qualified int\n");

            bslma::TestAllocatorMonitor dam(&da);
            bslma::ManagedPtr<const int> o(0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\tTest constructing with three null pointers\n");

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tBasic test object\n");

            bslma::TestAllocatorMonitor dam(&da);
            Obj o(0, 0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tvoid type\n");

            bslma::TestAllocatorMonitor dam(&da);
            VObj o(0, 0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) printf("\t\tconst-qualified int\n");

            bslma::TestAllocatorMonitor dam(&da);
            bslma::ManagedPtr<const int> o(0, 0, 0);

            ASSERT(0 == o.get());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'ManagedPtr_Ref'
        //
        // 'bslma::ManagedPtr_Ref' is similar to an in-core value-semantic type
        // having a single pointer as its only attribute; it does not offer the
        // traditional range of value-semantic operations such as equality
        // comparison and printing.  Its test concerns and plan are closely
        // modeled after such a value-semantic type.
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   ManagedPtr_Ref(ManagedPtr_Members *base, TARGET_TYPE *target);
        //   ManagedPtr_Ref(const bslma::ManagedPtr_Ref& original);
        //   ~ManagedPtr_Ref();
        //   ManagedPtr_Ref& operator=(const bslma::ManagedPtr_Ref&);
        //   ManagedPtr_Members *base() const;
        // * TARGET_TYPE *target() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'ManagedPtr_Ref'"
                            "\n========================\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        {
            int deleteCount = 0;
            MyTestObject x(&deleteCount);

            {
                bslma::ManagedPtr_Members empty;
                bslma::ManagedPtr_Members simple(&x, 0, doNothingDeleter);

                if (verbose) printf("\tTest value constructor\n");

                const bslma::ManagedPtr_Ref<MyTestObject> ref(&empty, 0);
                bslma::ManagedPtr_Members *base = ref.base();
                ASSERTV(&empty, base, &empty == base);

                if (verbose) printf("\tTest copy constructor\n");

                bslma::ManagedPtr_Ref<MyTestObject> other = ref;
                base = ref.base();
                ASSERTV(&empty, base, &empty == base);
                base = other.base();
                ASSERTV(&empty, base, &empty == base);

                if (verbose) printf("\tTest assignment\n");

                const bslma::ManagedPtr_Ref<MyTestObject> second(&simple, &x);
                base = second.base();
                ASSERTV(&simple, base, &simple == base);

                other = second;

                base = ref.base();
                ASSERTV(&empty, base,  &empty  == base);
                base = other.base();
                ASSERTV(&simple, base, &simple == base);
                base = second.base();
                ASSERTV(&simple, base, &simple == base);

                if (verbose) printf("\tTest destructor\n");
            }

            ASSERTV(deleteCount, 0 == deleteCount);
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tNegative testing\n");

        {
            bsls::AssertTestHandlerGuard guard;
            ASSERT_SAFE_FAIL_RAW(bslma::ManagedPtr_Ref<MyTestObject> x(0, 0));
        }
#else
        if (verbose) printf("\tNegative testing disabled due to lack of "
                             "exception support\n");
#endif

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST MACHINERY
        //
        // Concerns:
        //: 1 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //    and 'MyDerivedDerivedObject'
        //:   objects do not allocate any memory from the default allocator nor
        //:   from the global allocator for any of their operations.
        //:
        //: 2 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //    and 'MyDerivedDerivedObject'
        //:   objects, created with a pointer to an integer, increment the
        //:   referenced integer exactly once when they are destroyed.
        //:
        //: 3 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //    and 'MyDerivedDerivedObject'
        //:   objects, created by copying another object of the same type,
        //:   increment the integer referenced by the original object, exactly
        //:   once, when they are destroyed.
        //:
        //: 4 'MyDerivedObject' is derived from 'MyTestObject'.
        //:
        //: 5 'MySecondDerivedObject' is derived from 'MyTestObject'.
        //:
        //: 6 'MyDerivedObject' is *not* derived from 'MySecondDerivedObject',
        //:   nor is 'MySecondDerivedObject' derived from 'MyDerivedObject'.
        //:
        //: 7 'MyDerivedDerivedObject' is derived from 'MyDerivedObject'.
        //:
        //: 8 'MyDerivedDerivedObject' is *not* derived from
        //    'MySecondDerivedObject'.
        //
        // Plan:
        //: 1 Install test allocator monitors to verify that neither the global
        //:   nor default allocators allocate any memory executing this test
        //:   case.
        //:
        //: 2 For each test-class type:
        //:   1 Initialize an 'int' counter to zero
        //:   2 Create a object of tested type, having the address of the 'int'
        //:     counter.
        //:   3 Confirm the test object 'deleterCounter' points to the 'int'
        //:     counter.
        //:   4 Confirm the 'int' counter value has not changed.
        //:   5 Destroy the test object and confirm the 'int' counter value
        //:     has incremented by exactly 1.
        //:   6 Create a second object of tested type, having the address of
        //:     the 'int' counter.
        //:   7 Create a copy of the second test object, and confirm both test
        //:     object's 'deleterCount' point to the same 'int' counter.
        //:   8 Confirm the 'int' counter value has not changed.
        //:   9 Destroy one test object, and confirm test 'int' counter is
        //:     incremented exactly once.
        //:  10 Destroy the other test object, and confirm test 'int' counter
        //:     is incremented exactly once.
        //:
        //: 3 For each test-class type:
        //:   1 Create a function overload set, where one function takes a
        //:     pointer to the test-class type and returns 'true', while the
        //:     other overload matches anything and returns 'false'.
        //:   2 Call each of the overloaded function sets with a pointer to
        //:     'int', and confirm each returns 'false'.
        //:   3 Call each of the overloaded function sets with a pointer to
        //:     an object of each of the test-class types, and confirm each
        //:     call returns 'true' only when the pointer type matches the
        //:     test-class type for that function, or points to a type publicly
        //:     derived from that test-class type.
        //:
        //: 4 Verify that no unexpected memory was allocated by inspecting the
        //:   allocator guards.
        //
        // Testing:
        //    TEST MACHINERY
        //    class MyTestObject
        //    class MyDerivedObject
        //    class MySecondDerivedObject
        //    class MyDerivedDerivedObject
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST MACHINERY"
                            "\n======================\n");

        if (verbose) printf("\tTest class MyTestObject\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        int destructorCount = 0;
        {
            MyTestObject mt(&destructorCount);
            ASSERT(&destructorCount == mt.deleteCounter());
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1 == destructorCount);

        destructorCount = 0;
        {
            MyTestObject mt1(&destructorCount);
            {
                MyTestObject mt2 = mt1;
                ASSERT(&destructorCount == mt1.deleteCounter());
                ASSERT(&destructorCount == mt2.deleteCounter());
                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 1 == destructorCount);
        }
        ASSERT(2 == destructorCount);

        if (verbose) printf("\tTest class MyDerivedObject\n");

        destructorCount = 0;
        {
            MyDerivedObject dt(&destructorCount);
            ASSERT(&destructorCount == dt.deleteCounter());
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERT(100 == destructorCount);

        destructorCount = 0;
        {
            MyDerivedObject dt1(&destructorCount);
            {
                MyDerivedObject dt2 = dt1;
                ASSERT(&destructorCount == dt1.deleteCounter());
                ASSERT(&destructorCount == dt2.deleteCounter());
                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 100 == destructorCount);
        }
        ASSERT(200 == destructorCount);

        if (verbose) printf("\tTest class MySecondDerivedObject\n");

        destructorCount = 0;
        {
            MySecondDerivedObject st(&destructorCount);
            ASSERT(&destructorCount == st.deleteCounter());
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 10000 == destructorCount);

        destructorCount = 0;
        {
            MySecondDerivedObject st1(&destructorCount);
            {
                MySecondDerivedObject st2 = st1;
                ASSERT(&destructorCount == st1.deleteCounter());
                ASSERT(&destructorCount == st2.deleteCounter());
                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 10000 == destructorCount);
        }
        ASSERT(20000 == destructorCount);

        if (verbose) printf("\tTest class MyDerivedDerivedObject\n");

        destructorCount = 0;
        {
            MyDerivedDerivedObject st(&destructorCount);
            ASSERT(&destructorCount == st.deleteCounter());
            ASSERTV(destructorCount, 0 == destructorCount);
        }
        ASSERTV(destructorCount, 1000000 == destructorCount);

        destructorCount = 0;
        {
            MyDerivedDerivedObject st1(&destructorCount);
            {
                MyDerivedDerivedObject st2 = st1;
                ASSERT(&destructorCount == st1.deleteCounter());
                ASSERT(&destructorCount == st2.deleteCounter());
                ASSERTV(destructorCount, 0 == destructorCount);
            }
            ASSERTV(destructorCount, 1000000 == destructorCount);
        }
        ASSERT(2000000 == destructorCount);

        if (verbose) printf("\tTest pointer conversions\n");

        struct Local {
            static bool matchBase(MyTestObject *) { return true; }
            static bool matchBase(...) { return false; }

            static bool matchDerived(MyDerivedObject *) { return true; }
            static bool matchDerived(...) { return false; }

            static bool matchDerivedDerived(MyDerivedDerivedObject*)
            {
                return true;
            }
            static bool matchDerivedDerived(...) { return false; }

            static bool matchSecond(MySecondDerivedObject *) { return true; }
            static bool matchSecond(...) { return false; }
        };

        {
            int badValue;
            ASSERT(!Local::matchBase(&badValue));
            ASSERT(!Local::matchDerived(&badValue));
            ASSERT(!Local::matchSecond(&badValue));
            ASSERT(!Local::matchDerivedDerived(&badValue));
        }

        {
            MyTestObject mt(&destructorCount);
            ASSERT(Local::matchBase(&mt));
            ASSERT(!Local::matchDerived(&mt));
            ASSERT(!Local::matchSecond(&mt));
            ASSERT(!Local::matchDerivedDerived(&mt));
        }

        {
            MyDerivedObject dt(&destructorCount);
            ASSERT(Local::matchBase(&dt));
            ASSERT(Local::matchDerived(&dt));
            ASSERT(!Local::matchSecond(&dt));
            ASSERT(!Local::matchDerivedDerived(&dt));
        }

        {
            MySecondDerivedObject st(&destructorCount);
            ASSERT(Local::matchBase(&st));
            ASSERT(!Local::matchDerived(&st));
            ASSERT(Local::matchSecond(&st));
            ASSERT(!Local::matchDerivedDerived(&st));
        }

        {
            MyDerivedDerivedObject st(&destructorCount);
            ASSERT(Local::matchBase(&st));
            ASSERT(Local::matchDerived(&st));
            ASSERT(!Local::matchSecond(&st));
            ASSERT(Local::matchDerivedDerived(&st));
        }

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality but *tests* *nothing*.
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\tTest copy construction.\n");

        bslma::TestAllocator ta("object", veryVeryVeryVerbose);

        int numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2(o);

            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest assignment.\n");

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            o2  = o;

            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest construction from an rvalue.\n");

        numDeletes = 0;
        {
            bslma::TestAllocatorMonitor tam(&ta);

            Obj x(returnManagedPtr(&numDeletes, &ta)); const Obj& X = x;

            ASSERT(X.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest assignment from an rvalue.\n");

        numDeletes = 0;
        {
            Obj x; const Obj& X = x;
            x = returnManagedPtr(&numDeletes, &ta);

            ASSERT(X.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest conversion construction.\n");

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o2(o); // conversion construction

            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            CObj o3(o2); // const-conversion construction

            ASSERT(p == o3.get());
            ASSERT(0 == o2.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        if (verbose) printf("\tTest conversion assignment.\n");

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o2;
            o2  = o; // conversion assignment

            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            CObj o3;
            o3 = o2; // const-conversion assignment

            ASSERT(p == o3.get());
            ASSERT(0 == o2.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        if (verbose)
            printf("\tTest conversion construction from an rvalue.\n");

        numDeletes = 0;
        {
            Obj x(returnDerivedPtr(&numDeletes, &ta));  const Obj& X = x;

            ASSERT(X.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        if (verbose)
            printf("\tTest conversion assignment from an rvalue.\n");

        numDeletes = 0;
        {
            Obj x;  const Obj& X = x;
            x = returnDerivedPtr(&numDeletes, &ta); // conversion-assignment
                                                    // from an rvalue

            ASSERT(X.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        if (verbose) printf("\tTest alias construction.\n");

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            ASSERT(0 != p);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            bslma::ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest alias construction with conversion.\n");

        numDeletes = 0;
        {
            Derived *p = new MyDerivedObject(&numDeletes);
            ASSERT(0 != p);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            bslma::ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 100 == numDeletes);

        if (verbose) printf("\tTest 'load' method.\n");

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            Test *p = new MyTestObject(&numDeletes2);
            ASSERT(0 != p);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            Test *p2 = new(da) MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            o.load(p2);
            ASSERT(p2 == o.get());
            ASSERT( 1 == numDeletes2);
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest 'load' method with allocator.\n");

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            Test *p = new MyTestObject(&numDeletes2);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            Test *p2 = new(ta) MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            o.load(p2,&ta);
            ASSERT(p2 == o.get());
            ASSERTV(numDeletes2, 1 == numDeletes2);
            ASSERTV(numDeletes,  0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest 'loadAlias'.\n");

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            bslma::ManagedPtr<int> o2;

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            o2.loadAlias(o, o->valuePtr());

            ASSERT(p->valuePtr() == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            // Check load alias to self
            o2.loadAlias(o2, p->valuePtr(1));
            ASSERT(p->valuePtr(1) == o2.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest 'swap'.\n");

        numDeletes = 0;
        {
            Test *p = new MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest 'swap' with custom deleter.\n");

        numDeletes = 0;
        {
            Test *p = new(ta) MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(p == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.get());
            ASSERT(0 == o.get());
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest boolean.\n");

        numDeletes = 0;
        {
            Test *p = new(ta) MyTestObject(&numDeletes);
            ASSERTV(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(o);
            ASSERT(!o2);

            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest no-op deleter.\n");

        numDeletes = 0;
        {
            Test x(&numDeletes);
            {
                Obj p(&x, 0, &bslma::ManagedPtrUtil::noOpDeleter);
            }
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest (deprecated) nil deleter.\n");

        numDeletes = 0;
        {
            Test x(&numDeletes);
            {
                Obj p(&x,
                      0,
                      &bslma::ManagedPtrNilDeleter<MyTestObject>::deleter);
            }
            ASSERTV(numDeletes, 0 == numDeletes);
        }
        ASSERTV(numDeletes, 1 == numDeletes);

        if (verbose) printf("\tTest unambiguous overloads.\n");

        {
            bslma::ManagedPtr<void>      pV;
            bslma::ManagedPtr<int>       pI;
            bslma::ManagedPtr<const int> pCi;

            ASSERT(0 == OverloadTest::invoke(pV));
            ASSERT(1 == OverloadTest::invoke(pI));
            ASSERT(2 == OverloadTest::invoke(pCi));

#if 0  // compile fail test, think about giving a named macro to test
            bslma::ManagedPtr<double>    pD;
            ASSERT(0 == OverloadTest::invoke(pD));
#endif
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // VERIFYING FAILURES TO COMPILE
        //
        //   This test is checking for the *absence* of the following operators
        //: o 'operator=='.
        //: o 'operator!='.
        //: o 'operator<'.
        //: o 'operator<='.
        //: o 'operator>='.
        //: o 'operator>'.
        //
        // Concerns:
        //: 1 Two 'bslma::ManagedPtr<T>' objects should not be comparable with
        //:   the equality operator.
        //:
        //: 2 Two objects of different instantiations of the
        //:   'bslma::ManagedPtr' class template should not be comparable with
        //:   the equality operator.
        //
        // Plan:
        //   The absence of a specific operator will be tested by failing to
        //   compile test code using that operator.  These tests will be
        //   configured to compile only when specific macros are defined as
        //   part of the build configuration, and not routinely tested.
        //
        // Testing:
        //   VERIFYING FAILURES TO COMPILE
        // --------------------------------------------------------------------

        if (verbose) printf("\nVERIFYING FAILURES TO COMPILE"
                            "\n=============================\n");

//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
//#define BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING

#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
        {
            bslma::ManagedPtr<int> x;
            bool b;

            // The following two lines should fail to compile.
            b = (x == x);
            b = (x != x);
        }
#endif

#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
        {
            bslma::ManagedPtr<int> x;
            bool b;

            // The following four lines should fail to compile.
            b = (x <  x);
            b = (x <= x);
            b = (x >= x);
            b = (x >  x);
        }
#endif

#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
        {
            bslma::ManagedPtr<int>    x;
            bslma::ManagedPtr<double> y;

            bool b;

            // The following four lines should fail to compile.
            b = (x == y);
            b = (x != y);

            b = (y == x);
            b = (y != x);
        }
#endif

#if defined BSLMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING
        {
            bslma::ManagedPtr<int>    x;
            bslma::ManagedPtr<double> y;

            bool b;

            // The following eight lines should fail to compile.
            b = (x <  y);
            b = (x <= y);
            b = (x >= y);
            b = (x >  y);

            b = (y <  x);
            b = (y <= x);
            b = (y >= x);
            b = (y >  x);
        }
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
