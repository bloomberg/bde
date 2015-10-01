// bdlsb_fixedmeminstreambuf.t.cpp                                    -*-C++-*-
#include <bdlsb_fixedmeminstreambuf.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>            // for testing only
#include <bsl_sstream.h>            // for testing only
#include <bsl_string.h>             // for testing only

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete class
// 'FixedMemInStreamBuf', as well as each new (non-protocol) public method
// added in the 'FixedMemInStreamBuf' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defines
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
//: o Value constructor 'FixedMemInStreamBuf(const char *, streamsize)'
//: o 'sbumpc'
//
/// Basic Accessors:
//: o 'data'
//: o 'length'
//
// Global Concerns:
//: o Ensure correct interaction between reloaded methods of the
//:   'FixedMemInStreamBuf' class  and the base class methods that use them.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FixedMemInStreamBuf(const char *, streamsize);
// [ 2] ~FixedMemInStreamBuf();
//
// MANIPULATORS
// [ 2] int_type sbumpc();
// [ 6] streamsize xsgetn(char_type, streamsize);
// [ 7] pos_type seekoff(off_type, seekdir, openmode);
// [ 7] pos_type seekpos(pos_type, openmode);
// [ 8] FixedMemInStreamBuf *setbuf(char *, streamsize);
// [ 8] FixedMemInStreamBuf *setbuf(const char *, streamsize);
// [ 8] FixedMemInStreamBuf *pubsetbuf(char *, streamsize);
// [ 8] FixedMemInStreamBuf *pubsetbuf(const char *, streamsize);
// [ 9] int_type pbackfail(int_type);
//
//
// ACCESSORS
// [ 4] const char *data() const;
// [ 4] streamsize length() const;
// [10] streamsize showmanyc();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] ostream& operator<<(ostream&, const FixedMemInStreamBuf&);
// [ 9] CONCERN: 'pbackfail' base class implementation works correctly.
// [11] CONCERN: 'setbuf' overrides base class method.
// [12] USAGE EXAMPLE

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
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsb::FixedMemInStreamBuf Obj;

const bsl::size_t   INIT_BUFSIZE = 20;
const Obj::off_type INIT_BUFSIZE_OFF = 20;
const bsl::size_t   MIDPOINT     = INIT_BUFSIZE / 2;
const Obj::off_type MIDPOINT_OFF = INIT_BUFSIZE_OFF / 2;

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

namespace {

// This function is used to fill the provided buffer with pattern used across
// multiple tests.
void fillBuffer(char* buffer, bsl::size_t length)
{
    char        ch = 'a';
    bsl::size_t i  = 0;

    while (i < length) {
        buffer[i++] = ch++;
    }
}

}  // close unnamed namespace

               // ==================================
               // operator<< for FixedMemInStreamBuf
               // ==================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const Obj& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream& stream, const Obj& streamBuffer)
{
    const bsl::streamsize  len   = streamBuffer.length();
    const char             *data = streamBuffer.data();

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

    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
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
///Example 1: Basic usage of the 'bdlsb::FixedMemInStreamBuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdlsb::FixedMemInStreamBuf' can be used in situations when you already
// have an array of bytes in memory and you'd like to wrap it in an input
// stream to extract data in a formatted manner.  A
// 'bdlsb::FixedMemInStreamBuf' object refers to an externally managed buffer
// that is supplied either at construction, or using the 'pubsetbuf' method of
// the 'bsl::streambuf' base-class.
//
// First, we create an array of characters to provide data that needs to be
// parsed, and construct 'bdlsb::FixedMemInStreamBuf' on that array:
//..
    {
        const char *inputText = "1 1 2 3 5 8 13 21";
        bdlsb::FixedMemInStreamBuf buffer(inputText, strlen(inputText));
//..
// Notice that 'bdlsb::FixedMemInStreamBuf' can be used with buffers referring
// to stack memory or to heap memory.
//
// Then, we use 'buffer' to construct a 'bsl::istream':
//..
        bsl::istream stream(&buffer);
//..
// Finally, we can input the data from the stream in a formatted manner:
//..
        int value;
        while (stream >> value) {
            cout << "Value is: " << value << endl;
        }
    }
//..
//
///Example 2: Scanning input data
/// - - - - - - - - - - - - - - -
// This example illustrates scanning of the input stream buffer for particular
// pattern ( digits, in our case ) and then using stream to read out found
// number.
//
// First, we create an array of characters to provide data that needs to be
// parsed, and construct 'bdlsb::FixedMemInStreamBuf' on that array:
//..
    {
        const char *inputText = "The answer is: 42.";
        bdlsb::FixedMemInStreamBuf buffer(inputText, strlen(inputText));
//..
// Then, we use 'buffer' to construct a 'bsl::istream' that will be used later
// to read found number:
//..
        bsl::istream stream(&buffer);
//..
// Next, we scan input buffer one character at a time searching for the first
// digit:
//..
        char ch;
        do {
            ch = buffer.sbumpc();

            if ( (ch >= '0') && (ch <= '9') ) {
//..
// Now, when the digit character is found, we return the first digit into the
// input stream buffer for subsequent read:
//..
                buffer.sputbackc(ch);
                int n;
//..
// Finally, we read out the whole number:
//..
                stream >> n;
                ASSERT( 42 == n );
                cout << "The answer is " << n << " indeed..." << endl;
                break;
            }
        } while ( ch != EOF );
    }
//..

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'setbuf' OVERRIDE
        //   Due to the presence of second 'setbuf' protected method with a
        //   const qualifier for the client-provided buffer, Coverity gives a
        //   "bad override" error on 'setbuf', claiming that:
        //
        //   bad_override: Method
        //   BloombergLP::bdlsb::FixedMemInStreamBuf::setbuf hides but does not
        //   override std::basic_streambuf<char, std::char_traits<char>
        //   >::setbuf because some type qualifiers do not match.
        //
        //   This test case confirms that the override actually does work.
        //
        // Concerns:
        //: 1 'setbuf' can be called from a base class pointer.
        //:
        //: 2 Calling 'setbuf' through a base class pointer replaces the
        //:   internal buffer.
        //
        // Plan:
        //: 1 Create a 'bdlsb::FixedMemInStreamBuf' with a known source buffer.
        //:   Replace the buffer with a second known source by calling
        //:   'pubsetbuf' through a base class pointer.  Observe that the
        //:   buffer has been replaced.
        //
        // Testing:
        //   CONCERN: 'setbuf' overrides base class method.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
               << "TESTING 'setbuf' OVERRIDE" << endl
               << "=========================" << endl;

        const char* text = "Lorem ipsum dolor sit amet, consectetur";
        bsl::string newText = "Some meaningfull text here";

        Obj mSB(text, strlen(text));

        bsl::streambuf* mBSB = &mSB;

        mBSB->pubsetbuf(&newText[0], newText.size());

        bsl::string result(mSB.data());

        ASSERTV(newText, result, newText == result);
      } break;

      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'showmanyc' METHOD
        //
        // Concerns:
        //: 1 That 'showmanyc' return the correct number of items left
        //    to consume in the streambuf.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'showmanyc' method for remaining-characters
        //:   categories 0, [1..stream buffer capacity-1] and [entire capacity]
        //:   (C-1)
        //
        // Testing:
        //   streamsize showmanyc();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'showmanyc' METHOD" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nTesting 'showmanyc' with no buffer." << endl;
        {
            Obj mSB(0, 0);

            ASSERT(-1 == mSB.in_avail());
        }

        if (verbose) cout <<
            "\nTesting 'showmanyc' in various positions." << endl;
        {
            static const struct {
                int           d_line;          // line number
                int           d_bufferSize;    // buffer size
                streamsize    d_charsToRead;   // number of characters to read
                streamsize    d_expectedAvail; // expected number of available
                                               // characters in the streambuf
            } DATA[] = {
               //LINE   BUFFER_SIZE   CHARS_TO_READ EXPECTED_AVAIL
               //----  ------------  -------------- --------------
               { L_,             0,             0,             -1 },
               { L_,             0,             1,             -1 },
               { L_,             1,             0,              1 },
               { L_,             1,             1,             -1 },
               { L_,             1,             2,             -1 },
               { L_,             2,             0,              2 },
               { L_,             2,             1,              1 },
               { L_,             2,             2,             -1 },
               { L_,             2,             3,             -1 },
               { L_,             3,             0,              3 },
               { L_,             3,             1,              2 },
               { L_,             3,             2,              1 },
               { L_,             3,             3,             -1 },
               { L_,             3,             4,             -1 },
               { L_,             4,             0,              4 },
               { L_,             4,             1,              3 },
               { L_,             4,             2,              2 },
               { L_,             4,             3,              1 },
               { L_,             4,             4,             -1 },
               { L_,             4,             5,             -1 },
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int                   LINE = DATA[i].d_line;
                const int            BUFFER_SIZE = DATA[i].d_bufferSize;
                const streamsize   CHARS_TO_READ = DATA[i].d_charsToRead;
                const streamsize  EXPECTED_AVAIL = DATA[i].d_expectedAvail;

                if (veryVerbose) {
                    cout << "\tTesting 'showmanyc'  for streambuf of size "
                         << BUFFER_SIZE << "." << endl ;
                }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj mSB(buffer, BUFFER_SIZE);

                char temp[16];
                mSB.sgetn(temp, CHARS_TO_READ);
                streamsize retval = mSB.in_avail();
                ASSERTV(LINE, EXPECTED_AVAIL == retval);

                if (veryVeryVerbose) {
                    cout << "\t Buffer size: "  << BUFFER_SIZE
                         << " Chars to Read: "  << CHARS_TO_READ
                         << " Expected Avail: " << EXPECTED_AVAIL << endl;
                }
                delete[] buffer;
            }
        }

      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'pbackfail' METHOD
        //
        // 'pfailback' is protected method that can be called by 2 public
        // methods 'sungetc' and 'sputbackc' under certain corner conditions.
        //
        // Concerns:
        //: 1 Ensure that base class implementation of 'pfailback' works
        //:   for this class.
        //:
        //: 2 Ensure that characters can be put back into the buffer.
        //:
        //: 3 Ensure that when 'pbackfail' is called, it correctly handles
        //:   corner cases.
        //
        // Plan:
        //: 1 Invoke the 'sputbackc' and 'sungetc' public methods for the cases
        //:   when they do not call 'pbackfail'.  Ensure that the stream buffer
        //:   position indicator updated correctly. (C-2)
        //:
        //: 2 Invoke the 'sputbackc' and 'sungetc' public methods for the cases
        //:   when they call 'pbackfail'.  Ensure that the content of the
        //:   streambuf is not modified, position indicator is not changed.
        //:   (C-1, 3)
        //
        // Testing:
        //   int_type pbackfail(int_type);
        //   CONCERN: 'pbackfail' base class implementation works correctly.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'pbackfail' METHOD" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting 'pbackfail' is called by public methods." << endl;
        {

            static const struct {
                int         d_line;          // line number
                bsl::size_t d_bufferSize;    // buffer length
            } DATA[] = {
               //LINE   BUFFER_SIZE
               //----  ------------
               { L_,              0  },
               { L_,              1  },
               { L_,   INIT_BUFSIZE  },
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int          LINE        = DATA[i].d_line;
                const bsl::size_t BUFFER_SIZE = DATA[i].d_bufferSize;

                if (veryVerbose) {
                    T_ P_(LINE) P(BUFFER_SIZE);
                }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);

                Obj::int_type retval;
                retval = mSB.sungetc();
                ASSERT(Obj::traits_type::eof() == retval);
                if (veryVeryVerbose) {
                    cout << "\t'sungetc' returned ";
                    if (Obj::traits_type::eof() == retval) {
                        cout << "EOF." << endl;
                    } else {
                        cout << Obj::traits_type::to_char_type(retval)
                            << "." << endl;
                    }
                }

                retval = mSB.sputbackc('Z');
                ASSERT(Obj::traits_type::eof() == retval);
                if (veryVeryVerbose) {
                    cout << "\t'sputbackc' returned ";
                    if (Obj::traits_type::eof() == retval) {
                        cout << "EOF." << endl;
                    } else {
                        cout << Obj::traits_type::to_char_type(retval)
                            << "." << endl;
                    }
                }

                delete[] buffer;
            }
        }

        if (verbose) cout <<
            "\nTesting 'pbackfail' is not called by public methods." << endl;
        {

            static const struct {
                int           d_line;          // line number
                Obj::off_type d_startPos;      // start position
                Obj::int_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE        START_POS                     RETVAL
               //----  ----------------  ------------------------
               // 'pbackfail' is not invoked for position > 0
               { L_,                 1,                       'a' },
               { L_,                 2,                       'b' },
               { L_,          MIDPOINT,      'a' + (MIDPOINT - 1) },
               { L_,      INIT_BUFSIZE,  'a' + (INIT_BUFSIZE - 1) },
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE      = DATA[i].d_line;
                const Obj::off_type START_POS = DATA[i].d_startPos;
                const Obj::int_type RETVAL    = DATA[i].d_retVal;

                if (veryVerbose) { T_ P_(LINE) P_(START_POS) P(RETVAL) }

                char buffer[INIT_BUFSIZE];
                fillBuffer(buffer, INIT_BUFSIZE);

                Obj mSB(buffer, INIT_BUFSIZE);

                mSB.pubseekpos(START_POS, GET);

                // Put one character back into the buffer
                Obj::int_type retval;
                retval = mSB.sungetc();

                ASSERTV(i, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sungetc' returned ";
                    if (Obj::traits_type::eof() == retval) {
                        cout << "EOF." << endl;
                    } else {
                        cout << Obj::traits_type::to_char_type(retval)
                            << "." << endl;
                    }
                }

                // Putting back valid character
                mSB.pubseekpos(START_POS, GET);

                retval = mSB.sputbackc(static_cast<char>(RETVAL));

                ASSERTV(i, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sputback' returned ";
                    if (Obj::traits_type::eof() == retval) {
                        cout << "EOF." << endl;
                    } else {
                        cout << Obj::traits_type::to_char_type(retval)
                            << "." << endl;
                    }
                }

                // Putting back invalid character
                mSB.pubseekpos(START_POS, GET);

                retval = mSB.sputbackc('Z');

                ASSERTV(i, Obj::traits_type::eof() == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sputback' (invalid) returned ";
                    if (Obj::traits_type::eof() == retval) {
                        cout << "EOF." << endl;
                    } else {
                        cout << Obj::traits_type::to_char_type(retval)
                            << "." << endl;
                    }
                }
            }
        }
      } break;

      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'setbuf' METHODS
        //
        // Protected 'setbuf' methods are called by the public 'pubsetbuf'
        // method.  Ensure that we can reset put area to client-provided buffer
        // for a constructed stream buffer object
        //
        // Concerns:
        //: 1 Ensure the 'setbuf' methods reset internal buffer via base public
        //:   interface.
        //:
        //: 2 Ensure the 'setbuf' methods reset position indicator.
        //:
        //: 3 Ensure 'const' and non-'const' buffers can be used as new
        //:   buffers.
        //
        // Plan:
        //: 1 Manually call 'pubsetbuf' methods and verify that the buffer has
        //:   been reset to the new address and length. (C-1, C-3)
        //:
        //: 2 Manually verify position indicator value after resetting the
        //:   stream buffer. (C-2)
        //
        // Testing:
        //   FixedMemInStreamBuf *setbuf(char *, streamsize);
        //   FixedMemInStreamBuf *setbuf(const char *, streamsize);
        //   FixedMemInStreamBuf *pubsetbuf(char *, streamsize);
        //   FixedMemInStreamBuf *pubsetbuf(const char *, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setbuf' METHODS" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nTesting 'setbuf' using non-const C arrays." << endl;

        {
            static const bsl::size_t sampleSizes[] = { 0, 1, INIT_BUFSIZE };

            for (int j = 0; j < 3; ++j) {
                const bsl::size_t BUFFER_SIZE = sampleSizes[j];

                if (veryVerbose) { T_ P(BUFFER_SIZE) }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                char fakeBuffer[1];

                Obj        mSB(fakeBuffer, 1);
                const Obj&  SB = mSB;

                ASSERT(1          == SB.length());
                ASSERT(fakeBuffer == SB.data());

                mSB.pubsetbuf(buffer, BUFFER_SIZE);
                ASSERT(BUFFER_SIZE == SB.length());
                ASSERT(buffer == SB.data());

                Obj::int_type retval;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    retval = mSB.sbumpc();
                    ASSERTV(i, buffer[i] ==
                                       Obj::traits_type::to_char_type(retval));
                    if (veryVeryVerbose) {
                        cout << "\t'sbumpc' returned ";
                        if (Obj::traits_type::eof() == retval) {
                            cout << "EOF." << endl;
                        } else {
                            cout << Obj::traits_type::to_char_type(retval)
                                << "." << endl;
                        }
                    }
                }
                delete [] buffer;
            }
        }

        if (verbose) cout <<
            "\nTesting 'setbuf' using const C arrays." << endl;
        {
            static const bsl::size_t sampleSizes[] = { 0, 1, INIT_BUFSIZE };

            for (int j = 0; j < 3; ++j) {
                const bsl::size_t BUFFER_SIZE = sampleSizes[j];

                if (veryVerbose) { T_ P(BUFFER_SIZE) }

                const char *buffer = new char[BUFFER_SIZE];
                char       *hack   = const_cast<char *>(buffer);
                fillBuffer(hack, BUFFER_SIZE);

                char fakeBuffer[1];

                Obj        mSB(fakeBuffer, 1);
                const Obj&  SB = mSB;

                ASSERT(1          == SB.length());
                ASSERT(fakeBuffer == SB.data());

                mSB.pubsetbuf(buffer, BUFFER_SIZE);
                ASSERT(BUFFER_SIZE == SB.length());
                ASSERT(buffer  == SB.data());

                Obj::int_type retval;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    retval = mSB.sbumpc();
                    ASSERTV(i, buffer[i] ==
                                       Obj::traits_type::to_char_type(retval));
                    if (veryVeryVerbose) {
                        cout << "\t'sbumpc' returned ";
                        if (Obj::traits_type::eof() == retval) {
                            cout << "EOF." << endl;
                        } else {
                            cout << Obj::traits_type::to_char_type(retval)
                                << "." << endl;
                        }
                    }
                }
                delete [] buffer;
            }
        }
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //
        // As the only action performed in 'seekpos' is the call for 'seekoff'
        // with predetermined second parameter, then we can test 'seekpos'
        // superficially.  Public 'pubseekoff' and 'pubseekpos' methods are
        // used to test protected 'seekoff' and 'seekpos' methods.
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
        //:   parameter (bsl::ios_base::beg).
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

        if (verbose) cout <<
                "\nTesting seekoff from beginning and end." << endl;

        {
            static const struct {
                int           d_line;          // line number
                io_openmode   d_areaFlags;     // "put"/"get" area
                Obj::off_type d_offset;        // seek offset
                io_seekdir    d_base;          // relative position
                Obj::pos_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE  AREA                OFFSET  BASE            RETVAL
               //----  ----   -------------------  ----   ---------------
               // seekoff from the start of the streambuf
               { L_,   GET,                    0,  BEG,                 0 },
               { L_,   GET,                    1,  BEG,                 1 },
               { L_,   GET,                   -1,  BEG,                -1 },
               { L_,   GET,   INIT_BUFSIZE_OFF-1,  BEG,  INIT_BUFSIZE - 1 },
               { L_,   GET,                  100,  BEG,                -1 },
               { L_,   GET,     INIT_BUFSIZE_OFF,  BEG,      INIT_BUFSIZE },

               // seekoff from the end of the streambuf.
               { L_,   GET,                    0,  END,      INIT_BUFSIZE },
               { L_,   GET,                    1,  END,                -1 },
               { L_,   GET,                   -1,  END,  INIT_BUFSIZE - 1 },
               { L_,   GET,                 -100,  END,                -1 },
               { L_,   GET,    -INIT_BUFSIZE_OFF,  END,                 0 },

               // seekoff from the current cursor, where cur == begin
               { L_,   GET,                    0,  CUR,                 0 },
               { L_,   GET,                    1,  CUR,                 1 },
               { L_,   GET,                   -1,  CUR,                -1 },
               { L_,   GET,     INIT_BUFSIZE_OFF,  CUR,      INIT_BUFSIZE },

               // seekoff in the "put" area
               { L_,   PUT,                    0,  BEG,                -1 },
               { L_,   PUT,                    1,  BEG,                -1 },
               { L_,   PUT,                   -1,  BEG,                -1 },
               { L_,   PUT,                    0,  CUR,                -1 },
               { L_,   PUT,                    1,  CUR,                -1 },
               { L_,   PUT,                   -1,  CUR,                -1 },
               { L_,   PUT,                    0,  END,                -1 },
               { L_,   PUT,                    1,  END,                -1 },
               { L_,   PUT,                   -1,  END,                -1 }
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE      = DATA[i].d_line;
                const io_openmode   AREA      = DATA[i].d_areaFlags;
                const Obj::off_type OFFSET    = DATA[i].d_offset;
                const io_seekdir    BASE      = DATA[i].d_base;
                const Obj::pos_type RETVAL    = DATA[i].d_retVal;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OFFSET) P(RETVAL)
                }
                char buffer[INIT_BUFSIZE];
                fillBuffer(buffer, INIT_BUFSIZE);

                Obj mSB(buffer, INIT_BUFSIZE);

                Obj::pos_type retval;
                retval = mSB.pubseekoff(OFFSET, BASE, AREA);

                ASSERTV(LINE, RETVAL == retval);
            }
        }

        if (verbose) cout << "\nTesting seekoff from a variety of "
                             "current-pointer positions." << endl;
        {
            static const struct {
                int           d_line;          // line number
                Obj::pos_type d_startPos;      // start seek position
                Obj::off_type d_offset;        // seek offset
                Obj::pos_type d_retVal;        // return from pubseekoff()
            } DATA[] = {
               //LINE      START_POS             OFFSET            RETVAL
               //----  -------------  -----------------  ----------------
               { L_,              0,                 0,                 0 },
               { L_,              1,                -1,                 0 },
               { L_,              1,                -2,                -1 },
               { L_,              0,                -5,                -1 },
               { L_,   INIT_BUFSIZE,                 0,      INIT_BUFSIZE },
               { L_,   INIT_BUFSIZE,                -1,  INIT_BUFSIZE - 1 },
               { L_,   INIT_BUFSIZE,               100,                -1 },
               { L_,   INIT_BUFSIZE,              -100,                -1 },
               { L_,   INIT_BUFSIZE, -INIT_BUFSIZE_OFF,                 0 },
               { L_,       MIDPOINT,                 0,          MIDPOINT },
               { L_,       MIDPOINT,                 5,      MIDPOINT + 5 },
               { L_,       MIDPOINT,                -5,      MIDPOINT - 5 },
               { L_,       MIDPOINT,               100,                -1 },
               { L_,       MIDPOINT,              -100,                -1 },
               { L_,       MIDPOINT,     -MIDPOINT_OFF,                 0 }
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE      = DATA[i].d_line;
                const Obj::pos_type START_POS = DATA[i].d_startPos;
                const Obj::off_type OFFSET    = DATA[i].d_offset;
                const Obj::pos_type RETVAL    = DATA[i].d_retVal;
                const Obj::pos_type FINAL_POS = (0 <= RETVAL ? RETVAL :
                                                               START_POS);

                if (veryVerbose) {
                    T_ P_(LINE) P_(START_POS) P_(OFFSET) P(RETVAL)
                }

                char buffer[INIT_BUFSIZE];
                fillBuffer(buffer, INIT_BUFSIZE);

                Obj mSB(buffer, INIT_BUFSIZE);

                Obj::pos_type retval;

                mSB.pubseekoff(START_POS, BEG, GET);
                retval = mSB.pubseekoff(OFFSET, CUR, GET);

                ASSERTV(LINE, RETVAL == retval);
            }
        }

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            static const struct {
                int           d_line;          // line number
                Obj::pos_type d_startPos;      // start seek position
                Obj::off_type d_offset;        // seek offset
                Obj::pos_type d_retVal;        // return from pubseekoff()
            } DATA[] = {
               //LINE     START_POS      OFFSET     RETVAL
               //----  ------------   ---------   --------
               { L_,              0,  MIDPOINT,   MIDPOINT },
               { L_,       MIDPOINT,  MIDPOINT,   MIDPOINT },
               { L_,   INIT_BUFSIZE,  MIDPOINT,   MIDPOINT }
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE      = DATA[i].d_line;
                const Obj::pos_type START_POS = DATA[i].d_startPos;
                const Obj::off_type OFFSET    = DATA[i].d_offset;
                const Obj::pos_type RETVAL    = DATA[i].d_retVal;

                if (veryVerbose) {
                    T_ P_(LINE) P_(START_POS) P_(OFFSET) P(RETVAL)
                }

                char buffer[INIT_BUFSIZE];

                Obj        mSB(buffer, INIT_BUFSIZE);

                Obj::pos_type retval;
                retval = mSB.pubseekpos(OFFSET, GET);

                ASSERTV(LINE, RETVAL == retval);
            }
        }
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'xsgetn' METHOD
        //
        // 'xsgetn' is a protected methods that is called by public 'sgetn'
        // method.
        //
        // Concerns:
        //: 1 Ensure that 'xsgetn' reads the correct bytes from the streambuf.
        //:
        //: 2 Ensure that 'xsgetn' properly handle requests for any number of
        //:   characters from streambufs of various length.
        //:
        //: 3 Ensure that no more than specified number of characters can be
        //:   read from the streambuf.
        //
        // Plan:
        //: 1 Read out characters from specifically constructed streambuf and
        //:   verify that all border cases handled correctly.
        //
        // Testing:
        //   streamsize xsgetn(char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'xsgetn' METHOD" << endl
                          << "======================" << endl;

        static const struct {
            int         d_line;         // line number
            int         d_readLen;      // number of bytes to read
            int         d_bufferSize;   // size of the streambuf
            int         d_retVal;       // expected return value
        } DATA[] = {
            //LINE  READ_LEN  BUFFER_SIZE  RETVAL
            //----  --------  -----------  ------
            { L_,         0,           0,       0  },
            { L_,         0,           1,       0  },
            { L_,         0,          10,       0  },
            { L_,         1,           0,       0  },
            { L_,         1,           1,       1  },
            { L_,         1,          10,       1  },
            { L_,        10,           0,       0  },
            { L_,        10,           1,       1  },
            { L_,        10,          10,      10  }
        };

        enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

        if (verbose) cout << "\nTesting 'sgetn' right after streambuf "
                             "creation." << endl;

        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_line;
                const int READ_LEN    = DATA[i].d_readLen;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int RETVAL      = DATA[i].d_retVal;

                if (veryVerbose) {
                    T_ P_(LINE) P_(BUFFER_SIZE) P_(READ_LEN) P(RETVAL)
                }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);
                const Obj&  SB = mSB;

                char *temp = new char[READ_LEN];

                streamsize retval = mSB.sgetn(temp, READ_LEN);
                ASSERTV(LINE, RETVAL == retval );
                ASSERTV(LINE, 0 == strncmp(temp, SB.data(), retval));

                delete [] temp;
                delete [] buffer;
            }
        }

        if (verbose) cout << "\nTesting successive 'sgetn' from same "
                             "streambuf." << endl;
        {
            const int BUFFER_SIZE = 100;

            char buffer[BUFFER_SIZE];
            fillBuffer(buffer, BUFFER_SIZE);

            Obj mSB(buffer, BUFFER_SIZE);

            char a[2], b[3], c[4], d[5], e[6], f[7], g[8], h[9], i[10], j[11];
            a[1] = b[2] = c[3] = d[4] = e[5] = f[6] = g[7] = h[8] = i[9] =
                  j[10] = 'X';

            char *resultBuf[10] = { a, b, c, d, e, f, g, h, i, j };

            const char *nextCompare = buffer;

            streamsize retval;
            for(int j = 1; j < 10; ++j) {
                retval = mSB.sgetn(resultBuf[j - 1], j);

                ASSERTV(j,   j == retval);
                ASSERTV(j,   0 == memcmp(nextCompare, resultBuf[j - 1], j));
                ASSERTV(j, 'X' == resultBuf[j - 1][j]);

                if (veryVerbose) {
                    cout << "\tPicked up " << j
                         << " chars from the same streambuf.\n";
                }
                nextCompare += j;
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
        // Verify the basic accessors of the 'bdlsb::FixedMemInStreamBuf'
        // object.  Note that none of the accessors are strictly needed for
        // followup tests, but simplify the overall test logic.
        //
        // Concerns:
        //: 1 Accessors work off of a references to 'const' objects.
        //:
        //: 2 The address of the user provided buffer is correctly reported.
        //:
        //: 3 The number of characters read from the stream buffer is
        //:   correctly reported.
        //
        // Plan:
        //: 1 Verify accessors for corner cases. (C 1..3)
        //:
        //: 2 Read data from the stream buffer and verify that all accessors
        //:   report expected values. (C 1..3)
        //
        // Testing:
        //   const char *data() const;
        //   streamsize length() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
            "\nTesting 'data' and 'length' with no buffer." << endl;
        {
            Obj        mSB(0, 0);
            const Obj&  SB = mSB;

            if(veryVerbose) { T_ P(SB) };

            ASSERT(0 == SB.data());
            ASSERT(0 == SB.length());

        }

        if (verbose) cout <<
            "\nTesting 'data' and 'length' after streambuf creation." << endl;
        {
            char buffer[INIT_BUFSIZE];
            fillBuffer(buffer, INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            if(veryVerbose) { T_ P(SB) };

            ASSERT(buffer       == SB.data());
            ASSERT(INIT_BUFSIZE == SB.length());

        }

        if (verbose) cout <<
          "\nTesting 'data' and 'length' after character consumption." << endl;
        {
            char buffer[INIT_BUFSIZE];
            fillBuffer(buffer, INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            Obj::int_type retval;
            for (bsl::size_t i = 0; i < INIT_BUFSIZE; ++i) {
                ASSERTV(i, INIT_BUFSIZE - i == SB.length());
                ASSERTV(i, buffer == SB.data());

                retval = mSB.sbumpc();

                ASSERTV(i, INIT_BUFSIZE - i - 1 == SB.length());
                ASSERTV(i, buffer == SB.data());
                ASSERTV(i, buffer[i] ==
                                       Obj::traits_type::to_char_type(retval));

                if (veryVerbose) {
                    cout << "\t(Diminishing) stream length is "
                         << SB.length() << ".\n";
                }
            }

            if (veryVerbose) {
                cout << "\tCausing underflow, and then verifying length.\n";
            }

            mSB.sbumpc();
            ASSERT(0 == SB.length());
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
        //   ostream& operator<<(ostream&, const FixedMemInStreamBuf&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tChecking operator<< return value." << endl;
        {
            char buffer[INIT_BUFSIZE];
            fillBuffer(buffer, INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << SB << "next";    // Ensure modifiable
            out2 << SB << "next";    // stream is returned.
        }

        if (verbose) cout << "\tChecking operator<<-generated content."
                          << endl;
        {
            const int BUFFER_SIZE = 11;

            char buffer[BUFFER_SIZE];
            fillBuffer(buffer, BUFFER_SIZE);

            Obj        mSB(buffer, BUFFER_SIZE);
            const Obj&  SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB;
            out2 << SB;

            string str1 = out1.str();
            string str2 = out2.str();
            ASSERT (str1 == str2);

            const string EXPECTED =
                     "\n0000\t01100001 01100010 01100011 01100100 "
                             "01100101 01100110 01100111 01101000"
                     "\n0008\t01101001 01101010 01101011";

            if (veryVerbose) { T_ P_(str1) P_(EXPECTED) }

            ASSERT(EXPECTED == str1);
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
        //   on using the base-class-implemented 'sbumpc' to check that
        //   reading from the stream buffer in fact reads from the
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
        //: 1 Using 2-argument constructor, create and object with the
        //:   specified buffer of the specified length. (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope. (C-4)
        //:
        //: 3 Use 'data' method to ensure that the buffer is set correctly.
        //:   (C-2)
        //:
        //: 4 Read the stream buffer content via base class method 'sbumpc'
        //:   and verify that the data is read from the client-provided buffer.
        //:   (C-2..3)
        //:
        //: 5 Read enough characters to verify that the length as specified is
        //:   completely usable, and then read one more to ensure that the
        //:   stream buffer is using the client-provided length as an upper
        //:   bound as well as a lower bound. (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid constructor parameter values, but not
        //:   triggered for adjacent valid ones. (C-5)
        //
        // Testing:
        //   FixedMemInStreamBuf(const char *, streamsize);
        //   ~FixedMemInStreamBuf();
        //   int_type sbumpc();
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        static const bsl::size_t sampleSizes[] = { 0, 1, 5 };
        enum { SAMPLE_NUM = sizeof(sampleSizes) / sizeof(sampleSizes[0]) };

        if (verbose) cout <<
            "\nTesting constructor using non-const C arrays" << endl;
        {
            for (int j = 0; j < SAMPLE_NUM; ++j) {
                const bsl::size_t BUFFER_SIZE = sampleSizes[j];

                if (veryVerbose) T_ P(BUFFER_SIZE);

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);
                const Obj&  SB = mSB;
                ASSERT(BUFFER_SIZE == SB.length());

                Obj::int_type retval;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    retval = mSB.sbumpc();
                    ASSERT (buffer[i] ==
                                       Obj::traits_type::to_char_type(retval));
                    if (veryVeryVerbose) {
                        cout << "\t'sbumpc' returned ";
                        if (Obj::traits_type::eof() == retval) {
                            cout << "EOF." << endl;
                        } else {
                            cout << Obj::traits_type::to_char_type(retval)
                                 << "." << endl;
                        }
                    }
                }
                ASSERT(0 == SB.length());

                retval = mSB.sbumpc();
                ASSERT(Obj::traits_type::eof() == retval);

                delete [] buffer;
            }
        }

        if (verbose) cout <<
            "\nTesting constructor using const C arrays" << endl;
        {
            for (int j = 0; j < SAMPLE_NUM; ++j) {
                const bsl::size_t BUFFER_SIZE = sampleSizes[j];

                if (veryVerbose) T_ P(BUFFER_SIZE);

                const char *buffer = new char[BUFFER_SIZE];
                char       *hack   = const_cast<char *>(buffer);
                fillBuffer(hack, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);
                const Obj&  SB = mSB;
                ASSERT(BUFFER_SIZE == SB.length());

                Obj::int_type retval;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    retval = mSB.sbumpc();
                    ASSERT (buffer[i] ==
                                       Obj::traits_type::to_char_type(retval));
                    if (veryVeryVerbose) {
                        cout << "\t'sbumpc' returned ";
                        if (Obj::traits_type::eof() == retval) {
                            cout << "EOF." << endl;
                        } else {
                            cout << Obj::traits_type::to_char_type(retval)
                                 << "." << endl;
                        }
                    }
                }
                ASSERT(0 == SB.length());

                retval = mSB.sbumpc();
                ASSERT(Obj::traits_type::eof() == retval);

                delete [] buffer;
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            char buffer[INIT_BUFSIZE];

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj(     0,  INIT_BUFSIZE));
            ASSERT_SAFE_PASS(Obj(     0,             0));
            ASSERT_SAFE_PASS(Obj(buffer,             0));
            ASSERT_SAFE_PASS(Obj(buffer,  INIT_BUFSIZE));
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

        if (verbose) cout <<
            "\nMake sure we can create and use a 'bdlsb::FixedMemInStreamBuf'."
                          << endl;

        char buffer[INIT_BUFSIZE];
        fillBuffer(buffer, INIT_BUFSIZE);

        if (verbose) cout << "\nCreating an object" << endl;

        Obj        mSB(buffer, INIT_BUFSIZE);
        const Obj&  SB = mSB;
        ASSERT(INIT_BUFSIZE == SB.length());
        ASSERT(buffer       == SB.data());

        if (verbose) cout << "\nCalling 'sgetc' to get character 'a'." << endl;

        Obj::int_type result;
        result = mSB.sgetc();
        ASSERT('a' == result);
        // 'sgetc' does not advance read position
        ASSERT(INIT_BUFSIZE == SB.length());

        if (verbose)
            cout << "\nCalling 'sbumpc' to get character 'a'." << endl;

        result = mSB.sbumpc();
        ASSERT('a' == result);
        // 'sbumpc' advances read position
        ASSERT(INIT_BUFSIZE - 1 == mSB.length());

        if (verbose) cout << "\nCalling 'sgetn' to get 10 characters." << endl;

        char temp[11];
        memset(temp, 'X', 11);
        mSB.sgetn(temp, 10);
        // 'sgetn' internally calls 'sbumpc'
        ASSERT(INIT_BUFSIZE - 11 == SB.length());
        ASSERT(  0 == strncmp(temp, "bcdefghijk", 10));
        ASSERT('X' == temp[10]);
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
