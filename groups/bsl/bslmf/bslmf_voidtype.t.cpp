// bslmf_voidtype.t.cpp                                               -*-C++-*-
#include <bslmf_voidtype.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The metafunction defined in this component does no actual calculation; it
// simply provides a 'void' type member.  Testing it is trivial: simply
// instantiate it with 0 to 14 parameters and verify that it compiles.  The
// usefulness of this component is demonstrated in the usage examples.  Note
// that as this component is a primitive building block for metaprogramming, we
// wish to ensure there are no dependencies on any other 'bslmf' component for
// testing purposes, so that a dependency on 'bslmf_voidtype' has no further
// implication on levelization.  For this reason, we choose a testing strategy
// that does not depend on 'bslmf_issame'.
//-----------------------------------------------------------------------------
// [2] VoidType::type
// [3] bsl::void_t
// [4] BSLMF_VOIDTYPE
//-----------------------------------------------------------------------------
// [5] USAGE EXAMPLE
// [1] CONCERN: Test machinery works as expected

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
//              PLATFORM DEFECT TRACKING MACROS TO SUPPORT TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
# define BSLMF_VOIDTYPE_NO_ABOMINABLE_TYPES 1
#endif

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of 'VoidType' to determine whether a
// given type 'T' has a member type 'T::iterator'.  Our goal is to create a
// metafunction, 'HasIteratorType', such that 'HasIteratorType<T>::VALUE' is
// 'true' if 'T::iterator' is a valid type and 'false' otherwise.  This example
// is adapted from the paper proposing 'std::void_t' for the C++ Standard,
// N3911.
//
// First, we define the base-case metafunction that returns 'false':
//..
    template <class TYPE, class = void>
    struct HasIteratorType {
        enum { VALUE = false };
    };
//..
// Then, we create a partial specialization that uses 'VoidType' to probe for
// 'T::iterator':
//..
    template <class TYPE>
    struct HasIteratorType<TYPE, BSLMF_VOIDTYPE(typename TYPE::iterator)> {
        enum { VALUE = true };
    };
//..
// Now, we define a class that has a 'iterator' member and apply
// 'HasIteratorType' to it:
//..
    struct WithIterator {
        typedef short *iterator;
    };
//..
// Finally, we define a class that has an 'iterator' member and apply
// 'HasIteratorType' to it:
//..
    int usageExample1()
    {
        ASSERT(true == HasIteratorType<WithIterator>::VALUE);
//..
// Since 'WithIterator::iterator' is a valid type,
// 'BSLMF_VOIDTYPE(TYPE::iterator)' will be 'void' and the second
// 'HasIteratorType' template will be more specialized than the primary
// template and will thus get instantiated, yielding a 'VALUE' of 'true'.
//
// Conversely, if we try to instantiate 'HasIteratorType<int>', any use of
// 'BSLMF_VOIDTYPE(TYPE::iterator)' will result in a substitution failure.
// Fortunately, the Substitution Failure Is Not An Error (SFINAE) rule applies,
// so the code will compile, but the specialization is eliminated from
// consideration, resulting in the primary template being instantiated and
// yielding a 'VALUE' of 'false':
//..
        ASSERT(false == HasIteratorType<int>::VALUE);

        return 0;
    }
//..
//
///Usage Example 2
///- - - - - - - -
// This example demonstrates the use of 'VoidType' to probe for more than one
// type at once.  As in the previous example, we are defining a metafunction.
// We'll define 'IsTraversable<T>::VALUE' to be 'true' if 'T::iterator' and
// 'T::value_type' both exist.  First, we define a primary template that always
// yields 'false':
//..
    template <class TYPE, class = void>
    struct IsTraversable {
        enum { VALUE = false };
    };
//..
// Then, we create a partial specialization that uses 'BSLMF_VOIDTYPE' with two
// parameters:
//..
    template <class TYPE>
    struct IsTraversable<TYPE,
                         BSLMF_VOIDTYPE(typename TYPE::iterator,
                                        typename TYPE::value_type)> {
        enum { VALUE = true };
    };
//..
// Now, we define a type that meets the requirement for being traversable:
//..
    struct MyTraversable {
        typedef int  value_type;
        typedef int *iterator;
    };
//..
// Finally, the 'IsTraversable' metafunction yields 'true' for 'Traversable'
// but not for either 'WithIterator', which lacks a 'value_type' member, nor
// 'int', which lacks both 'iterator' and 'value_type' members:
//..
    int usageExample2()
    {
        ASSERT(true  == IsTraversable<MyTraversable>::VALUE);
        ASSERT(false == IsTraversable<WithIterator>::VALUE);
        ASSERT(false == IsTraversable<int>::VALUE);

        return 0;
    }
//..

//=============================================================================
//                  GLOBAL CLASS TEMPLATES FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE, class = void>
struct IsNonAbstractClass {
    // This trait derives provides a nested enumerator 'value' that is 'true'
    // if the (template parameter) 'TYPE' is a non-abstract class type, and is
    // 'false' otherwise.  For testing purposes, this trait uses the
    // 'BSLMF_VOIDTYPE' macro with more than one argument.

    enum { value = false };
};

template <class TYPE>
struct IsNonAbstractClass<TYPE, BSLMF_VOIDTYPE(int TYPE::*, TYPE(), TYPE[])> {
    enum { value = true };
};

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct MyStruct {
    int d_x;
};

enum MyEnum { E0, E1 };

union MyUnion {
    int d_x;
};

class Incomplete;

struct ConvertibleToVoidStar {
    operator void*() const { return 0; }
};

struct Abstract {
    virtual ~Abstract() = 0;
};

//=============================================================================
//                  GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
inline bool isVoidPointer(const TYPE&)
    // Return 'false'.  Note that the (non-template) function below will be
    // selected in overload resolution if (the template parameter) 'TYPE' is a
    // pointer to cv-unqalified 'void'.
{
    return false;
}

inline bool isVoidPointer(void *)
    // Return 'true'.
{
    return true;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'BSLMF_VOIDTYPE'
        //   Due to the C++98 requirements that 'typename' be used only where
        //   the compiler absolutely requires it (rather than allowing for
        //   redundant use) the 'BSLMF_VOIDTYPE' macro is to be used only in
        //   type-dependant contexts.  That means testing must be in terms of
        //   instantiating templates declared outside 'main'.  Note that the
        //   macro arguments supplied to 'BSLMF_VOIDTYPE' cannot contain any
        //   mix of non-type expressions:
        //: o literal values
        //: o string literals
        //: o enumerators
        //: o uninstantiated templates
        //
        // Concerns:
        //: 1 'BSLMF_VOIDTYPE(T1, T2... TN)' is an alias for 'void', for any
        //:   number of *valid* arguments, at least one of which shall depend
        //:   on the type parameters of the template being instantiated.
        //:
        //: 2 'BSLMF_VOIDTYPE(T1, T2... TN)' fails to instantiate in a SFINAE
        //:   compatible manner for all dialects of C++, when any of the types
        //:   'T1, T2, ... TN' forms an invalid (dependant) type expression.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'bslmf::VoidType<T1, T2... TN>::type'
        //:   with 0 to 14 parameters and verify that the result is type
        //:   'void'.  (C-1)
        //:
        //: 2 For concern 2, ensure that P-1 has a healthy mix of parameter
        //:   types.  (C-2)
        //
        // Testing:
        //   BSLMF_VOIDTYPE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'BSLMF_VOIDTYPE'"
                            "\n========================\n");

        ASSERT(!IsNonAbstractClass<      void    >::value);
        ASSERT(!IsNonAbstractClass<const void    >::value);
        ASSERT(!IsNonAbstractClass<      int     >::value);
        ASSERT(!IsNonAbstractClass<const int     >::value);
        ASSERT(!IsNonAbstractClass<const int&    >::value);
        ASSERT( IsNonAbstractClass<      MyStruct>::value);
        ASSERT( IsNonAbstractClass<const MyStruct>::value);
        ASSERT(!IsNonAbstractClass<      MyEnum  >::value);
        ASSERT( IsNonAbstractClass<Incomplete    >::value);
#if !defined(BSLS_PLATFORM_CMP_SUN) || BSLS_PLATFORM_CMP_VERSION >= 0x5130
        ASSERT(!IsNonAbstractClass<Abstract      >::value);
#else   // Expect the wrong answer on Sun CC compilers prior to CC 12.4
        ASSERT( IsNonAbstractClass<Abstract      >::value);
#endif
        ASSERT(!IsNonAbstractClass<void   ()     >::value);
        ASSERT(!IsNonAbstractClass<void(*)()     >::value);
        ASSERT(!IsNonAbstractClass<void(&)()     >::value);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::void_t<T>'
        //
        // Concerns:
        //: 1 'bsl::void_t<T1, T2... TN>' is an alias for 'void' (for any N).
        //:
        //: 2 The types used to instantiate 'bsl::void_t' can be any mix of
        //:   valid types, including (potentiallly cv-qualified):
        //:   o fundamental types (including 'void')
        //:   o enumerations
        //:   o classes
        //:   o unions
        //:   o incomplete classes
        //:   o pointers
        //:   o array pointers
        //:   o function pointers
        //:   o pointers-to-data-member
        //:   o pointers-to-function-member
        //:   o arrays of known bound
        //:   o arrays of unknown bound
        //:   o reference types
        //:   o function references
        //:   o array references
        //:   o regular function types
        //:   o abominable function types
        //:
        //: 3 Duplicate types are allowed.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'bsl::void_t<T1, T2... TN>::type'
        //:   with 0 to 14 parameters and verify that the result is type
        //:   'void'.  (C-1)
        //:
        //: 2 For concern 2, ensure that P-1 has a healthy mix of parameter
        //:   types.  (C-2)
        //
        // Testing:
        //   bsl::void_t
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::void_t<T>'"
                            "\n========================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

# define TEST(...) \
        ASSERT(isVoidPointer((bsl::void_t<__VA_ARGS__> *)0))

        // Preserve the structure of test case 2 for the initial test cases up
        // to 14 arguments.

        TEST();
        TEST(signed long long);
        TEST(signed long long,   signed long long);  // test for duplicates
        TEST(signed long long,   MyEnum);
        TEST(signed long long,   MyEnum,   MyStruct);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *
            );
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &
            );

        // As we can assume variadic templates, all the remaining type concerns
        // can be verified with just one more parameter pack.

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &,
             const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*,
             int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...),
             int() const, int(int...) volatile, int(int, int(&)()...) const
             );

# undef TEST
#else
        if (verbose) printf("Alias templates unsupported on this platform\n");
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'VoidType<T>::type'
        //
        // Concerns:
        //: 1 'bslmf::VoidType<T1, T2... TN>::type' is 'void' for N from 0 to
        //:   14.
        //:
        //: 2 The types used to instantiate 'bslmf::VoidType' can be any mix of
        //:   valid types, including (potentiallly cv-qualified):
        //:   o fundamental types (including 'void')
        //:   o enumerations
        //:   o classes
        //:   o unions
        //:   o incomplete classes
        //:   o pointers
        //:   o array pointers
        //:   o function pointers
        //:   o pointers-to-data-member
        //:   o pointers-to-function-member
        //:   o arrays of known bound
        //:   o arrays of unknown bound
        //:   o reference types
        //:   o function references
        //:   o array references
        //:   o regular function types
        //:   o abominable function types
        //:
        //: 3 Duplicate types are allowed.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'bslmf::VoidType<T1, T2... TN>::type'
        //:   with 0 to 14 parameters and verify that the result is type
        //:   'void'.  (C-1)
        //:
        //: 2 For concern 2, ensure that P-1 has a healthy mix of parameter
        //:   types.  (C-2)
        //
        // Testing:
        //   VoidType::type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'VoidType<T>::type'"
                            "\n===========================\n");

#define TEST(...) \
        ASSERT(isVoidPointer((bslmf::VoidType<__VA_ARGS__>::type *)0))

        // Incrementally test up to 14 types

        TEST();
        TEST(signed long long);
        TEST(signed long long,   signed long long);  // test for duplicates
        TEST(signed long long,   MyEnum);
        TEST(signed long long,   MyEnum,   MyStruct);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *
            );
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &);

# if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        // Additional test for 15 types, demonstrating variadic support

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &
            );

        // Test remaining types that may have interesting parses.  We do not
        // need to break this remaining list of types into smaller fragmnents
        // as variadic templates will consume as many arguments as we supply
        // (up to a compiler limit that must be at least 1000)

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &,
             const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*,
             int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...),
             int() const, int(int...) volatile, int(int, int(&)()...) const
             );
# else
        // Test remaining types that may have interesting parses, splitting
        // tests into sub-lists that fit the supported limit of 14 types.

        TEST(const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*);

        TEST(int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...));

#  if !defined(BSLMF_VOIDTYPE_NO_ABOMINABLE_TYPES)
        TEST(int() const, int(int...) volatile, int(int, int(&)()...) const);
#   endif
# endif

#undef TEST

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING THE TEST MACHINERY
        //
        // Concerns:
        //: 1 'isVoidPointer(arg)' returns 'true' if (and only if) 'arg' has
        //:   the type 'void *'.
        //:
        //: 2 'isVoidPointer(arg)' returns 'false' is 'arg' has the type of a
        //:   pointer to cv-qualfied 'void'.
        //:
        //: 3 'isVoidPointer(arg)' returns 'false' is 'arg' has the type of a
        //:   to any object type other than 'void'.
        //:
        //: 4 'isVoidPointer(arg)' returns 'false' is 'arg' has the type of a
        //:   to any function type.
        //:
        //: 5 'isVoidPointer(arg)' returns 'false' is 'arg' has the type of a
        //:   class with a conversion operator to 'void *'.
        //
        // Plan:
        //: 1 Call 'isVoidPointer' with a variety of null pointers, cast to
        //:   pointers to cv-qualified 'void', asserting that only the pointer
        //:   to unqualified 'void' returns 'true'.
        //:
        //: 2 Call 'isVoidPointer' with a variety of null pointers, cast to
        //:   types types that may fool a careless human into thinking they are
        //:   related to 'void *', and verify that all return 'false'.
        //:
        //: 3 Call 'isVoidPointer' with 0 and a null pointer literal, ensuring
        //:   that the result is 'false' in each case.
        //:
        //: 4 Call 'isVoidPointer' with an object of a call type implicitly
        //:   convertible to 'void *'.
        //
        // Testing:
        //   CONCERN: Test machinery works as expected
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING THE TEST MACHINERY"
                            "\n==========================\n");

        ASSERT( isVoidPointer((void *)0));
        ASSERT(!isVoidPointer((const void *)0));
        ASSERT(!isVoidPointer((volatile void *)0));
        ASSERT(!isVoidPointer((const volatile void *)0));

        ASSERT(!isVoidPointer((void **)0));
        ASSERT(!isVoidPointer((int *)0));
        ASSERT(!isVoidPointer((void *(*)[])0));
        ASSERT(!isVoidPointer((void  (*)())0));
        ASSERT(!isVoidPointer((void *(*)())0));

        ASSERT(!isVoidPointer((MyStruct *)0));

        const ConvertibleToVoidStar X = {};
        ASSERT(!isVoidPointer(X));
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
// Copyright 2019 Bloomberg Finance L.P.
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
