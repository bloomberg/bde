// bdlsb_fixedmemoutstreambuf.t.cpp                                   -*-C++-*-
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete class
// 'bdlsb::FixedMemOutStreamBuf', as well as each new (non-protocol) public
// method added in the 'bdlsb::FixedMemOutStreamBuf' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
// The protected methods (whose implementation or re-implementation define the
// behavior that differentiates one kind of stream buffer from another) are
// "driven", or invoked, by various (base-class) public methods provided for
// stream-buffer client use.  One recurrent concern in this test driver is to
// ensure correct interplay between these protected methods and the base-
// class-provided implementations that use them.
//
// Primary Manipulators:
//: o Value constructor 'FixedMemOutStreamBuf(char *, bsl::streamsize)'
//: o 'sputc'
//
// Basic Accessors:
//: o 'data'
//: o 'length'
//: o 'capacity'
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
// [ 2] ~FixedMemOutStreamBuf();
//
// MANIPULATORS
// [ 2] char* data();
// [ 2] int_type sputc(char_type);
// [ 7] pos_type seekpos(pos_type, openmode);
// [ 7] pos_type seekoff(off_type, seekdir, openmode);
// [ 8] FixedMemOutStreamBuf *setbuf(char_type *buf, streamsize length);
// [ 6] streamsize sputn(const char_type, streamsize);
//
// ACCESSORS
// [ 4] const char* data() const;
// [ 4] streamsize capacity() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] ostream& operator<<(ostream&, const FixedMemOutStreamBuf&);
// [ 9] USAGE EXAMPLE

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

typedef bdlsb::FixedMemOutStreamBuf Obj;
const int INIT_BUFSIZE = 20;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

               // ==========================================
               // operator<< for bdlsb::FixedMemOutStreamBuf
               // ==========================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream,
                         const Obj&    streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream& stream,
                         const Obj&    streamBuffer)
{
    const bsl::streamsize  len  = streamBuffer.length();
    const char            *data = streamBuffer.data();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    for (bsl::streamsize i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }
    stream.flags(flags); // reset stream format flags
    return stream;
}


typedef bsl::ios_base::openmode io_openmode;
typedef bsl::ios_base::seekdir  io_seekdir;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
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
      case 9: {
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
///Example 1: Directly Observing Stream Buffer Contents
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Unlike most implementations of the 'bsl::basic_streambuf' concept,
// 'bdlsb::FixedMemOutStreamBuf' gives the user direct access to the stream's
// storage, both through the 'data' accessor and through the buffer originally
// supplied to the constructor.  Note that this can be useful in many contexts,
// such as when we need to perform extra security validation on buffer during
// the streaming process.
//
// First, we create an array to provide storage for the stream buffer, and
// construct a 'bdlsb::FixedMemOutStreamBuf' on that array:
//..
    const unsigned int          STORAGE_SIZE = 64;
    char                        storage[STORAGE_SIZE];
    bdlsb::FixedMemOutStreamBuf buffer(storage, STORAGE_SIZE);
//..
// Notice that 'storage' is on the stack.  'bdlsb::FixedMemOutStreamBuf' can be
// easily used without resorting to dynamic memory allocation.
//
// Then, we observe that 'buffer' already has a capacity of 64.  Note that this
// capacity is fixed at construction:
//..
    ASSERT(STORAGE_SIZE == buffer.capacity());
    ASSERT( 0 == buffer.length());
    ASSERT(buffer.data() == storage);
//..
// Next, we use 'buffer' to construct a 'bsl::ostream':
//..
    bsl::ostream stream(&buffer);
//..
// Now, we output some data to the 'stream':
//..
    stream << "The answer is " << 42 << ".";
//..
// Finally, we observe that the data is present in the storage array that we
// supplied to 'buffer':
//..
    ASSERT(17 == buffer.length());
    ASSERT(buffer.length() < STORAGE_SIZE);
    ASSERT(0 == strncmp("The answer is 42.", storage, 17));
//..
//
///Example 2: Fixed buffer's size illustration
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Unlike most implementations of the 'bsl::basic_streambuf' concept,
// 'bdlsb::FixedMemOutStreamBuf' uses a buffer of limited size, provided to the
// constructor together with the address of the storage buffer.  That limit
// will not be exceeded even in case of superfluous data.  Symbols beyond this
// limit will be ignored.  Note that this can be useful if memory allocation
// should be strictly controlled.
//
// First, we create an array to provide storage for the stream buffer, fill it
// with some data and construct a 'bdlsb::FixedMemOutStreamBuf' on the part of
// that array:
//..
    const unsigned int SMALL_STORAGE_SIZE = 16;
    const unsigned int SMALL_BUFFER_CAPACITY = SMALL_STORAGE_SIZE/2;
    char               smallStorage[SMALL_STORAGE_SIZE];
    memset(smallStorage, 'Z', SMALL_STORAGE_SIZE);

    bdlsb::FixedMemOutStreamBuf smallBuffer(smallStorage,
                                            SMALL_BUFFER_CAPACITY);
//..
// Next, we write some characters to the buffer and check that it handles them
// correctly and superfluous data is ignored:
//..
    bsl::streamsize returnedSize = smallBuffer.sputn("The answer is 42.", 17);
    ASSERT(SMALL_BUFFER_CAPACITY == returnedSize);
    ASSERT(SMALL_BUFFER_CAPACITY == smallBuffer.length());
    ASSERT('Z' == smallStorage[smallBuffer.length()]);
//..
// Then, we reset position indicator to the beginning of storage:
//..
    smallBuffer.pubseekpos(0,bsl::ios_base::out);
    ASSERT(0 == smallBuffer.length());
//..
// Now, we write another string, containing fewer characters than the storage
// capacity:
//..
    returnedSize = smallBuffer.sputn("Truth.", 6);
//..
// Finally, we observe that given string has been successfully placed to
// buffer:
//..
    ASSERT(6 == returnedSize);
    ASSERT(6 == smallBuffer.length());
    ASSERT(0 == strncmp("Truth.", smallStorage, 6));
//..
      } break;

      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'setbuf' METHOD
        //
        // Ensure that we can reset put area to client-provided buffer for a
        // constructed stream buffer object
        //
        // Concerns:
        //: 1 The 'setbuf' method can reset internal buffer via base public
        //:   interface.
        //
        // Plan:
        //: 1 Manually call 'pubsetbuf' method and verify that the buffer has
        //:   been reset to the new address and length. (C-1)
        //
        // Testing:
        //   FixedMemOutStreamBuf *setbuf(char_type *buf, streamsize length);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'setbuf' METHOD" << endl
                          << "=======================" << endl;

        {
            char buffer1[INIT_BUFSIZE];
            memset(buffer1, 'Z', INIT_BUFSIZE);
            char buffer2[INIT_BUFSIZE/2];
            memset(buffer2, 'A', INIT_BUFSIZE/2);

            Obj        mSB(buffer1, INIT_BUFSIZE);
            const Obj&  SB = mSB;
            ASSERT(buffer1 == SB.data());
            ASSERT(INIT_BUFSIZE == SB.capacity());

            mSB.pubsetbuf(buffer2, INIT_BUFSIZE/2);
            ASSERT(buffer2 == SB.data());
            ASSERT(INIT_BUFSIZE/2 == SB.capacity());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //
        // As the only action performed in 'seekpos' is the call for 'seekoff'
        // with predetermined second parameter, then we can test 'seekpos'
        // superficially.
        //
        // Concerns:
        //: 1 Seeking is correct for:
        //:   - all relative positions.
        //:   - positive, 0, and negative values.
        //:   - out of buffer boundaries.
        //:
        //: 2 Seeking into the "get" area has no effect.
        //:
        //: 3 'seekpos' calls 'seekoff' with correctly predetermined second
        //:   parameter (bsl::ios_base::beg)
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   Note that seekoff method is called by base class method
        //:   'pubseekoff'. (C-1..2)
        //:
        //: 2 Perform several seeks with different initial states of the
        //:   tested object. (C-3)
        //
        // Testing:
        //   pos_type seekoff(off_type, seekdir, openmode);
        //   pos_type seekpos(pos_type, openmode);
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
                Obj mSB(buffer, INIT_BUFSIZE);

                Obj::pos_type ret;
                mSB.sputn(mFILL, INIT_BUFSIZE);

                ret = mSB.pubseekoff(DATA[i].d_amount,
                                     DATA[i].d_base,
                                     DATA[i].d_areaFlags);
                ASSERTV(LINE, DATA[i].d_retVal == ret);
                if (veryVerbose) P(ret);

                ASSERTV(LINE, DATA[i].d_retVal == ret);
                ASSERTV(LINE, FINAL_POS == mSB.length());
                ASSERTV(LINE, 0 == bsl::memcmp(mFILL, buffer, mSB.length()));
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
                const Obj&  SB = mSB;

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
                const Obj&  SB = mSB;

                Obj::pos_type ret;
                mSB.sputn(mFILL, DATA[i].d_initialPosition);
                ret = mSB.pubseekpos(DATA[i].d_offset, PUT);
                if (veryVerbose) P(ret);

                ASSERTV(LINE, DATA[i].d_retVal == ret);
                ASSERTV(LINE, FINAL_POS == SB.length());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'sputn' METHOD
        //
        // Concerns:
        //: 1 Ensure that strings of varying length are written correctly.
        //:
        //: 2 Ensure that writing strings does not overwrite existing buffer
        //:   content.
        //:
        //: 3 Ensure that no more than the specified number of characters are
        //:   written.
        //:
        //: 4 Ensure that no writing happens beyond existing capacity.
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
            const char      *source = "a";
            bsl::streamsize  retResult;

            retResult = mSB.sputn(source, strlen(source));
            ASSERT(0 == retResult);
        }

        if (verbose) cout << "\nTesting sputn." << endl;
        {
            const struct TestData {
                int         d_line;          // line number
                const char *d_outStr;        // string to output
                int         d_strCap;        // stream capacity
                const char *d_initialCont;   // initial contents of stream
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

                char *bytes = new char[DATA[i].d_strCap];

                Obj mSB(bytes, DATA[i].d_strCap);

                for(unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j ) {
                    mSB.sputc(DATA[i].d_initialCont[j]);
                }
                if (veryVerbose) { T_ cout << "Initial contents: "; P(mSB)};
                bsl::streamsize retResult;
                retResult = mSB.sputn(DATA[i].d_outStr,
                                      strlen(DATA[i].d_outStr));
                ASSERTV(LINE, 0 == strncmp(bytes,
                                           DATA[i].d_result,
                                           strlen(DATA[i].d_result )) );
                ASSERTV(LINE, DATA[i].d_returnVal == retResult );
                ASSERTV(LINE, DATA[i].d_length == mSB.length());
                delete [] bytes;
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // This component does not provide output operator.  Note that output
        // operator used for test tracing purposes is tested in test case 3.
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING OUTPUT (<<) OPERATOR" << endl
                          << "============================" << endl;

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Verify the basic accessors of the 'bdlsb::FixedMemOutStreamBuf'
        // object.  Note that none of the accessors are strictly needed for
        // followup tests, but simplify the overall test logic.
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
        //: 1 Verify accessors for corner cases. (C 1..4)
        //:
        //: 2 Fill the stream buffer with data and verify that all accessors
        //:   report expected values. (C 1..4)
        //
        // Testing:
        //   const char* data() const;
        //   streamsize length() const;
        //   streamsize capacity() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting streambuf with no buffer." << endl;
        {
            Obj        mSB(0, 0);
            const Obj&  SB = mSB;

            ASSERT(0 == SB.data());
            ASSERT(0 == SB.length());
            ASSERT(0 == SB.capacity());

            if(veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting empty streambuf." << endl;
        {
            char buffer[INIT_BUFSIZE];

            Obj        mSB(buffer, INIT_BUFSIZE - 1);
            const Obj&  SB = mSB;

            ASSERT(buffer == SB.data());
            ASSERT(0 == SB.length());
            ASSERT(INIT_BUFSIZE - 1 == SB.capacity());

            if(veryVerbose) P(SB);
        }

        {
            if (verbose) { T_ cout <<
                                   "Testing different initial buffer states."
                                   << endl; }

            const struct TestData {
                int         d_line;          // line number
                char        d_outChar;       // character to output
                int         d_strCap;        // stream capacity
                const char *d_initialCont;   // initial contents of stream
                const char *d_result;        // expected contents after 'sputc'
                int         d_length;        // number of put chars
                int         d_returnVal;     // 'sputc' return val
            } DATA[] = {
               //LINE   OUT   STREAM  INIT    RESULT  STREAM  RET
               //       CHAR  CPCITY  CONTNT  CONTNT  LENGTH  VAL
               //----   ----  ------  ------  ------  ------  ---
               { L_,    's',      0,     "",     "",      0,   -1  }, // N = 0

               { L_,    's',      1,     "",    "s",      1,  's'  }, // N = 1
               { L_,    's',      1,    "a",    "a",      1,   -1  },

               { L_,    's',      2,     "",    "s",      1,  's'  }, // N = 2
               { L_,    's',      2,    "a",   "as",      2,  's'  },
               { L_,    's',      2,   "ab",   "ab",      2,   -1  },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // buffer states (buffer length x buffer contents.)
            for(size_t i = 0; i < DATA_LEN; ++i ) {
                const int   LINE = DATA[i].d_line;
                char      *bytes = new char[DATA[i].d_strCap];

                Obj        mSB(bytes, DATA[i].d_strCap);
                const Obj&  SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j) {
                    mSB.sputc(DATA[i].d_initialCont[j]);
                }
                if (veryVerbose) { T_ cout << "Initial contents: "; P(SB)};
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
        //
        // Verify the auxiliary function used in the test driver. Note that
        // the tested function is not part of the component and use only to
        // provide human readable test traces.
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
        //   ostream& operator<<(ostream&, const FixedMemOutStreamBuf&);
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
            const Obj&  SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
            ASSERT(true == out1.good());
            ASSERT(true == out2.good());
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output."
                              << endl;

            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

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
            ASSERT(EXPECTED == str1);

            if (verbose) {
                T_ P(str1)
            }
        }

        {
            if (verbose) cout << "\n\tChecking operator<< output again."
                              << endl;

            // One more test, just to see something different
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

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

            ASSERT(EXPECTED == str1);
            if (verbose) cout << endl;
            if (verbose) {
                T_ P(EXPECTED)
                T_ P(str1)
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        //   Ensure that we can use the 2-argument value constructor to create
        //   an object having any state relevant for thorough testing, and use
        //   the destructor to destroy it safely.
        //   There are no public functions with which to cleanly observe the
        //   state of the object without manipulating it.  This test will rely
        //   on using the base-class-implemented 'pbase' and 'sputc' to check
        //   that writing into the stream buffer in fact writes into the
        //   client-provided buffer.
        //
        // Concerns:
        //: 1 The 2-argument value constructor can create an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 2 The constructor sets up all streambuf machinery properly
        //:   so that streambuf operations use the client-provided buffer.
        //:
        //: 3 The streambuf is capable of using the entirety of the available
        //:   storage, i.e., the full amount specified by the client.
        //:
        //: 4 An object can be safely destroyed.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using 2-argument constructure, create and object with the
        //:   specified buffer of the specified length. (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope. (C-4)
        //:
        //: 2 Use 'data' method (forwarded to the base class method 'pbase') to
        //:   ensure that the buffer is set correctly. (C-2)
        //:
        //: 3 Modify the stream buffer content via base class method 'sputc'
        //:   and verify that the data is landed into client-provided buffer.
        //:   (C-2..3)
        //:
        //: 4 Write enough characters to verify that the length as specified is
        //:   completely usable, and then write one more to ensure that the
        //:   stream buffer is using the client-supplied length as an upper
        //:   bound as well as a lower bound. (C-3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros). (C-5)
        //
        // Testing:
        //   FixedMemOutStreamBuf(char *buffer, bsl::streamsize length);
        //   ~FixedMemOutStreamBuf();
        //   char* data();
        //   int_type sputc(char_type);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        {
            char buffer[INIT_BUFSIZE];
            memset(buffer, 'Z', INIT_BUFSIZE);
            Obj        mSB(buffer, INIT_BUFSIZE - 1);

            if (verbose) cout <<
                    "\nEnsure that the stream buffer pointer is set correctly."
                              << endl;

            ASSERT(buffer == mSB.data());

            if (verbose) cout <<
             "\nEnsure that the stream buffer has at least specified capacity."
                              << endl;

            for (int i = 0; i < INIT_BUFSIZE - 1; ++i) {
                mSB.sputc('a');
                ASSERTV(i, 'a' == buffer[i]);
                ASSERTV(i, 'Z' == buffer[i + 1]);
            }

            if (verbose) cout <<
         "\nEnsure that the stream buffer has no more than specified capacity."
                             << endl;

            mSB.sputc('a');
            ASSERT('Z' == buffer[INIT_BUFSIZE - 1]);
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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Note: This test merely exercises basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Manually test various methods
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
          "\nMake sure we can create and use a 'bdlsb::FixedMemOutStreamBuf'."
                          << endl;
        {
            char buffer[INIT_BUFSIZE];

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            if (verbose) { cout <<
                      "\tCreate a fixed-length output stream buffer: "; P(SB) }
            ASSERT(0 == SB.length());

            mSB.sputn("hello", 5);
            if (verbose) { cout <<
                      "\n\tWrite a string (five chars) to the stream buffer: ";
                                                                        P(SB) }
            ASSERT(5 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hello", 5));

            mSB.sputc('s');
            if (verbose) { cout <<
                     "\n\tWrite a single char to the stream buffer: "; P(SB) }
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
