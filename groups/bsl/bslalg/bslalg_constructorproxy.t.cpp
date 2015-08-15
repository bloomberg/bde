// bslalg_constructorproxy.t.cpp                                      -*-C++-*-

#include <bslalg_constructorproxy.h>

#include <bslma_usesbslmaallocator.h>            // for testing only
#include <bslma_allocator.h>                     // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The test plan for this component is very straightforward.  We will create
// the following test types:
//
//     o TestType_YesAllocator - a class that uses 'bslma' allocators.
//     o TestType_NoAllocator  - a class that does not use 'bslma' allocators.
//     o TestType_Other        - a class that can be used to construct
//                               'TestType_YesAllocator' objects and
//                               'TestType_NoAllocator' objects.
//
// Test [1] will use these types to test the constructor proxy using all the
// relevant combinations.  Test [2] and [3] will test the usage examples to
// make sure they compile and run as expected.
//-----------------------------------------------------------------------------
// The following abbreviations are used in the signatures below:
//
//     o b_A  -> bslma::Allocator
//     o b_CP -> bslalg::ConstructorProxy
//
// CREATORS
// [ 1] bslalg::ConstructorProxy(b_A*);
// [ 1] bslalg::ConstructorProxy(const b_CP<OTHER>&, b_A*);
// [ 1] bslalg::ConstructorProxy(const OTHER&, b_A*);
// [ 1] ~bslalg::ConstructorProxy();
//
// MANIPULATORS
// [ 1] TYPE& object();
//
// ACCESSORS
// [ 1] const TYPE& object() const;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE 1
// [ 3] USAGE EXAMPLE 2

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int g_objectCount = 0;  // Global counter for number of objects
                               // currently constructed.  If this value is not
                               // zero when the test driver exits, that means
                               // the constructor proxy did not construct or
                               // destroy the objects correctly.

template <class TYPE>
class TestType_Other_Template {
    // Template for generating the 'TestType_Other' type.  The 'TestType_Other'
    // type can be used to construct 'TestType_YesAllocator' and
    // 'TestType_NoAllocator' objects.
    //
    // Note that a template is used in order to be able to test
    // 'bslalg::ConstructorProxy' using 'TYPE's that are generated from
    // templates (some compilers sometimes behave strangely when combination of
    // template classes are used).

    // PRIVATE DATA MEMBERS
    TYPE d_value;  // value of this object

  public:
    // CREATORS
    TestType_Other_Template()
    {
        ++g_objectCount;
    }

    TestType_Other_Template(const TestType_Other_Template& original)
    : d_value(original.d_value)
    {
        ++g_objectCount;
    }

    explicit TestType_Other_Template(const TYPE& value)
    : d_value(value)
    {
        ++g_objectCount;
    }

    ~TestType_Other_Template()
    {
        --g_objectCount;
    }

    // MANIPULATORS
    TYPE& value()
    {
        return d_value;
    }

    // ACCESSORS
    const TYPE& value() const
    {
        return d_value;
    }
};

template <class TYPE>
class TestType_YesAllocator_Template {
    // Template for generating the 'TestType_YesAllocator' type.  The
    // 'TestType_YesAllocator' type simulates a class that uses 'bslma'
    // allocators.
    //
    // Note that a template is used in order to be able to test
    // 'bslalg::ConstructorProxy' using 'TYPE's that are generated from
    // templates (some compilers sometimes behave strangely when combination of
    // template classes are used).

    // PRIVATE DATA MEMBERS
    TYPE              d_value;        // value of this object
    bslma::Allocator *d_allocator_p;  // allocator passed to constructor

  public:
    // CREATORS
    explicit TestType_YesAllocator_Template(
                                          bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    TestType_YesAllocator_Template(
                     const TestType_YesAllocator_Template&  original,
                     bslma::Allocator                      *basicAllocator = 0)
    : d_value(original.d_value)
    , d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    explicit TestType_YesAllocator_Template(
                      const TestType_Other_Template<TYPE>&  other,
                      bslma::Allocator                     *basicAllocator = 0)
    : d_value(other.value())
    , d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    explicit TestType_YesAllocator_Template(
                                          const TYPE&       value,
                                          bslma::Allocator *basicAllocator = 0)
    : d_value(value)
    , d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    ~TestType_YesAllocator_Template()
    {
        --g_objectCount;
    }

    // MANIPULATORS
    TYPE& value()
    {
        return d_value;
    }

    // ACCESSORS
    const TYPE& value() const
    {
        return d_value;
    }

    bslma::Allocator* allocator() const
    {
        return d_allocator_p;
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <class TYPE>
struct UsesBslmaAllocator<TestType_YesAllocator_Template<TYPE> >
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

template <class TYPE>
class TestType_NoAllocator_Template {
    // Template for generating the 'TestType_NoAllocator' type.  The
    // 'TestType_NoAllocator' type simulates a class that does not use 'bslma'
    // allocators.
    //
    // Note that a template is used in order to be able to test
    // 'bslalg::ConstructorProxy' using 'TYPE's that are generated from
    // templates (some compilers sometimes behave strangely when combination of
    // template classes are used).

    // PRIVATE DATA MEMBERS
    TYPE d_value;  // value of this object

  public:
    // CREATORS
    TestType_NoAllocator_Template()
    {
        ++g_objectCount;
    }

    TestType_NoAllocator_Template(
                                 const TestType_NoAllocator_Template& original)
    : d_value(original.d_value)
    {
        ++g_objectCount;
    }

    explicit TestType_NoAllocator_Template(
                                    const TestType_Other_Template<TYPE>& other)
    : d_value(other.value())
    {
        ++g_objectCount;
    }

    explicit TestType_NoAllocator_Template(const TYPE& value)
    : d_value(value)
    {
        ++g_objectCount;
    }

    ~TestType_NoAllocator_Template()
    {
        --g_objectCount;
    }

    // MANIPULATORS
    TYPE& value()
    {
        return d_value;
    }

    // ACCESSORS
    const TYPE& value() const
    {
        return d_value;
    }
};

class ValueType {
    // Value type that will be used as the template argument 'TYPE' in the
    // template classes above.  This is basically a wrapper around 'int' with a
    // 'DEFAULT_VALUE'.

    // PRIVATE DATA MEMBERS
    int d_value;

  public:
    // CONSTANTS
    enum { DEFAULT_VALUE = 45 };

    // CREATORS
    ValueType()
    : d_value(DEFAULT_VALUE)
    {
    }

    ValueType(const ValueType& original)
    : d_value(original.d_value)
    {
    }

    explicit ValueType(int value)
    : d_value(value)
    {
    }

    ~ValueType()
    {
    }

    // MANIPULATORS
    ValueType& operator=(const ValueType& rhs)
    {
        d_value = rhs.d_value;
        return *this;
    }

    // ACCESSORS
    operator int() const
    {
        return d_value;
    }
};

inline
bool operator==(const ValueType& lhs, const ValueType& rhs)
{
    return int(lhs) == int(rhs);
}

inline
bool operator!=(const ValueType& lhs, const ValueType& rhs)
{
    return int(lhs) != int(rhs);
}

typedef TestType_YesAllocator_Template<ValueType>     TestType_YesAllocator;
typedef TestType_NoAllocator_Template<ValueType>      TestType_NoAllocator;
typedef TestType_Other_Template<ValueType>            TestType_Other;
typedef bslalg::ConstructorProxy<TestType_YesAllocator> Obj_YesAllocator;
typedef bslalg::ConstructorProxy<TestType_NoAllocator>  Obj_NoAllocator;
typedef bslalg::ConstructorProxy<TestType_Other>        Obj_Other;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample1 {

///Usage 1
///-------
// The following snippets of code illustrate the minimal syntax for using a
// constructor proxy.  The second usage example will provide a more typical
// example that happens in generic components.
//
// Suppose you have some arbitrary class:
//..
class SomeArbitraryClass {
    // Some arbitrary class.

    // PRIVATE DATA MEMBERS
    bslma::Allocator *d_allocator_p;  // allocator passed to constructor

  public:
    // CREATORS
    explicit SomeArbitraryClass(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
    }

    SomeArbitraryClass(const SomeArbitraryClass&  /*original*/,
                       bslma::Allocator          *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
    }

    // ACCESSORS
    bslma::Allocator* allocator() const
    {
        return d_allocator_p;
    }
};

}  // close namespace UsageExample1

// TRAITS
namespace BloombergLP {
namespace bslma {
template <> struct UsesBslmaAllocator<UsageExample1::SomeArbitraryClass>
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace
//..
// The 'SomeArbitraryClass' class may or may not declare the
// 'bslma::UsesBslmaAllocator' trait.  The following code illustrates
// how a 'SomeArbitraryClass' object can be constructed using a constructor
// proxy for detecting this trait:
//..
namespace UsageExample1 {

void run()
    // Run usage example 1.
{
    bslma::TestAllocator allocator;

    bslalg::ConstructorProxy<SomeArbitraryClass> proxy(&allocator);

    SomeArbitraryClass& myObject = proxy.object();

    ASSERT(&allocator == myObject.allocator());
}
//..
// If 'SomeArbitraryClass' declared the 'bslma::UsesBslmaAllocator'
// trait, then the specified 'myObject' will obtain its memory from the
// specified 'allocator'.  Otherwise, the specified 'allocator' will be
// ignored.

}  // close namespace UsageExample1

namespace UsageExample2 {

///Usage 2
///-------
// The following snippets of code illustrate the usage of this component in a
// more typical scenario.
//
// The 'MyContainer' class below contains an object of a templated 'TYPE':
//..
//  template <class TYPE>
//  class MyContainer {
//      // A class that contains an object of the specified 'TYPE'.
//
//      // PRIVATE DATA MEMBERS
//      TYPE d_object;  // contained object
//
//    public:
//      // CREATORS
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
//          // Construct a container using the specified 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, an appropriate default
//          // allocator is used.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // ACCESSORS
//      const TYPE& getObject() const;
//          // Return a reference to the non-modifiable object stored in this
//          // container.
//
//      // ... rest of class definition ...
//  };
//..
// The implementation for the 'MyContainer' constructor will be a little
// tricky without a constructor proxy.  One possible implementation is as
// follows:
//..
//  template <class TYPE>
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
//  {
//  }
//..
// This implementation will compile successfully for all 'TYPE's that have a
// default constructor, but it will *not* behave as documented.  More
// specifically, the specified 'basicAllocator' will *not* be used to supply
// memory.
//
// Another possible implementation for the 'MyContainer' constructor is as
// follows:
//..
//  template <class TYPE>
//  MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
//  : d_object(basicAllocator)
//  {
//  }
//..
// This implementation behaves as documented, but it will *not* compile
// successfully for 'TYPE's that do not have a constructor that takes a
// 'bslma::Allocator' pointer.  For example, the following declaration of
// 'container' will *not* compile:
//..
//  bslma::TestAllocator testAllocator;
//
//  MyContainer<int> container(&testAllocator);
//..
// The solution to this problem is to use a constructor proxy.  The following
// definition for 'MyContainer' uses a constructor proxy for 'TYPE':
//..

namespace WithoutAllocatorTrait {

template <class TYPE>
class MyContainer {
    // A class that contains an object of the specified 'TYPE'.

    // PRIVATE DATA MEMBERS
    bslalg::ConstructorProxy<TYPE> d_proxy;

  public:
    // CREATORS
    explicit MyContainer(bslma::Allocator *basicAllocator = 0);
        // Construct a container using the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, an appropriate default
        // allocator is used.

    ~MyContainer();
        // Destroy this container.

    // ACCESSORS
    const TYPE& getObject() const;
        // Return a reference to the non-modifiable object stored in this
        // container.

    // ... rest of class definition ...
};
//..
// The constructor for 'MyContainer' can now be implemented as follows:
//..
template <class TYPE>
MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
: d_proxy(basicAllocator)
{
}

template <class TYPE>
MyContainer<TYPE>::~MyContainer()
{
}
//..
//  The 'getObject' method of 'MyContainer' can be implemented as follows:
//..
template <class TYPE>
const TYPE& MyContainer<TYPE>::getObject() const
{
    return d_proxy.object();
}
//..
// Now, the following code, which previously did not compile, *will* compile
// successfully:
//..
void run1()
{
    bslma::TestAllocator testAllocator;

    MyContainer<int> container(&testAllocator);
}
//..
// The specified 'testAllocator' will simply be ignored because 'int' does not
// use a 'bslma' allocator to supply memory.  Also, if there is a class defined
// as follows:
//..
class SomeArbitraryClassUsingAllocator {
    // Some arbitrary class that uses a 'bslma' allocator.

    // PRIVATE DATA MEMBERS
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit SomeArbitraryClassUsingAllocator(
                                          bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // ACCESSORS
    bslma::Allocator* getAllocator() const
    {
        return d_allocator_p;
    }
};

}  // close namespace WithoutAllocatorTrait

}  // close namespace UsageExample2

// TRAITS
namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<UsageExample2::
                          WithoutAllocatorTrait::
                          SomeArbitraryClassUsingAllocator>
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace
//..
// then the following code *will* compile and run *without* an assertion
// failure:
//..
namespace UsageExample2 {

namespace WithoutAllocatorTrait {

void run2()
{
    bslma::TestAllocator testAllocator;

    MyContainer<SomeArbitraryClassUsingAllocator> container(&testAllocator);

    ASSERT(&testAllocator == container.getObject().getAllocator());
}

}  // close namespace WithoutAllocatorTrait

//..
// Finally, since the 'MyContainer' class uses a 'bslma' allocator to supply
// memory, it would be nice to expose this trait to other containers.  This can
// be done by declaring the 'bslma::UsesBslmaAllocator' trait to
// complete the definition of 'MyContainer':
//..

namespace WithAllocatorTrait {

class SomeArbitraryClassUsingAllocator {
    // Some arbitrary class that uses a 'bslma' allocator.

    // PRIVATE DATA MEMBERS
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit SomeArbitraryClassUsingAllocator(
                                          bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // ACCESSORS
    bslma::Allocator* getAllocator() const
    {
        return d_allocator_p;
    }
};

}  // close namespace WithAllocatorTrait

}  // close namespace UsageExample2

// TRAITS
namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<UsageExample2::
                          WithAllocatorTrait::SomeArbitraryClassUsingAllocator>
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

namespace UsageExample2 {

namespace WithAllocatorTrait {

template <class TYPE>
class MyContainer {
    // A class that contains an object of the specified 'TYPE' and declares
    // the 'bslma::UsesBslmaAllocator' trait.

    // PRIVATE DATA MEMBERS
    bslalg::ConstructorProxy<TYPE> d_proxy;

  public:
    // CREATORS
    explicit MyContainer(bslma::Allocator *basicAllocator = 0);
        // Construct a container using the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, an appropriate default
        // allocator is used.

    ~MyContainer();
        // Destroy this container.

    // ACCESSORS
    const TYPE& getObject() const;
        // Return a reference to the non-modifiable object stored in this
        // container.

    // ... rest of class definition ...
};

}  // close namespace WithAllocatorTrait

}  // close namespace UsageExample2

// TRAITS
namespace BloombergLP {
namespace bslma {
template <class TYPE>
struct UsesBslmaAllocator<UsageExample2::
                          WithAllocatorTrait::MyContainer<TYPE> >
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

namespace UsageExample2 {

namespace WithAllocatorTrait {

template <class TYPE>
MyContainer<TYPE>::MyContainer(bslma::Allocator *basicAllocator)
: d_proxy(basicAllocator)
{
}

template <class TYPE>
MyContainer<TYPE>::~MyContainer()
{
}

template <class TYPE>
const TYPE& MyContainer<TYPE>::getObject() const
{
    return d_proxy.object();
}

//..
// Now, the following code will *also* compile and run without an assertion
// failure:
//..

void run()
{
    bslma::TestAllocator testAllocator;

    MyContainer<MyContainer<SomeArbitraryClassUsingAllocator> >
                                            containedContainer(&testAllocator);

    ASSERT(&testAllocator
                 == containedContainer.getObject().getObject().getAllocator());
}

}  // close namespace WithAllocatorTrait

//..

}  // close namespace UsageExample2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //   This will test the first usage example.
        //
        // Concerns:
        //   The usage example must compile and run successfully.
        //
        // Plan:
        //   Run the usage example and make sure it compiles and runs
        //   successfully.
        //
        // Testing:
        //   Usage Example 2
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example 2"
                            "\n=======================\n");

        UsageExample2::WithoutAllocatorTrait::run1();
        UsageExample2::WithoutAllocatorTrait::run2();
        UsageExample2::WithAllocatorTrait::run();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //   This will test the first usage example.
        //
        // Concerns:
        //   The usage example must compile and run successfully.
        //
        // Plan:
        //   Run the usage example and make sure it compiles and runs
        //   successfully.
        //
        // Testing:
        //   Usage Example 1
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example 1"
                            "\n=======================\n");

        UsageExample1::run();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR PROXY TEST
        //   This test thoroughly exercises the constructor proxy.
        //
        // Concerns:
        //   The constructor proxy must pass the allocator to the contained
        //   'TYPE' if 'TYPE' declares the
        //   'bslma::UsesBslmaAllocator' trait.  The objects must be
        //   correctly constructed and destructed.  The 'object()' manipulator
        //   and accessor must return references to the contained object.
        //
        // Plan:
        //   Use the 'TestType_YesAllocator', 'TestType_NoAllocator', and the
        //   'TestType_Other' types to exercise all the possible combinations
        //   of constructing objects.
        //
        //       1) Constructing without value.
        //          a) 'TYPE' = 'TestType_YesAllocator'.
        //          b) 'TYPE' = 'TestType_NoAllocator'.
        //       2) Constructing with 'bslalg::ConstructorProxy<OTHER>'.
        //          a) 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_YesAllocator'.
        //          b) 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //          c) 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_NoAllocator'.
        //          d) 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //       3) Constructing with 'OTHER'.
        //          a) 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_YesAllocator'.
        //          b) 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //          c) 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_NoAllocator'.
        //          d) 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //
        //  For each case, ensure that the allocator and value are handled
        //  correctly.
        //
        // Testing:
        //   bslalg::ConstructorProxy(b_A*);
        //   bslalg::ConstructorProxy(const b_CP<OTHER>&, b_A*);
        //   bslalg::ConstructorProxy(const OTHER&, b_A*);
        //   ~bslalg::ConstructorProxy();
        //   TYPE& object();
        //   const TYPE& object() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nConstructor Proxy Test"
                            "\n======================\n");

        bslma::TestAllocator  testAllocator;
        bslma::Allocator     *ALLOCATOR = &testAllocator;

        const ValueType DEFAULT;
        const ValueType VALUE(123);

        ASSERT(VALUE != DEFAULT);

        if (verbose) printf("\n1. Constructing without value." "\n");
        {
            if (veryVerbose) printf(
                                   "\ta) 'TYPE' = 'TestType_YesAllocator'.\n");
            {
                typedef Obj_YesAllocator Obj;

                Obj        mX(ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                // test return-by-reference
                mX.object().value() = VALUE;
                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                ASSERT(ALLOCATOR == X.object().allocator());
            }

            if (veryVerbose) printf("\tb) 'TYPE' = 'TestType_NoAllocator'.\n");
            {
                typedef Obj_NoAllocator Obj;

                Obj        mX(ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                // test return-by-reference
                mX.object().value() = VALUE;
                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);
            }
        }

        if (verbose) printf(
                "\n2. Constructing with 'bslalg::ConstructorProxy<OTHER>'.\n");
        {
            if (veryVerbose) printf("\ta) 'TYPE'  = 'TestType_YesAllocator'."
                                  "\n\t   'OTHER' = 'TestType_YesAllocator'."
                                  "\n");
            {
                typedef TestType_YesAllocator Type;
                typedef TestType_YesAllocator OtherType;

                typedef bslalg::ConstructorProxy<Type>      Obj;
                typedef bslalg::ConstructorProxy<OtherType> OriginalType;

                const OriginalType ORIGINAL(OtherType(VALUE), ALLOCATOR);

                Obj        mX(ORIGINAL, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                ASSERT(ALLOCATOR == X.object().allocator());
            }

            if (veryVerbose) printf("\tb) 'TYPE'  = 'TestType_YesAllocator'."
                                  "\n\t   'OTHER' = 'TestType_Other'."
                                  "\n");
            {
                typedef TestType_YesAllocator Type;
                typedef TestType_Other        OtherType;

                typedef bslalg::ConstructorProxy<Type>      Obj;
                typedef bslalg::ConstructorProxy<OtherType> OriginalType;

                const OriginalType ORIGINAL(OtherType(VALUE), ALLOCATOR);

                Obj        mX(ORIGINAL, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                ASSERT(ALLOCATOR == X.object().allocator());
            }

            if (veryVerbose) printf("\tc) 'TYPE'  = 'TestType_NoAllocator'."
                                  "\n\t   'OTHER' = 'TestType_NoAllocator'."
                                  "\n");
            {
                typedef TestType_NoAllocator Type;
                typedef TestType_NoAllocator OtherType;

                typedef bslalg::ConstructorProxy<Type>      Obj;
                typedef bslalg::ConstructorProxy<OtherType> OriginalType;

                const OriginalType ORIGINAL(OtherType(VALUE), ALLOCATOR);

                Obj        mX(ORIGINAL, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);
            }

            if (veryVerbose) printf("\td) 'TYPE'  = 'TestType_NoAllocator'."
                                  "\n\t   'OTHER' = 'TestType_Other'."
                                  "\n");
            {
                typedef TestType_NoAllocator Type;
                typedef TestType_Other       OtherType;

                typedef bslalg::ConstructorProxy<Type>      Obj;
                typedef bslalg::ConstructorProxy<OtherType> OriginalType;

                const OriginalType ORIGINAL(OtherType(VALUE), ALLOCATOR);

                Obj        mX(ORIGINAL, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);
            }
        }

        if (verbose) printf("\n3. Constructing with 'OTHER'." "\n");
        {
            if (veryVerbose) printf("\ta) 'TYPE'  = 'TestType_YesAllocator'."
                                  "\n\t   'OTHER' = 'TestType_YesAllocator'."
                                  "\n");
            {
                typedef TestType_YesAllocator          Type;
                typedef TestType_YesAllocator          OtherType;
                typedef bslalg::ConstructorProxy<Type> Obj;

                const OtherType OTHER_VALUE(VALUE);

                Obj        mX(OTHER_VALUE, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                ASSERT(ALLOCATOR == X.object().allocator());
            }

            if (veryVerbose) printf("\tb) 'TYPE'  = 'TestType_YesAllocator'."
                                  "\n\t   'OTHER' = 'TestType_Other'."
                                  "\n");
            {
                typedef TestType_YesAllocator          Type;
                typedef TestType_Other                 OtherType;
                typedef bslalg::ConstructorProxy<Type> Obj;

                const OtherType OTHER_VALUE(VALUE);

                Obj        mX(OTHER_VALUE, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);

                ASSERT(ALLOCATOR == X.object().allocator());
            }

            if (veryVerbose) printf("\tc) 'TYPE'  = 'TestType_NoAllocator'."
                                  "\n\t   'OTHER' = 'TestType_NoAllocator'."
                                  "\n");
            {
                typedef TestType_NoAllocator           Type;
                typedef TestType_NoAllocator           OtherType;
                typedef bslalg::ConstructorProxy<Type> Obj;

                const OtherType OTHER_VALUE(VALUE);

                Obj        mX(OTHER_VALUE, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);
            }

            if (veryVerbose) printf("\td) 'TYPE'  = 'TestType_NoAllocator'."
                                  "\n\t   'OTHER' = 'TestType_Other'."
                                  "\n");
            {
                typedef TestType_NoAllocator           Type;
                typedef TestType_Other                 OtherType;
                typedef bslalg::ConstructorProxy<Type> Obj;

                const OtherType OTHER_VALUE(VALUE);

                Obj        mX(OTHER_VALUE, ALLOCATOR);
                const Obj& X = mX;

                ASSERT(mX.object().value() == VALUE);
                ASSERT( X.object().value() == VALUE);

                // test return-by-reference
                mX.object().value() = DEFAULT;
                ASSERT(mX.object().value() == DEFAULT);
                ASSERT( X.object().value() == DEFAULT);
            }
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    LOOP2_ASSERT(test, g_objectCount, 0 == g_objectCount);

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
