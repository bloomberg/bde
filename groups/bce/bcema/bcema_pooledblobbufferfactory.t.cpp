// bcema_pooledblobbufferfactory.t.cpp                                -*-C++-*-
#include <bcema_pooledblobbufferfactory.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_cstring.h>     // memcpy(), memset()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------
//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bcema_PooledBlobBufferFactory Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

void checkBlob(int LINE, int bufferSize, int length, int maxLength,
               bcema_Blob& mX)
{
    const int NUM_BUFFERS = (0 < maxLength) ? 1+(maxLength-1)/bufferSize : 0;
    const bcema_Blob& X = mX;

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

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

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
        //   now, I simply copy the breathing test of a 'bcema_blob' but this
        //   time using the 'bcema_PooledBlobBufferFactory' instead of the
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
            bslma_TestAllocator ta(veryVeryVerbose);
            bslma_DefaultAllocatorGuard gard(&ta);

            {
                int maxLength = 0;
                Obj fa(bufferSize, &ta);

                bcema_Blob mX(&fa, &ta);  const bcema_Blob& X = mX;
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

                bcema_BlobBuffer buf;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
