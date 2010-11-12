// bdema_bufferedsequentialallocator.t.cpp                            -*-C++-*-
#include <bdema_bufferedsequentialallocator.h>

#include <bdema_bufferedsequentialpool.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>

#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdema_BufferedSequentialAllocator' adopts the
// 'bdema_BufferedSequentialPool' mechanism to a 'bslma_ManagedAllocator'
// protocol.  The primary concern is that the allocator correctly proxies the
// memory allocation requests to the buffered sequential pool it adopts.
//
// To test requests are correctly proxied, we can create a buffered sequential
// allocator and a buffered sequential pool using the same external buffer and
// two different test allocators.  We then verify that either:
//
// 1) The address returned by the allocator and the pool is the same (i.e., the
//    memory is allocated from the buffer).
//
// 2) If the memory used by either test allocator is non-zero, the number of
//    bytes used by both test allocators is the same.
//
// We also need to verify that 'deallocate' method has no effect.  Again, we
// make use of the test allocator to ensure no memory is deallocated when the
// 'deallocate' method of a buffered sequential allocator is invoked on
// previously allocated memory (both from the buffer and from the fall back
// allocator).
//
// Finally, the destructor of 'bdema_BufferedSequentialAllocator' is tested
// throughout the test driver.  At destruction, the allocator should reclaim
// all outstanding memory allocation.  By setting the global allocator, default
// allocator and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// allocator.
//-----------------------------------------------------------------------------
// // CREATORS
// [2] bdema_BufferedSequentialAllocator(char *b, int s,
//                                              bslma_Allocator *a = 0)
// [2] bdema_BufferedSequentialAllocator(char *b, int s,
//                        AlignmentStrategy st, bslma_Allocator *a = 0)
// [5] ~bdema_BufferedSequentialAllocator()
//
// // MANIPULATORS
// [2] void *allocate(int size)
// [3] void deallocate(void *address)
// [4] void release()
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE TEST
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------
typedef bdema_BufferedSequentialAllocator Obj;

// On windows, when a 8-byte aligned object is created on the stack, it
// actually get aligned on 4-byte boundary.  To fix this, create a static
// buffer instead.
enum { BUFFER_SIZE = 256 };
static bsls_AlignedBuffer<BUFFER_SIZE> bufferStorage;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1: Precise calculation
///- - - - - - - - - - - - - - -
// Suppose we need to implement a method, 'calculate' that performs
// calculations (where the specifics are not important to illustrate the use of
// this component) which requires three vectors of doubles.  Furthermore,
// suppose we know that we need at most 100 doubles for each vector:
//..
    double calculate(const bsl::vector<double>& data)
    {
//..
// Since the amount of memory needed is known in advance, we can optimize the
// memory allocation by using a 'bdema_BufferedSequentialAllocator' to supply
// memory for the vectors.  We can also prevent the vectors from resizing
// (which triggers more allocations) by reserving for the specific capacity we
// need:
//..
        enum { SIZE = 3 * 100 * sizeof(double) };
//..
// In the above calculation, we assumed that the only memory allocation request
// by the vector is the allocation for the array that stores the 'double'
// values.  Furthermore, we assumed that the 'reserve' method allocates the
// exact amount of memory for the number of items (in this case, 'double')
// specified.  Note that both of these assumptions are true in BDE's
// implementation of 'vector'.
//
// To avoid alignment issues described in the "warning" section, we can create
// a 'bsls_AlignedBuffer':
//..
        bsls_AlignedBuffer<SIZE> bufferStorage;

        bdema_BufferedSequentialAllocator alloc(bufferStorage.buffer(), SIZE);

        bsl::vector<double> v1(&alloc);     v1.reserve(100);
        bsl::vector<double> v2(&alloc);     v2.reserve(100);
        bsl::vector<double> v3(&alloc);     v3.reserve(100);

        // ...

        return 0.0;
    }
//..
// By making use of a 'bdema_BufferedSequentialAllocator', all dynamic memory
// allocation is eliminated.
//
///Example 2: Using 'bdema_BufferedSequentialAllocator' with fallback
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are receiving updates for price quotes for a list of securities
// through the following function 'receivePriceQuotes':
//..
    void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap);
        // Load into the specified 'updateMap' updates for price quotes for a
        // list of securities.
//..
// Furthermore, suppose the number of securities we are interested in are
// limited.  We can then use a 'bdema_BufferedSequentialAllocator' to optimize
// memory allocation for the 'bsl::map'.  We first create a buffer on the
// stack:
//..
//  enum {
//      NUM_SECURITIES = 100,
//
//      TREE_NODE_SIZE = sizeof(bsl::map<bsl::string, double>::value_type)
//                       + sizeof(void *) * 4,
//
//      AVERAGE_SECURITY_LENGTH = 5,
//
//      TOTAL_SIZE = NUM_SECURITIES *
//                   (TREE_NODE_SIZE + AVERAGE_SECURITY_LENGTH )
//
//  };
//
//  char buffer[TOTAL_SIZE];
//..
// The calculation of the amount of memory needed is just an estimate, as we
// used the average security size instead of the maximum security size.  A
// 'bsl::map's node contains a pointer to its parent, left node, right node and
// also a color flag (for red-black-tree implementation).
//..
//  bdema_BufferedSequentialAllocator bsa(buffer, TOTAL_SIZE);
//  bsl::map<bsl::string, double> updateMap(&bsa);
//
//  receivePriceQuotes(&updateMap);
//..
// With the use of a 'bdema_BufferedSequentialAllocator', we can be reasonably
// assured that the memory allocation performance is optimized (i.e., minimal
// use of dynamic allocation).

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------
void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap)
{
    enum { NUM_SECURITIES = 100 };

    for (int i = 0; i < NUM_SECURITIES; ++i) {
        char buffer[256];
        int n = bsl::sprintf(buffer, "sec%d", i);
        bsl::string security(buffer, n);
        updateMap->insert(bsl::make_pair(security, 1.0));
    }
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

    // Object Test Allocator.
    bslma_TestAllocator objectAllocator("Object Allocator",
                                        veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma_TestAllocator defaultAllocator("Default Allocator",
                                         veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma_TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma_Allocator *originalGlobalAllocator =
                           bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Copy usage example from the header file, uncomment the code and
        //   change all 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        if (verbose) cout << "\nTesting usage example 1." << endl;
        {
            bsl::vector<double> v;
            calculate(v);

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBytesInUse());
        }


        if (verbose) cout << "\nTesting usage example 2." << endl;
        {

            enum {
                NUM_SECURITIES = 100,

                TREE_NODE_SIZE = sizeof(bsl::map<bsl::string,
                                                 double>::value_type)
                                 + sizeof(void *) * 4,

                AVERAGE_SECURITY_LENGTH = 5,

                TOTAL_SIZE = NUM_SECURITIES *
                             (TREE_NODE_SIZE + AVERAGE_SECURITY_LENGTH )

            };

            bsls_AlignedBuffer<TOTAL_SIZE> bufferStorage;
            char *buffer = bufferStorage.buffer();

           bdema_BufferedSequentialAllocator bsa(buffer, TOTAL_SIZE,
                                                             &objectAllocator);

            bsl::map<bsl::string, double> updateMap(&bsa);
            receivePriceQuotes(&updateMap);

            if (verbose) P(objectAllocator.numBytesInUse());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // DTOR TEST
        //
        // Concerns:
        //   1) That the previously managed buffer is not changed in any way
        //      after destruction of the allocator.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction are deallocated after destruction of the
        //      buffered sequential allocator.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential allocator using the buffer and
        //   the same test allocator.  Finally, destroy the pool, and verify
        //   that the bytes in the first buffer remains '0xA' and the buffer
        //   is not deallocated.
        //
        //   For concern 2, construct a buffered sequential allocator using a
        //   'bslma_TestAllocator', allocate sufficient memory such that the
        //   buffer runs out and the allocator is used.  Finally, destroy
        //   the buffered sequential allocator, and verify, using the test
        //   allocator, that there is no outstanding memory allocated.
        //
        // Testing:
        //   ~bdema_BufferedSequentialPool()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR TEST" << endl
                                  << "=========" << endl;

        enum { BUFFER_SIZE = 256 };

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "destruction." << endl;
        {
            char bufferRef[BUFFER_SIZE];
            int total = 0;

            ASSERT(0 == objectAllocator.numBlocksInUse());
            char *buffer = (char *)objectAllocator.allocate(BUFFER_SIZE);
            total = objectAllocator.numBlocksInUse();

            {
                memset(buffer,    0xA, BUFFER_SIZE);
                memset(bufferRef, 0xA, BUFFER_SIZE);

                Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

                ASSERT(total == objectAllocator.numBlocksInUse());

                // Allocate some memory.
                bsa.allocate(1);
                bsa.allocate(16);
                ASSERT(total == objectAllocator.numBlocksInUse());
            }
            ASSERT(total == objectAllocator.numBlocksInUse());
            ASSERT(0 == memcmp(buffer, bufferRef, BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting allocated memory are deallocated after"
                             " destruction." << endl;
        {
            char buffer[BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            bsa.allocate(BUFFER_SIZE + 1);
            bsa.allocate(1);
            bsa.allocate(16);
            ASSERT(0 != objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That the previously managed buffer is not changed in any way
        //      after 'release'.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction are deallocated after 'release'.
        //
        //   3) That subsequent allocation requests after invocation of the
        //      'release' method are satisfied by the buffer supplied at
        //      construction.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential allocator using the buffer and
        //   the same test allocator.  Finally, invoke 'release', and verify
        //   that the bytes in the first buffer remains '0xA' and the buffer is
        //   not deallocated.
        //
        //   For concern 2 and 3, construct a buffered sequential allocator
        //   using a 'bslma_TestAllocator', allocate sufficient memory such
        //   that the buffer runs out and the allocator is used.  Finally,
        //   invoke 'release' and verify, using the test allocator, that there
        //   is no outstanding memory allocated.  Then, allocate memory again
        //   and verify memory comes from the buffer.
        //
        // Testing:
        //   void release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        enum { BUFFER_SIZE = 256 };

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "'release'." << endl;

        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            char *buffer = (char *)objectAllocator.allocate(BUFFER_SIZE);
            char bufferRef[BUFFER_SIZE];

            int total = objectAllocator.numBlocksInUse();

            memset(buffer,    0xA, BUFFER_SIZE);
            memset(bufferRef, 0xA, BUFFER_SIZE);

            Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(total == objectAllocator.numBlocksInUse());

            // Allocate some memory.
            bsa.allocate(1);
            bsa.allocate(16);
            ASSERT(total == objectAllocator.numBlocksInUse());

            // Release all memory.
            bsa.release();
            ASSERT(total == objectAllocator.numBlocksInUse());

            ASSERT(0 == memcmp(buffer, bufferRef, BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

        if (verbose) cout << "\nTesting allocated memory are deallocated after"
                             " 'release'." << endl;
        {
            char buffer[BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            bsa.allocate(BUFFER_SIZE + 1);
            bsa.allocate(1);
            bsa.allocate(16);

            ASSERT(0 != objectAllocator.numBlocksInUse());

            // Release all memory.
            bsa.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting subsequent allocations comes"
                                 " first from the initial buffer." << endl;
            void *addr = bsa.allocate(16);

            ASSERT(&buffer[0] <= addr);
            ASSERT(&buffer[0] + BUFFER_SIZE > addr);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'deallocate' TEST
        //
        // Concerns:
        //   That 'deallocate' has no effect.
        //
        // Plan:
        //   Create a buffered sequential allocator initialized with a test
        //   allocator.  Request memory of varying sizes and then deallocate
        //   each memory.  Verify that the number of bytes in use indicated by
        //   the test allocator does not decrease after each 'deallocate'
        //   method invocation.
        //
        // Testing:
        //   void deallocate(void *address)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deallocate' TEST" << endl
                                  << "=================" << endl;

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

        int lastNumBytesInUse = objectAllocator.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = bsa.allocate(SIZE);
            const int numBytesInUse = objectAllocator.numBytesInUse();
            bsa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == objectAllocator.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <=
                                              objectAllocator.numBytesInUse());
            lastNumBytesInUse = objectAllocator.numBytesInUse();
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR / ALLOCATE TEST
        //
        // Concerns:
        //   1) That the 'bdema_BufferedSequentialAllocator' correctly proxies
        //      its constructor argument and allocation request to the
        //      'bdema_BufferedSequentialPool' it adopts.
        //
        //   2) That allocating 0 bytes should return 0.
        //
        // Plan:
        //   Using the array driven approach, create an array of various memory
        //   allocation request sizes.  Then, create both a buffered sequential
        //   allocator and buffered sequential pool using the same buffer and
        //   two different test allocators.  Finally, verify that:
        //
        //   1) The address returned by the allocator and the pool is the same
        //      (i.e., the memory is allocated from the buffer).
        //
        //   2) If the memory used by either test allocator is non-zero, the
        //      number of bytes used by both test allocators is the same.
        //
        // Testing:
        //   bdema_BufferedSequentialAllocator(char *b, int s,
        //                                              bslma_Allocator *a = 0)
        //   bdema_BufferedSequentialAllocator(char *b, int s,
        //                        AlignmentStrategy st, bslma_Allocator *a = 0)
        //   void *allocate(int size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ALLOCATE TEST" << endl
                                  << "====================" << endl;

        char *buffer = bufferStorage.buffer();

        bsls_Alignment::Strategy MAX = bsls_Alignment::BSLS_MAXIMUM;
        bsls_Alignment::Strategy NAT = bsls_Alignment::BSLS_NATURAL;

        const int DATA[] = { 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator taA(veryVeryVeryVerbose);
        bslma_TestAllocator taB(veryVeryVeryVerbose);
        bslma_TestAllocator taC(veryVeryVeryVerbose);
        bslma_TestAllocator taX(veryVeryVeryVerbose);
        bslma_TestAllocator taY(veryVeryVeryVerbose);
        bslma_TestAllocator taZ(veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting single allocation." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            // allocators
            Obj mX(buffer, BUFFER_SIZE, &taX);
            Obj mY(buffer, BUFFER_SIZE, MAX, &taY);
            Obj mZ(buffer, BUFFER_SIZE, NAT, &taZ);

            // pools
            bdema_BufferedSequentialPool mA(buffer, BUFFER_SIZE, &taA);
            bdema_BufferedSequentialPool mB(buffer, BUFFER_SIZE, MAX, &taB);
            bdema_BufferedSequentialPool mC(buffer, BUFFER_SIZE, NAT, &taC);

            void *addrA = mA.allocate(DATA[i]);
            void *addrX = mX.allocate(DATA[i]);

            void *addrB = mB.allocate(DATA[i]);
            void *addrY = mY.allocate(DATA[i]);

            void *addrC = mC.allocate(DATA[i]);
            void *addrZ = mZ.allocate(DATA[i]);

            // Allocating from the test allocator.
            if (taA.numBytesInUse()) {
                ASSERT(taA.numBytesInUse() == taX.numBytesInUse());
            }
            else {
                ASSERT(addrA == addrX);
            }

            if (taB.numBytesInUse()) {
                ASSERT(taB.numBytesInUse() == taY.numBytesInUse());
            }
            else {
                ASSERT(addrB == addrY);
            }

            if (taC.numBytesInUse()) {
                ASSERT(taC.numBytesInUse() == taZ.numBytesInUse());
            }
            else {
                ASSERT(addrC == addrZ);
            }
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taB.numBytesInUse());
        ASSERT(0 == taC.numBytesInUse());
        ASSERT(0 == taX.numBytesInUse());
        ASSERT(0 == taY.numBytesInUse());
        ASSERT(0 == taZ.numBytesInUse());

        if (verbose) cout << "\nTesting connsecutive allocations." << endl;
        {
            // allocators
            Obj mX(buffer, BUFFER_SIZE, &taX);
            Obj mY(buffer, BUFFER_SIZE, MAX, &taY);
            Obj mZ(buffer, BUFFER_SIZE, NAT, &taZ);

            // pools
            bdema_BufferedSequentialPool mA(buffer, BUFFER_SIZE, &taA);
            bdema_BufferedSequentialPool mB(buffer, BUFFER_SIZE, MAX, &taB);
            bdema_BufferedSequentialPool mC(buffer, BUFFER_SIZE, NAT, &taC);

            for (int i = 0; i < NUM_DATA; ++i) {
                void *addrA = mA.allocate(DATA[i]);
                void *addrX = mX.allocate(DATA[i]);

                void *addrB = mB.allocate(DATA[i]);
                void *addrY = mY.allocate(DATA[i]);

                void *addrC = mC.allocate(DATA[i]);
                void *addrZ = mZ.allocate(DATA[i]);

                // Allocating from the test allocator.
                if (taA.numBytesInUse()) {
                    ASSERT(taA.numBytesInUse() == taX.numBytesInUse());
                }
                else {
                    ASSERT(addrA == addrX);
                }

                if (taB.numBytesInUse()) {
                    ASSERT(taB.numBytesInUse() == taY.numBytesInUse());
                }
                else {
                    ASSERT(addrB == addrY);
                }

                if (taC.numBytesInUse()) {
                    ASSERT(taC.numBytesInUse() == taZ.numBytesInUse());
                }
                else {
                    ASSERT(addrC == addrZ);
                }
            }
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taB.numBytesInUse());
        ASSERT(0 == taC.numBytesInUse());
        ASSERT(0 == taX.numBytesInUse());
        ASSERT(0 == taY.numBytesInUse());
        ASSERT(0 == taZ.numBytesInUse());

        if (verbose) cout << "\nTesting allocation size of 0 bytes." << endl;
        {
            Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);
            void *addr = bsa.allocate(0);
            ASSERT(0 == addr);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdema_BufferedSequentialAllocator' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and alignment strategy.
        //
        //   3) That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the remaining free space in the
        //      external buffer.
        //
        //   5) Destruction of the allocator releases all managed memory,
        //      including memory that comes from dynamic allocation.
        //
        // Plan:
        //   For concerns 1, 2, and 3, first, create a buffer on the stack,
        //   then initialize a 'bdema_BufferedSequentialAllocator' with the
        //   buffer.  Next, allocate a block of memory from the allocator and
        //   verify that it comes from the external buffer.  Then, allocate
        //   another block of memory from the allocator, and verify that the
        //   first allocation returned a block of memory of sufficient size by
        //   checking that 'addr2 > addr1 + allocsize1'.  Also verify that the
        //   alignment strategy specified at construction is followed by
        //   checking the address of the second allocation.
        //
        //   For concern 4, initialize a 'bdema_BufferedSequentialAllocator'
        //   with a 'bslma_TestAllocator'.  Then allocate a block of memory
        //   that is larger than the buffer supplied at construction of the
        //   buffered sequential allocator.  Verify that memory is allocated
        //   from the test allocator.
        //
        //   For concern 5, let the allocator created with the test allocator
        //   go out of scope, and verify, through the test allocator, that all
        //   allocated memory are deallocated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATING TEST" << endl
                                  << "=============" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8 };
        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;
        Obj bsa(buffer, BUFFER_SIZE, &objectAllocator);

        if (verbose) cout << "\nTesting allocate from buffer." << endl;
        void *addr1 = bsa.allocate(ALLOC_SIZE1);

        // Allocation starts at the beginning of the aligned buffer.
        ASSERT(&buffer[0] == addr1);

        // Allocation comes from within the buffer.
        LOOP2_ASSERT((void *)&buffer[BUFFER_SIZE - 1],
                     addr1,
                     &buffer[BUFFER_SIZE - 1] >= addr1);

        void *addr2 = bsa.allocate(ALLOC_SIZE2);

        // Allocation comes from within the buffer.
        ASSERT(&buffer[0]               <  addr2);
        ASSERT(&buffer[BUFFER_SIZE - 1] >= addr2);

        // Allocation respects alignment strategy.
        LOOP2_ASSERT((void *)&buffer[8],
                     addr2,
                     &buffer[8] == addr2);

        // Make sure no memory comes from the object, default and global
        // allocators.
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting allocate when buffer runs out."
                          << endl;
        {
            Obj pool2(buffer, BUFFER_SIZE, &objectAllocator);
            addr1 = pool2.allocate(BUFFER_SIZE + 1);

            // Allocation request is satisfied even when larger than the
            // supplied buffer.
            LOOP_ASSERT(addr1, 0 != addr1);

            // Allocation comes from the objectAllocator.
            ASSERT(0 != objectAllocator.numBlocksInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }
        // All dynamically allocated memory are released after the allocator's
        // destruction.
        ASSERT(0 == objectAllocator.numBlocksInUse());

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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
