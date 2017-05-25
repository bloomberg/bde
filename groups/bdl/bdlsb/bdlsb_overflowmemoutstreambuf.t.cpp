// bdlsb_overflowmemoutstreambuf.t.cpp                                -*-C++-*-
#include <bdlsb_overflowmemoutstreambuf.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bslalg_typetraits.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>     // setw(), setfill()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>      // isdigit(), isupper(), islower()

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// This test driver exercises all the public methods from the 'basic_streambuf'
// protocol that are implemented by the class 'bdlsb::OverflowMemOutStreamBuf',
// as well as each public method in the 'bdlsb::OverflowMemOutStreamBuf' class
// that is not part of the 'basic_streambuf' protocol.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the
// standard as described in the function documentation, where the standard
// defined behavior only loosely.  For those methods that are not protocol
// defined, we check only compliance with the behavior as described in the
// function documentation.
//
/// Primary Constructors:
//: o OverflowMemOutStreamBuf(char *, int, bslma::Allocator *);
//
// Primary Manipulators:
//: o int_type sputc(char_type);
//: o pos_type seekoff(off_type, seekdir, openmode);
//
// Basic accessors:
//: o size_t dataLength() const;
//: o size_t dataLengthInInitialBuffer() const;
//: o size_t dataLengthInOverflowBuffer() const;
//: o const char *initialBuffer() const;
//: o size_t initialBufferSize() const;
//: o const char *overflowBuffer() const;
//: o size_t overflowBufferSize() const;
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] OverflowMemOutStreamBuf(char *, int, bslma::Allocator *);
// [ 2] ~OverflowMemOutStreamBuf();
//
// PRIVATE MANIPULATORS
// [ 7] void grow(size_t numBytes);
//
// MANIPULATORS
// [ 2] int_type overflow(int_type c = traits_type::eof());
// [ 2] pos_type seekoff(off_type, seekdir, openmode);
// [ 6] pos_type seekpos(pos_type, openmode);
// [ 5] streamsize xsputn(const char_type *, streamsize);
//
// ACCESSORS
// [ 4] size_t dataLength() const;
// [ 4] size_t dataLengthInInitialBuffer() const;
// [ 4] size_t dataLengthInOverflowBuffer() const;
// [ 4] size_t initialBufferSize() const;
// [ 4] size_t overflowBufferSize() const;
// [ 4] const char *initialBuffer() const;
// [ 4] const char *overflowBuffer() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS: os& operator<<(os&, const OverflowMemOutStrBuf&);
// [ 2] PROXY: int_type sputc(char_type);
// [ 5] PROXY: streamsize sputn(const char_type *, streamsize);
// [ 2] PROXY: pos_type pubseekoff(off_type, seekdir, openmode);
// [ 6] PROXY: pos_type pubseekpos(pos_type, openmode);
// [ 8] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdlsb::OverflowMemOutStreamBuf Obj;

const int INIT_BUFSIZE = 20;
const int DIBS         = INIT_BUFSIZE * 2; // double    INIT_BUFSIZE
const int TIBS         = INIT_BUFSIZE * 3; // triple    INIT_BUFSIZE
const int QIBS         = INIT_BUFSIZE * 4; // quadruple INIT_BUFSIZE

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

              // ================================================
              // operator<< for bdlsb::OverflowMemOutStreamBuf
              // ================================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const bdlsb::OverflowMemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const bdlsb::OverflowMemOutStreamBuf& streamBuffer)
{
    size_t      len  = streamBuffer.dataLengthInInitialBuffer();
    const char *data = streamBuffer.initialBuffer();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;

    stream << "\nInitial Buffer:";
    for (size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }

    len   = streamBuffer.dataLengthInOverflowBuffer();
    data  = streamBuffer.overflowBuffer();

    stream << "\nOverflow Buffer:";

    for (size_t i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }
    stream.flags(flags); // reset stream format flags
    return stream;
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
/// Example 1: Basic Use of 'bdlsb::OverflowMemOutStreamBuf'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'bdlsb::OverflowMemOutStreamBuf' in order
// to test a user defined stream type, 'CapitalizingStream'. In this example,
// we'll define a simple example stream type 'CapitalizingStream' that
// capitalizes lower-case ASCII data written to the stream. In order to test
// this 'CapitalizingStream' type, we'll create an instance, and supply it a
// 'bdlsb::OverflowMemOutStreamBuf' object as its stream buffer; after we write
// some character data to the 'CapitalizingStream' we'll inspect the buffer of
// the 'bdlsb::OverflowMemOutStreamBuf' and verify its contents match our
// expected output. Note that to simplify the example, we do not include the
// functions for streaming non-character data, e.g., numeric values.
//
// First, we define our example stream class, 'CapitalizingStream' (which we
// will later test using 'bdlsb::OverflowMemOutStreamBuf):
//..
    class CapitalizingStream {
        // This class capitalizes lower-case ASCII characters that are output.

        // DATA
        bsl::streambuf  *d_streamBuffer_p;   // pointer to a stream buffer

        // FRIENDS
        friend CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                              const char          *data);
      public:
        // CREATORS
        explicit CapitalizingStream(bsl::streambuf *streamBuffer);
            // Create a capitalizing stream using the specified 'streamBuffer'
            // as the underlying stream buffer for the stream.
    };

    // FREE OPERATORS
    CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                   const char          *data);
        // Write the specified 'data' in capitalized form to the specified
        // 'stream'.

    CapitalizingStream::CapitalizingStream(bsl::streambuf *streamBuffer)
    : d_streamBuffer_p(streamBuffer)
    {
    }
//..
// As is typical, the streaming operators are made friends of the class.
//
// Note that we cannot directly use 'bsl::toupper' to capitalize each
// individual character, because 'bsl::toupper' operates on 'int' instead of
// 'char'.  Instead, we call a function 'ucharToUpper' that works in terms of
// 'unsigned char'.  some care must be made to avoid undefined and
// implementation-specific behavior during the conversions to and from 'int'.
// Therefore we wrap 'bsl::toupper' in an interface that works in terms of
// 'unsigned char':
//..
    static unsigned char ucharToUpper(unsigned char input)
        // Return the upper-case equivalent to the specified 'input' character.
    {
        return static_cast<unsigned char>(bsl::toupper(input));
    }
//..
// Finally, we use the 'transform' algorithm to convert lower-case characters
// to upper-case.
//..
    // FREE OPERATORS
    CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                   const char          *data)
    {
        bsl::string tmp(data);
        bsl::transform(tmp.begin(),
                       tmp.end(),
                       tmp.begin(),
                       ucharToUpper);
        stream.d_streamBuffer_p->sputn(tmp.data(), tmp.length());
        return stream;
    }
//..

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)         veryVerbose;
    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
// Now, we create an instance of 'bdlsb::OverflowMemOutStreamBuf' that will
// serve as underlying stream buffer for our 'CapitalingStream':
//..
    enum { INITIAL_CAPACITY = 10 };
    char buffer[INITIAL_CAPACITY];

    bdlsb::OverflowMemOutStreamBuf streamBuffer(buffer, INITIAL_CAPACITY);
//..
// Now, we test our 'CapitalingStream' by supplying the created instance of
// 'bdlsb::OverflowMemOutStreamBuf' and using it to inspect the output of the
// stream:
//..
    CapitalizingStream  testStream(&streamBuffer);
    testStream << "Hello world.";
//..
// Finally, we verify that the streamed data has been capitalized and the
// portion of the data that does not fit into initial buffer is placed into
// dynamically allocated overflow buffer:
//..
    ASSERT(10 == streamBuffer.dataLengthInInitialBuffer());
    ASSERT(0  == strncmp("HELLO WORL",
                         streamBuffer.initialBuffer(),
                         streamBuffer.dataLengthInInitialBuffer()));
    ASSERT(2  == streamBuffer.dataLengthInOverflowBuffer());
    ASSERT(0  == strncmp("D.",
                         streamBuffer.overflowBuffer(),
                         streamBuffer.dataLengthInOverflowBuffer()));
//..
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // GROW TEST
        //   Various public methods of the 'bdlsb::OverflowMemOutStreamBuf' can
        //   lead to internal growth of the internal overflow memory buffer.
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
        //:   overflow buffer.  Verify that growth size and data is correct.
        //:   The methods that can trigger internal overflow memory buffer grow
        //:   grow are:
        //:   1 'sputc'
        //:   2 'sputn'
        //:   3 'pubseekoff'
        //
        // Testing:
        //   void grow(size_t numBytes);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "GROW TEST" << endl
                          << "=========" << endl;

        bslma::TestAllocator sputcTA(veryVeryVeryVerbose);
        bslma::TestAllocator sputnTA(veryVeryVeryVerbose);
        bslma::TestAllocator seekoffTA(veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting grow." << endl;

        size_t     end      = INIT_BUFSIZE * 4;
        const char filler[] = "This is the buffer that is put into the "
                              "stream.  As long as this is larger than "
                              "4x the initial buffer size then we are "
                              "good. 0123456789 0123456789 0123456789";
        ASSERT(INIT_BUFSIZE * 4 < strlen(filler));

        if (verbose) cout << "\tTesting sputc grow." << endl;
        {
            char buffer[INIT_BUFSIZE];

            Obj        mSB(buffer, INIT_BUFSIZE, &sputcTA);
            const Obj& SB = mSB;

            ASSERT(0 == SB.overflowBufferSize());
            ASSERT(0 == SB.overflowBuffer());

            for (size_t i = 0; i < end; ++i) {
                // Action
                mSB.sputc(filler[i]);

                 // Checking memory allocation
                if (SB.dataLength() <= INIT_BUFSIZE) {
                    ASSERTV(i, 0 == SB.overflowBufferSize());
                    ASSERTV(i, 0 == SB.overflowBuffer());
                    ASSERTV(i, 0 == sputcTA.numBytesInUse());
                }
                else if (SB.dataLength() <= DIBS) {
                    ASSERTV(i, 0            != SB.overflowBuffer());
                    ASSERTV(i, INIT_BUFSIZE == SB.overflowBufferSize());
                    ASSERTV(i, INIT_BUFSIZE == sputcTA.numBytesInUse());
                }
                else if (SB.dataLength() <= TIBS) {
                    ASSERTV(i, 0    != SB.overflowBuffer());
                    ASSERTV(i, DIBS == SB.overflowBufferSize());
                    ASSERTV(i, DIBS == sputcTA.numBytesInUse());
                }
                else {
                    ASSERTV(i, 0    != SB.overflowBuffer());
                    ASSERTV(i, QIBS == SB.overflowBufferSize());
                    ASSERTV(i, QIBS == sputcTA.numBytesInUse());
                }
            }
            ASSERT(0 != SB.overflowBuffer());
            ASSERT(QIBS == SB.overflowBufferSize());
            ASSERT(QIBS == sputcTA.numBytesInUse());

            // Checking data coping
            ASSERT(0 == strncmp(filler,
                                SB.initialBuffer(),
                                SB.initialBufferSize()));
            ASSERT(0 == strncmp(filler + SB.initialBufferSize(),
                                SB.overflowBuffer(),
                                end - SB.initialBufferSize()));
        }

        if (verbose) cout << "\tTesting seekoff grow." << endl;
        {
            for (size_t i = 0; i < end; ++i) {
                for (size_t j = i; j < end - i; ++j) {
                    if (veryVerbose) { T_ P_(i) P(j) }

                    char       buffer[INIT_BUFSIZE];
                    Obj        mSB(buffer, INIT_BUFSIZE, &seekoffTA);
                    const Obj& SB = mSB;

                    ASSERT(0 == SB.overflowBufferSize());
                    ASSERT(0 == SB.overflowBuffer());

                    // Position setting up
                    for (size_t k = 0; k < i; ++k) {
                        mSB.sputc('c');
                    }

                    // Action
                    mSB.pubseekoff(j, CUR, PUT);

                    // Checking memory allocation
                    if (SB.dataLength() <= INIT_BUFSIZE) {
                        ASSERTV(i, j, 0 == SB.overflowBufferSize());
                        ASSERTV(i, j, 0 == SB.overflowBuffer());
                        ASSERTV(i, j, 0 == seekoffTA.numBytesInUse());
                    }
                    else if (SB.dataLength() <= DIBS) {
                        ASSERTV(i, j, 0            != SB.overflowBuffer());
                        ASSERTV(i, j, INIT_BUFSIZE == SB.overflowBufferSize());
                        ASSERTV(i,
                                j,
                                INIT_BUFSIZE == seekoffTA.numBytesInUse());
                    }
                    else if (SB.dataLength() <= TIBS) {
                        ASSERTV(i, j, 0    != SB.overflowBuffer());
                        ASSERTV(i, j, DIBS == SB.overflowBufferSize());
                        ASSERTV(i, j, DIBS == seekoffTA.numBytesInUse());
                    }
                    else {
                        ASSERTV(i, j, 0    != SB.overflowBuffer());
                        ASSERTV(i, j, QIBS == SB.overflowBufferSize());
                        ASSERTV(i, j, QIBS == seekoffTA.numBytesInUse());
                    }
                }
            }
        }

        if (verbose) cout << "\tTesting sputn grow." << endl;
        {
            for (size_t i = 0; i < end; ++i) {

                if (veryVerbose) { T_ P(i) }

                char       buffer[INIT_BUFSIZE];
                Obj        mSB(buffer, INIT_BUFSIZE, &sputnTA);
                const Obj& SB = mSB;

                ASSERT(0 == SB.overflowBufferSize());
                ASSERT(0 == SB.overflowBuffer());

                // Set initial position
                mSB.pubseekpos(i, PUT);

                mSB.sputn(filler + i, end - i);

                // Checking memory allocation
                ASSERTV(i, 0                       != SB.overflowBuffer());
                ASSERTV(i, INIT_BUFSIZE * 4        == SB.overflowBufferSize());
                ASSERTV(i, static_cast<size_t>(sputnTA.numBytesInUse()) ==
                                                      SB.overflowBufferSize());

                if (i <= SB.initialBufferSize()) {
                    ASSERTV(i, 0 == strncmp(filler + i,
                                            SB.initialBuffer() + i,
                                            SB.initialBufferSize() - i));
                    ASSERTV(i, 0 == strncmp(filler + SB.initialBufferSize(),
                                            SB.overflowBuffer(),
                                            end - SB.initialBufferSize()));
                }
                else {
                    ASSERTV(i, 0 == strncmp(
                              filler + i,
                              SB.overflowBuffer() + i - SB.initialBufferSize(),
                              end - i));
                }
            }
        }

        ASSERT(0 <  sputcTA.numAllocations());
        ASSERT(0 == sputcTA.numBytesInUse());
        ASSERT(0 <  seekoffTA.numAllocations());
        ASSERT(0 == seekoffTA.numBytesInUse());
        ASSERT(0 <  sputnTA.numAllocations());
        ASSERT(0 == sputnTA.numBytesInUse());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // SEEKPOS TEST
        //   As the only action performed in 'seekpos' is the call for
        //   'seekoff' with predetermined second parameter, then we can test
        //   'seekpos' superficially.  Note that 'seekpos' method is called by
        //   the base class method 'pubseekpos'.
        //
        // Concerns:
        //: 1 Seeking uses the correct location from which to offset.
        //:
        //: 2 Both negative and positive offsets compute correctly.
        //:
        //: 3 Seeking sets the "cursor" (i.e., the base-class' pptr()) position
        //:   to the correct location.
        //:
        //: 4 Seeking out of bounds is handled correctly and returns invalid
        //:   value.
        //:
        //: 5 Trying to seek in the "get" area has no effect and returns
        //:   invalid value.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of offsets for
        //:   seek operations.
        //:
        //: 2 For each row 'R' in the table of P-2:
        //:
        //:   1 Create two identical 'bdlsb::OverflowMemOutStreamBuf' objects
        //:     and fill their buffers with the same content.
        //:
        //:   2 Perform 'seekpos' operation for one object and 'seekoff'
        //:     operation for another (reference sample) with specified offset.
        //:
        //:   3 Verify that two objects have the same state.  (C-1..5)
        //
        // Testing:
        //   pos_type seekpos(pos_type, openmode);
        //   PROXY: pos_type pubseekpos(pos_type, openmode);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEEKPOS TEST" << endl
                          << "============" << endl;

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            char buffer[INIT_BUFSIZE];

            static const struct {
                bsl::ios_base::openmode d_areaFlags;  // "put" or "get" area
                Obj::pos_type           d_amount;     // amount to seek
            } DATA[] = {
               //AREA  AMOUNT
               //----  ----------------
               { PUT,  -2               },
               { PUT,  0                },
               { PUT,  1                },
               { PUT,  INIT_BUFSIZE - 1 },
               { PUT,  INIT_BUFSIZE     },
               { PUT,  INIT_BUFSIZE + 1 },
               { PUT,  DIBS - 1         },
               { PUT,  DIBS             },
               { PUT,  DIBS + 1         },
               { PUT,  TIBS - 1         },
               { PUT,  TIBS             },
               { PUT,  TIBS + 1         },
               { PUT,  QIBS - 1         },
               { PUT,  QIBS             },
               { PUT,  QIBS + 1         },
               // seek into the 'get' area
               { GET,  22               }
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                Obj           mSBOff(buffer, INIT_BUFSIZE);
                Obj           mSBPos(buffer, INIT_BUFSIZE);
                const Obj&    SBOff = mSBOff;
                const Obj&    SBPos = mSBPos;
                Obj::pos_type retOff;
                Obj::pos_type retPos;

                mSBOff.pubseekoff(DATA[i].d_amount,
                                  BEG,
                                  DATA[i].d_areaFlags);
                mSBPos.pubseekoff(DATA[i].d_amount,
                                  BEG,
                                  DATA[i].d_areaFlags);

                for (size_t j = 0; j < DATA_LEN; ++j ) {
                    if (veryVerbose) {
                        T_ P_(DATA[i].d_amount) P(DATA[j].d_amount)
                    }

                    retOff = mSBOff.pubseekoff(DATA[j].d_amount,
                                               BEG,
                                               DATA[j].d_areaFlags);
                    retPos = mSBPos.pubseekpos(DATA[j].d_amount,
                                               DATA[j].d_areaFlags);

                    // Assert return values and new positions.
                    ASSERTV(DATA[i].d_amount,
                            DATA[j].d_amount,
                            retOff             == retPos);
                    ASSERTV(DATA[i].d_amount,
                            DATA[j].d_amount,
                            SBOff.dataLength() == SBPos.dataLength());
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // XSPUTN TEST
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
        //: 4 Ensure that overflow buffer re-locations triggered by 'sputn' do
        //:   not corrupt existing buffer content.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
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
        //:   re-locations.  (C-4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations with
        //:   invalid input parameters values (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-5)
        //
        // Testing:
        //   streamsize xsputn(const char_type *, streamsize);
        //   PROXY: streamsize sputn(const char_type *, streamsize);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "XSPUTN TEST" << endl
                          << "===========" << endl;

        bslma::TestAllocator               da(veryVeryVerbose);
        const bslma::DefaultAllocatorGuard dag(&da);

        {
            const size_t DATA[] = { 0,
                                    1,
                                    2,
                                    INIT_BUFSIZE / 2,
                                    INIT_BUFSIZE - 1,
                                    INIT_BUFSIZE,
                                    INIT_BUFSIZE + 1,
                                    DIBS - 1,
                                    DIBS,
                                    DIBS + 1,
                                    TIBS - 1,
                                    TIBS,
                                    TIBS + 1,
                                    QIBS - 1,
                                    QIBS,
                                    QIBS + 1};

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

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

                    const size_t SPUTC_FILLER_LENGTH = DATA[i];
                    const size_t SPUTN_FILLER_LENGTH = DATA[j];
                    const size_t TOTAL_LENGTH = SPUTC_FILLER_LENGTH +
                                                           SPUTN_FILLER_LENGTH;

                    if (veryVeryVerbose) {
                        T_ P_(SPUTC_FILLER_LENGTH) P(SPUTN_FILLER_LENGTH)
                    }

                    char buffer[INIT_BUFSIZE+1];
                    memset(buffer, 'Z', INIT_BUFSIZE+1);

                    char *sputnFiller = new char[SPUTN_FILLER_LENGTH];
                    memset(sputnFiller, SPUTN_FILLER, SPUTN_FILLER_LENGTH);

                    Obj        mSB(buffer, INIT_BUFSIZE);
                    const Obj& SB = mSB;

                    for(size_t k = 0; k < SPUTC_FILLER_LENGTH; ++k ) {
                        mSB.sputc(SPUTC_FILLER);
                    }

                    bsl::streamsize ret = mSB.sputn(sputnFiller,
                                                    SPUTN_FILLER_LENGTH);

                    ASSERTV(i, j, SPUTN_FILLER_LENGTH ==
                                                     static_cast<size_t>(ret));
                    ASSERTV(i, j, TOTAL_LENGTH == SB.dataLength());
                    ASSERTV(i, j, TOTAL_LENGTH ==
                                            SB.dataLengthInInitialBuffer()
                                            + SB.dataLengthInOverflowBuffer());

                    // Checking no writes past the initial buffer capacity.
                    ASSERTV(i, j, 'Z' == buffer[INIT_BUFSIZE]);

                    // Checking each character and the filler border position
                    size_t      checkPos    = 0;
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

        if (verbose) cout << "\tNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char buffer[INIT_BUFSIZE];
            Obj  mSB(buffer, INIT_BUFSIZE);

            ASSERT_SAFE_FAIL(mSB.sputn(0, 1));
            ASSERT_SAFE_FAIL(mSB.sputn(buffer, -1));
            ASSERT_SAFE_PASS(mSB.sputn(0, 0));
            ASSERT_SAFE_PASS(mSB.sputn(buffer, 0));
            ASSERT_SAFE_PASS(mSB.sputn(buffer, 1));
        }

        ASSERT(0 <  da.numAllocations());
        ASSERT(0 == da.numBytesInUse());
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
        //:   'pubseeoff' methods and verify that accessors report correct
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

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout << "\nTesting not overflown streambuf." << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE, &ta);
            const Obj& SB = mSB;

            for (size_t i = 0; i < INIT_BUFSIZE; ++i) {
                ASSERTV(i, i            == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, i            == SB.dataLengthInInitialBuffer());

                ASSERTV(i, 0            == SB.overflowBufferSize());
                ASSERTV(i, 0            == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer       == SB.initialBuffer());
                ASSERTV(i, 0            == SB.overflowBuffer());

                mSB.sputc('a');
            }
            if(veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting overflown streambuf." << endl;
        {
            char         buffer[INIT_BUFSIZE];
            Obj          mSB(buffer, INIT_BUFSIZE, &ta);
            const Obj&   SB = mSB;
            for (size_t i = 0; i < INIT_BUFSIZE; ++i) {
                mSB.sputc('a');
            }

            ASSERT(0 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());

            for (size_t i = 0; i < INIT_BUFSIZE; ++i) {
                ASSERTV(i, INIT_BUFSIZE + i == SB.dataLength());

                ASSERTV(i, INIT_BUFSIZE == SB.initialBufferSize());
                ASSERTV(i, INIT_BUFSIZE == SB.dataLengthInInitialBuffer());

                ASSERTV(i, ta.lastAllocatedNumBytes() ==
                                                      SB.overflowBufferSize());
                ASSERTV(i, i == SB.dataLengthInOverflowBuffer());

                ASSERTV(i, buffer == SB.initialBuffer());
                ASSERTV(i, ta.lastAllocatedAddress() == SB.overflowBuffer());
                mSB.sputc('a');
            }
            if(veryVerbose) P(SB);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the auxiliary function used in the test driver. Note that
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
        //: 1 Create a 'bdlbs::OverflowMemOutStreamBuf' object and write some
        //:   characters to it.  Use 'ostrstream' to write that object's value
        //:   to two separate character buffers each with different initial
        //:   values.  Compare the contents of these buffers with the literal
        //:   expected output format and verify that the characters beyond the
        //:   length of the streambuf contents are unaffected in both buffers.
        //:   (C-1..3)
        //:
        //: 2 Create an empty 'bdlbs::OverflowMemOutStreamBuf' object.  Use
        //:   'ostrstream' to write that object's value to the character
        //:   buffer.  Compare the content of this buffer with the literal
        //:   expected output format and verify that the characters beyond the
        //:   length of the streambuf contents are unaffected.  (C-1..3)
        //:
        //: 3 Create a 'bdlbs::OverflowMemOutStreamBuf' object and write to it
        //:   number of characters enough to fulfil initial buffer.  Write one
        //:   more character.   Use 'ostrstream' to write that object's value
        //:   to the character buffer.  Compare the content of this buffer with
        //:   the literal expected output format and verify that the characters
        //:   beyond the length of the streambuf contents are unaffected.
        //:   (C-1..3)
        //:
        //: 4 Create a 'bdlbs::OverflowMemOutStreamBuf' object.  Use
        //:    'ostrstream' to write that object's value and some characters in
        //:    consecutive order.  (C-4)
        //
        // Testing:
        //   TEST APPARATUS: os& operator<<(os&, const OverflowMemOutStrBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nChecking correctness of value formatting."
                          << endl;
        {
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);
            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

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

        if (verbose) cout << "\nChecking operator<< for empty streambuf."
                          << endl;
        {
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj          mSB(buffer, INIT_BUFSIZE);
            const Obj&   SB = mSB;
            stringstream out;

            out << SB;

            const char *EXPECTED = "\nInitial Buffer:"
                                   "\nOverflow Buffer:";

            if (verbose) {
                T_ P(EXPECTED)
                T_ P(out.str())
            }

            ASSERT(EXPECTED == out.str());
        }

        if (verbose) cout << "\nChecking operator<< for overflown streambuf."
                          << endl;
        {
            char         buffer[1];
            Obj          mSB(buffer, 1);
            stringstream out;

            mSB.sputc('a');
            mSB.sputc('b');

            out << mSB;

            const char *EXPECTED = "\nInitial Buffer:"
                       "\n0000\t01100001"
                       "\nOverflow Buffer:"
                       "\n0000\t01100010";

            if (verbose) {
                T_ P(EXPECTED)
                T_ P(out.str())
            }

            ASSERT(EXPECTED == out.str());
        }

        if (verbose) cout << "\nChecking operator<< return value." << endl;
        {
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB << "next";   // Ensure modifiable stream is returned
            out2 << SB  << "next";
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Note that this test does not constitute proof, because the
        //   accessors have not been tested, and so cannot be relied upon
        //   completely.
        //   Also note that 'seekoff' method is called by base class method
        //   'pubseekoff'.
        //
        // Concerns:
        //: 1 Object can be created and "wired-up" properly with value
        //:   constructor.
        //:
        //: 2 The default allocator comes from 'bslma::Default::allocator'.
        //:
        //: 3 The internal memory management system is hooked up properly
        //:   so that internally allocated memory draws from a user-supplied
        //:   allocator whenever one is specified.
        //:
        //: 4 Method 'sputc' writes printing and non-printing characters
        //:   correctly.
        //:
        //: 5 Method 'sputc' writes bytes with leading bit set correctly.
        //:
        //: 6 Method 'sputc' writes no more than one character.
        //:
        //: 7 Virtual method 'overflow' is called if there is no place to store
        //:   a character, written by 'sputc' method.
        //:
        //: 8 Method 'seekoff' uses the correct location from which to offset.
        //:
        //: 9 Method 'seekoff' computes both negative and positive offsets
        //:    correctly.
        //:
        //:10 Method 'seekoff' sets the "cursor" (i.e., the base-class' pptr())
        //:   position to the correct location.
        //:
        //:11 Seeking out of bounds is handled correctly and returns invalid
        //:   value.
        //:
        //:12 Trying to seek in the "get" area has no effect and returns
        //:   invalid value.
        //:
        //:13 The destructor works properly and releases allocated memory.
        //
        // Plan:
        //: 1 Create an object with constructor.  Verify values, received from
        //:   the accessors.  (C-1)
        //:
        //: 2 Construct three distinct objects, in turn, but configured
        //:   differently: (a) without passing an allocator, (b) passing a null
        //:   allocator address explicitly, and (c) passing the address of a
        //:   test allocator distinct from the default.  Verify that right
        //:   allocator is used to obtain memory in each case.  (C-2..3)
        //:
        //: 3 Using the table-driven technique, specify a set of characters to
        //:   write to the stream buffer.
        //:
        //: 4 For each row 'R' in the table of P-3:
        //:
        //:   1 Write character using the 'sputc' method, and verify that the
        //:     bit pattern for that character is correct present and in the
        //:     stream buffer.  (C-4..5)
        //:
        //:   2 Verify that no more than one symbol has been written.  (C-6)
        //:
        //: 5 Write character using the 'sputc' method, to overflow existed
        //:   buffers (initial and overflow).  Verify that new overflow buffer
        //:   is created and correct amount of memory is allocated.  (C-7)
        //:
        //: 6 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-8..11)
        //:
        //: 7 Create an 'bdlsb::OverflowMemOutStreamBuf' object, write some
        //:   characters to overflow initial buffer and let it go out of scope.
        //:   Verify that all memory has been released.  (C-12)
        //
        // Testing:
        //   OverflowMemOutStreamBuf(char *, int, bslma::Allocator *);
        //   ~OverflowMemOutStreamBuf();
        //   int_type sputc(char_type);
        //   pos_type seekoff(off_type, seekdir, openmode);
        //   int_type overflow(int_type c = traits_type::eof());
        //   PROXY: pos_type pubseekoff(off_type, seekdir, openmode);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nConstructor testing." << endl;
        {
            if (verbose) cout << "\tBasic constructor test." << endl;
            {
                bslma::TestAllocator ta(veryVeryVerbose);

                char       buffer[INIT_BUFSIZE];
                Obj        mSB(buffer, INIT_BUFSIZE - 1, &ta);
                const Obj& SB = mSB;

                ASSERT(0                == SB.dataLength());
                ASSERT(INIT_BUFSIZE - 1 == SB.initialBufferSize());
                ASSERT(0                == SB.dataLengthInInitialBuffer());
                ASSERT(0                == SB.overflowBufferSize());
                ASSERT(0                == SB.dataLengthInOverflowBuffer());
                ASSERT(buffer           == SB.initialBuffer());
                ASSERT(0                == SB.overflowBuffer());
            }

            if (verbose) cout << "\tNegative Testing." << endl;
            {
                bslma::TestAllocator            ta(veryVeryVerbose);
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                char buffer[1];
                ASSERT_SAFE_FAIL(Obj(     0,  0, &ta));
                ASSERT_SAFE_FAIL(Obj(     0,  1, &ta));
                ASSERT_SAFE_FAIL(Obj(buffer,  0, &ta));
                ASSERT_SAFE_PASS(Obj(buffer,  1, &ta));
            }
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            static const struct {
                int  d_line;     // line number
                char d_outChar;  // character to output
                int  d_retVal;   // character to output
            } DATA[] = {
                //LINE  OUTPUT   RETVAL
                //      CHAR
                //----  -------  ------
                // Printing character equivalence classes are
                // ranges [0..31], [32..126], and {127}.
                { L_,   0,       0      },
                { L_,   3,       3      },
                { L_,   126,     126    },
                { L_,   127,     127    },
                // Leading-bit equivalence classes are ranges
                // [0..127] and [128..255]
                { L_,   0,       0      },
                { L_,   127,     127    },
                { L_,   '\x80',  128    },
                { L_,   '\xFF',  255    },
            };   // end table DATA

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. adds the character, and
            //    2. does not overwrite beyond the character.
            bslma::TestAllocator ta(veryVeryVerbose);

            char  buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE, &ta);
            const Obj& SB = mSB;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int      LINE = DATA[i].d_line;
                const char OUT_CHAR = DATA[i].d_outChar;
                const int  RET_VAL  = DATA[i].d_retVal;

                if (veryVerbose) { T_ P((int)OUT_CHAR) }

                int retVal = mSB.sputc(OUT_CHAR);

                ASSERTV(LINE, RET_VAL == retVal);
                ASSERTV(LINE, OUT_CHAR == buffer[i]);
                ASSERTV(LINE, 'Z' == buffer[i + 1]);
                ASSERTV(LINE, i + 1 == SB.dataLengthInInitialBuffer());
            }
        }

        if (verbose) cout << "\nAllocator installation test." << endl;
        {
            if (verbose) cout << "\tConstructor with default allocator."
                              << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                char       buffer[1];
                Obj        mSB(buffer, 1);
                const Obj& SB = mSB;

                mSB.sputc('a');

                ASSERT(0 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 == SB.overflowBuffer());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mSB.sputc('b');

                ASSERT(1 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 != SB.overflowBuffer());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose) cout
                            << "\tConstructor with explicit default allocator."
                            << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                char       buffer[1];
                Obj        mSB(buffer, 1, static_cast<bslma::Allocator *>(0));
                const Obj& SB = mSB;

                mSB.sputc('a');

                ASSERT(0 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 == SB.overflowBuffer());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mSB.sputc('b');

                ASSERT(1 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 != SB.overflowBuffer());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose) cout << "\tConstructor with object allocator."
                              << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ASSERT(0 == oa.numAllocations());

                char       buffer[1];
                Obj        mSB(buffer, 1, &oa);
                const Obj& SB = mSB;

                mSB.sputc('a');

                ASSERT(0 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 == SB.overflowBuffer());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesTotal());
                ASSERT(0 == da.numAllocations());

                mSB.sputc('b');

                ASSERT(1 == SB.dataLengthInOverflowBuffer());
                ASSERT(0 != SB.overflowBuffer());
                ASSERT(1 == oa.numAllocations());
                ASSERT(0 != oa.numBytesTotal());
                ASSERT(0 == da.numAllocations());
            }
        }

        if (verbose) cout << "\nOverflow test." << endl;
        {
            bslma::TestAllocator ta(veryVeryVerbose);

            const size_t BUF_SIZE = 2;
            char         buffer[BUF_SIZE];
            memset(buffer, 'Z', BUF_SIZE);

            Obj          mSB(buffer, BUF_SIZE - 1, &ta);
            const Obj&   SB = mSB;

            const size_t INIT_CAPACITY = SB.initialBufferSize();

            ASSERT(0                 == ta.numAllocations());
            ASSERT(0                 == SB.dataLength());

            bsls::Types::Int64 numAllocations = ta.numAllocations();
            bsls::Types::Int64 numDeallocations = ta.numDeallocations();

            int result = mSB.sputc('a');

            ASSERT('a'               == result);
            ASSERT('a'               == SB.initialBuffer()[0]);
            ASSERT('Z'               == SB.initialBuffer()[1]);
            ASSERT(INIT_CAPACITY     == SB.dataLength());
            ASSERT(0                 == SB.dataLengthInOverflowBuffer());
            ASSERT(0                 == SB.overflowBuffer());
            ASSERT(numAllocations    == ta.numAllocations());
            ASSERT(numDeallocations  == ta.numDeallocations());

            // exceeding initial capacity
            result = mSB.sputc('b');

            ASSERT('b'               == result);
            ASSERT('Z'               == SB.initialBuffer()[1]);
            ASSERT(INIT_CAPACITY + 1 == SB.dataLength());
            ASSERT(0                 != SB.overflowBuffer());
            ASSERT('b'               == SB.overflowBuffer()[0]);
            ASSERT(INIT_CAPACITY     == SB.overflowBufferSize());
            ASSERT(1                 == SB.dataLengthInOverflowBuffer());
            ASSERT(numAllocations    <  ta.numAllocations());
            ASSERT(numDeallocations  <= ta.numDeallocations());

            // exceeding overflow capacity
            const char *prevOverflowBuf = SB.overflowBuffer();
            ASSERT(SB.dataLengthInOverflowBuffer() == SB.overflowBufferSize());

            result = mSB.sputc('c');

            ASSERT('c'               == result);
            ASSERT(INIT_CAPACITY + 2 == SB.dataLength());
            ASSERT('b'               == SB.overflowBuffer()[0]);
            ASSERT('c'               == SB.overflowBuffer()[1]);
            ASSERT(INIT_CAPACITY * 2 == SB.overflowBufferSize());
            ASSERT(2                 == SB.dataLengthInOverflowBuffer());
            ASSERT(prevOverflowBuf   == ta.lastDeallocatedAddress());
            ASSERT(INIT_CAPACITY     == ta.lastDeallocatedNumBytes());
        }

        if (verbose) cout << "\nTesting 'seekoff'." << endl;
        {
            int startRangeBeg =  0;
            int startRangeEnd =  INIT_BUFSIZE * 4;

            int offsetRangeBeg = -INIT_BUFSIZE * 4;
            int offsetRangeEnd =  INIT_BUFSIZE * 4;

            for (int start = startRangeBeg; start < startRangeEnd; start++) {
                for (int offset = offsetRangeBeg; offset < offsetRangeEnd;
                                                                    offset++) {
                    Obj::pos_type  ret;      // return of pubseekoff.
                    char           buffer[INIT_BUFSIZE];

                    if (veryVerbose) { T_ P_(start) T_ P(offset) }

                    {
                        Obj        mSB(buffer, INIT_BUFSIZE);
                        const Obj& SB = mSB;

                        // Initialize start position.
                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('Z');
                        }

                        // Test from the beginning, while currently not at the
                        // beginning.
                        ret = mSB.pubseekoff(offset, BEG, PUT);

                        if (0 > offset ) {
                            ASSERTV(start, offset, -1 == ret);
                            ASSERTV(
                                start,
                                offset,
                                static_cast<size_t>(start) == SB.dataLength());
                        }
                        else {
                            ASSERTV(start, offset, offset == ret);
                            ASSERTV(
                                  start,
                                  offset,
                                  SB.dataLength() == static_cast<size_t>(ret));
                        }
                    }
                    {
                        Obj        mSB(buffer, INIT_BUFSIZE);
                        const Obj& SB = mSB;

                        // Initialize start position.
                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('Z');
                        }

                        // Test from the current position.
                        ret = mSB.pubseekoff(offset, CUR, PUT);

                        if (0 > start + offset ) {
                            ASSERTV(start, offset, -1 == ret);
                            ASSERTV(
                                start,
                                offset,
                                static_cast<size_t>(start) == SB.dataLength());
                        }
                        else {
                            ASSERTV(start, offset, start + offset == ret);
                            ASSERTV(
                                  start,
                                  offset,
                                  SB.dataLength() == static_cast<size_t>(ret));
                        }
                    }
                    {
                        Obj        mSB(buffer, INIT_BUFSIZE);
                        const Obj& SB = mSB;

                        // Initialize start position.
                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('Z');
                        }

                        long int capacity = mSB.initialBufferSize() +
                                            mSB.overflowBufferSize();

                        // Test from the end, while currently not at the
                        // beginning.
                        ret = mSB.pubseekoff(offset, END, PUT);

                        if (0 > capacity + offset ) {
                            ASSERTV(start, offset, -1 == ret);
                            ASSERTV(
                                start,
                                offset,
                                static_cast<size_t>(start) == SB.dataLength());
                        }
                        else {
                            ASSERTV(start, offset, capacity + offset == ret);
                            ASSERTV(
                                  start,
                                  offset,
                                  SB.dataLength() == static_cast<size_t>(ret));
                        }
                    }

                    // Seeking into 'get' area should always fail.
                    {
                        Obj mSB(buffer, INIT_BUFSIZE);
                        // Initialize start position.
                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('Z');
                        }

                        ASSERTV(start, offset, bsl::streambuf::pos_type(-1) ==
                                            mSB.pubseekoff(offset, CUR, GET));
                        ASSERTV(start, offset, bsl::streambuf::pos_type(-1) ==
                                            mSB.pubseekoff(offset, BEG, GET));
                        ASSERTV(start, offset, bsl::streambuf::pos_type(-1) ==
                                            mSB.pubseekoff(offset, END, GET));
                    }
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nTesting exception neutrality." << endl;
        {
            const size_t DATA[] = { 0,
                                    INIT_BUFSIZE,
                                    DIBS };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // stream buffer states.

            for(size_t i = 0; i < DATA_LEN; ++i ) {
                size_t INITIAL_DATA_LEN = DATA[i];

                bslma::TestAllocator eta("exception test",
                                         veryVeryVeryVerbose);
                {
                    char buffer[INIT_BUFSIZE];
                    Obj  mSB(buffer, INIT_BUFSIZE, &eta);
                    // Pre-fill stream buffer with some data.  This might also
                    // trigger some initial memory allocation.
                    for (size_t j = 0; j < INITIAL_DATA_LEN; ++j) {
                        mSB.sputc('a');
                    }

                    int numIterations = 0;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(eta) {
                        bslma::TestAllocatorMonitor monitor(&eta);

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

                    ASSERTV(i, numIterations, 2 == numIterations);
                }

                // Test exception neutrality: all memory has been returned.
                ASSERTV(i, eta.numBlocksInUse(), 0 == eta.numBlocksInUse());
            }
        }
#endif

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator               da(veryVeryVerbose);
            const bslma::DefaultAllocatorGuard dag(&da);

            char buffer[INIT_BUFSIZE];

            ASSERT(0 == da.numBlocksInUse());

            {
                // Overflow buffer doesn't exist
                Obj mSB(buffer, INIT_BUFSIZE);
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == da.numBlocksInUse());
            }

            {
                // Overflow buffer exists
                Obj mSB(buffer, INIT_BUFSIZE);
                mSB.pubseekoff(INIT_BUFSIZE + 1, BEG, PUT);
                ASSERT(INIT_BUFSIZE == da.numBytesInUse());
                ASSERT(1            == da.numBlocksInUse());
            }

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == da.numBlocksInUse());
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
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nMake sure we can create and use a "
                          << "'bdlsb::OverflowMemOutStreamBuf'."
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
