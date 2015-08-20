// bslmf_allocatorargt.t.cpp                                          -*-C++-*-

#include "bslmf_allocatorargt.h"

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component consists of an empty tag type and a constant object of that
// type.  There is no functionality to test.  Instead, we test that this tag
// can be used for its intended purpose, which is to disambiguate constructor
// and function calls.  Thus, the entire test consists of a usage example.
//-----------------------------------------------------------------------------
// [1] struct bsl::allocator_arg_t
// [1] static const bsl::allocator_arg_t bsl::allocator_arg
//
// [1] USAGE EXAMPLE

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
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: push    // Usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -AQJ02  // Headers are included where examples need them
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose
// BDE_VERIFY pragma: -FD01   // Function contracts replaced by expository text
// BDE_VERIFY pragma: -FD03   // Function contracts replaced by expository text

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Disambiguate a constructor invocation
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a nullable type that can be in the full state
// (holding an object) or the null state (not holding an object).  When in the
// full state, memory is allocated for the held object using a memory
// allocator.  For simplicity, this memory allocator is not automatically
// propagated to the held object.
//
// First, we define a simple allocator class hierarchy with an abstract
// 'xyzma::Allocator' base class and two derived classes:
// 'xyzma::NewDeleteAllocator' and 'xyzma::TestAllocator':
//..
    #include <cstddef>

    namespace xyzma {

    class Allocator {
        // Abstract allocator base class
      public:
        virtual ~Allocator() { }

        virtual void *allocate(std::size_t nbytes) = 0;
        virtual void deallocate(void *ptr) = 0;
    };

    class NewDeleteAllocator : public Allocator {
        // Concrete allocator that uses operators 'new' and 'delete'

      public:
        static NewDeleteAllocator* singleton();
            // Returns a singleton instance of this class

        virtual void *allocate(std::size_t nbytes);
        virtual void deallocate(void *ptr);
    };

    NewDeleteAllocator *NewDeleteAllocator::singleton() {
        static NewDeleteAllocator s;
        return &s;
    }

    void *NewDeleteAllocator::allocate(std::size_t nbytes) {
        return ::operator new(nbytes);
    }

    void NewDeleteAllocator::deallocate(void *ptr) {
        ::operator delete(ptr);
    }

    class TestAllocator : public Allocator {
        // Concrete allocator that keeps track of number of blocks allocated
        // and deallocated.

        std::size_t d_allocatedBlocks;
        std::size_t d_deallocatedBlocks;

      public:
        TestAllocator() : d_allocatedBlocks(0), d_deallocatedBlocks(0) { }

        virtual void *allocate(std::size_t nbytes);
        virtual void deallocate(void *ptr);

        // ACCESSORS
        std::size_t allocatedBlocks() const { return d_allocatedBlocks; }
        std::size_t deallocatedBlocks() const { return d_deallocatedBlocks; }
        std::size_t outstandingBlocks() const {
            return d_allocatedBlocks - d_deallocatedBlocks;
        }
    };

    void *TestAllocator::allocate(std::size_t nbytes) {
        ++d_allocatedBlocks;
        return ::operator new(nbytes);
    }

    void TestAllocator::deallocate(void *ptr) {
        ++d_deallocatedBlocks;
        ::operator delete(ptr);
    }

    }  // close namespace xyzma
//..
// Next, we define our nullable class template, declaring two constructors: one
// that constructs the null object, and one that constructs a non-null object
// using the specified constructor argument.  For flexibility, the second
// constructor is a template that takes any type and can therefore construct
// the object without necessarily invoking the copy constructor.  (Ideally,
// this second constructor would be variadic, but that is not necessary for
// this example.):
//..
    #include <new>

    namespace xyzutl {

    template <class TYPE>
    class Nullable {
        xyzma::Allocator *d_alloc_p;
        TYPE             *d_object_p;

      public:
        // CREATORS
        Nullable();
            // Construct a null object.  Note: this is ctor A.

        template <class ARG>
        Nullable(const ARG& arg);
            // Construct a non-null object using the specified 'arg' as the
            // constructor argument for the 'TYPE' object.  Note: this is ctor
            // B.
//..
// Next, we want to add constructors that supply an allocator for use by the
// 'Nullable' object.  Our first thought is to add two more constructors like
// the two above, but with an additional allocator argument at the end:
//..
        // Nullable(xyzma::Allocator *alloc);
            // ctor C

        // template <class ARG>
        // Nullable(const ARG& arg, xyzma::Allocator *alloc);
            // ctor D
//..
// However, ctor C is difficult to invoke, because ctor B is almost always a
// better match.  Nor can we use SFINAE to disqualify ctor B in cases where
// ARG is 'xyzma::Allocator*' because 'xyzma::Allocator*' is a perfectly valid
// constructor argument for many 'TYPE's.
//
// We solve this problem by using 'allocator_arg_t' to explicitly tag the
// constructor that takes an allocator argument:
//..
        Nullable(bsl::allocator_arg_t, xyzma::Allocator *alloc);
            // Construct a null object with the specified 'alloc' allocator.
            // Note: this is ctor E
//..
// The 'allocator_arg_t' argument disambiguates the constructor.
//
// Next, to make things consistent (which is important for generic
// programming), we use the 'allocator_arg_t' tag in the other allocator-aware
// constructor, as well:
//..
        template <class ARG>
        Nullable(bsl::allocator_arg_t,
                 xyzma::Allocator *alloc,
                 const ARG&        arg);
            // Construct a non-null object using the specified 'arg' as the
            // constructor argument for the 'TYPE' object, and the specified
            // 'alloc' allocator.  Note: this is ctor F.
//..
// Next, we finish the class interface and implementation:
//..
        ~Nullable();

        // MANIPULATORS
        Nullable& operator=(const Nullable& rhs);
            // Copy assign this object from the specified 'rhs'.

        Nullable& operator=(const TYPE& rhs);
            // Construct a non-null object holding a copy of the specified
            // 'rhs' object.

        // ACCESSORS
        const TYPE& value() const { return *d_object_p; }
            // Return the object stored in this Nullable. The behavior is
            // undefined if this is null.

        bool isNull() const { return ! d_object_p; }
            // Returns true if this object is not null.

        xyzma::Allocator *allocator() const { return d_alloc_p; }
    };

    template <class TYPE>
    Nullable<TYPE>::Nullable()
        : d_alloc_p(xyzma::NewDeleteAllocator::singleton())
        , d_object_p(0)
    {
    }

    template <class TYPE>
    template <class ARG>
    Nullable<TYPE>::Nullable(const ARG& arg)
        : d_alloc_p(xyzma::NewDeleteAllocator::singleton())
        , d_object_p(static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE))))
    {
        ::new(d_object_p) TYPE(arg);
    }

    template <class TYPE>
    Nullable<TYPE>::Nullable(bsl::allocator_arg_t, xyzma::Allocator *alloc)
        : d_alloc_p(alloc)
        , d_object_p(0)
    {
    }

    template <class TYPE>
    template <class ARG>
    Nullable<TYPE>::Nullable(bsl::allocator_arg_t,
                             xyzma::Allocator *alloc,
                             const ARG&        arg)
        : d_alloc_p(alloc)
        , d_object_p(static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE))))
    {
        ::new(d_object_p) TYPE(arg);
    }

    template <class TYPE>
    Nullable<TYPE>::~Nullable() {
        if (d_object_p) {
            d_object_p->~TYPE();
            d_alloc_p->deallocate(d_object_p);
        }
    }

    template <class TYPE>
    Nullable<TYPE>& Nullable<TYPE>::operator=(const Nullable& rhs) {
        if (&rhs == this) return *this;                               // RETURN
        if (!isNull() && !rhs.isNull()) {
            *d_object_p = *rhs.d_object_p;
        }
        else if (!isNull() /* && rhs.isNull() */) {
            // Make null
            d_object_p->~TYPE();
            d_alloc_p->deallocate(d_object_p);
            d_object_p = 0;
        }
        else if (/* isNull() && */ !rhs.isNull()) {
            // Allocate and copy from 'rhs'
            d_object_p = static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE)));
            ::new(d_object_p) TYPE(*rhs.d_object_p);
        }
        // else both are null

        return *this;
    }

    template <class TYPE>
    Nullable<TYPE>& Nullable<TYPE>::operator=(const TYPE& rhs) {
        if (isNull()) {
            d_object_p = static_cast<TYPE*>(d_alloc_p->allocate(sizeof(TYPE)));
            ::new(d_object_p) TYPE(*rhs.d_object_p);
        }
        else {
            *d_object_p = rhs;
        }

        return *this;
    }

    }  // close namespace xyzutl
//..
// Now, for testing purposes, we define a class that takes an allocator
// constructor argument:
//..
    class Obj {
        xyzma::Allocator *d_alloc_p;
        int               d_count;
      public:
        explicit Obj(xyzma::Allocator *alloc = 0)
            : d_alloc_p(alloc), d_count(0)
        {
        }

        Obj(int count, xyzma::Allocator *alloc = 0)                 // IMPLICIT
            : d_alloc_p(alloc), d_count(count)
        {
        }

        int count() const { return d_count; }
        xyzma::Allocator *allocator() const { return d_alloc_p; }
    };

    bool operator==(const Obj& a, const Obj& b) {
        return a.count() == b.count();
    }

    bool operator!=(const Obj& a, const Obj& b) {
        return a.count() != b.count();
    }
//..
// Finally, we test that our nullable type can be constructed with and without
// an allocator pointer and that the allocator pointer can unambiguously be
// used for the object's allocator.
//..
    int usageExample() {

        using xyzutl::Nullable;

        xyzma::TestAllocator ta;

        Nullable<Obj> no1;
        ASSERT(  no1.isNull());
        ASSERT(xyzma::NewDeleteAllocator::singleton() == no1.allocator());

        Nullable<Obj> no2(2);
        ASSERT(! no2.isNull());
        ASSERT(xyzma::NewDeleteAllocator::singleton() == no2.allocator());
        ASSERT(2 == no2.value());
        ASSERT(0 == no2.value().allocator());

        Nullable<Obj> no3(bsl::allocator_arg, &ta);
        ASSERT(  no3.isNull());
        ASSERT(&ta == no3.allocator());
        ASSERT(0 == ta.outstandingBlocks());

        Nullable<Obj> no4(bsl::allocator_arg, &ta, 4);
        ASSERT(! no4.isNull());
        ASSERT(&ta == no4.allocator());
        ASSERT(1 == ta.outstandingBlocks());
        ASSERT(4 == no4.value());
        ASSERT(0 == no4.value().allocator());

        // '&ta' used by 'Obj', not by 'Nullable'.
        Nullable<Obj> no5(&ta);
        ASSERT(! no5.isNull());
        ASSERT(xyzma::NewDeleteAllocator::singleton() == no5.allocator());
        ASSERT(1 == ta.outstandingBlocks());  // No change
        ASSERT(0 == no5.value());
        ASSERT(&ta == no5.value().allocator());

        // '&ta' used by both 'Nullable' and by 'Obj'
        Nullable<Obj> no6(bsl::allocator_arg, &ta, &ta);
        ASSERT(! no6.isNull());
        ASSERT(&ta == no6.allocator());
        ASSERT(2 == ta.outstandingBlocks());
        ASSERT(0 == no6.value());
        ASSERT(&ta == no6.value().allocator());

        return 0;
    }
//..

// BDE_VERIFY pragma: pop  // end of usage example-example relaxed rules

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example compiles and runs.
        //: 2 'allocator_arg_t' can disambiguate constructors.
        //
        // Plan:
        //: 1 For concern 1, copy the usage example from the header file.
        //: 2 The usage example is designed to address concern 2.
        //
        // Testing:
        //   struct bsl::allocator_arg_t
        //   static const bsl::allocator_arg_t bsl::allocator_arg
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

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
// Copyright 2015 Bloomberg Finance L.P.
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
