// bdema_protectablememoryscopedguard.t.cpp  -*-C++-*-

#include <bdema_protectablememoryscopedguard.h>

#include <bslma_defaultallocatorguard.h>                // for testing only
#include <bdema_protectableblocklist.h>                 // for testing only
#include <bslma_testallocator.h>                        // for testing only
#include <bslma_testallocatorexception.h>               // for testing only
#include <bdema_testprotectableblockdispenser.h>        // for testing only

#include <bsls_alignment.h>

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// bdema_ProtectableMemoryScopedGuard is a guard component which has two
// methods, a constructor and a destructor. We'll create a trivial
// implementation of a class with 'protect()' and 'unprotect()' methods  to
// verify that the memory is 'protected' and 'unprotected' correctly.
//-----------------------------------------------------------------------------
// [2] Helper Functions (TestProtectableMemoryAllocator)
// [2] constructor, destructor
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [4] USAGE EXAMPLE
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdema_TestProtectableBlockDispenser TestDisp;
//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class TestProtectableMemoryAllocator {

    // DATA
    bool d_isProtected;

  public:
    // CREATORS
    TestProtectableMemoryAllocator(bool isProtected = true)
        : d_isProtected(isProtected) {}
        // Construct this 'TestProtectableMemoryAllocator' with the specified
        // 'isProtected' value.

    ~TestProtectableMemoryAllocator();
        // Destroy this 'TestProtectableMemoryAllocator'.

    // MUTATORS
    void *allocate(int size);
        // Return '0'. A no-op implementation of 'allocate()'.

    void deallocate(void *address);
        // A no-op implementation of 'deallocate()'.

    void protect();
        // Set the 'd_isProtected' flag to true.

    void unprotect();
        // Set the 'd_isProtected' flag to false.

    void release();
        // A no-op implementation of 'release()'.

    // ACCESSORS
    bool isProtected() const
        // Return the protected status of this allocator.
        { return d_isProtected; }
};

TestProtectableMemoryAllocator::~TestProtectableMemoryAllocator()
{
}

void *TestProtectableMemoryAllocator::allocate(int size)
{
    return 0;
}

void TestProtectableMemoryAllocator::deallocate(void *address)
{
}

void TestProtectableMemoryAllocator::protect()
{
    d_isProtected = true;
}

void TestProtectableMemoryAllocator::unprotect()
{
    d_isProtected = false;
}

void TestProtectableMemoryAllocator::release()
{
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This usage example defines a function 'setMemory' that safely assigns a
// 'value' to the memory in a 'protectedBuffer' that had been allocated with
// the specified 'blockList':
//..
    void setMemory(char                       *protectedBuffer,
                   int                         length,
                   char                        value,
                   bdema_ProtectableBlockList *blockList)
        // Set all bytes in the specified 'protectedBuffer' of the
        // specified 'length' to the specified 'value', using the specified
        // 'allocator' to handle memory protection.  The 'protectedBuffer'
        // will be returned to a protected, READ-ONLY, state before this call
        // returns.  The behavior is undefined unless the 'protectedBuffer'
        // was supplied by the 'allocator'.
    {
//..
// Instantiate the guard, unprotecting the memory previously supplied by
// 'allocator', allowing us to write to the memory that 'allocator' is
// managing (e.g. 'protectedBuffer'):
//..
        bdema_ProtectableMemoryScopedGuard<bdema_ProtectableBlockList>
                                                            guard(blockList);
//..
//..
// Write to the memory:
//..
//
        for (int i = 0; i < length; ++i) {
            protectedBuffer[i] = value;
        }
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    TestDisp testDispenser(TestDisp::BDEMA_DEFAULT_PAGE_SIZE, veryVeryVerbose);

    bslma_TestAllocator         talloc;
    bslma_DefaultAllocatorGuard guard(&talloc);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

        if (verbose) cout << endl << "Testing Usage Example"
                          << endl << "=====================" << endl;

        {
            bdema_ProtectableBlockList blockList(&testDispenser);
            bdema_MemoryBlockDescriptor block = blockList.allocate(2);
            char *buffer = static_cast<char *>(block.address());
            blockList.protect();
            setMemory(buffer, 2, '0', &blockList);
            ASSERT('0' == buffer[0]);
            ASSERT('0' == buffer[1]);

            blockList.protect();
            setMemory(buffer, 10, '1', &blockList);
            ASSERT('1' == buffer[0]);
            ASSERT('1' == buffer[1]);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR AND DESTRUCTOR
        //
        // Concerns:
        //   That the constructor unprotects the memory in the supplied
        //   allocator and the destructor protects the memory in the supplied
        //   allocator.
        //
        // Plan:
        //   Construct a write guard around a test implementation of a
        //   protected allocator.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing constructor and destructor"
                          << endl << "=================================="
                          << endl;
        {
            TestProtectableMemoryAllocator alloc(true);
            ASSERT(alloc.isProtected());
            {
                ASSERT(alloc.isProtected());
                bdema_ProtectableMemoryScopedGuard<
                            TestProtectableMemoryAllocator> guard(&alloc);
                ASSERT(!alloc.isProtected());
            }
            ASSERT(alloc.isProtected());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPER
        //
        // Concerns:
        //   The TestProtectableMemoryAllocator keeps the track of the
        //   protection state.
        //
        // Plan:
        //   Construct a test allocators with various initial values for the
        //   protection status, verify the status, toggle the status, verify
        //   the status.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Helper"
                          << endl << "==============" << endl;
        {
            TestProtectableMemoryAllocator a(true);
            TestProtectableMemoryAllocator b(false);
            TestProtectableMemoryAllocator c;

            ASSERT( a.isProtected());
            ASSERT(!b.isProtected());
            ASSERT( c.isProtected());

            a.protect();
            b.protect();
            c.protect();

            ASSERT( a.isProtected());
            ASSERT( b.isProtected());
            ASSERT( c.isProtected());

            a.unprotect();
            b.unprotect();
            c.unprotect();

            ASSERT(!a.isProtected());
            ASSERT(!b.isProtected());
            ASSERT(!c.isProtected());

            a.protect();
            b.protect();
            c.protect();

            ASSERT( a.isProtected());
            ASSERT( b.isProtected());
            ASSERT( c.isProtected());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic functionality of
        //  'bdema_ProtectableMemoryScopedGuard'
        //
        // Plan:
        //   Create a 'bdema_ProtectableMemoryScopedGuard' around a
        //   'TestProtectableMemoryAllocator' and verify that it marks the
        //    memory as unprotected on construction and marks it as protected
        //    on destruction.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        {
            TestProtectableMemoryAllocator alloc(true);
            ASSERT(alloc.isProtected());
            {
                ASSERT(alloc.isProtected());
                bdema_ProtectableMemoryScopedGuard<
                       TestProtectableMemoryAllocator> guard(&alloc);
                ASSERT(!alloc.isProtected());
            }
            ASSERT(alloc.isProtected());
        }
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
