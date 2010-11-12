// bdeut_constructorproxy.t.cpp    -*-C++-*-

#include <bdeut_constructorproxy.h>

#include <bdema_testallocator.h>      // for testing only

#include <iostream>

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
//     o TestType_YesAllocator - a class that uses 'bdema' allocators.
//     o TestType_NoAllocator  - a class that does not use 'bdema' allocators.
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
//     o b_A  -> bdema_Allocator
//     o b_CP -> bdeut_ConstructorProxy
//
// CREATORS
// [ 1] bdeut_ConstructorProxy(b_A*);
// [ 1] bdeut_ConstructorProxy(const b_CP<OTHER>&, b_A*);
// [ 1] bdeut_ConstructorProxy(const OTHER&, b_A*);
// [ 1] ~bdeut_ConstructorProxy();
//
// MANIPULATORS
// [ 1] TYPE& object();
//
// ACCESSORS
// [ 1] const TYPE& object() const;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE 1
// [ 3] USAGE EXAMPLE 2

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int g_objectCount = 0;  // Global counter for number of objects
                               // currently constructed.  If this value is not
                               // zero when the test driver exits, that means
                               // the constructor proxy did not construct or
                               // destroy the objects correctly.

template <typename TYPE>
class TestType_Other_Template {
    // Template for generating the 'TestType_Other' type.  The 'TestType_Other'
    // type can be used to construct 'TestType_YesAllocator' and
    // 'TestType_NoAllocator' objects.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_ConstructorProxy' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

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

template <typename TYPE>
class TestType_YesAllocator_Template {
    // Template for generating the 'TestType_YesAllocator' type.  The
    // 'TestType_YesAllocator' type simulates a class that uses 'bdema'
    // allocators.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_ConstructorProxy' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

    // PRIVATE DATA MEMBERS
    TYPE             d_value;        // value of this object
    bdema_Allocator *d_allocator_p;  // allocator passed to constructor

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(TestType_YesAllocator_Template,
                                 bdealg_TypeTraitUsesBdemaAllocator);

    // CREATORS
    explicit TestType_YesAllocator_Template(
                                           bdema_Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    TestType_YesAllocator_Template(
                     const TestType_YesAllocator_Template&  original,
                     bdema_Allocator                       *basicAllocator = 0)
    : d_value(original.d_value)
    , d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    explicit TestType_YesAllocator_Template(
                      const TestType_Other_Template<TYPE>&  other,
                      bdema_Allocator                      *basicAllocator = 0)
    : d_value(other.value())
    , d_allocator_p(basicAllocator)
    {
        ++g_objectCount;
    }

    explicit TestType_YesAllocator_Template(
                                           const TYPE&      value,
                                           bdema_Allocator *basicAllocator = 0)
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

    bdema_Allocator* allocator() const
    {
        return d_allocator_p;
    }
};

template <typename TYPE>
class TestType_NoAllocator_Template {
    // Template for generating the 'TestType_NoAllocator' type.  The
    // 'TestType_NoAllocator' type simulates a class that does not use 'bdema'
    // allocators.
    //
    // Note that a template is used in order to be able to test
    // 'bdeut_ConstructorProxy' using 'TYPE's that are generated from templates
    // (some compilers sometimes behave strangely when combination of template
    // classes are used).

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
int operator==(const ValueType& lhs, const ValueType& rhs)
{
    return int(lhs) == int(rhs);
}

inline
int operator!=(const ValueType& lhs, const ValueType& rhs)
{
    return int(lhs) != int(rhs);
}

typedef TestType_YesAllocator_Template<ValueType>     TestType_YesAllocator;
typedef TestType_NoAllocator_Template<ValueType>      TestType_NoAllocator;
typedef TestType_Other_Template<ValueType>            TestType_Other;
typedef bdeut_ConstructorProxy<TestType_YesAllocator> Obj_YesAllocator;
typedef bdeut_ConstructorProxy<TestType_NoAllocator>  Obj_NoAllocator;
typedef bdeut_ConstructorProxy<TestType_Other>        Obj_Other;

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
    bdema_Allocator *d_allocator_p;  // allocator passed to constructor

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(SomeArbitraryClass,
                                 bdealg_TypeTraitUsesBdemaAllocator);

    // CREATORS
    explicit SomeArbitraryClass(bdema_Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
    }

    SomeArbitraryClass(const SomeArbitraryClass& original,
                       bdema_Allocator           *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
    }

    // ACCESSORS
    bdema_Allocator* allocator() const
    {
        return d_allocator_p;
    }
};
//..
// The 'SomeArbitraryClass' class may or may not declare the
// 'bdealg_TypeTraitUsesBdemaAllocator' trait.  The following code illustrates
// how a 'SomeArbitraryClass' object can be constructed using a constructor
// proxy for detecting this trait:
//..
void run()
    // Run usage example 1.
{
    bdema_TestAllocator allocator;

    bdeut_ConstructorProxy<SomeArbitraryClass> proxy(&allocator);

    SomeArbitraryClass& myObject = proxy.object();

    ASSERT(&allocator == myObject.allocator());
}
//..
// If 'SomeArbitraryClass' declared the 'bdealg_TypeTraitUsesBdemaAllocator'
// trait, then the specified 'myObject' will obtain its memory from the
// specified 'allocator'.  Otherwise, the specified 'allocator' will be
// ignored.
}  // end namespace UsageExample1

namespace UsageExample2 {
///Usage 2
///-------
// The following snippets of code illustrate the usage of this component in a
// more typical scenario.
//
// The 'MyContainer' class below contains an object of a templated 'TYPE':
//..
//  template <typename TYPE>
//  class MyContainer {
//      // A class that contains an object of the specified 'TYPE'.
//
//      // PRIVATE DATA MEMBERS
//      TYPE d_object;  // contained object
//
//    public:
//      // CREATORS
//      explicit MyContainer(bdema_Allocator *basicAllocator = 0);
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
//  template <typename TYPE>
//  MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
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
//  template <typename TYPE>
//  MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
//  : d_object(basicAllocator)
//  {
//  }
//..
// This implementation behaves as documented, but it will *not* compile
// successfully for 'TYPE's that do not have a constructor that takes a
// 'bdema_Allocator' pointer.  For example, the following declaration of
// 'container' will *not* compile:
//..
//  bdema_TestAllocator testAllocator;
//
//  MyContainer<int> container(&testAllocator);
//..
// The solution to this problem is to use a constructor proxy.  The following
// definition for 'MyContainer' uses a constructor proxy for 'TYPE':
//..

namespace WithoutAllocatorTrait {

template <typename TYPE>
class MyContainer {
    // A class that contains an object of the specified 'TYPE'.

    // PRIVATE DATA MEMBERS
    bdeut_ConstructorProxy<TYPE> d_proxy;

  public:
    // CREATORS
    explicit MyContainer(bdema_Allocator *basicAllocator = 0);
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
template <typename TYPE>
MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
: d_proxy(basicAllocator)
{
}

template <typename TYPE>
MyContainer<TYPE>::~MyContainer()
{
}
//..
//  The 'getObject' method of 'MyContainer' can be implemented as follows:
//..
template <typename TYPE>
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
    bdema_TestAllocator testAllocator;

    MyContainer<int> container(&testAllocator);
}
//..
// The specified 'testAllocator' will simply be ignored because 'int' does not
// use a 'bdema' allocator to supply memory.  Also, if there is a class defined
// as follows:
//..
class SomeArbitraryClassUsingAllocator {
    // Some arbitrary class that uses a 'bdema' allocator.

    // PRIVATE DATA MEMBERS
    bdema_Allocator *d_allocator_p;

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(SomeArbitraryClassUsingAllocator,
                                 bdealg_TypeTraitUsesBdemaAllocator);

    // CREATORS
    explicit SomeArbitraryClassUsingAllocator(
                                       bdema_Allocator *basicAllocator = 0)
    : d_allocator_p(bdema_Default::allocator(basicAllocator))
    {
    }

    // ACCESSORS
    bdema_Allocator* getAllocator() const
    {
        return d_allocator_p;
    }
};
//..
// then the following code *will* compile and run *without* an assertion
// failure:
//..
void run2()
{
    bdema_TestAllocator testAllocator;

    MyContainer<SomeArbitraryClassUsingAllocator> container(&testAllocator);

    ASSERT(&testAllocator == container.getObject().getAllocator());
}

}  // close namespace WithoutAllocatorTrait

//..
// Finally, since the 'MyContainer' class uses a 'bdema' allocator to supply
// memory, it would be nice to expose this trait to other containers.  This can
// be done by declaring the 'bdealg_TypeTraitUsesBdemaAllocator' trait to
// complete the definition of 'MyContainer':
//..

namespace WithAllocatorTrait {

class SomeArbitraryClassUsingAllocator {
    // Some arbitrary class that uses a 'bdema' allocator.

    // PRIVATE DATA MEMBERS
    bdema_Allocator *d_allocator_p;

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(SomeArbitraryClassUsingAllocator,
                                 bdealg_TypeTraitUsesBdemaAllocator);

    // CREATORS
    explicit SomeArbitraryClassUsingAllocator(
                                       bdema_Allocator *basicAllocator = 0)
    : d_allocator_p(bdema_Default::allocator(basicAllocator))
    {
    }

    // ACCESSORS
    bdema_Allocator* getAllocator() const
    {
        return d_allocator_p;
    }
};

template <typename TYPE>
class MyContainer {
    // A class that contains an object of the specified 'TYPE' and declares
    // the 'bdealg_TypeTraitUsesBdemaAllocator' trait.

    // PRIVATE DATA MEMBERS
    bdeut_ConstructorProxy<TYPE> d_proxy;

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(MyContainer,
                                 bdealg_TypeTraitUsesBdemaAllocator);

    // CREATORS
    explicit MyContainer(bdema_Allocator *basicAllocator = 0);
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

template <typename TYPE>
MyContainer<TYPE>::MyContainer(bdema_Allocator *basicAllocator)
: d_proxy(basicAllocator)
{
}

template <typename TYPE>
MyContainer<TYPE>::~MyContainer()
{
}

template <typename TYPE>
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
    bdema_TestAllocator testAllocator;

    MyContainer<MyContainer<SomeArbitraryClassUsingAllocator> >
                                            containedContainer(&testAllocator);

    ASSERT(&testAllocator
                 == containedContainer.getObject().getObject().getAllocator());
}

}  // close namespace WithAllocatorTrait

//..
}  // end namespace UsageExample2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;

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

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;

        UsageExample1::run();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR PROXY TEST
        //   This test thoroughly exercises the constructor proxy.
        //
        // Concerns:
        //   The constructor proxy must pass the allocator to the contained
        //   'TYPE' if 'TYPE' declares the 'bdealg_TypeTraitUsesBdemaAllocator'
        //   trait.  The objects must be correctly constructed and destructed.
        //   The 'object()' manipulator and accessor must return references to
        //   the contained object.
        //
        // Plan:
        //   Use the 'TestType_YesAllocator', 'TestType_NoAllocator', and the
        //   'TestType_Other' types to exercise all the possible combinations
        //   of constructing objects.
        //
        //       1. Constructing without value.
        //          a. 'TYPE' = 'TestType_YesAllocator'.
        //          b. 'TYPE' = 'TestType_NoAllocator'.
        //       2. Constructing with 'bdeut_ConstructorProxy<OTHER>'.
        //          a. 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_YesAllocator'.
        //          b. 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //          c. 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_NoAllocator'.
        //          d. 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //       3. Constructing with 'OTHER'.
        //          a. 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_YesAllocator'.
        //          b. 'TYPE'  = 'TestType_YesAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //          c. 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_NoAllocator'.
        //          d. 'TYPE'  = 'TestType_NoAllocator'.
        //             'OTHER' = 'TestType_Other'.
        //
        //  For each case, ensure that the allocator and value are handled
        //  correctly.
        //
        // Testing:
        //   bdeut_ConstructorProxy(b_A*);
        //   bdeut_ConstructorProxy(const b_CP<OTHER>&, b_A*);
        //   bdeut_ConstructorProxy(const OTHER&, b_A*);
        //   ~bdeut_ConstructorProxy();
        //   TYPE& object();
        //   const TYPE& object() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nConstructor Proxy Test"
                          << "\n======================" << endl;

        bdema_TestAllocator  testAllocator;
        bdema_Allocator     *ALLOCATOR = &testAllocator;

        const ValueType DEFAULT;
        const ValueType VALUE(123);

        ASSERT(VALUE != DEFAULT);

        if (verbose) cout << "\n1. Constructing without value." << endl;
        {
            if (veryVerbose) cout << "\ta. 'TYPE' = 'TestType_YesAllocator'."
                                  << endl;
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

            if (veryVerbose) cout << "\tb. 'TYPE' = 'TestType_NoAllocator'."
                                  << endl;
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

        if (verbose) cout << "\n2. Constructing with 'bdeut_ConstructorProxy"
                          << "<OTHER>'." << endl;
        {
            if (veryVerbose) cout << "\ta. 'TYPE'  = 'TestType_YesAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_YesAllocator'."
                          << endl;
            {
                typedef TestType_YesAllocator Type;
                typedef TestType_YesAllocator OtherType;

                typedef bdeut_ConstructorProxy<Type>      Obj;
                typedef bdeut_ConstructorProxy<OtherType> OriginalType;

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

            if (veryVerbose) cout << "\tb. 'TYPE'  = 'TestType_YesAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_Other'."
                          << endl;
            {
                typedef TestType_YesAllocator Type;
                typedef TestType_Other        OtherType;

                typedef bdeut_ConstructorProxy<Type>      Obj;
                typedef bdeut_ConstructorProxy<OtherType> OriginalType;

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

            if (veryVerbose) cout << "\tc. 'TYPE'  = 'TestType_NoAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_NoAllocator'."
                          << endl;
            {
                typedef TestType_NoAllocator Type;
                typedef TestType_NoAllocator OtherType;

                typedef bdeut_ConstructorProxy<Type>      Obj;
                typedef bdeut_ConstructorProxy<OtherType> OriginalType;

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

            if (veryVerbose) cout << "\td. 'TYPE'  = 'TestType_NoAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_Other'."
                          << endl;
            {
                typedef TestType_NoAllocator Type;
                typedef TestType_Other       OtherType;

                typedef bdeut_ConstructorProxy<Type>      Obj;
                typedef bdeut_ConstructorProxy<OtherType> OriginalType;

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

        if (verbose) cout << "\n3. Constructing with 'OTHER'." << endl;
        {
            if (veryVerbose) cout << "\ta. 'TYPE'  = 'TestType_YesAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_YesAllocator'."
                          << endl;
            {
                typedef TestType_YesAllocator        Type;
                typedef TestType_YesAllocator        OtherType;
                typedef bdeut_ConstructorProxy<Type> Obj;

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

            if (veryVerbose) cout << "\tb. 'TYPE'  = 'TestType_YesAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_Other'."
                          << endl;
            {
                typedef TestType_YesAllocator        Type;
                typedef TestType_Other               OtherType;
                typedef bdeut_ConstructorProxy<Type> Obj;

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

            if (veryVerbose) cout << "\tc. 'TYPE'  = 'TestType_NoAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_NoAllocator'."
                          << endl;
            {
                typedef TestType_NoAllocator         Type;
                typedef TestType_NoAllocator         OtherType;
                typedef bdeut_ConstructorProxy<Type> Obj;

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

            if (veryVerbose) cout << "\td. 'TYPE'  = 'TestType_NoAllocator'."
                          << endl << "\t   'OTHER' = 'TestType_Other'."
                          << endl;
            {
                typedef TestType_NoAllocator         Type;
                typedef TestType_Other               OtherType;
                typedef bdeut_ConstructorProxy<Type> Obj;

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
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    LOOP2_ASSERT(test, g_objectCount, 0 == g_objectCount);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
