// bdlsb_fixedmeminput.t.cpp                                          -*-C++-*-
#include <bdlsb_fixedmeminput.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>            // for testing only
#include <bsl_sstream.h>            // for testing only
#include <bsl_string.h>             // for testing only

#include <bslx_byteoutstream.h>     // for testing only
#include <bslx_genericinstream.h>   // for testing only

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the public methods from the 'basic_streambuf'
// protocol that are implemented by the 'FixedMemInput', as well as each new
// (non-protocol) public method added in the 'FixedMemInput' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
// Primary Manipulators:
//: o Value constructor 'FixedMemInput(const char *, streamsize)'
//: o 'sbumpc'
//
/// Basic Accessors:
//: o 'data'
//: o 'length'
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FixedMemInput(const char *, streamsize);
// [ 2] ~FixedMemInput();
//
// MANIPULATORS
// [ 2] int_type sbumpc();
// [ 8] int_type sgetc();
// [ 9] int_type snextc();
// [ 5] streamsize sgetn(char_type, streamsize);
// [ 6] pos_type pubseekoff(off_type, seekdir, openmode);
// [ 6] pos_type pubseekpos(pos_type, openmode);
// [ 7] FixedMemInput *pubsetbuf(char *, streamsize);
// [ 7] FixedMemInput *pubsetbuf(const char *, streamsize);
// [10] int_type sputbackc(char_type);
// [11] int_type sungetc();
//
//
// ACCESSORS
// [ 4] const char *data() const;
// [ 4] streamsize length() const;
// [ 4] streamsize capacity() const;
// [12] streamsize in_avail();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] ostream& operator<<(ostream&, const FixedMemInput&);
// [13] USAGE EXAMPLE

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

typedef bdlsb::FixedMemInput Obj;

const bsl::size_t   INIT_BUFSIZE = 20;
const Obj::off_type INIT_BUFSIZE_POS = 20;
const bsl::size_t   MIDPOINT     = INIT_BUFSIZE / 2;
const Obj::off_type MIDPOINT_POS = INIT_BUFSIZE_POS / 2;

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
    char         ch;
    bsl::size_t  i;
    for (ch = 'a', i = 0; i < length; ++i, ++ch) {
        buffer[i] = ch;
    }
}

}  // close unnamed namespace

               // ============================
               // operator<< for FixedMemInput
               // ============================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const Obj& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream& stream, const Obj& streamBuffer)
{
    const bsl::streamsize   len  = streamBuffer.length();
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
      case 13: {
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
///Example 1: Basic Use of 'bdlsb::FixedMemInput'
/// - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlsb::FixedMemInput' class is intended to be used as a template
// parameter to the 'bslx::GenericInStream' class.  Such specialization
// provides user with performance efficient way to unexternalize BDEX encoded
// data from existing character buffer.
//
// See the 'bslx_genericinstream' component usage example for a more practical
// example of using 'bslx' streams.
//
// This example demonstrates instantiating a template, 'bslx::GenericInStream',
// on a 'bdlsb::FixedMemInput' object and using the 'bslx::GenericInStream'
// object to stream in some data.
//
// First, create 'bslx::ByteOutStream' 'outStream' and externalize some user
// data to it.  Note that this code only prepares the character buffer that is
// used to illustrate the purpose of the 'bdlsb::FixedMemInput' class.
//..
    bslx::ByteOutStream outStream(20131127);

    unsigned int MAGIC = 0x1812;

    outStream.putUint32(MAGIC);
    outStream.putInt32(83);
    outStream.putString(bsl::string("test"));
    ASSERT(outStream.isValid());
//..
// Next, create a 'bdlsb::FixedMemInput' stream buffer initialized with the
// buffer from the 'bslx::ByteOutStream' object 'outStream':
//..
    bdlsb::FixedMemInput streamBuffer(outStream.data(), outStream.length());
//..
// Then, create the 'bslx::GenericInStream' stream parametrized with
// 'bdlsb::FixedMemInput':
//..
    bslx::GenericInStream<bdlsb::FixedMemInput>  inStream(&streamBuffer);
//..
// Now, use resulting 'inStream' to unexternalize user data:
//..
    unsigned int  magic;
    int           key;
    bsl::string   value;

    inStream.getUint32(magic);
    inStream.getInt32(key);
    inStream.getString(value);
    ASSERT(inStream.isValid());
//..
// Finally, verify that the data from the supplied buffer was unexternalized
// correctly:
//..
    ASSERT(MAGIC  == magic);
    ASSERT(83     == key);
    ASSERT("test" == value);
//..

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'in_avail' METHOD
        //
        // Concerns:
        //: 1 That 'in_avail' returns the correct number of items left to
        //:   consume in the streambuf.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'in_avail' method for remaining-characters
        //:   categories 0, [1..stream buffer capacity-1] and [entire capacity]
        //:   (C-1)
        //
        // Testing:
        //   streamsize in_avail();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'in_avail' METHOD" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting 'in_avail' with no buffer." << endl;
        {
            Obj mSB(0, 0);

            ASSERT(-1 == mSB.in_avail());
        }

        if (verbose) cout <<
            "\nTesting 'in_avail' in various positions." << endl;
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

                if (veryVerbose) { T_ P_(LINE) P(BUFFER_SIZE) }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj mSB(buffer, BUFFER_SIZE);

                char temp[16];
                mSB.sgetn(temp, CHARS_TO_READ);
                streamsize retval = mSB.in_avail();
                ASSERTV(LINE, EXPECTED_AVAIL, retval,
                                                     EXPECTED_AVAIL == retval);

                if (veryVeryVerbose) {
                    cout << "\t Buffer size: "  << BUFFER_SIZE
                         << " Chars to Read: "  << CHARS_TO_READ
                         << " Expected Avail: " << EXPECTED_AVAIL << endl;
                }
                delete[] buffer;
            }
        }

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'sungetc' METHOD
        //
        // Concerns:
        //: 1 Ensure that characters can be put back into the buffer.
        //:
        //: 2 Ensure that method correctly handles corner cases.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'sungetc' method. Ensure that the stream buffer
        //:    position indicator updated correctly. (C-1..2)
        //
        // Testing:
        //   int_type sungetc();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'sungetc' METHOD" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nTesting 'sungetc' method at position 0." << endl;
        {

            static const struct {
                int           d_line;          // line number
                int           d_bufferSize;    // buffer length
            } DATA[] = {
               //LINE   BUFFER_SIZE
               //----  ------------
               { L_,              0 },
               { L_,              1 },
               { L_,   INIT_BUFSIZE },
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;

                if (veryVerbose) { T_ P_(LINE) P(BUFFER_SIZE) }

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

                delete[] buffer;
            }
        }

        if (verbose) cout <<
            "\nTesting 'sungetc' at various position." << endl;
        {

            static const struct {
                int           d_line;          // line number
                Obj::off_type d_startPos;      // start position
                Obj::int_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE        START_POS                     RETVAL
               //----  ----------------  ------------------------
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

                ASSERTV(i, RETVAL, retval, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sungetc' returned ";
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

      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'sputbackc' METHOD
        //
        // Concerns:
        //: 1 Ensure that characters can be put back into the buffer.
        //:
        //: 2 Ensure that method correctly handles corner cases.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'sputbackc' method. Ensure that the stream
        //:   buffer position indicator updated correctly. (C-1..2)
        //
        // Testing:
        //   int_type sputbackc(char_type);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'sputbackc' METHOD" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting 'sputbackc' method at position 0." << endl;
        {

            static const struct {
                int           d_line;          // line number
                int           d_bufferSize;    // buffer length
            } DATA[] = {
               //LINE   BUFFER_SIZE
               //----  ------------
               { L_,              0 },
               { L_,              1 },
               { L_,   INIT_BUFSIZE },
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int LINE        = DATA[i].d_line;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;

                if (veryVerbose) { T_ P_(LINE) P(BUFFER_SIZE) }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);

                Obj::int_type retval;

                // Putting back valid character
                retval = mSB.sputbackc('a');
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

                // Putting back invalid character
                retval = mSB.sputbackc('Z');
                ASSERT(Obj::traits_type::eof() == retval);
                if (veryVeryVerbose) {
                    cout << "\t'sputbackc' (invalid) returned ";
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
            "\nTesting 'sputbackc' at various position." << endl;
        {

            static const struct {
                int           d_line;          // line number
                Obj::off_type d_startPos;      // start position
                Obj::int_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE        START_POS                     RETVAL
               //----  ----------------  ------------------------
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

                Obj::int_type retval;

                // Putting back valid character
                mSB.pubseekpos(START_POS, GET);

                retval = mSB.sputbackc(Obj::traits_type::to_char_type(RETVAL));

                ASSERTV(i, RETVAL, retval, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sputbackc' returned ";
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
                    cout << "\t'sputbackc' (invalid) returned ";
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

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'snextc' METHOD
        //
        // Concerns:
        //: 1 Ensure that 'snextc' correctly reads character from the current
        //:   position.
        //:
        //: 2 Ensure that method correctly handles corner cases.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'snextc' method. Ensure that the stream
        //:   buffer position indicator updated correctly. (C-1..2)
        //
        // Testing:
        //   int_type snextc();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'snextc' METHOD" << endl
                          << "=======================" << endl;

        if (verbose) cout <<
            "\nTesting 'snextc' method at position 0." << endl;
        {

            static const struct {
                int           d_line;          // line number
                int           d_bufferSize;    // buffer length
                Obj::int_type d_retVal;        // expected character
            } DATA[] = {
               //LINE    BUFFER_SIZE  RETVAL
               //----  -------------  ------
               { L_,              0,      -1 },
               { L_,              1,      -1 },
               { L_,              2,     'b' },
               { L_,   INIT_BUFSIZE,     'b' }
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE        = DATA[i].d_line;
                const int           BUFFER_SIZE = DATA[i].d_bufferSize;
                const Obj::int_type RETVAL      = DATA[i].d_retVal;

                if (veryVerbose) { T_ P_(LINE) P(BUFFER_SIZE) }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);

                Obj::int_type retval;

                // Reading character at current position
                retval = mSB.snextc();
                ASSERT(RETVAL == retval);
                if (veryVeryVerbose) {
                    cout << "\t'snextc' returned ";
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
            "\nTesting 'snextc' at various position." << endl;
        {

            static const struct {
                int           d_line;          // line number
                Obj::off_type d_startPos;      // start position
                Obj::int_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE        START_POS                     RETVAL
               //----  ----------------  ------------------------
               { L_,                 0,                       'b' },
               { L_,                 1,                       'c' },
               { L_,                 2,                       'd' },
               { L_,          MIDPOINT,        'a' + (MIDPOINT+1) },
               { L_,    INIT_BUFSIZE-2,  'a' + (INIT_BUFSIZE - 1) },
               { L_,    INIT_BUFSIZE-1,                       -1  },
               { L_,      INIT_BUFSIZE,                       -1  }
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

                Obj::int_type retval;

                // Putting back valid character
                mSB.pubseekpos(START_POS, GET);

                retval = mSB.snextc();

                ASSERTV(i, RETVAL, retval, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'snextc' returned ";
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
        // TESTING 'sgetc' METHOD
        //
        // Concerns:
        //: 1 Ensure that 'sgetc' correctly reads character from the current
        //:   position.
        //:
        //: 2 Ensure that method correctly handles corner cases.
        //
        // Plan:
        //: 1 Using the table-driven test-case-implementation technique, test
        //:   the output of the 'sgetc' method. Ensure that the stream
        //:   buffer position indicator updated correctly. (C-1..2)
        //
        // Testing:
        //   int_type sgetc();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'sgetc' METHOD" << endl
                          << "======================" << endl;

        if (verbose) cout <<
            "\nTesting 'sgetc' method at position 0." << endl;
        {

            static const struct {
                int           d_line;          // line number
                int           d_bufferSize;    // buffer length
                Obj::int_type d_retVal;        // expected character
            } DATA[] = {
               //LINE    BUFFER_SIZE  RETVAL
               //----  -------------  ------
               { L_,              0,      -1  },
               { L_,              1,     'a'  },
               { L_,   INIT_BUFSIZE,     'a'  }
            };
            enum { NUM_DATA = sizeof(DATA)/sizeof(DATA[0]) };

            for (int i = 0; i < NUM_DATA; ++i ) {
                const int           LINE        = DATA[i].d_line;
                const int           BUFFER_SIZE = DATA[i].d_bufferSize;
                const Obj::int_type RETVAL      = DATA[i].d_retVal;

                if (veryVerbose) { T_ P_(LINE) P(BUFFER_SIZE) }

                char *buffer = new char[BUFFER_SIZE];
                fillBuffer(buffer, BUFFER_SIZE);

                Obj        mSB(buffer, BUFFER_SIZE);

                Obj::int_type retval;

                // Reading character at current position
                retval = mSB.sgetc();
                ASSERT(RETVAL == retval);
                if (veryVeryVerbose) {
                    cout << "\t'sgetc' returned ";
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
            "\nTesting 'sgetc' at various position." << endl;
        {

            static const struct {
                int           d_line;          // line number
                Obj::off_type d_startPos;      // start position
                Obj::int_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE        START_POS                     RETVAL
               //----  ----------------  ------------------------
               { L_,                 0,                       'a' },
               { L_,                 1,                       'b' },
               { L_,                 2,                       'c' },
               { L_,          MIDPOINT,          'a' + (MIDPOINT) },
               { L_,    INIT_BUFSIZE-1,  'a' + (INIT_BUFSIZE - 1) },
               { L_,      INIT_BUFSIZE,                        -1 }
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

                Obj::int_type retval;

                // Putting back valid character
                mSB.pubseekpos(START_POS, GET);

                retval = mSB.sgetc();

                ASSERTV(i, RETVAL, retval, RETVAL == retval);

                if (veryVeryVerbose) {
                    cout << "\t'sgetc' returned ";
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

      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'pubsetbuf' METHODS
        //
        // Protected 'setbuf' methods are called by the public 'pubsetbuf'
        // method.  Ensure that we can reset put area to client-provided buffer
        // for a constructed stream buffer object
        //
        // Concerns:
        //: 1 Ensure the 'pubsetbuf' methods reset internal buffer.
        //:
        //: 2 Ensure the 'pubsetbuf' methods reset position indicator.
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
        //   FixedMemInput *pubsetbuf(char *, streamsize);
        //   FixedMemInput *pubsetbuf(const char *, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'pubsetbuf' METHODS" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nTesting 'pubsetbuf' using non-const C arrays." << endl;

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
                ASSERT(buffer      == SB.data());
                ASSERT(BUFFER_SIZE == SB.length());
                ASSERT(BUFFER_SIZE == SB.capacity());

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
            "\nTesting 'pubsetbuf' using const C arrays." << endl;
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
                ASSERT(1          == SB.capacity());
                ASSERT(fakeBuffer == SB.data());

                mSB.pubsetbuf(buffer, BUFFER_SIZE);
                ASSERT(buffer      == SB.data());
                ASSERT(BUFFER_SIZE == SB.length());
                ASSERT(BUFFER_SIZE == SB.capacity());

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

      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'seek' METHODS
        //
        // Concerns:
        //: 1 Seeking is correct for:
        //:   - all relative positions.
        //:   - positive, 0, and negative values.
        //:   - out of buffer boundaries.
        //:
        //: 2 Seeking into the "put" area has no effect.
        //:
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-1..2)
        //:
        //
        // Testing:
        //   pos_type pubseekoff(off_type, seekdir, openmode);
        //   pos_type pubseekpos(pos_type, openmode);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'seek' METHODS" << endl
                          << "======================" << endl;

        if (verbose) cout <<
                "\nTesting 'seekoff' after buffer creation." << endl;

        {
            static const struct {
                int           d_line;          // line number
                io_openmode   d_areaFlags;     // "put"/"get" area
                Obj::off_type d_offset;        // seek offset
                io_seekdir    d_base;          // relative position
                Obj::pos_type d_retVal;        // expected return value
            } DATA[] = {
               //LINE  AREA                OFFSET  BASE            RETVAL
               //----  ----   -------------------  ----  ----------------
               // seekoff from the start of the streambuf
               { L_,   GET,                    0,  BEG,                 0 },
               { L_,   GET,                    1,  BEG,                 1 },
               { L_,   GET,                   -1,  BEG,                -1 },
               { L_,   GET, INIT_BUFSIZE_POS - 1,  BEG,  INIT_BUFSIZE - 1 },
               { L_,   GET,                  100,  BEG,                -1 },
               { L_,   GET,     INIT_BUFSIZE_POS,  BEG,      INIT_BUFSIZE },

               // seekoff from the end of the streambuf.
               { L_,   GET,                    0,  END,      INIT_BUFSIZE },
               { L_,   GET,                    1,  END,                -1 },
               { L_,   GET,                   -1,  END,  INIT_BUFSIZE - 1 },
               { L_,   GET,                 -100,  END,                -1 },
               { L_,   GET,    -INIT_BUFSIZE_POS,  END,                 0 },

               // seekoff from the current cursor, where cur == begin
               { L_,   GET,                    0,  CUR,                 0 },
               { L_,   GET,                    1,  CUR,                 1 },
               { L_,   GET,                   -1,  CUR,                -1 },
               { L_,   GET,     INIT_BUFSIZE_POS,  CUR,      INIT_BUFSIZE },

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

                ASSERTV(LINE, RETVAL, retval, RETVAL == retval);
            }
        }

        if (verbose) cout << "\nTesting 'pubseekoff' from a variety of "
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
               { L_,   INIT_BUFSIZE, -INIT_BUFSIZE_POS,                 0 },
               { L_,       MIDPOINT,                 0,          MIDPOINT },
               { L_,       MIDPOINT,                 5,      MIDPOINT + 5 },
               { L_,       MIDPOINT,                -5,      MIDPOINT - 5 },
               { L_,       MIDPOINT,               100,                -1 },
               { L_,       MIDPOINT,              -100,                -1 },
               { L_,       MIDPOINT,     -MIDPOINT_POS,                 0 }
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
                fillBuffer(buffer, INIT_BUFSIZE);

                Obj mSB(buffer, INIT_BUFSIZE);


                Obj::pos_type retval;

                mSB.pubseekoff(START_POS, BEG, GET);
                retval = mSB.pubseekoff(OFFSET, CUR, GET);

                ASSERTV(LINE, RETVAL, retval, RETVAL == retval);
            }
        }

        if (verbose) cout << "\nTesting 'pubseekpos' method." << endl;
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

                ASSERTV(LINE, RETVAL, retval, RETVAL == retval);
            }
        }
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'sgetn' METHOD
        //
        // Concerns:
        //: 1 Ensure that 'sgetn' reads the correct bytes from the streambuf.
        //:
        //: 2 Ensure that 'sgetn' properly handle requests for any number of
        //:   characters from stream buffers of various length.
        //:
        //: 3 Ensure that no more than specified number of characters can be
        //:   read from the streambuf.
        //
        // Plan:
        //: 1 Read out characters from specifically constructed streambuf and
        //:   verify that all border cases handled correctly.
        //
        // Testing:
        //   streamsize sgetn(char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'sgetn' METHOD" << endl
                          << "======================" << endl;

        static const struct {
            int         d_line;         // line number
            int         d_readLen;      // number of bytes to read
            int         d_bufferSize;   // size of the streambuf
            int         d_retVal;       // expected return value
        } DATA[] = {
            //LINE  READ_LEN  BUFFER_SIZE  RETVAL
            //----  --------  -----------  ------
            { L_,         0,           0,       0 },
            { L_,         0,           1,       0 },
            { L_,         0,          10,       0 },
            { L_,         1,           0,       0 },
            { L_,         1,           1,       1 },
            { L_,         1,          10,       1 },
            { L_,        10,           0,       0 },
            { L_,        10,           1,       1 },
            { L_,        10,          10,      10 }
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
                ASSERTV(LINE, RETVAL, retval, RETVAL == retval );
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

                ASSERTV(j, retval, j == retval);
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

      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Verify the basic accessors of the 'bdlsb::FixedMemInput' object.
        // Note that none of the accessors are strictly needed for followup
        // tests, but simplify the overall test logic.
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
        //   streamsize capacity() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
            "\nTesting accessors with no buffer." << endl;
        {
            Obj        mSB(0, 0);
            const Obj&  SB = mSB;

            ASSERT(0 == SB.data());
            ASSERT(0 == SB.length());
            ASSERT(0 == SB.capacity());

            if(veryVerbose) { T_ P(SB) };
        }

        if (verbose) cout <<
            "\nTesting accessors after streambuf creation." << endl;
        {
            char buffer[INIT_BUFSIZE];
            fillBuffer(buffer, INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            ASSERT(buffer       == SB.data());
            ASSERT(INIT_BUFSIZE == SB.length());
            ASSERT(INIT_BUFSIZE == SB.capacity());

            if(veryVerbose) { T_ P(SB) };
        }

        if (verbose) cout <<
          "\nTesting accessors after characters consumption." << endl;
        {
            char buffer[INIT_BUFSIZE];
            fillBuffer(buffer, INIT_BUFSIZE);

            Obj        mSB(buffer, INIT_BUFSIZE);
            const Obj&  SB = mSB;

            Obj::int_type retval;
            for (bsl::size_t i = 0; i < INIT_BUFSIZE; ++i) {
                ASSERTV(i, buffer           == SB.data());
                ASSERTV(i, INIT_BUFSIZE - i == SB.length());
                ASSERTV(i, INIT_BUFSIZE     == SB.capacity());

                retval = mSB.sbumpc();

                ASSERTV(i, buffer               == SB.data());
                ASSERTV(i, INIT_BUFSIZE - i - 1 == SB.length());
                ASSERTV(i, INIT_BUFSIZE         == SB.capacity());
                ASSERTV(i, buffer[i] == (char) retval);

                if (veryVerbose) {
                    cout << "\t(Diminishing) stream length is "
                         << SB.length() << ".\n";
                }
            }

            if (veryVerbose) {
                cout << "\tCausing underflow, and then verifying length.\n";
            }

            mSB.sbumpc();
            ASSERT(0            == SB.length());
            ASSERT(INIT_BUFSIZE == SB.capacity());
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
        //   ostream& operator<<(ostream&, const FixedMemInput&);
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

            if (veryVerbose) { T_ P(str1) T_  P(EXPECTED) }

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
        //   on using the 'sbumpc' to check that reading from the stream buffer
        //   in fact reads from the client-provided buffer.
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
        //: 3 Read the stream buffer content via class method 'sbumpc'
        //:   and verify that the data is read from the client-provided buffer.
        //:   (C-2..3)
        //:
        //: 4 Read enough characters to verify that the length as specified is
        //:   completely usable, and then read one more to ensure that the
        //:   stream buffer is using the client-provided length as an upper
        //:   bound as well as a lower bound. (C-3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid constructor parameter values, but not
        //:   triggered for adjacent valid ones. (C-5)
        //
        // Testing:
        //   FixedMemInput(const char *, streamsize);
        //   ~FixedMemInput();
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
                ASSERT(BUFFER_SIZE == SB.length());

                Obj::int_type ret;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    ret = mSB.sbumpc();
                    ASSERT (buffer[i] == (char)ret);
                    if (veryVerbose) {T_ P((char)ret)}
                }
                ASSERT(0 == SB.length());

                ret = mSB.sbumpc();
                ASSERT(Obj::traits_type::eof() == ret);

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

                Obj::int_type ret;
                for (bsl::size_t i = 0; i < BUFFER_SIZE; ++i) {
                    ret = mSB.sbumpc();
                    ASSERT (buffer[i] == (char)ret);
                    if (veryVerbose) {T_ P((char)ret)}
                }
                ASSERT(0 == SB.length());

                ret = mSB.sbumpc();
                ASSERT(Obj::traits_type::eof() == ret);

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
            "\nMake sure we can create and use a 'bdlsb::FixedMemInput'."
                          << endl;

        char buffer[INIT_BUFSIZE];
        fillBuffer(buffer, INIT_BUFSIZE);

        if (verbose) cout << "\nCreating an object" << endl;

        Obj        mSB(buffer, INIT_BUFSIZE);
        const Obj&  SB = mSB;
        ASSERT(INIT_BUFSIZE == SB.length());
        ASSERT(buffer == SB.data());

        if (verbose) cout << "\nCalling 'sgetc' to get character 'a'." << endl;

        int result;
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

        if (verbose)
            cout << "\nCalling 'snextc' to get character 'c'." << endl;

        result = mSB.snextc();
        ASSERT('c' == result);
        // 'snextc' advances read position
        ASSERT(INIT_BUFSIZE - 2 == mSB.length());

        if (verbose) cout << "\nCalling 'sgetn' to get 10 characters." << endl;

        char temp[11];
        memset(temp, 'X', 11);
        mSB.sgetn(temp, 10);
        // 'sgetn' internally calls 'sbumpc'
        ASSERT(INIT_BUFSIZE - 12 == SB.length());
        ASSERT(  0 == strncmp(temp, "cdefghijkl", 10));
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
