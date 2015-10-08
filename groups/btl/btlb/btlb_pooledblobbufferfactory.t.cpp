// btlb_pooledblobbufferfactory.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlb_pooledblobbufferfactory.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>
#include <bsl_cstring.h>     // 'memcpy', 'memset'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef btlb::PooledBlobBufferFactory Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

void checkBlob(int         LINE,
               int         bufferSize,
               int         length,
               int         maxLength,
               btlb::Blob& mX)
{
    const int NUM_BUFFERS = (0 < maxLength) ? 1+(maxLength-1)/bufferSize : 0;
    const btlb::Blob& X = mX;

    LOOP2_ASSERT(bufferSize, LINE, bufferSize*NUM_BUFFERS == X.totalSize());
    LOOP2_ASSERT(bufferSize, LINE, length == X.length());
    LOOP2_ASSERT(bufferSize, LINE, NUM_BUFFERS == X.numBuffers());
    for (size_t i = 0; i < X.numBuffers(); ++i) {
        LOOP3_ASSERT(bufferSize, LINE, i, bufferSize == X.buffer(i).size());
        bsl::memset(mX.buffer(i).data(), (char)i, X.buffer(i).size());
    }
    for (size_t i = 0; i < X.numBuffers(); ++i) {
        LOOP3_ASSERT(bufferSize, LINE, i, X.buffer(i).data()[0] == (char)i);
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Plan: Due to the amount of work needed to build a test driver, for
        //   now, I simply copy the breathing test of a 'btlb_blob' but this
        //   time using the 'btlb::PooledBlobBufferFactory' instead of the
        //   (local) testing class 'TestBlobBufferFactory' in that component.
        //   In order to gain confidence against alignment and number of
        //   allocations, we try all buffer sizes in a reasonable range.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting bcema_PooledBlobBufferFactory."
                          << "\n--------------------------------------\n";
        for (int bufferSize = 1; bufferSize < 32 * sizeof(void*); ++bufferSize)
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::DefaultAllocatorGuard gard(&ta);

            {
                int maxLength = 0;
                Obj fa(bufferSize, &ta);

                btlb::Blob mX(&fa, &ta);  const btlb::Blob& X = mX;
                ASSERT(0 == X.length());
                ASSERT(0 == X.totalSize());
                ASSERT(0 == X.numBuffers());

                mX.setLength(0);
                ASSERT(0 == X.totalSize());
                ASSERT(0 == X.length());
                ASSERT(0 == X.numBuffers());

                mX.setLength(maxLength = 1);
                checkBlob(L_, bufferSize, 1, maxLength, mX);

                mX.setLength(maxLength = 2);
                checkBlob(L_, bufferSize, 2, maxLength, mX);

                mX.setLength(0);
                checkBlob(L_, bufferSize, 0, maxLength, mX);

                mX.setLength(1);
                checkBlob(L_, bufferSize, 1, maxLength, mX);

                mX.setLength(maxLength = 4);
                checkBlob(L_, bufferSize, 4, maxLength, mX);

                mX.setLength(maxLength = 5);
                checkBlob(L_, bufferSize, 5, maxLength, mX);

                mX.setLength(maxLength = 30);
                checkBlob(L_, bufferSize, 30, maxLength, mX);

                mX.setLength(maxLength = 34);
                checkBlob(L_, bufferSize, 34, maxLength, mX);

                mX.setLength(maxLength = 512*bufferSize);
                checkBlob(L_, bufferSize, maxLength, maxLength, mX);

                mX.removeBuffer(5);
                maxLength -= bufferSize;
                checkBlob(L_, bufferSize, maxLength, maxLength, mX);

                mX.removeBuffer(2);
                maxLength -= bufferSize;
                checkBlob(L_, bufferSize, maxLength, maxLength, mX);

                mX.setLength(45);
                checkBlob(L_, bufferSize, 45, maxLength, mX);

                mX.setLength(44);
                checkBlob(L_, bufferSize, 44, maxLength, mX);

                mX.setLength(1);
                checkBlob(L_, bufferSize, 1, maxLength, mX);

                btlb::BlobBuffer buf;
                fa.allocate(&buf);
                mX.appendBuffer(buf);
                maxLength += bufferSize;
                checkBlob(L_, bufferSize, 1, maxLength, mX);

                mX.setLength(bufferSize+3);
                checkBlob(L_, bufferSize, bufferSize+3, maxLength, mX);

                fa.allocate(&buf);
                mX.insertBuffer(1, buf);
                maxLength += bufferSize;
                checkBlob(L_, bufferSize, 2*bufferSize+3, maxLength, mX);

                fa.allocate(&buf);
                mX.insertBuffer(50, buf);
                maxLength += bufferSize;
                checkBlob(L_, bufferSize, 2*bufferSize+3, maxLength, mX);

                mX.setLength(0);
            }
            ASSERT(0 <  ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
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
