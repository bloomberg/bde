// ball_countingallocator.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_countingallocator.h>

#include <bslma_testallocator.h>

#include <bsls_alignmentutil.h>

#include <bsl_c_stdlib.h>     // atoi()

#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// [ 1] ball::CountingAllocator(bslma::Allocator *ba = 0);
// [ 1] ~ball::CountingAllocator();
// [ 1] void *allocate(int size);
// [ 1] void deallocate(void *address);
// [ 1] void resetNumBytesTotal();
// [ 1] int numBytesTotal() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int effectiveSize(int size)
{
    return bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
           + bsls::AlignmentUtil::roundUpToMaximalAlignment(size);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    //    int veryVerbose = argc > 3; // not used
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.

      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        bslma::Allocator *allocator = 0;
        ball::CountingAllocator countingAllocator(allocator);

        bsl::vector<int> vec(&countingAllocator);

        vec.push_back(1);
        if (verbose) {
            bsl::cout << "dynamic memory after first push back: "
                      << countingAllocator.numBytesTotal() << bsl::endl;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation.
        //
        // Plan:
        //   Create a counting allocator, invoke 'allocate' once, verify
        //   the accessor, invoke the 'deallocate' and verify the
        //   accessor.
        //
        //   Create a counting allocator, invoke 'allocate' twice,
        //   verifying the accessor after each allocation, invoke the
        //   'deallocate' twice (in the order of allocations) verifying
        //   the accessor after each deallocation.
        //
        //   Create a counting allocator, invoke 'allocate' twice,
        //   verifying the accessor after each allocation, invoke the
        //   'deallocate' twice (in the reverse order of allocations)
        //   verifying the accessor after each deallocation.
        //
        //   Create a counting allocator, invoke 'allocate' twice,
        //   verifying the accessor after each allocation, invoke
        //   'resetNumBytesTotal' and verify the accessor.
        //
        // Testing:
        //   ball::CountingAllocator(bslma::Allocator *ba = 0);
        //   ~ball::CountingAllocator();
        //   void *allocate(int size);
        //   void deallocate(void *address);
        //   void resetNumBytesTotal();
        //   int numBytesTotal() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        int SZ1 = 3;
        int SZ2 = 8;
        int EFF_SZ1 = effectiveSize(SZ1);
        int EFF_SZ2 = effectiveSize(SZ2);
        if (verbose) {
            P(SZ1);
            P(SZ2);
            P(EFF_SZ1);
            P(EFF_SZ2);
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        ball::CountingAllocator ca(&ta);
        void *p1, *p2;

        // One allocation than deallocation
        // --------------------------------
        p1 = ca.allocate(SZ1);
        ASSERT(0 != p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ1);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ1);

        ca.deallocate(p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == 0);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == 0);

        // two allocations than deallocations in the same order
        // ----------------------------------------------------
        p1 = ca.allocate(SZ1);
        ASSERT(0 != p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ1);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ1);

        p2 = ca.allocate(SZ2);
        ASSERT(0 != p2);
        LOOP_ASSERT(ca.numBytesTotal(),
                    ca.numBytesTotal() == EFF_SZ1 + EFF_SZ2);
        LOOP_ASSERT(ta.numBytesInUse(),
                    ta.numBytesInUse() == EFF_SZ1 + EFF_SZ2);

        ca.deallocate(p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ2);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ2);

        ca.deallocate(p2);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == 0);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == 0);

        // two allocations than deallocations in the reverse order
        // -------------------------------------------------------
        p1 = ca.allocate(SZ1);
        ASSERT(0 != p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ1);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ1);

        p2 = ca.allocate(SZ2);
        ASSERT(0 != p2);
        LOOP_ASSERT(ca.numBytesTotal(),
                    ca.numBytesTotal() == EFF_SZ1 + EFF_SZ2);
        LOOP_ASSERT(ta.numBytesInUse(),
                    ta.numBytesInUse() == EFF_SZ1 + EFF_SZ2);

        ca.deallocate(p2);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ1);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ1);

        ca.deallocate(p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == 0);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == 0);

        // two allocations and invoke 'resetNumBytesTotal'
        // -----------------------------------------------
        p1 = ca.allocate(SZ1);
        ASSERT(0 != p1);
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == EFF_SZ1);
        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == EFF_SZ1);

        p2 = ca.allocate(SZ2);
        ASSERT(0 != p2);
        LOOP_ASSERT(ca.numBytesTotal(),
                    ca.numBytesTotal() == EFF_SZ1 + EFF_SZ2);
        LOOP_ASSERT(ta.numBytesInUse(),
                    ta.numBytesInUse() == EFF_SZ1 + EFF_SZ2);

        ca.resetNumBytesTotal();
        LOOP_ASSERT(ca.numBytesTotal(), ca.numBytesTotal() == 0);

        ca.deallocate(p2);
        ca.deallocate(p1);

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
