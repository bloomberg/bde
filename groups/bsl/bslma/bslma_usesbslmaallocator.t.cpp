// bslma_usesbslmaallocator.t.cpp                                     -*-C++-*-

#include <bslma_usesbslmaallocator.h>

#include <bslma_allocator.h>
#include <bslmf_assert.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;
using namespace bsl;

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
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

class DoesNotUseAnAllocatorType {
    // ...
};

class UsesAllocatorType1 {
    // ...

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
                                   bslma::UsesBslmaAllocator);
    // CREATORS
    explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
       // ...

    UsesAllocatorType1(const UsesAllocatorType1&  original, 
                       bslma::Allocator          *basicAllocator = 0);
       // ...
};

class UsesAllocatorType2 {
    // ...

  public:
    // CREATORS
    explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
       // ...

    UsesAllocatorType2(const UsesAllocatorType2&  original, 
                       bslma::Allocator          *basicAllocator = 0);
       // ...
};

namespace BloombergLP {
namespace bslma {

template <> struct UsesBslmaAllocator<UsesAllocatorType2> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace



//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

struct SniffUsesBslmaAllocatorFromConstructor
{
    SniffUsesBslmaAllocatorFromConstructor(bslma::Allocator *);
};

struct ConstructFromAnyPointer
{
    template <typename TYPE>
    ConstructFromAnyPointer(TYPE *);
};

struct ClassUsingBslmaAllocator
{
};

struct DerivedAllocator : bslma::Allocator
{
};

struct ConvertibleToAny
    // Type that can be converted to any type.  'DetectNestedTrait' shouldn't
    // assign it any traits.  The concern is that since
    // 'BSLMF_NESTED_TRAIT_DECLARATION' defines its own conversion operator,
    // the "convert to anything" operator shouldn't interfere with the nested
    // trait logic.
{
    template <typename T>
    operator T() const { return T(); }
};

}

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ClassUsingBslmaAllocator> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<ConvertibleToAny> : bsl::true_type {
    // Even though the nested trait logic is disabled by the template
    // conversion operator, the out-of-class trait specialization should still
    // work.
};

}
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------


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
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
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

        if (verbose) printf("\nMain example usage test.\n");

	ASSERT(false == 
	       bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);

	ASSERT(true  ==
	       bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);

	ASSERT(true  ==
	       bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);

	BSLMF_ASSERT(false == 
		  bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);

        BSLMF_ASSERT(true  ==
		     bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);

	BSLMF_ASSERT(true ==
		     bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
	
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
                            "\n==============\n");

        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor const>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    SniffUsesBslmaAllocatorFromConstructor volatile>::value);

        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator const>::value);
        ASSERT(bslma::UsesBslmaAllocator<
                    ClassUsingBslmaAllocator volatile>::value);

        ASSERT(! bslma::UsesBslmaAllocator<ConstructFromAnyPointer>::value);
        ASSERT(! bslma::UsesBslmaAllocator<bslma::Allocator *>::value);
        ASSERT(! bslma::UsesBslmaAllocator<bslma::Allocator const *>::value);
        ASSERT(! bslma::UsesBslmaAllocator<bslma::Allocator volatile *>::value);

        ASSERT(! bslma::UsesBslmaAllocator<DerivedAllocator *>::value);

        ASSERT(bslma::UsesBslmaAllocator<ConvertibleToAny>::value);
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
// Copyright (C) 2013 Bloomberg Finance L.P.
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
