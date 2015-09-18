// bslalg_typetraits.t.cpp                                            -*-C++-*-
#include <bslalg_typetraits.h>

#include <bslma_testallocator.h>

#include <bslmf_isconvertible.h>
#include <bslmf_metaint.h>
#include <bslmf_removecvq.h>
#include <bslmf_if.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <new>
#include <utility>

using namespace BloombergLP;

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
const unsigned TRAIT_USESBSLMAALLOCATOR           = 0x0020;
const unsigned TRAIT_HASSTLITERATORS              = 0x0040;

// Traits group
const unsigned TRAIT_POD = (TRAIT_BITWISEMOVEABLE |
                            TRAIT_BITWISECOPYABLE |
                            TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR);
const unsigned TRAIT_EQPOD = (TRAIT_POD |
                              TRAIT_BITWISEEQUALITYCOMPARABLE);

// Traits detection
template <class TYPE, class TRAIT>
struct HasTrait {
    enum {
        VALUE = TRAIT::template Metafunction<TYPE>::value
    };

    typedef bslmf::MetaInt<VALUE> Type;
};

// Traits bit vector
template <class TYPE>
unsigned traitBits()
{
    unsigned result = TRAIT_NIL;

    result |= HasTrait<TYPE, bslalg::TypeTraitBitwiseMoveable>::VALUE
            ? TRAIT_BITWISEMOVEABLE
            : 0;
    result |= HasTrait<TYPE, bslalg::TypeTraitBitwiseCopyable>::VALUE
            ? TRAIT_BITWISECOPYABLE
            : 0;
    result |= HasTrait<TYPE,
                       bslalg::TypeTraitHasTrivialDefaultConstructor>::VALUE
            ? TRAIT_HASTRIVIALDEFAULTCONSTRUCTOR
            : 0;
    result |= HasTrait<TYPE, bslalg::TypeTraitBitwiseEqualityComparable>::VALUE
            ? TRAIT_BITWISEEQUALITYCOMPARABLE
            : 0;
    result |= HasTrait<TYPE, bslalg::TypeTraitPair>::VALUE
            ? TRAIT_PAIR
            : 0;
    result |= HasTrait<TYPE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE
            ? TRAIT_USESBSLMAALLOCATOR
            : 0;
    result |= HasTrait<TYPE, bslalg::TypeTraitHasStlIterators>::VALUE
            ? TRAIT_HASSTLITERATORS
            : 0;
    return result;
}

template <class TYPE>
struct Identity {
    // Use this struct to convert a cast-style type (e.g., 'void (*)(int)')
    // into a named type (e.g., 'void (*Type)(int)').
    // Example: 'Identity<void (*)(int)>::Type'.

    typedef TYPE Type;
};

// Test that 'traitBits<TYPE>()' returns the value 'TRAIT_BITS' for every
// combination of cv-qualified 'TYPE' and reference to 'TYPE'.
#define TRAIT_TEST(TYPE, TRAIT_BITS) do {                              \
    typedef Identity<TYPE >::Type Type;                                \
    typedef Type const            cType;                               \
    typedef Type volatile         vType;                               \
    typedef Type const volatile   cvType;                              \
    static const char *TypeName = #TYPE;                               \
    static const unsigned traits = traitBits<  Type>();                \
    LOOP2_ASSERT(TypeName, traits, traitBits<  Type>() == (TRAIT_BITS)); \
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
namespace bslmf {

// Being empty, 'my_Class0' would normally be implicitly bitwise moveable.
// Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class0> : bsl::false_type { };

// Being empty, 'my_Class1' would normally be implicitly bitwise moveable.
// Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class1> : bsl::false_type { };

}  // close bslmf namespace

namespace bslma {

template <> struct UsesBslmaAllocator<my_Class1> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

template <class T>
struct my_Class2
{
    // Class template that has nested type traits
    BSLALG_DECLARE_NESTED_TRAITS3(my_Class2,
                                bslalg::TypeTraitBitwiseCopyable,
                                bslalg::TypeTraitBitwiseMoveable,
                                bslalg::TypeTraitHasTrivialDefaultConstructor);
};

struct my_Class4
{
    // Class with no special traits but has conversion from 'void*'.
    // Used to check against false positives for 'bslma::Allocator*' traits.
    my_Class4(void*);
};

enum my_Enum
{
    // Enumeration type (is automatically bitwise copyable)
    MY_ENUM_0
};

namespace BloombergLP {
namespace bslmf {

// Being empty, 'my_Class4' would normally be implicitly bitwise moveable.
// Override, making it explicitly NOT bitwise moveable.
template <>
struct IsBitwiseMoveable<my_Class4> : bsl::false_type { };

}  // close bslmf namespace
}  // close enterprise namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BSLALG_TYPETRAITS_USAGE_EXAMPLE {

///Usage
///-----
// In this usage example, we show how to enable the 'bslma' allocator model for
// generic containers, by implementing simplified versions of the
// 'bslalg_constructorproxy' and 'bslalg_scalarprimitives' components.  The
// interested reader should refer to the documentation of those components.
//
///A generic container
///- - - - - - - - - -
// Suppose we want to implement a generic container of a parameterized 'TYPE',
// which may or may not follow the 'bslma' allocator model.  If it does, our
// container should pass an extra 'bslma::Allocator*' argument to copy
// construct a value; but if it does not, then passing this extra argument is
// going to generate a compile-time error.  It thus appears we need two
// implementations of our container.  This can be done more succinctly by
// encapsulating into the constructor some utilities which will, through a
// single interface, determine whether 'TYPE' has the trait
// 'bslalg::TypeTraitUsesBslmaAllocator' and copy-construct it accordingly.
//
// The container contains a single data member of the parameterized 'TYPE'.
// Since we are going to initialize this data member manually, we do not want
// it to be automatically constructed by the compiler.  For this reason, we
// encapsulate it in a 'bsls::ObjectBuffer'.
//..
    // my_genericcontainer.hpp          -*-C++-*-

    template <class TYPE>
    class MyGenericContainer {
        // This generic container type contains a single object, always
        // initialized, which can be replaced and accessed.  This container
        // always takes an allocator argument and thus follows the
        // 'bslalg::TypeTraitUsesBslmaAllocator' protocol.

        // PRIVATE DATA MEMBERS
        bsls::ObjectBuffer<TYPE> d_object;
//..
// Since the container offers a uniform interface that always takes an extra
// allocator argument, regardless of whether 'TYPE' does or not, we can declare
// it to have the 'bslalg::TypeTraitUsesBslmaAllocator' trait:
//..
      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(MyGenericContainer,
                                     bslalg::TypeTraitUsesBslmaAllocator);
//..
// For simplicity, we let the container contain only a single element, and
// require that an element always be initialized.
//..
        // CREATORS
        MyGenericContainer(const TYPE&       object,
                           bslma::Allocator *allocator = 0);
            // Create an container containing the specified 'object', using the
            // optionally specified 'allocator' to allocate memory.  If
            // 'allocator' is 0, the currently installed allocator is used.

        MyGenericContainer(const MyGenericContainer&  container,
                           bslma::Allocator          *allocator = 0);
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
        static void copyConstruct(TYPE             *location,
                                  const TYPE&       value,
                                  bslma::Allocator *allocator,
                                  bslalg::TypeTraitUsesBslmaAllocator)
            // Create a copy of the specified 'value' at the specified
            // 'location', using the specified 'allocator' to allocate memory.
        {
            new (location) TYPE(value, allocator);
        }
//..
// For types that don't use an allocator, we offer the following overload which
// will be selected if the type trait of 'TYPE' cannot be converted to
// 'bslalg::TypeTraitUsesBslmaAllocator'.  In that case, note that the type
// traits always inherits from 'bslalg::TypeTraitNil'.
//..
        template <class TYPE>
        static void copyConstruct(TYPE             *location,
                                  const TYPE&       value,
                                  bslma::Allocator * /* allocator */,
                                  bslalg::TypeTraitNil)
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
        static void copyConstruct(TYPE             *location,
                                  const TYPE&       value,
                                  bslma::Allocator *allocator)
            // Create a copy of the specified 'value' at the specified
            // 'location', optionally using the specified 'allocator' to supply
            // memory if the parameterized 'TYPE' possesses the
            // 'bslalg::TypeTraitUsesBslmaAllocator'.
        {
            copyConstruct(location, value, allocator,
                typename bslmf::If<HasTrait<TYPE,
                              bslalg::TypeTraitUsesBslmaAllocator>::VALUE,
                          bslalg::TypeTraitUsesBslmaAllocator,
                          bslalg::TypeTraitNil>::Type());
        }

    };
//..
///Generic container implementation
///- - - - - - - - - - - - - - - -
// With these utilities, we can now implement 'MyGenericContainer'.
//..
    // CREATORS
    template <class TYPE>
    MyGenericContainer<TYPE>::MyGenericContainer(const TYPE&       object,
                                                 bslma::Allocator *allocator)
    {
        my_GenericContainerUtil::copyConstruct(&d_object.object(),
                                               object,
                                               allocator);
    }

    template <class TYPE>
    MyGenericContainer<TYPE>::MyGenericContainer(
                                          const MyGenericContainer&  container,
                                          bslma::Allocator          *allocator)
    {
        my_GenericContainerUtil::copyConstruct(&d_object.object(),
                                               container.object(),
                                               allocator);
    }
//..
// Note that all this machinery only affects the constructors, and not the
// destructor which only invokes the destructor of 'd_object'.
//..
    template <class TYPE>
    MyGenericContainer<TYPE>::~MyGenericContainer()
    {
        (&d_object.object())->~TYPE();
    }
//..
// To finish, the accessors and manipulators are trivially implemented.
//..
    // MANIPULATORS
    template <class TYPE>
    TYPE& MyGenericContainer<TYPE>::object()
    {
        return d_object.object();
    }

    // ACCESSORS
    template <class TYPE>
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
// one has the 'bslalg::TypeTraitUsesBslmaAllocator' trait and the other
// hasn't:
//..
    bslma::Allocator *allocSlot;

    struct MyTestTypeWithBslmaAllocatorTraits {
        // Class with declared traits.  Calling copy constructor without an
        // allocator will compile, but will not set 'allocSlot'.

        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(
                             MyTestTypeWithBslmaAllocatorTraits,
                             BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        MyTestTypeWithBslmaAllocatorTraits() {}

        MyTestTypeWithBslmaAllocatorTraits(
                         const MyTestTypeWithBslmaAllocatorTraits&,
                         bslma::Allocator                           *allocator)
        {
            allocSlot = allocator;
        }
    };

    struct MyTestTypeWithNoBslmaAllocatorTraits {
        // Class with no declared traits.  Calling copy constructor without
        // an allocator will not set the 'allocSlot', but passing it by mistake
        // will set it.

        // CREATORS
        MyTestTypeWithNoBslmaAllocatorTraits() {}

        MyTestTypeWithNoBslmaAllocatorTraits(
                      const MyTestTypeWithNoBslmaAllocatorTraits &,
                      bslma::Allocator                              *allocator)
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
        bslma::TestAllocator ta0;
        bslma::TestAllocator ta1;
//..
// With 'MyTestTypeWithNoBslmaAllocatorTraits', the slot should never be set.
//..
        MyTestTypeWithNoBslmaAllocatorTraits x;

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x0(x);
        ASSERT(&ta0 == allocSlot);

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x1(x, &ta1);
        ASSERT(&ta0 == allocSlot);
//..
// With 'MyTestTypeWithBslmaAllocatorTraits', the slot should be set to the
// allocator argument, or to 0 if not specified:
//..
        MyTestTypeWithBslmaAllocatorTraits y;

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y0(y);
        ASSERT(0 == allocSlot);

        allocSlot = &ta0;
        MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y1(y, &ta1);
        ASSERT(&ta1 == allocSlot);

        return 0;
    }
//..

}  // close namespace BSLALG_TYPETRAITS_USAGE_EXAMPLE
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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

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

        using namespace BSLALG_TYPETRAITS_USAGE_EXAMPLE;
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
        TRAIT_TEST(bsls::Types::Int64, TRAIT_EQPOD);
        TRAIT_TEST(bsls::Types::Uint64, TRAIT_EQPOD);
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
        TRAIT_TEST(my_Class1, TRAIT_USESBSLMAALLOCATOR);
        TRAIT_TEST(my_Class2<int>, TRAIT_POD);

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
