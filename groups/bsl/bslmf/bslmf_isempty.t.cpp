// bslmf_isempty.t.cpp                  -*-C++-*-

#include "bslmf_isempty.h"

#include <stdio.h>
#include <stdlib.h>

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component implements a relatively simple metafunction that computes
// a true/false result.  The only type that will yield a true result is an
// empty struct or class.  All other types, including fundemental types,
// enumerations, and non-empty classes will yield false.  Testing involves
// simply evaluating the metafunction with an empty class type and with a
// representative of each of the non-empty type categories
//-----------------------------------------------------------------------------
// [2] is_empty<TYPE>::value
//-----------------------------------------------------------------------------
// [3] USAGE EXAMPLE
// [1] BREATHING TEST
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

struct EmptyStruct {
};

class EmptyClassWithMembers
{
    // Empty struct with constructor, destructor, member function and
    // static data member, but no non-static data members other than a
    // zero-width bit field.

    int :0;  // Unnamed, zero-width bit-field
    static int s_data;

public:
    EmptyClassWithMembers(int i) { s_data = i; }
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
    int d_int;
    double d_double;
};

enum EnumType { ENUMERATOR };

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example: Compute storage requirements for a type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to create a generic function which will allocate a record
// comprising a value of specified 'TYPE' and a description in the form of a
// null-terminated character string.  First, we declare the function
// prototype:
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


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: That the usage example compiles and runs correctly.
        //
        // Plan: Copy usage example from component documentation.  Run and
        //   confirm expected results.
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
        // TESTING is_empty<TYPE>::value
        //
        // Concerns:
        //  1 'is_empty<T>::value' is true if 'T' is an empty class,
        //    an empty class with members but no non-static data members,
        //    or an empty class that derives from another empty class.
        //  2 'is_empty<T>::value' is false if 'T' is a fundamental type,
        //    pointer type, reference type, or enum type.
        //  3 'is_empty<T>::value' is false if 'T' is a non-empty class,
        //    a class that inherits from a non-empty class, a class with
        //    a virtual member function, or a class with a virtual base class.
        //  4 'is_empty<cvq T>::value == is_empty<T>::value' for any cv
        //    qualifier, 'cvq'.
        //
        // Plan:
        //  1 For concern 1, instantiate 'is_empty' with various types of
        //    empty classes and verify that the 'value' member is true.
        //  2 For concern 2, instantiate 'is_empty' with fundamental types,
        //    pointer types, reference types, and enum types, and verify that
        //    the 'value' member is false.
        //  3 For concern 3, instantiate 'is_empty' with various kinds of
        //    non-empty classes and verify that the 'value' member is false.
        //  4 For concern 4, perform all of steps 1-3 using a macro that tests
        //    every combination of const and volatile qualitiers on the type
        //    being tested.
        //
        // Testing:
        //     is_empty<TYPE>::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING is_empty<TYPE>::value"
                            "\n=============================\n");

        // Concern 4: Macro to add cv qualifiers
#define TEST_IS_EMPTY(T, EXP)                                 \
        ASSERT(EXP == bsl::is_empty<T>::value);               \
        ASSERT(EXP == bsl::is_empty<const T>::value);         \
        ASSERT(EXP == bsl::is_empty<volatile T>::value);      \
        ASSERT(EXP == bsl::is_empty<const volatile T>::value);

        // Concern 1: empty classes
        TEST_IS_EMPTY(EmptyStruct, true);
        TEST_IS_EMPTY(EmptyClassWithMembers, true);
        TEST_IS_EMPTY(EmptyDerivedClass, true);

        // Concern 2: Fundamental, pointer, reference, and enum types
        TEST_IS_EMPTY(void, false);
        TEST_IS_EMPTY(int, false);
        TEST_IS_EMPTY(double, false);
        TEST_IS_EMPTY(EmptyStruct*, false);
        TEST_IS_EMPTY(EmptyStruct&, false);
        TEST_IS_EMPTY(EnumType, false);
        // Union doesn't work because is_class cannot be fully implemented
        // without compiler support.
//      TEST_IS_EMPTY(UnionType, false);

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
        //  1 The basic functionality of 'is_empty' works as expected.
        //
        // Plan:
        //  1 Instantiate 'is_empty' with several combinations of types.  For
        //    each combination, verify that the static 'value' member has
        //    the expected value and that the 'type' member exists and
        //    has the same static 'value' member.
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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
