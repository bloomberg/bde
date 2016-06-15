// bslmf_usesallocatorargt.t.cpp                                      -*-C++-*-

#include <bslmf_usesallocatorargt.h>

#include <bslmf_allocatorargt.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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

// Class that uses nested trait declaration
template <class ALLOC>
class TypeC {
    ALLOC d_allocator;
    // Additional data members go here

public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TypeC, bslmf::UsesAllocatorArgT);

    // CREATORS
    TypeC() : d_allocator() { }
    TypeC(bsl::allocator_arg_t, const ALLOC& alloc) : d_allocator(alloc) { }
    TypeC(const TypeC& other) : d_allocator() { (void) other; }
    TypeC(bsl::allocator_arg_t, const ALLOC& alloc, const TypeC& other)
         : d_allocator(alloc) { (void) other; }
    // Etc..  Destructor and assignment not shown

    // ACCESSORS
    const ALLOC& allocator() const { return d_allocator; }
    // etc..
};


//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: A discriminated union
/// - - - - - - - - - - - - - - - -
// In this example, we create a class template for a discriminated union
// (i.e., a simplified variant) type that can hold a value of two different
// types, but only one at a time (like a union) as well as a tag indicating
// which type is in use. It holds an allocator, which is passed to the
// constructors of the union elements.  The allocator must conform to the STL
// allocator requirements and must be the same allocator type for both
// elements of the union. However, the union elements can take the allocator
// argument either as the last argument to each constructor or as the second
// argument preceded by an argument of type 'bsl::allocator_arg_t'.
//
// First, we define the interface to our discriminated union:
//..
    #include <bslmf_allocatorargt.h>
    #include <new>

    namespace BloombergLP {

    template <class T1, class T2, class ALLOC>
    class DiscriminatedUnion {
    public:
        enum Tag { TAG_EMPTY, TAG_FIRST, TAG_SECOND };

    private:
        union {
            char   d_buffer1[sizeof(T1)];
            char   d_buffer2[sizeof(T2)];
            void  *d_align1; // For alignment only
            double d_align2; // For alignment only
        }     d_buffer;
        ALLOC d_allocator;
        Tag   d_tag;

        template <class TYPE>
        void constructObj(const TYPE& val,
                          bsl::true_type /* uses allocator_arg_t */);
            // Construct an object of 'TYPE' at the address of 'd_buffer'
            // using the extended copy constructor invocation
            // 'TYPE(bsl::allocator_arg, d_allocator, val)'.

        template <class TYPE>
        void constructObj(const TYPE& val,
                          bsl::false_type /* uses allocator_arg_t */);
            // Construct an object of 'TYPE' at the address of 'd_buffer'
            // using the extended copy constructor invocation
            // 'TYPE(val, d_allocator)'.

    public:
        // CREATORS
        DiscriminatedUnion(const T1&    val,
                           const ALLOC& alloc = ALLOC());
            // Create a union containing an element of type 'T1'

        DiscriminatedUnion(const T2&    val,
                           const ALLOC& alloc = ALLOC());
            // Create a union containing an element of type 'T2'

        // Copy constructor, destructor, and assignment operators not shown.

        // MANIPULATORS
        T1& firstValue();
            // Return a modifiable reference to the first element.  The
            // behavior is undefined unless 'tag() == TAG_FIRST'.

        T2& secondValue();
            // Return a modifiable reference to the second element.  The
            // behavior is undefined unless 'tag() == TAG_SECOND'.

        // ACCESSORS
        Tag tag() const;
            // Return a tag indicating what kind of element is current.

        const T1& firstValue() const;
            // Return a const reference to the first element.  The behavior is
            // undefined unless 'tag() == TAG_FIRST'.

        const T2& secondValue() const;
            // Return a const reference to the second element.  The behavior is
            // undefined unless 'tag() == TAG_SECOND'.
    };
//..
// Next, we implement the two 'constructObj' functions:
//..
    template <class T1, class T2, class ALLOC>
    template <class TYPE>
    void DiscriminatedUnion<T1,T2,ALLOC>::constructObj(const TYPE& val,
                                                       bsl::true_type)
    {
        ::new(&d_buffer) TYPE(bsl::allocator_arg, d_allocator, val);
    }

    template <class T1, class T2, class ALLOC>
    template <class TYPE>
    void DiscriminatedUnion<T1,T2,ALLOC>::constructObj(const TYPE& val,
                                                       bsl::false_type)
    {
        ::new(&d_buffer) TYPE(val, d_allocator);
    }
//..
// Next, we dispatch to the correct 'constructObj' overlaod within the
// constructors using the 'UsesAllocatorArgT' metafunction to determine which
// extended copy constructor is the right one:
//..
    template <class T1, class T2, class ALLOC>
    DiscriminatedUnion<T1,T2,ALLOC>::DiscriminatedUnion(const T1&    val,
                                                        const ALLOC& alloc)
        : d_allocator(alloc), d_tag(TAG_FIRST)
    {
        constructObj(val, bslmf::UsesAllocatorArgT<T1>());
    }

    template <class T1, class T2, class ALLOC>
    DiscriminatedUnion<T1,T2,ALLOC>::DiscriminatedUnion(const T2&    val,
                                                        const ALLOC& alloc)
        : d_allocator(alloc), d_tag(TAG_SECOND)
    {
        constructObj(val, bslmf::UsesAllocatorArgT<T2>());
    }
//..
// Next, we fill out the other member functions:
//..
    // MANIPULATORS
    template <class T1, class T2, class ALLOC>
    T1& DiscriminatedUnion<T1,T2,ALLOC>::firstValue() {
        ASSERT(TAG_FIRST == d_tag);
        return reinterpret_cast<T1&>(d_buffer);
    }

    template <class T1, class T2, class ALLOC>
    T2& DiscriminatedUnion<T1,T2,ALLOC>::secondValue() {
        ASSERT(TAG_SECOND == d_tag);
        return reinterpret_cast<T2&>(d_buffer);
    }

    // ACCESSORS
    template <class T1, class T2, class ALLOC>
    typename DiscriminatedUnion<T1,T2,ALLOC>::Tag
    DiscriminatedUnion<T1,T2,ALLOC>::tag() const {
        return d_tag;
    }

    template <class T1, class T2, class ALLOC>
    const T1& DiscriminatedUnion<T1,T2,ALLOC>::firstValue() const {
        ASSERT(TAG_FIRST == d_tag);
        return reinterpret_cast<const T1&>(d_buffer);
    }

    template <class T1, class T2, class ALLOC>
    const T2& DiscriminatedUnion<T1,T2,ALLOC>::secondValue() const {
        ASSERT(TAG_SECOND == d_tag);
        return reinterpret_cast<const T2&>(d_buffer);
    }
//..
// Now we can create a couple of types that use STL allocators. 'TypeA' uses
// the traditional constructor pattern taking an allocator at the end of the
// argument list.  'TypeB' uses the newer pattern of prefixing the allocator
// argument with a tag of type 'allocator_arg_t':
//..
    template <class ALLOC>
    class TypeA {
        ALLOC d_allocator;
        // Additional data members go here

    public:
        // CREATORS
        explicit TypeA(const ALLOC& alloc = ALLOC()) : d_allocator(alloc) { }
        TypeA(const TypeA& other, const ALLOC& alloc = ALLOC())
            : d_allocator(alloc) { (void) other; }
        // Etc..  Destructor and assignment not shown

        // ACCESSORS
        const ALLOC& allocator() const { return d_allocator; }
        // etc..
    };

    template <class ALLOC>
    class TypeB {
        ALLOC d_allocator;
        // Additional data members go here

    public:
        // CREATORS
        TypeB() : d_allocator() { }
        TypeB(bsl::allocator_arg_t, const ALLOC& alloc) : d_allocator(alloc) {}
        TypeB(const TypeB& other) : d_allocator() { (void) other; }
        TypeB(bsl::allocator_arg_t, const ALLOC& alloc, const TypeB& other)
             : d_allocator(alloc) { (void) other; }
        // Etc..  Destructor and assignment not shown

        // ACCESSORS
        const ALLOC& allocator() const { return d_allocator; }
        // etc..
    };
//..
// Next, since 'TypeB' uses the 'allocator_arg_t' idiom, it must define a
// specialization of the 'UsesAllocatorArgT' trait:
//..
    namespace bslmf {

    template <class ALLOC>
    struct UsesAllocatorArgT<TypeB<ALLOC> > : bsl::true_type { };

    } // Close namespace bslmf
//..
// Next, we create an allocator template that meets the requirements of an STL
// allocator:
//..
    template <class TYPE>
    class MyAllocator {
        int d_id;
    public:
        typedef TYPE value_type;

        MyAllocator(int id = 0) : d_id(id) { }
        int id() const { return d_id; }

        // Other member types and functions not shown
    };

    } // Close enterprise namespace
//..
// Finally, we create a couple of 'DiscriminatedUnion' objects and verify that
// the allocator is correctly propagated to the contained elements:
//..
    int usageExample1() {

        typedef MyAllocator<int> AllocType;
        typedef DiscriminatedUnion<TypeA<AllocType>,
                                   TypeB<AllocType>,
                                   AllocType>  UnionType;

        // Construct objects using default allocator
        TypeA<AllocType> vA;
        ASSERT(vA.allocator().id() == 0);
        TypeB<AllocType> vB;
        ASSERT(vB.allocator().id() == 0);

        AllocType a2(2);  // Create a second allocator

        UnionType u1(vA, a2);
        ASSERT(UnionType::TAG_FIRST == u1.tag());
        ASSERT(u1.firstValue().allocator().id() == 2);
        UnionType u2(vB, a2);
        ASSERT(UnionType::TAG_SECOND == u2.tag());
        ASSERT(u2.secondValue().allocator().id() == 2);

        return 0;
    }

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

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  This usage test
        //   also happens to exhaustively test the entire component and is thus
        //   the only test in the suite.
        //
        // Concerns:
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef MyAllocator<int> AllocType;

        ASSERT(! bslmf::UsesAllocatorArgT<TypeA<AllocType> >::value);
        ASSERT(  bslmf::UsesAllocatorArgT<TypeB<AllocType> >::value);
        ASSERT(  bslmf::UsesAllocatorArgT<TypeC<AllocType> >::value);

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
