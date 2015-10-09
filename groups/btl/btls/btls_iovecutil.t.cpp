// btls_iovecutil.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls_iovecutil.h>

#include <bslim_testutil.h>

#include <btlb_blob.h>
#include <btlb_pooledblobbufferfactory.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// [ 1]  btls::IovecUtil::length
// [ 1]  btls::IovecUtil::scatter
// [ 1]  btls::IovecUtil::gather
// [ 2]  btls::IovecUtil::appendToBlob
//-----------------------------------------------------------------------------
// [ 3]  USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // Usage example
        //
        // Concerns:
        //
        // Plan:
        //
        // --------------------------------------------------------------------

///Usage
///-----
//..
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test appendToBlob()
        //
        // Concerns:
        //   The blob is correctly generated using the correct offset.
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'appendToBlob'"
                          << "\n======================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        enum {
            k_BUFFER_MAX_SIZE = 5,
            k_VECTOR_SIZE     = 100,
            k_VECTOR_NB       = 14 // must be big enough to contain BUFFER_SIZE
        };

        // Generate seemingly random buffer.
        if (verbose) cout << "\tGenerating buffer...\n";

        char buffer[k_VECTOR_SIZE];
        for (int i = 0; i < k_VECTOR_SIZE; ++i) {
            buffer[i] = (char)(67 + i * (i+13));
            if (veryVerbose) { int c = buffer[i]; P_(c); }
        }
        if (veryVerbose) { P_(k_BUFFER_MAX_SIZE); P(k_VECTOR_SIZE); }

        if (verbose) cout << "\n\tWith 'btls::Iovec'.";

        // Try several buffer sizes for the 'factory'.
        for (int m = 1; m < k_BUFFER_MAX_SIZE; ++m) {
            btlb::PooledBlobBufferFactory factory(m, &testAllocator);

            if (verbose)
                cout << "\tTrying factory of buffers of size " << m << ".\n";

        //--^
        // Try any number of vectors, containing up to k_VECTOR_SIZE
        // characters.
        for (int i = 1;
             i < k_VECTOR_SIZE && (i * (i+1) / 2) < k_VECTOR_SIZE; ++i) {
            btls::Iovec                    vecs[k_VECTOR_NB];
            int                           numVecs = 0;

            // Generate up to k_VECTOR_NB vectors with vec[j].length() == j+1.
            // The data is contiguously taken from the buffer.
            if (veryVerbose) cout << "\tGenerating vectors..." << m << "\n";

            int dataSize = 0;
            for (int j = 1; j <= i; ++j) {
                vecs[j - 1].setBuffer(buffer + j * (j - 1) / 2, j);
                ++numVecs;
                dataSize += j;
            }
            ASSERT(numVecs <= k_VECTOR_NB);
            ASSERT(dataSize <= k_VECTOR_SIZE);
            if (verbose) { T_; P_(numVecs); P(dataSize); }

            // Create a blob with 0 or more initial characters, append the
            // vectors to this blob starting at all possible offsets, and
            // verify that the resulting blob is valid.

            for (int j = 0; j < dataSize; ++j) {
                for (int offset = j; offset < dataSize; ++offset) {
                    if (veryVerbose)
                        cout << "\tTesting offset " << offset << ".\n";
                    // Create and initialize blob.
                    btlb::Blob *blob = new (testAllocator)
                                          btlb::Blob(&factory, &testAllocator);
                    blob->setLength(j);

                    for (int k = 0; k < j; ++k) {
                         (blob->buffer(k / m)).data()[k % m] = buffer[k];
                    }

                    // Utility under testing:
                    btls::IovecUtil::appendToBlob(blob, vecs, numVecs, offset);

                    // Verify length of blob i as expected.
                    if (veryVerbose) { T_; T_; P(offset); }
                    LOOP5_ASSERT(m, i, offset, blob->length(), numVecs,
                                    blob->length() == j + (dataSize - offset));

                    // Verify initial blob was not modified.
                    for (int k = 0; k < j; ++k) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                            buffer[k] == (blob->buffer(k / m)).data()[k % m]);
                    }

                    // Verify vectors contents after offset have been appended.
                    for (int k = offset, l = j; k < dataSize; ++k, ++l) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                             buffer[k] == (blob->buffer(l / m)).data()[l % m]);
                    }
                    testAllocator.deleteObjectRaw(blob);
                }
            }

        } // for (int i...)
        } // for (int m...)

        if (verbose) cout << "\n\tWith 'btls::Iovec'.";

        // Try several buffer sizes for the 'factory'.
        for (int m = 1; m < k_BUFFER_MAX_SIZE; ++m) {
            btlb::PooledBlobBufferFactory factory(m, &testAllocator);

            if (verbose)
                cout << "\tTrying factory of buffers of size " << m << ".\n";

        //--^
        // Try any number of vectors, containing up to k_VECTOR_SIZE
        // characters.
        for (int i = 1;
             i < k_VECTOR_SIZE && (i * (i+1) / 2) < k_VECTOR_SIZE; ++i) {
            btls::Ovec vecs[k_VECTOR_NB];
            int       numVecs = 0;

            // Generate up to k_VECTOR_NB vectors with vec[j].length() == j+1.
            // The data is contiguously taken from the buffer.
            if (veryVerbose) cout << "\tGenerating vectors..." << m << "\n";

            int dataSize = 0;
            for (int j = 1; j <= i; ++j) {
                vecs[j - 1].setBuffer(buffer + j * (j - 1) / 2, j);
                ++numVecs;
                dataSize += j;
            }
            ASSERT(numVecs <= k_VECTOR_NB);
            ASSERT(dataSize <= k_VECTOR_SIZE);
            if (verbose) { T_; P_(numVecs); P(dataSize); }

            // Create a blob with 0 or more initial characters, append the
            // vectors to this blob starting at all possible offsets, and
            // verify that the resulting blob is valid.

            for (int j = 0; j < dataSize; ++j) {
                for (int offset = j; offset < dataSize; ++offset) {
                    if (veryVerbose)
                        cout << "\tTesting offset " << offset << ".\n";
                    // Create and initialize blob.
                    btlb::Blob *blob = new (testAllocator)
                                          btlb::Blob(&factory, &testAllocator);
                    blob->setLength(j);

                    for (int k = 0; k < j; ++k) {
                         (blob->buffer(k / m)).data()[k % m] = buffer[k];
                    }

                    // Utility under testing:
                    btls::IovecUtil::appendToBlob(blob, vecs, numVecs, offset);

                    // Verify length of blob i as expected.
                    if (veryVerbose) { T_; T_; P(offset); }
                    LOOP5_ASSERT(m, i, offset, blob->length(), numVecs,
                                    blob->length() == j + (dataSize - offset));

                    // Verify initial blob was not modified.
                    for (int k = 0; k < j; ++k) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                            buffer[k] == (blob->buffer(k / m)).data()[k % m]);
                    }

                    // Verify vectors contents after offset have been appended.
                    for (int k = offset, l = j; k < dataSize; ++k, ++l) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                             buffer[k] == (blob->buffer(l / m)).data()[l % m]);
                    }
                    testAllocator.deleteObjectRaw(blob);
                }
            }

        } // for (int i...)
        } // for (int m...)

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'length', 'scatter', 'gather', and 'pivot'
        //
        // Concerns:
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'length', 'scatter', 'gather' and 'pivot'"
                 << "\n================================================="
                 << endl;

        if (verbose) cout << "\tTesting 'length' with 'btls::Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);
            ASSERT(0 == btls::IovecUtil::length(vector, 0));
            ASSERT(10 == btls::IovecUtil::length(vector, 1));
            ASSERT(14 == btls::IovecUtil::length(vector, 2));
            ASSERT(21 == btls::IovecUtil::length(vector, 3));
        }

        if (verbose) cout << "\tTesting 'length' with 'btls::Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);
            ASSERT(0 == btls::IovecUtil::length(vector, 0));
            ASSERT(10 == btls::IovecUtil::length(vector, 1));
            ASSERT(14 == btls::IovecUtil::length(vector, 2));
            ASSERT(21 == btls::IovecUtil::length(vector, 3));
        }

        if (verbose) cout << "\tTesting 'scatter' with 'btls::Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            const char CONTROL[25] = "abcdefghijklmnopqrstuvwx";

            ASSERT(0 == btls::IovecUtil::scatter(vector, 0, CONTROL, 0));
            ASSERT(0 == btls::IovecUtil::scatter(vector, 0, CONTROL, 1));
            ASSERT(0 == btls::IovecUtil::scatter(vector, 1, CONTROL, 0));

            ASSERT(1 == btls::IovecUtil::scatter(vector, 1, CONTROL, 1));
            ASSERT(9 == btls::IovecUtil::scatter(vector, 1, CONTROL, 9));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 1, CONTROL, 10));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 1, CONTROL, 11));

            ASSERT(9 == btls::IovecUtil::scatter(vector, 2, CONTROL, 9));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 2, CONTROL, 10));
            ASSERT(11 == btls::IovecUtil::scatter(vector, 2, CONTROL, 11));
            ASSERT(12 == btls::IovecUtil::scatter(vector, 2, CONTROL, 12));
            ASSERT(14 == btls::IovecUtil::scatter(vector, 2, CONTROL, 14));
            ASSERT(14 == btls::IovecUtil::scatter(vector, 2, CONTROL, 15));

            ASSERT(20 == btls::IovecUtil::scatter(vector, 3, CONTROL, 20));
            ASSERT(21 == btls::IovecUtil::scatter(vector, 3, CONTROL, 21));
            ASSERT(21 == btls::IovecUtil::scatter(vector, 3, CONTROL, 25));
        }

        if (verbose) cout << "\tTesting 'scatter' with 'btls::Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            const char CONTROL[25] = "abcdefghijklmnopqrstuvwx";

            ASSERT(0 == btls::IovecUtil::scatter(vector, 0, CONTROL, 0));
            ASSERT(0 == btls::IovecUtil::scatter(vector, 0, CONTROL, 1));
            ASSERT(0 == btls::IovecUtil::scatter(vector, 1, CONTROL, 0));

            ASSERT(1 == btls::IovecUtil::scatter(vector, 1, CONTROL, 1));
            ASSERT(9 == btls::IovecUtil::scatter(vector, 1, CONTROL, 9));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 1, CONTROL, 10));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 1, CONTROL, 11));

            ASSERT(9 == btls::IovecUtil::scatter(vector, 2, CONTROL, 9));
            ASSERT(10 == btls::IovecUtil::scatter(vector, 2, CONTROL, 10));
            ASSERT(11 == btls::IovecUtil::scatter(vector, 2, CONTROL, 11));
            ASSERT(12 == btls::IovecUtil::scatter(vector, 2, CONTROL, 12));
            ASSERT(14 == btls::IovecUtil::scatter(vector, 2, CONTROL, 14));
            ASSERT(14 == btls::IovecUtil::scatter(vector, 2, CONTROL, 15));

            ASSERT(20 == btls::IovecUtil::scatter(vector, 3, CONTROL, 20));
            ASSERT(21 == btls::IovecUtil::scatter(vector, 3, CONTROL, 21));
            ASSERT(21 == btls::IovecUtil::scatter(vector, 3, CONTROL, 25));
        }

        if (verbose) cout << "\tTesting 'gather' with 'btls::Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            char result[25];

            ASSERT(0 == btls::IovecUtil::gather(result, 0, vector, 0));
            ASSERT(0 == btls::IovecUtil::gather(result, 0, vector, 1));
            ASSERT(0 == btls::IovecUtil::gather(result, 1, vector, 0));

            ASSERT(1 == btls::IovecUtil::gather(result, 1, vector, 1));
            ASSERT(9 == btls::IovecUtil::gather(result, 9, vector, 1));
            ASSERT(10 == btls::IovecUtil::gather(result, 10, vector, 1));
            ASSERT(10 == btls::IovecUtil::gather(result, 11, vector, 1));

            ASSERT(9 == btls::IovecUtil::gather(result, 9, vector, 2));
            ASSERT(10 == btls::IovecUtil::gather(result, 10, vector, 2));
            ASSERT(11 == btls::IovecUtil::gather(result, 11, vector, 2));
            ASSERT(12 == btls::IovecUtil::gather(result, 12, vector, 2));
            ASSERT(14 == btls::IovecUtil::gather(result, 14, vector, 2));
            ASSERT(14 == btls::IovecUtil::gather(result, 15, vector, 2));

            ASSERT(20 == btls::IovecUtil::gather(result, 20, vector, 3));
            ASSERT(21 == btls::IovecUtil::gather(result, 21, vector, 3));
            ASSERT(21 == btls::IovecUtil::gather(result, 25, vector, 3));
        }

        if (verbose) cout << "\tTesting 'gather' with 'btls::Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            char result[25];

            ASSERT(0 == btls::IovecUtil::gather(result, 0, vector, 0));
            ASSERT(0 == btls::IovecUtil::gather(result, 0, vector, 1));
            ASSERT(0 == btls::IovecUtil::gather(result, 1, vector, 0));

            ASSERT(1 == btls::IovecUtil::gather(result, 1, vector, 1));
            ASSERT(9 == btls::IovecUtil::gather(result, 9, vector, 1));
            ASSERT(10 == btls::IovecUtil::gather(result, 10, vector, 1));
            ASSERT(10 == btls::IovecUtil::gather(result, 11, vector, 1));

            ASSERT(9 == btls::IovecUtil::gather(result, 9, vector, 2));
            ASSERT(10 == btls::IovecUtil::gather(result, 10, vector, 2));
            ASSERT(11 == btls::IovecUtil::gather(result, 11, vector, 2));
            ASSERT(12 == btls::IovecUtil::gather(result, 12, vector, 2));
            ASSERT(14 == btls::IovecUtil::gather(result, 14, vector, 2));
            ASSERT(14 == btls::IovecUtil::gather(result, 15, vector, 2));

            ASSERT(20 == btls::IovecUtil::gather(result, 20, vector, 3));
            ASSERT(21 == btls::IovecUtil::gather(result, 21, vector, 3));
            ASSERT(21 == btls::IovecUtil::gather(result, 25, vector, 3));
        }

        if (verbose) cout << "\tTesting 'pivot' with 'btls::Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            int bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 0);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 1);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 14);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 15);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
            for (int idx = 14; idx < 12; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(2 == bufferIdx);
                ASSERT(idx - 14 == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 21);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 22);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
        }

        if (verbose) cout << "\tTesting 'pivot' with 'btls::Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btls::Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            int bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 0);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 1);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 14);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 15);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
            for (int idx = 14; idx < 12; ++idx) {
                    bufferIdx = -1, offset = -1;
                btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(2 == bufferIdx);
                ASSERT(idx - 14 == offset);
            }
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 21);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btls::IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 22);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
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
