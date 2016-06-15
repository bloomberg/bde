// bslma_allocatoradaptor.t.cpp                                       -*-C++-*-

#include <bslma_allocatoradaptor.h>

#include <bslma_testallocator.h>

#include <bsls_alignment.h>
#include <bsls_bsltestutil.h>

#include <new>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Exercise basic functionality of component
        //
        // Plan:
        //: 1
        //
        // Testing:
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
        ASSERT(sizeof(TestObj) <= static_cast<size_t>(ta.numBytesInUse()));
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
