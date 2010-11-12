// btes_iovecutil.t.cpp          -*-C++-*-

#include <btes_iovecutil.h>

#include <bcema_blob.h>
#include <bcema_pooledbufferchain.h>
#include <bcema_pooledblobbufferfactory.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>

#if defined(BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES)
#include <strstrea.h>
#else
#include <bsl_strstream.h>
#endif
using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// [ 1]  btes_IovecUtil::length
// [ 1]  btes_IovecUtil::scatter
// [ 1]  btes_IovecUtil::gather
// [ 2]  btes_IovecUtil::chain
// [ 3]  btes_IovecUtil::appendToBlob
//-----------------------------------------------------------------------------
// [ 4]  USAGE EXAMPLE
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
#define Q_(X) cout << "<| " #X " |>";         // Q(X) without '\n'
#define L_ __LINE__                           // current Line number

#define T_() cout << "\t"<< flush; // indentation

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // Usage example
        //
        // Concerns:
        //
        // Plan:
        //
        // --------------------------------------------------------------------

      } break;
      case 3: {
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
            BUFFER_MAX_SIZE = 5,
            VECTOR_SIZE     = 100,
            VECTOR_NB       = 14   // must be big enough to contain BUFFER_SIZE
        };

        // Generate seemingly random buffer.
        if (verbose) cout << "\tGenerating buffer...\n";

        char buffer[VECTOR_SIZE];
        for (int i = 0; i < VECTOR_SIZE; ++i) {
            buffer[i] = (char)(67 + i * (i+13));
            if (veryVerbose) { int c = buffer[i]; P_(c); }
        }
        if (veryVerbose) { P_(BUFFER_MAX_SIZE); P(VECTOR_SIZE); }

        if (verbose) cout << "\n\tWith 'btes_Iovec'.";

        // Try several buffer sizes for the 'factory'.
        for (int m = 1; m < BUFFER_MAX_SIZE; ++m) {
            bcema_PooledBlobBufferFactory factory(m, &testAllocator);

            if (verbose)
                cout << "\tTrying factory of buffers of size " << m << ".\n";

        //--^
        // Try any number of vectors, containing up to VECTOR_SIZE characters.
        for (int i = 1;
             i < VECTOR_SIZE && (i * (i+1) / 2) < VECTOR_SIZE; ++i) {
            btes_Iovec                    vecs[VECTOR_NB];
            int                           numVecs = 0;

            // Generate up to VECTOR_NB vectors with vec[j].length() == j+1.
            // The data is contiguously taken from the buffer.
            if (veryVerbose) cout << "\tGenerating vectors..." << m << "\n";

            int dataSize = 0;
            for (int j = 1; j <= i; ++j) {
                vecs[j - 1].setBuffer(buffer + j * (j - 1) / 2, j);
                ++numVecs;
                dataSize += j;
            }
            ASSERT(numVecs <= VECTOR_NB);
            ASSERT(dataSize <= VECTOR_SIZE);
            if (verbose) { T_(); P_(numVecs); P(dataSize); }

            // Create a blob with 0 or more initial characters, append the
            // vectors to this blob starting at all possible offsets, and
            // verify that the resulting blob is valid.

            for (int j = 0; j < dataSize; ++j) {
                for (int offset = j; offset < dataSize; ++offset) {
                    if (veryVerbose)
                        cout << "\tTesting offset " << offset << ".\n";
                    // Create and initialize blob.
                    bcema_Blob *blob = new (testAllocator)
                                          bcema_Blob(&factory, &testAllocator);
                    blob->setLength(j);

                    for (int k = 0; k < j; ++k) {
                         (blob->buffer(k / m)).data()[k % m] = buffer[k];
                    }

                    // Utility under testing:
                    btes_IovecUtil::appendToBlob(blob, vecs, numVecs, offset);

                    // Verify length of blob i as expected.
                    if (veryVerbose) { T_(); T_(); P(offset); }
                    LOOP5_ASSERT(m, i, offset, blob->length(), numVecs,
                                    blob->length() == j + (dataSize - offset));

                    // Verify initial blob was not modified.
                    for (int k = 0; k < j; ++k) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                            buffer[k] == (blob->buffer(k / m)).data()[k % m]);
                    }

                    // Verify vecs contents after offset have been appended.
                    for (int k = offset, l = j; k < dataSize; ++k, ++l) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                             buffer[k] == (blob->buffer(l / m)).data()[l % m]);
                    }
                    testAllocator.deleteObjectRaw(blob);
                }
            }

        } // for (int i...)
        } // for (int m...)

        if (verbose) cout << "\n\tWith 'btes_Iovec'.";

        // Try several buffer sizes for the 'factory'.
        for (int m = 1; m < BUFFER_MAX_SIZE; ++m) {
            bcema_PooledBlobBufferFactory factory(m, &testAllocator);

            if (verbose)
                cout << "\tTrying factory of buffers of size " << m << ".\n";

        //--^
        // Try any number of vectors, containing up to VECTOR_SIZE characters.
        for (int i = 1;
             i < VECTOR_SIZE && (i * (i+1) / 2) < VECTOR_SIZE; ++i) {
            btes_Ovec vecs[VECTOR_NB];
            int       numVecs = 0;

            // Generate up to VECTOR_NB vectors with vec[j].length() == j+1.
            // The data is contiguously taken from the buffer.
            if (veryVerbose) cout << "\tGenerating vectors..." << m << "\n";

            int dataSize = 0;
            for (int j = 1; j <= i; ++j) {
                vecs[j - 1].setBuffer(buffer + j * (j - 1) / 2, j);
                ++numVecs;
                dataSize += j;
            }
            ASSERT(numVecs <= VECTOR_NB);
            ASSERT(dataSize <= VECTOR_SIZE);
            if (verbose) { T_(); P_(numVecs); P(dataSize); }

            // Create a blob with 0 or more initial characters, append the
            // vectors to this blob starting at all possible offsets, and
            // verify that the resulting blob is valid.

            for (int j = 0; j < dataSize; ++j) {
                for (int offset = j; offset < dataSize; ++offset) {
                    if (veryVerbose)
                        cout << "\tTesting offset " << offset << ".\n";
                    // Create and initialize blob.
                    bcema_Blob *blob = new (testAllocator)
                                          bcema_Blob(&factory, &testAllocator);
                    blob->setLength(j);

                    for (int k = 0; k < j; ++k) {
                         (blob->buffer(k / m)).data()[k % m] = buffer[k];
                    }

                    // Utility under testing:
                    btes_IovecUtil::appendToBlob(blob, vecs, numVecs, offset);

                    // Verify length of blob i as expected.
                    if (veryVerbose) { T_(); T_(); P(offset); }
                    LOOP5_ASSERT(m, i, offset, blob->length(), numVecs,
                                    blob->length() == j + (dataSize - offset));

                    // Verify initial blob was not modified.
                    for (int k = 0; k < j; ++k) {
                        LOOP5_ASSERT(m, i, j, offset, k,
                            buffer[k] == (blob->buffer(k / m)).data()[k % m]);
                    }

                    // Verify vecs contents after offset have been appended.
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
      case 2: {
        // --------------------------------------------------------------------
        // Test chain()
        //
        // Concerns:
        //   The chain is correctly generated using the correct offset.
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'chain'"
                          << "\n===============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        enum {
            BUFFER_MAX_SIZE = 5,
            BUFFER_SIZE     = 100,
            VECTOR_NB       = 14   // must be big enough to contain BUFFER_SIZE
        };

        // Generate seemingly random buffer.

        char buffer[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            buffer[i] = (char)(67 + i * (i+13));
            if (veryVerbose) { int c = buffer[i]; P_(c); }
        }
        if (veryVerbose) { P(BUFFER_SIZE); }

        // Try several buffer sizes for the 'factory'.
        for (int m = 1; m < BUFFER_MAX_SIZE; ++m) {
            bcema_PooledBlobBufferFactory factory(m, &testAllocator);

        //--^
        // Try any number of vectors, containing up to BUFFER_SIZE characters.
        for (int i = 1; i < BUFFER_SIZE && (i * (i+1) / 2) < BUFFER_SIZE;
             ++i) {
            bcema_PooledBufferChainFactory factory(m, &testAllocator);
            btes_Iovec                     vecs[VECTOR_NB];
            int                            numVecs = 0;

            // Generate up to VECTOR_NB vectors with vec[j].length() == j+1.
            // The data is contiguously taken from the buffer.

            int dataSize = 0;
            for (int j = 1; j <= i; ++j) {
                vecs[j - 1].setBuffer(buffer + j * (j - 1) / 2, j);
                ++numVecs;
                dataSize += j;
            }
            ASSERT(numVecs <= VECTOR_NB);
            ASSERT(dataSize <= BUFFER_SIZE);
            ASSERT(numVecs * (numVecs+1) / 2 == dataSize);
            if (verbose) { T_(); P_(numVecs); P(dataSize); }

            // Chain the vectors, starting at all possible offsets, and verify
            // that the resulting chain is valid.

            for (int offset = 0; offset < dataSize; ++offset) {
                if (veryVerbose) { T_(); T_(); T_(); P(offset); }
                // Utility under testing:
                bcema_PooledBufferChain *chain =
                                               btes_IovecUtil::chain(vecs,
                                                                     numVecs,
                                                                     offset,
                                                                     &factory);

                // Verify length of chain is as expected.
                LOOP5_ASSERT(m, i, offset, chain->length(), numVecs,
                                         chain->length() == dataSize - offset);

                // Verify contents of chain is as expected.
                for (int k = offset, l = 0; k < dataSize; ++k, ++l) {
                    LOOP4_ASSERT(m, i, offset, k,
                                     buffer[k] == chain->buffer(l / m)[l % m]);
                }
                factory.deleteObject(chain);
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

        if (verbose) cout << "\tTesting 'length' with 'btes_Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);
            ASSERT(0 == btes_IovecUtil::length(vector, 0));
            ASSERT(10 == btes_IovecUtil::length(vector, 1));
            ASSERT(14 == btes_IovecUtil::length(vector, 2));
            ASSERT(21 == btes_IovecUtil::length(vector, 3));
        }

        if (verbose) cout << "\tTesting 'length' with 'btes_Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);
            ASSERT(0 == btes_IovecUtil::length(vector, 0));
            ASSERT(10 == btes_IovecUtil::length(vector, 1));
            ASSERT(14 == btes_IovecUtil::length(vector, 2));
            ASSERT(21 == btes_IovecUtil::length(vector, 3));
        }

        if (verbose) cout << "\tTesting 'scatter' with 'btes_Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            const char CONTROL[25] = "abcdefghijklmnopqrstuvwx";

            ASSERT(0 == btes_IovecUtil::scatter(vector, 0, CONTROL, 0));
            ASSERT(0 == btes_IovecUtil::scatter(vector, 0, CONTROL, 1));
            ASSERT(0 == btes_IovecUtil::scatter(vector, 1, CONTROL, 0));

            ASSERT(1 == btes_IovecUtil::scatter(vector, 1, CONTROL, 1));
            ASSERT(9 == btes_IovecUtil::scatter(vector, 1, CONTROL, 9));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 1, CONTROL, 10));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 1, CONTROL, 11));

            ASSERT(9 == btes_IovecUtil::scatter(vector, 2, CONTROL, 9));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 2, CONTROL, 10));
            ASSERT(11 == btes_IovecUtil::scatter(vector, 2, CONTROL, 11));
            ASSERT(12 == btes_IovecUtil::scatter(vector, 2, CONTROL, 12));
            ASSERT(14 == btes_IovecUtil::scatter(vector, 2, CONTROL, 14));
            ASSERT(14 == btes_IovecUtil::scatter(vector, 2, CONTROL, 15));

            ASSERT(20 == btes_IovecUtil::scatter(vector, 3, CONTROL, 20));
            ASSERT(21 == btes_IovecUtil::scatter(vector, 3, CONTROL, 21));
            ASSERT(21 == btes_IovecUtil::scatter(vector, 3, CONTROL, 25));
        }

        if (verbose) cout << "\tTesting 'scatter' with 'btes_Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            const char CONTROL[25] = "abcdefghijklmnopqrstuvwx";

            ASSERT(0 == btes_IovecUtil::scatter(vector, 0, CONTROL, 0));
            ASSERT(0 == btes_IovecUtil::scatter(vector, 0, CONTROL, 1));
            ASSERT(0 == btes_IovecUtil::scatter(vector, 1, CONTROL, 0));

            ASSERT(1 == btes_IovecUtil::scatter(vector, 1, CONTROL, 1));
            ASSERT(9 == btes_IovecUtil::scatter(vector, 1, CONTROL, 9));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 1, CONTROL, 10));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 1, CONTROL, 11));

            ASSERT(9 == btes_IovecUtil::scatter(vector, 2, CONTROL, 9));
            ASSERT(10 == btes_IovecUtil::scatter(vector, 2, CONTROL, 10));
            ASSERT(11 == btes_IovecUtil::scatter(vector, 2, CONTROL, 11));
            ASSERT(12 == btes_IovecUtil::scatter(vector, 2, CONTROL, 12));
            ASSERT(14 == btes_IovecUtil::scatter(vector, 2, CONTROL, 14));
            ASSERT(14 == btes_IovecUtil::scatter(vector, 2, CONTROL, 15));

            ASSERT(20 == btes_IovecUtil::scatter(vector, 3, CONTROL, 20));
            ASSERT(21 == btes_IovecUtil::scatter(vector, 3, CONTROL, 21));
            ASSERT(21 == btes_IovecUtil::scatter(vector, 3, CONTROL, 25));
        }

        if (verbose) cout << "\tTesting 'gather' with 'btes_Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            char result[25];

            ASSERT(0 == btes_IovecUtil::gather(result, 0, vector, 0));
            ASSERT(0 == btes_IovecUtil::gather(result, 0, vector, 1));
            ASSERT(0 == btes_IovecUtil::gather(result, 1, vector, 0));

            ASSERT(1 == btes_IovecUtil::gather(result, 1, vector, 1));
            ASSERT(9 == btes_IovecUtil::gather(result, 9, vector, 1));
            ASSERT(10 == btes_IovecUtil::gather(result, 10, vector, 1));
            ASSERT(10 == btes_IovecUtil::gather(result, 11, vector, 1));

            ASSERT(9 == btes_IovecUtil::gather(result, 9, vector, 2));
            ASSERT(10 == btes_IovecUtil::gather(result, 10, vector, 2));
            ASSERT(11 == btes_IovecUtil::gather(result, 11, vector, 2));
            ASSERT(12 == btes_IovecUtil::gather(result, 12, vector, 2));
            ASSERT(14 == btes_IovecUtil::gather(result, 14, vector, 2));
            ASSERT(14 == btes_IovecUtil::gather(result, 15, vector, 2));

            ASSERT(20 == btes_IovecUtil::gather(result, 20, vector, 3));
            ASSERT(21 == btes_IovecUtil::gather(result, 21, vector, 3));
            ASSERT(21 == btes_IovecUtil::gather(result, 25, vector, 3));
        }

        if (verbose) cout << "\tTesting 'gather' with 'btes_Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            char result[25];

            ASSERT(0 == btes_IovecUtil::gather(result, 0, vector, 0));
            ASSERT(0 == btes_IovecUtil::gather(result, 0, vector, 1));
            ASSERT(0 == btes_IovecUtil::gather(result, 1, vector, 0));

            ASSERT(1 == btes_IovecUtil::gather(result, 1, vector, 1));
            ASSERT(9 == btes_IovecUtil::gather(result, 9, vector, 1));
            ASSERT(10 == btes_IovecUtil::gather(result, 10, vector, 1));
            ASSERT(10 == btes_IovecUtil::gather(result, 11, vector, 1));

            ASSERT(9 == btes_IovecUtil::gather(result, 9, vector, 2));
            ASSERT(10 == btes_IovecUtil::gather(result, 10, vector, 2));
            ASSERT(11 == btes_IovecUtil::gather(result, 11, vector, 2));
            ASSERT(12 == btes_IovecUtil::gather(result, 12, vector, 2));
            ASSERT(14 == btes_IovecUtil::gather(result, 14, vector, 2));
            ASSERT(14 == btes_IovecUtil::gather(result, 15, vector, 2));

            ASSERT(20 == btes_IovecUtil::gather(result, 20, vector, 3));
            ASSERT(21 == btes_IovecUtil::gather(result, 21, vector, 3));
            ASSERT(21 == btes_IovecUtil::gather(result, 25, vector, 3));
        }

        if (verbose) cout << "\tTesting 'pivot' with 'btes_Iovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Iovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            int bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 0);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 1);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 14);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 15);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
            for (int idx = 14; idx < 12; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(2 == bufferIdx);
                ASSERT(idx - 14 == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 21);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 22);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
        }

        if (verbose) cout << "\tTesting 'pivot' with 'btes_Ovec'\n";
        {
            char buf1[10];
            char buf2[4];
            char buf3[7];
            btes_Ovec vector[3];
            vector[0].setBuffer(buf1, 10);
            vector[1].setBuffer(buf2, 4);
            vector[2].setBuffer(buf3, 7);

            int bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 0);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 0, 1);
            ASSERT( 0 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 1, 10);
            ASSERT( 1 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 14);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 2, 15);
            ASSERT( 2 == bufferIdx);
            ASSERT(-1 == offset);

            for (int idx = 0; idx < 10; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(0 == bufferIdx);
                ASSERT(idx == offset);
            }
            for (int idx = 10; idx < 14; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(1 == bufferIdx);
                ASSERT(idx - 10 == offset);
            }
            for (int idx = 14; idx < 12; ++idx) {
                    bufferIdx = -1, offset = -1;
                btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, idx);
                ASSERT(2 == bufferIdx);
                ASSERT(idx - 14 == offset);
            }
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 21);
            ASSERT( 3 == bufferIdx);
            ASSERT(-1 == offset);
                bufferIdx = -1, offset = -1;
            btes_IovecUtil::pivot(&bufferIdx, &offset, vector, 3, 22);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
