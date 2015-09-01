// bdlsb_overflowmemoutstreambuf.t.cpp                                -*-C++-*-
#include <bdlsb_overflowmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>                  // for testing only

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>     // 'setw', 'setfill'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cctype.h>      // 'isdigit', 'isupper', 'islower'

using namespace BloombergLP;
using bsl::cout; using bsl::flush; using bsl::endl; using bsl::cerr;
using bsl::strlen; using bsl::strncmp; using bsl::memset;
using bsl::strchr; using bsl::string; using bsl::strcmp;
using bsl::memcmp; using bsl::streambuf;

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
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ---------------------------------------------------------------
// Primary Constructors:
//   A 'bdlsb::OverflowMemOutStreamBuf' is created with a client-supplied C
//   char array, a length specification, and a allocator.  The allocator is
//   used for the overflow buffer.  This is the only constructor, so our set of
//   primary constructors will be:
//
//    o bdlsb::OverflowMemOutStreamBuf(char             *initialBuffer,
//                                    int               initialBufferSize,
//                                    bslma::Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   We can bring a 'bdlsb::OverflowMemOutStreamBuf' to any achievable
//   white-box state by using a combination of 'pubseekpos', which allows us to
//   reposition the "cursor" (i.e., the position that the next write operation
//   will output to) anywhere in the stream buffer, and 'sputc', which writes a
//   single character into the stream buffer.  'sputn' is widely used with
//   'sputc' to modify the buffer.
//
//    o int_type sputc(char_type);
//    o pos_type pubseekpos(pos_type, ios_base::openmode);
//    o streamsize xsputn(const char_type, streamsize);
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  The
//   'bdlsb::OverflowMemOutStreamBuf' component has 'dataLength',
//   'dataLengthInInitialBuffer', 'dataLengthInOverflowBuffer',
//   'initialBuffer', 'overflowBuffer', 'initialBufferSize' and
//   'overflowBufferSize' as accessors, and so they form our accessor set:
//
//     o int dataLength() const;
//     o int dataLengthInOverflowBuffer() const;
//     o int dataLengthInInitialBuffer() const;
//     o const char *initialBuffer() const;
//     o int initialBufferSize() const;
//     o const char *overflowBuffer() const;
//     o int overflowBufferSize() const;
//
// This class is an out stream and should behave as such.  All classes in this
// component are a value-semantic types that represent big-endian integer
// types.  They have the same value if they have the same in-core big endian
// representation.
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [11] void grow(int n);
// [ 4] void privateSync();
// [ 4] virtual int_type overflow(
//                            int_type c = bsl::streambuf::traits_type::eof());
//
// CREATORS
// [ 3] bdlsb::OverflowMemOutStreamBuf(char             *buffer,
//                                    int               size,
//                                    bslma::Allocator *basicAllocator = 0);
//
// MANIPULATORS
// [ 4] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 4] int_type sputc(char_type);
// [ 4] pos_type seekpos(pos_type, openmode);
// [ 8] streamsize xsputn(const char_type, streamsize);
// [10] pos_type seekoff(off_type, seekdir, openmode);
//
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize dataLength() const;
// [ 4] int dataLengthInOverflowBuffer() const;
// [ 4] int dataLengthInInitialBuffer() const;
// [ 4] int initialBufferSize() const;
// [ 4] int overflowBufferSize() const;
// [ 4] const char *initialBuffer() const;
// [ 4] const char *overflowBuffer() const;
//
// FREE OPERATORS
// [ 5] bsl::ostream& 'operator<<'
//-----------------------------------------------------------------------------
// [ 1] BREATHING/USAGE TEST
// [ 2] TEST APPARATUS
// [ 3] PRIMARY CONSTRUCTOR
// [ 4] PRIMARY MANIPULATORS/BASIC ACCESSORS
// [ 5] OUTPUT: bsl::ostream& 'operator<<'
// [ 6] NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
// [ 7] PUT string TEST
// [ 8] SEEKOFF/SEEKPOS TEST
// [ 9] CAPITALIZING USAGE EXAMPLE
// [10] TESTING CONCERN: EOF IS STREAMED CORRECTLY

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
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
const int INITIAL_BUFSIZE = 20;
const int FIRST_OVER_BUFSIZE = 40;
const int TOTAL_CAP = INITIAL_BUFSIZE + FIRST_OVER_BUFSIZE;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

              // =============================================
              // operator<< for bdlsb::OverflowMemOutStreamBuf
              // =============================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bdlsb::OverflowMemOutStreamBuf&
                                                           streamBuffer);
// Write the contents of the specified 'streamBuffer' (as well as a marker
// indicating eight bytes groupings) to the specified output 'stream' in binary
// format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bdlsb::OverflowMemOutStreamBuf&
                                                           streamBuffer)
{
    int   len  = streamBuffer.dataLengthInInitialBuffer();
    const char *data = streamBuffer.initialBuffer();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    stream << "\nInitial Buffer:";
    for (int i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }

    len   = streamBuffer.dataLengthInOverflowBuffer();
    data  = streamBuffer.overflowBuffer();

    stream << "\nOverflow Buffer:";

    for (int i = 0; i < len; ++i) {
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
///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case a
// stream with simple formatting requirements -- namely, capitalizing all
// character data that passes through its management.  (To simplify the
// example, we do not include the functions for streaming non-character data.)
//
// The stream uses a user-supplied 'char'-array-based stream buffer, which is
// inherently a fixed-size buffer.
//..
    // my_capitalizingstream.h

    class my_CapitalizingStream {
        // This class capitalizes character data....

        // PRIVATE TYPES
        enum { k_STREAMBUF_CAPACITY = 10 };

        // DATA
        char                           *d_buffer;       // initial buffer
                                                        // (owned)

        bdlsb::OverflowMemOutStreamBuf *d_streamBuf;    // stream buffer
                                                        // (owned)

        bslma::Allocator               *d_allocator_p;  // memory allocator
                                                        // (held, not owned)

        // FRIENDS
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                          const bsl::string&      data);
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                          const char             *data);
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                          char                    data);
      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_CapitalizingStream,
                                     bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_CapitalizingStream(bslma::Allocator *allocator = 0);
            // Create a stream that capitalizes everything.

        ~my_CapitalizingStream();
            // Destroy this object.

        // ACCESSORS
        const bdlsb::OverflowMemOutStreamBuf *streamBuf();
            // Return the stream buffer used by this stream.  Note that this
            // function is for debugging only.
    };

    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const bsl::string&      data);
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data);
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      char                    data);
        // Write the specified 'data' in capitalized form to the
        // specified 'stream'.
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert all string characters to upper-
// case.
//..
    // my_capitalizingstream.cpp

    // CREATORS
    my_CapitalizingStream::my_CapitalizingStream(
                                              bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_buffer = reinterpret_cast<char*>(
                                d_allocator_p->allocate(k_STREAMBUF_CAPACITY));

        d_streamBuf = new(*d_allocator_p) bdlsb::OverflowMemOutStreamBuf(
                                                          d_buffer,
                                                          k_STREAMBUF_CAPACITY,
                                                          d_allocator_p);
    }

    my_CapitalizingStream::~my_CapitalizingStream()
    {
        d_allocator_p->deleteObjectRaw(d_streamBuf);
        d_allocator_p->deleteObjectRaw(d_buffer);
    }

    // ACCESSORS
    const bdlsb::OverflowMemOutStreamBuf *my_CapitalizingStream::streamBuf()
    {
        return d_streamBuf;
    }

    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                      const bsl::string&     data)
    {
        bsl::string tmp(data);
        bsl::transform(tmp.begin(),
                       tmp.end(),
                       tmp.begin(),
                       (int(*)(int))bsl::toupper);
        stream.d_streamBuf->sputn(tmp.data(), tmp.length());
        return stream;
    }

    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data)
    {
        bsl::string tmp(data);
        bsl::transform(tmp.begin(),
                       tmp.end(),
                       tmp.begin(),
                       (int(*)(int))bsl::toupper);
        stream.d_streamBuf->sputn(tmp.data(), tmp.length());
        return stream;
    }

    my_CapitalizingStream& operator<<(my_CapitalizingStream& stream,
                                      char                   data)
    {
         stream.d_streamBuf->sputc(bsl::toupper(data));
         stream.d_streamBuf->pubsync();
         return stream;
    }
//..

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING grow
        //
        // Concerns:
        //   A. If sputn writes past the existing total capacity, the overflow
        //      buffer will grow accordingly.
        //   B. If sputc writes past the existing total capacity, the overflow
        //      buffer will grow accordingly.
        //   C. If seekoff seek past the existing total capacity the overflow
        //      buffer will grow accordingly.
        //   D. If seekpos seek past the existing total capacity the overflow
        //      buffer will grow accordingly.
        //
        //   On any grow operation, if an overflow buffer does not exist, it
        //   shall be allocated.  If one does exist, a new one will be
        //   allocated, data copied over, and the old one deallocated.
        //
        // Plan:
        //   To address concern A, create a buffer at various different states.
        //   For each state, sputn past the total capacity.  Verify growth
        //   size and data is correct.
        //
        //   To address concern B, create a buffer at various different states.
        //   For each state, sputc past the total capacity.  Verify growth
        //   size and data is correct.
        //
        //   To address concern C, create a buffer at various different states.
        //   For each state, seekoff past the total capacity.  Verify growth
        //   size and data is correct.
        //
        //   To address concern D, create a buffer at various different states.
        //   For each state, seekpos past the total capacity.  Verify growth
        //   size and data is correct.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Brute-Force Techniques
        //
        // Testing:
        //   void grow(int n);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GROW TEST" << endl
                          << "=========" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            if (verbose) cout << "\nTesting grow." << endl;
            int start = 0;
            int end   = 4*INITIAL_BUFSIZE;
            const char buf[] = "This is the buffer that is put into the "
                               "stream.  As long as this is larger than "
                               "4x the initial buffer size then we're good.";
            ASSERT(4*INITIAL_BUFSIZE < strlen(buf));

            for (int i=0;i<end;i++) {
                // Testing sputn grow.

                char *bytes = new char[INITIAL_BUFSIZE];
                bdlsb::OverflowMemOutStreamBuf sb(bytes,
                                                     INITIAL_BUFSIZE, &ta);
                ASSERT(0 == sb.overflowBufferSize());
                ASSERT(0 == sb.overflowBuffer());

                sb.pubseekpos(i, bsl::ios_base::out);

                if (sb.dataLength() <= INITIAL_BUFSIZE) {
                    ASSERT(0 == sb.overflowBufferSize());
                    ASSERT(0 == sb.overflowBuffer());
                }
                else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                    ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                else {
                    ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                sb.sputn(buf + i, end - i);
                ASSERT(4*INITIAL_BUFSIZE <= sb.overflowBufferSize());
                ASSERT(0 != sb.overflowBuffer());
                if (i <= sb.initialBufferSize()) {
                    ASSERT(0 == strncmp(buf + i, sb.initialBuffer() + i,
                                        sb.initialBufferSize() - i));
                    ASSERT(0 == strncmp(buf + sb.initialBufferSize(),
                                        sb.overflowBuffer(),
                                        end - sb.initialBufferSize()));
                }
                else {
                    ASSERT(0 == strncmp(buf + i,
                                        sb.overflowBuffer() + i -
                                        sb.initialBufferSize(), end - i));
                }
            }

            for (int i=0;i<end;i++) {
                // Testing sputc grow.

                char *bytes = new char[INITIAL_BUFSIZE];
                bdlsb::OverflowMemOutStreamBuf sb(bytes,
                                             INITIAL_BUFSIZE, &ta);
                ASSERT(0 == sb.overflowBufferSize());
                ASSERT(0 == sb.overflowBuffer());

                sb.pubseekpos(i, bsl::ios_base::out);

                if (sb.dataLength() <= INITIAL_BUFSIZE) {
                    ASSERT(0 == sb.overflowBufferSize());
                    ASSERT(0 == sb.overflowBuffer());
                }
                else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                    ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                else {
                    ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                for (int j=i;j<end;j++) {
                    sb.sputc(buf[j]);
                    sb.pubsync();
                    if (sb.dataLength() <= INITIAL_BUFSIZE) {
                        ASSERT(0 == sb.overflowBufferSize());
                        ASSERT(0 == sb.overflowBuffer());
                    }
                    else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                        ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                }
                ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                ASSERT(0 != sb.overflowBuffer());
                if (i <= sb.initialBufferSize()) {
                    ASSERT(0 == strncmp(buf + i, sb.initialBuffer() + i,
                                        sb.initialBufferSize() - i));
                    ASSERT(0 == strncmp(buf + sb.initialBufferSize(),
                                        sb.overflowBuffer(),
                                        end - sb.initialBufferSize()));
                }
                else {
                    ASSERT(0 == strncmp(buf + i,
                                        sb.overflowBuffer() + i -
                                        sb.initialBufferSize(), end - i));
                }
            }

            for (int i=0;i<end;i++) {
                // Testing seekoff grow.

                char *bytes = new char[INITIAL_BUFSIZE];
                bdlsb::OverflowMemOutStreamBuf sb(bytes,
                                             INITIAL_BUFSIZE, &ta);
                ASSERT(0 == sb.overflowBufferSize());
                ASSERT(0 == sb.overflowBuffer());

                for (int j=0;j<i;j++) {
                    sb.sputc('c');
                }

                if (sb.dataLength() <= INITIAL_BUFSIZE) {
                    ASSERT(0 == sb.overflowBufferSize());
                    ASSERT(0 == sb.overflowBuffer());
                }
                else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                    ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                else {
                    ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                for (int j=i;j<end;j++) {
                    sb.pubseekoff(j, bsl::ios_base::beg, bsl::ios_base::out);
                    sb.pubsync();
                    if (sb.dataLength() <= INITIAL_BUFSIZE) {
                        ASSERT(0 == sb.overflowBufferSize());
                        ASSERT(0 == sb.overflowBuffer());
                    }
                    else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                        ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                }
                ASSERT(4*INITIAL_BUFSIZE <= sb.overflowBufferSize());
                ASSERT(0 != sb.overflowBuffer());
            }

            for (int i=0;i<end;i++) {
                // Testing seekpos grow.

                char *bytes = new char[INITIAL_BUFSIZE];
                bdlsb::OverflowMemOutStreamBuf sb(bytes,
                                             INITIAL_BUFSIZE, &ta);
                ASSERT(0 == sb.overflowBufferSize());
                ASSERT(0 == sb.overflowBuffer());

                for (int j=0;j<i;j++) {
                    sb.sputc('c');
                }

                if (sb.dataLength() <= INITIAL_BUFSIZE) {
                    ASSERT(0 == sb.overflowBufferSize());
                    ASSERT(0 == sb.overflowBuffer());
                }
                else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                    ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                else {
                    ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                    ASSERT(0 != sb.overflowBuffer());
                }
                for (int j=i;j<end;j++) {
                    sb.pubseekpos(j, bsl::ios_base::out);
                    sb.pubsync();
                    if (sb.dataLength() <= INITIAL_BUFSIZE) {
                        ASSERT(0 == sb.overflowBufferSize());
                        ASSERT(0 == sb.overflowBuffer());
                    }
                    else if (sb.dataLength() <= 3*INITIAL_BUFSIZE) {
                        ASSERT(2*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                    else {
                        ASSERT(4*INITIAL_BUFSIZE == sb.overflowBufferSize());
                        ASSERT(0 != sb.overflowBuffer());
                    }
                }
                ASSERT(4*INITIAL_BUFSIZE <= sb.overflowBufferSize());
                ASSERT(0 != sb.overflowBuffer());
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: EOF IS STREAMED CORRECTLY
        //
        // Concerns:
        //   * That 'xsputn' copies EOF when it appears within a buffer
        //     boundary.
        //
        //   * That 'xsputn' copies EOF when it appears as the first character
        //     in a buffer (the static or the additional buffer).
        //
        //   * That 'xsputn' copies EOF when it appears as the last character
        //     in a buffer (the static or the additional buffer).
        //
        // Plan:
        //   Instantiate an 'bdlsb::OverflowMemOutStreamBuf', use sputc to
        //   insert a EOF at the beginning of the initial buffer, the end of
        //   the initial buffer and the first char of overflow buffer via
        //   overflow().  Then seek to the end of the overflow buffer, and use
        //   putc to insert a EOF there, and trigger an overflow buffer
        //   expansion.
        //
        //   Repeat using sputn.
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
            typedef bdlsb::OverflowMemOutStreamBuf Obj;
            const int EOF_VAL = bsl::streambuf::traits_type::eof();

            enum {
                k_BUF_SIZE              = 20 ,
                k_MAGIC_CHAR_SIZE       = 6
            };

            char buffer[k_BUF_SIZE + k_MAGIC_CHAR_SIZE];
            for (int i=0; i < k_BUF_SIZE + k_MAGIC_CHAR_SIZE; i++) {
                buffer[i] = 'A' + i;
            }

            Obj mX(buffer, k_BUF_SIZE, &ta);

            // Beginning of initial buffer.

            ASSERT((char)EOF_VAL == (char)mX.sputc(EOF_VAL));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(0        == mX.overflowBufferSize());
            ASSERT(0        == mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(0        == mX.dataLengthInOverflowBuffer());
            ASSERT(1        == mX.dataLengthInInitialBuffer());
            ASSERT(1        == mX.dataLength());
            ASSERT((char)EOF_VAL == (char)buffer[0]);
            ASSERT('B'      == buffer[1]);

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            ASSERT('R' == mX.sputc('R'));
            ASSERT((char)EOF_VAL == (char)mX.sputc(EOF_VAL));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(0        == mX.overflowBufferSize());
            ASSERT(0        == mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(0        == mX.dataLengthInOverflowBuffer());
            ASSERT(2        == mX.dataLengthInInitialBuffer());
            ASSERT(2        == mX.dataLength());
            ASSERT('R'      == buffer[0]);
            ASSERT((char)EOF_VAL == (char)buffer[1]);
            ASSERT('C'      == buffer[2]);

            // End of initial buffer.

            mX.pubseekoff(k_BUF_SIZE - 1, bsl::ios_base::beg,
                          bsl::ios_base::out);
            ASSERT((char)EOF_VAL == (char)mX.sputc(EOF_VAL));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(0        == mX.overflowBufferSize());
            ASSERT(0        == mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(0        == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(20       == mX.dataLength());
            ASSERT((char)EOF_VAL  == (char)buffer[19]);

            // First char of new buffer, trigger overflow.

            ASSERT((char)EOF_VAL == (char)mX.sputc(EOF_VAL));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(1        == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(21       == mX.dataLength());
            ASSERT((char)EOF_VAL == (char)buffer[19]);
            ASSERT((char)EOF_VAL == (char)mX.overflowBuffer()[0]);

            ASSERT((char)EOF_VAL == (char)mX.sputc(EOF_VAL));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(2        == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(22       == mX.dataLength());
            ASSERT((char)EOF_VAL == (char)buffer[19]);
            ASSERT((char)EOF_VAL == (char)mX.overflowBuffer()[0]);
            ASSERT((char)EOF_VAL == (char)mX.overflowBuffer()[1]);

            char source[] = "The only thing that tastes better than free"
                            " liquor is stolen liquor";
            source[19] = EOF_VAL;
            source[20] = EOF_VAL;
            source[21] = EOF_VAL;

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            int putOut;
            putOut = 19;

            // Write to end of initial buffer - 1.

            ASSERT(putOut == mX.sputn(source, putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(0        == mX.dataLengthInOverflowBuffer());
            ASSERT(19       == mX.dataLengthInInitialBuffer());
            ASSERT(19       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE - 1));

            // Write to end of initial buffer.

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            putOut = 20;
            ASSERT(putOut == mX.sputn(source, putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(0        == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(20       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE));

            // Write to first char of overflow buffer triggering expansion.

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            putOut = 21;
            ASSERT(putOut == mX.sputn(source, putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(1        == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(21       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE));
            ASSERT(0        == strncmp(source + mX.initialBufferSize(),
                                       mX.overflowBuffer(), 1));

            // Write to last char of overflow buffer - 1.

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            putOut = 39;
            ASSERT(mX.initialBufferSize() == mX.sputn(source,
                                                      mX.initialBufferSize()));
            ASSERT(putOut == mX.sputn(source + mX.initialBufferSize(),
                                      putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(39       == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(59       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE));
            ASSERT(0        == strncmp(source + mX.initialBufferSize(),
                                       mX.overflowBuffer(),
                                       mX.dataLength()
                                       - mX.initialBufferSize()));

            // Write to last char of overflow buffer.

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            putOut = 40;
            ASSERT(mX.initialBufferSize() == mX.sputn(source,
                                                      mX.initialBufferSize()));
            ASSERT(putOut == mX.sputn(source + mX.initialBufferSize(),
                                      putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(40       == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(40       == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(60       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE));
            ASSERT(0        == strncmp(source + mX.initialBufferSize(),
                                       mX.overflowBuffer(),
                                       mX.dataLength()
                                       - mX.initialBufferSize()));

            // Write past last char of overflow buffer, triggering another
            // expansion.

            mX.pubseekoff(0, bsl::ios_base::beg, bsl::ios_base::out);

            putOut = 41;
            ASSERT(mX.initialBufferSize() == mX.sputn(source,
                                                      mX.initialBufferSize()));
            ASSERT(putOut == mX.sputn(source + mX.initialBufferSize(),
                                      putOut));

            mX.pubsync();
            ASSERT(k_BUF_SIZE == mX.initialBufferSize());
            ASSERT(160      == mX.overflowBufferSize());
            ASSERT(0        != mX.overflowBuffer());
            ASSERT(buffer   == mX.initialBuffer());
            ASSERT(41       == mX.dataLengthInOverflowBuffer());
            ASSERT(20       == mX.dataLengthInInitialBuffer());
            ASSERT(61       == mX.dataLength());
            ASSERT(0        == strncmp(source, mX.initialBuffer(),
                                       k_BUF_SIZE));
            ASSERT(0        == strncmp(source + mX.initialBufferSize(),
                                       mX.overflowBuffer(),
                                       mX.dataLength()
                                       - mX.initialBufferSize()));
        }

        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   A capitalizing stream.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Given the above two functions, we can now write 'main', as follows:
//..
    // my_app.m.cpp

    bslma::TestAllocator allocator;

    {
        my_CapitalizingStream cs(&allocator);
        cs << "Hello" << ' ' << "world." << '\0';

        ASSERT(10 == cs.streamBuf()->dataLengthInInitialBuffer());
        ASSERT(0 == strncmp("HELLO WORLD", cs.streamBuf()->initialBuffer(),
                            cs.streamBuf()->dataLengthInInitialBuffer()));
        ASSERT(3 == cs.streamBuf()->dataLengthInOverflowBuffer());
        ASSERT(0 == strncmp("D.", cs.streamBuf()->overflowBuffer(),
                            cs.streamBuf()->dataLengthInOverflowBuffer()));
    }

    ASSERT(0 <  allocator.numAllocations());
    ASSERT(0 == allocator.numBytesInUse());
//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SEEKOFF/SEEKPOS TEST
        //
        // Concerns:
        //   A. Seekoff and seekpos into valid areas (including both positive
        //      and negative seeks) will return the final position.  If the
        //      position sought is beyond the total capacity, grow the
        //      stream.
        //   B. Seekoff and seekpos positions the "cursor" (i.e., pptr()) at
        //      the correct location.
        //   C. Seekoff and seekpos into invalid areas will return pos_type(-1)
        //      and does not throw or abort or crash the program.
        //   D. Seeking in the "get" area has no effect.
        //
        //
        // Plan:
        //   To address all concerns, perform a variety of seeks that cover the
        //   entire range of possible categorized starting states (with/without
        //   overflow buffer) and possible ending states (seek to range from
        //   negative positions to past the overflow buffer).  Starting states
        //   will be from starting position at 0, to starting position at 4x
        //   initial buffer size.  Offset values will be from -4x initial
        //   buffer size to 4x initial buffer size.  Seekoff from beg, cur and
        //   end will be done for all of these tests.  On success the return
        //   value is verified to be the new offset into the buffer, and the
        //   new position verified to be correct.  Otherwise, the return value
        //   is verified to be -1, and position unchanged from the starting
        //   position.
        //
        // Tactics:
        //   - Brute-Force Techniques for 'seekpos' and 'seekoff'
        //
        // Testing:
        // virtual pos_type seekoff(
        //                    off_type                offset,
        //                    bsl::ios_base::seekdir  fixedPosition,
        //                    bsl::ios_base::openmode which = bsl::ios_base::in
        //                                               | bsl::ios_base::out);
        // virtual pos_type seekpos(
        //                    pos_type                position,
        //                    bsl::ios_base::openmode which = bsl::ios_base::in
        //                                               | bsl::ios_base::out);
        // --------------------------------------------------------------------
        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout << endl
                          << "SEEKOFF/SEEKPOS TESTS" << endl
                          << "=====================" << endl;
        {
            if (verbose) { T_ cout << "\nTesting seekoff." << endl;}

            int startRangeBeg =  0;
            int startRangeEnd =  INITIAL_BUFSIZE * 4;

            int offsetRangeBeg = -INITIAL_BUFSIZE * 4;
            int offsetRangeEnd =  INITIAL_BUFSIZE * 4;

            for (int start = startRangeBeg; start < startRangeEnd; start++) {
                for (int offset = offsetRangeBeg; offset < offsetRangeEnd;
                                                                    offset++) {
                    int ret;      // return of pubseekoff.
                    int finalpos; // final position.
                    char buffer[INITIAL_BUFSIZE];

                    {
                        bdlsb::OverflowMemOutStreamBuf
                                             mSB(buffer, INITIAL_BUFSIZE, &ta);

                        // Initialize start position.

                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        // Test pubseekoff from beginning, while currently not
                        // at the beginning..

                        ret = mSB.pubseekoff(offset, bsl::ios_base::beg,
                                             bsl::ios_base::out);

                        if (0 > offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == mSB.dataLength());
                        }
                        else {
                            ASSERT(offset == ret);
                            ASSERT(mSB.dataLength() == ret);
                        }
                    }
                    {
                        bdlsb::OverflowMemOutStreamBuf
                                             mSB(buffer, INITIAL_BUFSIZE, &ta);

                        // Initialize start position.

                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        // Test from current position.

                        ret = mSB.pubseekoff(offset, bsl::ios_base::cur,
                                             bsl::ios_base::out);

                        if (0 > start + offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == mSB.dataLength());
                        }
                        else {
                            ASSERT(start + offset == ret);
                            ASSERT(mSB.dataLength() == ret);
                        }
                    }
                    {
                        bdlsb::OverflowMemOutStreamBuf
                                             mSB(buffer, INITIAL_BUFSIZE, &ta);

                        // Initialize start position.

                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        int capacity = mSB.initialBufferSize()
                                     + mSB.overflowBufferSize();

                        // Test pubseekoff from end, while currently not at the
                        // beginning.

                        ret = mSB.pubseekoff(offset, bsl::ios_base::end,
                                             bsl::ios_base::out);

                        if (0 > capacity + offset ) {
                            ASSERT(-1 == ret);
                            ASSERT(start == mSB.dataLength());
                        }
                        else {
                            ASSERT(capacity + offset == ret);
                            ASSERT(mSB.dataLength() == ret);
                        }
                    }

                    // Moving bsl::ios_base::in should always fail.

                    {
                        bdlsb::OverflowMemOutStreamBuf
                                             mSB(buffer, INITIAL_BUFSIZE, &ta);
                        for (int i = 0; i < start; i++ ) {
                            mSB.sputc('A');
                        }

                        ASSERT(bsl::streambuf::pos_type(-1) ==
                                     mSB.pubseekoff(offset, bsl::ios_base::cur,
                                                    bsl::ios_base::in));
                        ASSERT(bsl::streambuf::pos_type(-1) ==
                                     mSB.pubseekoff(offset, bsl::ios_base::beg,
                                                    bsl::ios_base::in));
                        ASSERT(bsl::streambuf::pos_type(-1) ==
                                     mSB.pubseekoff(offset, bsl::ios_base::end,
                                                    bsl::ios_base::in));
                    }

                }
            }
        }
        {
            if (verbose) { T_ cout << "\nTesting seekpos." << endl;}

            int offsetRangeBeg = -INITIAL_BUFSIZE;
            int offsetRangeEnd = INITIAL_BUFSIZE * 4;

            for (int offset = offsetRangeBeg; offset < offsetRangeEnd;
                                                                offset++) {
                char buffer[INITIAL_BUFSIZE];
                bdlsb::OverflowMemOutStreamBuf
                                         mSB(buffer, INITIAL_BUFSIZE, &ta);

                int ret;

                ret = mSB.pubseekpos(offset, bsl::ios_base::out);

                ASSERT((-1 == ret && 0 == mSB.dataLength() && offset < 0)
                    || (offset == ret && ret == mSB.dataLength()));

                // Moving bsl::ios_base::in should always fail.

                ASSERT(bsl::streambuf::pos_type(-1) ==
                                mSB.pubseekpos(offset, bsl::ios_base::in));
                ASSERT(bsl::streambuf::pos_type(-1) ==
                                mSB.pubseekpos(offset, bsl::ios_base::in));
                ASSERT(bsl::streambuf::pos_type(-1) ==
                                mSB.pubseekpos(offset, bsl::ios_base::in));

            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PUT string TEST
        //
        // Concerns:
        //   - that string of varying lengths are written correctly
        //   - that writing strings does not overwrite existing buffer
        //     contents
        //   - that no more than the specified number of characters are written
        //   - that no writing happens beyond existing capacity
        //
        // Plan:
        //   Write out representative strings from the categories 0 characters,
        //   1 character, and > 1 character, into streambufs with
        //   representative contents "empty", substantially less than
        //   capacity, and almost-full-so-that-next-write-exceeds-capacity
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   streamsize xsputn(const char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT STRING TEST" << endl
                          << "===============" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            if (verbose) cout << "\nTesting sputn." << endl;

            const struct TestData {
                int         d_line;          // line number
                const char *d_outStr;        // string to output
                int         d_initSize;      // stream capacity
                const char *d_initData;      // Initial data in initial and
                                             // overflow buffer.
                const char *d_initResult;    // expected contents after 'sputc'
                const char *d_overResult;    // expected contents after 'sputc'
            } DATA[] = {
              //           init
              //L#  out    buf  init     init     over
              //    str    size data     resul    resul
              //--  ------ ---- ----     -----    -----
              // N = 0,0 sized initial buffers not allowed.
              // N = 1
              { L_, "",    1,   "",      "",      ""       },
              { L_, "s",   1,   "a",     "a",     "s"      },
              { L_, "abc", 1,   "ab",    "a",     "babc"   },
              { L_, "abc", 1,   "abc",   "a",     "bcabc"  },
              // N = 2
              { L_, "",    2,   "",      "",      ""       },
              { L_, "s",   2,   "a",     "as",    ""       },
              { L_, "abc", 2,   "a",     "aa",    "bc"     },
              { L_, "abc", 2,   "abc",   "ab",    "cabc"   },
              { L_, "abc", 2,   "abcd",  "ab",    "cdabc"  },
              { L_, "abc", 2,   "abcde", "ab",    "cdeabc" },
              // N = 3
              { L_, "s",   3,   "ab",    "abs",   ""       },
              { L_, "abc", 3,   "ab",    "aba",   "bc"     },
              { L_, "abc", 3,   "abcd",  "abc",   "dabc"   },
              { L_, "abc", 3,   "abcde", "abc",   "deabc"  },
              // N = 4
              { L_, "s",   4,   "ab",    "abs",   ""       },
              { L_, "abc", 4,   "ab",    "abab",  "c"      },
              { L_, "abc", 4,   "abc",   "abca",  "bc"     },
              { L_, "abc", 4,   "abcd",  "abcd",  "abc"    },
              { L_, "abc", 4,   "abcde", "abcd",  "eabc"   },
              // N = 5
              { L_, "abc", 5,   "ab",    "ababc", ""       },
              { L_, "abc", 5,   "abcde", "abcde", "abc"    }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // buffer states (buffer length x buffer contents.)

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;

                char *bytes = new char[DATA[i].d_initSize];
                bdlsb::OverflowMemOutStreamBuf sb(bytes,
                                                     DATA[i].d_initSize, &ta);
                for (unsigned j = 0; j < strlen(DATA[i].d_initData); ++j ) {
                    sb.sputc(DATA[i].d_initData[j]);
                }
                sb.pubsync();
                int retResult = sb.sputn(DATA[i].d_outStr,
                                         strlen(DATA[i].d_outStr));

                sb.pubsync();
                LOOP_ASSERT(LINE, 0 == strncmp(sb.initialBuffer(),
                                               DATA[i].d_initResult,
                                               strlen(DATA[i].d_initResult )));
                LOOP_ASSERT(LINE, 0 == strncmp(sb.overflowBuffer(),
                                               DATA[i].d_overResult,
                                               strlen(DATA[i].d_overResult )));
                LOOP_ASSERT(LINE, bsl::strlen(DATA[i].d_outStr) == retResult);
                LOOP_ASSERT(LINE, DATA[i].d_initSize ==
                                                       sb.initialBufferSize());
                LOOP_ASSERT(LINE, sb.dataLengthInInitialBuffer()
                                              == strlen(DATA[i].d_initResult));
                LOOP_ASSERT(LINE, sb.dataLengthInOverflowBuffer()
                                              == strlen(DATA[i].d_overResult));
                LOOP_ASSERT(LINE, strlen(DATA[i].d_initResult)
                                  + strlen(DATA[i].d_overResult)
                                  == sb.dataLength());
                delete [] bytes;
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // This component has no non-primary constructors, and the destructor
        // does nothing.
        //
        // Testing:
        //   ~bdlsb::FixedMemOutStreamBuf();
        // --------------------------------------------------------------------
        {
            if (verbose) cout << "There are no non-primary constructors, and "
                              << "the destructor does nothing."
                              << endl;

        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OUTPUT: bsl::ostream& 'operator<<'
        //
        // NOTE: 'op<<' is NOT part of the component.  It is, however, an
        // auxiliary function used in the test driver, and so we include the
        // 'traditional' test case 5 here in order to test the operator.
        //
        // Concerns:
        //   - that value is formatted correctly (i.e., as binary)
        //   - that no additional characters are written after terminating.
        //   - that these functions work on references to 'const' instances.
        //   - that each return a reference to the modifiable stream argument.
        //
        // Plan:
        //   For each of a small representative set of object values use
        //   'ostrstream' to write that object's value to two separate
        //   character buffers each with different initial values.  Compare
        //   the contents of these buffers with the literal expected output
        //   format and verify that the characters beyond the length of the
        //   streambuf contents are unaffected in both buffers.
        //   - Test operator<< on the empty streambuf.
        //   - Test operator<< on a streambuf containing some characters.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdlsb::MemOutStreamBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "OUTPUT bsl::ostream& 'operator<<'" << endl
                  << "=================================" << endl;

        if (verbose) cout << "\nCreate stream buffers to be printed." << endl;

        {
            if (verbose) cout << "\tChecking operator<< return value."
                              << endl;

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdlsb::OverflowMemOutStreamBuf mSB(buffer, INITIAL_BUFSIZE);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            bsl::ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << SB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output."
                              << endl;

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                  INITIAL_BUFSIZE);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

            mSB.sputn("hello", 5);

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            memset(buf1, 'X', SIZE);
            memset(buf2, 'Y', SIZE);
            bsl::ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << mSB;
            out2 << SB;
            char *endOfString = strchr(buf1, 'X');
            int stringLen = endOfString - buf1;
            ASSERT(0 == memcmp(buf1, buf2, stringLen));
            ASSERT('X' == buf1[stringLen]);
            ASSERT('Y' == buf2[stringLen]);

            const char *EXPECTED =
                "\nInitial Buffer:"
                "\n0000\t01101000 01100101 01101100 01101100 01101111"
                "\nOverflow Buffer:";
            ASSERT(0 == memcmp(buf1, EXPECTED, stringLen));
            buf1[stringLen] = 0;
            if (verbose) {
                T_ P(buf1)
            }
        }
        {
            if (verbose) cout << "\n\tChecking operator<< output again."
                              << endl;

            // One more test, just to see something different

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                  INITIAL_BUFSIZE);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');
            mSB.pubsync();
            const char *EXPECTED =
                                 "\nInitial Buffer:"
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010"
                                 "\nOverflow Buffer:";
            char ACTUAL[500];
            memset(ACTUAL, 'Z', 500);
            bsl::ostrstream out(ACTUAL, 500);
            out << SB << bsl::ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (verbose) cout << endl;
            if (verbose) {
                T_ P(EXPECTED)
                T_ P(ACTUAL)
            }
            ASSERT('Z' == ACTUAL[LEN]); // check for overrun
            ASSERT(0 == memcmp(ACTUAL, EXPECTED, LEN));
        }
        {
            if (verbose) cout << "\n\tChecking operator<< output again."
                              << endl;

            // One more test, just to see something different

            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                  INITIAL_BUFSIZE);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');
            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');
            mSB.pubsync();
            const char *EXPECTED =
                                 "\nInitial Buffer:"
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010 "
                                         "00110000 00110001 00110010 00110011"
                                 "\n0010\t00110100 00110101 00110110 00110111"
                                 "\nOverflow Buffer:"
                                 "\n0000\t00111000 00111001 01000001 01000010";
            char ACTUAL[500];
            memset(ACTUAL, 'Z', 500);
            bsl::ostrstream out(ACTUAL, 500);
            out << SB << bsl::ends;
            const int LEN = strlen(EXPECTED) + 1;
            if (verbose) cout << endl;
            if (verbose) {
                T_ P(EXPECTED)
                T_ P(ACTUAL)
            }
            ASSERT('Z' == ACTUAL[LEN]); // check for overrun
            ASSERT(0 == memcmp(ACTUAL, EXPECTED, LEN));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a
        //   'bdlsb::OverflowMemOutStreamBuf'.
        //
        // Concerns:
        //   (sputc) MANIPULATOR
        //   - that printing and non-printing characters are written correctly
        //   - that bytes with leading bit set are written correctly
        //   - that no more than one character is written
        //
        //   (seekpos) MANIPULATOR
        //   - that seeking positions the "cursor" (i.e., pptr()), at the
        //     correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //     the program
        //   - that trying to seek in the "get" area has no effect and returns
        //     an error.
        //
        //    (data & length) ACCESSORS
        //   - To ensure that accessors work off of references to 'const'
        //     instances.
        //   - That 'initialBuffer' return the address of the underlying
        //     character array.
        //   - That 'initialBufferSize' returns the capacity of the
        //     'initialBuffer' as specified in the constructor.
        //   - That 'overflowBuffer' return the address of the internally
        //     allocated underlying character array for storing data that does
        //     not fit in the initial buffer.
        //   - That 'overflowBufferSize' returns the capacity of the internally
        //     allocated buffer for data that did not fit in the initial
        //     buffer.
        //   - That 'dataLength' return the number of characters written to the
        //     stream buffer.
        //   - That 'dataLengthInInitialBuffer' return the number of characters
        //     written to the initial buffer.
        //   - That 'dataLengthInOverflowBuffer' return the number of
        //     characters written to the overflow buffer.
        //
        // Plan:
        //   (sputc)
        //   - Write at least one sample value from each category of characters
        //     using the 'sputc' method, and verify that the bit pattern for
        //     that character is present and correct in the stream buffer.
        //
        //   (seekpos)
        //   - Perform a variety of seeks, each testing different settings so
        //     as to address various combinations of the above concerns.
        //
        //   (data and length)
        //   - Create an empty 'bdlsb::OverflowMemOutStreamBuf' and verify
        //     its length.
        //   - Add a character, and verify the length and content.
        //   - Add enough characters to use all the initial capacity, and then
        //     verify length and content.
        //   - Then add one more character so as to exceed initial capacity,
        //     and verify length and content.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method for 'sputc'
        //   - Table-Based and Brute-Force Implementation Technique for
        //     'sputc'
        //   - Ad-Hoc Data Selection Method for 'seekpos'
        //   - Brute-Force Techniques for 'seekpos'
        //
        // Testing:
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        //   int_type sputc(char_type);
        //   pos_type seekoff(off_type                offset,
        //                    bsl::ios_base::seekdir  fixedPosition,
        //                    bsl::ios_base::openmode which)
        //   const char_type *data() const;
        //   streamsize dataLength() const;
        //   int dataLengthInOverflowBuffer() const;
        //   int dataLengthInInitialBuffer() const;
        //   int initialBufferSize() const;
        //   int overflowBufferSize() const;
        //   const char *initialBuffer() const;
        //   const char *overflowBuffer() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout <<
            "\nEmpty streambuf -- checking lengths and sizes." << endl;
        {
            char buffer[INITIAL_BUFSIZE];
            memset(buffer, 'Z', INITIAL_BUFSIZE);
            bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                  INITIAL_BUFSIZE, &ta);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;
            ASSERT(0                   == SB.dataLength());

            ASSERT(INITIAL_BUFSIZE     == SB.initialBufferSize());
            ASSERT(0                   == SB.dataLengthInInitialBuffer());

            ASSERT(0                   == SB.overflowBufferSize());
            ASSERT(0                   == SB.dataLengthInOverflowBuffer());

            ASSERT(SB.initialBuffer()  == buffer);
            ASSERT(SB.overflowBuffer() == 0);

            if (veryVerbose) P(SB);
        }
        if (verbose) cout << "\nTesting sputc." << endl;
        {
            typedef bdlsb::OverflowMemOutStreamBuf::char_type T;

            static const struct {
                int            d_line;       // line number
                T              d_outChar;    // character to output
                const char    *d_result;     // expected contents after 'sputc'
            } DATA[] = {
                  //L#  output char    resulting
                  //                 stream contents
                  //--  -----------  ---------------
                  // Add ordinary ASCII character
                  { L_,    's',            "s"     },
                  // Add non-printing character
                  { L_,    10,             "\xA"   },
                  // 0 <= Add <= 127
                  { L_,    127,            "\x7F"  },
                  // -128 <= Add <= -1
                  { L_,   -128,            "\x80"  }
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. Adds the character, and
            //    2. Does not overwrite beyond the character.

            if (verbose) { T_ cout << "Testing different character types."
                                   << endl; }

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                char buffer[INITIAL_BUFSIZE];
                memset(buffer, 'Z', INITIAL_BUFSIZE);
                bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                      INITIAL_BUFSIZE - 1,
                                                      &ta);
                const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

                mSB.sputc(DATA[i].d_outChar);
                mSB.pubsync();
                LOOP_ASSERT(LINE, DATA[i].d_outChar == SB.initialBuffer()[0]);
                LOOP_ASSERT(LINE, 0   == SB.overflowBuffer());
                LOOP_ASSERT(LINE, 1   == SB.dataLength());
                LOOP_ASSERT(LINE, 1   == SB.dataLengthInInitialBuffer());
                LOOP_ASSERT(LINE, 0   == SB.dataLengthInOverflowBuffer());
                LOOP_ASSERT(LINE, INITIAL_BUFSIZE - 1 ==
                                                       SB.initialBufferSize());
                LOOP_ASSERT(LINE, 0   == SB.overflowBufferSize());
                LOOP_ASSERT(LINE, 'Z' == SB.initialBuffer()[1]);
                if (veryVerbose) { P(SB); }
            }
        }
        {
            if (verbose) { T_ cout <<
                                   "Testing different initial buffer states."
                                   << endl; }

            const struct TestData {
                int         d_line;           // line number
                char        d_outChar;        // character to output
                int         d_initSize;       // initial buffer size
                const char *d_initBufInit;    // init contents of initial buf
                const char *d_overBufInit;    // init contents of overflow buf
                const char *d_initResult;     // expected contents in initial
                                              // buffer after 'sputc'
                const char *d_overResult;     // expected contents in overflow
                                              // buffer after 'sputc'
                int         d_returnVal;      // 'sputc' return val
            } DATA[] = {
               //         init init   over
               //L#  out  buf  buf    buf    init   over    ret
               //    char size init   init   resul  resul   val
               //--  ---- ---- ------ ------ ------ ------- ---
               // N = 0.  Init size 0 is not allowed.  Offenders will be
               // prosecuted to the full extent of the law.
               // N = 1
               { L_,  's',  1, "",    "",    "s",   "",     's' },
               { L_,  's',  1, "a",   "",    "a",   "s",    's' },
               { L_,  's',  1, "a",   "b",   "a",   "bs",   's' },
               { L_,  's',  1, "a",   "bc",  "a",   "bcs",  's' },
               { L_,  's',  1, "a",   "bcd", "a",   "bcds", 's' },
               { L_,  's',  1, "a",   "bcde","a",   "bcdes",'s' },
               // N = 2
               { L_,  's',  2, "",    "",    "s",   "",     's' },
               { L_,  's',  2, "a",   "",    "as",  "",     's' },
               { L_,  's',  2, "ab",  "",    "ab",  "s",    's' },
               { L_,  's',  2, "ab",  "c",   "ab",  "cs",   's' },
               { L_,  's',  2, "ab",  "cd",  "ab",  "cds",  's' },
               { L_,  's',  2, "ab",  "cde", "ab",  "cdes", 's' },
               // N = 3
               { L_,  's',  3, "",    "",    "s",   "",     's' },
               { L_,  's',  3, "ab",  "",    "abs", "",     's' },
               { L_,  's',  3, "abc", "",    "abc", "s",    's' },
               { L_,  's',  3, "abc", "d",   "abc", "ds",   's' },
               { L_,  's',  3, "abc", "de",  "abc", "des",  's' },
               // N = 4
               { L_,  's',  4, "",    "",    "s",   "",     's' },
               { L_,  's',  4, "ab",  "",    "abs", "",     's' },
               { L_,  's',  4, "abcd","",    "abcd","s",    's' },
               { L_,  's',  4, "abcd","e",   "abcd","es",   's' },
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // buffer states (buffer length x buffer contents.)

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                char *bytes         = new char[DATA[i].d_initSize];
                bdlsb::OverflowMemOutStreamBuf mSB(bytes,
                                                      DATA[i].d_initSize, &ta);
                const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initBufInit); ++j) {
                    mSB.sputc(DATA[i].d_initBufInit[j]);
                }
                mSB.pubsync();
                for (unsigned j = 0; j < strlen(DATA[i].d_overBufInit); ++j) {
                    ASSERT(SB.dataLengthInInitialBuffer()
                                                    == SB.initialBufferSize());
                    mSB.sputc(DATA[i].d_overBufInit[j]);
                }

                mSB.pubsync();
                if (veryVerbose) { T_ cout << "Initial contents: "; P(SB) };
                int retResult = mSB.sputc(DATA[i].d_outChar);

                mSB.pubsync();
                LOOP_ASSERT(LINE, 0 == strncmp(
                                      SB.initialBuffer(), DATA[i].d_initResult,
                                      strlen(DATA[i].d_initResult)));
                LOOP_ASSERT(LINE, 0 == strncmp(
                                     SB.overflowBuffer(), DATA[i].d_overResult,
                                     strlen(DATA[i].d_overResult)));
                LOOP_ASSERT(LINE, DATA[i].d_returnVal  == retResult);
                LOOP_ASSERT(LINE, strlen(DATA[i].d_initResult) ==
                                               SB.dataLengthInInitialBuffer());
                LOOP_ASSERT(LINE, strlen(DATA[i].d_overResult) ==
                                              SB.dataLengthInOverflowBuffer());
                LOOP_ASSERT(LINE, strlen(DATA[i].d_initResult) +
                                  strlen(DATA[i].d_overResult)
                                  == SB.dataLength());

                LOOP_ASSERT(LINE, DATA[i].d_initSize   ==
                                                       SB.initialBufferSize());
                delete [] bytes;
            }
        }
        {
            if (verbose) {
                    cout <<
                  "\n\tVerifying that overflow does not corrupt the streambuf."
                                   << endl; }

            // Do an extra test to ensure that overflow does not corrupt the
            // stream.
            char buffer[5];
            memset(buffer, 'X', 5);
            bdlsb::OverflowMemOutStreamBuf x(buffer, 5, &ta);
            x.sputc('a'); x.sputc('b'); x.sputc('c'); x.sputc('d');
            x.sputc('e');

            x.pubsync();
            ASSERT(0 == strncmp(x.initialBuffer(), "abcde",
                                x.dataLengthInInitialBuffer()));
            ASSERT(0 == x.dataLengthInOverflowBuffer());
            ASSERT(0 == x.overflowBufferSize());
            ASSERT(5 == x.initialBufferSize());
            ASSERT(0 == x.overflowBuffer());
            ASSERT(5 == x.dataLengthInInitialBuffer());
            ASSERT(5 == x.dataLength());

            char putChar = 'f';
            x.sputc(putChar);
            putChar++;

            x.pubsync();
            ASSERT(0 == strncmp(x.initialBuffer(), "abcde",
                                x.dataLengthInInitialBuffer()));
            ASSERT(0 == strncmp(x.overflowBuffer(), "f",
                                x.dataLengthInOverflowBuffer()));
            ASSERT(0 != x.overflowBufferSize());
            ASSERT(5 == x.initialBufferSize());
            ASSERT(5 == x.dataLengthInInitialBuffer());
            ASSERT(1 == x.dataLengthInOverflowBuffer());
            ASSERT(6 == x.dataLength());

            int overflowSize = x.overflowBufferSize();
            for (int i = x.dataLengthInOverflowBuffer();
                                             i < x.overflowBufferSize(); i++) {
                x.sputc(putChar);
                putChar++;
            }
            x.pubsync();

            ASSERT(0  == strncmp(x.initialBuffer(), "abcde",
                                 x.dataLengthInInitialBuffer()));
            ASSERT(0  == strncmp(x.overflowBuffer(), "fghijklmnopqrstuvwxyz",
                                 x.dataLengthInOverflowBuffer()));
            ASSERT(overflowSize == x.overflowBufferSize());
            ASSERT(5  == x.initialBufferSize());
            ASSERT(5  == x.dataLengthInInitialBuffer());
            ASSERT(overflowSize == x.dataLengthInOverflowBuffer());
            ASSERT(overflowSize + 5 == x.dataLength());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the constructor sets up all streambuf machinery properly
        //     so that streambuf operations use the client-supplied buffer.
        //   - That the streambuf is capable of using the entirety of the
        //     available storage in the initial buffer, i.e., the full amount
        //     specified by the client.
        //   - On overflow, additional memory comes from the supplied
        //     allocator.
        //
        // Plan:
        //   Write one character using 'sputc' to the stream, verifying via
        //   accessors 'initialBuffer', 'initialBufferSize',
        //   'dataLengthInInitialBuffer', 'overflowBufferSize',
        //   'overflowBuffer', 'dataLengthInOverflowBuffer' that the
        //   'initialBuffer' is filled before memory is allocated for
        //   additional data.  We will write enough characters to verify that
        //   the length as specified is completely usable, and then write one
        //   more to ensure that the stream buffer allocates from the supplied
        //   allocator and uses the overflow buffer for additional data.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        // bdlsb::OverflowMemOutStreamBuf(char *buffer,
        //                               int               size,
        //                               bslma::Allocator *basicAllocator = 0)
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout <<
            "\nEnsure that the stream buffer uses client-specified buffer."
                          << endl;
        {
            if (verbose) cout <<
             "\nEnsure that the stream buffer has at least specified capacity."
                              << endl;

             char buffer[INITIAL_BUFSIZE];
             memset(buffer, 'Z', INITIAL_BUFSIZE);
             bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                   INITIAL_BUFSIZE - 1, &ta);
             const bdlsb::OverflowMemOutStreamBuf& SB = mSB;

             for (int i = 0; i < INITIAL_BUFSIZE - 1; ++i) {
                 mSB.sputc('a');
                 LOOP_ASSERT(i, 'a' == buffer[i]);
                 LOOP_ASSERT(i, 'Z' == buffer[i + 1]);

                 mSB.pubsync();
                 ASSERT(INITIAL_BUFSIZE - 1
                               == SB.initialBufferSize());
                 ASSERT(0      == SB.overflowBufferSize());
                 ASSERT(buffer == SB.initialBuffer());
                 ASSERT(0      == SB.overflowBuffer());
                 ASSERT(i + 1  == SB.dataLengthInInitialBuffer());
                 ASSERT(0      == SB.dataLengthInOverflowBuffer());
                 ASSERT(i + 1  == SB.dataLength());
                 ASSERT(0      == ta.numAllocations());
                 ASSERT(0      == ta.numBytesInUse());

                 if (veryVerbose) P(SB);
             }

             if (verbose) cout << "\nEnsure that the stream buffer will "
                               << "allocate an overflow buffer if needed"
                               << endl;

             mSB.sputc('a');
             mSB.pubsync();
             ASSERT('Z' == buffer[INITIAL_BUFSIZE - 1]);

             ASSERT(INITIAL_BUFSIZE - 1 == SB.initialBufferSize());
             ASSERT(0                   != SB.overflowBufferSize());
             ASSERT(buffer              == SB.initialBuffer());
             ASSERT(0                   != SB.overflowBuffer());
             ASSERT('a'                 == *SB.overflowBuffer());
             ASSERT(INITIAL_BUFSIZE - 1 == SB.dataLengthInInitialBuffer());
             ASSERT(1                   == SB.dataLengthInOverflowBuffer());
             ASSERT(INITIAL_BUFSIZE     == SB.dataLength());
             ASSERT(1                   == ta.numAllocations());
             ASSERT(0                   != ta.numBytesInUse());

        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the equipment we have set up to test itself works properly.
        //
        // Concerns:
        //     This driver uses no test apparatus.
        //
        // Plan:
        //      N/A
        //
        // Tactics:
        //   - N/A
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << endl
                          << "This driver uses no apparatus." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test merely exercises basic functionality.
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Do whatever is needed:
        //   - Try writing out to a stream buffer.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
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

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            char buffer[INITIAL_BUFSIZE];
            bdlsb::OverflowMemOutStreamBuf mSB(buffer,
                                                  INITIAL_BUFSIZE, &ta);
            const bdlsb::OverflowMemOutStreamBuf& SB = mSB;
            char source[] = "hellos, This is 30 chars long.";
            ASSERT(30 == bsl::strlen(source));

            if (verbose) { cout <<
                 "\tCreate a fixed-length output stream buffer: "; P(SB) }
            ASSERT(0 == SB.dataLength());

            if (verbose) { cout <<
                      "\n\tWrite a string (five chars) to the stream buffer: ";
                           P(SB) }
            mSB.sputn(source, 5);
            ASSERT(5  == SB.dataLength());

            ASSERT(INITIAL_BUFSIZE == SB.initialBufferSize());
            ASSERT(5               == SB.dataLengthInInitialBuffer());
            ASSERT(0               == strncmp(SB.initialBuffer(), source,
                                              SB.dataLengthInInitialBuffer()));

            ASSERT(0               == SB.overflowBufferSize());
            ASSERT(0               == SB.dataLengthInOverflowBuffer());
            ASSERT(0               == strncmp(SB.overflowBuffer(),
                                              source +
                                              SB.dataLengthInInitialBuffer(),
                                             SB.dataLengthInOverflowBuffer()));

            if (verbose) { cout <<
                     "\n\tWrite a single char to the stream buffer: "; P(SB) }
            mSB.sputc(*(source + SB.dataLength()));

            mSB.pubsync();
            ASSERT(6               == SB.dataLength());

            ASSERT(INITIAL_BUFSIZE == SB.initialBufferSize());
            ASSERT(6               == SB.dataLengthInInitialBuffer());
            ASSERT(0               == strncmp(SB.initialBuffer(), source,
                                              SB.dataLengthInInitialBuffer()));

            ASSERT(0               == SB.overflowBufferSize());
            ASSERT(0               == SB.dataLengthInOverflowBuffer());
            ASSERT(0               == strncmp(SB.overflowBuffer(),
                                              source
                                              + SB.dataLengthInInitialBuffer(),
                                             SB.dataLengthInOverflowBuffer()));

            if (verbose) { cout <<
                        "\n\tWrite a string (23 chars) to the stream buffer: ";
                           P(SB) }
            mSB.sputn(source + SB.dataLength(),
                      bsl::strlen(source) - SB.dataLength());

            mSB.pubsync();
            ASSERT(bsl::strlen(source) == SB.dataLength());

            ASSERT(INITIAL_BUFSIZE == SB.initialBufferSize());
            ASSERT(INITIAL_BUFSIZE == SB.dataLengthInInitialBuffer());
            ASSERT(0               == strncmp(SB.initialBuffer(), source,
                                              SB.dataLengthInInitialBuffer()));

            ASSERT(INITIAL_BUFSIZE * 2 == SB.overflowBufferSize());
            ASSERT(10                  == SB.dataLengthInOverflowBuffer());
            ASSERT(0                   == strncmp(SB.overflowBuffer(),
                                                  source +
                                                SB.dataLengthInInitialBuffer(),
                                             SB.dataLengthInOverflowBuffer()));

            mSB.sputc('s');
            if (verbose) { cout <<
                     "\n\tWrite a single char to the stream buffer: "; P(SB) }

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
