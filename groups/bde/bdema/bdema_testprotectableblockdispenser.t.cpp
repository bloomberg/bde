// bdema_testprotectableblockdispenser.t.cpp -*-C++-*-

#include <bdema_testprotectableblockdispenser.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>      // BDES_PLATFORM__ defines
#include <bsls_platformutil.h>  // BDES_PLATFORMUTIL__ defines
#include <bsls_objectbuffer.h>

#include <bsl_cstdio.h>               // printf()
#include <bsl_cstdlib.h>              // atoi()
#include <bsl_cstring.h>              // memset(), strlen()
#include <bsl_new.h>
#include <bsl_iostream.h>
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>             // pipe(), close() and dup().
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  This "test"
// implementation calls 'malloc' and 'free' directly.  We must verify that
// instances of this concrete class do not call global operators 'new' and
// 'delete'.  We can do that by redefining these global operators and
// instrumenting them to be sure that these operators are in fact not called.
//
// We also need to verify that when bad addresses are supplied that we can
// detect them and report the problem to 'stdout'.  Since this behavior is
// not an error during the testing of this component, we will first set the
// quiet flag to suppress the output part, but will still verify the status
// to ensure that the problem was in fact detected.
//
// We must also verify that when exceptions are enabled, the test allocator
// throws an exception after the number of requests exceeds the allocator's
// (non-negative) allocation limit.  We achieve this by repeatedly allocating
// memory from the test allocator until the number of requests exceeds the
// allocation limit, then verify that exceptions thrown by the allocator are
// caught and that they contain the expected contents.
//-----------------------------------------------------------------------------
// [ 1] bdema_TestProtectableBlockDispenser(int verboseFlag = 0);
// [ 2] ~bdema_TestProtectableBlockDispenser();
// [ 3] bdema_MemoryBlockDescriptor allocate(int size);
// [ 3] void deallocate(bdema_MemoryBlockDescriptor);
// [ 2] void setNoAbort(int noAbortFlag);
// [ 2] void setQuiet(int quietFlag);
// [ 2] void setVerbose(int verboseFlag);
// [ 2] void setAllocationLimit(int numAlloc);
// [ 2] bool isNoAbort() const;
// [ 2] bool isQuiet() const;
// [ 2] bool isVerbose() const;
// [ 1] int numBytesInUse() const;
// [ 1] int numBlocksInUse() const;
// [ 1] int numBytesMax() const;
// [ 1] int numBlocksMax() const;
// [ 1] int numBytesTotal() const;
// [ 1] int numBlocksTotal() const;
// [ 1] int numErrors() const;
// [ 2] int status() const;
// [ 2] int allocationLimit() const;
// [ 1] int lastAllocateNumBytes() const;
// [ 1] int lastDeallocateNumBytes() const;
// [ 1] void *lastAllocateAddress() const;
// [ 1] void *lastDeallocateAddress() const;
// [ 1] int numAllocations() const;
// [ 1] int numDeallocations() const;
// [ 1] int protect(const bdema_MemoryBlockDescriptor&) const;
// [ 1] int unprotect(const bdema_MemoryBlockDescriptor&) const;
// [ 1] bool isProtected(const bdema_MemoryBlockDescriptor&) const;
// [ 1] int numProtectedBlocks();
// [ 5] ostream& operator<<(
//                      ostream&                                   stream,
//                      const bdema_TestProtectableBlockDispenser& rhs);
//-----------------------------------------------------------------------------
// [ 7] USAGE TEST - Make sure usage example for exception neutrality works.
// [ 6] Ensure that exception is thrown after allocation limit is exceeded.
// [ 1] Make sure that all counts are initialized to zero (placement new).
// [ 1] Make sure that global operators new and delete are *not* called.
// [ 3] Ensure that the allocator is incompatible with new/delete.
// [ 3] Ensure that the allocator is incompatible with malloc/free.
// [ 3] Ensure that mismatched deallocations are detected/reported.
// [ 3] Ensure that repeated deallocations are detected/reported.
// [ 3] Ensure that an invalid cached length is reported.
// [ 3] Ensure that deallocated memory is scribbled.
// [ 3] Ensure that memory leaks (byte/block) are detected/reported.
// [ 3] Ensure that invalid block descriptor sizes are detected/reported.
// [ 3] Ensure that invalid protect and unprotect operations are reported.
// [ 3] Ensure that deallocating protected memory is reported.
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

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n"; \
              aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define Q_(X) cout << "<| " #X " |>" << flush;  // Q(X) without '\n'
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                       GLOBAL TYPEDEFS AND CONSTANTS
//-----------------------------------------------------------------------------

// This is copied from 'bdema_testprotectableblockdispenser.cpp'
// to compare with scribbled deallocated memory.
const unsigned char SCRIBBLED_MEMORY = 0xA5;   // byte used to scribble
                                               // deallocated memory

typedef bdema_TestProtectableBlockDispenser Obj;
typedef bdema_MemoryBlockDescriptor         BlkDesc;
typedef bdema_ProtectableBlockDispenser     BlkDisp;
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// integerstack.h
    class IntegerStack {
        // This is a trivial implementation of a stack of ints whose data
        // has READ-ONLY access protection.  This class does not perform
        // bounds checking.

        bdema_MemoryBlockDescriptor     d_data;        // block of memory
                                                       // holding the stack

        int                            *d_cursor_p;    // top of the stack
        bdema_ProtectableBlockDispenser
                                       *d_dispenser_p; // memory dispenser
                                                       // (held not owned)

  // ...

        // PRIVATE AUXILIARY METHODS
        void resize()
            // Increase the size
        {
            bdema_MemoryBlockDescriptor oldData = d_data;
            d_data = d_dispenser_p->allocate(d_data.size() * 2);
            d_cursor_p = (int *)((char *)d_data.address() +
                             ((char *)d_cursor_p - (char *)oldData.address()));
            memcpy(d_data.address(),oldData.address(), oldData.size());
            d_dispenser_p->deallocate(oldData);
        }

      public:
        // CREATORS
        IntegerStack(
                   int                                    initialSize,
                   bdema_ProtectableBlockDispenser *protectedDispenser)
            // Create an 'IntegerStack' of the specified 'initialSize' given
            // the specified 'protectedDispenser'.   The behavior is
            // undefined unless '0 < = maxStackSize' and
            // 'protectedDispenser != 0'.
        : d_data()
        , d_dispenser_p(protectedDispenser)
        {
            d_data   = d_dispenser_p->allocate(initialSize * sizeof(int));
            ASSERT(!d_data.isNull());
            d_cursor_p = (int *)d_data.address();
            d_dispenser_p->protect(d_data);
        }
//..
// We must unprotect the dispenser before modifying or deallocating
// memory:
//..
        ~IntegerStack()
            // Destroy this object and release its memory.
        {
            d_dispenser_p->unprotect(d_data);
            d_dispenser_p->deallocate(d_data);
        }
  // ...
        // MANIPULATORS
        void push(int value)
            // Push the specified 'value' onto the stack.
        {
            d_dispenser_p->unprotect(d_data);
            if (sizeof(int) >
               (char *)d_data.address() + d_data.size() - (char *)d_cursor_p){
                resize();
            }
            *d_cursor_p++ = value;
            d_dispenser_p->protect(d_data);
        }

        int pop()
            // Remove the top value from the stack and return it.
        {
            // The buffer isn't modified so there is no need to unprotect it.
            const int value = *--d_cursor_p;
            return value;
        }

  // ...
    };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// integerstack.t.cpp

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                      \
    {                                                                     \
        static int firstTime = 1;                                         \
        if (veryVerbose && firstTime) cout <<                             \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;           \
        firstTime = 0;                                                    \
    }                                                                     \
    if (veryVeryVerbose) cout <<                                          \
        "### Begin bdema exception test." << endl;                        \
    int bdemaExceptionCounter = 0;                                        \
    static int bdemaExceptionLimit = 100;                                 \
    testDispenser.setAllocationLimit(bdemaExceptionCounter);              \
    do {                                                                  \
        try {

#define END_BDEMA_EXCEPTION_TEST                                          \
        } catch (bslma_TestAllocatorException& e) {                       \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {  \
                --bdemaExceptionLimit;                                    \
                cout << "(*** " << bdemaExceptionCounter << ')';          \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "       \
                    << "alloc limit = " << bdemaExceptionCounter << ", "  \
                    << "last alloc size = " << e.numBytes();              \
                }                                                         \
                else if (0 == bdemaExceptionLimit) {                      \
                    cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                         \
                cout << endl;                                             \
            }                                                             \
            testDispenser.setAllocationLimit(++bdemaExceptionCounter);    \
            continue;                                                     \
        }                                                                 \
        testDispenser.setAllocationLimit(-1);                             \
        break;                                                            \
    } while (1);                                                          \
    if (veryVeryVerbose) cout <<                                          \
        "### End bdema exception test." << endl;                          \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                        \
{                                                                         \
    static int firstTime = 1;                                             \
    if (verbose && firstTime) { cout <<                                   \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;           \
        firstTime = 0;                                                    \
    }                                                                     \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//-----------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//-----------------------------------------------------------------------------

static int globalNewCalledCount = 0;
static int globalNewCalledCountIsEnabled = 0;

static int globalDeleteCalledCount = 0;
static int globalDeleteCalledCountIsEnabled = 0;

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(bsl::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        printf ("global new called, count = %d: %p\n",
                                                globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        printf("global delete freeing: %p\n", address);
    }

    free(address);
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Additional code for usage test:
    Obj testDispenser(Obj::BDEMA_DEFAULT_PAGE_SIZE, veryVeryVerbose);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // TEST USAGE
        //   Verify that the usage example for testing exception neutrality is
        //   free of syntax errors and works as advertised.
        //
        // Testing:
        //   USAGE TEST - Make sure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST USAGE" << endl
                                  << "==========" << endl;

        typedef short Element;
        const Element VALUES[] = { 1, 2, 3, 4, -5 };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const Element &V0 = VALUES[0],
                      &V1 = VALUES[1],
                      &V2 = VALUES[2],
                      &V3 = VALUES[3],
                      &V4 = VALUES[4];

        struct {
            int d_line;
            int d_numElem;
            short d_exp[NUM_VALUES];
        } DATA[] = {
            { L_, 0, { 0 } },
            { L_, 1, { V0 } },
            { L_, 5, { V0, V1, V2, V3, V4 } }
        };

        const int NUM_TEST = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            const int    LINE     = DATA[ti].d_line;
            const int    NUM_ELEM = DATA[ti].d_numElem;
            const short *EXP      = DATA[ti].d_exp;

            BEGIN_BDEMA_EXCEPTION_TEST {
                IntegerStack mA(1, &testDispenser);
                const IntegerStack& A = mA;
                for (int testI = 0; testI < NUM_ELEM; ++testI) {
                    for (int ei = 0; ei <= testI; ++ei) {
                        mA.push(VALUES[ei]);
                    }
                    for (int ei = testI; ei >= 0; --ei) {
                        LOOP2_ASSERT(testI, ei, VALUES[ei] == mA.pop());
                    }
                }
            } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) testDispenser.print();
        } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST MEMORY PROTECTION
        //   Create a test allocator and a block of memory.  Verify that
        //   modifying the block of memory will generate an error if and only
        //   if the memory is protected at the time.
        //
        // Testing:
        //   Ensure that exception is thrown after allocation limit is exceeded
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST PROTECTION" << endl
                                  << "===============" << endl;

        // The following is 256 unsigned character values randomly generated
        // by a script.
        unsigned char DATA[] =
            { 49, 38, 185, 121, 8, 99, 123, 74, 234, 148, 128, 172, 209,
              155, 2, 50, 27, 248, 153, 36, 31, 229, 62, 137, 51,
              220, 144, 189, 43, 70, 188, 195, 53, 154, 236, 226, 115,
              175, 96, 193, 123, 79, 207, 44, 209, 73, 121, 122, 210,
              42, 133, 94, 252, 198, 48, 169, 4, 7, 208, 206, 168,
              129, 199, 119, 244, 28, 102, 193, 166, 90, 3, 30, 200,
              89, 82, 196, 77, 93, 12, 53, 134, 144, 248, 136, 121,
              9, 66, 113, 190, 251, 15, 100, 37, 157, 234, 110, 128,
              139, 36, 46, 111, 68, 234, 31, 240, 164, 65, 168, 110,
              14, 151, 151, 49, 69, 197, 154, 218, 154, 76, 193, 82,
              90, 236, 15, 214, 29, 123, 215, 27, 229, 42, 67, 177,
              213, 1, 100, 76, 9, 86, 34, 143, 89, 209, 211, 1,
              98, 154, 113, 44, 57, 103, 167, 77, 40, 39, 18, 238,
              64, 32, 192, 39, 108, 18, 34, 125, 69, 37, 52, 248,
              112, 26, 26, 246, 231, 185, 137, 221, 131, 173, 103, 151,
              186, 119, 136, 183, 241, 81, 149, 111, 98, 150, 247, 214,
              174, 113, 69, 70, 231, 80, 29, 183, 13, 151, 9, 90,
              254, 79, 52, 232, 112, 114, 117, 15, 106, 143, 90, 168,
              40, 48, 122, 241, 165, 242, 23, 39, 236, 130, 140, 164,
              221, 213, 239, 195, 12, 5, 217, 135, 10, 2, 6, 143,
              99, 226, 92, 208, 54, 182, 119, 238, 80, 70, 97, 218,
              112, 223, 98 };
        int DATA_SIZE = sizeof(DATA)/sizeof(*DATA);
        int SIZES[] = {2, 4, 5, 8, 10, 16, 32, 64 };
        int NUM_SIZES = sizeof(SIZES)/sizeof(*SIZES);

        for (int i = 0; i < NUM_SIZES; ++i) {
            // For each size of allocation perform a series of memory tests
            int SIZE = SIZES[i];

            Obj mX(SIZE, veryVeryVerbose);
            mX.setNoAbort(verbose); mX.setQuiet(!veryVerbose);

            BlkDesc  block     = mX.allocate(SIZE);
            char    *blockData =(char *)block.address();
            ASSERT(SIZE == block.size());

            if (verbose) {
                bsl::cout << "\tVerify updating one byte of protected memory"
                          << bsl::endl;
            }
            for (int j = 0; j < SIZE; ++j) {
                int mismatches = mX.numErrors();
                memset(blockData, 0, SIZE);
                mX.protect(block);
                mX.unprotect(block);

                ASSERT( mismatches == mX.numErrors());
                mX.protect(block);
                blockData[j]++;
                mX.unprotect(block);
                ASSERT( mismatches + 1 == mX.numErrors());
            }

            if (verbose) {
                bsl::cout << "\tVerify using a random initial block"
                          << bsl::endl;
            }
            for (int j = 0; j < DATA_SIZE / SIZE; ++j) {
                for (int k = 0; k < SIZE; ++k) {
                    int mismatches = mX.numErrors();
                    memcpy(blockData, &DATA[j * SIZE], SIZE);
                    mX.protect(block);
                    mX.unprotect(block);
                    ASSERT( mismatches == mX.numErrors());
                    mX.protect(block);
                    blockData[k]++;
                    mX.unprotect(block);
                    ASSERT( mismatches + 1 == mX.numErrors());
                }
                for (int k = 0; k < DATA_SIZE / SIZE; ++k) {
                    int mismatches = mX.numErrors();
                    memcpy(blockData, &DATA[j * SIZE], SIZE);
                    mX.protect(block);
                    mX.unprotect(block);
                    ASSERT( mismatches == mX.numErrors());
                    mX.protect(block);
                    memcpy(blockData, &DATA[k * SIZE], SIZE);
                    mX.unprotect(block);
                    LOOP3_ASSERT( i, j, k,
                               j == k || mismatches + 1 == mX.numErrors());
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST ALLOCATION LIMIT
        //   Create a test allocator and set its allocation limit to varying
        //   values.  Verify that exception is thrown with the correct content
        //   for every allocation request that occurs after the number of
        //   requests exceeds the current allocation limit.  Also verify that
        //   exception is never thrown for negative allocation limits.
        //
        // Testing:
        //   Ensure that exception is thrown after allocation limit is exceeded
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST ALLOCATION LIMIT" << endl
                                  << "=====================" << endl;

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "Testing for exception thrown after exceeding "
                          << "allocation limit." << endl;

        const int NUM_ALLOC = 5;
        const int LIMIT[] = { 0, 1, 4, 5, -1, -100 };
        const int NUM_TEST = sizeof LIMIT / sizeof *LIMIT;

        Obj mX(Obj::BDEMA_DEFAULT_PAGE_SIZE, veryVeryVerbose);

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            mX.setAllocationLimit(LIMIT[ti]);

            for (int ai = 0; ai < NUM_ALLOC; ++ai) {
                const int SIZE = ai + 1; // alloc size varies for each test
                if (veryVerbose) { P_(ti); P_(ai); P_(SIZE); P(LIMIT[ti]); }
                try {
                    BlkDesc blk = mX.allocate(SIZE);
                    mX.deallocate(blk);
                }
                catch (bslma_TestAllocatorException& e) {
                    int numBytes = e.numBytes();
                    if (veryVerbose) { cout << "Caught: "; P(numBytes); }
                    LOOP2_ASSERT(ti, ai, LIMIT[ti] == ai);
                    LOOP2_ASSERT(ti, ai, SIZE == numBytes);
                    continue;
                }
                LOOP2_ASSERT(ti, ai, LIMIT[ti] != ai);
            }
        }
#else
        if (verbose) cout << "No testing.  Exceptions are not enabled."
                          << endl;
#endif
} break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST OUTPUT OPERATOR (<<)
        //   Lightly verify that the output operator works.
        //
        // Testing:
        //   ostream& operator<<(
        //            ostream&                                         lhs,
        //            const bdema_TestProtectableBlockDispenser& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT OPERATOR (<<)" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTest a single case with unique fields." <<endl;

        const int X = 64;
        Obj a(X);

        if (verbose) cout << "\tSet up unique fields." <<endl;
        BlkDesc b1 = a.allocate(1);
        BlkDesc b2 = a.allocate(X - 1);
        BlkDesc b3 = a.allocate(X);
        BlkDesc b4 = a.allocate(X + 1);

        a.deallocate(b4);
        a.deallocate(b1);

        b1 = a.allocate(1);

        ASSERT(3 == a.numBlocksInUse());  ASSERT(3 * X == a.numBytesInUse());
        ASSERT(4 == a.numBlocksMax());    ASSERT(5 * X == a.numBytesMax());
        ASSERT(5 == a.numBlocksTotal());  ASSERT(6 * X == a.numBytesTotal());
        ASSERT(0 == a.numErrors());

// The following code uses pipe() and fork(), so only works on Unix.
#ifdef BSLS_PLATFORM_OS_UNIX
        if (verbose) cout << "\tCompare with expected result." <<endl;

        const int SIZE = 1000;     // Must be big enough to hold output string.
        const char XX = (char) 0xFF;  // Value used to represent an unset char.

        char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
        const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

        char buf[SIZE];
        memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' char values.
        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t3\t192\n"
            "             MAX\t4\t320\n"
            "           TOTAL\t5\t384\n"
            "      NUM ERRORS\t0\n"
            "--------------------------------------------------\n"
            ;

        if (veryVerbose) cout << "\nEXPECTED FORMAT:" << endl << FMT << endl;

        // Because bdema is a low-level utility,
        // bdema_TestProtectableBlockDispenser does not have a function
        // to print to ostream, and thus cannot print to a strstream.  The
        // print() member function always prints to stdout.  The code below
        // forks a process and captures stdout to a memory
        // buffer.
        int pipes[2];
        int sz;
        pipe(pipes);
        if (fork()) {
            // Parent process.  Read pipe[0] into memory
            sz = read(pipes[0], buf, SIZE);
            if (sz >= 0) { buf[sz] = '\0'; }
        }
        else {
            // Child process, print to pipes[1].
            close(1);
            dup(pipes[1]);

            // This call print() function sends its output to the pipe,
            // which is in turn read into 'buf' by the parent process.
            a.print();

            exit(0);
        }

        if (veryVerbose) cout << "\nACTUAL FORMAT:" << endl << buf << endl;

        ASSERT(sz > 0);     // Check that something was printed (and read).
        ASSERT(sz < SIZE);  // Check buffer is large enough.
        ASSERT(XX == buf[SIZE - 1]);  // Check for overrun.
        ASSERT(0 == memcmp(buf, FMT, sz));
#endif // defined BSLS_PLATFORM_OS_UNIX

        a.deallocate(b1);
        a.deallocate(b2);
        a.deallocate(b3);

    } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST ALLOCATION SIZES
        //   Ensure that the amount of memory allocated on a call to allocate
        //   behaves correctly with respect to the page size.
        //
        // Testing:
        //   bdema_MemoryBlockDescriptor allocate(int size);
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TEST ALLOCATION SIZES" << endl
                                  << "=====================" << endl;

        struct  {
            int d_requestPages;        // number of pages of mem to request
            int d_requestPagesAdjust;  // adjustment to requested mem size
            int d_allocatedPages;      // actual # of allocated pages for rqst
        } ALLOC_SIZES []  = {
            { 0, 0, 0}, { 0, 1, 1}, { 0, 3, 1}, { 1, -1, 1}, { 1, 0, 1},
            { 1, 1, 2}, { 1, 3, 2}, { 2, -1, 2}, { 2, 0, 2},
            { 2, 1, 3}, { 2, 3, 3},
            { 5, -1, 5}, { 5, 0, 5},
            { 5, 1, 6}
        };
        const int NUM_ALLOC_SIZES = sizeof(ALLOC_SIZES)/sizeof(*ALLOC_SIZES);
        const int PAGE_SIZES[] = {5, 13, 20, 64};
        const int NUM_PAGE_SIZES = sizeof(PAGE_SIZES)/sizeof(*PAGE_SIZES);

        for (int i = 0; i < NUM_PAGE_SIZES; ++i) {
            const int PG_SIZE = PAGE_SIZES[i];
            Obj a(PG_SIZE, veryVeryVerbose);
            for (int j = 0; j < NUM_ALLOC_SIZES; ++j) {
                const int rqstPages   = ALLOC_SIZES[j].d_requestPages;
                const int adjustment  = ALLOC_SIZES[j].d_requestPagesAdjust;
                const int allocdPages = ALLOC_SIZES[j].d_allocatedPages;

                const int requestSize  = (rqstPages * PG_SIZE) + adjustment;
                const int expectedSize = allocdPages * PG_SIZE;

                BlkDesc block = a.allocate(requestSize);
                ASSERT((0 != requestSize && 0 != block.address()) ||
                       (0 == requestSize && 0 == block.address()));
                ASSERT( expectedSize == block.size());

                a.deallocate(block);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ERROR COUNTS
        //   Disable the abort mode and ensure that attempting failure modes
        //   is correctly detected and logged in the numMismatches counter.
        //   Also verify that status is correct.  Note that this test case
        //   will cause purify errors.
        //
        // Testing:
        //   ~bdema_TestProtectableBlockDispenser();
        //   bdema_MemoryBlockDescriptor allocate(int size);
        //   void deallocate(const bdema_MemoryBlockDescriptor&);
        //
        //   Ensure that the allocator is incompatible with new/delete.
        //   Ensure that the allocator is incompatible with malloc/free.
        //   Ensure that mismatched deallocations are detected/reported.
        //   Ensure that repeated deallocations are detected/reported.
        //   Ensure that an invalid cached length is reported.
        //   Ensure that deallocated memory is scribbled.
        //   Ensure that memory leaks (byte/block) are detected/reported.
        //   Ensure that bad block descriptors cannot be used
        //   Ensure that modifying protected blocks are detected/reported
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST ERROR COUNTS" << endl
                                  << "=================" << endl;

        if (verbose) cout << endl
            << "Note:" << endl
            << "  Error messages can be viewed in veryVerbose mode" << endl
            << "  and Memory trace messages in veryVeryVerbose mode." << endl;

        const char *const LINE =
        "-------------------------------------------------------------------";

        {
            int i;
            if (verbose) cout <<
                "\nEnsure incompatibility with new/delete." << endl;
            const int X = Obj::BDEMA_DEFAULT_PAGE_SIZE;
            Obj a(Obj::BDEMA_DEFAULT_PAGE_SIZE, veryVeryVerbose);
            a.setNoAbort(verbose); a.setQuiet(!veryVerbose);
            void    *p = operator new(100);
            BlkDesc  bk = BlkDesc(p, 100);
            for (i = 0; i < 100; ++i) { ((char *)p)[i] = i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 == a.status());
                                ASSERT(0 == a.numErrors());

            if (verbose) cout << "\t[deallocate unallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numErrors());
            operator delete(p);
            bk = a.allocate(7); ((char *)bk.address())[0] = (char) 0xA7;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(1 == a.numErrors());

            a.deallocate(bk);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numErrors());

            if (verbose) cout <<
              "\nEnsure repeated deallocations are detected/reported." << endl;

            if (verbose) cout << "\t[deallocate deallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numErrors());

            if (verbose) cout <<
                "\nEnsure incompatibility with malloc/free." << endl;
            p  = malloc(200);
            bk = BlkDesc(p, 200);
            for (i = 0; i < 200; ++i) { ((char *)p)[i] = i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numErrors());
            if (verbose) cout << "\t[deallocate unallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(3 == a.status());
                                ASSERT(3 == a.numErrors());
            free(p);

            bk = a.allocate(5); ((char *)bk.address())[0] = (char) 0xA5;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(3 == a.numErrors());
            if (verbose) cout <<
          "\nEnsure immediately repeated deallocations are detected/reported."
                                                                       << endl;
            if (verbose) cout << "\t[deallocate deallocated pointer]" << endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(bk);
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(4 == a.status());
                                ASSERT(4 == a.numErrors());

            bk = a.allocate(3);  ((char *)bk.address())[0] = (char) 0xA3;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(1 == a.numBlocksMax());
                                ASSERT(X == a.numBytesMax());
                                ASSERT(3 == a.numBlocksTotal());
                                ASSERT(3*X == a.numBytesTotal());
                                ASSERT(4 == a.status());
                                ASSERT(4 == a.numErrors());

            if (verbose) cout <<
            "\nEnsure mismatched deallocations are detected/reported." << endl;
            if (verbose) cout << "\t[deallocate pointer + sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            BlkDesc bkoffset = BlkDesc( ((int *)bk.address()) + 1, bk.size());
            a.deallocate(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(5 == a.numErrors());

            if (verbose) cout << "\t[protect pointer + sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.protect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(6 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            if (verbose) cout << "\t[unprotect pointer + sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.unprotect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(7 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            if (verbose) cout << "\t[deallocate pointer - sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            bkoffset = BlkDesc(((int *)bk.address()) - 1, bk.size());
            a.deallocate(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(8 == a.numErrors());

            if (verbose) cout << "\t[protect pointer - sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.protect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(9 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            if (verbose) cout << "\t[unprotect pointer - sizeof(int)]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.unprotect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(10 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            a.deallocate(bk);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 < a.status());
                                ASSERT(10 == a.numErrors());

            if (verbose) cout <<
               "\nEnsure an invalid cached length is reported." << endl;

            struct B {
                int d_m;
                int d_s;
            } b[2] = { { 0xdeadbeef, -1 }, { 0x11, 0x22 } };
            bk = BlkDesc(b + 1, sizeof(b));
            if (verbose) cout <<"\t[deallocate memory with bad length]" <<endl;
            if (veryVerbose) cout << LINE << endl;
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(11 == a.numErrors());

            bk = a.allocate(9);  ((char *)bk.address())[0] = (char) 0xA9;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(11 == a.numErrors());

            if (verbose) cout <<
                "\nEnsure invalid descriptor sizes are detected." << endl;
            if (verbose) cout << "\t[deallocate desc(pointer, incorrect size)]"
                              <<endl;
            bkoffset = BlkDesc(bk.address(), bk.size() - 1);
            a.deallocate(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(12 == a.numErrors());

            bkoffset = BlkDesc(bk.address(), bk.size() + 1);
            a.deallocate(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(13 == a.numErrors());

            a.protect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(14 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            a.unprotect(bkoffset);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(15 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            a.protect(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(15 == a.numErrors());
                                ASSERT(1 == a.numBlocksProtected());
            if (verbose) cout <<
                "\nEnsure deallocations of protected memory fail." << endl;
            if (verbose) cout << "\t[deallocate protected mem]"
                              <<endl;
            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(16 == a.numErrors());
                                ASSERT(1 == a.numBlocksProtected());

            a.unprotect(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(16 == a.numErrors());
                                ASSERT(0  == a.numBlocksProtected());

            a.deallocate(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(16 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            bk = a.allocate(9);  ((char *)bk.address())[0] = (char) 0xA9;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(16 == a.numErrors());

            a.protect(bk);
            a.unprotect(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(16 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            if (verbose) cout << "\t[modify protected memory]" << endl;
            a.protect(bk);
            {
                char *mem = (char *)bk.address();
                mem[0]++;
                mem[1]++;
            }
            a.unprotect(bk);
            if (veryVerbose) cout << LINE << endl;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(X == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(17 == a.numErrors());
                                ASSERT(0 == a.numBlocksProtected());

            if (verbose) {
                cout << "\nEnsure deallocations of a block\n" << endl;
            }
            if (verbose) cout << "\t[memory leak]" << endl;
            if (veryVerbose) cout << LINE << endl;
        }
        if (veryVerbose) cout << LINE << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GET/SET FLAGS
        //   Verify that each of the get/set flags is working properly.
        //   Also verify that status correctly returns 0.
        //
        // Testing:
        //   void setVerbose(int verboseFlag);
        //   bool isVerbose() const;
        //   void setNoAbort(int noAbortFlag);
        //   bool isNoAbort() const;
        //   void setQuiet(int quietFlag);
        //   bool isQuiet() const;
        //   void setAllocationLimit(int numAlloc);
        //   int allocationLimit() const;
        //   int status() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GET/SET FLAGS" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTest get/set flags" << endl;
        {
            Obj a(9, veryVeryVerbose);

            ASSERT(9 == a.pageSize());
            ASSERT(0 == a.isQuiet());
            ASSERT(0 == a.isNoAbort());
            ASSERT(veryVeryVerbose == a.isVerbose());
            ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tVerbose" << endl;

            a.setVerbose(10);   ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(1 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setVerbose(0);    ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tQuite" << endl;

            a.setQuiet(10);     ASSERT(1 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setQuiet(0);      ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tNoAbort" << endl;

            a.setNoAbort(10);   ASSERT(0 == a.isQuiet());
                                ASSERT(1 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setNoAbort(0);    ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) cout << "\tAllocationLimit" << endl;

            a.setAllocationLimit(5);
                                ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(5 ==  a.allocationLimit());

            a.setAllocationLimit(0);
                                ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 == a.allocationLimit());

            a.setAllocationLimit(-1);
                                ASSERT( 0 == a.isQuiet());
                                ASSERT( 0 == a.isNoAbort());
                                ASSERT( 0 == a.isVerbose());
                                ASSERT(-1 == a.allocationLimit());

            if (verbose) cout << "\tStatus" << endl;

                                ASSERT(0 == a.status());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST:
        //   Create a test allocator in a buffer to make sure each field
        //   is properly initialized (with no verbose flag).  Then create a
        //   test allocator on the program stack and verify that all of the
        //   non-error counts are working properly.  Ensure that neither
        //   'new' or 'delete' are called.
        //
        // Testing:
        //   Make sure that all counts are initialized to zero (placement new).
        //
        //   bdema_TestProtectableBlockDispenser(int verboseFlag = 0);
        //   int numBytesInUse() const;
        //   int numBlocksInUse() const;
        //   int numBytesMax() const;
        //   int numBlocksMax() const;
        //   int numBytesTotal() const;
        //   int numBlocksTotal() const;
        //   int numErrors() const;
        //   int lastAllocateNumBytes() const;
        //   int lastDeallocateNumBytes() const;
        //   void *lastAllocateAddress() const;
        //   void *lastDeallocateAddress() const;
        //   int numAllocations() const;
        //   int numDeallocations() const;
        //   int numBlocksProtected() const;
        //   bool isBlockProtected() const;
        //   int protect(const bdema_MemoryBlockDescriptor& )
        //   int unprotect(const bdema_MemoryBlockDescriptor& )
        //
        //   Make sure that global operators new and delete are *not* called.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nCreate an allocator in a buffer" << endl;

        const int X = Obj::BDEMA_DEFAULT_PAGE_SIZE;
        bsls_ObjectBuffer<Obj> arena;

        memset(&arena, 0xA5, sizeof arena);
        Obj *p = new(&arena) Obj(X);

        if (verbose) cout <<
            "\nMake sure all counts/and flags are initialized" << endl;

        if (veryVerbose) p->print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == p->numBlocksInUse());
        ASSERT(0 == p->numBytesInUse());
        ASSERT(0 == p->numBlocksMax());
        ASSERT(0 == p->numBytesMax());
        ASSERT(0 == p->numBlocksTotal());
        ASSERT(0 == p->numBytesTotal());
        ASSERT(0 == p->isQuiet());
        ASSERT(0 == p->isNoAbort());
        ASSERT(0 == p->isVerbose());
        ASSERT(0 == p->status());
        ASSERT(0 == p->lastAllocateNumBytes());
        ASSERT(0 == p->lastAllocateAddress());
        ASSERT(0 == p->lastDeallocateNumBytes());
        ASSERT(0 == p->lastDeallocateAddress());
        ASSERT(0 == p->numAllocations());
        ASSERT(0 == p->numDeallocations());
        ASSERT(0 == p->numBlocksProtected());

        if (verbose) cout << "\nCreate an allocator" << endl;

        Obj a(X, veryVeryVerbose);

        if (verbose) cout << "\nMake sure counts work properly" << endl;

        if (veryVerbose) a.print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocateNumBytes());
        ASSERT(0 == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(0 == a.numBlocksProtected());

        if (verbose) cout << "\tallocate 1" << endl;

        globalNewCalledCountIsEnabled = 1;
        BlkDesc desc1 = a.allocate(1);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(desc1.address());
        ASSERT(X == desc1.size());
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(desc1.size() == a.numBytesInUse());
        ASSERT(X == a.numBytesInUse());
        ASSERT(1 == a.numBlocksMax());
        ASSERT(X == a.numBytesMax());
        ASSERT(1 == a.numBlocksTotal());
        ASSERT(X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc1.address() == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(1 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(0 == a.numBlocksProtected());
        ASSERT(!a.isProtected(desc1));

        if (verbose) cout << "\tallocate 20" << endl;
        globalNewCalledCountIsEnabled = 1;
        BlkDesc desc2 = a.allocate(20);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(desc2.address());
        ASSERT(X == desc2.size());
        ASSERT(2 == a.numBlocksInUse());
        ASSERT(2 * X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(2 * X == a.numBytesMax());
        ASSERT(2 == a.numBlocksTotal());
        ASSERT(2 * X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc2.address() == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(2 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(0 == a.numBlocksProtected());
        ASSERT(!a.isProtected(desc1));
        ASSERT(!a.isProtected(desc2));

        if (verbose) cout << "\tprotect20" << endl;
        a.protect(desc2);
        ASSERT(2 == a.numBlocksInUse());
        ASSERT(2 * X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(2 * X == a.numBytesMax());
        ASSERT(2 == a.numBlocksTotal());
        ASSERT(2 * X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc2.address() == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(2 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(1 == a.numBlocksProtected());
        ASSERT(!a.isProtected(desc1));
        ASSERT(a.isProtected(desc2));

        if (verbose) cout << "\tprotect1" << endl;
        a.protect(desc1);
        ASSERT(2 == a.numBlocksInUse());
        ASSERT(2 * X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(2 * X == a.numBytesMax());
        ASSERT(2 == a.numBlocksTotal());
        ASSERT(2 * X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc2.address() == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(2 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(2 == a.numBlocksProtected());
        ASSERT(a.isProtected(desc1));
        ASSERT(a.isProtected(desc2));

        if (verbose) cout << "\tunprotect20" << endl;
        a.unprotect(desc2);
        ASSERT(2 == a.numBlocksInUse());
        ASSERT(2 * X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(2 * X == a.numBytesMax());
        ASSERT(2 == a.numBlocksTotal());
        ASSERT(2 * X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc2.address() == a.lastAllocateAddress());
        ASSERT(0 == a.lastDeallocateNumBytes());
        ASSERT(0 == a.lastDeallocateAddress());
        ASSERT(2 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());
        ASSERT(1 == a.numBlocksProtected());
        ASSERT(a.isProtected(desc1));
        ASSERT(!a.isProtected(desc2));

        if (verbose) cout << "\tdeallocate 20" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(desc2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(2 * X== a.numBytesMax());
        ASSERT(2 == a.numBlocksTotal());
        ASSERT(2 * X == a.numBytesTotal());
        ASSERT(X == a.lastAllocateNumBytes());
        ASSERT(desc2.address() == a.lastAllocateAddress());
        ASSERT(X == a.lastDeallocateNumBytes());
        ASSERT(desc2.address() == a.lastDeallocateAddress());
        ASSERT(2 == a.numAllocations());
        ASSERT(1 == a.numDeallocations());
        ASSERT(1 == a.numBlocksProtected());
        ASSERT(a.isProtected(desc1));

        if (verbose) cout << "\tallocate 3/2 * page size (2 blocks)" << endl;
        globalNewCalledCountIsEnabled = 1;
        BlkDesc desc3 = a.allocate(X + (X/2));
        globalNewCalledCountIsEnabled = 0;
        ASSERT(desc3.address());
        ASSERT(2 * X == desc3.size());
        ASSERT(2 == a.numBlocksInUse());
        ASSERT(3 * X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(3 * X == a.numBytesMax());
        ASSERT(3 == a.numBlocksTotal());
        ASSERT(4 * X == a.numBytesTotal());
        ASSERT(2 * X == a.lastAllocateNumBytes());
        ASSERT(desc3.address() == a.lastAllocateAddress());
        ASSERT(X == a.lastDeallocateNumBytes());
        ASSERT(desc2.address() == a.lastDeallocateAddress());
        ASSERT(3 == a.numAllocations());
        ASSERT(1 == a.numDeallocations());
        ASSERT(1 == a.numBlocksProtected());
        ASSERT(a.isProtected(desc1));
        ASSERT(!a.isProtected(desc3));

        if (verbose) cout << "\tdeallocate 3/2 * PageSize (2 Blocks)" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(desc3);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(3 * X == a.numBytesMax());
        ASSERT(3 == a.numBlocksTotal());
        ASSERT(4 * X == a.numBytesTotal());
        ASSERT(2 * X == a.lastAllocateNumBytes());
        ASSERT(desc3.address() == a.lastAllocateAddress());
        ASSERT(2 * X == a.lastDeallocateNumBytes());
        ASSERT(desc3.address() == a.lastDeallocateAddress());
        ASSERT(3 == a.numAllocations());
        ASSERT(2 == a.numDeallocations());
        ASSERT(1 == a.numBlocksProtected());
        ASSERT(a.isProtected(desc1));

        if (verbose) cout << "\tprotect1" << endl;
        a.unprotect(desc1);
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(X == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(3 * X == a.numBytesMax());
        ASSERT(3 == a.numBlocksTotal());
        ASSERT(4 * X == a.numBytesTotal());
        ASSERT(2 * X == a.lastAllocateNumBytes());
        ASSERT(desc3.address() == a.lastAllocateAddress());
        ASSERT(2 * X == a.lastDeallocateNumBytes());
        ASSERT(desc3.address() == a.lastDeallocateAddress());
        ASSERT(3 == a.numAllocations());
        ASSERT(2 == a.numDeallocations());
        ASSERT(0 == a.numBlocksProtected());
        ASSERT(!a.isProtected(desc1));

        if (verbose) cout << "\tdeallocate 1" << endl;
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(desc1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(2 == a.numBlocksMax());
        ASSERT(3 * X == a.numBytesMax());
        ASSERT(3 == a.numBlocksTotal());
        ASSERT(4 * X == a.numBytesTotal());
        ASSERT(2 * X == a.lastAllocateNumBytes());
        ASSERT(desc3.address() == a.lastAllocateAddress());
        ASSERT(X == a.lastDeallocateNumBytes());
        ASSERT(desc1.address() == a.lastDeallocateAddress());
        ASSERT(3 == a.numAllocations());
        ASSERT(3 == a.numDeallocations());

        if (verbose) cout << "\nMake sure allocate/deallocate invalid "
                          << "size/address is recorded." << endl;
        a.setNoAbort(1);
        a.setQuiet(1);

        if (verbose) cout << "\tallocate 0" << endl;
        BlkDesc nullBlk = a.allocate(0);
        ASSERT(0 == nullBlk.address());
        ASSERT(0 == nullBlk.size());
        ASSERT(0 == a.lastAllocateNumBytes());
        ASSERT(0 == a.lastAllocateAddress());
        ASSERT(X == a.lastDeallocateNumBytes());
        ASSERT(desc1.address() == a.lastDeallocateAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(3 == a.numDeallocations());

        if (verbose) cout << "\tallocate -1" << endl;
        BlkDesc bad1 = a.allocate(-1);
        ASSERT( 0 == bad1.address());
        ASSERT( 0 == bad1.size())
        ASSERT( -1 == a.lastAllocateNumBytes());
        ASSERT( 0 == a.lastAllocateAddress());
        ASSERT( X == a.lastDeallocateNumBytes());
        ASSERT( desc1.address() == a.lastDeallocateAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 3 == a.numDeallocations());

        if (verbose) cout << "\tdeallocate -1" << endl;
        a.deallocate(bad1);
        ASSERT(-1 == a.lastAllocateNumBytes());
        ASSERT( 0 == a.lastAllocateAddress());
        ASSERT( 0 == a.lastDeallocateNumBytes());
        ASSERT( 0 == a.lastDeallocateAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 4 == a.numDeallocations());

        if (verbose) cout << "deallocate -1" << endl;
        a.deallocate(bad1);
        ASSERT(-1 == a.lastAllocateNumBytes());
        ASSERT( 0 == a.lastAllocateAddress());
        ASSERT( 0 == a.lastDeallocateNumBytes());
        ASSERT( 0 == a.lastDeallocateAddress());
        ASSERT( 5 == a.numAllocations());
        ASSERT( 5 == a.numDeallocations());

        if (verbose) cout << "\nEnsure new and delete are not called." << endl;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
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
