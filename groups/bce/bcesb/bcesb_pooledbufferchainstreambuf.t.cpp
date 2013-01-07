// bcesb_pooledbufferchainstreambuf.t.cpp        -*-C++-*-

#include <bcesb_pooledbufferchainstreambuf.h>

#include <bcema_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_strstream.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

// Note: on Windows -> WinDef.h:#define max(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(max)
#undef max
#endif

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// 'bcesb_PooledBufferChainStreamBuf' private interface
// MANIPULATORS
// [  ] void setGetPosition(int off);
// [  ] void setPutPosition(int off);
//
// ACCESSORS
// [  ] int calcGetPosition() const;
// [  ] int calcPutPosition() const;
// [  ] int checkInvariant() const;
//
// 'bcesb_PooledBufferChainStreamBuf' protected interface
// MANIPULATORS
// [ 2] int_type overflow(int_type c = bsl::streambuf::traits_type::eof());
// [  ] int_type pbackfail(int_type c = bsl::streambuf::traits_type::eof());
// [  ] pos_type seekoff(off_type                offset,
//                       bsl::ios_base::seekdir  fixedPosition,
//                       bsl::ios_base::openmode which = bsl::ios_base::in
//                                                     | bsl::ios_base::out);
// [  ] pos_type seekpos(pos_type                position,
//                       bsl::ios_base::openmode which = bsl::ios_base::in
//                                                     | bsl::ios_base::out);
// [  ] bsl::streamsize showmanyc();
// [ 2] int sync();
// [  ] int_type underflow();
// [ 7] bsl::streamsize xsgetn(char_type       *destination,
//                             bsl::streamsize  numChars);
// [ 7] bsl::streamsize xsputn(const char_type *source,
//                             bsl::streamsize  numChars);
//
// 'bcesb_PooledBufferChainStreamBuf' public interface
// CREATORS
// [ 2] bcesb_PooledBufferChainStreamBuf(
//                                 bcema_PooledBufferChain *pooledBufferChain);
// [ 2] ~bcesb_PooledBufferChainStreamBuf();
//
// MANIPULATORS
// [  ] bcema_PooledBufferChain *data();
// [ 6] void reset(bcema_PooledBufferChain *pooledBufferChain);
//
// ACCESSORS
// [ 2] const bcema_PooledBufferChain *data() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMARY TEST APPARATUS
// [ 5] OUTPUT (<<) OPERATOR
// [ 8] CONCERN: EOF IS STREAMED CORRECTLY
// [  ] USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

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

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   coutMutex.lock(); { bsl::cout << bcemt_ThreadUtil::self() \
                                               << ": "
#define MTENDL   bsl::endl;  } coutMutex.unlock()
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
typedef bcesb_PooledBufferChainStreamBuf Obj;
typedef bcema_PooledBufferChainFactory   Factory;
typedef bcema_PooledBufferChain          Chain;

static int verbose = 0;
// static int veryVerbose = 0;  // not used
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    // veryVerbose = (argc > 3);  // not used
    veryVeryVerbose = (argc > 4);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // TESTING <YOUR TEST HERE>
        //
        // Concerns:
        //   * That ...
        //
        // Plan:
        //   Iterate over a set of test vectors varying in ...
        //
        // Testing:
        //   ...
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing <Your Test Here>" << endl
                 << "========================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {}
        //ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: EOF IS STREAMED CORRECTLY
        //
        // Concerns:
        //   * That 'xsputn' copies EOF when it appears within a buffer
        //     boundary.
        //
        //   * That 'xsputn' copies EOF when it appears as the first character
        //     in a buffer (i.e., when it crosses a buffer boundary).
        //
        // Plan:
        //   Iterate over a set of test vectors varying in buffer size and
        //   length of data to write.  For each test vector, instantiate a
        //   'bcema_PooledBufferChainFactory', 'mF', allocate a
        //   'bcema_PooledBufferChain', 'mC', and use 'mC' to instantiate a
        //   'bcema_PooledBufferChainStreamBuf', 'mX'.  Write the specified
        //   number of bytes to 'mX' using 'sputn', and verify the length of
        //   'mC'.  Read the specified number of buffers from 'mX', and verify
        //   the result, and the get area offset of 'mX'.
        //
        //   Instantiate a 'bcema_PooledBufferChainFactory', 'mF', allocate a
        //   'bcema_PooledBufferChain', 'mC', and use 'mC' to instantiate a
        //   'bcema_PooledBufferChainStreamBuf, 'mX'.  Iterate over an input
        //   buffer whose length is more than the size of 'mC', but less
        //   than twice the size of 'mC'.  On each iteration, write the input
        //   buffer into 'mX', substituting the i'th character for EOF.  Read
        //   back the data form 'mX', and verify the result.
        //
        // Testing:
        //   Concern: EOF is streamed correctly
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: EOF Is Streamed Correctly" << endl
                 << "==================================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            const struct {
                int d_line;          // source line number
                int d_bufferSize;    // factory buffer size
                int d_dataLength;    // length of data to read and write
            } DATA[] = {
                //Line  Buffer Size  Data Length
                //----  -----------  -----------
                { L_,   1,           1,         },
                { L_,   1,           5,         },
                { L_,   2,           1,         },
                { L_,   2,           2,         },
                { L_,   2,           5,         },
                { L_,   3,           9,         },
                { L_,   37,          101,       },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            const bsl::ios_base::seekdir  CUR  = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1 = bsl::ios_base::out;
            const bsl::ios_base::openmode IN1  = bsl::ios_base::in;
            const int EOF_VAL = Obj::traits_type::eof();

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int DATA_LENGTH = DATA[i].d_dataLength;

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(BUFFER_SIZE); P(DATA_LENGTH);
                }

                Factory  mF(BUFFER_SIZE, &ta);
                Chain   *mC = mF.allocate(0);
                {
                    Obj   mX(mC);
                    char *EOFS = (char *)ta.allocate(DATA_LENGTH);
                    bsl::memset(EOFS, EOF_VAL, DATA_LENGTH);

                    // Write out data.
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                             mX.sputn(EOFS, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                  mX.pubseekoff(0, CUR, OUT1));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH == mC->length());

                    // Read in data.
                    char *result = (char *)ta.allocate(DATA_LENGTH);
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                mX.sgetn(result, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                   mX.pubseekoff(0, CUR, IN1));
                    LOOP2_ASSERT(i, LINE, 0 == bsl::memcmp(EOFS,
                                                           result,
                                                           DATA_LENGTH));
                    ta.deallocate(EOFS);
                    ta.deallocate(result);
                }
                mF.deleteObject(mC);
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            enum {
                BUFFER_SIZE = 8,
                DATA_LENGTH = 12
            };
            ASSERT(DATA_LENGTH > BUFFER_SIZE);
            ASSERT(DATA_LENGTH < BUFFER_SIZE * 2);

            Factory mF(BUFFER_SIZE, &ta);

            char data[DATA_LENGTH];
            char result[DATA_LENGTH];
            bsl::memset(data, '*', DATA_LENGTH);

            for (int i = 0; i < DATA_LENGTH; ++i) {
                Chain *mC = mF.allocate(0);
                {
                    Obj mX(mC);
                    data[i] = EOF_VAL;
                    LOOP_ASSERT(i, DATA_LENGTH == mX.sputn(data, DATA_LENGTH));
                    LOOP_ASSERT(i, DATA_LENGTH == mX.pubseekoff(0, CUR, OUT1));
                    LOOP_ASSERT(i, DATA_LENGTH == mC->length());
                    LOOP_ASSERT(i, DATA_LENGTH == mX.sgetn(result,
                                                           DATA_LENGTH));
                    LOOP_ASSERT(i, 0 == bsl::memcmp(data,
                                                    result,
                                                    DATA_LENGTH));
                    data[i] = '*';
                }
                mF.deleteObject(mC);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'xsgetn' AND 'xsputn' FUNCTIONS
        //
        // Concerns:
        //   * That 'xsputn' returns the requested number of bytes when that
        //     number is less than the current buffer capacity.
        //
        //   * That 'xsputn' returns the requested number of bytes when that
        //     number is greater than the current buffer capacity.
        //
        //   * That 'xsgetn' returns the requested number of bytes when that
        //     number is less than the current buffer capacity.
        //
        //   * That 'xsgetn' returns the requested number of bytes when that
        //     number is greater than the current buffer capacity.
        //
        // Plan:
        //   Iterate over a set of test vectors varying in buffer size and
        //   length of data to write.  For each test vector, instantiate a
        //   'bcema_PooledBufferChainFactory', 'mF', allocate a
        //   'bcema_PooledBufferChain', 'mC', and use 'mC' to instantiate a
        //   'bcema_PooledBufferChainStreamBuf', 'mX'.  Write the specified
        //   number of bytes to 'mX' using 'sputn', and verify the length of
        //   'mC'.  Read the specified number of buffers from 'mX', and verify
        //   the result, and the get area offset of 'mX'.
        //
        // Testing:
        //   bsl::streamsize xsgetn(char_type       *destination,
        //                          bsl::streamsize  numChars);
        //   bsl::streamsize xsputn(const char_type *source,
        //                          bsl::streamsize  numChars);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'xsgetn' and 'xsputn' Functions" << endl
                 << "=======================================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            const struct {
                int d_line;          // source line number
                int d_bufferSize;    // factory buffer size
                int d_dataLength;    // length of data to read and write
            } DATA[] = {
                //Line  Buffer Size  Data Length
                //----  -----------  -----------
                { L_,   1,           1,         },
                { L_,   1,           5,         },
                { L_,   2,           1,         },
                { L_,   2,           2,         },
                { L_,   2,           5,         },
                { L_,   3,           9,         },
                { L_,   37,          101,       },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            const bsl::ios_base::seekdir  CUR  = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1 = bsl::ios_base::out;
            const bsl::ios_base::openmode IN1  = bsl::ios_base::in;

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int DATA_LENGTH = DATA[i].d_dataLength;

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(BUFFER_SIZE); P(DATA_LENGTH);
                }

                Factory  mF(BUFFER_SIZE, &ta);
                Chain   *mC = mF.allocate(0);
                {
                    Obj   mX(mC);
                    char *HASHMARKS = (char *)ta.allocate(DATA_LENGTH);
                    bsl::memset(HASHMARKS, '#', DATA_LENGTH);

                    // Write out data.
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                             mX.sputn(HASHMARKS, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                  mX.pubseekoff(0, CUR, OUT1));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH == mC->length());

                    // Read in data.
                    char *result = (char *)ta.allocate(DATA_LENGTH);
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                mX.sgetn(result, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                   mX.pubseekoff(0, CUR, IN1));
                    LOOP2_ASSERT(i, LINE, 0 == bsl::memcmp(HASHMARKS,
                                                           result,
                                                           DATA_LENGTH));
                    ta.deallocate(HASHMARKS);
                    ta.deallocate(result);
                }
                mF.deleteObject(mC);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'reset' FUNCTION
        //
        // Concerns:
        //   * That 'reset' called with the default argument resets the get
        //     and put areas, but does not affect the underlying buffer chain.
        //
        //   * That 'reset' called with a buffer chain argument both resets
        //     the underlying buffer chain to the specified buffer chain, and
        //     resets the get and put areas.
        //
        // Plan:
        //   Create two modifiable 'bcema_PooledBufferChain' objects, 'mCa'
        //   and 'mCb'.  Create a modifiable 'bcesb_PooledBufferChainStreamBuf'
        //   'mX', instantiated with 'mCa', and a non-modifiable reference to
        //   'mX' named 'X'.  Using 'X', verify that 'mX' is supported by
        //   'mCa'.  Adjust the get and put areas by calling 'pubseekpos' on
        //   'mX'.  Call 'reset' with the default argument on 'mX', and verify
        //   using 'X', and by calling 'pubseekoff' on 'mX', that the get and
        //   put areas have been reset.  Call 'reset' with argument 'mCb' on
        //   'mX', and verify as before that the get and put areas have been
        //   reset.  Additionally verify that 'mX' is not supported by 'mCb'.
        //
        // Testing:
        //   void reset(bcema_PooledBufferChain *pooledBufferChain);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'reset' Function" << endl
                 << "========================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum {
                BUFFER_SIZE_A = 16,    // buffer size for factory "A"
                BUFFER_SIZE_B = 32,    // buffer size for factory "B"
                SEEK_OFFSET   = 37,    // arbitrary offset
                DATA_LENGTH   = 64     // amount of data to write to stream
            };

            Factory  mFa(BUFFER_SIZE_A, &ta);
            Chain   *mCa = mFa.allocate(0);
            ASSERT(0 == mCa->length());
            ASSERT(0 == mCa->numBuffers());

            Factory  mFb(BUFFER_SIZE_B, &ta);
            Chain   *mCb = mFb.allocate(0);
            ASSERT(0 == mCb->length());
            ASSERT(0 == mCb->numBuffers());

            ASSERT(mCa->bufferSize() < mCb->bufferSize());

            const bsl::ios_base::seekdir  CUR  = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1 = bsl::ios_base::out;
            const bsl::ios_base::openmode IN1  = bsl::ios_base::in;

            {
                Obj mX(mCa);    const Obj& X = mX;
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));
                ASSERT(0 == mX.pubseekoff(0, CUR, OUT1));

                const bsl::string HASHMARKS(DATA_LENGTH, '#');
                bsl::ostream      out(&mX);

                out << HASHMARKS << flush;

                ASSERT(SEEK_OFFSET == mX.pubseekpos(SEEK_OFFSET, IN1));
                ASSERT(DATA_LENGTH == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(SEEK_OFFSET == mX.pubseekoff(0, CUR, IN1));
                ASSERT(DATA_LENGTH == mCa->length());
                ASSERT(4 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                mX.reset();
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));
                ASSERT(DATA_LENGTH == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(DATA_LENGTH == mCa->length());
                ASSERT(4 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                out << HASHMARKS << flush;

                ASSERT(SEEK_OFFSET == mX.pubseekpos(SEEK_OFFSET, IN1));
                ASSERT(2 * DATA_LENGTH == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(SEEK_OFFSET == mX.pubseekoff(0, CUR, IN1));
                ASSERT(2 * DATA_LENGTH == mCa->length());
                ASSERT(8 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                mX.reset(mCb);
                ASSERT(X.data() != mCa);
                ASSERT(X.data() == mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));
                ASSERT(0 == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(2 * DATA_LENGTH == mCa->length());
                ASSERT(8 == mCa->numBuffers());;
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                out << HASHMARKS << flush;

                ASSERT(SEEK_OFFSET == mX.pubseekpos(SEEK_OFFSET, IN1));
                ASSERT(DATA_LENGTH == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(SEEK_OFFSET == mX.pubseekoff(0, CUR, IN1));
                ASSERT(2 * DATA_LENGTH == mCa->length());
                ASSERT(8 == mCa->numBuffers());;
                ASSERT(DATA_LENGTH == mCb->length());
                ASSERT(2 == mCb->numBuffers());
            }
            mFa.deleteObject(mCa);
            mFb.deleteObject(mCb);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Verify the correct behavior of the output operator.
        //
        //   Currently, there is no 'operator<<' defined for
        //   'btemt_ChannelOutStreamBuf', so this test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Output (<<) Operator" << endl
                 << "============================" << endl;
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   * That ...
        //
        // Plan:
        //   Iterate over a set of test vectors varying in ...
        //
        // Testing:
        //   ...
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Basic Accessors" << endl
                 << "=======================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {}
        //ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
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
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   * That it is possible to instantiate a
        //     'bcesb_PooledBufferChainStreamBuf' object with a variety of
        //     'bcema_PooledBufferChain' parameters.
        //
        //   * That 'sync' and 'overflow' update the chain length.
        //
        // Plan:
        //   Iterate over a set of test vectors varying in buffer size.  For
        //   each test vector, instantiate a modifiable
        //   'bcesb_PooledBufferChainStreamBuf', 'mX', and a non-modifiable
        //   reference to 'mX' named 'X'.  Write data of length 3 times the
        //   specified buffer size to 'mX' in chunks of buffer size, 'sync'ing
        //   'mX' after each write, and verifying the chain length and number
        //   of buffers using 'X'.
        //
        // Testing:
        //   bcesb_PooledBufferChainStreamBuf(
        //                         bcema_PooledBufferChain *pooledBufferChain);
        //   ~bcesb_PooledBufferChainStreamBuf();
        //   int_type overflow(int_type c =
        //                                 bsl::streambuf::traits_type::eof());
        //   int sync();
        //   const bcema_PooledBufferChain *data() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            const struct {
                int d_line;          // source line number
                int d_bufferSize;    // factory buffer size
            } DATA[] = {
                //Line  Buffer Size
                //----  -----------
                { L_,   1,         },
                { L_,   2,         },
                { L_,   4,         },
                { L_,   11,        },
                { L_,   101,       },
                { L_,   1024,      },
                { L_,   4096,      },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;

                Factory            factory(BUFFER_SIZE, &ta);
                Chain             *chain = factory.allocate(0);
                Obj                mX(chain);
                const Obj&         X = mX;
                const bsl::string  HASHMARKS(BUFFER_SIZE, '#');

                if (verbose) {
                    P_(i); P_(LINE); P(BUFFER_SIZE);
                }

                enum { NUM_ITERATIONS = 3 };
                for (int j = 0; j < NUM_ITERATIONS; ++j) {
                    const int PRE_PUT_LENGTH   = j * BUFFER_SIZE;
                    const int POST_PUT_LENGTH  = PRE_PUT_LENGTH + 1;
                    const int POST_SYNC_LENGTH = (j + 1) * BUFFER_SIZE;

                    LOOP3_ASSERT(i, LINE, j,
                                 PRE_PUT_LENGTH == X.data()->length());
                    LOOP3_ASSERT(i, LINE, j, j == X.data()->numBuffers());

                    mX.sputn(HASHMARKS.c_str(), HASHMARKS.length());
                    LOOP3_ASSERT(i, LINE, j,
                                 POST_PUT_LENGTH == X.data()->length());
                    LOOP3_ASSERT(i, LINE, j, j + 1 == X.data()->numBuffers());

                    mX.pubsync();
                    LOOP3_ASSERT(i, LINE, j,
                                 POST_SYNC_LENGTH == X.data()->length());
                    LOOP3_ASSERT(i, LINE, j, j + 1 == X.data()->numBuffers());
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the
        //   'bcesb_PooledBufferChainStreamBuf' class.  We want to ensure that
        //   streambuf objects can be instantiated and destroyed.  We also want
        //   to exercise the primary manipulators and accessors.
        //
        // Plan:
        //   Iterate over a number of buffer sizes, and for each, create a
        //   modifiable 'bcesb_PooledBufferChainStreamBuf' object 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Create a
        //   'bsl::iostream' object, 'stream', using 'mX'.  Write an integer
        //   value to 'stream' and read it back, verify the result.  Reset the
        //   get area of 'mX', and read the integer value again, verifying it
        //   against the previously read value.  Reset the put and get areas
        //   of 'mX', and write and read a different integer value, again
        //   verifying the result.  Reset the put and get areas of 'mX', and
        //   write a string of length at least three times the buffer size.
        //   Read and verify the result.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum { MAX_BUFFER_SIZE = 10 };
            for(int i = 0; i < MAX_BUFFER_SIZE; ++i) {
                const int  BUFFER_SIZE = i + 1;
                Factory    factory(BUFFER_SIZE, &ta);
                Chain     *chain = factory.allocate(0);
                {
                    Obj           mX(chain);
                    const Obj&    X = mX;
                    bsl::iostream stream(&mX);

                    if (verbose) {
                        P_(i); P(BUFFER_SIZE);
                    }

                    stream << 12345;

                    int j;
                    stream >> j;
                    LOOP_ASSERT(i, 12345 == j);
                    if (verbose) {
                        T_(); P_(i); P(j);
                    }

                    stream.clear();
                    stream.seekg(0);

                    int k;
                    stream >> k;
                    LOOP_ASSERT(i, j == k);
                    if (verbose) {
                        T_(); P_(i); P_(j); P(k);
                    }

                    stream.clear();
                    stream.seekp(0);
                    stream.seekg(0);

                    bsl::string value;
                    stream << 654321;
                    stream >> value;
                    LOOP_ASSERT(i, "654321" == value);

                    stream.clear();
                    stream.seekp(0);
                    stream.seekg(0);

                    // Since we cannot truncate the streambuf, we must write
                    // at least as many bytes as already exist in the buffer.

                    int length = bsl::max(3 * BUFFER_SIZE, X.data()->length());
                    const bsl::string HASHMARKS(length, '#');
                    bsl::string       result;
                    stream << HASHMARKS;
                    stream >> result;
                    LOOP_ASSERT(i, HASHMARKS == result);
                    if (verbose) {
                        T_(); P_(i);
                        P_(HASHMARKS.length()); P(result.length());
                    }
                }
                factory.deleteObject(chain);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case -1: {
        // ------------------------------------------------------------
        // TEST ALL BYTE VALUES
        // ------------------------------------------------------------

        if (verbose) {
            cout << "BIT-PATTERN TEST" << endl
                 << "================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            const struct {
                int d_line;    // source line number
                int d_bufferSize;    // factory buffer size
            } DATA[] = {
                //Line  Buffer Size
                //----  -----------
                { L_,   1,         },
                { L_,   2,         },
                { L_,   3,         },
                { L_,   4,         },
                { L_,   5,         },
                { L_,   10,        },
                { L_,   255,       },
                { L_,   256,       },
                { L_,   1024,      },
            };
            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            // Create an array of all characters.
            char allChars[256];
            for (int i = 0; i < (int)sizeof allChars; ++i) {
                allChars[i] = char(i);
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            for (int i = 0; i < DATA_LEN; ++i) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;

                if (verbose) {
                    P_(i); P_(LINE); P(BUFFER_SIZE);
                }

                Factory  factory(BUFFER_SIZE, &ta);
                Chain   *chain = factory.allocate(0);

                Obj mX(chain);

                // Write bit patterns as two blocks of 256 bytes each.
                // Then read them back as 512 separate bytes.
                int n;

                if (verbose) {
                    cout << "Writing first 256 bytes." << endl;
                }
                n = mX.sputn(allChars, sizeof allChars);
                LOOP2_ASSERT(i, LINE, sizeof allChars == n);

                if (verbose) {
                    cout << "Writing second 256 bytes." << endl;
                }
                n = mX.sputn(allChars, sizeof allChars);
                LOOP2_ASSERT(i, LINE, sizeof allChars == n);

                if (verbose) {
                    cout << "Reading 512 bytes." << endl;
                }
                mX.pubseekpos(0, bsl::ios_base::in);
                for (int j = 0; j < (int)(2 * sizeof allChars); ++j) {
                    const int c1 = mX.sgetc();
                    LOOP3_ASSERT(i, LINE, j, EOF != c1);
                    if (EOF == c1) {
                        break;
                    }

                    const int c2 = mX.sbumpc();
                    LOOP3_ASSERT(i, LINE, j, c1 == c2);
                    LOOP3_ASSERT(i, LINE, j, c2 == (j & 0xff));

                    if (verbose) {
                        T_(); P_(i); P_(LINE); P_(j); P_(c1); P(c2);
                    }
                }

                if (verbose) {
                    cout << "Checking for EOF." << endl;
                }
                const int c3 = mX.sgetc();
                LOOP2_ASSERT(i, LINE, EOF == c3);
                const int c4 = mX.sgetc();
                LOOP2_ASSERT(i, LINE, c3 == c4);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
