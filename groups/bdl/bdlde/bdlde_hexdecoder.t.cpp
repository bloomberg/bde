// bdlde_hexdecoder.t.cpp                                             -*-C++-*-
#include <bdlde_hexdecoder.h>

#include <bdlde_hexencoder.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cstring.h>  // 'bsl::strlen', 'bsl::strncmp'
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a decoder for converting hexadecimal
// character representation into plain text.
//
// Testing of 'bdlde::HexDecoder' consists of verifying that input character
// sequence is converted correctly and the object itself takes the expected
// state as a result of method calls.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] HexDecoder();
//
// MANIPULATORS
// [ 2] int convert(OUTPUT_ITERATOR, INPUT_ITERATOR, INPUT_ITERATOR);
// [ 2] int convert(OUTPUT_ITER, int *, int *, INPUT_ITER,INPUT_ITER,int);
// [ 2] int endConvert();
// [ 5] void reset();
//
// ACCESSORS
// [ 3] bool isDone() const;
// [ 3] bool isError() const;
// [ 3] bool isAcceptable() const;
// [ 3] int outputLength() const;
// [ 4] bool isInitialState() const;
// [ 4] bool isMaximal() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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
//                         GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

typedef bdlde::HexDecoder Obj;

// The following is a very long text to use in the usage example as a stress
// test.

const char *BLOOMBERG_NEWS =
"        (Commentary.  Chet Currier is a Bloomberg News  \n"
"columnist.  The opinions expressed are his own.)  \n"
" \n"
"By Chet Currier \n"
"     Sept.  14 (Bloomberg) -- The trouble with college finance in \n"
"21st Century America is way too much homework.  \n"
"     Study up on UGMAs, ESAs, Section 529 savings programs that \n"
"come in more than 50 different versions, and prepaid tuition \n"
"plans.  Learn when interest on U.S.  Savings Bonds may, or may not, \n"
"be exempt from taxes when the money is used to pay educational \n"
"expenses.  \n"
"     Comb through some of the 117,687 items that turn up when you \n"
"do a ``pay for college'' search on the Web site of Amazon.com.  \n"
"Read articles like ``Understanding the Aid Form,'' a discussion \n"
"of something called the FAFSA that ran as part of a 28-page \n"
"special ``Paying for College'' section in the Sept.  6 U.S.  News & \n"
"World Report.  \n"
"     ``A college education is a worthwhile, but expensive, \n"
"investment,'' says the Vanguard Group in a current bulletin to \n"
"investors in its $730 billion stable of mutual funds.  ``The right \n"
"savings plan can help ease the financial burden, but with the \n"
"variety of alternatives available you may need a crash course on \n"
"the pros and cons of each one.'' \n"
"     Please, life already has all the crash courses it needs.  Is \n"
"this the best we can do? Even if each piece of the system was \n"
"designed with the best of intentions, the aggregate has become an \n"
"absurd monstrosity.  \n"
" \n"
"                         Fails the Test \n"
" \n"
"     The system's first and most obvious offense is its utter \n"
"disregard for the virtues of simplicity, both aesthetic and \n"
"practical.  That's just the beginning.  \n"
"     It wastes huge amounts of time, energy and economic \n"
"resources.  Along the way, it undercuts several of the most \n"
"important ideas colleges are supposed to be dealing in.  \n"
"     Consider the difference between real problems that naturally \n"
"arise in life, and artificial obstacles that humans put in their \n"
"own path.  Dealing with real problems is an unavoidable part of \n"
"the human condition -- and quite often turns out to be a \n"
"productive exercise that gets you somewhere.  Artificial problems, \n"
"being both unnecessary and sterile, offer few benefits of that \n"
"kind.  \n"
"     The wish to give young people an education, and the \n"
"necessity of paying the cost of that effort, are classic examples \n"
"of naturally occurring problems.  When you study 529 plans, by \n"
"contrast, you learn nothing but an arbitrary set of details that \n"
"have no bearing on anything else.  \n"
" \n"
"                          Hypocritical \n"
" \n"
"     College should teach ways of thinking that a student can \n"
"apply and build on after the degree is awarded.  The college- \n"
"finance system is completely out of synch with that ideal.  \n"
"     In the few years while the teenagers in a family struggle \n"
"through the miasma known as the ``admissions process,'' college- \n"
"finance information presents itself as vital.  Immediately after \n"
"the last child matriculates, it becomes junk to be jettisoned as \n"
"fast as the brain cells will allow.  \n"
"     Also, college ought to be a place to acquire a sense of \n"
"ethics -- more now than ever amid the outcry over the moral and \n"
"ethical failures of so many in business, government, religion and \n"
"the press.  Yet a basic part of college planning, usually \n"
"presented without the slightest moral compunction, is learning \n"
"the slyest ways to game the system.  \n"
"     Parents saving for a child's tuition are confronted with \n"
"issues such as ``can that stash hurt your aid?'' to quote one \n"
"headline in the U.S.  News college-planning report.  Here, son, you \n"
"go study Plato and Aristotle, and we'll scrounge up the money to \n"
"pay for it by hiding assets in Aunt Adelaide's name.  \n"
" \n"
"                        Lessons to Learn \n"
" \n"
"     ``Mankind, left free, instinctively complicates life,'' said \n"
"the writer Katharine Fullerton Gerould.  \n"
"     Beyond the portrait it paints of human folly, though, maybe \n"
"there is something to be learned from the sorry state of college \n"
"finance.  It gives us a glimpse into the workings of a so-called \n"
"``information economy'' where, instead of education steering us \n"
"toward life, life steers us to education.  \n"
"     Like the computer, education was originally a tool, now is a \n"
"product unto itself.  Hence the resume of achievements and \n"
"experiences that have come to be required of children aspiring to \n"
"get into a ``selective'' college.  \n"
"     The economics of information is pretty new stuff.  One sign \n"
"we are starting to figure it out will come when we do a better \n"
"job of designing mechanisms like college finance.  \n"
" \n"
"--Editors: Ahearn, Wolfson \n"
" \n"
"Story Illustration: To graph increase in population of U.S.  \n"
"college graduates, click on  USP CL25 <Index> GP <GO> .  To see \n"
"additional Currier columns, click on  NI CURRIER <GO> .  To \n"
"comment on this column, click on  LETT <GO>  and send a letter to \n"
"the editor.  \n"
" \n"
"To contact the writer of this column: Chet Currier in New York \n"
"(1) (212) 318-2605 or ccurrier@bloomberg.net.";

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

void decode(char *output, const char *input, bsl::size_t inputSize)
    // Convert the specified 'input' hex encoding of the specified 'inputSize'
    // into plain text, and write the decoded text to the specified 'output'.
    // The behaviour is undefined unless the 'inputSize' is an even number.
{
    bsl::map<char, const char *> decodeTable;
    decodeTable['0'] = "0000";
    decodeTable['1'] = "0001";
    decodeTable['2'] = "0010";
    decodeTable['3'] = "0011";
    decodeTable['4'] = "0100";
    decodeTable['5'] = "0101";
    decodeTable['6'] = "0110";
    decodeTable['7'] = "0111";
    decodeTable['8'] = "1000";
    decodeTable['9'] = "1001";
    decodeTable['a'] = "1010";
    decodeTable['b'] = "1011";
    decodeTable['c'] = "1100";
    decodeTable['d'] = "1101";
    decodeTable['e'] = "1110";
    decodeTable['f'] = "1111";
    decodeTable['A'] = "1010";
    decodeTable['B'] = "1011";
    decodeTable['C'] = "1100";
    decodeTable['D'] = "1101";
    decodeTable['E'] = "1110";
    decodeTable['F'] = "1111";

    ASSERT(!(inputSize % 2));

    while (inputSize) {
        for (char i = 0; i < 4; ++i) {
            if ('1' == decodeTable[*input][i]) {
                *output |= static_cast<char>(1 << (7 - i));
            }
        }
        ++input;
        --inputSize;

        for (char i = 0; i < 4; ++i) {
            if ('1' == decodeTable[*input][i]) {
                *output |= static_cast<char>(1 << (3 - i));
            }
        }
        ++input;
        --inputSize;

        ++output;
    }
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of 'bdlde::HexDecoder'
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example shows how to use a 'bdlde::HexDecoder' object to
// implement a function, 'streamDecoder', that reads a hex representation from
// 'bsl::istream', decodes that text, and writes the decoded text to a
// 'bsl::ostream'.  'streamDecoder' returns 0 on success and a negative value
// if the input data could not be successfully decoded or if there is an I/O
// error.
//..
    int streamDecoder(bsl::ostream& os, bsl::istream& is)
        // Read the entire contents of the specified input stream 'is', convert
        // the input hex encoding into plain text, and write the decoded text
        // to the specified output stream 'os'.  Return 0 on success, and a
        // negative value otherwise.
    {
        enum {
            SUCCESS      =  0,
            DECODE_ERROR = -1,
            IO_ERROR     = -2
        };
//..
// First we create an object, create buffers for storing data, and start loop
// that runs while the input stream contains some data:
//..
        bdlde::HexDecoder converter;

        const int INBUFFER_SIZE  = 1 << 10;
        const int OUTBUFFER_SIZE = 1 << 10;

        char inputBuffer[INBUFFER_SIZE];
        char outputBuffer[OUTBUFFER_SIZE];

        char *output    = outputBuffer;
        char *outputEnd = outputBuffer + sizeof outputBuffer;

        while (is.good()) {  // input stream not exhausted
//..
// On each iteration we read some data from the input stream:
//..
            is.read(inputBuffer, sizeof inputBuffer);

            const char *input    = inputBuffer;
            const char *inputEnd = input + is.gcount();

            while (input < inputEnd) { // input encoding not complete

                int numOut = 0;
                int numIn  = 0;
//..
// Convert obtained text using 'bdlde::HexDecoder':
//..
                int status = converter.convert(
                                         output,
                                         &numOut,
                                         &numIn,
                                         input,
                                         inputEnd,
                                         static_cast<int>(outputEnd - output));
                if (status < 0) {
                    return DECODE_ERROR;                              // RETURN
                }

                output += numOut;
                input  += numIn;
//..
// And write decoded text to the output stream:
//..
                if (output == outputEnd) {  // output buffer full; write data
                    os.write(outputBuffer, sizeof outputBuffer);
                    if (os.fail()) {
                        return IO_ERROR;                              // RETURN
                    }
                    output = outputBuffer;
                }
            }
        }

        if (output > outputBuffer) {
            os.write (outputBuffer, output - outputBuffer);
        }
//..
// Then we need to complete the work of our decoder:
//..
        int more = converter.endConvert();
        if (more < 0) {
            return DECODE_ERROR;                                      // RETURN
        }

        return is.eof() && os.good() ? SUCCESS : IO_ERROR;
    }
//..

int streamEncoder(bsl::ostream& os, bsl::istream& is)
    // Read the entire contents of the specified input stream 'is', convert
    // the input plain text to hex representation, and write the encoded text
    // to the specified output stream 'os'.  Return 0 on success, and a
    // negative value otherwise.
{
    enum {
        SUCCESS      =  0,
        ENCODE_ERROR = -1,
        IO_ERROR     = -2
    };

    bdlde::HexEncoder converter;

    const int INBUFFER_SIZE  = 1 << 10;
    const int OUTBUFFER_SIZE = 1 << 10;

    char inputBuffer[INBUFFER_SIZE];
    char outputBuffer[OUTBUFFER_SIZE];

    char *output    = outputBuffer;
    char *outputEnd = outputBuffer + sizeof outputBuffer;

    while (is.good()) {  // input stream not exhausted

        is.read(inputBuffer, sizeof inputBuffer);

        const char *input    = inputBuffer;
        const char *inputEnd = input + is.gcount();

        while (input < inputEnd) { // input encoding not complete

            int numOut;
            int numIn;

            int status = converter.convert(
                                         output,
                                         &numOut,
                                         &numIn,
                                         input,
                                         inputEnd,
                                         static_cast<int>(outputEnd - output));
            if (status < 0) {
                return ENCODE_ERROR;                                  // RETURN
            }

            output += numOut;
            input  += numIn;

            if (output == outputEnd) {  // output buffer full; write data
                os.write(outputBuffer, sizeof outputBuffer);
                if (os.fail()) {
                    return IO_ERROR;                                  // RETURN
                }
                output = outputBuffer;
            }
        }
    }

    while (1) {
        int numOut = 0;

        int more = converter.endConvert(output,
                                        &numOut,
                                        static_cast<int>(outputEnd - output));
        if (more < 0) {
            return ENCODE_ERROR;                                      // RETURN
        }

        output += numOut;

        if (!more) { // no more output
            break;
        }

        ASSERT(output == outputEnd);  // output buffer is full

        os.write(outputBuffer, sizeof outputBuffer);  // write buffer
        if (os.fail()) {
            return IO_ERROR;                                          // RETURN
        }
        output = outputBuffer;
    }

    if (output > outputBuffer) {
        os.write(outputBuffer, output - outputBuffer);
    }

    return is.eof() && os.good() ? SUCCESS : IO_ERROR;
}


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

// Next, to demonstrate how our function works we need to create a stream with
// encoded data.  Assume that we have some character string, 'BLOOMBERG_NEWS',
// and a function, 'streamEncoder' mirroring the work of the 'streamDecoder':
//..
    bsl::istringstream inStream(bsl::string(BLOOMBERG_NEWS,
                                            strlen(BLOOMBERG_NEWS)));
    bsl::stringstream  outStream;
    bsl::stringstream  backInStream;

    ASSERT(0 == streamEncoder(outStream,    inStream));
//..
// Now, we use our function to decode text:
//..
    ASSERT(0 == streamDecoder(backInStream, outStream));
//..
// Finally, we observe that the output fully matches the original text:
//..
    ASSERT(0 == strcmp(BLOOMBERG_NEWS, backInStream.str().c_str()));
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //
        // Concerns:
        //: 1 The 'reset' method can be successfully called in any object's
        //:   state and returns the object to its initial state.
        //
        // Plan:
        //: 1 Create an object and set it to different states.  In each state
        //:   call the 'reset' method and verify object's status.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset'" << endl
                          << "===============" << endl;

        char hexToDecode[] = "444";
        char buffer[2];
        int  numIn = 0;
        int  numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Testing inner buffer resetting.

        int rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            hexToDecode,
                            hexToDecode + 1);
        ASSERTV(rc, 0 == rc);

        ASSERTV(!X.isAcceptable());

        mX.reset();

        ASSERTV(X.isInitialState());

        // Testing output length resetting.

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 2);
        ASSERTV(rc, 0 == rc);

        ASSERTV(X.isAcceptable());
        ASSERTV(X.outputLength(), 1 == X.outputLength());

        mX.reset();

        ASSERTV(X.isInitialState());

        // Testing state resetting.

        rc = mX.endConvert();
        ASSERTV(rc, 0 == rc);

        ASSERTV(X.isDone());

        mX.reset();

        ASSERTV(X.isInitialState());

        // Various parameters resetting.

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 3);
        ASSERTV(rc,  0 == rc);
        rc = mX.endConvert();
        ASSERTV(rc, -1 == rc);

        ASSERTV(X.isError());
        ASSERTV(X.outputLength(), 1 == X.outputLength());

        mX.reset();

        ASSERTV(X.isInitialState());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //: 1 'isInitialState' takes into account not only the state of the
        //:   object but also the presence of pending character in the internal
        //:   buffer and output length.
        //:
        //: 2 All accessors are 'const'.
        //
        // Plan:
        //: 1 Create an object and set it to different states.  In each state
        //:   invoke functions under test from a reference providing
        //:   non-modifiable access to the object and verify the returned
        //:   value.  (C-1,2)
        //
        // Testing:
        //   bool isInitialState() const;
        //   bool isMaximal() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        char hexToDecode[] = "44";
        char buffer[2];
        int  numIn = 0;
        int  numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Initial state
        ASSERT( X.isInitialState());
        ASSERT(!X.isMaximal()  );

        int rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            hexToDecode,
                            hexToDecode + 1);
        ASSERTV(rc, 0 == rc);

        // intermediate state (no errors, inner buffer contains unhandled
        // symbol)
        ASSERT(!X.isInitialState());
        ASSERT(!X.isMaximal()  );

        ASSERTV(X.outputLength(), 0 == X.outputLength());

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 1);
        ASSERTV(rc, 0 == rc);

        // accepting state (no errors, no unhandled symbols)
        ASSERT(!X.isInitialState());
        ASSERT(!X.isMaximal()  );

        rc = mX.endConvert();
        ASSERTV(rc, 0 == rc);

        // done state
        ASSERT(!X.isInitialState());
        ASSERT(!X.isMaximal()  );

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 1);
        ASSERTV(rc, -2 == rc);

        // error state
        ASSERT(!X.isInitialState());
        ASSERT(!X.isMaximal()  );
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Basic accessors were used in the test for primary manipulators.
        //   In order not to duplicate the code, we will conduct surface tests
        //   here, bringing the object into the required state and checking it.
        //
        // Concerns:
        //: 1 'isAcceptable' takes into account not only the state of the
        //:   object but also the presence of pending character in the internal
        //:   buffer.
        //:
        //: 2 All accessors are 'const'.
        //
        // Plan:
        //: 1 Create an object and set it to different states.  In each state
        //:   invoke functions under test from a reference providing
        //:   non-modifiable access to the object and verify the returned
        //:   value.  (C-1,2)
        //
        // Testing:
        //   bool isDone() const;
        //   bool isError() const;
        //   bool isAcceptable() const;
        //   int outputLength() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;


        char hexToDecode[] = "44";
        char buffer[2];
        int  numIn = 0;
        int  numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Initial state
        ASSERT( X.isAcceptable());
        ASSERT(!X.isDone()      );
        ASSERT(!X.isError()     );

        ASSERTV(X.outputLength(), 0 == X.outputLength());

        int rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            hexToDecode,
                            hexToDecode + 1);
        ASSERTV(rc, 0 == rc);

        // intermediate state (no errors, inner buffer contains unhandled
        // symbol)
        ASSERT(!X.isAcceptable());
        ASSERT(!X.isDone()      );
        ASSERT(!X.isError()     );

        ASSERTV(X.outputLength(), 0 == X.outputLength());

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 1);
        ASSERTV(rc, 0 == rc);

        // accepting state (no errors, no unhandled symbols)
        ASSERT( X.isAcceptable());
        ASSERT(!X.isDone()      );
        ASSERT(!X.isError()     );

        ASSERTV(X.outputLength(), 1 == X.outputLength());

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 2);
        ASSERTV(rc, 0 == rc);

        ASSERT( X.isAcceptable());
        ASSERT(!X.isDone()      );
        ASSERT(!X.isError()     );

        ASSERTV(X.outputLength(), 2 == X.outputLength());

        rc = mX.endConvert();
        ASSERTV(rc, 0 == rc);

        // done state
        ASSERT(!X.isAcceptable());
        ASSERT( X.isDone()      );
        ASSERT(!X.isError()     );

        ASSERTV(X.outputLength(), 2 == X.outputLength());

        rc = mX.convert(buffer, &numOut, &numIn, hexToDecode, hexToDecode + 1);
        ASSERTV(rc, -2 == rc);

        // error state
        ASSERT(!X.isAcceptable());
        ASSERT(!X.isDone()      );
        ASSERT( X.isError()     );

        ASSERTV(X.outputLength(), 2 == X.outputLength());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   To bring an object into any possible state we have to use three
        //   methods: constructor to create an object and put it to the initial
        //   state, 'convert' to modify output length and put object to the
        //   'error' state and 'endCovert' to put object to the 'done' state.
        //   In order to completely test the 'convert' function we need to call
        //   it from the 'error' and 'done' states, which in turn required a
        //   fully tested 'endConvert' function.  To resolve this conflict we
        //   test single calls of the 'convert' first, then we test the
        //   'endConvert' and finally we test sequential calls of the 'convert'
        //   function.
        //
        // Concerns:
        //: 1 All characters from the hexadecimal alphabet are correctly
        //:   converted.
        //:
        //: 2 Attempt to convert non-hexadecimal symbol sets an object to the
        //:   'error' state.
        //:
        //: 3 The 'convert' method correctly handles a zero-length input
        //:   sequence.
        //:
        //: 4 The 'convert' method ignores space symbols.
        //:
        //: 5 Any pending character stored in the inner buffer is used for
        //:   conversion.
        //:
        //: 6 The output length value considers the results of all successive
        //:   'convert' calls, not just the last one.
        //:
        //: 7 The 'convert' method returns different values for different
        //:   errors.
        //:
        //: 8 The 'endConvert' method can be called several times in a row.
        //:
        //: 9 The 'convert' method correctly handles "real-life" iterators.
        //:
        //:10 QoI: Asserted precondition violations are detected when enabled.
        //:   errors.
        //
        // Plan:
        //: 1 Create an object and verify that it is in the expected state.
        //:
        //: 2 Using a predefined sequence of hexadecimal characters run the
        //:   loop by changing the start point.
        //:
        //:   1 On each iteration create an object and call the 'convert'
        //:     method passing a zero-length input sequence.
        //:
        //:   2 Verify that there is no output and that the object remains in
        //:     the initial state.  (C-3)
        //:
        //: 3 Create a sequence of hexadecimal characters containing all
        //:   possible pairs of these symbols.  Run the loop by changing the
        //:   length of the incoming data.
        //:
        //:   1 On each iteration create an object and call the 'convert'
        //:     method.
        //:
        //:   2 Using special function, 'decode', create the expected decoded
        //:     text.
        //:
        //:   3 Compare the output with expected sequence and verify that all
        //:     characters are converted correctly.
        //:
        //:   4 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   5 Verify the state of the object after conversion.  (C-1)
        //:
        //: 4 Using a predefined sequence of different characters run the loop
        //:   by changing the maximum output length.
        //:
        //:   1 On each iteration create an object and call the 'convert'
        //:     method.
        //:
        //:   2 Verify that the number of symbols written to the output buffer
        //:     does not exceed passed threshold.
        //:
        //:   3 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   4 Verify the state of the object after conversion.
        //:
        //: 5 Using a table-based approach verify that space symbols are
        //:   ignored by the 'HexDecoder'.  (C-4)
        //:
        //: 6 Create a sequence of valid hexadecimal characters.  Run the loop.
        //:
        //:   1 On each iteration add one invalid symbol to the sequence from
        //:     P-6, create an object and call the 'convert' method.
        //:
        //:   2 Verify that the function returns error value.
        //:
        //:   3 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   4 Verify that the object is in 'error' state.  (C-2)
        //:
        //: 7 Create an object and set it to different states, using the
        //:   'convert' method. Call the 'endConvert' method and verify that
        //:   the expected value is returned and object is set to the expected
        //:   state. (C-8)
        //:
        //: 8 Create an object and using a predefined sequence of the same
        //:   characters run the loop by changing the length of incoming data
        //:   and maximum output length.
        //:
        //:   1 On each iteration call the 'convert' method.
        //:
        //:   2 Compare the output with predefined expected sequence and verify
        //:     that all characters are converted correctly, that output length
        //:     is increased accordingly.
        //:
        //:   3 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   4 Verify the state of the object after conversion.  (C-5-6)
        //:
        //: 9 Create an object and set it to the 'error' state and to the
        //:   'done' state.  Call the 'convert' method and verify the returned
        //:   value and the state of the object afterwards.  (C-7)
        //:
        //:10 Create an object.  Create a couple of standard containers to use
        //:   them as the input and output buffers.  Call the 'convert' method
        //:   passing iterators to the containers as parameters.  Verify the
        //:   results.  (C-9)
        //:
        //:11 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-10)
        //
        // Testing:
        //   HexDecoder();
        //   int convert(OUTPUT_ITERATOR, INPUT_ITERATOR, INPUT_ITERATOR);
        //   int convert(OUTPUT_ITER, int *, int *, INPUT_ITER,INPUT_ITER,int);
        //   int endConvert();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        const char    XDIGIT_STRING[] = "0123456789ABCDEFabcdef";
        const size_t  XDIGIT_COUNT    = sizeof XDIGIT_STRING - 1;

        if (verbose) cout << "\tTesting constructor" << endl;
        {
            Obj        mX;
            const Obj& X = mX;

            ASSERT( X.isAcceptable());
            ASSERT(!X.isDone()      );
            ASSERT(!X.isError()     );

            ASSERTV(X.outputLength(), 0 == X.outputLength());
        }

        if (verbose) cout << "\tTesting empty input" << endl;
        {
            char buffer = 0;

            int numOut = 0;
            int numIn  = 0;

            for (size_t i = 0; i < XDIGIT_COUNT; ++i) {
                const size_t  SHIFT  = i;
                const char   *BEGIN  = XDIGIT_STRING + SHIFT;

                if (veryVerbose) { T_ T_ P(SHIFT) }

                Obj         mX;
                const Obj&  X = mX;

                ASSERT( X.isAcceptable());
                ASSERT(!X.isDone()      );
                ASSERT(!X.isError()     );

                ASSERT(0 == X.outputLength());

                int rc = mX.convert(&buffer, &numOut, &numIn, BEGIN, BEGIN);

                ASSERTV(rc, 0 == rc);

                ASSERT( X.isAcceptable());
                ASSERT(!X.isDone()      );
                ASSERT(!X.isError()     );

                ASSERTV(X.outputLength(), 0 == X.outputLength());
                ASSERTV(numOut          , 0 == numOut          );
                ASSERTV(numIn           , 0 == numIn           );
                ASSERTV(buffer          , 0 == buffer          );

                rc = mX.endConvert();
                ASSERTV(rc, 0 == rc);
            }
        }

        if (verbose) cout << "\tTesting correctness of conversion" << endl;
        {
            const size_t  HEX_LENGTH      = XDIGIT_COUNT * XDIGIT_COUNT * 2;
            char          hexToDecode[HEX_LENGTH];
            char         *shuttle         = hexToDecode;

            // Fill string to decode with all possible pair combinations.

            for (size_t i = 0; i < XDIGIT_COUNT; ++i) {
                for (size_t j = 0; j < XDIGIT_COUNT; ++j) {
                    *shuttle = XDIGIT_STRING[i];
                    ++shuttle;
                    *shuttle = XDIGIT_STRING[j];
                    ++shuttle;
                }
            }

            for (size_t i = HEX_LENGTH; i > 0; --i) {
                const size_t LENGTH                 = i;
                const size_t EXPECTED_OUTPUT_LENGTH = LENGTH / 2;
                const bool   ODD_ITERATION          = LENGTH % 2;
                const bool   ACCEPTABLE             = !ODD_ITERATION;
                const int    EXPECTED_END_RESULT    = ACCEPTABLE ? 0 : -1;

                char *buffer   = new char[EXPECTED_OUTPUT_LENGTH];
                char *expected = new char[EXPECTED_OUTPUT_LENGTH];

                bsl::memset(buffer,   0, EXPECTED_OUTPUT_LENGTH);
                bsl::memset(expected, 0, EXPECTED_OUTPUT_LENGTH);

                if (veryVerbose) { T_ T_ P(LENGTH) }

                Obj         mX;
                const Obj&  X = mX;

                ASSERT( X.isAcceptable());
                ASSERT(!X.isDone()      );
                ASSERT(!X.isError()     );

                ASSERT(0 == X.outputLength());

                int numOut = 0;
                int numIn  = 0;

                decode(expected,
                       hexToDecode,
                       ODD_ITERATION ? LENGTH - 1 : LENGTH);

                int rc = mX.convert(buffer,
                                    &numOut,
                                    &numIn,
                                    hexToDecode,
                                    hexToDecode + LENGTH);

                ASSERTV(rc, 0 == rc);

                ASSERT(!X.isDone()     );
                ASSERT(!X.isError()    );

                ASSERT(ACCEPTABLE             ==  X.isAcceptable()           );
                ASSERT(EXPECTED_OUTPUT_LENGTH ==
                                        static_cast<size_t>(X.outputLength()));
                ASSERT(EXPECTED_OUTPUT_LENGTH == static_cast<size_t>(numOut) );
                ASSERT(LENGTH                 == static_cast<size_t>(numIn)  );

                rc = mX.endConvert();
                ASSERTV(rc, EXPECTED_END_RESULT == rc);

                delete [] buffer;
                delete [] expected;
            }
        }

        if (verbose) cout << "\tTesting output limit" << endl;
        {
            const char   *HEX        = "404142434445464748494A4B4C4D4E4F";
            const char   *EXPECTED   = "@ABCDEFGHIJKLMNO";
            const size_t  HEX_LENGTH = strlen(HEX);
            const int     EXP_LENGTH = static_cast<int>(strlen(EXPECTED));

            char buffer[32];

            for (int i = -2; i <= EXP_LENGTH + 1; ++i) {
                const int  MAX_NUM_OUT      = i;
                const int  EXPECTED_NUM_OUT =
                    (MAX_NUM_OUT < 0 || MAX_NUM_OUT > EXP_LENGTH)
                        ? EXP_LENGTH
                        : MAX_NUM_OUT;
                const int  EXPECTED_NUM_IN  = EXPECTED_NUM_OUT * 2;

                int numOut = 0;
                int numIn  = 0;

                bsl::memset(buffer, 0, sizeof(buffer));

                if (veryVerbose) { T_ T_ P(MAX_NUM_OUT) }

                Obj         mX;
                const Obj&  X = mX;

                ASSERTV( X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV(!X.isError()     );

                int rc = mX.convert(buffer,
                                    &numOut,
                                    &numIn,
                                    HEX,
                                    HEX + HEX_LENGTH,
                                    MAX_NUM_OUT);



                ASSERTV( X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV(!X.isError()     );

                ASSERTV(rc,     0                == rc                       );
                ASSERTV(numOut, EXPECTED_NUM_OUT == numOut                   );
                ASSERTV(numIn,  EXPECTED_NUM_IN  == numIn                    );
                ASSERTV(        0                == bsl::strncmp(
                                                            EXPECTED,
                                                            buffer,
                                                            EXPECTED_NUM_OUT));
                ASSERTV(X.outputLength(),
                        EXPECTED_NUM_OUT == X.outputLength());

                rc = mX.endConvert();
                ASSERTV(rc, 0 == rc);
            }
        }

        if (verbose) cout << "\tTesting space symbol ignoring" << endl;
        {
            static const struct {
                int         d_line;            // source line number

                const char *d_hex_p;           // hex to decode

                const char *d_expected_p;      // expected output

                bool        d_acceptableFlag;  // is object in acceptable state
                                               // after conversion
            } DATA[] = {
                //LINE   HEX               EXPECTED  ACCEPTABLE
                //----  -----------------  --------  ----------
                { L_,   " ",               "",       true      },
                { L_,   "  ",              "",       true      },
                { L_,   "   ",             "",       true      },
                { L_,   " a",              "",       false     },
                { L_,   "a ",              "",       false     },
                { L_,   " 41",             "A",      true      },
                { L_,   "4 1",             "A",      true      },
                { L_,   "41 ",             "A",      true      },
                { L_,   " 4 1 ",           "A",      true      },
                { L_,   " 41a",            "A",      false     },
                { L_,   "4 1 a",           "A",      false     },
                { L_,   "41a ",            "A",      false     },
                { L_,   " 4 1 a ",         "A",      false     },
                { L_,   "  4  1  4  2  ",  "AB",     true      },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int   LINE                = DATA[i].d_line;
                const char *HEX                 = DATA[i].d_hex_p;
                const char *EXP                 = DATA[i].d_expected_p;
                bool        ACCEPTABLE          = DATA[i].d_acceptableFlag;
                const int   EXPECTED_NUM_IN     =
                                                 static_cast<int>(strlen(HEX));
                const int   EXPECTED_NUM_OUT    =
                                                 static_cast<int>(strlen(EXP));
                const int   EXPECTED_END_RESULT = ACCEPTABLE ? 0 : -1;

                char        buffer[16];
                int         numIn  = 0;
                int         numOut = 0;

                bsl::memset(buffer, 0, sizeof(buffer));

                if (veryVerbose) { T_ T_ P(HEX) }

                Obj         mX;
                const Obj&  X = mX;

                ASSERTV(LINE,  X.isAcceptable());
                ASSERTV(LINE, !X.isDone()      );
                ASSERTV(LINE, !X.isError()     );

                int rc = mX.convert(buffer,
                                    &numOut,
                                    &numIn,
                                    HEX,
                                    HEX + EXPECTED_NUM_IN);


                ASSERTV(LINE, !X.isDone() );
                ASSERTV(LINE, !X.isError());

                ASSERTV(LINE, rc,     0                == rc                 );
                ASSERTV(LINE,         ACCEPTABLE       == X.isAcceptable()   );
                ASSERTV(LINE, numOut, EXPECTED_NUM_OUT == numOut             );
                ASSERTV(LINE, numIn,  EXPECTED_NUM_IN  == numIn              );
                ASSERTV(LINE,         0                == bsl::strncmp(
                                                            EXP,
                                                            buffer,
                                                            EXPECTED_NUM_OUT));
                ASSERTV(X.outputLength(),
                        EXPECTED_NUM_OUT == X.outputLength());

                rc = mX.endConvert();
                ASSERTV(rc, EXPECTED_END_RESULT == rc);
            }
        }

        if (verbose) cout << "\tTesting inappropriate symbols conversion"
                          << endl;
        {
            const char HEX_LENGTH = 8;
            char       buffer[4];

            for (char i = 0; i < HEX_LENGTH; ++i) {
                const char INDEX            = i;
                const int  EXPECTED_NUM_OUT = INDEX / 2;
                int        numIn            = 0;
                int        numOut           = 0;

                bsl::memset(buffer, 0, sizeof(buffer));

                char hexToDecode[] = "44444444";

                // Add invalid symbol
                hexToDecode[INDEX] = INDEX;

                if (veryVerbose) { T_ T_ P(INDEX) }

                Obj        mX;
                const Obj& X = mX;

                ASSERTV( X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV(!X.isError()     );

                int rc = mX.convert(buffer,
                                    &numOut,
                                    &numIn,
                                    hexToDecode,
                                    hexToDecode + HEX_LENGTH);

                ASSERTV((int)INDEX, !X.isDone()      );
                ASSERTV((int)INDEX,  X.isError()     );
                ASSERTV((int)INDEX, !X.isAcceptable());

                ASSERTV((int)INDEX, rc,     -1               == rc    );
                ASSERTV((int)INDEX, numOut, EXPECTED_NUM_OUT == numOut);
                ASSERTV((int)INDEX, numIn,  INDEX + 1        == numIn );
                ASSERTV((int)INDEX, X.outputLength(),
                        EXPECTED_NUM_OUT == X.outputLength());

                for (int j = 0; j < EXPECTED_NUM_OUT; ++j) {
                    ASSERTV((int)INDEX, j, buffer[j], 'D' == buffer[j]);
                }
            }
        }

        if (verbose) cout << "\tTesting 'endConvert'" << endl;
        {

            char hexToDecode[] = "44";
            char invalidHex    = 0;
            char buffer        = 0;
            int  numOut        = 0;
            int  numIn         = 0;

            // initial and done states
            {
                Obj        mX;
                const Obj& X = mX;

                ASSERTV( X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV(!X.isError()     );

                int rc = mX.endConvert();

                ASSERTV(rc, 0 == rc      );
                ASSERTV(!X.isAcceptable());
                ASSERTV( X.isDone()      );
                ASSERTV(!X.isError()     );


                rc = mX.endConvert();

                ASSERTV(rc, 0 == rc      );
                ASSERTV(!X.isAcceptable());
                ASSERTV( X.isDone()      );
                ASSERTV(!X.isError()     );
            }

            // acceptable state
            {
                Obj        mX;
                const Obj& X = mX;

                mX.convert(&buffer,
                           &numOut,
                           &numIn,
                           hexToDecode,
                           hexToDecode + 2);

                ASSERTV( X.isAcceptable());
                ASSERTV(!X.isDone()     );
                ASSERTV(!X.isError()    );

                int rc = mX.endConvert();

                ASSERTV(rc, 0 == rc      );
                ASSERTV(!X.isAcceptable());
                ASSERTV( X.isDone()      );
                ASSERTV(!X.isError()     );
            }

            // intermediate state (no errors, inner buffer contains unhandled
            // symbol)
            {
                Obj        mX;
                const Obj& X = mX;

                int rc = mX.convert(&buffer,
                                    &numOut,
                                    &numIn,
                                    hexToDecode,
                                    hexToDecode + 1);
                ASSERTV(rc, 0 == rc);

                ASSERTV(!X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV(!X.isError()     );

                rc = mX.endConvert();

                ASSERTV(rc, -1 == rc     );
                ASSERTV(!X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV( X.isError()     );
            }

            // error state
            {
                Obj        mX;
                const Obj& X = mX;

                int rc = mX.convert(&buffer,
                                    &numOut,
                                    &numIn,
                                    &invalidHex,
                                    &invalidHex + 1);
                ASSERTV(rc, -1 == rc);

                ASSERTV(!X.isAcceptable());
                ASSERTV(!X.isDone()     );
                ASSERTV( X.isError()    );

                rc = mX.endConvert();

                ASSERTV(rc, -1 == rc     );
                ASSERTV(!X.isAcceptable());
                ASSERTV(!X.isDone()      );
                ASSERTV( X.isError()     );
            }
        }

        if (verbose) cout << "\tTesting sequential conversions" << endl;
        {
            int   HEX_LENGTH  = 8;
            char *hexToDecode = new char[HEX_LENGTH];
            char *buffer      = new char[HEX_LENGTH];

            int numOut = 0;
            int numIn  = 0;

            for (int i = 0; i < HEX_LENGTH; ++i) {
                hexToDecode[i] = '4';
            }

            for (int i = -1; i < HEX_LENGTH; ++i) {
                const int   MAX_NUM_OUT          = i;
                const char *HEX                  = hexToDecode;
                bool        acceptable           = true;
                int         expectedOutputLength = 0;

                Obj         mX;
                const Obj&  X = mX;

                for (int j = 0; j < HEX_LENGTH; ++j) {
                    const int INPUT_LENGTH        = j;
                    const int NUM_PENDING_SYMBOLS = acceptable ? 0 : 1;
                    const int EXPECTED_NUM_IN     =
                        MAX_NUM_OUT < 0
                            ? INPUT_LENGTH
                            : MAX_NUM_OUT >
                                     ((INPUT_LENGTH + NUM_PENDING_SYMBOLS) / 2)
                                  ? INPUT_LENGTH
                                  : MAX_NUM_OUT * 2 - NUM_PENDING_SYMBOLS;
                    const int EXPECTED_NUM_OUT    =
                                   (EXPECTED_NUM_IN + NUM_PENDING_SYMBOLS) / 2;
                    expectedOutputLength += EXPECTED_NUM_OUT;

                    numOut = 0;
                    numIn  = 0;
                    bsl::memset(buffer, 0, HEX_LENGTH);

                    acceptable = EXPECTED_NUM_OUT * 2 ==
                                 EXPECTED_NUM_IN + NUM_PENDING_SYMBOLS;

                    if (veryVerbose) { T_ T_ P_(MAX_NUM_OUT) P(INPUT_LENGTH) }

                    int rc = mX.convert(buffer,
                                        &numOut,
                                        &numIn,
                                        HEX,
                                        HEX + INPUT_LENGTH,
                                        MAX_NUM_OUT);

                    ASSERTV(!X.isDone() );
                    ASSERTV(!X.isError());

                    ASSERTV(        acceptable       == X.isAcceptable());
                    ASSERTV(rc,     0                == rc              );
                    ASSERTV(numOut, EXPECTED_NUM_OUT == numOut          );
                    ASSERTV(numIn,  EXPECTED_NUM_IN  == numIn           );
                    ASSERTV(expectedOutputLength, X.outputLength(),
                            expectedOutputLength == X.outputLength()    );

                    for (int j = 0; j < EXPECTED_NUM_OUT; ++j) {
                        ASSERTV(buffer[j], 'D' == buffer[j]);
                    }
                }
            }

            // Testing conversion in error state
            {
                char invalidHex = 0;

                Obj         mX;
                const Obj&  X = mX;

                int rc = mX.convert(buffer,
                                    &numOut,
                                    &numIn,
                                    &invalidHex,
                                    &invalidHex + 1);
                ASSERTV(rc, -1 == rc);

                ASSERTV(!X.isDone()      );
                ASSERTV( X.isError()     );
                ASSERTV(!X.isAcceptable());

                numOut          = 0;
                numIn           = 0;
                bsl::memset(buffer, 0, HEX_LENGTH);

                rc = mX.convert(buffer,
                                &numOut,
                                &numIn,
                                &invalidHex,
                                &invalidHex + 1);

                ASSERTV(!X.isDone()      );
                ASSERTV(X.isError()      );
                ASSERTV(!X.isAcceptable());

                ASSERTV(rc,               -1 == rc              );
                ASSERTV(numOut,            0 == numOut          );
                ASSERTV(numIn,             0 == numIn           );
                ASSERTV(X.outputLength(),  0 == X.outputLength());
            }

            // Testing conversion in done state
            {
                Obj         mX;
                const Obj&  X = mX;

                int rc = mX.endConvert();
                ASSERTV(rc, 0 == rc);

                ASSERTV( X.isDone()      );
                ASSERTV(!X.isError()     );
                ASSERTV(!X.isAcceptable());

                numOut          = 0;
                numIn           = 0;
                bsl::memset(buffer, 0, HEX_LENGTH);

                rc = mX.convert(buffer,
                                &numOut,
                                &numIn,
                                hexToDecode,
                                hexToDecode + 1);

                ASSERTV(!X.isDone()      );
                ASSERTV(X.isError()      );
                ASSERTV(!X.isAcceptable());

                ASSERTV(rc,               -2 == rc              );
                ASSERTV(numOut,            0 == numOut          );
                ASSERTV(numIn,             0 == numIn           );
                ASSERTV(X.outputLength(),  0 == X.outputLength());
            }

            delete [] hexToDecode;
            delete [] buffer;
        }

        if (verbose) cout << "\tTesting truncated overload" << endl;
        {
            // Since the overload with 3 parameters just calls the overload
            // with 5 parameters, we can use the last one (fully tested by now)
            // as a model for comparison.  We want to make sure that incoming
            // parameters are correctly passed to the overload with 5
            // parameters and the return value is correctly passed back.

            const char *HEX        = "404142434445464748494A4B4C#4D4E4F";
            const int   HEX_LENGTH = static_cast<int>(strlen(HEX));

            char buffer[32];
            char modelBuffer[32];

            for (int i = 0; i <= HEX_LENGTH; ++i) {
                const int   LENGTH = i;
                const char *BEGIN  = HEX;
                const char *END    = BEGIN + LENGTH;

                int numOut = 0;
                int numIn  = 0;

                bsl::memset(buffer,      0, sizeof(buffer     ));
                bsl::memset(modelBuffer, 0, sizeof(modelBuffer));

                if (veryVerbose) { T_ T_ P(LENGTH) }

                Obj         mX;
                const Obj&  X = mX;
                Obj         mXModel;
                const Obj&  XModel = mXModel;

                const int EXP_RESULT = mXModel.convert(modelBuffer,
                                                       &numOut,
                                                       &numIn,
                                                       BEGIN,
                                                       END);

                int       rc = mX.convert(buffer, BEGIN, END);

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(EXP_RESULT, rc, EXP_RESULT  == rc);

                ASSERTV(0 == bsl::strncmp(modelBuffer, buffer, numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());
            }
        }

        if (verbose) cout << "\tTesting compatibility with standard iterators"
                          << endl;
        {
            const char *HEX        = "404142434445464748494A4B4C#4D4E4F";
            const int   HEX_LENGTH = static_cast<int>(strlen(HEX));

            bsl::list<char>   input;
            bsl::vector<char> output(32);

            char modelBuffer[32];

            for (int i = 0; i <= HEX_LENGTH; ++i) {
                const int   LENGTH = i;
                const char *BEGIN  = HEX;
                const char *END    = BEGIN + LENGTH;

                input.insert(input.end(), BEGIN, END);
                ASSERTV(LENGTH == static_cast<int>(input.size()));

                int numOutModel = 0;
                int numInModel  = 0;
                int numOut      = 0;
                int numIn       = 0;


                bsl::memset(modelBuffer, 0, sizeof(modelBuffer));

                if (veryVerbose) { T_ T_ P(LENGTH) }

                Obj         mX;
                const Obj&  X = mX;
                Obj         mXModel;
                const Obj&  XModel = mXModel;

                const int EXP_RESULT = mXModel.convert(modelBuffer,
                                                       &numOutModel,
                                                       &numInModel,
                                                       BEGIN,
                                                       END);

                int       rc = mX.convert(output.begin(),
                                          &numOut,
                                          &numIn,
                                          input.begin(),
                                          input.end());

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(EXP_RESULT,  rc,     EXP_RESULT   == rc    );
                ASSERTV(numOutModel, numOut, numOutModel  == numOut);
                ASSERTV(numInModel,  numIn,  numInModel   == numIn );

                ASSERTV(0 == bsl::strncmp(modelBuffer, output.data(), numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());

                input.clear();
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            char  buffer = 0;
            int   num    = 0;
            char *begin  = &buffer;
            char *end    = &buffer + 1;
            int  *nullI  = 0;

            ASSERT_SAFE_FAIL(mX.convert(&buffer, nullI, &num,  begin, end  ));
            ASSERT_SAFE_FAIL(mX.convert(&buffer, &num,  nullI, begin, end  ));
            ASSERT_SAFE_PASS(mX.convert(&buffer, &num,  &num,  begin, end  ));
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
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const char *HEX              = "404142434445464748494A4B4C4D4E4F";
        const char *EXPECTED         = "@ABCDEFGHIJKLMNO";
        const int   EXPECTED_NUM_IN  = 32;
        const int   EXPECTED_NUM_OUT = 16;
        char        buffer[EXPECTED_NUM_OUT + 1];
        int         numIn;
        int         numOut;

        bsl::memset(buffer, 0, sizeof(buffer));

        Obj         mX;
        const Obj&  X = mX;

        ASSERT( X.isInitialState());
        ASSERT( X.isAcceptable()  );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        int rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            HEX,
                            HEX + EXPECTED_NUM_IN);

        ASSERTV(rc,     0                == rc                            );
        ASSERTV(numOut, EXPECTED_NUM_OUT == numOut                        );
        ASSERTV(numIn,  EXPECTED_NUM_IN  == numIn                         );
        ASSERTV(buffer, 0                == bsl::strncmp(EXPECTED,
                                                         buffer,
                                                         EXPECTED_NUM_OUT));

        ASSERTV(X.outputLength(), EXPECTED_NUM_OUT == X.outputLength());

        ASSERT(!X.isInitialState());
        ASSERT( X.isAcceptable()   );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        rc = mX.endConvert();

        ASSERTV(rc, 0 == rc);
        ASSERT(!X.isInitialState());
        ASSERT(!X.isAcceptable()  );
        ASSERT( X.isDone()        );
        ASSERT(!X.isError()       );

        mX.reset();

        ASSERT( X.isInitialState());
        ASSERT( X.isAcceptable()  );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );
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
// Copyright 2022 Bloomberg Finance L.P.
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
