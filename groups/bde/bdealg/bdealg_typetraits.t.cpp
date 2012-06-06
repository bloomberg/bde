// bdealg_typetraits.t.cpp                  -*-C++-*-

#include <bdealg_typetraits.h>

#include <bslalg_typetraitnil.h>
#include <bslma_testallocator.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_new.h>

using namespace BloombergLP;
using namespace bsl;

#if BSL_LEGACY == 0
#define bdealg_TypeTraits bslalg_TypeTraits
#define bdealg_HasTrait bslalg_HasTrait
#define BDEALG_DECLARE_NESTED_TRAITS(T, TRAITS)                               \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAITS)
#define bdealg_TypeTraitsGroupPod bslalg_TypeTraitsGroupPod
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// This test driver is not complete.  In particular all the testing is
// performed in the breathing test.  There is a very good infrastructure for
// testing, computing the traits of a type as a bit-field, which should be
// reused systematically.
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

# define LOOP_ASSERT(I,X) \
    if (!(X)) { printf("%s = %s\n", #I, (I)); aSsErT(!(X), #X, __LINE__); }

# define LOOP2_ASSERT(I,J,X) \
    if (!(X)) { printf("%s = %s, %s = %d\n", #I, (I), #J, (J));  \
                aSsErT(!(X), #X, __LINE__); }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) printf("%s = %d\n", #X, (X)); // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Numeric values corresponding to trait types.
// Can be ANDed together to represent multiple traits.
const unsigned TRAIT_NIL                          = 0x0000;
const unsigned TRAIT_BITWISEMOVEABLE              = 0x0001;
const unsigned TRAIT_BITWISECOPYABLE              = 0x0002;
const unsigned TRAIT_BITWISEEQUALITYCOMPARABLE    = 0x0004;
const unsigned TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR = 0x0008;
const unsigned TRAIT_PAIR                         = 0x0010;
const unsigned TRAIT_USESBDEMAALLOCATOR           = 0x0020;
const unsigned TRAIT_HASSTLITERATORS              = 0x0040;

// Traits group
const unsigned TRAIT_POD = (TRAIT_BITWISEMOVEABLE |
                            TRAIT_BITWISECOPYABLE |
                            TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR);
const unsigned TRAIT_EQPOD = (TRAIT_POD |
                              TRAIT_BITWISEEQUALITYCOMPARABLE);

template <typename TYPE>
unsigned traitBits()
{
    unsigned result = TRAIT_NIL;

    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitBitwiseMoveable>::VALUE
            ? TRAIT_BITWISEMOVEABLE
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitBitwiseCopyable>::VALUE
            ? TRAIT_BITWISECOPYABLE
            : 0;
    result |= bdealg_HasTrait<TYPE,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
            ? TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR
            : 0;
    result |= bdealg_HasTrait<TYPE,
                              bslalg_TypeTraitBitwiseEqualityComparable>::VALUE
            ? TRAIT_BITWISEEQUALITYCOMPARABLE
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitPair>::VALUE
            ? TRAIT_PAIR
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE
            ? TRAIT_USESBDEMAALLOCATOR
            : 0;
    result |= bdealg_HasTrait<TYPE, bslalg_TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASSTLITERATORS
            : 0;
    return result;
}

template <typename TYPE>
struct Identity
{
    // Use this struct to convert a cast-style type (e.g., 'void (*)(int)')
    // into a named type (e.g., 'void (*Type)(int)').
    // Example: 'Identity<void (*)(int)>::Type'.

    typedef TYPE Type;
};

// Test that 'traitBits<TYPE>()' returns the value 'TRAIT_BITS' for every
// combination of cv-qualified 'TYPE' and reference to 'TYPE'.
#define TRAIT_TEST(TYPE, TRAIT_BITS) do {                            \
    typedef Identity<TYPE >::Type Type;                              \
    typedef Type const          cType;                               \
    typedef Type volatile       vType;                               \
    typedef Type const volatile cvType;                              \
    static const char *TypeName = #TYPE;                               \
    static const unsigned traits = traitBits<  Type>();                \
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() == TRAIT_BITS); \
    LOOP2_ASSERT(TypeName, traits, traitBits< cType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits< vType>() == traits);     \
    LOOP2_ASSERT(TypeName, traits, traitBits<cvType>() == traits);     \
} while (0)

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

struct my_Class0
{
    // Class with no defined traits.
};

struct my_Class1
{
    // Class that uses explicitly-specialized type traits.
};

namespace BloombergLP {

    template <>
        struct bdealg_TypeTraits<my_Class1>
        : bslalg_TypeTraitUsesBslmaAllocator {};

}  // close namespace BloombergLP

template <class T>
struct my_Class2
{
    // Class template that has nested type traits
    BDEALG_DECLARE_NESTED_TRAITS(my_Class2,
                                 BloombergLP::bdealg_TypeTraitsGroupPod);
};

struct my_Class3
{
    // Class with no declared traits but implicitly uses allocator.
    my_Class3(bslma_Allocator*);
};

struct my_Class4
{
    // Class with no special traits but has conversion from 'void*'.
    // Used to check against false positives for 'bslma_Allocator*' traits.
    my_Class4(void*);
};

struct my_Class5
{
    // Class with no special traits but has conversion from anything.  Used
    // the check against false positives for nested traits and
    // 'bslma_Allocator*' traits.
    template <class T> my_Class5(const T& t);
    template <class T> my_Class5(const volatile T& t);

#if defined(BSLS_PLATFORM__CMP_IBM) || defined(BSLS_PLATFORM__OS_LINUX)
    // Workaround for AIX xlC 6.0 and and Linux gcc compilers.  Without this
    // declaration, the compiler tries to instantiate the templated
    // constructors when probing for 'bslma_Allocator*' conversions.  This
    // declaration short-circuits the traits-sniffing logic so that it will
    // not probe for conversion from 'bslma_Allocator*'.
    BDEALG_DECLARE_NESTED_TRAITS(my_Class5, bslalg_TypeTraitNil);
#endif
};

// Implementations of my_Class5 constructors.  If these constructors are
// actually instantiated, they will fail to compile.
template <class T> my_Class5::my_Class5(const T& t) { t->foo(); }
template <class T> my_Class5::my_Class5(const volatile T& t) { t->foo(); }

enum my_Enum
{
    // Enumeration type (is automatically bitwise copyable)
    MY_ENUM_0
};

//=============================================================================
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BDEALG_TYPETRAITS_USAGE_EXAMPLE {

///Usage
///-----
// In this usage example, we show how to enable the 'bdema' allocator model for
// generic containers, by implementing simplified versions of the
// 'bdealg_constructorproxy' and 'bdealg_scalarprimitives' components.  The
// interested reader should refer to the documentation of those components.
//
///A generic container
///- - - - - - - - - -
// Suppose we want to implement a generic container of a parameterized 'TYPE',
// which may or may not follow the 'bdema' allocator model.  If it does, our
// container should pass an extra 'bslma_Allocator*' argument to copy construct
// a value; but if it does not, then passing this extra argument is going to
// generate a compile-time error.  It thus appears we need two implementations
// of our container.  This can be done more succinctly by encapsulating into
// the constructor some utilities which will, through a single interface,
// determine whether 'TYPE' has the trait 'bslalg_TypeTraitUsesBslmaAllocator'
// and copy-construct it accordingly.
//
// The container contains a single data member of the parameterized 'TYPE'.
// Since we are going to initialize this data member manually, we do not want
// it to be automatically constructed by the compiler.  For this reason, we
// encapsulate it in a 'bsls_ObjectBuffer'.
//..
    // my_genericcontainer.hpp          -*-C++-*-

    template <class TYPE>
    class MyGenericContainer {
        // This generic container type contains a single object, always
        // initialized, which can be replaced and accessed.  This container
        // always takes an allocator argument and thus follows the
        // 'bslalg_TypeTraitUsesBslmaAllocator' protocol.

        // PRIVATE DATA MEMBERS
        bsls_ObjectBuffer<TYPE> d_object;
//..
// Since the container offers a uniform interface that always takes an extra
// allocator argument, regardless of whether 'TYPE' does or not, we can declare
// it to have the 'bslalg_TypeTraitUsesBslmaAllocator' trait:
//..
      public:
        // TRAITS
        BDEALG_DECLARE_NESTED_TRAITS(MyGenericContainer,
                                     bslalg_TypeTraitUsesBslmaAllocator);
//..
// For simplicity, we let the container contain only a single element, and
// require that an element always be initialized.
//..
        // CREATORS
        MyGenericContainer(const TYPE& object, bslma_Allocator *allocator = 0);
            // Create an container containing the specified 'object', using the
            // optionally specified 'allocator' to allocate memory.  If
            // 'allocator' is 0, the currently installed allocator is used.

        MyGenericContainer(const MyGenericContainer&  container,
                           bslma_Allocator           *allocator = 0);
            // Create an container containing the same object as the specified
            // 'container', using the optionally specified 'allocator' to
            // allocate memory.  If 'allocator' is 0, the currently installed
            // allocator is used.

        ~MyGenericContainer();
            // Destroy this container.
//..
// We can also allow the container to change the object it contains, by
// granting modifiable as well as non-modifiable access to this object:
//..
        // MANIPULATORS
        TYPE& object();

        // ACCESSORS
        const TYPE& object() const;
    };
//..
///Using the type traits
///- - - - - - - - - - -
// The challenge in the implementation lays in using the traits of the
// contained 'TYPE' to determine whether to pass the allocator argument to its
// copy constructor.  We rely here on a property of templates that
// templates are not compiled (and thus will not generate compilation errors)
// until they are instantiated.  Hence, we can use two function templates, and
// let the overloading resolution (based on the nested traits) decide which to
// instantiate.  The generic way to create an object, passing through all
// arguments (value and allocator) is as follows.  For brevity and to avoid
// breaking the flow of this example, we have embedded the function definition
// into the class.
//..
    // my_genericcontainer.cpp          -*-C++-*-

    struct my_GenericContainerUtil {
        // This 'struct' provides a namespace for utilities implementing the
        // allocator pass-through mechanism in a generic container.

        template <class TYPE>
        static void copyConstruct(TYPE            *location,
                                  const TYPE&      value,
                                  bslma_Allocator *allocator,
                                  bslalg_TypeTraitUsesBslmaAllocator)
            // Create a copy of the specified 'value' at the specified
            // 'location', using the specified 'allocator' to allocate memory.
        {
            new (location) TYPE(value, allocator);
        }
//..
// For types that don't use an allocator, we offer the following overload which
// will be selected if the type trait of 'TYPE' cannot be converted to
// 'bslalg_TypeTraitUsesBslmaAllocator'.  In that case, note that the type
// traits always inherits from 'bslalg_TypeTraitNil'.
//..
        template <class TYPE>
        static void copyConstruct(TYPE            *location,
                                  const TYPE&      value,
                                  bslma_Allocator *allocator,
                                  bslalg_TypeTraitNil)
            // Create a copy of the specified 'value' at the specified
            // 'location'.  Note that the specified 'allocator' is ignored.
        {
            new (location) TYPE(value);
        }
//..
// And finally, this function will instantiate the type trait and pass it to
// the appropriately (compiler-)chosen overload:
//..
        template <class TYPE>
        static void copyConstruct(TYPE            *location,
                                  const TYPE&      value,
                                  bslma_Allocator *allocator)
            // Create a copy of the specified 'value' at the specified
            // 'location', optionally using the specified 'allocator' to supply
            // memory if the parameterized 'TYPE' possesses the
            // 'bslalg_TypeTraitUsesBslmaAllocator'.
        {
            copyConstruct(location, value, allocator,
                          bdealg_TypeTraits<TYPE>());
        }

    };
//..
///Generic container implementation
///- - - - - - - - - - - - - - - -
// With these utilities, we can now implement 'MyGenericContainer'.
//..
    // CREATORS
    template <typename TYPE>
    MyGenericContainer<TYPE>::MyGenericContainer(const TYPE&      object,
                                                 bslma_Allocator *allocator)
    {
        my_GenericContainerUtil::copyConstruct(&d_object.object(),
                                               object,
                                               allocator);
    }

    template <typename TYPE>
    MyGenericContainer<TYPE>::MyGenericContainer(
                                          const MyGenericContainer&  container,
                                          bslma_Allocator           *allocator)
    {
        my_GenericContainerUtil::copyConstruct(&d_object.object(),
                                               container.object(),
                                               allocator);
    }
//..
// Note that all this machinery only affects the constructors, and not the
// destructor which only invokes the destructor of 'd_object'.
//..
    template <typename TYPE>
    MyGenericContainer<TYPE>::~MyGenericContainer()
    {
        (&d_object.object())->~TYPE();
    }
//..
// To finish, the accessors and manipulators are trivially implemented.
//..
    // MANIPULATORS
    template <typename TYPE>
    TYPE& MyGenericContainer<TYPE>::object()
    {
        return d_object.object();
    }

    // ACCESSORS
    template <typename TYPE>
    const TYPE& MyGenericContainer<TYPE>::object() const
    {
        return d_object.object();
    }
//..
///Usage verification
///- - - - - - - - -
// We can check that our container actually forwards the correct allocator to
// its contained instance with a very simple test apparatus, consisting of two
// classes which have exactly the same signature and implementation except that
// one has the 'bslalg_TypeTraitUsesBslmaAllocator' trait and the other has
// not:
//..
    bslma_Allocator *allocSlot;

    struct MyTestTypeWithBdemaAllocatorTraits {
        // Class with declared traits.  Calling copy constructor without an
        // allocator will compile, but will not set 'allocSlot'.

        // TRAITS
        BDEALG_DECLARE_NESTED_TRAITS(MyTestTypeWithBdemaAllocatorTraits,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        MyTestTypeWithBdemaAllocatorTraits() {}

        MyTestTypeWithBdemaAllocatorTraits(
                         const MyTestTypeWithBdemaAllocatorTraits&,
                         bslma_Allocator                            *allocator)
        {
            allocSlot = allocator;
        }
    };

    struct MyTestTypeWithNoBdemaAllocatorTraits {
        // Class with no declared traits.  Calling copy constructor without
        // an allocator will not set the 'allocSlot', but passing it by mistake
        // will set it.

        // CREATORS
        MyTestTypeWithNoBdemaAllocatorTraits() {}

        MyTestTypeWithNoBdemaAllocatorTraits(
                      const MyTestTypeWithNoBdemaAllocatorTraits &,
                      bslma_Allocator                               *allocator)
        {
            allocSlot = allocator;
        }
    };
//..
// Our verification program simply instantiate several 'MyGenericContainer'
// templates with the two test types above, and checks that the allocator
// slot is as expected:
//..
    int usageExample()
    {
        bslma_TestAllocator ta0;
        bslma_TestAllocator ta1;
//..
// With 'MyTestTypeWithNoBdemaAllocatorTraits', the slot should never be set.
//..
        MyTestTypeWithNoBdemaAllocatorTraits x;

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithNoBdemaAllocatorTraits> x0(x);
        ASSERT(&ta0 == allocSlot);

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithNoBdemaAllocatorTraits> x1(x, &ta1);
        ASSERT(&ta0 == allocSlot);
//..
// With 'MyTestTypeWithBdemaAllocatorTraits', the slot should be set to the
// allocator argument, or to 0 if not specified:
//..
        MyTestTypeWithBdemaAllocatorTraits y;

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithBdemaAllocatorTraits> y0(y);
        ASSERT(0 == allocSlot);

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithBdemaAllocatorTraits> y1(y, &ta1);
        ASSERT(&ta1 == allocSlot);

        return 0;
    }
//..

} // close namespace BDEALG_TYPETRAITS_USAGE_EXAMPLE
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============");

        using namespace BDEALG_TYPETRAITS_USAGE_EXAMPLE;
        usageExample();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        // Nil traits
        TRAIT_TEST(my_Class0, TRAIT_NIL);
        TRAIT_TEST(my_Class4, TRAIT_NIL);
        TRAIT_TEST(my_Class5, TRAIT_NIL);

        // Reference traits.  (Cannot use TRAIT_TEST for references.
        ASSERT(traitBits<int&>() == TRAIT_NIL);
        ASSERT(traitBits<const int&>() == TRAIT_NIL);
        ASSERT(traitBits<volatile int&>() == TRAIT_NIL);
        ASSERT(traitBits<const volatile int&>() == TRAIT_NIL);
        ASSERT(traitBits<char*&>() == TRAIT_NIL);
        ASSERT(traitBits<char* const&>() == TRAIT_NIL);
        ASSERT(traitBits<const char*&>() == TRAIT_NIL);
        ASSERT(traitBits<const char* const&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Enum&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Class1&>() == TRAIT_NIL);
        ASSERT(traitBits<my_Class2<int>&>() == TRAIT_NIL);

        // Autodetected fundamental traits
        TRAIT_TEST(char, TRAIT_EQPOD);
        TRAIT_TEST(unsigned char, TRAIT_EQPOD);
        TRAIT_TEST(signed char, TRAIT_EQPOD);
        TRAIT_TEST(short, TRAIT_EQPOD);
        TRAIT_TEST(unsigned short, TRAIT_EQPOD);
        TRAIT_TEST(int, TRAIT_EQPOD);
        TRAIT_TEST(unsigned int, TRAIT_EQPOD);
        TRAIT_TEST(bsls_Types::Int64, TRAIT_EQPOD);
        TRAIT_TEST(bsls_Types::Uint64, TRAIT_EQPOD);
        TRAIT_TEST(float, TRAIT_EQPOD);
        TRAIT_TEST(double, TRAIT_EQPOD);
        TRAIT_TEST(char*, TRAIT_EQPOD);
        TRAIT_TEST(const char*, TRAIT_EQPOD);
        TRAIT_TEST(void*, TRAIT_EQPOD);
        TRAIT_TEST(const void*, TRAIT_EQPOD);
        TRAIT_TEST(void* const, TRAIT_EQPOD);
        TRAIT_TEST(my_Enum, TRAIT_EQPOD);
        TRAIT_TEST(int (*)(int), TRAIT_EQPOD);
        TRAIT_TEST(int (my_Class1::*)(int), TRAIT_EQPOD);

        // Explicit traits
        TRAIT_TEST(my_Class1, TRAIT_USESBDEMAALLOCATOR);
        TRAIT_TEST(my_Class2<int>, TRAIT_POD);

        // Auto-detect bdema traits
        TRAIT_TEST(my_Class3, TRAIT_USESBDEMAALLOCATOR);

        // Implied traits ???

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
