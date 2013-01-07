// bdema_alignedallocator.t.cpp                                       -*-C++-*-
#include <bdema_alignedallocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_platform.h>
#include <bsls_protocoltest.h>

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
// We are testing a pure protocol class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) that a usage example
// obtains the behavior specified by the protocol from the concrete subclass.
// We also need to ensure that this class derives from the 'bslma_Allocator'
// class.
//-----------------------------------------------------------------------------
// [ 1] virtual void allocate(size_type size);
// [ 1] virtual void *allocateAligned(bsl::size_t, size_type);
// [ 1] virtual void deallocate(void *address);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] CONCERN: The protocol derives from 'bslma_Allocator'.
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
namespace {

struct AlignedAllocatorTestImp : bsls_ProtocolTestImp<bdema_AlignedAllocator> {
    typedef bslma_Allocator::size_type size_type;

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
// This section illustrates intended usage of this component.
//
///Example 1: Implementing 'bdema_AlignedAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdema_AlignedAllocator' protocol provided in this component
// defines a bilateral contract between suppliers and consumers of raw
// aligned memory.  In order for the 'bdema_AlignedAllocator' interface to be
// useful, we must supply a concrete allocator that implements it.
//
// In this example, we demonstrate how to adapt 'posix_memalign' on Linux and
// AIX, 'memalign' on SunOS and '_aligned_malloc' on Windows, to this
// protocol base class:
//
// First, we specify the interface of the concrete implementation of
// 'MyAlignedAllocator:
//..
    // myposixmemalignallocator.h
    // ...

    class MyAlignedAllocator: public bdema_AlignedAllocator {
        // This class is a sample concrete implementation of the
        // 'bdema_AlignedAllocator' protocol that provides direct access to the
        // system-supplied 'posix_memalign' and 'free' on Linux and AIX
        // platforms, 'memalign' and 'free' on SunOS, or '_aligned_malloc' and
        // '_aligned_free' on Windows.

      private:
        // NOT IMPLEMENTED
        MyAlignedAllocator(const MyAlignedAllocator&);
        MyAlignedAllocator& operator=(const MyAlignedAllocator&);

      public:
        // CREATORS
        MyAlignedAllocator();
            // Create a 'MyAlignedAllocator' object.  Note that all
            // objects of this class share the same underlying resource.

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
            // requirement for any object of the specified 'size'.  Also note
            // that global 'operator new' is *not* called when 'size' is 0 (in
            // order to avoid having to acquire a lock, and potential
            // contention in multi-threaded programs).

        virtual void *allocateAligned(bsl::size_t size, size_type alignment);
            // Return the address of a newly allocated block of memory of at
            // least the specified positive 'size' (in bytes), sufficiently
            // aligned such that the returned 'address' satisfies
            // '0 == (address & (alignment - 1))'.  If 'size' is 0, a null
            // pointer is returned with no other effect.  If the requested
            // number of appropriately aligned bytes cannot be returned, then a
            // 'bsl::bad_alloc' exception is thrown, or in a non-exception
            // build the program is terminated.  The behavior is undefined
            // unless 'alignment' is both a multiple of 'sizeof(void *)' and an
            // integral non-negative power of two.

        virtual void deallocate(void *address);
            // Return the memory block at the specified 'address' back to this
            // allocator.  If 'address' is 0, this function has no effect.  The
            // behavior is undefined unless 'address' was allocated using this
            // allocator object and has not already been deallocated.
    };
//  // ...
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
        BSLS_ASSERT_SAFE(0 <= size);

        if (0 == size) {
            return 0;                                                 // RETURN
        }

        int alignment = bsls_AlignmentUtil::calculateAlignmentFromSize(size);
        return allocateAligned(size, alignment);
    }

    void *MyAlignedAllocator::allocateAligned(bsl::size_t size,
                                              size_type   alignment)
    {
        BSLS_ASSERT_SAFE(0 <= alignment);
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
            bslma_Allocator::throwBadAlloc();
        }
    #elif defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_HPUX)
        ret = memalign(alignment, size);
        if (0 == ret) {
            bslma_Allocator::throwBadAlloc();
        }
    #else
        int rc = ::posix_memalign(&ret, alignment, size);
        if (0 != rc) {
            bslma_Allocator::throwBadAlloc();
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
//..
//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

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

///Example 2: Using the 'bdema_AlignedAllocator' protocol
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use the 'bdema_AlignedAllocator'
// protocol to allocate memory that is aligned to the beginning of a memory
// page.  Third party libraries, for example device drivers that perform DMA
// access of device drivers, or some extreme optimizations to reduce the
// number of page faults, might require page aligned allocations.
//
// First, we create an aligned allocator 'myAlignedAllocator' using the class
// 'MyAlignedAllocator' defined in the previous example, and obtain a
// 'bdema_AlignedAllocator' pointer to it:
//..
    MyAlignedAllocator myAlignedAllocator;
    bdema_AlignedAllocator *alignedAllocator = &myAlignedAllocator;
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
        //   We need to make sure that 'baetzo_Loader' satisfies the protocol
        //   class requirements.
        //
        // Concerns:
        //: 1 'bdema_AlignedAllocator' is an abstract class, i.e., no objects
        //:    of the 'bdema_AlignedAllocator' class can be created.
        //:
        //: 2 'bdema_AlignedAllocator' has no data members.
        //:
        //: 3 All members of 'bdema_AlignedAllocator' are pure virtual.
        //:
        //: 4 'bdema_AlignedAllocator' has a pure virtual destructor.
        //:
        //: 5 All of 'bdema_AlignedAllocator' methods are publicly accessible.
        //:
        //: 6 The class inherits publicly from 'bsls_Allocator'.
        //
        // Plan:
        //: 1 Use 'bsl_ProtocolTest' component to test the following subset of
        //:   the 'bdema_AlignedAllocator' protocol concerns:
        //:
        //:   1 'bdema_AlignedAllocator' protocol is an abstract class, i.e.,
        //:      no objects of 'bdema_AlignedAllocator' protocol class can be
        //:      created.
        //:
        //:   2 'bdema_AlignedAllocator' has no data members.
        //:
        //:   3 Each of the known and tested methods of
        //:     'bdema_AlignedAllocator' is virtual.
        //:
        //:   4 'bdema_AlignedAllocator' has a virtual destructor.
        //:
        //:   5 Each of the known and tested methods of
        //:     'bdema_AlignedAllocator' is publicly accessible.
        //:
        //: 2 Assign an address of type 'bdema_AlignedAllocator' to a
        //:   'bsls_Allocator *' variable, to verify that
        //:   'bdema_AlignedAllocator' publicly inherited from
        //:   'bslma_Allocator' .
        //
        // Testing:
        //   virtual void *allocate(size_type size) = 0;
        //   virtual void *allocateAligned(bsl::size_t, size_type) = 0;
        //   virtual void deallocate(void *address) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTest the protocol with 'bsls_ProtocolTest'"
                          << endl;

        bsls_ProtocolTest<AlignedAllocatorTestImp> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, allocate(0));
        BSLS_PROTOCOLTEST_ASSERT(t, allocateAligned(0, 0));
        BSLS_PROTOCOLTEST_ASSERT(t, deallocate(0));

        LOOP_ASSERT(t.failures(), !t.failures());

        if (verbose) cout <<
           "\nTest 'bdema_AlignedAllocator' derives from 'bslma_Allocator'"
                                                                       << endl;
        bslma_Allocator *dummy = (bdema_AlignedAllocator *) 0;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
