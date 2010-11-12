// bdem_aggregateimp.t.cpp                   -*-C++-*-

#include <bdem_aggregateimp.h>

#include <bdem_aggregateoption.h>
#include <bdem_descriptor.h>
#include <bdem_elemtype.h>
#include <bdem_rowdef.h>

#include <bdema_testallocator.h>
#include <bdema_testallocatorexception.h>       // for testing only

#include <bdes_alignment.h>

#include <iostream>

using namespace BloombergLP;
using namespace std;

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

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
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                     GLOBAL CONSTANTS/TYPEDEFS USED FOR TESTING
//-----------------------------------------------------------------------------

typedef char DescriptorType;

const bdem_ElemType::Type DESCRIPTOR_TYPE_ENUM = bdem_ElemType::CHAR;

void testNullConstruct(void*,
                       bdem_AggregateOption::AllocationStrategy,
                       bdema_Allocator*)
{
}

void testCopyConstruct(void*, const void*,
                       bdem_AggregateOption::AllocationStrategy,
                       bdema_Allocator*)
{
}

void testDestroy(void*)
{
}

void testAssign(void*, const void*)
{
}

void testBitwiseMove(void*, void*)
{
}

void testMakeNull(void*)
{
}

int testIsNull(const void*)
{
    return 0;
}

int testAreEqual(const void*, const void*)
{
    return 0;
}

std::ostream& testPrint(const void*, std::ostream& stream, int, int)
{
    return stream;
}

const bdem_Descriptor TEST_DESCRIPTOR = {
    DESCRIPTOR_TYPE_ENUM,
    sizeof(DescriptorType),
    bdes_AlignmentOf<DescriptorType>::VALUE,
    testNullConstruct,
    testCopyConstruct,
    testDestroy,
    testAssign,
    testBitwiseMove,
    testMakeNull,
    testIsNull,
    testAreEqual,
    testPrint
};

const bdem_Descriptor* DESCRIPTOR_LOOKUP_TABLE[] = { &TEST_DESCRIPTOR };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bdema_TestAllocator  testAllocator;
    
    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'reserveMemory'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void reserveMemory(int numBytes);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'reserveMemory'"
                          << "\n=======================" << endl;

        const int SIZE = 4000;

        if (verbose) cout << "\nUsing 'WRITE_ONCE'" << endl;
        {
#if !defined(BDES_PLATFORM__CMP_MSVC)
          BEGIN_BDEMA_EXCEPTION_TEST {
#endif

            bdem_AggregateImp mX(bdem_AggregateOption::WRITE_ONCE,
                                 &testAllocator);

            const int beforeSize = testAllocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(testAllocator.numBytesInUse(), beforeSize,
                         SIZE <= testAllocator.numBytesInUse() - beforeSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
          } END_BDEMA_EXCEPTION_TEST
#endif
        }

        if (verbose) cout << "\nUsing 'WRITE_MANY'" << endl;
        {
#if !defined(BDES_PLATFORM__CMP_MSVC)
          BEGIN_BDEMA_EXCEPTION_TEST {
#endif

            bdem_AggregateImp mX(bdem_AggregateOption::WRITE_MANY,
                                 &testAllocator);

            const int beforeSize = testAllocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(testAllocator.numBytesInUse(), beforeSize,
                         SIZE <= testAllocator.numBytesInUse() - beforeSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
          } END_BDEMA_EXCEPTION_TEST
#endif
        }

        if (verbose) cout << "\nEnd of 'reserveMemory' Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CTORS WITH INITIAL MEMORY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   InitialMemory
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING CTORS WITH INITIAL MEMORY"
                          << "\n=================================" << endl;

        const int SIZE = 4000;  // initial memory

        if (verbose) cout << "\nWith allocMode only." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'WRITE_ONCE'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_AggregateImp mX(bdem_AggregateOption::WRITE_ONCE,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP_ASSERT(testAllocator.numBytesInUse(),
                            SIZE <= testAllocator.numBytesInUse());

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
            if (veryVerbose) cout << "\tUsing 'WRITE_MANY'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_AggregateImp mX(bdem_AggregateOption::WRITE_MANY,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP_ASSERT(testAllocator.numBytesInUse(),
                            SIZE <= testAllocator.numBytesInUse());

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
        }

        if (verbose) cout << "\nWith element types." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'WRITE_ONCE'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_AggregateImp mX(&DESCRIPTOR_TYPE_ENUM,
                                     1,
                                     DESCRIPTOR_LOOKUP_TABLE,
                                     bdem_AggregateOption::WRITE_ONCE,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP_ASSERT(testAllocator.numBytesInUse(),
                            SIZE <= testAllocator.numBytesInUse());

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
            if (veryVerbose) cout << "\tUsing 'WRITE_MANY'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_AggregateImp mX(&DESCRIPTOR_TYPE_ENUM,
                                     1,
                                     DESCRIPTOR_LOOKUP_TABLE,
                                     bdem_AggregateOption::WRITE_MANY,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP_ASSERT(testAllocator.numBytesInUse(),
                            SIZE <= testAllocator.numBytesInUse());

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
        }

        if (verbose) cout << "\nWith row def." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'WRITE_ONCE'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_RowDef rowDef(bdem_AggregateOption::PASS_THROUGH,
                                   &testAllocator);

                const int startSize = testAllocator.numBytesInUse();

                bdem_AggregateImp mX(rowDef,
                                     bdem_AggregateOption::WRITE_ONCE,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP2_ASSERT(testAllocator.numBytesInUse(), startSize,
                            SIZE <= testAllocator.numBytesInUse() - startSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
            if (veryVerbose) cout << "\tUsing 'WRITE_MANY'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_RowDef rowDef(bdem_AggregateOption::PASS_THROUGH,
                                   &testAllocator);

                const int startSize = testAllocator.numBytesInUse();

                bdem_AggregateImp mX(rowDef,
                                     bdem_AggregateOption::WRITE_MANY,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP2_ASSERT(testAllocator.numBytesInUse(), startSize,
                            SIZE <= testAllocator.numBytesInUse() - startSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
        }

        if (verbose) cout << "\nWith row header." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'WRITE_ONCE'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_RowDef rowDef(bdem_AggregateOption::PASS_THROUGH,
                                   &testAllocator);

                char memory[128];
                bdem_RowHeader *rowHeader = rowDef.constructRow(memory);

                const int startSize = testAllocator.numBytesInUse();

                bdem_AggregateImp mX(*rowHeader,
                                     bdem_AggregateOption::WRITE_ONCE,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP2_ASSERT(testAllocator.numBytesInUse(), startSize,
                            SIZE <= testAllocator.numBytesInUse() - startSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
            if (veryVerbose) cout << "\tUsing 'WRITE_MANY'." << endl;
            {

#if !defined(BDES_PLATFORM__CMP_MSVC)
              BEGIN_BDEMA_EXCEPTION_TEST {
#endif

                bdem_RowDef rowDef(bdem_AggregateOption::PASS_THROUGH,
                                   &testAllocator);

                char memory[128];
                bdem_RowHeader *rowHeader = rowDef.constructRow(memory);

                const int startSize = testAllocator.numBytesInUse();

                bdem_AggregateImp mX(*rowHeader,
                                     bdem_AggregateOption::WRITE_MANY,
                                     bdem_AggregateImp::InitialMemory(SIZE),
                                     &testAllocator);

                LOOP2_ASSERT(testAllocator.numBytesInUse(), startSize,
                            SIZE <= testAllocator.numBytesInUse() - startSize);

#if !defined(BDES_PLATFORM__CMP_MSVC)
              } END_BDEMA_EXCEPTION_TEST
#endif
            }
        }

        if (verbose) cout << "\nEnd of 'InitialMemory' Test." << endl;
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
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
