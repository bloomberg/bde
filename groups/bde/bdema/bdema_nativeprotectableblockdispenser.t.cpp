// bdema_nativeprotectableblockdispenser.t.cpp  -*-C++-*-

#include <bdema_nativeprotectableblockdispenser.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatorexception.h>  // for testing only

#include <bsls_alignment.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()
#include <bsl_iostream.h>

#include <setjmp.h>
#include <signal.h>

#ifdef BSLS_PLATFORM__OS_UNIX
    // Unix includes for mprotect() and sysconf()
    #include <sys/mman.h>
    #include <unistd.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
    // Windows include for GetSystemInfo, VirtualAlloc, VirtualFree
    #include <windows.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// bdema_nativeprotectableblockdispenser component contains a factory
// and a platform specific implementations of bdema_protectedblocklist.  The
// class is a simple wrapper around the allocate and protect system calls.
//-----------------------------------------------------------------------------
// [2] Helper Functions (TestProtectedAllocator)
// [3] constructor, destructor
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
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdema_NativeProtectableBlockDispenser Obj;
typedef bdema_MemoryBlockDescriptor           BlkDesc;
typedef bdema_ProtectableBlockDispenser       BlkDisp;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
typedef jmp_buf JumpBuffer;
#else
typedef sigjmp_buf JumpBuffer;
#endif

static JumpBuffer g_jumpBuffer;
static bool       g_inTest = false;

extern "C" {

void segfaultHandler(int x)
// This is a segmentation fault signal handler.  It uses the global variables
// above to manage it's state.  Mark that a segmentation fault has occurred in
// g_fault and, if protection is under test, unprotect the memory pointed to by
// the allocator under test.
{
    if (g_inTest) {
#ifdef BSLS_PLATFORM__OS_WINDOWS
        longjmp(g_jumpBuffer, 1);
#else
        siglongjmp(g_jumpBuffer, 1);
#endif
    }
}

}

bool causesFault(void *data, char val)
// Return true if assigning the specified 'val' to the specified 'data' causes
// a memory fault.
{
    int  rc;
    bool fault = false;

    // set the signal handler to the test handler.
    signal(SIGSEGV, segfaultHandler);
#ifndef BSLS_PLATFORM__OS_WINDOWS
    signal(SIGBUS, segfaultHandler);
#endif

    // set the global test flag (used by the signal handler).
    g_inTest = true;

    // set the jump position.  Returns 0 on a true set, returns 1 if it's long
    // jumped to by the signal handler.

#ifdef BSLS_PLATFORM__OS_WINDOWS
    rc = setjmp(g_jumpBuffer);
#else
    rc = sigsetjmp(g_jumpBuffer, 1);
#endif

    if (rc == 1) {
        // long jump from the signal handler.
        fault = true;
    }
    else if (rc == 0) {
        ASSERT(0 == rc);
        // Assign the value.  If there is a memory fault this will jump into
        // the signal handler.
        *((char *)data) = val;
    }
    else {
        ASSERT(false);
    }

    // Replace the signal handler with the default handler.
    signal(SIGSEGV, SIG_DFL);
#ifndef BSLS_PLATFORM__OS_WINDOWS
    signal(SIGBUS, SIG_DFL);
#endif

    // set the global test flag (used by the signal handler)
    g_inTest = false;

    return fault;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#ifdef BSLS_PLATFORM__OS_UNIX
    const int PG_SIZE = sysconf(_SC_PAGESIZE);
#endif
#ifdef BSLS_PLATFORM__OS_WINDOWS
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    const int PG_SIZE = info.dwPageSize;
#endif

    switch (test) { case 0:
      case 4:{
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

///Usage
///-----
// The following is a simple example of using a
// 'bdema_NativeProtectableBlockDispenser' to allocate, protect,
// unprotect, and deallocate memory:
//..
      bdema_NativeProtectableBlockDispenser *dispenser =
                &bdema_NativeProtectableBlockDispenser::singleton();
//..
// Allocate a memory block from this dispenser:
//..
      int size = dispenser->minimumBlockSize();
      bdema_MemoryBlockDescriptor block = dispenser->allocate(size);
//..
// Unprotect the memory block before modifying it:
//..
      dispenser->unprotect(block);
//..
// Modify its contents:
//..
      char *memory = static_cast<char *>(block.address());
      std::memset(memory, 0, size);
                                     ASSERT(0 == memory[0]);
      memory[0] = 1;
                                     ASSERT(1 == memory[0]);
//..
// Once the memory is protected it cannot be modified:
//..
      dispenser->protect(block);
      // memory[0] = 2;  // WARNING: THIS WILL CAUSE A SEGMENTATION VIOLATION!
//..
// The block can be unprotected so that it can again be written to:
//..
      dispenser->unprotect(block);
      memory[0] = 2;
                                     ASSERT(2 == memory[0]);
      dispenser->deallocate(block);
//..
// Note that the block must also be unprotected in order to deallocate it.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PROTECT and UNPROTECT
        //
        // Concerns:
        //  Test that protect() and unprotect() protect and unprotect the
        //  memory passed to them.
        //
        // Plan:
        //  First, test that the return codes are returned correctly by making
        //  an invalid call to protect.  Then allocate a large block of memory,
        //  protected various portions of it, verify that the correct portions
        //  of the array are protected using the causesFault() function.
        //
        // Testing:
        //  int protect();
        //  int unprotect();
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing singleton factory, allocate, deallocate"
                          << "\n============================================="
                          << endl;
        Obj *dispenser = &Obj::singleton();

#ifndef BDES_PLATFORM__OS_FREEBSD
            // FreeBSD allows misaligned address in mprotect.
        {
            if (veryVerbose) cout << "\tVerify system error codes" << endl;

            // Validate that invalid calls to protect return non - zero
            // (e.g. that the error code from the system call is propagated)

            BlkDesc BLOCK = dispenser->allocate(1);
            ASSERT(BLOCK.size() == dispenser->minimumBlockSize());

            char *address = (char *)BLOCK.address();
            char *badAddress = address + 1; // Mis aligned memory to protect.
            BlkDesc BADBLOCK(badAddress, 0);
            ASSERT(0 != dispenser->protect(BADBLOCK));
            ASSERT(0 != dispenser->protect(BADBLOCK));
            dispenser->deallocate(BLOCK);
        }
#endif
        {
            if (veryVerbose) cout << "\tVerify system protection" << endl;
            struct {
                    int d_size;
                    int d_allocatedSize;
            } DATA[] = {
                { 1,            PG_SIZE },
                { 10,           PG_SIZE },
                { 100,          PG_SIZE },
                { PG_SIZE -1,   PG_SIZE },
                { PG_SIZE,      PG_SIZE },
                { PG_SIZE + 1,  2 * PG_SIZE },
                { 2*PG_SIZE -1, 2 * PG_SIZE },
                { 2*PG_SIZE,    2 * PG_SIZE },
                { 2*PG_SIZE+1,  3 * PG_SIZE }
            };

            const  int TEST_LOC  [] = { 0,
                                        PG_SIZE/2,
                                        PG_SIZE -1,
                                        PG_SIZE,
                                        PG_SIZE + 1,
                                        2*PG_SIZE -1,
                                        2*PG_SIZE,
                                        2*PG_SIZE+1,
                                        3*PG_SIZE-1};

            const int NUM_SIZES = sizeof(DATA)/sizeof(*DATA);
            const int NUM_TEST_LOC = sizeof(TEST_LOC)/sizeof(*TEST_LOC);

            for  (int i = 0; i < NUM_SIZES; ++i) {
                const int SIZE           = DATA[i].d_size;
                const int ALLOCATED_SIZE = DATA[i].d_allocatedSize;

                BlkDesc BLOCK = dispenser->allocate(SIZE);

                if (veryVeryVerbose) P(SIZE);

                char *data = static_cast<char *>(BLOCK.address());

                ASSERT(ALLOCATED_SIZE  == BLOCK.size());
                for (int j = 0; j < NUM_TEST_LOC; ++j) {
                    if (TEST_LOC[j] < ALLOCATED_SIZE) {
                        ASSERT(!causesFault(&data[TEST_LOC[j]], 'x'));
                    }
                }

                // protect the memory
                ASSERT(0 == dispenser->protect(BLOCK));
                //verify the correct portion is protected
                for (int j = 0; j < NUM_TEST_LOC; ++j) {
                    if (TEST_LOC[j] < ALLOCATED_SIZE) {
                        ASSERT(causesFault(&data[TEST_LOC[j]], 'x'));
                    }
                }

                // unprotect the memory
                ASSERT(0 == dispenser->unprotect(BLOCK));
                for (int j = 0; j < NUM_TEST_LOC; ++j) {
                    if (TEST_LOC[j] < ALLOCATED_SIZE) {
                        ASSERT(!causesFault(&data[TEST_LOC[j]], 'x'));
                    }
                }
                dispenser->deallocate(BLOCK);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SINGLETON FACTORY ALLOCATE AND FREE
        //
        // Concerns:
        //  That the singleton factory returns a valid instance of the native
        //  protected block allocator, and that the methods allocate and
        //  deallocate methods properly delegate to the appropriate
        //  (overwritten) system calls.
        //
        // Plan:
        //   Construct a platform specific block allocator and call the
        //   various allocations methods on it.  Use causesFault() to verify
        //   (to the extent possible) that memory is allocated.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing singleton factory, allocate, deallocate"
                          << "\n============================================="
                          << endl;

        struct {
                int d_size;
                int d_allocatedSize;
        } DATA[] = {
            { 1,            PG_SIZE },
            { 10,           PG_SIZE },
            { 100,          PG_SIZE },
            { PG_SIZE -1,   PG_SIZE },
            { PG_SIZE,      PG_SIZE },
            { PG_SIZE + 1,  2 * PG_SIZE },
            { 2*PG_SIZE -1, 2 * PG_SIZE },
            { 2*PG_SIZE,    2 * PG_SIZE },
            { 2*PG_SIZE+1,  3 * PG_SIZE }
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
        BlkDesc BLOCKS[NUM_DATA];

        Obj *dispenser = &Obj::singleton();
        for  (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE           = DATA[i].d_size;

            BLOCKS[i] = dispenser->allocate(SIZE);
            ASSERT(0 == ((bsls_PlatformUtil::size_type)BLOCKS[i].address()) %
                        PG_SIZE);
            for (int j = 0; j < i; ++j) {
                ASSERT(BLOCKS[j].address() != BLOCKS[i].address());
            }
            char *data = static_cast<char *>(BLOCKS[i].address());
            ASSERT(!causesFault(&data[0], 'x'));
            ASSERT(!causesFault(&data[SIZE/2], 'x'));
            ASSERT(!causesFault(&data[SIZE-1], 'x'));
        }
        for  (int i = 0; i < NUM_DATA; ++i) {
            dispenser->deallocate(BLOCKS[i]);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HELPER
        //
        // Concerns:
        //  That the caughtFault() will return true for an assignment that
        //  causes a fault and false for an assignment that doesn't cause a
        //  fault.
        //
        // Testing:
        //   bool caughtFault(void *mem, char val);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Helper: caughtFault\n"
                          << "==========================="
                          << endl;

        char good = 'a';
        ASSERT(!causesFault(&good, 'x'));
        ASSERT( causesFault(0, 'x'));
        ASSERT( causesFault(0, 'x'));
        ASSERT('x' == good);
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
