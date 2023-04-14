// bdlbb_blobstreambuf.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlbb_blobstreambuf.h>

#include <bdlbb_blob.h>

#include <bslim_testutil.h>

#include <bdlt_datetime.h>

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>      // 'isdigit' 'isupper' 'islower'
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memset'/memcmp()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::flush;
using bsl::endl;
using bsl::cerr;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// All classes in this component are a value-semantic types that represent
// big-endian integer types.  They have the same value if they have the same
// in-core big endian representation.
//
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

class testBlobBufferFactory : public bdlbb::BlobBufferFactory
{
    bslma::Allocator *d_allocator_p;
    bsl::size_t       d_currentBufferSize;
    bool              d_growFlag;

    private:
    // not implemented
    testBlobBufferFactory(const testBlobBufferFactory&);
    testBlobBufferFactory& operator=(const testBlobBufferFactory&);

    public:
    // CREATORS
    explicit testBlobBufferFactory(bslma::Allocator *allocator,
                                   bsl::size_t       currentBufferSize = 4);
    ~testBlobBufferFactory();

    // MANIPULATORS
    void allocate(bdlbb::BlobBuffer *buffer);
    void setGrowFlag(bool growFlag);

    // ACCESSORS
    bsl::size_t currentBufferSize() const;
    bool growFlag() const;
};

testBlobBufferFactory::testBlobBufferFactory(
                                           bslma::Allocator *allocator,
                                           bsl::size_t       currentBufferSize)
: d_allocator_p(allocator)
, d_currentBufferSize(currentBufferSize)
, d_growFlag(true)
{
}

testBlobBufferFactory::~testBlobBufferFactory()
{
}

void testBlobBufferFactory::allocate(bdlbb::BlobBuffer *buffer)
{

    bsl::shared_ptr<char> shptr(
                         (char *) d_allocator_p->allocate(d_currentBufferSize),
                          d_allocator_p);

    buffer->reset(shptr, d_currentBufferSize);
    if (d_growFlag && d_currentBufferSize < 1024) {
        d_currentBufferSize *= 2;
    }
}

void testBlobBufferFactory::setGrowFlag(bool growFlag)
{
    d_growFlag = growFlag;
}

bsl::size_t testBlobBufferFactory::currentBufferSize() const
{
    return d_currentBufferSize;
}

bool testBlobBufferFactory::growFlag() const
{
    return d_growFlag;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
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
        // Iterate over a set of test vectors varying in buffer size and length
        // of data to write.  For each test vector, instantiate a
        // 'bdlbb::PooledBlobBufferFactory', 'mF', allocate a 'bdlbb::Blob',
        // 'mC', and use 'mC' to instantiate a 'bdlbb::BlobStreamBuf', 'mX'.
        // Write the specified number of bytes to 'mX' using 'sputn', and
        // verify the length of 'mC'.  Read the specified number of buffers
        // from 'mX', and verify the result, and the get area offset of 'mX'.
        //
        // Instantiate a 'bdlbb::PooledBlobBufferFactory', 'mF', allocate a
        // 'bdlbb::Blob', 'mC', and use 'mC' to instantiate a
        // 'bdlbb::BlobStreamBuf, 'mX'.  Iterate over an input buffer whose
        // length is more than the size of 'mC', but less than twice the size
        // of 'mC'.  On each iteration, write the input buffer into 'mX',
        // substituting the i'th character for EOF.  Read back the data form
        // 'mX', and verify the result.
        //
        // Testing:
        //   Concern: EOF is streamed correctly
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: EOF Is Streamed Correctly" << endl
                 << "==================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
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
            enum { k_DATA_SIZE = sizeof DATA / sizeof *DATA };

            const bsl::ios_base::seekdir  CUR     = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1    = bsl::ios_base::out;
            const bsl::ios_base::openmode IN1     = bsl::ios_base::in;
            const int EOF_VAL = bdlbb::InBlobStreamBuf::traits_type::eof();

            for (int i = 0; i < k_DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int DATA_LENGTH = DATA[i].d_dataLength;

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(BUFFER_SIZE); P(DATA_LENGTH);
                }

                testBlobBufferFactory fa(&ta, BUFFER_SIZE);
                fa.setGrowFlag(false);

                bdlbb::Blob blob(&fa, &ta);
                {
                    bdlbb::InBlobStreamBuf  in(&blob);
                    bdlbb::OutBlobStreamBuf out(&blob);
                    char *EOFS = (char *)ta.allocate(DATA_LENGTH);
                    bsl::memset(EOFS, EOF_VAL, DATA_LENGTH);

                    // Write out data.
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                            out.sputn(EOFS, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                 out.pubseekoff(0, CUR, OUT1));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH == blob.length());

                    // Read in data.
                    char *result = (char *)ta.allocate(DATA_LENGTH);
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                in.sgetn(result, DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, DATA_LENGTH ==
                                                   in.pubseekoff(0, CUR, IN1));
                    LOOP2_ASSERT(i, LINE, 0 == bsl::memcmp(EOFS,
                                                           result,
                                                           DATA_LENGTH));
                    ta.deallocate(EOFS);
                    ta.deallocate(result);
                }
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            enum {
                k_BUFFER_SIZE = 8,
                k_DATA_LENGTH = 12
            };
            ASSERT(k_DATA_LENGTH > k_BUFFER_SIZE);
            ASSERT(k_DATA_LENGTH < k_BUFFER_SIZE * 2);

            testBlobBufferFactory fa(&ta, k_BUFFER_SIZE);
            fa.setGrowFlag(false);

            char data[k_DATA_LENGTH];
            char result[k_DATA_LENGTH];
            bsl::memset(data, '*', k_DATA_LENGTH);

            const std::streamoff DATA_LENGTH_STREAMOFF = k_DATA_LENGTH;

            for (int i = 0; i < k_DATA_LENGTH; ++i) {
                bdlbb::Blob blob(&fa, &ta);
                {
                    bdlbb::InBlobStreamBuf  in(&blob);
                    bdlbb::OutBlobStreamBuf out(&blob);
                    data[i] = EOF_VAL;
                    LOOP_ASSERT(i, k_DATA_LENGTH ==
                                               out.sputn(data, k_DATA_LENGTH));
                    LOOP_ASSERT(i, DATA_LENGTH_STREAMOFF ==
                                                 out.pubseekoff(0, CUR, OUT1));
                    LOOP_ASSERT(i, k_DATA_LENGTH == blob.length());
                    LOOP_ASSERT(i, k_DATA_LENGTH == in.sgetn(result,
                                                           k_DATA_LENGTH));
                    LOOP_ASSERT(i, 0 == bsl::memcmp(data,
                                                    result,
                                                    k_DATA_LENGTH));
                    data[i] = '*';
                }
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
        //   'bdlbb::PooledBlobBufferFactory', 'mF', allocate a
        //   'bdlbb::Blob', 'mC', and use 'mC' to instantiate a
        //   'bdlbb::BlobStreamBuf', 'mX'.  Write the specified
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

        bslma::TestAllocator ta(veryVeryVerbose);
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
            enum { k_DATA_SIZE = sizeof DATA / sizeof *DATA };

            const bsl::ios_base::seekdir  CUR  = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1 = bsl::ios_base::out;
            const bsl::ios_base::openmode IN1  = bsl::ios_base::in;

            for (int i = 0; i < k_DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int k_BUFFER_SIZE = DATA[i].d_bufferSize;
                const int k_DATA_LENGTH = DATA[i].d_dataLength;

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(k_BUFFER_SIZE); P(k_DATA_LENGTH);
                }

                testBlobBufferFactory fa(&ta, k_BUFFER_SIZE);
                fa.setGrowFlag(false);

                bdlbb::Blob blob(&fa, &ta);
                {
                    bdlbb::InBlobStreamBuf  in(&blob);
                    bdlbb::OutBlobStreamBuf out(&blob);
                    char *HASHMARKS = (char *)ta.allocate(k_DATA_LENGTH);
                    bsl::memset(HASHMARKS, '#', k_DATA_LENGTH);

                    // Write out data.
                    LOOP2_ASSERT(i, LINE, k_DATA_LENGTH ==
                                          out.sputn(HASHMARKS, k_DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, k_DATA_LENGTH ==
                                                 out.pubseekoff(0, CUR, OUT1));
                    LOOP2_ASSERT(i, LINE, k_DATA_LENGTH == blob.length());

                    // Read in data.
                    char *result = (char *)ta.allocate(k_DATA_LENGTH);
                    LOOP2_ASSERT(i, LINE, k_DATA_LENGTH ==
                                              in.sgetn(result, k_DATA_LENGTH));
                    LOOP2_ASSERT(i, LINE, k_DATA_LENGTH ==
                                                   in.pubseekoff(0, CUR, IN1));
                    LOOP2_ASSERT(i, LINE, 0 == bsl::memcmp(HASHMARKS,
                                                           result,
                                                           k_DATA_LENGTH));
                    ta.deallocate(HASHMARKS);
                    ta.deallocate(result);
                }
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
        //   Create two modifiable 'bdlbb::Blob' objects, 'mCa'
        //   and 'mCb'.  Create a modifiable 'bdlbb::BlobStreamBuf'
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
        //   void reset(bdlbb::Blob *blob);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'reset' Function" << endl
                 << "========================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        if (verbose) cout << "\nTesting bdlbb::OutBlobStreamBuf." << endl;
        {
            enum {
                k_BUFFER_SIZE_A = 16,    // buffer size for factory "A"
                k_BUFFER_SIZE_B = 32,    // buffer size for factory "B"
                k_SEEK_OFFSET   = 37,    // arbitrary offset
                k_DATA_LENGTH   = 64     // amount of data to write to stream
            };

            testBlobBufferFactory factoryA(&ta, k_BUFFER_SIZE_A);
            factoryA.setGrowFlag(false);
            bdlbb::Blob mCaI(&factoryA, &ta); bdlbb::Blob* mCa = &mCaI;
            ASSERT(0 == mCa->length());
            ASSERT(0 == mCa->numBuffers());

            testBlobBufferFactory factoryB(&ta, k_BUFFER_SIZE_B);
            factoryB.setGrowFlag(false);
            bdlbb::Blob mCbI(&factoryB, &ta); bdlbb::Blob* mCb = &mCbI;
            ASSERT(0 == mCb->length());
            ASSERT(0 == mCb->numBuffers());

            const bsl::ios_base::seekdir  CUR  = bsl::ios_base::cur;
            const bsl::ios_base::openmode OUT1 = bsl::ios_base::out;

            const std::streamoff DATA_LENGTH_STREAMOFF = k_DATA_LENGTH;

            {
                bdlbb::OutBlobStreamBuf mX(mCa);
                const bdlbb::OutBlobStreamBuf& X = mX;
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, OUT1));

                const bsl::string HASHMARKS(k_DATA_LENGTH, '#');
                bsl::ostream      out(&mX);

                out << HASHMARKS << flush;

                ASSERT(DATA_LENGTH_STREAMOFF == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(k_DATA_LENGTH == mCa->length());
                ASSERT(4 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                mX.reset();
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(DATA_LENGTH_STREAMOFF == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(k_DATA_LENGTH == mCa->length());
                ASSERT(4 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                out << HASHMARKS << flush;

                ASSERT(2 * DATA_LENGTH_STREAMOFF ==
                       mX.pubseekoff(0, CUR, OUT1));
                ASSERT(8 == mCa->numBuffers());
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                mX.reset(mCb);
                ASSERT(X.data() != mCa);
                ASSERT(X.data() == mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(2 * k_DATA_LENGTH == mCa->length());
                ASSERT(8 == mCa->numBuffers());;
                ASSERT(0 == mCb->length());
                ASSERT(0 == mCb->numBuffers());

                out << HASHMARKS << flush;

                ASSERT(DATA_LENGTH_STREAMOFF == mX.pubseekoff(0, CUR, OUT1));
                ASSERT(2 * k_DATA_LENGTH == mCa->length());
                ASSERT(8 == mCa->numBuffers());;
                ASSERT(k_DATA_LENGTH == mCb->length());
                ASSERT(2 == mCb->numBuffers());

                mCbI.removeAll();

                ASSERT(0   == mCb->length());
                ASSERT(0   == mCb->numBuffers());
                ASSERT(mCb == X.data());

                mX.reset();

                ASSERT(mCa != X.data());
                ASSERT(mCb == X.data());
            }
        }

        if (verbose) cout << "\nTesting bdlbb::InBlobStreamBuf." << endl;
        {
            enum {
                k_BUFFER_SIZE_A = 16,  // buffer size for factory "A"
                k_BUFFER_SIZE_B = 32,  // buffer size for factory "B"
                k_SEEK_OFFSET   = 5    // arbitrary offset
            };

            testBlobBufferFactory factoryA(&ta, k_BUFFER_SIZE_A);
            factoryA.setGrowFlag(false);
            bdlbb::Blob mCaI(&factoryA, &ta); bdlbb::Blob* mCa = &mCaI;
            ASSERT(0 == mCa->length());
            ASSERT(0 == mCa->numBuffers());

            testBlobBufferFactory factoryB(&ta, k_BUFFER_SIZE_B);
            factoryB.setGrowFlag(false);
            bdlbb::Blob mCbI(&factoryB, &ta); bdlbb::Blob* mCb = &mCbI;
            ASSERT(0 == mCb->length());
            ASSERT(0 == mCb->numBuffers());

            const bsl::ios_base::seekdir  CUR = bsl::ios_base::cur;
            const bsl::ios_base::openmode IN1 = bsl::ios_base::in;

            const std::streamoff SEEK_OFFSET_STREAMOFF = k_SEEK_OFFSET;

            {
                mCa->setLength(10); bsl::memset(mCa->buffer(0).data(), 1, 10);
                mCb->setLength(10); bsl::memset(mCb->buffer(0).data(), 2, 10);

                bdlbb::InBlobStreamBuf mX(mCa);
                const bdlbb::InBlobStreamBuf& X = mX;
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));

                bsl::istream      in(&mX);

                ASSERT(SEEK_OFFSET_STREAMOFF ==
                       mX.pubseekpos(k_SEEK_OFFSET, IN1));
                char read;
                in >> read;
                ASSERT(1 == read);

                mX.reset();
                ASSERT(X.data() == mCa);
                ASSERT(X.data() != mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));

                in >> read;
                ASSERT(1 == read);

                mX.reset(mCb);
                ASSERT(X.data() != mCa);
                ASSERT(X.data() == mCb);
                ASSERT(0 == mX.pubseekoff(0, CUR, IN1));

                in >> read;
                ASSERT(2 == read);

                ASSERT(SEEK_OFFSET_STREAMOFF ==
                       mX.pubseekpos(k_SEEK_OFFSET, IN1));
                ASSERT(SEEK_OFFSET_STREAMOFF == mX.pubseekoff(0, CUR, IN1));
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 5: {
      } break;
      case 4: {
      } break;
      case 3: {
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   * That it is possible to instantiate a
        //     'bdlbb::BlobStreamBuf' object with a variety of
        //     'bdlbb::Blob' parameters.
        //
        //   * That 'sync' and 'overflow' update the chain length.
        //
        // Plan:
        //   Iterate over a set of test vectors varying in buffer size.  For
        //   each test vector, instantiate a modifiable
        //   'bdlbb::BlobStreamBuf', 'mX', and a non-modifiable
        //   reference to 'mX' named 'X'.  Write data of length 3 times the
        //   specified buffer size to 'mX' in chunks of buffer size, 'sync'ing
        //   'mX' after each write, and verifying the chain length and number
        //   of buffers using 'X'.
        //
        // Testing:
        //   bdlbb::BlobStreamBuf(bdlbb::Blob *blob);
        //   ~bdlbb::BlobStreamBuf();
        //   int_type overflow(int_type c =
        //                                 bsl::streambuf::traits_type::eof());
        //   int sync();
        //   const bdlbb::Blob *data() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
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
            enum { k_DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < k_DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int k_BUFFER_SIZE = DATA[i].d_bufferSize;

                testBlobBufferFactory fa(&ta, k_BUFFER_SIZE);
                fa.setGrowFlag(false);

                bdlbb::Blob blob(&fa, &ta);
                bdlbb::OutBlobStreamBuf mX(&blob);
                const bdlbb::OutBlobStreamBuf&         X = mX;
                const bsl::string  HASHMARKS(k_BUFFER_SIZE, '#');

                if (verbose) {
                    P_(i); P_(LINE); P(k_BUFFER_SIZE);
                }

                enum { k_NUM_ITERATIONS = 3 };
                for (int j = 0; j < k_NUM_ITERATIONS; ++j) {
                    const int PRE_PUT_LENGTH   = j * k_BUFFER_SIZE;
                    const int POST_PUT_LENGTH  = PRE_PUT_LENGTH + 1;
                    const int POST_SYNC_LENGTH = (j + 1) * k_BUFFER_SIZE;

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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout << "\nTesting bdlbb::InBlobStreamBuf." << endl;
        {
            typedef bdlbb::InBlobStreamBuf Obj;

            enum { k_MAX_k_BUFFER_SIZE = 20 };
            for(int i = 0; i < k_MAX_k_BUFFER_SIZE; ++i) {
                const bsl::size_t  k_BUFFER_SIZE = i + 1;

                testBlobBufferFactory fa(&ta, k_BUFFER_SIZE);
                //fa.setGrowFlag(false);

                bdlbb::Blob blob(&fa, &ta);
                {
                    Obj mX(&blob); const Obj&    X = mX;
                    bsl::istream stream(&mX);
                    ASSERT(stream.rdbuf() == &mX);

                    if (verbose) {
                        P_(i); P(k_BUFFER_SIZE);
                    }

                    {
                        int j;
                        ASSERT(!(stream >> j));
                        ASSERT(!stream);
                        stream.clear();
                    }

                    {
                        int posInBuf = 0;
                        int currentBuf = 0;
                        blob.setLength(k_MAX_k_BUFFER_SIZE);
                        for (int j = 0; j < k_MAX_k_BUFFER_SIZE; ++j) {
                            if (posInBuf == blob.buffer(currentBuf).size()) {
                                ++currentBuf;
                                posInBuf = 0;
                            }
                            *(blob.buffer(currentBuf).data() + posInBuf) =
                                                                       'A' + j;
                            if (veryVerbose) {
                                bsl::cout << "Wrote " << j << " at offset "
                                          << posInBuf << " in buffer "
                                          << currentBuf << bsl::endl;
                            }
                            ++posInBuf;
                        }

                    }

                    {
                        int j = 0;
                        char c;
                        while (stream >> c) {
                            LOOP_ASSERT(j, c == 'A' + j);
                            if (verbose) {
                                T_; P_(c); P(j);
                            }
                            ASSERT(stream.rdbuf() == &mX);
                            ASSERT(stream.unget());
                            if (j) {
                                ASSERT(stream.unget());
                                ASSERT(stream >> c)
                                LOOP_ASSERT(j, c == 'A' + j - 1);
                            }
                            else {
                                ASSERT(!stream.unget());
                                stream.clear();
                            }
                            ASSERT(stream >> c)
                            LOOP_ASSERT(j, c == 'A' + j);
                            ++j;

                        }
                        ASSERT(k_MAX_k_BUFFER_SIZE == j);
                    }

                    ASSERT(!stream);
                    stream.clear();
                    stream.seekg(0);
                    for (int j = 0; j < k_MAX_k_BUFFER_SIZE; ++j) {
                        for (int k = 0; k < k_MAX_k_BUFFER_SIZE; ++k) {
                            char c;
                            stream.seekg(j);
                            ASSERT(stream >> c);
                            LOOP2_ASSERT(j, k, c == j + 'A');
                            stream.seekg(k);
                            ASSERT(stream >> c);
                            LOOP2_ASSERT(j, k, c == k + 'A');
                        }
                    }

                    bsl::string str;
                    ASSERT(!(stream >> str));
                    stream.clear();
                    stream.seekg(0);
                    ASSERT(stream >> str);
                    ASSERT(k_MAX_k_BUFFER_SIZE == str.length());
                    ASSERT('A' == str[0]);
                    ASSERT('A' + k_MAX_k_BUFFER_SIZE - 1 ==
                                                 str[k_MAX_k_BUFFER_SIZE - 1]);
                }
            }
        }
        if (verbose) cout << "\nTesting bdlbb::OutBlobStreamBuf." << endl;
        {
            enum { k_MAX_k_BUFFER_SIZE = 20 };
            for(int i = 0; i < k_MAX_k_BUFFER_SIZE; ++i) {
                const bsl::size_t  k_BUFFER_SIZE = i + 1;

                testBlobBufferFactory fa(&ta, k_BUFFER_SIZE);
                //fa.setGrowFlag(false);

                bdlbb::Blob blob(&fa, &ta);
                {
                    bdlbb::OutBlobStreamBuf outbuf(&blob);
                    bsl::ostream ostream(&outbuf);
                    bdlbb::InBlobStreamBuf inbuf(&blob);
                    bsl::istream istream(&inbuf);

                    if (verbose) {
                        P_(i); P(k_BUFFER_SIZE);
                    }

                    ostream << 12345;
                    ostream.flush();

                    int j;
                    istream >> j;
                    LOOP_ASSERT(i, 12345 == j);
                    if (verbose) {
                        T_; P_(i); P(j);
                    }

                    istream.clear();
                    istream.seekg(0);

                    int k;
                    istream >> k;
                    LOOP_ASSERT(i, j == k);
                    if (verbose) {
                        T_; P_(i); P_(j); P(k);
                    }

                    istream.clear();
                    ostream.seekp(0);
                    istream.seekg(0);

                    bsl::string value;
                    ostream << 654321 << bsl::flush;
                    istream >> value;
                    LOOP_ASSERT(i, "654321" == value);

                    istream.clear();
                    ostream.seekp(0);
                    istream.seekg(0);

                    // Since we cannot truncate the streambuf, we must write at
                    // least as many bytes as already exist in the buffer.

                    int length = bsl::max(3 * (int)k_BUFFER_SIZE,
                                          blob.length());
                    const bsl::string HASHMARKS(length, '#');
                    bsl::string       result;
                    ostream << HASHMARKS << bsl::flush;
                    istream >> result;
                    LOOP_ASSERT(i, HASHMARKS == result);
                    if (verbose) {
                        T_; P_(i);
                        P_(HASHMARKS.length()); P(result.length());
                    }
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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
// Copyright 2018 Bloomberg Finance L.P.
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
