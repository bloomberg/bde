// bslma_allocatoradaptor.t.cpp                  -*-C++-*-

#include "bslma_allocatoradaptor.h"

#include <bslma_testallocator.h>
#include <bsls_alignment.h>

#include <new>
#include <cstdio>
#include <cstdlib>
#include <climits>

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Let's start with a simple class, 'my::FilePath', which allocates storage
// using a 'bslma::Allocator':
//..
    #include <bslma_allocator.h>
    #include <bslma_default.h>
    #include <bsls_nullptr.h>

    #include <cstring>
    #include <cstdlib>

    namespace my {

    class FilePath {
        // Store the path of a file or directory
        bslma::Allocator *d_allocator;
        char             *d_data;

    public:
        FilePath(bslma::Allocator* basicAllocator = 0 /* nullptr */)
            : d_allocator(bslma::Default::allocator(basicAllocator))
            , d_data(0 /* nullptr */) { }

        FilePath(const char* s, bslma::Allocator* basicAllocator = 0)
            : d_allocator(bslma::Default::allocator(basicAllocator))
        {
            d_data =
                 static_cast<char*>(d_allocator->allocate(std::strlen(s) + 1));
            std::strcpy(d_data, s);
        }

        bslma::Allocator *getAllocator() const { return d_allocator; }

        //...
    };

    } // close namespace my
//..
// Next, assume that an STL-allocator exists that uses memory exactly the way
// you need:
//..
    template <class TYPE>
    class MagicAllocator {
        bool d_useMalloc;
    public:
        typedef TYPE        value_type;
        typedef TYPE       *pointer;
        typedef const TYPE *const_pointer;
        typedef unsigned    size_type;
        typedef int         difference_type;

        template <class U>
        struct rebind {
            typedef MagicAllocator<U> other;
        };

        explicit MagicAllocator(bool useMalloc = false)
            : d_useMalloc(useMalloc) { }

        template <class U>
        MagicAllocator(const MagicAllocator<U>& other)
            : d_useMalloc(other.getUseMalloc()) { }

        value_type *allocate(std::size_t n, void* = 0 /* nullptr */) {
            if (d_useMalloc)
                return (value_type*) std::malloc(n * sizeof(value_type));
            else
                return (value_type*) ::operator new(n * sizeof(value_type));
        }

        void deallocate(value_type *p, std::size_t) {
            if (d_useMalloc)
                std::free(p);
            else
                ::operator delete(p);
        }

        static size_type max_size() { return UINT_MAX / sizeof(TYPE); }

        void construct(pointer p, const TYPE& value)
            { new((void *)p) TYPE(value); }

        void destroy(pointer p) { p->~TYPE(); }

        int getUseMalloc() const { return d_useMalloc; }
    };

    template <class T, class U>
    inline
    bool operator==(const MagicAllocator<T>& a, const MagicAllocator<U>& b)
    {
        return a.getUseMalloc() == b.getUseMalloc();
    }

    template <class T, class U>
    inline
    bool operator!=(const MagicAllocator<T>& a, const MagicAllocator<U>& b)
    {
        return a.getUseMalloc() != b.getUseMalloc();
    }
//..
// Now, if we want to create a 'FilePath' using a 'MagicAllocator', we
// need to adapt the 'MagicAllocator' to the 'bslma::Allocator' protocol.
// This is where 'bslma::AllocatorAdaptor' comes in:
//..
    int usageExample1()
    {
        MagicAllocator<char> ma(true);
        bslma::AllocatorAdaptor<MagicAllocator<char> >::Type maa(ma);

        my::FilePath usrbin("/usr/local/bin", &maa);

        ASSERT(&maa == usrbin.getAllocator());
        ASSERT(ma == maa.adaptedAllocator());

        return 0;
    }
//..

//=============================================================================
//                              TEST CLASSES
//-----------------------------------------------------------------------------

template <class TYPE>
class STLAllocator
{
    bslma::TestAllocator *d_mechanism;

public:
    typedef TYPE        value_type;
    typedef TYPE       *pointer;
    typedef const TYPE *const_pointer;
    typedef unsigned    size_type;
    typedef int         difference_type;

    template <class U>
    struct rebind {
        typedef STLAllocator<U> other;
    };

    explicit STLAllocator(bslma::TestAllocator *ta) : d_mechanism(ta) { }

    template <class U>
    STLAllocator(const STLAllocator<U>& other)
        : d_mechanism(other.mechanism()) { }

    TYPE *allocate(std::size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) d_mechanism->allocate(n * sizeof(TYPE));
    }

    void deallocate(TYPE *p, std::size_t) { d_mechanism->deallocate(p); }

    static size_type max_size() { return UINT_MAX / sizeof(TYPE); }

    void construct(pointer p, const TYPE& value)
        { new((void *)p) TYPE(value); }

    void destroy(pointer p) { p->~TYPE(); }

    bslma::TestAllocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const STLAllocator<T>& a, const STLAllocator<U>& b)
{
    return a.mechanism() == b.mechanism();
}

template <class T, class U>
inline
bool operator!=(const STLAllocator<T>& a, const STLAllocator<U>& b)
{
    return a.mechanism() != b.mechanism();
}

class TestObj
{
    static const int k_STUFF_SIZE =
        sizeof(bsls::AlignmentUtil::MaxAlignedType) / sizeof(unsigned);
    unsigned d_stuff[k_STUFF_SIZE];

public:
    TestObj() {
        for (int i = 0; i < k_STUFF_SIZE; ++i) {
            d_stuff[i] = 0x600dF00d;
        }
    }

    ~TestObj() {
        for (int i = 0; i < k_STUFF_SIZE; ++i) {
            d_stuff[i] = 0xDeadBeaf;
        }
    }
};


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
        // USAGE TEST
        //
        // Concerns:
        //
        // Plan:
	//
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta;
        STLAllocator<double> stla(&ta);
        bslma::AllocatorAdaptor<STLAllocator<short> > adaptor(stla);
        ASSERT(stla == adaptor.adaptedAllocator());
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());
        void *p = adaptor.allocate(sizeof(TestObj));
        ASSERT(1 == ta.numBlocksInUse());
        ASSERT(sizeof(TestObj) <= ta.numBytesInUse());
        ASSERT(ta.numBytesInUse() <=
               sizeof(TestObj) + sizeof(bsls::AlignmentUtil::MaxAlignedType));
        TestObj *tp = ::new(p) TestObj;
        tp->~TestObj();
        adaptor.deallocate(p);
        ASSERT(0 == ta.numBlocksInUse());

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
