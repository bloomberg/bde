// bdlsb_overflowmemoutput.t.cpp                                      -*-C++-*-
#include <bdlsb_overflowmemoutput.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_testallocatormonitor.h>         // for testing only

#include <bslx_genericoutstream.h>              // for testing only

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the public methods from the 'basic_streambuf'
// protocol that are implemented by the class 'bdlsb::OverflowMemOutput', as
// well as each public method in the 'bdlsb::OverflowMemOutput' class that is
// not part of the 'basic_streambuf' protocol.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the
// standard as described in the function documentation, where the standard
// defines behavior only loosely.  For those methods that are not protocol
// defined, we check only compliance with the behavior as described in the
// function documentation.
//
/// Primary Constructors:
//: o OverflowMemOutput(char             *buffer,
//:                     bsl::size_t       length,
//:                     bslma::Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//: o int_type sputc(char_type);
//: o pos_type pubseekoff(off_type, bsl::ios_base::beg, bsl::ios_base::out);
//
// Basic accessors:
//: o bsl::size_t dataLength() const;
//: o bsl::size_t dataLengthInInitialBuffer() const;
//: o bsl::size_t dataLengthInOverflowBuffer() const;
//: o const char *initialBuffer() const;
//: o bsl::size_t initialBufferSize() const;
//: o const char *overflowBuffer() const;
//: o bsl::size_t overflowBufferSize() const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] OverflowMemOutput(char             *buffer,
//                        size_t            length,
//                        bslma::Allocator *basicAllocator = 0);
// [ 2] ~OverflowMemOutput();
//
// PRIVATE MANIPULATORS
// [ 9] void grow(size_t, bool);
//
// MANIPULATORS
// [ 7] pos_type pubseekoff(off_type, seekdir, openmode);
// [ 7] pos_type pubseekpos(pos_type, openmode);
// [ 8] OverflowMemOutput *pubsetbuf(char *buffer, bsl::streamsize length);
// [ 5] int_type sputc(char_type);
// [ 6] streamsize sputn(const char *, streamsize);
//
// ACCESSORS
// [ 4] size_t dataLength() const;
// [ 4] size_t dataLengthInInitialBuffer() const;
// [ 4] size_t dataLengthInOverflowBuffer() const;
// [ 4] const char *initialBuffer() const;
// [ 4] size_t initialBufferSize() const;
// [ 4] const char *overflowBuffer() const;
// [ 4] size_t overflowBufferSize() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP: int_type sputc(char_type);
// [ 2] BOOTSTRAP: pos_type pubseekoff(off_type,
//                                     bsl::ios_base::beg,
//                                     bsl::ios_base::out);
// [ 3] TEST APPARATUS
// [10] USAGE EXAMPLE

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsb::OverflowMemOutput  Obj;

const bsl::size_t   INIT_BUFSIZE = 20;

// The aliases for types and constants used in seeking/positioning tests
typedef bsl::ios_base::openmode io_openmode;
typedef bsl::ios_base::seekdir  io_seekdir;

const io_openmode PUT = bsl::ios_base::out;
const io_openmode GET = bsl::ios_base::in;
const io_seekdir  CUR = bsl::ios_base::cur;
const io_seekdir  BEG = bsl::ios_base::beg;
const io_seekdir  END = bsl::ios_base::end;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

              // =======================================
              // operator<< for bdlsb::OverflowMemOutput
              // =======================================

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const bdlsb::OverflowMemOutput& streamBuffer)
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.
{
    bsl::size_t  len  = streamBuffer.dataLengthInInitialBuffer();
    const char  *data = streamBuffer.initialBuffer();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    stream << "\nInitial Buffer:";
    for (bsl::size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << ((data[i] >> j) & 0x01);
    }

    len   = streamBuffer.dataLengthInOverflowBuffer();
    data  = streamBuffer.overflowBuffer();

    stream << "\nOverflow Buffer:";

    for (bsl::size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << ((data[i] >> j) & 0x01);
    }
    stream.flags(flags); // reset stream format flags
    return stream;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, and replace 'assert' with
        //:   'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlsb::OverflowMemOutput'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates instantiating a template,
// 'bslx::GenericOutStream', on a 'bdlsb::OverflowMemOutput' object and using
// the 'bslx::GenericOutStream' object to stream out some data.
//
// First, we create a stream buffer, 'streamBuf', and supply it stack allocated
// memory as its initial buffer:
//..
    enum { k_STREAMBUF_CAPACITY = 8 };

    char                     buffer[k_STREAMBUF_CAPACITY];
    bdlsb::OverflowMemOutput streamBuf(buffer, k_STREAMBUF_CAPACITY);
//..
// Then, we create an instance of 'bslx::GenericOutStream' using 'streamBuf',
// with an arbitrary value for its 'versionSelector', and serialize some data:
//..
    bslx::GenericOutStream<bdlsb::OverflowMemOutput> outStream(&streamBuf,
                                                               20150707);
    int MAGIC = 0x1812;
    outStream.putInt32(MAGIC);
    outStream.putInt32(MAGIC+1);
//..
// Next, we verify that the data was correctly serialized and completely filled
// initial buffer supplied at the stream buffer construction:
//..
    ASSERT(outStream.isValid());
    ASSERT(8 == streamBuf.dataLength());
    ASSERT(0 == bsl::memcmp(streamBuf.initialBuffer(),
                            "\x00\x00\x18\x12\x00\x00\x18\x13",
                            8));
    ASSERT(0 == bsl::memcmp(buffer, "\x00\x00\x18\x12\x00\x00\x18\x13", 8));
    ASSERT(0 == streamBuf.overflowBuffer());
    ASSERT(0 == streamBuf.overflowBufferSize());
//..
// Then, we serialize some more data to trigger allocation of the internal
// overflow buffer:
//..
    outStream.putString(bsl::string("test"));
//..
// Finally, we verify that the additional data was serialized correctly and
// landed into dynamically allocated overflow buffer:
//..
    ASSERT(outStream.isValid());
    ASSERT(13 == streamBuf.dataLength());
    ASSERT(0  != streamBuf.overflowBuffer());
    ASSERT(5  == streamBuf.dataLengthInOverflowBuffer());
    ASSERT(0  == bsl::memcmp(streamBuf.overflowBuffer(), "\x04test", 5));
//..
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'grow' METHOD
        //
        //   Various public methods of the 'bdlsb::OverflowMemOutput' can lead
        //   to internal grow of the internal overflow memory buffer.
        //
        // Concerns:
        //: 1 On any operation that writes/positions past initial buffer
        //:   capacity, the overflow buffer should be allocated ( if is does
        //:   not exist ) and sized appropriately.
        //:
        //: 2 When the overflow buffer grows, the new overflow buffer should
        //:   have double capacity, the data from the old overflow buffer
        //:   should be copied over and the old overflow buffer should be
        //:   deallocated.
        //
        // Plan:
        //: 1 Create a stream buffer at various different states.  For each
        //:   state, invoke methods that can lead to internal grow of the
        //:   overflow bufffer.  Verify that growth size and data is correct.
        //:   The methods that can trigger internal overflow memory buffer grow
        //:   grow are:
        //:   1 'sputc'
        //:   2 'sputn'
        //:   3 'pubseekoff'
        //:   4 'pubseekpos'
        //
        // Testing:
        //   void grow(size_t, bool);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'grow' METHOD" << endl
                          << "=====================" << endl;

        bslma::TestAllocator oa(veryVeryVeryVerbose);
        {
            if (verbose) cout << "\nTesting grow." << endl;

            bsl::size_t end      = INIT_BUFSIZE*4;
            const char  filler[] = "This is the buffer that is put into the "
                                   "stream.  As long as this is larger than "
                                   "4x the initial buffer size then we are "
                                   "good. 0123456789 0123456789 0123456789";
            ASSERT(INIT_BUFSIZE*4 < strlen(filler));

            for (bsl::size_t i = 0; i < end; ++i) {
                // Testing 'sputc' grow.

                char buffer[INIT_BUFSIZE];

                Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj& SB = mSB;

                ASSERT(0 == SB.overflowBufferSize());
                ASSERT(0 == SB.overflowBuffer());

                mSB.pubseekpos(i);

                if (SB.dataLength() <= INIT_BUFSIZE) {
                    ASSERT(0 == SB.overflowBufferSize());
                    ASSERT(0 == SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                    ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0            != SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                    ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }
                else {
                    ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }

                for (bsl::size_t j = i; j < end; ++j) {
                    mSB.sputc(filler[j]);

                    if (SB.dataLength() <= INIT_BUFSIZE) {
                        ASSERT(0 == SB.overflowBufferSize());
                        ASSERT(0 == SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                        ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0            != SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                        ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                }
                ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                ASSERT(0 != SB.overflowBuffer());

                if (i <= SB.initialBufferSize()) {
                    ASSERT(0 == strncmp(filler + i,
                                        SB.initialBuffer() + i,
                                        SB.initialBufferSize() - i));
                    ASSERT(0 == strncmp(filler + SB.initialBufferSize(),
                                        SB.overflowBuffer(),
                                        end - SB.initialBufferSize()));
                }
                else {
                    ASSERT(0 == strncmp(filler + i,
                                        SB.overflowBuffer() + i -
                                                        SB.initialBufferSize(),
                                        end - i));
                }
            }

            for (bsl::size_t i = 0; i < end; ++i) {
                // Testing 'sputn' grow.

                char buffer[INIT_BUFSIZE];

                Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj& SB = mSB;

                ASSERT(0 == SB.overflowBufferSize());
                ASSERT(0 == SB.overflowBuffer());

                // Ignoring the return type.  Tested.
                mSB.pubseekpos(i);

                if (SB.dataLength() <= INIT_BUFSIZE) {
                    ASSERT(0 == SB.overflowBufferSize());
                    ASSERT(0 == SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                    ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0            != SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                    ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0              != SB.overflowBuffer());
                }
                else {
                    ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }

                mSB.sputn(filler + i, end - i);

                ASSERT(4*INIT_BUFSIZE <= SB.overflowBufferSize());
                ASSERT(0 != SB.overflowBuffer());

                if (i <= SB.initialBufferSize()) {
                    ASSERT(0 == strncmp(filler + i,
                                        SB.initialBuffer() + i,
                                        SB.initialBufferSize() - i));
                    ASSERT(0 == strncmp(filler + SB.initialBufferSize(),
                                        SB.overflowBuffer(),
                                        end - SB.initialBufferSize()));
                }
                else {
                    ASSERT(0 == strncmp(filler + i,
                                        SB.overflowBuffer() + i -
                                                        SB.initialBufferSize(),
                                        end - i));
                }
            }


            for (bsl::size_t i = 0; i < end; ++i) {
                // Testing 'pubseekoff' grow.

                char buffer[INIT_BUFSIZE];

                Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj& SB = mSB;

                ASSERT(0 == SB.overflowBufferSize());
                ASSERT(0 == SB.overflowBuffer());

                for (bsl::size_t j = 0; j < i; ++j) {
                    mSB.sputc('c');
                }

                if (SB.dataLength() <= INIT_BUFSIZE) {
                    ASSERT(0 == SB.overflowBufferSize());
                    ASSERT(0 == SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                    ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0            != SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                    ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }
                else {
                    ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }

                for (bsl::size_t j = i; j < end; ++j) {
                    mSB.pubseekoff(j, BEG, PUT);

                    if (SB.dataLength() <= INIT_BUFSIZE) {
                        ASSERT(0 == SB.overflowBufferSize());
                        ASSERT(0 == SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                        ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0            != SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                        ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                }
                ASSERT(4*INIT_BUFSIZE <= SB.overflowBufferSize());
                ASSERT(0 != SB.overflowBuffer());
            }

            for (bsl::size_t i = 0; i < end; ++i) {
                // Testing 'pubseekpos' grow.

                char buffer[INIT_BUFSIZE];

                Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj& SB = mSB;

                ASSERT(0 == SB.overflowBufferSize());
                ASSERT(0 == SB.overflowBuffer());

                for (bsl::size_t j = 0; j < i; ++j) {
                    mSB.sputc('c');
                }

                if (SB.dataLength() <= INIT_BUFSIZE) {
                    ASSERT(0 == SB.overflowBufferSize());
                    ASSERT(0 == SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                    ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0            != SB.overflowBuffer());
                }
                else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                    ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }
                else {
                    ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERT(0 != SB.overflowBuffer());
                }
                for (bsl::size_t j = i; j < end; ++j) {
                    mSB.pubseekpos(j);

                    if (SB.dataLength() <= INIT_BUFSIZE) {
                        ASSERT(0 == SB.overflowBufferSize());
                        ASSERT(0 == SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 2*INIT_BUFSIZE) {
                        ASSERT(INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0            != SB.overflowBuffer());
                    }
                    else if (SB.dataLength() <= 3*INIT_BUFSIZE) {
                        ASSERT(2*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERT(0 != SB.overflowBuffer());
                    }
                }
                ASSERT(4*INIT_BUFSIZE <= SB.overflowBufferSize());
                ASSERT(0 != SB.overflowBuffer());
            }
        }
        ASSERT(0 <  oa.numAllocations());
        ASSERT(0 == oa.numBytesInUse());

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'pubsetbuf' METHOD
        //
        // Concerns:
        //: 1 Call to the 'pubsetbuf' has no effect.
        //
        // Plan:
        //: 1 Call the 'pubsetbuf' method and ensure that it has no effect.
        //:   (C-1)
        //
        // Testing:
        // OverflowMemOutput *pubsetbuf(char *buffer, bsl::streamsize length);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'pubsetbuf' METHOD" << endl
                          << "==========================" << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            ASSERT(buffer       == SB.initialBuffer());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());

            char       new_buffer[2];

            mSB.pubsetbuf(new_buffer, 2);

            ASSERT(buffer       == SB.initialBuffer());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
        }
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //
        // Concerns:
        //: 1 Seeking is correct for:
        //:   - all relative positions.
        //:   - positive, 0, and negative values.
        //:   - into initial/overflow buffer.
        //:
        //: 2 Seeking beyond total capacity grow the stream buffer.
        //:
        //: 3 Seeking into "get" area has no effect.
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-1..3)
        //
        // Testing:
        //   pos_type pubseekoff(off_type, seekdir, openmode);
        //   pos_type pubseekpos(pos_type, openmode);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'seek' METHODS" << endl
                          << "======================" << endl;

        bslma::TestAllocator oa(veryVeryVeryVerbose);

        if (verbose) { cout << "\nTesting pubseekoff." << endl;}
        {
            Obj::off_type startRangeBeg = 0;
            Obj::off_type startRangeEnd = INIT_BUFSIZE * 4;

            Obj::off_type offsetRangeEnd =  INIT_BUFSIZE * 4;
            Obj::off_type offsetRangeBeg = -offsetRangeEnd;

            if (veryVeryVerbose) { T_ P_(startRangeBeg)  P(startRangeEnd) }
            if (veryVeryVerbose) { T_ P_(offsetRangeBeg) P(offsetRangeEnd) }
            for (Obj::off_type start = startRangeBeg;
                 start < startRangeEnd;
                 ++start) {
                for (Obj::off_type offset = offsetRangeBeg;
                     offset < offsetRangeEnd;
                     ++offset) {

                    if (veryVeryVerbose) { T_ P_(start) P(offset) }

                    char buffer[INIT_BUFSIZE];

                    {
                        Obj mSB(buffer, INIT_BUFSIZE, &oa);

                        // Initialize start position.

                        for (Obj::off_type i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        // Test from the beginning.

                        Obj::pos_type ret = mSB.pubseekoff(offset, BEG, PUT);

                        Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                             mSB.dataLength());
                        if (0 > offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == dataLength);
                        } else {
                            ASSERT(offset == ret);
                            ASSERT(dataLength == ret);
                        }
                    }

                    {
                        Obj mSB(buffer, INIT_BUFSIZE, &oa);

                        // Initialize start position.

                        for (Obj::off_type i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        // Test from the current position.

                        Obj::pos_type ret = mSB.pubseekoff(offset, CUR, PUT);

                        Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                             mSB.dataLength());
                        if (0 > start + offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == dataLength);
                        }
                        else {
                            ASSERT(start + offset == ret);
                            ASSERT(dataLength == ret);
                        }
                    }

                    {
                        Obj mSB(buffer, INIT_BUFSIZE, &oa);

                        // Initialize start position.

                        for (Obj::off_type i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        Obj::off_type capacity = mSB.initialBufferSize()
                                               + mSB.overflowBufferSize();

                        // Test from the end.

                        Obj::pos_type ret = mSB.pubseekoff(offset, END, PUT);

                        Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                             mSB.dataLength());
                        if (0 > capacity + offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == dataLength);
                        }
                        else {
                            ASSERT(capacity + offset == ret);
                            ASSERT(dataLength == ret);
                        }
                    }

                    {
                        Obj mSB(buffer, INIT_BUFSIZE, &oa);

                        // Initialize start position.

                        for (Obj::off_type i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        Obj::off_type capacity = mSB.initialBufferSize()
                                               + mSB.overflowBufferSize();

                        // Test from the end.

                        Obj::pos_type ret = mSB.pubseekoff(offset, END, PUT);

                        Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                             mSB.dataLength());
                        if (0 > capacity + offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == dataLength);
                        }
                        else {
                            ASSERT(capacity + offset == ret);
                            ASSERT(dataLength == ret);
                        }
                    }

                    // Seeking into get area should always fail.
                    {
                        Obj  mSB(buffer, INIT_BUFSIZE, &oa);

                        for (Obj::off_type i = 0; i < start; ++i ) {
                            mSB.sputc('A');
                        }

                        ASSERT(-1 == mSB.pubseekoff(offset, CUR, GET));
                        ASSERT(-1 == mSB.pubseekoff(offset, BEG, GET));
                        ASSERT(-1 == mSB.pubseekoff(offset, END, GET));
                    }
                }
            }
        }

        if (verbose) { cout << "\nTesting pubseekpos." << endl;}
        {

            Obj::off_type offsetRangeEnd = INIT_BUFSIZE * 4;
            Obj::off_type offsetRangeBeg = -offsetRangeEnd;

            for (Obj::off_type offset = offsetRangeBeg;
                 offset < offsetRangeEnd;
                 ++offset) {

                char buffer[INIT_BUFSIZE];

                if (veryVeryVerbose) {
                    T_ P(offset);
                }
                {
                    Obj  mSB(buffer, INIT_BUFSIZE, &oa);

                    Obj::pos_type ret = mSB.pubseekpos(offset, PUT);

                    Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                   mSB.dataLength());

                    if (0 > offset) {
                        ASSERTV(offset, -1 == ret);
                        ASSERTV(offset, 0  == dataLength);
                    } else {
                        ASSERTV(offset, offset     == ret);
                        ASSERTV(offset, dataLength == ret);
                    }
                }

                // Testing 'pubseekpos' with default parameters.
                {
                    Obj  mSB(buffer, INIT_BUFSIZE, &oa);

                    Obj::pos_type ret = mSB.pubseekpos(offset);

                    Obj::pos_type dataLength = static_cast<Obj::pos_type>(
                                                   mSB.dataLength());

                    if (0 > offset) {
                        ASSERTV(offset, -1 == ret);
                        ASSERTV(offset, 0  == dataLength);
                    } else {
                        ASSERTV(offset, offset     == ret);
                        ASSERTV(offset, dataLength == ret);
                    }
                }

                // Seeking into get area should always fail.
                if (veryVeryVerbose) {
                    cout << "\tTesting pubseekpos with get area."
                         << endl;
                }
                {
                    Obj  mSB(buffer, INIT_BUFSIZE, &oa);

                    Obj::pos_type ret = mSB.pubseekpos(offset, GET);

                    ASSERT(-1 == ret);
                }
            }
        }
        ASSERT(0 <  oa.numAllocations());
        ASSERT(0 == oa.numBytesInUse());

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'sputn' METHOD
        //
        // Concerns:
        //: 1 Ensure 'sputn' correctly writes string of varying length.
        //:
        //: 2 Ensure that no more than the specified number of characters are
        //:   written.
        //:
        //: 3 Ensure that 'sputn' correctly uses current write position
        //:   indicator.
        //:
        //: 4 Ensure that overflow buffer relocations triggered by 'sputn' do
        //:   not corrupt existing buffer content.
        //
        // Plan:
        //: 1 Write out representative strings from the categories 0
        //:   characters, 1 character, and > 1 character, into stream buffer
        //:   with representative contents "empty", substantially less than
        //:   capacity, almost-full-so-that-next-write-exceeds-capacity and
        //:   substantially more than current capacity.  (C-1..3)
        //:
        //: 2 After the whole sequence has been output into the stream buffer,
        //:   verify that the content is intact by internal overflow buffer
        //:   relocations.  (C-4)
        //
        // Testing:
        //   streamsize sputn(const char *, streamsize);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'sputn' METHOD" << endl
                          << "======================" << endl;

        bslma::TestAllocator oa(veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting sputn without seeks." << endl;
        {
            const bsl::size_t DATA[] = { 0,
                                         1,
                                         2,
                                         INIT_BUFSIZE/2,
                                         INIT_BUFSIZE - 1,
                                         INIT_BUFSIZE,
                                         INIT_BUFSIZE + 1,
                                         INIT_BUFSIZE*2 - 1,
                                         INIT_BUFSIZE*2,
                                         INIT_BUFSIZE*2 + 1,
                                         INIT_BUFSIZE*3 - 1,
                                         INIT_BUFSIZE*3,
                                         INIT_BUFSIZE*3 + 1,
                                         INIT_BUFSIZE*4 - 1,
                                         INIT_BUFSIZE*4,
                                         INIT_BUFSIZE*4 + 1,
                                         INIT_BUFSIZE*8 - 1,
                                         INIT_BUFSIZE*8,
                                         INIT_BUFSIZE*8 + 1};

            const bsl::size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            const char        SPUTC_FILLER = 'a';
            const char        SPUTN_FILLER = 'b';

            // This test builds a stream buffer with a content of the form
            // "aaaaa...abbbbb...", where the 'b' string is output using
            // 'sputn'.  The test verifies that the 'b' string start at the
            // correct position, has correct length.

            // This segment verifies correct behavior across different initial
            // buffer states.

            for(size_t i = 0; i < DATA_LEN; ++i ) {


                for(size_t j = 0; j < DATA_LEN; ++j ) {

                    const bsl::size_t SPUTC_FILLER_LENGTH = DATA[i];
                    const bsl::size_t SPUTN_FILLER_LENGTH = DATA[j];
                    const bsl::size_t TOTAL_LENGTH = SPUTC_FILLER_LENGTH
                                                     + SPUTN_FILLER_LENGTH;

                    if (veryVeryVerbose) {
                        T_ P_(SPUTC_FILLER_LENGTH) P(SPUTN_FILLER_LENGTH)
                    }

                    char buffer[INIT_BUFSIZE+1];
                    memset(buffer, 'Z', INIT_BUFSIZE+1);

                    char *sputnFiller = new char[SPUTN_FILLER_LENGTH];
                    memset(sputnFiller, SPUTN_FILLER, SPUTN_FILLER_LENGTH);

                    Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                    const Obj& SB = mSB;

                    for(size_t k = 0; k < SPUTC_FILLER_LENGTH; ++k ) {
                        mSB.sputc(SPUTC_FILLER);
                    }

                    bsl::streamsize ret = mSB.sputn(sputnFiller,
                                                    SPUTN_FILLER_LENGTH);

                    ASSERTV(i, j, SPUTN_FILLER_LENGTH ==
                                                static_cast<bsl::size_t>(ret));
                    ASSERTV(i, j, TOTAL_LENGTH == SB.dataLength());
                    ASSERTV(i, j, TOTAL_LENGTH ==
                                            SB.dataLengthInInitialBuffer()
                                            + SB.dataLengthInOverflowBuffer());

                    // Checking no writes past the initial buffer capacity.
                    ASSERTV(i, j, 'Z' == buffer[INIT_BUFSIZE]);

                    // Checking each character and the filler border position
                    bsl::size_t checkPos    = 0;
                    char        checkFiller = SPUTC_FILLER;
                    const char* checkBuffer = buffer;

                    while (checkPos < TOTAL_LENGTH) {
                        if (checkPos >= SPUTC_FILLER_LENGTH) {
                            checkFiller = SPUTN_FILLER;
                        }

                        if ( checkPos < INIT_BUFSIZE ) {
                            ASSERTV(checkPos, checkFiller ==
                                                        checkBuffer[checkPos]);
                        } else {
                            checkBuffer = SB.overflowBuffer();
                            ASSERTV(checkPos, checkBuffer);
                            ASSERTV(checkPos, checkFiller ==
                                         checkBuffer[checkPos - INIT_BUFSIZE]);
                        }

                        ++checkPos;
                    }

                    delete[] sputnFiller;
                }
            }
        }

        if (verbose) cout << "\nTesting sputn with seeks." << endl;
        {
            const bsl::size_t DATA[] = { 0,
                                         1,
                                         2,
                                         INIT_BUFSIZE/2,
                                         INIT_BUFSIZE - 1,
                                         INIT_BUFSIZE,
                                         INIT_BUFSIZE + 1,
                                         INIT_BUFSIZE*2 - 1,
                                         INIT_BUFSIZE*2,
                                         INIT_BUFSIZE*2 + 1,
                                         INIT_BUFSIZE*3 - 1,
                                         INIT_BUFSIZE*3,
                                         INIT_BUFSIZE*3 + 1,
                                         INIT_BUFSIZE*4 - 1,
                                         INIT_BUFSIZE*4,
                                         INIT_BUFSIZE*4 + 1,
                                         INIT_BUFSIZE*8 - 1,
                                         INIT_BUFSIZE*8,
                                         INIT_BUFSIZE*8 + 1};

            const bsl::size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            const char        SPUTN_FILLER = 'b';

            // This segment verifies correct behavior across different initial
            // write position indicators.

            for(size_t i = 0; i < DATA_LEN; ++i ) {

                for(size_t j = 0; j < DATA_LEN; ++j ) {

                    const bsl::size_t SPUTN_START_POS = DATA[i];
                    const bsl::size_t SPUTN_FILLER_LENGTH = DATA[j];
                    const bsl::size_t TOTAL_LENGTH = SPUTN_START_POS
                                                     + SPUTN_FILLER_LENGTH;

                    if (veryVeryVerbose) {
                        T_ P_(SPUTN_START_POS) P(SPUTN_FILLER_LENGTH)
                    }

                    char buffer[INIT_BUFSIZE+1];
                    memset(buffer, 'Z', INIT_BUFSIZE+1);

                    char *sputnFiller = new char[SPUTN_FILLER_LENGTH];
                    memset(sputnFiller, SPUTN_FILLER, SPUTN_FILLER_LENGTH);

                    Obj        mSB(buffer, INIT_BUFSIZE, &oa);
                    const Obj& SB = mSB;

                    const Obj::off_type startOffset =
                                   static_cast<Obj::off_type>(SPUTN_START_POS);

                    Obj::pos_type finalPos = mSB.pubseekoff(startOffset,
                                                            BEG,
                                                            PUT);
                    ASSERTV(i, j, startOffset == finalPos);

                    bsl::streamsize ret = mSB.sputn(sputnFiller,
                                                    SPUTN_FILLER_LENGTH);

                    ASSERTV(i, j, SPUTN_FILLER_LENGTH ==
                                                static_cast<bsl::size_t>(ret));

                    ASSERTV(i, j, TOTAL_LENGTH == SB.dataLength());
                    ASSERTV(i, j, TOTAL_LENGTH ==
                                            SB.dataLengthInInitialBuffer()
                                            + SB.dataLengthInOverflowBuffer());

                    // Checking no writes past the initial buffer capacity.
                    ASSERTV(i, j, 'Z' == buffer[INIT_BUFSIZE]);

                    // Checking each character and the filler border position
                    bsl::size_t checkPos    = SPUTN_START_POS;
                    char        checkFiller = SPUTN_FILLER;
                    const char* checkBuffer = buffer;

                    while (checkPos < TOTAL_LENGTH) {
                        if ( checkPos < INIT_BUFSIZE ) {
                            ASSERTV(checkPos, checkFiller ==
                                                        checkBuffer[checkPos]);
                        } else {
                            checkBuffer = SB.overflowBuffer();
                            ASSERTV(checkPos, checkBuffer);
                            ASSERTV(checkPos, checkFiller ==
                                         checkBuffer[checkPos - INIT_BUFSIZE]);
                        }

                        ++checkPos;
                    }

                    delete[] sputnFiller;
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char      buffer[INIT_BUFSIZE];
            Obj       mSB(buffer, INIT_BUFSIZE, &oa);
            memset(buffer, 'Z', INIT_BUFSIZE);

            ASSERT_SAFE_FAIL(mSB.sputn(0, 1));
            ASSERT_SAFE_FAIL(mSB.sputn(buffer, -1));
            ASSERT_SAFE_PASS(mSB.sputn(0, 0));
            ASSERT_SAFE_PASS(mSB.sputn(buffer, 1));
        }

        ASSERT(0 <  oa.numAllocations());
        ASSERT(0 == oa.numBytesInUse());
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'sputc' METHOD
        //
        // Concerns:
        //: 1 Ensure 'sputc' correctly places the specified character into
        //:   the stream buffer (both initially supplied by the client and
        //:   dynamically allocated overflow buffer).
        //:
        //: 2 Ensure that 'sputc' correctly uses current write position
        //:   indicator.
        //:
        //: 3 Ensure that overflow buffer relocations triggered by 'sputc' do
        //:   not corrupt existing buffer content.
        //
        // Plan:
        //: 1 Sequentially output a controlled sequence of characters one at a
        //:   time and verify that observable state change accordingly.  i
        //:   (C-1..2)
        //:
        //: 2 After the whole sequence has been output into the stream buffer,
        //:   verify that the content is intact by internal overflow buffer
        //:   relocations.  (C-3)
        //
        // Testing:
        //   int_type sputc(char_type);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'sputc' METHOD" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'sputc'." << endl;
        {
            char buffer[INIT_BUFSIZE+1];
            memset(buffer, 'Z', INIT_BUFSIZE + 1);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            char ch = 'b';
            char prevCh = 'a';

            for (bsl::size_t i = 0; i < INIT_BUFSIZE*5 + 1; ++i) {
                ASSERTV(i, i     == SB.dataLength());

                // Note that we check the previous character in each loop.
                mSB.sputc(ch);

                ASSERTV(i, i + 1 == SB.dataLength());

                if ( INIT_BUFSIZE > i ) {
                    ASSERTV(i, ch    == buffer[i]);
                    ASSERTV(i, 'Z'   == buffer[i + 1]);

                    if (i > 0) {
                        ASSERTV(i, prevCh  == buffer[i-1]);
                    }
                } else {
                    ASSERTV(i, i - INIT_BUFSIZE + 1 ==
                                              SB.dataLengthInOverflowBuffer());
                    const char* overflowBuffer = SB.overflowBuffer();
                    ASSERTV(i, 0  != overflowBuffer);
                    ASSERTV(i, ch == overflowBuffer[i-INIT_BUFSIZE]);
                    if ( INIT_BUFSIZE + 1 < i ) {
                        ASSERTV(i, prevCh ==
                                           overflowBuffer[i-INIT_BUFSIZE - 1]);
                    }
                }
                prevCh = ch;
                ch = static_cast<char> ('a' + (ch -'a' + 1)%26);
            }

            // Walking the whole stream buffer, verifying that the content is
            // intact after all overflow and relocations.
            ch = 'b';
            prevCh = 'a';

            for (bsl::size_t i = 0; i < INIT_BUFSIZE*5 + 1; ++i) {
                if ( INIT_BUFSIZE > i ) {
                    ASSERTV(i, ch == buffer[i]);
                    if (i > 0) {
                        ASSERTV(i, prevCh  == buffer[i-1]);
                    }
                } else {
                    const char* overflowBuffer = SB.overflowBuffer();
                    ASSERTV(i, 0  != overflowBuffer);
                    ASSERTV(i, ch == overflowBuffer[i-INIT_BUFSIZE]);
                    if ( INIT_BUFSIZE + 1 < i ) {
                        ASSERTV(i, prevCh ==
                                           overflowBuffer[i-INIT_BUFSIZE - 1]);
                    }
                }
                prevCh = ch;
                ch = static_cast<char> ('a' + (ch -'a' + 1)%26);
            }
        }

      } break;


      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors functionality.
        //
        // Concerns:
        //: 1 Ensure that basic accessors return valid, expected values.
        //:
        //
        // Plan:
        //: 1 Put stream buffer into various states using 'sputc' and
        //:   'pubseekoff' methods and verify that accessors report correct
        //:   values.  (C-1)
        //
        // Testing:
        //   size_t dataLength() const;
        //   size_t dataLengthInInitialBuffer() const;
        //   size_t dataLengthInOverflowBuffer() const;
        //   const char *initialBuffer() const;
        //   size_t initialBufferSize() const;
        //   const char *overflowBuffer() const;
        //   size_t overflowBufferSize() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting accessors with 'sputc'" << endl;
        {
            bslma::TestAllocator oa(veryVeryVeryVerbose);

            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE, &oa);
            const Obj& SB = mSB;

            for (bsl::size_t i = 0; i < INIT_BUFSIZE; ++i) {
                ASSERTV(i, i            == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, i            == SB.dataLengthInInitialBuffer());

                ASSERTV(i, 0            == SB.overflowBufferSize());
                ASSERTV(i, 0            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, 0            == SB.overflowBuffer());
                ASSERTV(i, 0            == oa.numAllocations());
                ASSERTV(i, 0            == oa.numBytesInUse());
                mSB.sputc('a');
            }
            if(veryVeryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting accessors with overflow." << endl;
        {
            bslma::TestAllocator oa(veryVeryVeryVerbose);

            char         buffer[INIT_BUFSIZE];
            Obj          mSB(buffer, INIT_BUFSIZE, &oa);
            const Obj&   SB = mSB;

            for (bsl::size_t i = 0; i < INIT_BUFSIZE; ++i) {
                mSB.sputc('a');
            }

            ASSERT(0 == oa.numAllocations());
            ASSERT(0 == oa.numBytesInUse());

            ASSERT(INIT_BUFSIZE == SB.dataLength());

            ASSERT(buffer       == SB.initialBuffer());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
            ASSERT(INIT_BUFSIZE == SB.dataLengthInInitialBuffer());
            ASSERT(0            == SB.overflowBuffer());
            ASSERT(0            == SB.dataLengthInOverflowBuffer());

            for (bsl::size_t i = 0; i < INIT_BUFSIZE*4+1; ++i) {
                ASSERTV(i, INIT_BUFSIZE + i == SB.dataLength());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, INIT_BUFSIZE == SB.dataLengthInInitialBuffer());

                ASSERTV(i, i == SB.dataLengthInOverflowBuffer());

                mSB.sputc('a');

                ASSERTV(i, 0 < oa.numAllocations());
                ASSERTV(i, oa.lastAllocatedNumBytes() ==
                                                      SB.overflowBufferSize());
                ASSERTV(i, oa.lastAllocatedAddress() == SB.overflowBuffer());
            }
        }

        // Seek into wide range of absolute positions and verify accessors
        // validity.  Triggers overflow.
        if (verbose) cout << "\nTesting accessors with 'pubseekoff'." << endl;
        {
            for (bsl::size_t i = 0; i < INIT_BUFSIZE*4+1; ++i) {
                bslma::TestAllocator oa(veryVeryVeryVerbose);

                char         buffer[INIT_BUFSIZE];
                Obj          mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj&   SB = mSB;

                ASSERTV(i, 0            == SB.dataLength());
                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, 0            == SB.dataLengthInInitialBuffer());
                ASSERTV(i, 0            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, 0 == oa.numAllocations());
                ASSERTV(i, 0 == oa.numBytesInUse());

                mSB.pubseekoff(i, BEG, PUT);

                ASSERTV(i, i            == SB.dataLength());
                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());

                if ( INIT_BUFSIZE >= i ) {
                    ASSERTV(i, i == SB.dataLengthInInitialBuffer());
                    ASSERTV(i, 0 == SB.dataLengthInOverflowBuffer());
                    ASSERTV(i, 0 == oa.numAllocations());
                    ASSERTV(i, 0 == SB.overflowBuffer());
                    ASSERTV(i, 0 == SB.overflowBufferSize());
                } else {
                    ASSERTV(i, INIT_BUFSIZE     ==
                                               SB.dataLengthInInitialBuffer());
                    ASSERTV(i, i - INIT_BUFSIZE ==
                                              SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, 0 < oa.numAllocations());
                    ASSERTV(i, oa.lastAllocatedNumBytes() ==
                                                      SB.overflowBufferSize());
                    ASSERTV(i, oa.lastAllocatedAddress()  ==
                                                          SB.overflowBuffer());
                }
            }
        }

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        //   Verify the auxiliary function used in the test driver.  The
        //   function is used to print out the content of the
        //   'bdlsb::OverflowMemOutput' in a human-readable form.  Note that
        //   the tested function is not part of the component and use only to
        //   provide human readable test traces.
        //
        // Concerns:
        //: 1 Output operator formats the stream buffer correctly.
        //:
        //: 2 Output operator does not produce any trailing characters.
        //:
        //: 3 Output operator works on references to 'const' object.
        //:
        //: 4 Output operator returns a reference to the modifiable stream
        //:   argument.
        //
        // Plan:
        //: 1 For each of a small representative set of object values use
        //:   'stringstream' to write that object's value to two separate
        //:   strings.  Compare the contents of these strings with the literal
        //:   expected output format and verify that they are equal.  (C-1..3)
        //:
        //: 2 Create a 'bdlsb::OverflowMemOutput' object.  Use 'ostrstream' to
        //:   write that object's value and some characters in consecutive
        //:   order.  (C-4)
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nChecking operator<< return value." << endl;
        {
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB << "next";   // Ensure modifiable
            out2 << SB  << "next";   // stream is returned.
        }

        if (verbose) cout << "\nChecking operator<< output." << endl;
        {

            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            // No intermediate checks are done.  'sputc' bootstrap already
            // verified correct behavior.
            mSB.sputc('h');
            mSB.sputc('e');
            mSB.sputc('l');
            mSB.sputc('l');
            mSB.sputc('o');

            stringstream out1;
            stringstream out2;

            out1 << mSB;
            out2 << SB;

            const char *EXPECTED =
                "\nInitial Buffer:"
                "\n0000\t01101000 01100101 01101100 01101100 01101111"
                "\nOverflow Buffer:";

            if (veryVerbose) { T_ P(EXPECTED) T_ P(out1.str()) }

            ASSERT(out1.str() == out2.str());
            ASSERT(EXPECTED   == out1.str());
        }

        if (verbose) cout << "\nChecking operator<< output (overflow)."
                          << endl;
        {
            // One more test, to see overflow buffer in output
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');
            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');
            const char *EXPECTED =
                                 "\nInitial Buffer:"
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010 "
                                         "00110000 00110001 00110010 00110011"
                                 "\n0010\t00110100 00110101 00110110 00110111"
                                 "\nOverflow Buffer:"
                                 "\n0000\t00111000 00111001 01000001 01000010";

            stringstream out;
            out << SB;

            if (veryVerbose) { T_ P(EXPECTED) T_ P(out.str()) }

            ASSERT(EXPECTED == out.str());
        }

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        //   Ensure that we can create an object and bring it to a well known
        //   state for subsequent tests. This test case also bootstrap 'sputc'
        //   method (via direct observation of the client supplied buffer) and
        //   a number of accessors.
        //
        // Concerns:
        //: 1 An object created with a value constructor (with or without
        //:   a supplied allocator) has expected state.
        //:
        //: 2 If an allocator is not supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator is supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 QoI: The value constructor allocates no memory.
        //:
        //:11 'sputc' correctly write characters into the client supplied
        //:   buffer and advances position indicator.
        //:
        //:12 QoI: Asserted precondition violations are detected when enabled.
        //:
        //:13 'pubseekoff' correctly changes write position indicator when
        //:   invoked with a selected parameters.
        //
        // Plan:
        //: 1 Construct 3 distinct objects with different configuration: (a)
        //:   without passing an allocator, (b) passing a null allocator
        //:   address explicitely, and (c) passing the address of a test
        //:   allocator distinct from the default.  For each object
        //:   instantiation: (C-1..8)
        //:
        //:   1 Create distinct 'bdsma::TestAllocator' objects and install
        //:     one as the current default allocator (note that an unique
        //:     test allocator is already installed as the global allocator).
        //:
        //:   2 Use value constructor to create an object "mSB', with its
        //:     object allocator configured appropriately. (C-1..5)
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the value constructor.  (C-10)
        //:
        //:   4 Use (yet untested) 'sputc' method to trigger memory allocation
        //:     and verify that memory comes from the correct allocator.  (C-6)
        //:
        //:   5 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 2 Write series of characters to the value-constructed stream buffer
        //:   using 'sputc' ( without causing overflow ) and verify that all
        //:   character values is written correctly and the write position
        //:   indicator is advanced.  (C-11)
        //:
        //: 3 Write enough characters to verify that the specified initial
        //:   buffer is completely usable, and then write one more to ensure
        //:   that the stream buffer allocates additional memory from the
        //:   supplied allocator.  (C-6)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-12)
        //:
        //: 5 Change the write position indicator with a selected parameters
        //:   ( offset from the beginning of the stream buffer, in the put
        //:   area ), write the character at the current position and verify
        //:   that the character landed into expected location.  (C-13)
        //
        // Testing:
        //   OverflowMemOutput(char             *buffer,
        //                     size_t            length,
        //                     bslma::Allocator *basicAllocator = 0);
        //   ~OverflowMemOutput();
        //   BOOTSTRAP: int_type sputc(char_type);
        //   BOOTSTRAP: pos_type pubseekoff(off_type,
        //                                  bsl::ios_base::beg,
        //                                  bsl::ios_base::out);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting 'sputc' (bootstrap)." << endl;
        {
            // This test verifies that 'sputc' at minimum:
            //    1. Adds the character to the specified buffer.
            //    2. Does not write beyond the write position..
            //    3. Character is not altered by the 'sputc'.
            //    4. On overflow, the 'sputc' writes to the overflow buffer.

            if (veryVerbose) { T_ cout << "Testing different character values."
                                   << endl; }

            char ch = -128;
            for (int i = 0; i < 256; ++i, ++ch ) {
                char buffer[2];
                memset(buffer, 'Z', 2);

                Obj mSB(buffer, 2);

                mSB.sputc(ch);

                ASSERTV(i, ch  == buffer[0]);
                ASSERTV(i, 'Z' == buffer[1]);
            }
        }
        {
            if (veryVerbose) { T_ cout << "Testing 'sputc' advances position."
                                   << endl; }

            char buffer[INIT_BUFSIZE+1];
            memset(buffer, 'Z', INIT_BUFSIZE + 1);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            char        ch = 'a';
            bsl::size_t i  = 0;
            for (; i < INIT_BUFSIZE; ++i, ++ch) {
                mSB.sputc(ch);

                ASSERTV(i, ch  == buffer[i]);
                ASSERTV(i, 'Z' == buffer[i + 1]);

                if (i > 0) {
                    ASSERTV(i, (ch-1)  == buffer[i-1]);
                }
            }
            // Causing overflow and testing that initial buffer is not altered.
            // Note that we also testing that last character landed in the
            // overflow buffer.
            if (veryVerbose) { T_ cout << "Triggering overflow." << endl; }
            mSB.sputc(ch);
            ASSERTV(i, ch, (ch-1) == buffer[i-1]);
            ASSERTV(i, ch, 'Z'    == buffer[i]);
            ASSERTV(i, ch, SB.overflowBuffer());
            ASSERTV(i, ch, ch     == SB.overflowBuffer()[0]);
        }

        if (verbose) {
            cout << "\nTesting value constructor and destructor." << endl;
        }
        {
            // Value constructor with default allocator
            if (veryVerbose) cout << "\tTesting default allocator." << endl;

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&da);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);

                Obj         mSB(buffer, INIT_BUFSIZE);
                const Obj&  SB = mSB;

                // Using yet not fully tested 'sputc' to fill initial buffer
                // and one more causing memory allocation.  Using (yet
                // untested) accessors to observe object state (bootstapping).
                char        ch = 'a';
                bsl::size_t i  = 0;
                for (; i < INIT_BUFSIZE+1; ++i, ++ch) {
                    ASSERTV(i, i            == SB.dataLength());

                    ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                    ASSERTV(i, i            == SB.dataLengthInInitialBuffer());

                    ASSERTV(i, 0       == SB.overflowBufferSize());
                    ASSERTV(i, 0       == SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, buffer  == SB.initialBuffer());
                    ASSERTV(i, 0       == SB.overflowBuffer());

                    ASSERTV(i, 0       == da.numAllocations());
                    mSB.sputc(ch);
                }

                // Last 'sputc' cause memory allocation.
                ASSERTV(i, INIT_BUFSIZE + 1 == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, INIT_BUFSIZE == SB.dataLengthInInitialBuffer());

                ASSERTV(i, INIT_BUFSIZE == SB.overflowBufferSize());
                ASSERTV(i, 1            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, 0            != SB.overflowBuffer());

                ASSERTV(i, 1            == da.numAllocations());
                ASSERTV(i, INIT_BUFSIZE == da.numBytesInUse());

                ASSERTV(i, da.lastAllocatedAddress() == SB.overflowBuffer());
            }

            // Verify that all memory is released on object destruction.
            ASSERTV(da.numBlocksInUse(),  0 ==  da.numBlocksInUse());
        }

        {
            // Value constructor with explicit default allocator
            if (veryVerbose) {
                T_ cout << "Testing explicit default allocator." << endl;
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&da);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);

                Obj         mSB(buffer, INIT_BUFSIZE, 0);
                const Obj&  SB = mSB;

                // Using yet not fully tested 'sputc' to fill initial buffer
                // and one more causing memory allocation.  Using (yet
                // untested) accessors to observe object state (bootstapping).
                char        ch = 'a';
                bsl::size_t i  = 0;
                for (; i < INIT_BUFSIZE+1; ++i, ++ch) {
                    ASSERTV(i, i            == SB.dataLength());

                    ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                    ASSERTV(i, i            == SB.dataLengthInInitialBuffer());

                    ASSERTV(i, 0       == SB.overflowBufferSize());
                    ASSERTV(i, 0       == SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, buffer  == SB.initialBuffer());
                    ASSERTV(i, 0       == SB.overflowBuffer());

                    ASSERTV(i, 0       == da.numAllocations());
                    mSB.sputc(ch);
                }

                // Last 'sputc' cause memory allocation.
                ASSERTV(i, INIT_BUFSIZE + 1 == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, INIT_BUFSIZE == SB.dataLengthInInitialBuffer());

                ASSERTV(i, INIT_BUFSIZE == SB.overflowBufferSize());
                ASSERTV(i, 1            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, 0            != SB.overflowBuffer());

                ASSERTV(i, 1            == da.numAllocations());
                ASSERTV(i, INIT_BUFSIZE == da.numBytesInUse());

                ASSERTV(i, da.lastAllocatedAddress() == SB.overflowBuffer());
            }

            // Verify that all memory is released on object destruction.
            ASSERTV(da.numBlocksInUse(),  0 ==  da.numBlocksInUse());
        }

        {
            // Value constructor with object allocator
            if (veryVerbose) {
                T_ cout << "Testing object allocator." << endl;
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            {
                bslma::DefaultAllocatorGuard guard(&da);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);

                Obj         mSB(buffer, INIT_BUFSIZE, &oa);
                const Obj&  SB = mSB;

                // Using yet not fully tested 'sputc' to fill initial buffer
                // and one more causing memory allocation.  Using (yet
                // untested) accessors to observe object state (bootstapping).
                char        ch = 'a';
                bsl::size_t i  = 0;
                for (; i < INIT_BUFSIZE+1; ++i, ++ch) {
                    ASSERTV(i, i            == SB.dataLength());

                    ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                    ASSERTV(i, i            == SB.dataLengthInInitialBuffer());

                    ASSERTV(i, 0       == SB.overflowBufferSize());
                    ASSERTV(i, 0       == SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, buffer  == SB.initialBuffer());
                    ASSERTV(i, 0       == SB.overflowBuffer());

                    ASSERTV(i, 0       == da.numAllocations());
                    ASSERTV(i, 0       == oa.numAllocations());
                    mSB.sputc(ch);
                }

                // Last 'sputc' cause memory allocation
                ASSERTV(i, INIT_BUFSIZE + 1 == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, INIT_BUFSIZE == SB.dataLengthInInitialBuffer());

                ASSERTV(i, INIT_BUFSIZE == SB.overflowBufferSize());
                ASSERTV(i, 1            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, 0            != SB.overflowBuffer());

                ASSERTV(i, 0            == da.numAllocations());
                ASSERTV(i, 1            == oa.numAllocations());
                ASSERTV(i, INIT_BUFSIZE == oa.numBytesInUse());

                ASSERTV(i, oa.lastAllocatedAddress() == SB.overflowBuffer());
            }

            // Verify that all memory is released on object destruction.
            ASSERTV(da.numBlocksInUse(),  0 ==  da.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting 'pubseekoff' (bootstrap)."
                          << endl;
        {
            if (veryVerbose) {
                T_ cout << "Testing 'pubseekoff' from beginning."
                        << endl;
            }

            char        ch = 'a';
            bsl::size_t i  = 0;
            while ( i < INIT_BUFSIZE*4+1) {

                char buffer[INIT_BUFSIZE+1];
                memset(buffer, 'Z', INIT_BUFSIZE + 1);

                Obj        mSB(buffer, INIT_BUFSIZE);
                const Obj& SB = mSB;

                Obj::off_type iToOff   = static_cast<Obj::off_type>(i);

                Obj::pos_type finalPos = mSB.pubseekoff(iToOff, BEG, PUT);

                ASSERTV(i, iToOff == finalPos);

                ASSERTV(i, i == SB.dataLength());

                if ( i <= INIT_BUFSIZE ) {
                    ASSERTV(i, i == SB.dataLengthInInitialBuffer());
                    ASSERTV(i, 0 == SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                    ASSERTV(i, 0            == SB.overflowBufferSize());

                    ASSERTV(i, buffer       == SB.initialBuffer());
                    ASSERTV(i, 0            == SB.overflowBuffer());
                    ASSERTV(i, 'Z'          == buffer[i]);
                } else {
                    ASSERTV(i, i == SB.dataLengthInInitialBuffer()
                                    + SB.dataLengthInOverflowBuffer());

                    ASSERTV(i, INIT_BUFSIZE     ==
                                               SB.dataLengthInInitialBuffer());
                    ASSERTV(i, i - INIT_BUFSIZE ==
                                              SB.dataLengthInOverflowBuffer());
                    ASSERTV(i, INIT_BUFSIZE     == SB.initialBufferSize());
                    ASSERTV(i, buffer           == SB.initialBuffer());
                    ASSERTV(i, SB.overflowBuffer());
                }

                // Using 'sputc' to output character and directly observing
                // the client supplied buffer for the character position.
                mSB.sputc(ch);

                if ( i < INIT_BUFSIZE ) {
                    ASSERTV(i, ch  == buffer[i]);
                    ASSERTV(i, 'Z' == buffer[i + 1]);

                    if (i > 0) {
                        ASSERTV(i, 'Z' == buffer[i-1]);
                    }
                } else {
                    ASSERTV(i, SB.overflowBuffer());
                    const char *checkBuffer = SB.overflowBuffer();
                    // In overflow buffer we can test only current position.
                    ASSERTV(i, ch  == checkBuffer[i - INIT_BUFSIZE]);
                }

                i+=1;
            }
        }

        if (verbose) cout << "\nTesting exception neutrality." << endl;
        {
            const bsl::size_t DATA[] = { 0,
                                         1,
                                         2,
                                         INIT_BUFSIZE/2,
                                         INIT_BUFSIZE - 1,
                                         INIT_BUFSIZE,
                                         INIT_BUFSIZE + 1,
                                         INIT_BUFSIZE*2 - 1,
                                         INIT_BUFSIZE*2,
                                         INIT_BUFSIZE*2 + 1,
                                         INIT_BUFSIZE*3 - 1,
                                         INIT_BUFSIZE*3,
                                         INIT_BUFSIZE*3 + 1,
                                         INIT_BUFSIZE*4 - 1,
                                         INIT_BUFSIZE*4,
                                         INIT_BUFSIZE*4 + 1};

            const bsl::size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // stream buffer states.

            for(size_t i = 0; i < DATA_LEN; ++i ) {
                bsl::size_t INITIAL_DATA_LEN = DATA[i];

                bslma::TestAllocator oa("exception test", veryVeryVeryVerbose);
                {
                    char buffer[INIT_BUFSIZE+1];
                    memset(buffer, 'Z', INIT_BUFSIZE + 1);

                    Obj        mSB(buffer, INIT_BUFSIZE, &oa);

                    // Pre-fill stream buffer with some data.  This might also
                    // trigger some initial memory allocation.
                    for (bsl::size_t j = 0; j < INITIAL_DATA_LEN; ++j) {
                        mSB.sputc('a');
                    }

                    int numIterations = 0;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        bslma::TestAllocatorMonitor monitor(&oa);

                        if (veryVeryVerbose) {
                            P_(INITIAL_DATA_LEN) P(numIterations)
                        }

                        ++numIterations;

                        // Add characters until the overflow buffer is created/
                        // grows.  This will trigger an exception on at least
                        // one iteration.

                        while (!monitor.isTotalUp()) {
                            mSB.sputc('a');
                        }

                        // sanity check only, does not establish exception
                        // neutrality: appending characters does allocate
                        // memory.

                        ASSERTV(numIterations, monitor.isTotalUp());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(i, 2 == numIterations);
                }

                // Test exception neutrality: all memory has been returned.
                ASSERTV(i, 0 == oa.numBlocksInUse());
            }
        }


        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char buffer[1];
            ASSERT_SAFE_FAIL(Obj(     0,  0, &ta));
            ASSERT_SAFE_FAIL(Obj(     0,  1, &ta));
            ASSERT_SAFE_FAIL(Obj(buffer,  0, &ta));
            ASSERT_SAFE_PASS(Obj(buffer,  1, &ta));
        }
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nMake sure we can create and use a "
                          << "'bdlsb::OverflowMemOutput'."
                          << endl;

        {
            char buffer[INIT_BUFSIZE];

            if (verbose) { cout << "\nCreating an object." << endl; }

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            ASSERT(0            == SB.dataLength());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
            ASSERT(0            == SB.overflowBufferSize());

            // Do not change the length of this string.  Our overflow buffer
            // grow checks depend on it
            //               01234567890123456789012345678901234
            char source[] = "Hello, this is initial test string.";

            if (verbose) {
                cout << "\nWrite a single char to the stream buffer." << endl;
            }

            mSB.sputc(source[0]);
            ASSERT(1            == SB.dataLength());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
            ASSERT(1            == SB.dataLengthInInitialBuffer());
            ASSERT(0            == strncmp(SB.initialBuffer(), source,
                                           SB.dataLength()));
            ASSERT(0            == SB.overflowBufferSize());
            ASSERT(0            == SB.dataLengthInOverflowBuffer());

            mSB.sputc(source[1]);

            ASSERT(2            == SB.dataLength());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
            ASSERT(2            == SB.dataLengthInInitialBuffer());
            ASSERT(0            == strncmp(SB.initialBuffer(), source,
                                           SB.dataLength()));
            ASSERT(0            == SB.overflowBufferSize());
            ASSERT(0            == SB.dataLengthInOverflowBuffer());


            if (verbose) {
                cout << "\nWrite multiple chars to the stream buffer." << endl;
            }

            mSB.sputn(source + SB.dataLength(),
                      INIT_BUFSIZE - SB.dataLength());

            ASSERT(INIT_BUFSIZE == SB.dataLength());
            ASSERT(INIT_BUFSIZE == SB.initialBufferSize());
            ASSERT(INIT_BUFSIZE == SB.dataLengthInInitialBuffer());
            ASSERT(0            == strncmp(SB.initialBuffer(), source,
                                           SB.dataLengthInInitialBuffer()));
            ASSERT(0            == SB.overflowBufferSize());
            ASSERT(0            == SB.dataLengthInOverflowBuffer());

            if (verbose) {
                cout << "\nTriggering stream buffer overflow." << endl;
            }
            mSB.sputn(source + SB.dataLength(),
                      strlen(source) - SB.dataLength());

            ASSERT(strlen(source) == SB.dataLength());
            ASSERT(INIT_BUFSIZE   == SB.initialBufferSize());
            ASSERT(INIT_BUFSIZE   == SB.dataLengthInInitialBuffer());
            ASSERT(0              == strncmp(SB.initialBuffer(), source,
                                             SB.dataLengthInInitialBuffer()));

            ASSERT(INIT_BUFSIZE   == SB.overflowBufferSize());
            ASSERT(strlen(source) - INIT_BUFSIZE ==
                                              SB.dataLengthInOverflowBuffer());
            ASSERT(0              == strncmp(SB.overflowBuffer(),
                                             source + INIT_BUFSIZE,
                                             SB.dataLengthInOverflowBuffer()));
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
