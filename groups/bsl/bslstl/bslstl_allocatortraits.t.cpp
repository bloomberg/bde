// bslstl_allocatortraits.t.cpp                  -*-C++-*-

#include "bslstl_allocatortraits.h"

#include <cstdio>
#include <cstdlib>
#include <bslma_testallocator.h>
#include <bslma_default.h>
#include <bslstl_allocator.h>
#include <bslalg_typetraitsgroupstlsequence.h>

using namespace BloombergLP;
using namespace bsl;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

// typedef bsls_Types::Int64             Int64;
// typedef bsls_Types::Uint64            Uint64;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
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

// Generic debug print function (3-arguments).
template <class T>
void dbg_print(const char* s, const T& val, const char* nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Let's create a container class that holds a single object and which meets
// the requirements of an STL container and of a Bloomberg container:
//..
    template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
    class my_Container {
        ALLOC  d_allocator;
        TYPE  *d_valueptr;

      public:
        typedef BloombergLP::bslalg_TypeTraitsGroupStlSequence<TYPE,ALLOC>
            MyTypeTraits;
        BSLALG_DECLARE_NESTED_TRAITS(my_Container, MyTypeTraits);
            // Declare nested type traits for this class.

        typedef TYPE  value_type;
        typedef ALLOC allocator_type;
        // etc.

        explicit my_Container(const ALLOC& a = ALLOC());
        explicit my_Container(const TYPE& v, const ALLOC& a = ALLOC());
        my_Container(const my_Container& other);
        ~my_Container();

        TYPE&       front()       { return *d_valueptr; }
        const TYPE& front() const { return *d_valueptr; }
        // etc.
    };
//..
// The implementation of the constructors needs to allocate memory and
// construct an object of type 'TYPE' in the allocated memory.  Rather than
// allocating the memory directly, we use the 'allocate' member of
// 'allocator_traits'.  More importantly, we construct the object using the
// 'construct' member of 'allocator_traits', which provides the correct
// semantic for passing the allocator to the constructed object when
// appropriate:
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const ALLOC& a)
        : d_allocator(a)
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr);
    }

    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const TYPE& v, const ALLOC& a)
        : d_allocator(a)
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr, v);
    }
//..
// The copy constructor needs to conditinally copy the allocator from the
// 'other' container.  It uses
// 'allocator_traits::select_on_container_copy_construction' to decide whether
// to copy the 'other' allocator (for non-bslma allocators) or to
// default-construct the allocator (for bslma allocators).
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const my_Container& other)
        : d_allocator(bsl::allocator_traits<ALLOC>::
                      select_on_container_copy_construction(other.d_allocator))
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr, *other.d_valueptr);
    }
//..
// Finally, the destructor uses 'allocator_traits' functions to destroy and
// deallocate the value object:
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::~my_Container()
    {
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        AllocTraits::destroy(d_allocator, d_valueptr);
        AllocTraits::deallocate(d_allocator, d_valueptr, 1);
    }
//..
// Now, given a value type that uses a 'bslma_Allocator' to allocate memory:
//..
    class my_Type {

        bslma_Allocator *d_allocator;
        // etc.
      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_Type,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        explicit my_Type(bslma_Allocator* basicAlloc = 0)
            : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
        my_Type(const my_Type& other)
            : d_allocator(bslma_Default::allocator(0)) { /* ... */ }
        my_Type(const my_Type& other, bslma_Allocator* basicAlloc)
            : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
        // etc.

        // ACCESSORS
        bslma_Allocator *allocator() const { return d_allocator; }
        // etc.
    };
//..
// We can see that the allocator is propagated to the container's element and
// that it is not copied on copy construction of the container:
//..
    int usageExample()
    {
        bslma_TestAllocator testAlloc;
        my_Container<my_Type> C1(&testAlloc);
        ASSERT(C1.front().allocator() == &testAlloc);
        my_Container<my_Type> C2(C1);
        ASSERT(C2.front().allocator() != &testAlloc);
        ASSERT(C2.front().allocator() == bslma_Default::defaultAllocator());
        return 0;
    }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 30: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   1. The usage example in the header file compiles.
        //   2. The usage example in the header produces correct results
        //
        // Test plan:
        //   Copy the usage examples from the header into this test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample();

      } break;
      case 1:
      {
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
                            "\n==============\n");

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
