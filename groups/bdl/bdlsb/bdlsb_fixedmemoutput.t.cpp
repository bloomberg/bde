// bdlsb_fixedmemoutput.t.cpp                                         -*-C++-*-
#include <bdlsb_fixedmemoutput.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>

#include <bslx_genericoutstream.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This test driver exercises all the public methods from the 'basic_streambuf'
// protocol that are implemented by the class 'bdlsb::FixedMemOutput', as well
// as each public method in the 'bdlsb::FixedMemOutput' class that is not part
// of the 'basic_streambuf' protocol.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
// Note that output operator used for test tracing purposes is tested in test
// case 3.
//
// Primary Constructors:
//: o FixedMemOutput(char *buffer, bsl::streamsize length);
//
// Primary Manipulators:
//: o int_type sputc(char_type);
//
// Basic Accessors:
//: o const char_type *data();
//: o streamsize length();
//: o streamsize capacity();
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FixedMemOutput(char *buffer, bsl::streamsize length);
// [ 2] ~FixedMemOutput();
//
// MANIPULATORS
// [ 8] char *data();
// [ 7] FixedMemOutput *pubsetbuf(char *buffer, streamsize length);
// [ 6] pos_type pubseekpos(pos_type, openmode);
// [ 6] pos_type pubseekoff(off_type, seekdir, openmode);
// [ 2] int_type sputc(char_type);
// [ 5] streamsize sputn(const char_type, streamsize);
// [ 9] locale pubimbue(const locale& loc);
// [ 9] int pubsync();
//
// ACCESSORS
// [ 4] streamsize capacity() const;
// [ 4] const char_type *data() const;
// [ 4] streamsize length() const;
// [ 9] bsl::locale getloc() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] ostream& operator<<(ostream&, const FixedMemOutput&);
// [10] USAGE EXAMPLE
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
typedef bdlsb::FixedMemOutput Obj;
typedef bsl::ios_base::openmode io_openmode;
typedef bsl::ios_base::seekdir  io_seekdir;

const int INIT_BUFSIZE = 20;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                  // ===============================
                  // operator<< for bdlsb::MemOutput
                  // ===============================
// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdlsb::FixedMemOutput& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdlsb::FixedMemOutput& streamBuffer)
{
    const bsl::streamsize  len  = streamBuffer.length();
    const char            *data = streamBuffer.data();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
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
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
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
///Example 1: Basic Use of 'bdlsb::FixedMemOutput'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates usage of a stream buffer by a stream, in this case
// a 'bslx::GenericOutStream'.
//
// First, we create an object of our stream buffer:
//..
    enum { k_STREAMBUF_CAPACITY = 30 };

    char                  buffer[k_STREAMBUF_CAPACITY];
    bdlsb::FixedMemOutput streamBuf(buffer, k_STREAMBUF_CAPACITY);
//..
// Then, we create an instance of 'bslx::GenericOutStream' using 'streamBuf',
// with an arbitrary value for its 'versionSelector', and externalize some
// values:
//..
    bslx::GenericOutStream<bdlsb::FixedMemOutput> outStream(&streamBuf,
                                                            20150707);
    outStream.putInt32(1);
    outStream.putInt32(2);
    outStream.putInt8('c');
    outStream.putString(bsl::string("hello"));
//..
// Finally, we compare the contents of the buffer to the expected value:
//..
    ASSERT(15 == streamBuf.length());
    ASSERT( 0 == bsl::memcmp(streamBuf.data(),
                             "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
                             15));
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'NO EFFECT' METHODS
        //  As 'pubimbue', 'pubsync' and 'getloc' methods have no effect, we
        //  can only ensure that these methods can be instantiated and return
        //  expected values.
        //
        // Concerns:
        //: 1 Methods can be instantiated.
        //:
        //: 2 Methods return expected values.
        //
        // Plan:
        //: 1 Create FixedMemOutput object.  Manually call methods and verify
        //:   returned values.  (C-1..2)
        //
        // Testing:
        //   locale pubimbue(const locale& loc);
        //   int pubsync();
        //   bsl::locale getloc() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'NO EFFECT' METHODS" << endl
                          << "===========================" << endl;

        char buffer[INIT_BUFSIZE];
        memset(buffer, 'Z', INIT_BUFSIZE);
        Obj mSB(buffer, INIT_BUFSIZE);

        if (verbose) {
            cout << "\nTesting 'no effect' methods" << endl;
        }

        bsl::locale loc;

        ASSERT(loc == mSB.pubimbue(loc));
        ASSERT(0   == mSB.pubsync());
        ASSERT(loc == mSB.getloc());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'data' METHOD
        //
        // Concerns:
        //: 1 'data' method return a pointer providing modifiable access to the
        //:   character buffer held by this stream buffer (supplied at
        //:   construction).
        //
        // Plan:
        //: 1 Create FixedMemOutput object.  Verify that we can modify
        //:   character buffer, using pointer, returned by 'data' method call.
        //:   (C-1)
        //
        // Testing:
        //   char *data();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'data' METHOD" << endl
                          << "=====================" << endl;

        char buffer[INIT_BUFSIZE];
        memset(buffer, 'Z', INIT_BUFSIZE);
        Obj mSB(buffer, INIT_BUFSIZE);

        if (verbose) {
            cout << "\nClient provided character buffer changing." << endl;
        }
        ASSERT('Z' == buffer[0]);

        *(mSB.data()) = 'a';
        ASSERT('a' == buffer[0]);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'pubsetbuf' METHOD
        //   Ensure that we can reset put area to client-provided buffer for a
        //   constructed stream buffer object
        //
        // Concerns:
        //: 1 The 'pubsetbuf' method can reset internal buffer.
        //:
        //: 2 The 'pubsetbuf' method can replace internal buffer with new user
        //:   provided buffer.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Manually call 'pubsetbuf' method and verify that the buffer has
        //:   been reset to the new address and length. (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros). (C-3)
        //
        // Testing:
        //  FixedMemOutput *pubsetbuf(char *buffer, streamsize length);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'pubsetbuf' METHOD" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nBuffer resetting." << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;
            mSB.sputc('a');

            ASSERT(buffer       == SB.data());
            ASSERT(INIT_BUFSIZE == SB.capacity());
            ASSERT(1            == SB.length());

            mSB.pubsetbuf(0, 0);

            ASSERT(0            == SB.data());
            ASSERT(0            == SB.capacity());
            ASSERT(0            == SB.length());
        }

        if (verbose) cout << "\nBuffer replacement." << endl;
        {
            char buffer1[INIT_BUFSIZE];
            memset(buffer1, 'Z', INIT_BUFSIZE);
            char buffer2[INIT_BUFSIZE];
            memset(buffer2, 'A', INIT_BUFSIZE);

            Obj        mSB(buffer1, INIT_BUFSIZE);
            const Obj& SB = mSB;
            ASSERT(buffer1      == SB.data());
            ASSERT(INIT_BUFSIZE == SB.capacity());
            ASSERT(0            == SB.length());

            mSB.sputc('a');
            ASSERT(1            == SB.length());

            mSB.pubsetbuf(buffer2, INIT_BUFSIZE/2);
            ASSERT(buffer2 == SB.data());
            ASSERT(INIT_BUFSIZE/2 == SB.capacity());
            ASSERT(0 == SB.length());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            char buffer[INIT_BUFSIZE];
            Obj  mSB(buffer, INIT_BUFSIZE);

            bsls::AssertFailureHandlerGuard hG(
                                         bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(mSB.pubsetbuf(     0,  INIT_BUFSIZE));
            ASSERT_SAFE_FAIL(mSB.pubsetbuf(     0,            -1));
            ASSERT_SAFE_FAIL(mSB.pubsetbuf(buffer,            -1));
            ASSERT_SAFE_PASS(mSB.pubsetbuf(     0,             0));
            ASSERT_SAFE_PASS(mSB.pubsetbuf(buffer,             0));
            ASSERT_SAFE_PASS(mSB.pubsetbuf(buffer,  INIT_BUFSIZE));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //   As the only action performed in 'pubseekpos' is the call for
        //   'pubseekoff' with predetermined second parameter, then we can test
        //   'pubseekpos' superficially.
        //
        // Concerns:
        //: 1 Seeking is correct for:
        //:   - all relative positions.
        //:   - positive, 0, and negative values.
        //:   - out of buffer boundaries.
        //:
        //: 2 Seeking into the "get" area has no effect.
        //:
        //: 3 'pubseekpos' calls 'pubseekoff' with correctly predetermined
        //:   second parameter (bsl::ios_base::beg)
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-1..2)
        //:
        //: 2 Perform several seeks with different initial states of the
        //:   tested object. (C-3)
        //
        // Testing:
        //   pos_type pubseekoff(off_type, seekdir, openmode);
        //   pos_type pubseekpos(pos_type, openmode);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'seek' METHODS" << endl
                          << "======================" << endl;

        const io_openmode PUT = bsl::ios_base::out;
        const io_openmode GET = bsl::ios_base::in;
        const io_seekdir  CUR = bsl::ios_base::cur;
        const io_seekdir  BEG = bsl::ios_base::beg;
        const io_seekdir  END = bsl::ios_base::end;

        char mFILL[INIT_BUFSIZE];
        memset(mFILL, 'a', sizeof mFILL);

        if (verbose) cout << "\nTesting seekoff with no buffer." << endl;
        {
            Obj           mSB(0, 0);
            Obj::pos_type ret;

            ret = mSB.pubseekoff(1, BEG, PUT);
            ASSERT(-1 == ret);
        }

        if (verbose) cout << "\nTesting seekoff from beginning and end."
                          << endl;
        {
            static const struct {
                int           d_line;          // line number
                io_openmode   d_areaFlags;     // "put"/"get" area flag
                Obj::off_type d_amount;        // seek offset
                io_seekdir    d_base;          // relative position
                int           d_retVal;        // expected return from seekoff
            } DATA[] = {
               //LINE AREA               OFFSET  POS                RETVAL
               //---- ---- --------------------- ----  -------------------
               // seekoff from the start of the streambuf
               { L_,  PUT,                    0, BEG,                    0  },
               { L_,  PUT,                    1, BEG,                    1  },
               { L_,  PUT,                   -1, BEG,                   -1  },
               { L_,  PUT,     INIT_BUFSIZE - 1, BEG,     INIT_BUFSIZE - 1  },
               { L_,  PUT,         INIT_BUFSIZE, BEG,         INIT_BUFSIZE  },
               { L_,  PUT,     INIT_BUFSIZE + 1, BEG,                   -1  },
               { L_,  PUT,     INIT_BUFSIZE + 9, BEG,                   -1  },

               // seekoff from the end of the streambuf.
               { L_,  PUT,    -INIT_BUFSIZE - 9, END,                   -1  },
               { L_,  PUT,    -INIT_BUFSIZE - 1, END,                   -1  },
               { L_,  PUT,        -INIT_BUFSIZE, END,                    0  },
               { L_,  PUT,    -INIT_BUFSIZE + 1, END,                    1  },
               { L_,  PUT,                  -10, END,    INIT_BUFSIZE - 10  },
               { L_,  PUT,                    0, END,         INIT_BUFSIZE  },
               { L_,  PUT,                    1, END,                   -1  },

               // seekoff from the current cursor, where cur == end
               { L_,  PUT,    -INIT_BUFSIZE - 9, CUR,                   -1  },
               { L_,  PUT,    -INIT_BUFSIZE - 1, CUR,                   -1  },
               { L_,  PUT,        -INIT_BUFSIZE, CUR,                    0  },
               { L_,  PUT,    -INIT_BUFSIZE + 1, CUR,                    1  },
               { L_,  PUT,                  -10, CUR,    INIT_BUFSIZE - 10  },
               { L_,  PUT,                    0, CUR,         INIT_BUFSIZE  },
               { L_,  PUT,                    1, CUR,                   -1  },

               // seekoff in the "get" area
               { L_,  GET,                    0, BEG,                   -1  },
               { L_,  GET,                    0, END,                   -1  },
               { L_,  GET,                    0, CUR,                   -1  },
               { L_,  GET,                    1, BEG,                   -1  },
               { L_,  GET,                    1, END,                   -1  },
               { L_,  GET,                    1, CUR,                   -1  },
               { L_,  GET,                   -1, BEG,                   -1  },
               { L_,  GET,                   -1, END,                   -1  },
               { L_,  GET,                   -1, CUR,                   -1  },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal : INIT_BUFSIZE);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);
                Obj           mSB(buffer, INIT_BUFSIZE);
                const Obj&    SB = mSB;
                Obj::pos_type ret;
                mSB.sputn(mFILL, INIT_BUFSIZE);

                ret = mSB.pubseekoff(DATA[i].d_amount,
                                     DATA[i].d_base,
                                     DATA[i].d_areaFlags);
                ASSERTV(LINE, DATA[i].d_retVal == ret);
                if (veryVerbose) P(ret);

                ASSERTV(LINE, DATA[i].d_retVal == ret);
                ASSERTV(LINE, FINAL_POS == SB.length());
                ASSERTV(LINE, 0 == bsl::memcmp(mFILL, buffer, SB.length()));
            }
        }

        if (verbose) cout << "\nTesting seekoff from a variety of"
                          << " current-pointer positions."
                          << endl;
        {
            static const struct {
                int         d_line;             // line number
                int         d_offset;           // seek offset
                int         d_initialPosition;  // where to set pptr() first
                int         d_retVal;           // expected return value
            } DATA[] = {
               //LINE            OFFSET       STARTPOS              RETVAL
               //----  ----------------  -------------  ------------------
               { L_,                 -1,             0,                 -1  },
               { L_,                  0,             0,                  0  },
               { L_,                  0,            10,                 10  },
               { L_,   INIT_BUFSIZE - 1,             0,                 -1  },
               { L_,       INIT_BUFSIZE,             0,                 -1  },
               { L_,   INIT_BUFSIZE + 1,             0,                 -1  },
               { L_,   INIT_BUFSIZE + 9,             0,                 -1  },

               { L_,               -110,            10,                 -1  },
               { L_,                -11,            10,                 -1  },
               { L_,                -10,            10,                  0  },
               { L_,                 -9,            10,                  1  },
               { L_,                  0,            10,                 10  },
               { L_,                 10,            10,                 -1  },
               { L_,                 11,            10,                 -1  },
               { L_,                200,            10,                 -1  },

               { L_,               -110,  INIT_BUFSIZE,                 -1  },
               { L_,  -INIT_BUFSIZE - 1,  INIT_BUFSIZE,                 -1  },
               { L_,      -INIT_BUFSIZE,  INIT_BUFSIZE,                  0  },
               { L_,  -INIT_BUFSIZE +1 ,  INIT_BUFSIZE,                  1  },
               { L_,                -10,  INIT_BUFSIZE,  INIT_BUFSIZE - 10  },
               { L_,                 -1,  INIT_BUFSIZE,   INIT_BUFSIZE - 1  },
               { L_,                  0,  INIT_BUFSIZE,       INIT_BUFSIZE  },
               { L_,                  1,  INIT_BUFSIZE,                 -1  },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal :
                                       DATA[i].d_initialPosition);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);
                Obj        mSB(buffer, INIT_BUFSIZE);
                const Obj& SB = mSB;

                Obj::pos_type ret;
                mSB.sputn(mFILL, DATA[i].d_initialPosition);
                ret = mSB.pubseekoff(DATA[i].d_offset, CUR, PUT);
                if (veryVerbose) P(ret);

                ASSERTV(LINE, DATA[i].d_retVal == ret);
                ASSERTV(LINE, FINAL_POS == SB.length());
                ASSERTV(LINE, 0 == bsl::memcmp(mFILL, buffer, SB.length()));
            }
        }
        if (verbose) cout << "\nTesting seekpos."
                          << endl;
        {
            static const struct {
                int         d_line;             // line number
                int         d_offset;           // seek offset
                int         d_initialPosition;  // where to set pptr() first
                int         d_retVal;           // expected return value
            } DATA[] = {
               //LINE          OFFSET         STARTPOS           RETVAL
               //----  --------------   --------------  ---------------
               { L_,   INIT_BUFSIZE/2,               0,              -1  },
               { L_,   INIT_BUFSIZE/2,  INIT_BUFSIZE/2,  INIT_BUFSIZE/2  },
               { L_,   INIT_BUFSIZE/2,    INIT_BUFSIZE,  INIT_BUFSIZE/2  },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;
                const int FINAL_POS = (0 <= DATA[i].d_retVal ?
                                       DATA[i].d_retVal :
                                       DATA[i].d_initialPosition);

                char buffer[INIT_BUFSIZE];
                memset(buffer, 'Z', INIT_BUFSIZE);
                Obj        mSB(buffer, INIT_BUFSIZE);
                const Obj& SB = mSB;

                Obj::pos_type ret;
                mSB.sputn(mFILL, DATA[i].d_initialPosition);
                ret = mSB.pubseekpos(DATA[i].d_offset, PUT);
                if (veryVerbose) P(ret);

                ASSERTV(LINE, DATA[i].d_retVal == ret);
                ASSERTV(LINE, FINAL_POS        == SB.length());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'sputn' METHOD
        //
        // Concerns:
        //: 1 Strings of varying length are written correctly.
        //:
        //: 2 Writing strings does not overwrite existing buffer content.
        //:
        //: 3 No more than the specified number of characters are written.
        //:
        //: 4 No writing happens beyond existing capacity.
        //
        // Plan:
        //: 1 Write out representative strings from the categories 0
        //:   characters, 1 character, and > 1 character, into streambufs with
        //:   representative contents "empty", substantially less than
        //:   capacity, and almost-full-so-that-next-write-exceeds-capacity.
        //:   (C-1..4)
        //
        // Testing:
        //   streamsize sputn(const char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'sputn' METHOD" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting sputn with no buffer." << endl;
        {
            Obj              mSB(0, 0);
            const Obj&       SB = mSB;
            const char      *source = "a";
            bsl::streamsize  retResult;

            ASSERT(0 == SB.capacity());

            retResult = mSB.sputn(source, strlen(source));
            ASSERT(0 == retResult);
            ASSERT(0 == SB.length());
            ASSERT(0 == SB.capacity());
        }

        if (verbose) cout << "\nTesting sputn." << endl;
        {
            const struct TestData {
                int         d_line;          // line number
                const char *d_outStr;        // string to output
                int         d_strCap;        // stream capacity
                const char *d_initCont;      // initial contents of stream
                const char *d_result;        // expected contents after 'sputc'
                int         d_length;        // number of put chars
                int         d_returnVal;     // 'sputn' return val
            } DATA[] = {
               //LINE OUT    STREAM  INIT   RESULT  STREAM  RET
               //     STRNG  CPCITY CONTNT  CONTNT  LENGTH  VAL
               //---- -----  ------ ------  ------  ------  ---
              { L_,     "",      0,    "",     "",      0,    0  }, // N = 0
              { L_,    "s",      0,    "",     "",      0,    0  },
              { L_,  "abc",      0,    "",     "",      0,    0  },

              { L_,     "",      1,    "",     "",      0,    0  }, // N = 1
              { L_,    "s",      1,   "a",    "a",      1,    0  },
              { L_,  "abc",      1,   "a",    "a",      1,    0  },

              { L_,     "",      2,    "",     "",      0,    0  }, // N = 2
              { L_,  "abc",      2,    "",   "ab",      2,    2  },
              { L_,    "s",      2,   "a",   "as",      2,    1  },
              { L_,  "abc",      2,   "a",   "aa",      2,    1  }
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // buffer states (buffer length x buffer contents.)
            for(size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE      = DATA[i].d_line;

                char       *bytes = new char[DATA[i].d_strCap];
                Obj         mSB(bytes, DATA[i].d_strCap);
                const Obj&  SB = mSB;

                for(unsigned j = 0; j < strlen(DATA[i].d_initCont); ++j ) {
                    mSB.sputc(DATA[i].d_initCont[j]);
                }
                if (veryVerbose) { cout << "\tInitial contents: "; P(SB)};
                bsl::streamsize retResult;
                retResult = mSB.sputn(DATA[i].d_outStr,
                                      strlen(DATA[i].d_outStr));
                ASSERTV(LINE, 0 == strncmp(bytes,
                                           DATA[i].d_result,
                                           strlen(DATA[i].d_result )) );
                ASSERTV(LINE, DATA[i].d_returnVal == retResult );
                ASSERTV(LINE, DATA[i].d_length    == SB.length());
                delete [] bytes;
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors of the 'bdlsb::FixedMemOutput'
        //   object.  Note that none of the accessors are strictly needed for
        //   followup tests, but simplify the overall test logic.
        //
        // Concerns:
        //: 1 Accessors work off of a references to 'const' objects.
        //:
        //: 2 The address of the user provided buffer is correctly reported.
        //:
        //: 3 The capacity of the stream buffer is correctly reported.
        //:
        //: 4 The number of characters written to the stream buffer is
        //:   correctly reported.
        //
        // Plan:
        //: 1 Verify accessors for corner cases. (C-1..4)
        //:
        //: 2 Fill the stream buffer with data and verify that all accessors
        //:   report expected values. (C-1..4)
        //
        // Testing:
        //   const char_type *data() const;
        //   streamsize length() const;
        //   streamsize capacity() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting streambuf with no buffer." << endl;
        {
            Obj mSB(0, 0);
            ASSERT(0 == mSB.data());
            ASSERT(0 == mSB.length());
            ASSERT(0 == mSB.capacity());

            const Obj& SB = mSB;
            ASSERT(0 == SB.data());
            ASSERT(0 == SB.length());
            ASSERT(0 == SB.capacity());

            if(veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting empty streambuf." << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE - 1);
            const Obj& SB = mSB;

            ASSERT(buffer           == SB.data());
            ASSERT(0                == SB.length());
            ASSERT(INIT_BUFSIZE - 1 == SB.capacity());

            if(veryVerbose) P(SB);
        }

        if (verbose) { cout << "\nTesting different initial buffer states."
                            << endl; }
        {
            const struct TestData {
                int         d_line;          // line number
                char        d_outChar;       // character to output
                int         d_strCap;        // stream capacity
                const char *d_initCont;      // initial contents of stream
                const char *d_result;        // expected contents after 'sputc'
                int         d_length;        // number of chars in stream
                                             // buffer after 'sputc'

                int         d_returnVal;     // 'sputc' return val
            } DATA[] = {
               //LINE   OUT   STREAM  INIT    RESULT  STREAM  RET
               //       CHAR  CPCITY  CONTNT  CONTNT  LENGTH  VAL
               //----   ----  ------  ------  ------  ------  ---
               { L_,    's',      0,     "",     "",      0,  -1  }, // N = 0

               { L_,    's',      1,     "",    "s",      1,  's' }, // N = 1
               { L_,    's',      1,    "a",    "a",      1,  -1  },

               { L_,    's',      2,     "",    "s",      1,  's' }, // N = 2
               { L_,    's',      2,    "a",   "as",      2,  's' },
               { L_,    's',      2,   "ab",   "ab",      2,  -1  },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // buffer states (buffer length x buffer contents.)
            for(size_t i = 0; i < DATA_LEN; ++i ) {
                const int  LINE = DATA[i].d_line;
                char      *bytes = new char[DATA[i].d_strCap];

                Obj        mSB(bytes, DATA[i].d_strCap);
                const Obj& SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initCont); ++j) {
                    mSB.sputc(DATA[i].d_initCont[j]);
                }
                if (veryVerbose) { cout << "\tInitial contents: "; P(SB)};
                int retResult = mSB.sputc(DATA[i].d_outChar);
                ASSERTV(LINE, 0 == strncmp(bytes,
                                           DATA[i].d_result,
                                           strlen(DATA[i].d_result)));
                ASSERTV(LINE, DATA[i].d_returnVal == retResult);
                ASSERTV(LINE, DATA[i].d_length    == SB.length());
                ASSERTV(LINE, DATA[i].d_strCap    == SB.capacity());
                delete [] bytes;
            }
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
        //
        // Plan:
        //:  1 For each of a small representative set of object values use
        //:    'stringstream' to write that object's value to two separate
        //:    strings.  Compare the contents of these strings with the literal
        //:    expected output format and verify that they are equal. (C-1..3)
        //
        // Testing:
        //   ostream& operator<<(ostream&, const FixedMemOutput&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nCreate stream buffers to be printed." << endl;
        {
            if (verbose) cout << "\tChecking operator<< return value."
                              << endl;

            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";       // stream is returned.
            ASSERT(true == out1.good());
            ASSERT(true == out2.good());
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output."
                              << endl;

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

            string str1 = out1.str();
            string str2 = out2.str();
            ASSERT(str1 == str2);

            const string EXPECTED =
                        "\n0000\t01101000 01100101 01101100 01101100 01101111";

            if (verbose) {
                T_ P(EXPECTED)
                T_ P(str1)
            }

            ASSERT(EXPECTED == str1);
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output again."
                              << endl;

            // One more test, just to see something different
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            mSB.sputc('0');  mSB.sputc('1');  mSB.sputc('2');  mSB.sputc('3');
            mSB.sputc('4');  mSB.sputc('5');  mSB.sputc('6');  mSB.sputc('7');
            mSB.sputc('8');  mSB.sputc('9');  mSB.sputc('A');  mSB.sputc('B');

            stringstream out1;
            stringstream out2;
            out1 << mSB;
            out2 << SB;

            string str1 = out1.str();
            string str2 = out2.str();
            ASSERT(str1 == str2);

            const string EXPECTED =
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010";

            if (verbose) {
                T_ P(EXPECTED)
                T_ P(str1)
            }

            ASSERT(EXPECTED == str1);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The value constructor can create an object to have any value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 An object can be safely destroyed.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //:
        //: 4 'sputc' method writes printing and non-printing characters
        //:   correctly.
        //:
        //: 5 'sputc' method writes bytes with leading bit set correctly.
        //:
        //: 6 'sputc' method writes only one character at once.
        //:
        //: 7 Data is landed only into client-provided buffer.
        //:
        //: 8 'sputc' method returns given parameter value in case of success.
        //:
        //: 9 Streambuf capacity exceeding does not corrupt the streambuf.
        //
        // Plan:
        //: 1 Create and object with the user provided buffer of the specified
        //:   length.  Verify, using the (as yet unproven) 'length', 'capacity'
        //:   and 'data' accessors, that all streambuf machinery has been set
        //:   up properly. (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope. (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros). (C-3)
        //:
        //: 4 Using the table-driven technique, specify a set of characters for
        //:   writing and their expected buffer presentations.
        //:
        //: 5 For each row 'R' in the table of P-4:
        //:
        //:   1 Create FixedMemOutput object.
        //:
        //:   2 Use 'sputc' to write character from the table to the object.
        //:
        //:   3 Verify that character was written to the buffer correctly.
        //:     (C-3..5)
        //:
        //:   4 Verify that next character in the buffer hasn't been changed.
        //:     (C-6)
        //:
        //: 6 Using the table-driven technique, specify a set of initial object
        //:   states, symbols for adding, expected resulting streambuf contents
        //:   and expected 'sputc' return values.
        //:
        //: 7 For each row 'R' in the table of P-6:
        //:
        //:   1 Create FixedMemOutput object.
        //:
        //:   2 Use 'sputc' to bring an object to required state.
        //:
        //:   3 Use 'sputc' to write character from the table to the object.
        //:
        //:   4 Verify obtained character set, comparing it with table model.
        //:     (C-3..5)
        //:
        //:   5 Verify obtained 'sputc' result, comparing it with table model.
        //:     (C-8)
        //:
        //:   6 Verify that no data has been changed outside of client-provided
        //:     buffer.  (C-7)
        //:
        //: 8 Create an object, write enough characters to fulfil available
        //:   memory, and then write one more.  Verify that written data hasn't
        //:   been corrupted.  (C-9)
        //
        // Testing:
        //   FixedMemOutput(char *buffer, bsl::streamsize length);
        //   ~FixedMemOutput();
        //   int_type sputc(char_type);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting value constructor" << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            if (verbose) cout <<
                    "\tEnsure that the stream buffer pointer is set correctly."
                              << endl;

            ASSERT(buffer == SB.data());

            if (verbose) cout <<
                      "\tEnsure that the stream buffer has specified capacity."
                              << endl;

            ASSERT(INIT_BUFSIZE == SB.capacity());

            if (verbose) cout <<
            "\tEnsure that the stream buffer contains no data on construction."
                              << endl;

            ASSERT(0 == SB.length());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            char buffer[INIT_BUFSIZE];

            bsls::AssertFailureHandlerGuard hG(
                                         bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj(     0,  INIT_BUFSIZE));
            ASSERT_SAFE_FAIL(Obj(     0,            -1));
            ASSERT_SAFE_FAIL(Obj(buffer,            -1));
            ASSERT_SAFE_PASS(Obj(     0,             0));
            ASSERT_SAFE_PASS(Obj(buffer,             0));
            ASSERT_SAFE_PASS(Obj(buffer,  INIT_BUFSIZE));
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            if (verbose) { cout << "\tBasic sputc test." << endl; }
            {
                typedef Obj::char_type T;

                static const struct {
                    int            d_line;       // line number
                    T              d_outChar;    // character to output
                    const char    *d_result;     // expected content
                } DATA[] = {
                    //LINE  OUTPUT  RESULT
                    //----  ------  ------
                    { L_,   's',    "s"    },  // Add ordinary ASCII character
                    { L_,   10,     "\xA"  },  // Add non-printing character
                    { L_,   127,    "\x7F" },  // Add <= 127
                    { L_,   (T)128, "\x80" }   // Add >= 128
                };   // end table DATA

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                // This loop verifies that 'sputc' both:
                //    1. Adds the character, and
                //    2. Does not overwrite beyond the character.

                if (verbose) { T_ cout << "Testing different character types."
                                       << endl; }

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int LINE = DATA[i].d_line;
                    char      buffer[INIT_BUFSIZE];
                    memset(buffer, 'Z', INIT_BUFSIZE);
                    Obj mSB(buffer, INIT_BUFSIZE - 1);

                    mSB.sputc(DATA[i].d_outChar);
                    ASSERTV(LINE, 0   == strncmp(buffer, DATA[i].d_result, 1));
                    ASSERTV(LINE, 'Z' == buffer[1]);
                }
            }

            if (verbose) { cout << "\tTesting different initial buffer states."
                                << endl; }
            {
                const struct TestData {
                    int         d_line;          // line number
                    char        d_outChar;       // character to output
                    int         d_strCap;        // stream capacity
                    const char *d_initCont;      // initial contents of stream
                    const char *d_result;        // expected content
                    int         d_returnVal;     // 'sputc' return val
                } DATA[] = {
                   //LINE   OUT  STREAM   INIT   RESULT   RET
                   //      CHAR  CPCITY  CONTNT  CONTNT   VAL
                   //----  ----  ------  ------  ------   ----
                   { L_,   's',  0,      "",     "",      -1  }, // N = 0

                   { L_,   's',  1,      "",     "s",     's' }, // N = 1
                   { L_,   's',  1,      "a",    "a",     -1  },

                   { L_,   's',  2,      "",     "s",     's' }, // N = 2
                   { L_,   's',  2,      "a",    "as",    's' },
                   { L_,   's',  2,      "ab",   "ab",    -1  },

                   { L_,   's',  3,      "",     "s",     's' }, // N = 3
                   { L_,   's',  3,      "ab",   "abs",   's' },
                   { L_,   's',  3,      "abc",  "abc",   -1  },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                // This segment verifies correct behavior across different
                // initial buffer states (buffer length x buffer contents.)
                for(size_t i = 0; i < DATA_LEN; ++i ) {
                    const int  LINE  = DATA[i].d_line;
                    char      *bytes = new char[DATA[i].d_strCap + 1];
                    memset(bytes, 'Z', DATA[i].d_strCap + 1);

                    Obj mSB(bytes, DATA[i].d_strCap);

                    for (size_t j = 0; j < strlen(DATA[i].d_initCont); ++j) {
                        mSB.sputc(DATA[i].d_initCont[j]);
                    }

                    int retResult = mSB.sputc(DATA[i].d_outChar);
                    ASSERTV(LINE, 0 == strncmp(bytes,
                                               DATA[i].d_result,
                                               strlen(DATA[i].d_result)));
                    ASSERTV(LINE, DATA[i].d_returnVal == retResult);
                    ASSERTV(LINE, 'Z' == bytes[DATA[i].d_strCap]);
                    delete [] bytes;
                }
            }

            if (verbose) { cout << "\tOverflow test." << endl; }
            {
                // Do an extra test to ensure that overflow does not corrupt
                // the stream
                const int  BUF_SIZE = 5;
                char       buffer[BUF_SIZE];
                Obj        mSB(buffer, BUF_SIZE);
                const Obj& SB = mSB;

                mSB.sputc('a');
                mSB.sputc('b');
                mSB.sputc('c');
                mSB.sputc('d');
                mSB.sputc('e');
                ASSERT( SB.length() == SB.capacity());

                int retResult = mSB.sputc('f');
                ASSERT(-1        == retResult);
                ASSERT( 0        == strncmp(buffer, "abcde", BUF_SIZE));
                ASSERT( BUF_SIZE == SB.capacity());
                ASSERT( BUF_SIZE == SB.length());
                ASSERT( buffer   == SB.data());
            }
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

        if (verbose) cout <<
          "\nMake sure we can create and use a 'bdlsb::FixedMemOutput'."
                          << endl;
        {
            char       buffer[INIT_BUFSIZE];
            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj& SB = mSB;

            if (verbose) { cout << \
                              "\tCreate a fixed-length output stream buffer: ";
                              P(SB) }

            ASSERT(0 == SB.length());

            mSB.sputn("hello", 5);

            if (verbose) { cout <<
                      "\n\tWrite a string (five chars) to the stream buffer: ";
                      P(SB) }

            ASSERT(5 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hello", 5));

            mSB.sputc('s');

            if (verbose) { cout <<
                              "\n\tWrite a single char to the stream buffer: ";
                              P(SB) }
            ASSERT(6 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hellos", 6));
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
