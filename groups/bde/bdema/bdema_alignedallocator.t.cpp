// bdema_alignedallocator.t.cpp                                       -*-C++-*-
#include <bdema_alignedallocator.h>

#include <bsls_protocoltest.h>

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
//-----------------------------------------------------------------------------
// [ 1] virtual void *allocateAligned(size_type size, int alignment);
// [ 1] virtual void deallcate(void *address);
// [ 2] USAGE EXAMPLE
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
    void *allocateAligned(size_type, int) { return markDone(); }
    int   deallocate(void* )              { return markDone(); }
};

}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// In the following examples we demonstrate how to use a
// 'bdema_AlignedAllocator' to allocate memory aligned according to a specified
// boundary.
//
///Example 1: Implementing 'bdema_AlignedAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdema_AlignedAllocator' protocol provided in this component
// defines a bilateral contract between suppliers and consumers of raw
// aligned memory.  In order for the 'bdema_AlignedAllocator' interface to be
// useful, we must supply a concrete allocator that implements it.
//
// In this example, we demonstrate how to adapt 'posix_memalign' to this
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
        // system-supplied 'posix_memalign' and 'free' on UNIX, or 
        // '_aligned_malloc' and '_aligned_free' on Windows. 
  
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
        virtual void *allocateAligned(bsl::size_type size, int alignment);
            // Return the address of a newly allocated block of memory of at
            // least the specified positive 'size' (in bytes), sufficiently
            // aligned such that '0 == (address & (alignement - 1)).  If 'size'
            // is 0, a null pointer is returned with no other effect.  If this
            // allocator cannot return the requested number of bytes, then it
            // throws an 'bsl::bad_alloc' exception, or abort if in a
            // non-exception build.  The behavior is undefined unless 
            // '0 <=  size' and 'alignment' is both a multiple of 
            // 'sizeof(void *)' and a power of two.  Note that the underlying
            // 'posix_memalign' function is *not* called when 'size' is 0.
  
        virtual void deallocate(void *address);
            // Return the memory block at the specified 'address' back to this
            // allocator.  If 'address' is 0, this function has no effect.  The
            // behavior is undefined unless 'address' was allocated using this
            // allocator object and has not already been deallocated.  Note
            // that the underlying 'posix_memalign' function is *not* called
            // when 'address' is 0.
    };
    // ...
//..
// Now, we define the virtual methods of 'MyAlignedAllocator', non
// inlined, as they would not be inlined when invoked from the base class (the
// typical usage in this case):
//..
    // MANIPULATORS
    void *MyAlignedAllocator::allocateAligned(size_type size,
                                              size_type alignment)
    {
        BSLS_ASSERT_SAFE(0 <= size);
        BSLS_ASSERT_SAFE(0 <= alignement);
        BSLS_ASSERT_SAFE(0 == ((bsl::log(alignement)/bsl::log(2)) % 2));
        BSLS_ASSERT_SAFE(0 == (alignement % sizeof(void *)));
  
        void *ret;
  
    #ifdef BSLS_PLATFORM_WINDOWS
        int rc = ::posix_memalign(&ret, alignment, size);
        if (0 != rc) {
            bslma_Allocator::throwBadAlloc();
        }
    #else
        errno = 0;
        void *ret = _aligned_malloc(size, alignment);
        if (0 != errno) {
            bslma_Allocator::throwBadAlloc();
        }
    #endif
  
        return ret;
    }
  
    void MyAlignedAllocator::deallocate(void *address)
    {
    #ifdef BSLS_PLATFORM_WINDOWS
        _aligned_free(address);
    #else
        ::free(address);
    }
//..
// Finally, we instantiate an object of type 'MyAlignedAllocator':
//..
    MyAlignedAllocator myAlignedAllocator;
//..
//
//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
///Example 2: Using a 'bdema_AlignedAllocator'.
///- - - - - - - - - - - - - - - - - - - - - -
// In this example we use an object of type 'MyAlignedAllocator', defined
// in the previous example to obtain memory aligned on a page boundary,
// assuming pages of 4096 bytes.
//
// First, we obtain a 'MyAlignedAllocator' reference to 'myP' we constructed in
// the previous example:
//..
    bdema_AlignedAllocator *alignedAllocator = &myAlignedAllocator;
//..
// Now, we allocate a buffer of 1024 bytes of memory and make sure that it is
// aligned on a 4096 boundary:
//
//..
    char *address = (char *) alignedAllocator->allocateAligned(1024, 4096);
//..
// Finally, we verify that the obtained address actually is aligned as
// expected:
//..
    assert(buffer & (bsls_Allocator::size_type)4096 - 1);
//..


        if (verbose) cout << endl << "TESTING USAGE EXAMPLE" << endl
                                  << "=====================" << endl;


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
        //: 2 It has no data members.
        //: 3 All of its members are pure virtual.
        //: 4 It has a pure virtual destructor.
        //: 5 All of its methods are publicly accessible.
        //: 6 The class inherits publicly from 'bsls_Allocator'.
        //
        // Plan:
        //: 1 Use 'bslsl_protocoltest' component to test this protocol class.
        //:   (C-1..5)
        //: 2 Convert implicitly the address of a concrete implementation of
        //:   'bdema_AlignedAllocator' object into an address to
        //:   'bsls_Allocator' to test that this protocol inherits publicly
        //:   form 'bsls_Allocator'.  (C-6)
        //
        // Testing:
        //   virtual void *allocate(size_type size, int alignment) = 0;
        //   virtual void dellocate(void *address) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        bsls_ProtocolTest<AlignedAllocatorTestImp> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, loadTimeZone(0, 0));

        LOOP_ASSERT(t.failures(), !t.failures());

        // Reuse the concrete implmentation of the usage example.
 
        MyPosixMemAlignAllocator alignAllocator;
        bsls_Assert *allocator = &alignAllocator;
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
