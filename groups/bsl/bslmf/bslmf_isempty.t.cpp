// bslmf_isempty.t.cpp                                                -*-C++-*-

#include <bslmf_isempty.h>

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component implements a relatively simple metafunction that computes a
// true/false result.  The only type that will yield a true result is an empty
// struct or class.  All other types, including fundamental types,
// enumerations, and non-empty classes will yield false.  Testing involves
// simply evaluating the metafunction with an empty class type and with a
// representative of each of the non-empty type categories
//-----------------------------------------------------------------------------
// [2] is_empty<TYPE>::value
//-----------------------------------------------------------------------------
// [3] USAGE EXAMPLE
// [1] BREATHING TEST
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
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consquently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_CMP_GNU)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180) // cv-qualifiers meaningless on function types
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct EmptyStruct {
};

class EmptyClassWithMembers
{
    // Empty struct with constructor, destructor, member function and static
    // data member, but no non-static data members other than a zero-width bit
    // field.

    int :0;  // Unnamed, zero-width bit-field
    static int s_data;

  public:
    explicit EmptyClassWithMembers(int i) { s_data = i; }
    ~EmptyClassWithMembers() { s_data = 0; }

    int data() const { return s_data; }
};

int EmptyClassWithMembers::s_data = 0;

class EmptyDerivedClass : public EmptyStruct
{
    // Empty class with (empty) base-class
};

struct NonEmptyStruct
{
    char d_data;
};

struct StructWithVirtualDestructor
{
    // Struct that would be empty except for having a virtual function
    virtual ~StructWithVirtualDestructor();
};

StructWithVirtualDestructor::~StructWithVirtualDestructor()
{
}

class ClassWithVirtualBase : virtual public EmptyStruct
{
    // Class that would be empty except for having a virtual base class.
};

class ClassWithNonEmptyBase : public NonEmptyStruct
{
    // Derived class with non-empty base class but no other members
};

union UnionType
{
    int    d_int;
    double d_double;
};

union TinyUnionType
{
    EmptyStruct data;
};

enum EnumType { ENUMERATOR };

typedef void function_type();

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Disable specific bde_verify warnings where practice of usage example may
// differ.

// BDE_VERIFY pragma: push

// BDE_VERIFY pragma: -FD01  // Function needs contract, not expository text
// BDE_VERIFY pragma: -FD02  // Banners diagnose badly unless we fix for FD01
// BDE_VERIFY pragma: -FD03  // no contract, so no ticks

///Example: Compute storage requirements for a type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to create a generic function that will allocate a record
// comprising a value of specified 'TYPE' and a description in the form of a
// null-terminated character string.  First, we declare the function prototype:
//..
    template <class TYPE>
    void *makeRecord(const TYPE& value, const char* description);
//..
// Next, we implement the function so that the copy of 'value' takes up no
// space if 'TYPE' is an empty class.  We manage this by computing a zero
// storage requirement if 'is_empty<TYPE>::value' is true:
//..
    #include <cstring>
    #include <new>

    template <class TYPE>
    void *makeRecord(const TYPE& value, const char* description)
    {
        // 'ValueSize' is computed at compile time.
        static const std::size_t ValueSize = bsl::is_empty<TYPE>::value ?
            0 : sizeof(TYPE);

        // Allocate memory for value and description
        const std::size_t MemSize = ValueSize + std::strlen(description) + 1;
        void *mem = ::operator new(MemSize);

        // Construct copy of value at front of allocated memory
        ::new(mem) TYPE(value);

        // Copy description into space following value.
        std::strcpy(static_cast<char*>(mem) + ValueSize, description);

        return mem;
    }
//..
// Finally, we use 'makeRecord' with both an empty and non-empty value type:
//..
    struct EmptyMarker { };

    int usageExample1()
    {
        void *record1 = makeRecord(9999, "four nines");
        // Value takes 'sizeof(int)' bytes at front of record.
        ASSERT(9999 == *static_cast<int*>(record1));
        ASSERT(0 == std::strcmp(static_cast<char*>(record1) + sizeof(int),
                                "four nines"));

        void *record2 = makeRecord(EmptyMarker(), "Empty");
        // Value takes no space at front of record.
        ASSERT(0 == std::strcmp(static_cast<char*>(record2), "Empty"));

        ::operator delete(record1);
        ::operator delete(record2);

        return 0;
    }
//..

// BDE_VERIFY pragma: pop

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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'is_empty<TYPE>::value'
        //
        // Concerns:
        //: 1 'is_empty<T>::value' is true if 'T' is an empty class,
        //:   an empty class with members but no non-static data members,
        //:   or an empty class that derives from another empty class.
        //: 2 'is_empty<T>::value' is false if 'T' is a fundamental type,
        //:   pointer type, reference type, or enum type.
        //: 3 'is_empty<T>::value' is false if 'T' is a non-empty class,
        //:   a class that inherits from a non-empty class, a class with
        //:   a virtual member function, or a class with a virtual base class.
        //: 4 'is_empty<cvq T>::value == is_empty<T>::value' for any cv
        //:   qualifier, 'cvq'.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'is_empty' with various types of
        //:   empty classes and verify that the 'value' member is true.
        //: 2 For concern 2, instantiate 'is_empty' with fundamental types,
        //:   pointer types, reference types, and enum types, and verify that
        //:   the 'value' member is false.
        //: 3 For concern 3, instantiate 'is_empty' with various kinds of
        //:   non-empty classes and verify that the 'value' member is false.
        //: 4 For concern 4, perform all of steps 1-3 using a macro that tests
        //:   every combination of const and volatile qualifiers on the type
        //:   being tested.
        //
        // Testing:
        //     is_empty<TYPE>::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'is_empty<TYPE>::value'"
                            "\n===============================\n");

        // Concern 4: Macro to add cv qualifiers
#define TEST_IS_EMPTY(T, EXP)                                 \
        ASSERT(EXP == bsl::is_empty<T>::value);               \
        ASSERT(EXP == bsl::is_empty<const T>::value);         \
        ASSERT(EXP == bsl::is_empty<volatile T>::value);      \
        ASSERT(EXP == bsl::is_empty<const volatile T>::value);

        // Concern 1: empty classes
        TEST_IS_EMPTY(EmptyStruct, true);
        TEST_IS_EMPTY(EmptyDerivedClass, true);
#if !defined(BSLS_PLATFORM_CMP_IBM)
    //  As a QoI issue, IBM does not treat 'EmptyClassWithMembers' as a type
    //  eligible for the empty-base optimization, and our trait implementation
    //  correctly recognizes that.
        TEST_IS_EMPTY(EmptyClassWithMembers, true);
#endif

        // Concern 2: Fundamental, pointer, reference, and enum types
        TEST_IS_EMPTY(void, false);
        TEST_IS_EMPTY(int, false);
        TEST_IS_EMPTY(double, false);
        TEST_IS_EMPTY(EmptyStruct*, false);
        TEST_IS_EMPTY(EmptyStruct&, false);
#if defined(BSLS_PLATFORM_CMP_IBM)
          // IBM xlC will not compile an attempt to cv-qualify a function type,
          // which should simply be ignored.
        ASSERT(false == bsl::is_empty<function_type>::value);
#else
        TEST_IS_EMPTY(function_type, false);
#endif
        TEST_IS_EMPTY(EnumType, false);
        TEST_IS_EMPTY(UnionType, false);
        // Tiny union doesn't work because is_class cannot be fully implemented
        // without compiler support.
//        TEST_IS_EMPTY(TinyUnionType, false);

        // Concern 3: non-empty classes
        TEST_IS_EMPTY(NonEmptyStruct, false);
        TEST_IS_EMPTY(StructWithVirtualDestructor, false);
        TEST_IS_EMPTY(ClassWithVirtualBase, false);
        TEST_IS_EMPTY(ClassWithNonEmptyBase, false);

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The basic functionality of 'is_empty' works as expected.
        //
        // Plan:
        //: 1 Instantiate 'is_empty' with several combinations of types.  For
        //:   each combination, verify that the static 'value' member has
        //:   the expected value and that the 'type' member exists and
        //:   has the same static 'value' member.
        //
        // Testing:
        //    BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT( bsl::is_empty<EmptyStruct>::value);
        ASSERT( bsl::is_empty<EmptyStruct>::type::value);
        ASSERT( bsl::is_empty<const EmptyStruct>::value);
        ASSERT(!bsl::is_empty<EmptyStruct&>::value);

        ASSERT(!bsl::is_empty<void>::value);
        ASSERT(!bsl::is_empty<void>::type::value);

        ASSERT(!bsl::is_empty<int>::value);
        ASSERT(!bsl::is_empty<int>::type::value);

        ASSERT(!bsl::is_empty<NonEmptyStruct>::value);
        ASSERT(!bsl::is_empty<NonEmptyStruct>::type::value);

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
