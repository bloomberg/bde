// bcema_pooledbufferchain.t.cpp        -*-C++-*-
#include <bcema_pooledbufferchain.h>
#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()

#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
//-----------------------------------------------------------------------------
// 'bcema_PooledBufferChain' public interface
// CREATORS
// [ 2] bcema_PooledBufferChain(bcema_Pool *pool);
// [ 2] ~bcema_PooledBufferChain();
//
// MANIPULATORS
// [  ] char& operator[](int index);
// [11] void append(const char *buffer, int numBytes);
// [ 4] char *buffer(int index);
// [  ] void removeAll();
// [ 7] void replace(int offset, const char *buffer, int numBytes);
// [10] void replace(int                            offset,
//                   const bcema_PooledBufferChain& source,
//                   int                            srcOffset,
//                   int                            numBytes);
// [ 2] void setLength(int newLength);
// [ 2] void setLength(int newLength, bcema_PooledBufferChain *source);
//
// ACCESSORS
// [  ] const char& operator[](int index) const;
// [  ] const char *buffer(int index) const;
// [  ] int bufferSize() const;
// [ 8] void copyOut(char *buffer, int numBytes, int offset) const;
// [  ] int length() const;
// [  ] int numBuffers() const;
//-----------------------------------------------------------------------------
// 'bcema_PooledBufferChainFactory' public interface
// CREATORS
// [ 6] bcema_PooledBufferChainFactory(int              bufferSize,
//                                     bslma_Allocator *basicAllocator = 0);
// [ 6] ~bcema_PooledBufferChainFactory();
//
// MANIPULATORS
// [ 6] bcema_PooledBufferChain *allocate(int length);
// [ 6] void deleteObject(bcema_PooledBufferChain *object);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMARY TEST APPARATUS
// [ 5] OUTPUT (<<) OPERATOR
// [12] USAGE TEST
//=============================================================================

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
typedef bcema_PooledBufferChain Obj;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//               GLOBAL CLASSES AND HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static void fillIn(Obj *chain, char value)
{
    // Fill in the specified 'chain' with the specified 'value'.

    int numBuffers = chain->numBuffers();
    int bufferSize = chain->bufferSize();
    for (int i = 0; i < numBuffers; ++i) {
        char *buffer = chain->buffer(i);
        bsl::memset(buffer, value, bufferSize);
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
      case 13: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Create a 'my_DoubleArray2' object and append varying values to it.
        //   Verify that the values are correctly appended using 'operator[]'.
        //   Invoke 'removeAll' and verify that the array length becomes 0.
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        bcema_TestAllocator ta;

        struct my_Tick {
            char   d_name[4];
            double d_bid;
            double d_offer;
            // ...
        };
        enum { NUM_TICKS = 2000 };
        my_Tick packet[NUM_TICKS];
        enum { BUFFER_SIZE = 8192 };
        bcema_PooledBufferChainFactory factory(BUFFER_SIZE);
        bcema_PooledBufferChain *chain = factory.allocate(0);
        ASSERT(chain);                      ASSERT(0 == chain->length());
        int length = sizeof packet;
        chain->replace(0,     // initial offset into the chain
                       (const char*)packet,
                       length // number of bytes to copy
                      );
        ASSERT(length == chain->length());
        bcema_SharedPtr<bcema_PooledBufferChain> handle;
        handle.load(chain, &factory, &ta);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'append' METHOD
        //
        // Concerns:
        //   o That 'append' works correctly on 0 length chains.
        //   o That 'append' works correctly when the new length is less than
        //     the buffer chain capacity.
        //   o That 'append' correctly handles message boundaries.
        //
        // Plan: TBD
        //
        // Testing:
        //   void append(const char *buffer, int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'append' METHOD" << endl
                                  << "========================" << endl;
        struct {
            int         d_line;      // source line number
            int         d_srcLength; // length of data at source
            int         d_dstBufSize;// buffer size of destination chain
            int         d_dstLength; // length of initial data at destination
            int         d_numBytes;  // number of bytes to copy
            int         d_expLength; // expected length of destination
            const char *d_result;    // expected result at destination
        } DATA[] = {
            // Append to zero-length destination.
            //Line  Src Len  Dst BS  Dst Len  NB  Exp  Result
            //----  -------  ------  -------  --  ---  ------
            { L_,   1,       1,      0,       1,  1,   "S",        },
            { L_,   2,       1,      0,       2,  2,   "SS",       },
            { L_,   3,       1,      0,       3,  3,   "SSS",      },
            { L_,   7,       3,      0,       7,  7,   "SSSSSSS",  },
            { L_,   8,       5,      0,       8,  8,   "SSSSSSSS", },

            // New length is less than destination chain capacity.
            //Line  Src Len  Dst BS  Dst Len  NB  Exp  Result
            //----  -------  ------  -------  --  ---  ------
            { L_,   1,       2,      1,       1,  2,   "DS",            },
            { L_,   2,       3,      1,       2,  3,   "DSS",           },
            { L_,   3,       4,      9,       3,  12,  "DDDDDDDDDSSS",  },
            { L_,   7,       15,     3,       7,  10,  "DDDSSSSSSS",    },
            { L_,   8,       7,      10,      3,  13,  "DDDDDDDDDDSSS", },

            // New length is more than destination chain capacity.
            //Line  Src Len  Dst BS  Dst Len  NB  Exp  Result
            //----  -------  ------  -------  --  ---  ------
            { L_,   1,       1,      1,       1,  2,   "DS",                 },
            { L_,   2,       3,      3,       2,  5,   "DDDSS",              },
            { L_,   3,       2,      7,       3,  10,  "DDDDDDDSSS",         },
            { L_,   7,       15,     13,      5,  18,  "DDDDDDDDDDDDDSSSSS", },
            { L_,   8,       3,      12,      3,  15,  "DDDDDDDDDDDDSSS",    },
        };
        enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            for (int i = 0; i < DATA_SIZE; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         SRC_LENGTH  = DATA[i].d_srcLength;
                const int         DST_BUFSIZE = DATA[i].d_dstBufSize;
                const int         DST_LENGTH  = DATA[i].d_dstLength;
                const int         NUM_BYTES   = DATA[i].d_numBytes;
                const int         EXP_LENGTH  = DATA[i].d_expLength;
                const bsl::string RESULT(DATA[i].d_result);

                if (veryVerbose) {
                    P_(i); P_(LINE);
                    P_(SRC_LENGTH); P_(DST_BUFSIZE); P_(DST_LENGTH);
                    P_(NUM_BYTES); P(EXP_LENGTH);
                }

                LOOP2_ASSERT(i, LINE, EXP_LENGTH == (int)RESULT.length());

                // Allocate source, and fill.
                char *source = (char *)ta.allocate(SRC_LENGTH);
                bsl::memset(source, 'S', SRC_LENGTH);

                // Allocate destination chain from destination factory.
                bcema_PooledBufferChainFactory  mFd(DST_BUFSIZE, &ta);
                bcema_PooledBufferChain        *mCd =
                                                  mFd.allocate(DST_LENGTH + 1);
                fillIn(mCd, 'D');
                mCd->setLength(DST_LENGTH);
                LOOP2_ASSERT(i, LINE, DST_LENGTH == mCd->length());

                mCd->append(source, NUM_BYTES);
                LOOP2_ASSERT(i, LINE, EXP_LENGTH == mCd->length());

                bsl::string result;
                int         remainingLength = mCd->length();
                for (int j = 0; j < mCd->numBuffers(); ++j) {
                    int bufSize = (j < mCd->numBuffers() - 1)
                                ? DST_BUFSIZE
                                : remainingLength;
                    result.append(mCd->buffer(j), bufSize);
                    remainingLength -= bufSize;
                }
                LOOP2_ASSERT(i, LINE, RESULT == result);

                ta.deallocate(source);
                mFd.deleteObject(mCd);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'replace' METHOD
        //
        // Concerns:
        //   o That 'replace' works correctly on 0 length chains.
        //   o That 'replace' overwrites data correctly
        //     - when offset is 0
        //     - when offset is 'length'
        //   o That 'replace' correctly handles message boundaries.
        //
        // Plan:
        // Testing:
        //   void replace(int                            offset,
        //                const btemt_PooledBufferChain& source,
        //                int                            srcOffset,
        //                int                            numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'replace' METHOD" << endl
                                  << "========================" << endl;
        struct {
            int         d_line;      // source line number
            int         d_srcBufSize;// buffer size of source chain
            int         d_srcLength; // length of initial data at source
            int         d_dstBufSize;// buffer size of destination chain
            int         d_dstLength; // length of initial data at destination
            int         d_offset;    // copy offset into destination chain
            int         d_srcOffset; // copy offset into source chain
            int         d_numBytes;  // number of bytes to copy
            int         d_expLength; // expected length of destination
            const char *d_result;    // expected result at destination
        } DATA[] = {
            // Copy into zero-length destination.
            //Line  Src BS  Src Len  Dst BS  Dst Len  Off  Src Off  NB  Exp
            //----  ------  -------  ------  -------  ---  -------  --  ---
            { L_,   1,      1,       1,      0,       0,   0,       1,  1,
                                                                      "S", },
            { L_,   1,      2,       1,      0,       0,   0,       2,  2,
                                                                     "SS", },
            { L_,   1,      3,       1,      0,       0,   0,       3,  3,
                                                                    "SSS", },
            { L_,   2,      7,       3,      0,       0,   0,       7,  7,
                                                                "SSSSSSS", },
            { L_,   4,      8,       5,      0,       0,   0,       8,  8,
                                                               "SSSSSSSS", },

            // Zero byte 'replace' truncates destination.
            //Line  Src BS  Src Len  Dst BS  Dst Len  Off  Src Off  NB  Exp
            //----  ------  -------  ------  -------  ---  -------  --  ---
            { L_,   1,      0,       1,      2,       0,   0,       0,  0,
                                                                       "", },
            { L_,   1,      2,       1,      2,       0,   0,       0,  0,
                                                                       "", },
            { L_,   2,      2,       3,      2,       1,   0,       0,  1,
                                                                      "D", },
            { L_,   3,      0,       7,      15,      8,   0,       0,  8,
                                                               "DDDDDDDD", },

            // Source and destination buffer sizes are equal.
            //Line  Src BS  Src Len  Dst BS  Dst Len  Off  Src Off  NB  Exp
            //----  ------  -------  ------  -------  ---  -------  --  ---
            { L_,   5,      15,      5,      8,       0,   0,       8,  8,
                                                               "SSSSSSSS", },
            { L_,   5,      15,      5,      8,       3,   12,      3,  6,
                                                                 "DDDSSS", },
            { L_,   5,      15,      5,      8,       3,   7,       8,  11,
                                                            "DDDSSSSSSSS", },
            { L_,   5,      15,      5,      8,       7,   3,       8,  15,
                                                        "DDDDDDDSSSSSSSS", },
            { L_,   5,      15,      5,      8,       8,   0,       15, 23,
                                                "DDDDDDDDSSSSSSSSSSSSSSS", },

            // Source and destination buffer sizes are multiples of each other.
            //Line  Src BS  Src Len  Dst BS  Dst Len  Off  Src Off  NB  Exp
            //----  ------  -------  ------  -------  ---  -------  --  ---
            { L_,   3,      24,      6,      24,      0,   0,       12, 12,
                                                           "SSSSSSSSSSSS", },
            { L_,   3,      24,      6,      24,      1,   1,       12, 13,
                                                          "DSSSSSSSSSSSS", },
            { L_,   3,      24,      6,      24,      6,   3,       12, 18,
                                                     "DDDDDDSSSSSSSSSSSS", },
            { L_,   3,      24,      6,      24,      7,   8,       12, 19,
                                                    "DDDDDDDSSSSSSSSSSSS", },
            { L_,   3,      24,      6,      24,      12,  12,      12, 24,
                                               "DDDDDDDDDDDDSSSSSSSSSSSS", },

            { L_,   6,      24,      3,      24,      0,   0,       12, 12,
                                                           "SSSSSSSSSSSS", },
            { L_,   6,      24,      3,      24,      1,   1,       12, 13,
                                                          "DSSSSSSSSSSSS", },
            { L_,   6,      24,      3,      24,      3,   6,       12, 15,
                                                        "DDDSSSSSSSSSSSS", },
            { L_,   6,      24,      3,      24,      8,   7,       12, 20,
                                                   "DDDDDDDDSSSSSSSSSSSS", },
            { L_,   6,      24,      3,      24,      12,  12,      12, 24,
                                               "DDDDDDDDDDDDSSSSSSSSSSSS", },

            // Source and destination buffer sizes are relatively prime.
            //Line  Src BS  Src Len  Dst BS  Dst Len  Off  Src Off  NB  Exp
            //----  ------  -------  ------  -------  ---  -------  --  ---
            { L_,   3,      21,      7,      14,      14,  0,       21, 35,
                                    "DDDDDDDDDDDDDDSSSSSSSSSSSSSSSSSSSSS", },
            { L_,   3,      21,      7,      14,      2,   5,       16, 18,
                                                     "DDSSSSSSSSSSSSSSSS", },
        };
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            for (int i = 0; i < DATA_SIZE; ++i) {
                const int         LINE        = DATA[i].d_line;
                const int         SRC_BUFSIZE = DATA[i].d_srcBufSize;
                const int         SRC_LENGTH  = DATA[i].d_srcLength;
                const int         DST_BUFSIZE = DATA[i].d_dstBufSize;
                const int         DST_LENGTH  = DATA[i].d_dstLength;
                const int         OFFSET      = DATA[i].d_offset;
                const int         SRC_OFFSET  = DATA[i].d_srcOffset;
                const int         NUM_BYTES   = DATA[i].d_numBytes;
                const int         EXP_LENGTH  = DATA[i].d_expLength;
                const bsl::string RESULT(DATA[i].d_result);

                if (veryVerbose) {
                    P_(i); P_(LINE);
                    P_(SRC_BUFSIZE); P_(SRC_LENGTH); P(SRC_OFFSET);

                    P_(i); P_(LINE);
                    P_(DST_BUFSIZE); P_(DST_LENGTH); P_(OFFSET);
                    P_(NUM_BYTES); P(EXP_LENGTH);
                }

                LOOP2_ASSERT(i, LINE, EXP_LENGTH == (int)RESULT.length());

                // Allocate source chain from source factory.
                bcema_PooledBufferChainFactory  mFs(SRC_BUFSIZE, &ta);
                bcema_PooledBufferChain        *mCs =
                                                  mFs.allocate(SRC_LENGTH + 1);
                fillIn(mCs, 'S');
                mCs->setLength(SRC_LENGTH);
                LOOP2_ASSERT(i, LINE, SRC_LENGTH == mCs->length());

                // Allocate destination chain from destination factory.
                bcema_PooledBufferChainFactory  mFd(DST_BUFSIZE, &ta);
                bcema_PooledBufferChain        *mCd =
                                                  mFd.allocate(DST_LENGTH + 1);
                fillIn(mCd, 'D');
                mCd->setLength(DST_LENGTH);
                LOOP2_ASSERT(i, LINE, DST_LENGTH == mCd->length());

                mCd->replace(OFFSET, *mCs, SRC_OFFSET, NUM_BYTES);
                LOOP2_ASSERT(i, LINE, EXP_LENGTH == mCd->length());

                bsl::string result;
                int         remainingLength = mCd->length();
                for (int j = 0; j < mCd->numBuffers(); ++j) {
                    int bufSize = (j < mCd->numBuffers() - 1)
                                ? DST_BUFSIZE
                                : remainingLength;
                    result.append(mCd->buffer(j), bufSize);
                    remainingLength -= bufSize;
                }
                LOOP2_ASSERT(i, LINE, RESULT == result);

                mFs.deleteObject(mCs);
                mFd.deleteObject(mCd);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'replace' METHOD
        //
        // Concerns:
        //   o replace method works correctly on 0 length chains
        //   o replace method overwrites data correctly
        //     - when offset is 0
        //     - when offset is 'length'
        //   o replace method handles correctly message boundaries
        //
        // Plan:
        //
        // Testing:
        //   void replace(int offset,
        //                const Obj& source, int srcOffset,
        //                int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'replace' METHOD" << endl
                                  << "========================" << endl;
        enum {
            BUFFER_SIZE             = 128,
            MAX_CONTROL_BUFFER_SIZE = BUFFER_SIZE * 5,
            BS                      = BUFFER_SIZE
        };

        struct {
            int    d_line;
            int    d_length;
            int    d_offset;
            int    d_sourceOffset;
            int    d_numBytes;
            int    d_expLength;
        } DATA[] = {
        //    d_line d_length d_offset d_sourceOffset d_numBytes  d_expLength
            // Replacing 0 bytes
            { L_,       0,       0,         0,            0,          0      },
            { L_,       1,       0,         0,            0,          1      },
            { L_,       1,       1,         0,            0,          1      },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_TestAllocator testAllocator(veryVeryVerbose);
        bcema_PooledBufferChainFactory factory(BUFFER_SIZE, &testAllocator);
        char controlBuffer[MAX_CONTROL_BUFFER_SIZE];
        char control = (unsigned char) 0xBA;
        char plain  = (unsigned char) 0xAB;
        bsl::memset(controlBuffer, control, MAX_CONTROL_BUFFER_SIZE);

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj *mX = factory.allocate(DATA[i].d_length);
            const Obj& X = *mX;
            LOOP_ASSERT(i, mX);
            LOOP_ASSERT(i, DATA[i].d_length == X.length());
            fillIn(mX, plain);
            factory.allocate(DATA[i].d_length);

            mX->replace(DATA[i].d_offset, controlBuffer,
                        DATA[i].d_numBytes);

            if (veryVerbose) {
                Q("STATE DUMP")
                P(DATA[i].d_line);
                P(DATA[i].d_length);
                P(DATA[i].d_offset);
                P(DATA[i].d_numBytes);
                P(DATA[i].d_expLength);
                P(mX->length());
            }
            int offset = DATA[i].d_offset;
            int numBytes = DATA[i].d_numBytes;
            int length = mX->length();
            LOOP_ASSERT(i, DATA[i].d_expLength == length);
            LOOP_ASSERT(i, offset + numBytes <= length);
            int j;
            for (j = 0; j < offset; ++j) {
                LOOP2_ASSERT(i, j, X[j] == plain);
            }
            offset += numBytes;
            for (; j < offset; ++j) {
                LOOP2_ASSERT(i, j, X[j] == control);
            }
            for (; j < length; ++j) {
                LOOP2_ASSERT(i, j, X[j] == plain);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'copyOut' METHOD
        //
        // Concerns:
        //   o copy out takes care correctly of the buffer boundaries
        //
        // Plan:
        //   Use table-driven approach.  Stretch the object under test,
        //   and fill it in with some constant characters.  Create a
        //   destination buffer and fill it in with some other character,
        //   and then copy out data from the object under test to the
        //   destination buffer.  Verify that the data is replaced correctly.
        //
        // Testing:
        //   void copyOut(char *buffer, int numBytes, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'copyOut' METHOD" << endl
                                  << "========================" << endl;
        enum {
            BUFFER_SIZE         = 128,
            MAX_DST_BUFFER_SIZE = BUFFER_SIZE * 5,
            BS                  = BUFFER_SIZE
        };

        struct {
            int    d_line;
            int    d_length;
            int    d_offset;
            int    d_numBytes;
        } DATA[] = {
            //d_line   d_length d_offset d_numBytes
            // Copy out 0 bytes
            { L_,         0,        0,            0         },
            { L_,         1,        0,            0         },
            { L_,         1,        1,            0         },

            // Copy out 1 bytes
            { L_,         1,        0,            1         },
            { L_,         1,        0,            1         },

            // Copy out 2 bytes
            { L_,         2,        0,            2         },
            { L_,         3,        0,            2         },
            { L_,         3,        1,            2         },
            { L_,        BS,      BS - 2,         2         },
            { L_,       BS + 1,   BS - 1,         2         },

            // Copy out BS - 1 bytes
            { L_,        BS,        0,         BS - 1        },
            { L_,        BS,        1,         BS - 1        },
            { L_,      2 * BS,    BS / 2,      BS - 1        },
            { L_,      3 * BS,    BS + 3,      BS - 1        },

            // Copy out BS bytes
            { L_,        BS,        0,         BS            },
            { L_,       BS + 2,     1,         BS            },
            { L_,       BS + 2,     2,         BS            },
            { L_,    3 * BS / 2,    BS / 2,    BS            },

            // Copy out 2 * BS bytes
            { L_,    2 * BS,        0,        2 * BS         },
            { L_,    2 * BS + 1,    1,        2 * BS         },
            { L_,    2 * BS + 2,    2,        2 * BS         },
            { L_,    3 * BS - 2,   BS - 2,    2 * BS         },
            { L_,    3 * BS    ,   BS - 2,    2 * BS         },
            { L_,    3 * BS    ,   BS,        2 * BS         },

            // Copy out 3 * BS bytes
            { L_,    3 * BS,        0,        3 * BS         },
            { L_,    3 * BS + 1,    1,        3 * BS         },
            { L_,    3 * BS + 2,    2,        3 * BS         },
            { L_,    4 * BS - 2,   BS - 2,    3 * BS         },
            { L_,    4 * BS    ,   BS - 2,    3 * BS         },
            { L_,    4 * BS    ,   BS,        3 * BS         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_TestAllocator testAllocator(veryVeryVerbose);
        bcema_PooledBufferChainFactory factory(BUFFER_SIZE, &testAllocator);

        char control = (unsigned char) 0xBA;
        char plain   = (unsigned char) 0xAB;


        for (int i = 0; i < NUM_DATA; ++i) {
            Obj *mX = factory.allocate(DATA[i].d_length);
            const Obj& X = *mX;
            LOOP_ASSERT(i, mX);
            LOOP_ASSERT(i, DATA[i].d_length == X.length());
            fillIn(mX, plain);

            char dstBuffer[MAX_DST_BUFFER_SIZE];
            bsl::memset(dstBuffer, control, MAX_DST_BUFFER_SIZE);

            mX->copyOut(dstBuffer, DATA[i].d_numBytes, DATA[i].d_offset);

            if (veryVerbose) {
                Q("STATE DUMP")
                P(DATA[i].d_line);
                P(DATA[i].d_length);
                P(DATA[i].d_offset);
                P(DATA[i].d_numBytes);
            }
            int numBytes = DATA[i].d_numBytes;
            int j;
            for (j = 0; j < numBytes; ++j) {
                LOOP2_ASSERT(i, j, dstBuffer[j] == plain);
            }
            for (; j < MAX_DST_BUFFER_SIZE; ++j) {
                LOOP2_ASSERT(i, j, dstBuffer[j] == control);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'replace' METHOD
        //
        // Concerns:
        //   o replace method works correctly on 0 length chain
        //   o replace method takes care correctly of allocating new buffers
        //   o replace method overwrites data correctly
        //
        // Plan:
        //   Use table-driven approach.  Stretch the object under test,
        //   and fill it in with some constant characters.  Create a
        //   replacement buffer, fill it in with some other characters.
        //   Then, replace the buffers and verify, using 'operator[]'
        //   that the data was replaced correctly.
        //
        // Testing:
        //   void replace(int offset, const char *buffer, int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'replace' METHOD" << endl
                                  << "=========================" << endl;
        enum {
            BUFFER_SIZE             = 128,
            MAX_CONTROL_BUFFER_SIZE = BUFFER_SIZE * 5,
            BS                      = BUFFER_SIZE
        };

        struct {
            int    d_line;
            int    d_length;
            int    d_offset;
            int    d_numBytes;
            int    d_expLength;
        } DATA[] = {
            //d_line d_length d_offset d_numBytes  d_expLength
            // Replacing 0 bytes
            { L_,       0,       0,        0,          0       },
            { L_,       1,       0,        0,          1       },
            { L_,       1,       1,        0,          1       },

            // Replacing 1 byte
            { L_,       0,       0,        1,          1       },
            { L_,       1,       0,        1,          1       },
            { L_,       2,       1,        1,          2       },
            { L_,       1,       1,        1,          2       },

            // Replacing 2 bytes
            { L_,       0,       0,        2,          2       },
            { L_,       1,       0,        2,          2       },
            { L_,       1,       1,        2,          3       },
            { L_,       2,       0,        2,          2       },
            { L_,       2,       1,        2,          3       },
            { L_,       2,       2,        2,          4       },
            { L_,      BS,      BS - 2,    2,          BS      },
            { L_,      BS,      BS - 1,    2,          BS + 1  },
            { L_,      BS,      BS,        2,          BS + 2  },

            // Replacing BS - 1 bytes
            { L_,       0,       0,       BS - 1,     BS - 1   },
            { L_,       1,       0,       BS - 1,     BS - 1   },
            { L_,      BS,       0,       BS - 1,     BS       },
            { L_,      BS,       1,       BS - 1,     BS       },
            { L_,      BS,       2,       BS - 1,     BS + 1   },
            { L_,      BS,       3,       BS - 1,     BS + 2   },

            { L_,      BS,     BS / 2,    BS - 1,  3 * BS / 2 - 1    },
            { L_,  2 * BS,       1,       BS - 1,     2 * BS         },
            { L_,  2 * BS,     BS / 2,    BS - 1,     2 * BS         },
            { L_,  2 * BS,   3 *  BS / 2, BS - 1,  5 * BS / 2 - 1    },

            // Replacing BS  bytes
            { L_,       0,       0,       BS,         BS           },
            { L_,       1,       0,       BS,         BS           },
            { L_,       1,       1,       BS,         BS + 1       },
            { L_,      BS,       0,       BS,         BS           },
            { L_,      BS,     BS - 1 ,   BS,     2 * BS - 1       },
            { L_,  2 * BS,     BS - 1 ,   BS,     2 * BS           },

            // Replacing 2 * BS bytes
            { L_,       0,       0,     2*BS,     2 * BS           },
            { L_,       1,       0,     2*BS,     2 * BS           },
            { L_,       1,       1,     2*BS,     2 * BS + 1       },
            { L_,      BS,       0,     2*BS,     2 * BS           },
            { L_,      BS,     BS - 1 , 2*BS,     3 * BS - 1       },
            { L_,      BS,     BS - 2 , 2*BS,     3 * BS - 2       },
            { L_,  2 * BS,     BS - 1 , 2*BS,     3 * BS - 1       },
            { L_,  2 * BS,     BS - 3 , 2*BS,     3 * BS - 3       },
            { L_,  3 * BS,     BS - 1 , 2*BS,     3 * BS           },

            // Replacing 3 * BS bytes
            { L_,       0,       0,     3*BS,     3 * BS           },
            { L_,       1,       0,     3*BS,     3 * BS           },
            { L_,       1,       1,     3*BS,     3 * BS + 1       },
            { L_,      BS,       0,     3*BS,     3 * BS           },
            { L_,      BS,     BS - 1 , 3*BS,     4 * BS - 1       },
            { L_,  2 * BS,     BS - 1 , 3*BS,     4 * BS - 1       },
            { L_,  2 * BS,     BS - 2 , 3*BS,     4 * BS - 2       },
            { L_,  3 * BS,     BS - 1 , 3*BS,     4 * BS - 1       },
            { L_,  3 * BS,     BS - 3 , 3*BS,     4 * BS - 3       },
            { L_,  4 * BS,     BS - 1 , 3*BS,     4 * BS           },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bcema_TestAllocator testAllocator(veryVeryVerbose);
        bcema_PooledBufferChainFactory factory(BUFFER_SIZE, &testAllocator);
        char controlBuffer[MAX_CONTROL_BUFFER_SIZE];
        char control = (unsigned char) 0xBA;
        char plain   = (unsigned char) 0xAB;
        bsl::memset(controlBuffer, control, MAX_CONTROL_BUFFER_SIZE);

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj *mX = factory.allocate(DATA[i].d_length);
            const Obj& X = *mX;
            LOOP_ASSERT(i, mX);
            LOOP_ASSERT(i, DATA[i].d_length == X.length());
            fillIn(mX, plain);

            mX->replace(DATA[i].d_offset, controlBuffer,
                        DATA[i].d_numBytes);

            if (veryVerbose) {
                Q("STATE DUMP")
                P(DATA[i].d_line);
                P(DATA[i].d_length);
                P(DATA[i].d_offset);
                P(DATA[i].d_numBytes);
                P(DATA[i].d_expLength);
                P(mX->length());
            }
            int offset = DATA[i].d_offset;
            int numBytes = DATA[i].d_numBytes;
            int length = mX->length();
            LOOP_ASSERT(i, DATA[i].d_expLength == length);
            LOOP_ASSERT(i, offset + numBytes <= length);
            int j;
            for (j = 0; j < offset; ++j) {
                LOOP2_ASSERT(i, j, X[j] == plain);
            }
            offset += numBytes;
            for (; j < offset; ++j) {
                LOOP2_ASSERT(i, j, X[j] == control);
            }
            for (; j < length; ++j) {
                LOOP2_ASSERT(i, j, X[j] == plain);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'bcema_PooledBufferChainFactory' CLASS
        //
        // Concerns:
        //    o that the buffer size is correctly set by the factory
        //    o that the length is correctly set by the 'Factory::allocate'
        //    o that factories can deallocate chains that they allocated
        //
        // Plan:
        //   Allocate chains of different specified length with a factory
        //   and verify that all concerns above are met.
        //
        // Testing:
        //   class bcema_PooledBufferChainFactory;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "TESTING 'bcema_PooledBufferChainFactory' CLASS" << endl
              << "==============================================" << endl;

        enum {
            BUFFER_SIZE      = 128,
            NUM_BUFFERS      = 1000,
            NUM_CHAINS       = 1000
        };

        bcema_TestAllocator testAllocator(veryVeryVerbose);
        bcema_PooledBufferChainFactory factory(BUFFER_SIZE, &testAllocator);
        bcema_PooledBufferChain *chains[NUM_CHAINS];

        for (int i = 0; i < NUM_CHAINS; ++i) {
            chains[i] = factory.allocate(i + 1);
            LOOP_ASSERT(i, BUFFER_SIZE == chains[i]->bufferSize());
            LOOP_ASSERT(i, i + 1       == chains[i]->length());
        }
        if (verbose) {
            P(NUM_CHAINS);
        }
        bcema_Deleter<bcema_PooledBufferChain> *deleter = &factory;
        for (int i = 0; i < NUM_CHAINS; ++i) {
            deleter->deleteObject(chains[i]);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Verify the correct behavior of the output operator.
        //
        //   Currently, there is no 'operator<<' defined for
        //   'bcema_PooledBufferChain' or 'bcema_PooledBufferChainFactory',
        //   so this test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Output (<<) Operator" << endl
                 << "============================" << endl;
        }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'loadBuffers' METHOD
        //
        // Concerns:
        //   o loadBuffers report the correct sequence, given 'index'
        //   o no overflows, and correct return value, in first overload
        //
        // Plan:
        //   Stretch objects to different length and dump data to buffers
        //   incrementally.
        //
        // Testing:
        //   int loadBuffers(const char *buffer, ...) const;
        //   void loadBuffers(bsl::vector<const char *> *buffer) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'loadBuffers' METHOD" << endl
                          << "============================" << endl;

        enum {
            POOL_BUFFER_SIZE = 128,
            BUFFER_SIZE      = POOL_BUFFER_SIZE - sizeof(char*),
            MAX_NUM_BUFFERS  = 100,
            PADDING          = 5
        };

        bcema_Pool pool(POOL_BUFFER_SIZE);

        for (int i = 0; i < MAX_NUM_BUFFERS; ++i) {
            int newLength = BUFFER_SIZE * i;
            Obj mX(&pool); const Obj& X = mX;
            ASSERT(BUFFER_SIZE  == X.bufferSize());
            mX.setLength(newLength);

            LOOP_ASSERT(i, X.length() == newLength);
            LOOP_ASSERT(i, X.numBuffers() ==
                         (newLength + BUFFER_SIZE - 1)
                         / BUFFER_SIZE);

            int numBuffers = X.numBuffers();
            if (veryVerbose) {
                P_(newLength); P(numBuffers);
            }

            const char *UNINITIALIZED = (const char *)0xDEADBEEF;
            const char *buffers[MAX_NUM_BUFFERS + PADDING];

            for (int j = 0; j < numBuffers; ++j) {
                bsl::fill(buffers, buffers + MAX_NUM_BUFFERS + PADDING,
                          UNINITIALIZED);
                X.loadBuffers(buffers, MAX_NUM_BUFFERS + PADDING, j);
                for (int k = j; k < numBuffers; ++k) {
                    LOOP3_ASSERT(i, j, k,
                                 X.buffer(k) == buffers[k - j]);
                }
                for (int k = numBuffers; k < MAX_NUM_BUFFERS + PADDING; ++k) {
                    LOOP3_ASSERT(i, j, k, UNINITIALIZED == buffers[k - j]);
                }
            }

            bsl::vector<const char *> buffersVector;
           buffersVector.push_back(UNINITIALIZED);
            X.loadBuffers(&buffersVector);
            LOOP_ASSERT(i, numBuffers == (int) buffersVector.size());
            for (int k = 0; k < numBuffers; ++k) {
                LOOP2_ASSERT(i, k, X.buffer(k) == buffersVector[k]);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'buffer' METHOD
        //
        // Concerns:
        //   o const and non-const versions return same value
        //   o buffer reports a continuous chunk of memory can be written to
        //
        // Plan:
        //   Stretch objects to different length and dump data to buffers
        //   incrementally.
        //
        // Testing:
        //   char *buffer(int index);
        //   const char *buffer(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'buffer' METHOD" << endl
                                  << "==========================" << endl;

        enum {
            POOL_BUFFER_SIZE = 128,
            BUFFER_SIZE      = POOL_BUFFER_SIZE - sizeof(char*),
            MAX_NUM_BUFFERS  = 100
        };

        bcema_Pool pool(POOL_BUFFER_SIZE);

        for (int i = 0; i < MAX_NUM_BUFFERS; ++i) {
            int newLength = BUFFER_SIZE * i;
            Obj mX(&pool); const Obj& X = mX;
            ASSERT(BUFFER_SIZE  == X.bufferSize());
            mX.setLength(newLength);

            LOOP_ASSERT(i, X.length() == newLength);
            LOOP_ASSERT(i, X.numBuffers() ==
                         (newLength + BUFFER_SIZE - 1)
                         / BUFFER_SIZE);
            int numBuffers = X.numBuffers();
            if (veryVerbose) {
                P_(newLength); P(numBuffers);
            }
            for (int j = 0; j < numBuffers; ++j) {
                // non-'const' version
                char *buffer = mX.buffer(j);
                int size = X.bufferSize();
                for (int k = 0; k < size; ++k) {
                    buffer[k] = 'A';
                }
                // 'const' version
                const char *constBuffer = X.buffer(j);
                LOOP2_ASSERT(i, j, buffer == constBuffer);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY TEST APPARATUS
        //
        // Concerns:
        //   Verify the correct behavior of the primary test apparatus.
        //
        //   Currently, there is no primary test apparatus to verify, so this
        //   test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Test Apparatus" << endl
                 << "==============================" << endl;
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'setLength' METHOD
        //
        // Concerns:
        //   o a length can be increased with proper buffer allocations
        //   o a length can be decreased with proper buffer deallocations
        //
        // Plan:
        //   Create an object under test and stretch it in order to test
        //   concern #1.  For concern #2, rely on correct behavior for
        //   increasing the length.
        //
        // Testing:
        //   setLength(int newLength);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'setLength' METHOD" << endl
                                  << "==========================" << endl;

        enum {
            POOL_BUFFER_SIZE = 128,
            NUM_BUFFERS      = 20
        };

        bcema_Pool pool(POOL_BUFFER_SIZE);

        if (verbose)
            cout << "\tStretching the buffer chain." << endl;
        {
            for (int i = 0; i < NUM_BUFFERS; ++i) {
                enum { BUFFER_SIZE = POOL_BUFFER_SIZE - sizeof(char*) };
                for (int j = -1; j < 2; ++j) {
                    Obj mX(&pool); const Obj& X = mX;
                    ASSERT(BUFFER_SIZE  == X.bufferSize());
                    int stretchLength = i * BUFFER_SIZE + (i ? j : j + 1);
                    if (veryVerbose) {
                        P_(i); P_(j); P(stretchLength);
                    }

                    mX.setLength(stretchLength);

                    LOOP2_ASSERT(i, j, X.length() == stretchLength);
                    LOOP2_ASSERT(i, j,
                                 X.numBuffers() ==
                                 (stretchLength + BUFFER_SIZE - 1)
                                 / BUFFER_SIZE);
                    if (veryVerbose) {
                        T_(); P_(X.length()); P(X.numBuffers());
                    }
                }
            }
        }
        if (verbose)
            cout << "\tStretching the buffer chain, with import." << endl;
        {
            for (int i = 0; i < NUM_BUFFERS; ++i) {
                enum { BUFFER_SIZE = POOL_BUFFER_SIZE - sizeof(char*) };
                for (int j = -1; j < 2; ++j) {

                    int stretchLength = i * BUFFER_SIZE + (i ? j : j+1);
                    if (veryVerbose) {
                        P_(i); P_(j); P(stretchLength);
                    }

                    for (int k = 0; k < NUM_BUFFERS; ++k) {
                        for (int l = -1; l < 2; ++l) {
                            Obj mX(&pool); const Obj& X = mX;
                            Obj mY(&pool); const Obj& Y = mY;
                            ASSERT(BUFFER_SIZE  == X.bufferSize());
                            ASSERT(BUFFER_SIZE  == Y.bufferSize());

                            int sourceLength = BUFFER_SIZE * k + (k ? l : l+1);
                            if (veryVerbose) {
                                T_(); P_(k); P_(l); P(sourceLength);
                            }

                            // Length available from X before Y is tapped.
                            int extraLength = (BUFFER_SIZE - (i ? j : j+1)) %
                                                                   BUFFER_SIZE;

                            mY.setLength(sourceLength);
                            mX.setLength(stretchLength, &mY);

                            LOOP4_ASSERT(i, j, k, l,
                                         X.length() == stretchLength);
                            LOOP4_ASSERT(i, j, k, l,
                                         X.numBuffers() ==
                                         (stretchLength + BUFFER_SIZE - 1)
                                         / BUFFER_SIZE);
                            LOOP4_ASSERT(i, j, k, l,
                                         Y.length() == bsl::max(0,
                                                                sourceLength
                                                              - stretchLength
                                                              - extraLength));
                            LOOP4_ASSERT(i, j, k, l,
                                         Y.numBuffers() ==
                                         (Y.length() + BUFFER_SIZE - 1)
                                         / BUFFER_SIZE);
                            if (veryVerbose) {
                                T_(); P_(stretchLength);
                                      P_(sourceLength);
                                      P(extraLength);
                                T_(); P_(X.length()); P(X.numBuffers());
                                T_(); P_(Y.length()); P(Y.numBuffers());
                            }
                        }
                    }
                }
            }
        }
        if (verbose)
            cout << "\tShrinking the buffer chain." << endl;
        {
            for (int i = 0; i < NUM_BUFFERS; ++i) {
                enum { BUFFER_SIZE = POOL_BUFFER_SIZE - sizeof(char*) };
                for (int j = -1; j < 2; ++j) {
                    Obj mX(&pool); const Obj& X = mX;
                    ASSERT(BUFFER_SIZE  == X.bufferSize());
                    int stretchLength = i * BUFFER_SIZE + (i ? j : j + 1);

                    mX.setLength(stretchLength);

                    LOOP2_ASSERT(i, j, X.length() == stretchLength);
                    LOOP2_ASSERT(i, j,
                                 X.numBuffers() ==
                                 (stretchLength + BUFFER_SIZE - 1)
                                 / BUFFER_SIZE);
                    for (int k = 0; k < 3; ++k) {
                        int shrinkLength = stretchLength - k * BUFFER_SIZE + j;
                        if (shrinkLength < 0) {
                            shrinkLength = 0;
                        }
                        if (veryVerbose) {
                            P_(i); P_(j); P_(stretchLength); P(shrinkLength);
                        }
                        mX.setLength(shrinkLength);

                        LOOP2_ASSERT(i, j, X.length() == shrinkLength);
                        LOOP2_ASSERT(i, j,
                                     X.numBuffers() ==
                                     (shrinkLength + BUFFER_SIZE - 1)
                                     / BUFFER_SIZE);
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   An object can be created and basic operations can be performed.
        //
        // Plan:
        //   Create an object under test and perform basic operations.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { POOL_BUFFER_SIZE = 128 };

        bcema_Pool pool(POOL_BUFFER_SIZE);

        Obj mX(&pool); const Obj& X = mX;
        ASSERT(POOL_BUFFER_SIZE - sizeof(char*) == X.bufferSize());
        ASSERT(0 == X.numBuffers()); ASSERT(0 == X.length());

        mX.setLength(1);
        ASSERT(1 == X.numBuffers()); ASSERT(1 == X.length());

        mX.setLength(2);
        ASSERT(1 == X.numBuffers()); ASSERT(2 == X.length());

        mX.setLength(X.bufferSize());
        ASSERT(1 == X.numBuffers()); ASSERT(X.bufferSize() == X.length());
        mX.setLength(X.bufferSize() + 1);
        ASSERT(2 == X.numBuffers()); ASSERT(X.bufferSize() + 1 == X.length());
        mX.removeAll();
        ASSERT(0 == X.numBuffers()); ASSERT(0 == X.length());
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
