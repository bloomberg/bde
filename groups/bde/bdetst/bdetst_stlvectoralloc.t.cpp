// bdetst_stlvectoralloc.t.cpp                  -*-C++-*-

#include <bdetst_stlvectoralloc.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static inline int min(int a, int b) { return a < b ? a : b; }
    // Return the minimum of the specified 'a' and 'b' arguments.

static int parseAbsVerboseArg(int *verboseFlag,
			      int verboseArgNum,
			      char *argv[],
			      unsigned char *switches = 0)
    // If '*verboseFlag' (at the specified 'verboseFlag' address) is 0, return
    // 0 immediately with no other effect.  Otherwise use 'atoi' to obtain the
    // integer value 'V' of the argument at the specified 'verboseArgNum'
    // position in the specified 'argv' array, set '*verboseFlag' to 0 if 'V'
    // is negative, and return the absolute value of 'V'.  If the optionally
    // specified 'switches' is non-zero, the character code for each characters
    // in 'inputString' that is not part of a leading C++-style integer value
    // will be used to increment the corresponding element in 'switches'. 
    // Additionally, making 'verboseArgNumber' negative suppresses modification
    // of the '*verboseFlag' and returns the argument as a signed integer
    // value.  The behavior is undefined unless *verboseFlag correctly
    // indicates the existence of the 'verboseArgNum' entry >= 2 in 'argv', and
    // 'switches' (if non-zero) is large enough to accommodate all relevant
    // character codes in 'argv[veroseArgNumber]'.
{
    ASSERT(verboseFlag);

    if (!*verboseFlag) return 0;

    int isAbsFlag = 1;
    if (verboseArgNum < 0) { // Allow test designer to return signed arg value.
	isAbsFlag = 0;
        verboseArgNum = -verboseArgNum;
    }
   
    ASSERT(2 <= verboseArgNum);	 // Don't mess with program or test case #.
    ASSERT(argv);
    const char *const input = argv[verboseArgNum];
    ASSERT(input);

    if (switches) {
        const char *in = input;

        // Skip optional leading sign provided second character is a digit.
        in += ('-' == *in || '+' == *in) && '0' <= in[1] && in[1] <= '9';

        // Skip leading decimal digits.
        while ('0' <= *in && *in <= '9') ++in;

	// Modify switches based on trailing characters of 'in'.
        for (; *in; ++in) {
	    ++switches[(unsigned char) *in];
	}
    }
 
    // Parse leading integer and return either signed or unsigned.
    int verboseArgValue = atoi(input);
    if (isAbsFlag && verboseArgValue < 0) { 			     
        *verboseFlag = 0;			      // Suppress this level.
        ASSERT(-verboseArgValue != verboseArgValue);  // Check for INT_MIN!
        return -verboseArgValue; // absolute value of leading integer
    }
    return verboseArgValue; // (possibly negative) leading integer argument
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        //   Allocator is not copied during copy construction.
        //   If allocator is not specified, copy-constructed container gets
        //       default allocator.
        //   If allocator is specified, copy-constructed container gets
        //       specified allocator.
        //
        // Plan:
        //   Construct 'vec1' using a 'testAlloc1' and add data.
        //   Construct 'vec2' from 'vec1', not specifying an allocator.
        //   Verify that contents wer copied.
        //   Verify that 'vec2' uses the default allocator.
        //   Verify that the copy operation did not change 'testAlloc1'.
        //   Construct 'vec3' from 'vec1' using 'testAlloc3'
        //   Verify that contents were copied.
        //   Verify that 'vec3' uses 'testAlloc3'
        //   Verify that the copy operation did not change 'testAlloc1'.
        
        bdema_TestAllocator testAlloc1, testAlloc3;
                        ASSERT(0 == testAlloc1.numBytesInUse());
                        ASSERT(0 == testAlloc3.numBytesInUse());
        
        vector<int> vec1(&testAlloc1);
        for (int i = 0; i < 1000; ++i) {
            vec1.push_back(i);
        }

        int numAlloc1 = testAlloc1.numAllocation();
        int numDealloc1 = testAlloc1.numDeallocation();
        int inUse1 = testAlloc1.numBytesInUse();

        vector<int> vec2(vec1);
                        ASSERT(1000 == vec2.size());
                        for (int i = 0; i < 1000; ++i) ASSERT(i == vec2[i]);
                        ASSERT(bdema_Default::defaultAllocator() ==
                               vec2.get_allocator());
                        ASSERT(numAlloc1 == testAlloc1.numAllocation());
                        ASSERT(numDealloc1 == testAlloc1.numDeallocation());
                        ASSERT(inUse1 == testAlloc1.numBytesInUse());

        vector<int> vec3(vec1, &testAlloc3);
                        ASSERT(1000 == vec3.size());
                        for (int i = 0; i < 1000; ++i) ASSERT(i == vec3[i]);
                        ASSERT(&testAlloc3 == vec3.get_allocator());
                        ASSERT(0 != testAlloc3.numBytesInUse());
                        ASSERT(numAlloc1 == testAlloc1.numAllocation());
                        ASSERT(numDealloc1 == testAlloc1.numDeallocation());
                        ASSERT(inUse1 == testAlloc1.numBytesInUse());
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // SWAP
        //
        // Concerns:
        //   Swapping containers doesn't swap allocators.
        //   Swapping containers with same allocator results in no allocation
        //      or deallocation operations.
        //   Swapping containers with different allocators does result in
        //      allocation and deallocation operations.
        //
        // Plan:
        //   Construct 'vec1' and 'vec2' with different test allocators.
        //   Add data to 'vec1'.  Remember allocation statistics.
        //   Swap 'vec1' and 'vec2'
        //   Verify that contents were swapped.
        //   Verify that allocators for each are unchanged.
        //   Verify that allocation statistics changed for each test allocator.
        //   Create a 'vec3' with same allocator as 'vec2'.
        //   Swap 'vec2' and 'vec3'
        //   Verify that contents were swapped.
        //   Verify that allocation statistics did not change.
        //   Let 'vec3' got out of scope.
        //   Verify that memory was returned to allocator.
        bdema_TestAllocator testAlloc2;
                        ASSERT(0 == testAlloc2.numBytesInUse());

        vector<int> vec1;
        vector<int> vec2(&testAlloc2);

        for (int i = 0; i < 1000; ++i) {
            vec1.push_back(i);
        }

        vec1.swap(vec2);
                        ASSERT(0 == vec1.size());
                        ASSERT(1000 == vec2.size());
                        for (int i = 0; i < 1000; ++i) ASSERT(i == vec2[i]);
                        ASSERT(bdema_Default::defaultAllocator() ==
                               vec1.get_allocator());
                        ASSERT(&testAlloc2 == vec2.get_allocator());
                        ASSERT(0 != testAlloc2.numBytesInUse());

        int numAlloc2 = testAlloc2.numAllocation();
        int numDealloc2 = testAlloc2.numDeallocation();
        int inUse2 = testAlloc2.numBytesInUse();

        {
            vector<int> vec3(&testAlloc2);
                        ASSERT(testAlloc2.numBytesInUse() == inUse2);
            vec3.swap(vec2);
                        ASSERT(vec2.empty());
                        ASSERT(1000 == vec3.size());
                        for (int i = 0; i < 1000; ++i) ASSERT(i == vec3[i]);
                        ASSERT(numAlloc2 == testAlloc2.numAllocation());
                        ASSERT(numDealloc2 == testAlloc2.numDeallocation());
                        ASSERT(inUse2 == testAlloc2.numBytesInUse());
        }            
                        // Destructor for vec3 should have freed memory
                        ASSERT(0 == testAlloc2.numBytesInUse());

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // EQUALITY AND ASSIGNMENT
        //
        // Concerns:
        //   Two vectors with different allocators can still compare equal
        //   Assignment does not transfer the allocator.
        //
        // Plan:
        //   Construct 'vec1' with default allocator and fill with 0..999
        //   Construct 'vec2' with test allocator and fill with 0..999
        //   Verify that 'vec1' compares equal to 'vec2'
        //   Clear 'vec2'.
        //   Verify that 'vec1' does not compare equal to 'vec2'
        //   Assign 'vec2' the value of 'vec1'
        //   Verify that 'vec2' allocator hasn't changed.
        //   Verify that no additional memory was used from test allocator.
        //   Verify that 'vec1' compares equal to 'vec2' again.
        //   Add elements to 'vec2' until it grows.
        //   Verify that additional memory was used from test allocator.

        if (verbose) printf("\nASSIGNMENT AND EQUALITY TEST"
                            "\n============================");

        bdema_TestAllocator theTestAlloc;
                        ASSERT(0 == theTestAlloc.numBytesInUse());

        vector<int> vec1;
                        ASSERT(bdema_Default::defaultAllocator() ==
                               vec1.get_allocator());
        for (int i = 0; i < 1000; ++i) {
            vec1.push_back(i);
        }

                        ASSERT(0 == theTestAlloc.numBytesInUse());

        vector<int> vec2(&theTestAlloc);
        for (int i = 0; i < 1000; ++i) {
            vec2.push_back(i);
        }

        int bytesInUse = theTestAlloc.numBytesInUse();
                        ASSERT(0 != bytesInUse);
                        ASSERT(vec1 == vec2);

        vec2.clear();   ASSERT(theTestAlloc.numBytesInUse() == bytesInUse);
                        ASSERT(vec1 != vec2);
        vec2 = vec1;    ASSERT(vec1 == vec2);
                        ASSERT(bdema_Default::defaultAllocator() ==
                               vec1.get_allocator());
                        ASSERT(&theTestAlloc == vec2.get_allocator());
                        ASSERT(theTestAlloc.numBytesInUse() == bytesInUse);

        unsigned v2capacity = vec2.capacity();
        while (vec2.capacity() == v2capacity) {
            vec2.push_back(vec2.size());
        }
                        ASSERT(theTestAlloc.numBytesInUse() > bytesInUse);
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   Constructing a vector with a non-default allocator causes the
        //   vector to use that allocator.
        //
        // Plan:
        //   Construct a vector using a test allocator.
        //   Add elements to that vector.
        //   Verify that memory was allocated from test alloc.
        //   Let vector go out of scope.
        //   Verify that memory was released back to test alloc.
	//
        // Testing:
        //   Constructor
        //   Destructor
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        bdema_TestAllocator theTestAlloc;
                        ASSERT(0 == theTestAlloc.numBytesInUse());

        {
            vector<int> theVec(&theTestAlloc);
            for (int i = 0; i < 1000; ++i) {
                theVec.push_back(i);
            }

            int bytesInUse = theTestAlloc.numBytesInUse();
                        ASSERT(0 != bytesInUse);

            // Calling 'clear' does not free any memory.
            theVec.clear(); ASSERT(theTestAlloc.numBytesInUse() == bytesInUse);
        }

        // theVec is now out of scope, memory should have been freed.
                        ASSERT(0 == theTestAlloc.numBytesInUse());
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
