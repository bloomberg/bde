// bdlma_alignedallocator.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_alignedallocator.h>

#include <bslim_testutil.h>

#include <bsls_alignmentutil.h>
#include <bsls_platform.h>
#include <bsls_protocoltest.h>
#include <bsls_bslexceptionutil.h>

#include <bsl_cerrno.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class.  We need to verify that (1) a concrete
// derived class compiles and links, and (2) that a usage example obtains the
// behavior specified by the protocol from the concrete subclass.  We also need
// to ensure that this class derives from the 'bslma::Allocator' class.
//-----------------------------------------------------------------------------
// [ 1] virtual void allocate(size_type size);
// [ 1] virtual void *allocateAligned(bsl::size_t, size_type);
// [ 1] virtual void deallocate(void *address);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] CONCERN: The protocol derives from 'bslma::Allocator'.
//=============================================================================
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

namespace {

struct AlignedAllocatorTestImp :
                               bsls::ProtocolTestImp<bdlma::AlignedAllocator> {
    typedef bslma::Allocator::size_type size_type;

    void *allocate(size_type)                     { return markDone(); }
    void *allocateAligned(bsl::size_t, size_type) { return markDone(); }
    void  deallocate(void* )                      {        markDone(); }
};

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing 'bdlma::AlignedAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlma::AlignedAllocator' protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw aligned memory.
// In order for the 'bdlma::AlignedAllocator' interface to be useful, we must
// supply a concrete allocator that implements it.
//
// In this example, we demonstrate how to adapt 'posix_memalign' on Linux and
// AIX, 'memalign' on SunOS and '_aligned_malloc' on Windows, to this protocol
// base class:
//
// First, we specify the interface of the concrete implementation of
// 'MyAlignedAllocator:
//..
    // myposixmemalignallocator.h
    // ...

    class MyAlignedAllocator: public bdlma::AlignedAllocator {
        // This class is a sample concrete implementation of the
        // 'bdlma::AlignedAllocator' protocol that provides direct access to
        // the system-supplied 'posix_memalign' and 'free' on Linux and AIX
        // platforms, 'memalign' and 'free' on SunOS, or '_aligned_malloc' and
        // '_aligned_free' on Windows.

      private:
        // NOT IMPLEMENTED
        MyAlignedAllocator(const MyAlignedAllocator&);
        MyAlignedAllocator& operator=(const MyAlignedAllocator&);

      public:
        // CREATORS
        MyAlignedAllocator();
            // Create a 'MyAlignedAllocator' object.  Note that all objects of
            // this class share the same underlying resource.

        virtual ~MyAlignedAllocator();
            // Destroy this object.  Note that destroying this object has no
            // effect on any outstanding allocated memory.

        // MANIPULATORS
        virtual void *allocate(size_type size);
            // Return a newly allocated block of memory of (at least) the
            // specified positive 'size' (in bytes).  If 'size' is 0, a null
            // pointer is returned with no other effect.  If this allocator
            // cannot return the requested number of bytes, then it will throw
            // an 'std::bad_alloc' exception in an exception-enabled build, or
            // else it will abort the program in a non-exception build.  The
            // behavior is undefined unless '0 <= size'.   Note that the
            // alignment of the address returned conforms to the platform
            // requirement for any object of the 'size'.  Also note that global
            // 'operator new' is *not* called when 'size' is 0 (in order to
            // avoid having to acquire a lock, and potential contention in
            // multi-threaded programs).

        virtual void *allocateAligned(bsl::size_t size, size_type alignment);
            // Return the address of a newly allocated block of memory of at
            // least the specified positive 'size' (in bytes), sufficiently
            // aligned such that the returned 'address' satisfies, for the
            // specified 'alignment', '0 == (address & (alignment - 1))'.  If
            // 'size' is 0, a null pointer is returned with no other effect.
            // If the requested number of appropriately aligned bytes cannot be
            // returned, then a 'bsl::bad_alloc' exception is thrown, or in a
            // non-exception build the program is terminated.  The behavior is
            // undefined unless 'alignment' is both a multiple of
            // 'sizeof(void *)' and an integral non-negative power of two.

        virtual void deallocate(void *address);
            // Return the memory block at the specified 'address' back to this
            // allocator.  If 'address' is 0, this function has no effect.  The
            // behavior is undefined unless 'address' was allocated using this
            // allocator object and has not already been deallocated.
    };
    // ...
//..
// Then, we implement the creators, trivially, as this class contains no
// instance data members.
//..
    // CREATORS
    MyAlignedAllocator::MyAlignedAllocator()
    {
    }

    MyAlignedAllocator::~MyAlignedAllocator()
    {
    }
//..
// Now, we define the virtual methods of 'MyAlignedAllocator'.  Note that these
// definitions are not 'inline', as they would not be inlined when invoked from
// the base class (the typical usage in this case):
//..
    // MANIPULATORS
    void *MyAlignedAllocator::allocate(size_type size)
    {
        if (0 == size) {
            return 0;                                                 // RETURN
        }

        int alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(size);
        return allocateAligned(size, alignment);
    }

    void *MyAlignedAllocator::allocateAligned(bsl::size_t size,
                                              size_type   alignment)
    {
        BSLS_ASSERT_SAFE(0 == (alignment & (alignment - 1)));
        BSLS_ASSERT_SAFE(0 == (alignment % sizeof(void *)));

        if (0 == size) {
            return 0;                                                 // RETURN
        }

        void *ret = 0;

    #ifdef BSLS_PLATFORM_OS_WINDOWS
        errno = 0;
        ret = _aligned_malloc(size, alignment);
        if (0 != errno) {
            bsls::BslExceptionUtil::throwBadAlloc();
        }
    #elif defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_HPUX)
        ret = memalign(alignment, size);
        if (0 == ret) {
            bsls::BslExceptionUtil::throwBadAlloc();
        }
    #else
        int rc = ::posix_memalign(&ret, alignment, size);
        if (0 != rc) {
            bsls::BslExceptionUtil::throwBadAlloc();
        }
    #endif

        return ret;
    }

    void MyAlignedAllocator::deallocate(void *address)
    {
        if (0 == address) {
            return;                                                   // RETURN
        }
    #ifdef BSLS_PLATFORM_WINDOWS
        _aligned_free(address);
    #else
        ::free(address);
    #endif
    }
//..
// Finally, we define a function 'f' that instantiates an object of type
// 'MyAlignedAllocator':
//..
    void f() {
        MyAlignedAllocator a;
    }
//..
// Note that the memory is not released when the allocator goes out of scope.

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Example 2: Using the 'bdlma::AlignedAllocator' protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use the 'bdlma::AlignedAllocator'
// protocol to allocate memory that is aligned to the beginning of a memory
// page.  Third party libraries, for example device drivers that perform DMA
// access of device drivers, or some extreme optimizations to reduce the number
// of page faults, might require page aligned allocations.
//
// First, we create an aligned allocator 'myAlignedAllocator' using the class
// 'MyAlignedAllocator' defined in the previous example, and obtain a
// 'bdlma::AlignedAllocator' pointer to it:
//..
    MyAlignedAllocator myAlignedAllocator;
    bdlma::AlignedAllocator *alignedAllocator = &myAlignedAllocator;
//..
// Now, assuming a page size of 4K, we allocate a buffer of 1024 bytes of
// memory and indicate that it should be aligned on a 4096 boundary:
//..
    char *address = (char *) alignedAllocator->allocateAligned(1024, 4096);
//..
// Finally, we verify that the obtained address actually is aligned as
// expected:
//..
    ASSERT(0 == ((bsl::size_t)address & (4096 - 1)));
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   We need to make sure that 'baltzo::Loader' satisfies the protocol
        //   class requirements.
        //
        // Concerns:
        //: 1 'bdlma::AlignedAllocator' is an abstract class, i.e., no objects
        //:    of the 'bdlma::AlignedAllocator' class can be created.
        //:
        //: 2 'bdlma::AlignedAllocator' has no data members.
        //:
        //: 3 All members of 'bdlma::AlignedAllocator' are pure virtual.
        //:
        //: 4 'bdlma::AlignedAllocator' has a pure virtual destructor.
        //:
        //: 5 All of 'bdlma::AlignedAllocator' methods are publicly accessible.
        //:
        //: 6 The class inherits publicly from 'bsls_Allocator'.
        //
        // Plan:
        //: 1 Use 'bsl_ProtocolTest' component to test the following subset of
        //:   the 'bdlma::AlignedAllocator' protocol concerns:
        //:
        //:   1 'bdlma::AlignedAllocator' protocol is an abstract class, i.e.,
        //:      no objects of 'bdlma::AlignedAllocator' protocol class can be
        //:      created.
        //:
        //:   2 'bdlma::AlignedAllocator' has no data members.
        //:
        //:   3 Each of the known and tested methods of
        //:     'bdlma::AlignedAllocator' is virtual.
        //:
        //:   4 'bdlma::AlignedAllocator' has a virtual destructor.
        //:
        //:   5 Each of the known and tested methods of
        //:     'bdlma::AlignedAllocator' is publicly accessible.
        //:
        //: 2 Assign an address of type 'bdlma::AlignedAllocator' to a
        //:   'bsls_Allocator *' variable, to verify that
        //:   'bdlma::AlignedAllocator' publicly inherited from
        //:   'bslma::Allocator' .
        //
        // Testing:
        //   virtual void *allocate(size_type size) = 0;
        //   virtual void *allocateAligned(bsl::size_t, size_type) = 0;
        //   virtual void deallocate(void *address) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTest the protocol with 'bsls::ProtocolTest'"
                          << endl;

        bsls::ProtocolTest<AlignedAllocatorTestImp> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, allocate(0));
        BSLS_PROTOCOLTEST_ASSERT(t, allocateAligned(0, 0));
        BSLS_PROTOCOLTEST_ASSERT(t, deallocate(0));

        LOOP_ASSERT(t.failures(), !t.failures());

        if (verbose) cout <<
           "\nTest 'bdlma::AlignedAllocator' derives from 'bslma::Allocator'"
                                                                       << endl;
        bslma::Allocator *dummy = (bdlma::AlignedAllocator *)0;
        ASSERT(0 == dummy);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
