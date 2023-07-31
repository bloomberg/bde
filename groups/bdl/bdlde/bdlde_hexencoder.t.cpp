// bdlde_hexencoder.t.cpp                                             -*-C++-*-

#include <bdlde_hexencoder.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cstring.h>  // 'bsl::strncmp'
#include <bsl_list.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements an encoder for converting characters
// into their hexadecimal representation.
//
// Testing of 'bdlde::HexEncoder' consists of verifying that characters are
// converted correctly and the object itself takes the expected state as a
// result of method calls.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] HexEncoder();
//
// MANIPULATORS
// [ 2] int convert(OUTPUT_ITERATOR, INPUT_ITERATOR, INPUT_ITERATOR);
// [ 2] int convert(OUTPUT_ITER, int *, int *, INPUT_ITER,INPUT_ITER,int);
// [ 2] int endConvert(OUTPUT_ITERATOR out);
// [ 2] int endConvert(OUTPUT_ITERATOR out, int *numOut, int maxNumOut);
// [ 5] void reset();
//
// ACCESSORS
// [ 3] bool isUpperCase() const;
// [ 3] bool isDone() const;
// [ 3] bool isError() const;
// [ 3] bool isInitialState() const;
// [ 3] int outputLength() const;
// [ 3] int numOutputPending() const;
// [ 4] bool isAcceptable() const;
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

typedef bdlde::HexEncoder Obj;

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

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of 'bdlde::HexEncoder'
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example shows using a 'bdlde::HexEncoder' object to encode
// bytes into a hexidecimal format. For dependency reasons, a more complete
// example, showing both encoding and decoding can be found in
// 'bdlde_hexdecoder'.
//
// In the example below, we implement a function 'streamEncoder', that reads
// text from 'bsl::istream', encodes that text into hex representation, and
// writes the encoded text to a 'bsl::ostream'.  'streamEncoder' returns 0 on
// success and a negative value if the input data could not be successfully
// encoded or if there is an I/O  error.
//..
    int streamEncoder(bsl::ostream& os, bsl::istream& is)
        // Read the entire contents of the specified input stream 'is', convert
        // the input plain text to hex representation, and write the encoded
        // text to the specified output stream 'os'.  Return 0 on success, and
        // a negative value otherwise.
    {
        enum {
            SUCCESS      =  0,
            ENCODE_ERROR = -1,
            IO_ERROR     = -2
        };
//..
// First we create an object, create buffers for storing data, and start loop
// that runs while the input stream contains some data:
//..
        bdlde::HexEncoder converter;

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

                int numOut;
                int numIn;
//..
// Convert obtained text using 'bdlde::HexEncoder':
//..
                int status = converter.convert(
                                         output,
                                         &numOut,
                                         &numIn,
                                         input,
                                         inputEnd,
                                         static_cast<int>(outputEnd - output));
                if (status < 0) {
                    return ENCODE_ERROR;                              // RETURN
                }

                output += numOut;
                input  += numIn;
//..
// And write encoded text to the output stream:
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

        while (1) {
            int numOut = 0;
//..
// Then, we need to store the unhandled symbol (if there is one) to the output
// buffer and complete the work of our encoder:
//..
            int more = converter.endConvert(
                                        output,
                                        &numOut,
                                        static_cast<int>(outputEnd - output));
            if (more < 0) {
                return ENCODE_ERROR;                                  // RETURN
            }

            output += numOut;

            if (!more) { // no more output
                break;
            }

            ASSERT(output == outputEnd);  // output buffer is full

            os.write(outputBuffer, sizeof outputBuffer);  // write buffer
            if (os.fail()) {
                return IO_ERROR;                                      // RETURN
            }
            output = outputBuffer;
        }

        if (output > outputBuffer) {
            os.write(outputBuffer, output - outputBuffer);
        }

        return is.eof() && os.good() ? SUCCESS : IO_ERROR;
    }
//..

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
        //   Note that 'streamDecoder' implementation has been removed from
        //   this test driver to avoid cycle dependency between encoder and
        //   decoder components.  So the use of the 'streamDecoder' has been
        //   commented in this usage example.  The full test that checks the
        //   'encoder - decoder' round trip is still run in the
        //   'bdlde_hexdecoder' test driver.  The 'streamDecoder'
        //   implementation can be seen there.
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
// data to encode.  Assume that we have some character buffer,
// 'BLOOMBERG_NEWS', and a function, 'streamDecoder' mirroring the work of the
// 'streamEncoder'.  Below we should encode this string into a hexidecimal
// format:
//..
    bsl::istringstream inStream(bsl::string(BLOOMBERG_NEWS,
                                            strlen(BLOOMBERG_NEWS)));
    bsl::stringstream  outStream;
    bsl::stringstream  backInStream;
//..
// Then, we use our function to encode text:
//..
    ASSERT(0 == streamEncoder(outStream, inStream));
//..
// This example does *not* decode the resulting hexidecimal text, for a
// more complete example, see 'bdlde_hexdecoder'.
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
        //:   call the 'reset' method and verify object's status.  (C-1)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset'" << endl
                          << "===============" << endl;

        char SOURCE = 'D';
        char buffer[2];
        int  numIn  = 0;
        int  numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Testing inner buffer resetting.

        int rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE + 1, 1);
        ASSERTV(rc, 1 == rc);

        ASSERTV(1 == X.numOutputPending());

        mX.reset();

        ASSERTV(0 == X.numOutputPending());

        // Testing output length resetting.

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE + 1);
        ASSERTV(rc, 0 == rc);

        ASSERTV(X.isAcceptable());
        ASSERTV(2 == X.outputLength());
        ASSERTV(0 == X.numOutputPending());

        mX.reset();

        ASSERTV(0 == X.outputLength());

        // Testing state resetting.

        rc = mX.endConvert(buffer, &numOut);
        ASSERTV(rc, 0 == rc);

        ASSERTV(X.isDone());

        mX.reset();

        ASSERTV(X.isInitialState());

        // Various parameters resetting.

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE + 1, 1);
        ASSERTV(rc,  1 == rc);
        rc = mX.endConvert(buffer, &numOut, 0);
        ASSERTV(rc,  1 == rc);
        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE);
        ASSERTV(rc, -2 == rc);

        ASSERTV(X.isError());
        ASSERTV(1 == X.outputLength());
        ASSERTV(1 == X.numOutputPending());

        mX.reset();

        ASSERTV(X.isInitialState());
        ASSERTV(0 == X.outputLength());
        ASSERTV(0 == X.numOutputPending());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'isAcceptable'
        //
        // Concerns:
        //: 1 The 'isAcceptable' method  takes into account not only the state
        //:   of the object but also the presence of a pending character in the
        //:   internal buffer.
        //:
        //: 2 The accessor is 'const'.
        //
        // Plan:
        //: 1 Create an object and set it to different states.  In each state
        //:   invoke the function under test from a reference providing
        //:   non-modifiable access to the object and verify the returned
        //:   value.  (C-1,2)
        //
        // Testing:
        //   bool isAcceptable() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isAcceptable'" << endl
                          << "======================" << endl;

        char SOURCE = 'D';
        char buffer[2];
        int  numIn  = 0;
        int  numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Initial state
        ASSERT(X.isAcceptable());

        int rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE + 1, 1);
        ASSERTV(rc, 1 == rc);

        // intermediate state (no errors, inner buffer contains pending
        // symbol)

        ASSERT(1 == X.numOutputPending());
        ASSERT(!X.isAcceptable());

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE);
        ASSERTV(rc, 0 == rc);

        // acceptable state (no errors, no unhandled symbols)
        ASSERT(0 == X.numOutputPending());
        ASSERT(X.isAcceptable());

        rc = mX.endConvert(buffer, &numOut);
        ASSERTV(rc, 0 == rc);

        // done state
        ASSERT(0 == X.numOutputPending());
        ASSERT( X.isDone()     );
        ASSERT(!X.isAcceptable());

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE);
        ASSERTV(rc, -2 == rc);

        // error state
        ASSERT(0 == X.numOutputPending());
        ASSERT( X.isError()    );
        ASSERT(!X.isAcceptable());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Basic accessors were used in the test for primary manipulators.
        //   In order not to duplicate the code, we will conduct surface tests
        //   here, bringing the object into the required state and checking it.
        //
        // Concerns:
        //: 1 'isInitialState' takes into account not only the state of the
        //:   object but also the output length.
        //:
        //: 2 All accessors are 'const'.
        //
        // Plan:
        //: 1 Create an object and set it to different states.  In each state
        //:   invoke the functions under test from a reference providing
        //:   non-modifiable access to the object and verify the returned
        //:   value.  (C-1,2)
        //
        // Testing:
        //   bool isUpperCase() const;
        //   bool isDone() const;
        //   bool isError() const;
        //   bool isInitialState() const;
        //   int outputLength() const;
        //   int numOutputPending() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        const char SOURCE = 'D';
        char       buffer[2];
        int        numIn  = 0;
        int        numOut = 0;

        Obj        mX;
        const Obj& X = mX;

        // Initial state
        ASSERT( X.isUpperCase()   );
        ASSERT( X.isInitialState());
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        ASSERTV(X.outputLength(),     0 == X.outputLength()    );
        ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

        int rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE + 1, 1);
        ASSERTV(rc, 1 == rc);

        // intermediate state (no errors, inner buffer contains pending
        // symbol)
        ASSERT(!X.isInitialState());
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        ASSERTV(X.outputLength(),     1 == X.outputLength()    );
        ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE);
        ASSERTV(rc, 0 == rc);

        // acceptable state (no errors, no unhandled symbols)
        ASSERT(!X.isInitialState());
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        ASSERTV(X.outputLength(),     2 == X.outputLength());
        ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

        rc = mX.endConvert(buffer, &numOut);
        ASSERTV(rc, 0 == rc);

        // done state
        ASSERT(!X.isInitialState());
        ASSERT( X.isDone()        );
        ASSERT(!X.isError()       );

        ASSERTV(X.outputLength(),     2 == X.outputLength());
        ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

        rc = mX.convert(buffer, &numOut, &numIn, &SOURCE, &SOURCE);
        ASSERTV(rc, -2 == rc);

        // error state
        ASSERT(!X.isInitialState());
        ASSERT(!X.isDone()        );
        ASSERT( X.isError()       );

        ASSERTV(X.outputLength(),     2 == X.outputLength());
        ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   To bring an object into any possible state we have to use three
        //   methods: constructor to create an object and put it to the initial
        //   state, 'convert' to modify the output length and put the object to
        //   the 'error' state and 'endConvert' to put object to the 'done'
        //   state.  In order to completely test the 'convert' function we need
        //   to call it from the 'error' and 'done' states, which in turn
        //   requires a fully tested 'endConvert' function.  To resolve this
        //   conflict we test single calls of the 'convert' method first, then
        //   we test the 'endConvert' method and finally we test sequential
        //   calls of the 'convert' function.
        //
        // Concerns:
        //: 1 Either uppercase or lowercase characters are used depending on
        //:   the parameter passed on the object's construction.
        //:
        //: 2 All characters are correctly converted to their hexadecimal
        //:   representations.
        //:
        //: 3 Any pending character is taken into account when checking if the
        //:   'maxNumOut' threshold is exceeded.
        //:
        //: 4 Any pending character is stored in the output buffer before
        //:   processing any new input symbol.
        //:
        //: 5 The output length value considers the results of all successive
        //:   'convert' calls, not just the last one.
        //:
        //: 6 The 'convert' method returns different values for different
        //:   errors.
        //:
        //: 7 The 'endConvert' method writes any pending character to the
        //:   output buffer and increases output length counter.
        //:
        //: 8 The 'endConvert' method returns the number of pending characters.
        //:
        //: 9 The manipulators correctly handle "real-life" iterators.
        //:
        //:10 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object and verify that it is in the expected state.
        //:
        //: 2 Using a predefined sequence of different characters run the loop
        //:   by changing the start point and the length of incoming data,
        //:   the maximum output length and the letter case.
        //:
        //:   1 On each iteration create an object and call the 'convert'
        //:     method.
        //:
        //:   2 Compare output with predefined expected sequence and verify
        //:     that all characters are converted correctly.  (C-1..2)
        //:
        //:   3 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   4 Verify the state of the object after conversion.
        //:
        //: 3 Create an object and set it to different states, using the
        //:   'convert' method. Call the 'endConvert' method and verify that
        //:   the expected value is returned and the object is set to the
        //:   expected state.  (C-7..8)
        //:
        //: 4 Create an object and using predefined sequence of the same
        //:   characters run the loop by changing the length of incoming data
        //:   and the maximum output length.
        //:
        //:   1 On each iteration call the 'convert' method.
        //:
        //:   2 Compare the output with predefined expected sequence and verify
        //:     that all characters are converted correctly, that the output
        //:     length is increased accordingly.  (C-3..5)
        //:
        //:   3 Verify that the number of processed symbols and the number
        //:     of symbols written to the output buffer are stored in the
        //:     corresponding variables.
        //:
        //:   4 Verify the state of the object after conversion.
        //:
        //: 5 Create an object, set it to the 'error' state and to the 'done'
        //:   state.  Call the 'convert' method and verify the returned
        //:   value and the state of the object afterwards.  (C-6)
        //:
        //: 6 Create an object.  Create a couple of standard containers to use
        //:   them as the input and output buffers.  Call the manipulators
        //:   passing iterators to the containers as parameters.  Verify the
        //:   results.  (C-9)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-10)
        //
        // Testing:
        //   HexEncoder();
        //   int convert(OUTPUT_ITERATOR, INPUT_ITERATOR, INPUT_ITERATOR);
        //   int convert(OUTPUT_ITER, int *, int *, INPUT_ITER,INPUT_ITER,int);
        //   int endConvert(OUTPUT_ITERATOR out);
        //   int endConvert(OUTPUT_ITERATOR out, int *numOut, int maxNumOut);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        const char SOURCE[]        = "0123456789 Test string .,!@#$%^";
        const int  SOURCE_LENGTH   = static_cast<int>(sizeof SOURCE - 1);
        const char EXPECTED_UPP[]  = "30313233343536373839205465737420737472"
                                     "696E67202E2C21402324255E";
        const char EXPECTED_LOW[]  = "30313233343536373839205465737420737472"
                                     "696e67202e2c21402324255e";
        const int  MAX_OUTPUT_SIZE = SOURCE_LENGTH * 2;

        if (verbose) cout << "\tTesting constructor" << endl;
        {
            Obj        mX1;
            const Obj& X1 = mX1;

            ASSERT( X1.isUpperCase()   );
            ASSERT( X1.isInitialState());
            ASSERT(!X1.isDone()        );
            ASSERT(!X1.isError()       );

            ASSERTV(X1.outputLength(),     0 == X1.outputLength()    );
            ASSERTV(X1.numOutputPending(), 0 == X1.numOutputPending());

            Obj        mX2(true);
            const Obj& X2 = mX2;

            ASSERT( X2.isUpperCase()   );
            ASSERT( X2.isInitialState());
            ASSERT(!X2.isDone()        );
            ASSERT(!X2.isError()       );

            ASSERTV(X2.outputLength(),     0 == X2.outputLength()    );
            ASSERTV(X2.numOutputPending(), 0 == X2.numOutputPending());

            Obj        mX3(false);
            const Obj& X3 = mX3;

            ASSERT(!X3.isUpperCase()   );
            ASSERT( X3.isInitialState());
            ASSERT(!X3.isDone()        );
            ASSERT(!X3.isError()       );

            ASSERTV(X3.outputLength(),     0 == X3.outputLength()    );
            ASSERTV(X3.numOutputPending(), 0 == X3.numOutputPending());
        }

        if (verbose) cout << "\tTesting correctness of conversion" << endl;
        {
            char *buffer = new char[MAX_OUTPUT_SIZE];
            int   numOut = 0;
            int   numIn  = 0;

            for (int i = 0; i < SOURCE_LENGTH; ++i) {
                const int   SHIFT = i;
                const char *BEGIN = SOURCE + SHIFT;

                for (int j = 0; j < SOURCE_LENGTH - i; ++j) {
                    const int   LENGTH = j;
                    const char *END = BEGIN + LENGTH;

                    for (int k = -1; k < MAX_OUTPUT_SIZE + 2; ++k) {
                        const int  MAX_NUM_OUT = k;
                        const int  EXPECTED_NUM_OUT =
                            MAX_NUM_OUT < 0 || MAX_NUM_OUT > LENGTH * 2
                                ? LENGTH * 2
                                : MAX_NUM_OUT;
                        const int  EXPECTED_NUM_PENDING = EXPECTED_NUM_OUT % 2;
                        const int  EXPECTED_NUM_IN =
                            (EXPECTED_NUM_OUT + EXPECTED_NUM_PENDING) / 2;
                        const bool INITIAL = EXPECTED_NUM_OUT ? false : true;

                        for (int l = 0; l < 2; ++l) {
                            const bool  UPPERCASE = l;
                            const char *EXPECTED =
                                UPPERCASE ? EXPECTED_UPP + SHIFT * 2
                                          : EXPECTED_LOW + SHIFT * 2;

                            bsl::memset(buffer, 0, MAX_OUTPUT_SIZE);
                            numOut = 0;
                            numIn  = 0;

                            if (veryVerbose) {
                                T_ T_ P_(SHIFT) P_(LENGTH)
                                      P_(MAX_NUM_OUT) P(UPPERCASE)
                            }

                            Obj         mX(UPPERCASE);
                            const Obj&  X = mX;

                            ASSERT( X.isInitialState());
                            ASSERT(!X.isDone()        );
                            ASSERT(!X.isError()       );

                            ASSERTV(X.outputLength(), 0 == X.outputLength());
                            ASSERTV(X.numOutputPending(),
                                    0 == X.numOutputPending());

                            int rc = mX.convert(buffer,
                                                &numOut,
                                                &numIn,
                                                BEGIN,
                                                END,
                                                MAX_NUM_OUT);

                            ASSERT(!X.isDone() );
                            ASSERT(!X.isError());

                            ASSERTV(rc, EXPECTED_NUM_PENDING == rc);
                            ASSERTV(numOut, EXPECTED_NUM_OUT == numOut);
                            ASSERTV(numIn,  EXPECTED_NUM_IN  == numIn );
                            ASSERTV(INITIAL == X.isInitialState());
                            ASSERTV(X.outputLength(),
                                    EXPECTED_NUM_OUT == X.outputLength());
                            ASSERTV(X.numOutputPending(),
                                    EXPECTED_NUM_PENDING ==
                                        X.numOutputPending());

                            ASSERTV(EXPECTED,
                                    buffer,
                                    0 == strncmp(EXPECTED,
                                                 buffer,
                                                 EXPECTED_NUM_OUT));
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tTesting 'endConvert'" << endl;
        {

            char source = 'A';
            char buffer = 0;
            int  numOut = 0;
            int  numIn  = 0;

            // initial and done states
            {
                Obj        mX;
                const Obj& X = mX;

                ASSERT( X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(X.outputLength(),     0 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

                int rc = mX.endConvert(&buffer, &numOut, 1);

                ASSERT(!X.isInitialState());
                ASSERT( X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(rc,                   0 == rc                  );
                ASSERTV(numOut,               0 == numOut              );
                ASSERTV(X.outputLength(),     0 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());


                rc = mX.endConvert(&buffer, &numOut, 1);

                ASSERT(!X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT( X.isError()       );

                ASSERTV(rc,                   -1 == rc                  );
                ASSERTV(numOut,                0 == numOut              );
                ASSERTV(X.outputLength(),      0 == X.outputLength()    );
                ASSERTV(X.numOutputPending(),  0 == X.numOutputPending());
            }

            // acceptable state
            {
                Obj        mX;
                const Obj& X = mX;

                int rc = mX.convert(&buffer,
                                    &numOut,
                                    &numIn,
                                    &source,
                                    &source + 1);
                ASSERTV(rc, 0 == rc);

                numOut = 0;

                ASSERT(!X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(X.outputLength(),     2 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

                rc = mX.endConvert(&buffer, &numOut, 1);

                ASSERT(!X.isInitialState());
                ASSERT( X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(rc,                   0 == rc                  );
                ASSERTV(numOut,               0 == numOut              );
                ASSERTV(X.outputLength(),     2 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());
            }

            // intermediate state (no errors, inner buffer contains retained
            // symbol)
            {
                const char EXP = '1';

                Obj        mX;
                const Obj& X = mX;

                int rc = mX.convert(&buffer,
                                    &numOut,
                                    &numIn,
                                    &source,
                                    &source + 1,
                                    1);
                ASSERTV(rc, 1 == rc);

                numOut = 0;
                buffer = 0;

                ASSERT(!X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(X.outputLength(),     1 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

                rc = mX.endConvert(&buffer, &numOut, 0);

                ASSERT(!X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(rc,                   1 == rc                  );
                ASSERTV(numOut,               0 == numOut              );
                ASSERTV(buffer,               0 == buffer              );
                ASSERTV(X.outputLength(),     1 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

                rc = mX.endConvert(&buffer, &numOut, 1);

                ASSERT(!X.isInitialState());
                ASSERT( X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(rc,                   0   == rc                  );
                ASSERTV(numOut,               1   == numOut              );
                ASSERTV(buffer,               EXP == buffer              );
                ASSERTV(X.outputLength(),     2   == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0   == X.numOutputPending());
            }

            // Testing default value of 'maxNumOut'
            {
                Obj        mX;
                const Obj& X = mX;

                int rc = mX.convert(&buffer,
                                    &numOut,
                                    &numIn,
                                    &source,
                                    &source + 1,
                                    1);
                ASSERTV(rc, 1 == rc);

                numOut = 0;

                ASSERT(!X.isInitialState());
                ASSERT(!X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(X.outputLength(),     1 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

                rc = mX.endConvert(&buffer, &numOut);

                ASSERT(!X.isInitialState());
                ASSERT( X.isDone()        );
                ASSERT(!X.isError()       );

                ASSERTV(rc,                   0 == rc                  );
                ASSERTV(numOut,               1 == numOut              );
                ASSERTV(X.outputLength(),     2 == X.outputLength()    );
                ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());
            }

            // Testing correctness of 'maxNumOut' handling
            {
                for (int i = -2; i < 2; ++i) {
                    const int  MAX_NUM_OUT       = i;
                    const int  EXP_NUM_PENDING   = MAX_NUM_OUT ? 0    : 1;
                    const int  EXP_OUTPUT_LENGTH = MAX_NUM_OUT ? 2    : 1;
                    const int  EXP_RESULT        = MAX_NUM_OUT ? 0    : 1;
                    const int  EXP_NUM_OUT       = MAX_NUM_OUT ? 1    : 0;
                    const bool EXP_DONE          = MAX_NUM_OUT ? true : false;

                    if (veryVerbose) { T_ T_ P(MAX_NUM_OUT) }

                    Obj        mX;
                    const Obj& X = mX;

                    int rc = mX.convert(&buffer,
                                        &numOut,
                                        &numIn,
                                        &source,
                                        &source + 1,
                                        1);
                    ASSERTV(rc, 1 == rc);

                    numOut = 0;

                    ASSERT(!X.isInitialState());
                    ASSERT(!X.isDone()        );
                    ASSERT(!X.isError()       );

                    ASSERTV(X.outputLength(),     1 == X.outputLength()    );
                    ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

                    rc = mX.endConvert(&buffer, &numOut, MAX_NUM_OUT);

                    ASSERT(!X.isInitialState());
                    ASSERT(!X.isError()       );

                    ASSERTV(EXP_DONE          == X.isDone()          );
                    ASSERTV(EXP_RESULT        == rc                  );
                    ASSERTV(EXP_NUM_OUT       == numOut              );
                    ASSERTV(EXP_OUTPUT_LENGTH == X.outputLength()    );
                    ASSERTV(EXP_NUM_PENDING   == X.numOutputPending());
                }
            }
        }

        if (verbose) cout << "\tTesting sequential conversions" << endl;
        {
            const char SOURCE[]      = "DDDDDDDD";
            const int  SOURCE_LENGTH = sizeof SOURCE - 1;
            char       buffer[32];
            int        numOut        = 0;
            int        numIn         = 0;
            int        outputLength  = 0;

            {
                Obj         mX;
                const Obj&  X = mX;

                for (int i = SOURCE_LENGTH - 1; i >= 0; --i) {
                    const int   LENGTH = i;
                    const char *BEGIN = SOURCE;
                    const char *END = BEGIN + LENGTH;

                    for (int j = -1; j < SOURCE_LENGTH + 2; ++j) {
                        const int  MAX_NUM_OUT = j;
                        const int  NUM_PENDING = X.numOutputPending();
                        const int  EXPECTED_NUM_OUT =
                            MAX_NUM_OUT < 0 ||
                                    MAX_NUM_OUT > LENGTH * 2 + NUM_PENDING
                                ? LENGTH * 2 + NUM_PENDING
                                : MAX_NUM_OUT;
                        const int  EXPECTED_NUM_PENDING =
                            (EXPECTED_NUM_OUT - NUM_PENDING) % 2;
                        const int  EXPECTED_NUM_IN =
                            (EXPECTED_NUM_OUT - NUM_PENDING +
                             EXPECTED_NUM_PENDING) / 2;
                        const int  EXP_OUTPUT_LENGTH =
                            outputLength + EXPECTED_NUM_OUT;
                        const bool INITIAL =
                            0 == EXP_OUTPUT_LENGTH ? true : false;

                        bsl::memset(buffer, 0, 32);
                        int   numOut = 0;
                        int   numIn  = 0;

                        if (veryVerbose) { T_ T_ P_(LENGTH) P(MAX_NUM_OUT) }

                        ASSERT(!X.isDone() );
                        ASSERT(!X.isError());

                        ASSERTV(X.outputLength(),
                                outputLength == X.outputLength());
                        ASSERTV(X.numOutputPending(),
                                NUM_PENDING == X.numOutputPending());

                        int rc = mX.convert(buffer,
                                            &numOut,
                                            &numIn,
                                            BEGIN,
                                            END,
                                            MAX_NUM_OUT);

                        ASSERT(!X.isDone());
                        ASSERT(!X.isError());

                        ASSERTV(rc,     EXPECTED_NUM_PENDING == rc);
                        ASSERTV(numOut, EXPECTED_NUM_OUT == numOut);
                        ASSERTV(numIn,  EXPECTED_NUM_IN == numIn);
                        ASSERTV(        INITIAL == X.isInitialState());
                        ASSERTV(EXP_OUTPUT_LENGTH, X.outputLength(),
                                EXP_OUTPUT_LENGTH == X.outputLength());
                        ASSERTV(EXPECTED_NUM_PENDING, X.numOutputPending(),
                                EXPECTED_NUM_PENDING == X.numOutputPending());

                        for (int k = 0; k < EXPECTED_NUM_OUT; ++k) {
                            ASSERTV(buffer[k], '4' == buffer[k]);
                        }

                        outputLength +=  EXPECTED_NUM_OUT;
                    }
                }
            }

            // Testing conversion in done and error state
            {

                Obj         mX;
                const Obj&  X = mX;

                int rc = mX.endConvert(buffer, &numOut);

                ASSERTV( X.isDone()      );
                ASSERTV(!X.isError()     );
                ASSERTV(!X.isAcceptable());

                numOut          = 0;
                numIn           = 0;
                bsl::memset(buffer, 0, 32);

                rc = mX.convert(buffer,
                                &numOut,
                                &numIn,
                                SOURCE,
                                SOURCE + 1);
                ASSERTV(rc, -2 == rc);

                ASSERTV(!X.isDone()      );
                ASSERTV( X.isError()     );
                ASSERTV(!X.isAcceptable());

                ASSERTV(rc,               -2 == rc              );
                ASSERTV(numOut,            0 == numOut          );
                ASSERTV(numIn,             0 == numIn           );
                ASSERTV(X.outputLength(),  0 == X.outputLength());

                rc = mX.convert(buffer, &numOut, &numIn, SOURCE, SOURCE + 1);

                ASSERTV(!X.isDone()      );
                ASSERTV( X.isError()     );
                ASSERTV(!X.isAcceptable());

                ASSERTV(rc,               -1 == rc              );
                ASSERTV(numOut,            0 == numOut          );
                ASSERTV(numIn,             0 == numIn           );
                ASSERTV(X.outputLength(),  0 == X.outputLength());
            }
        }

        if (verbose) cout << "\tTesting truncated overloads" << endl;
        {
            // Since overloads with fewer parameters just call overloads with
            // more parameters, we can use the last ones (fully tested by now)
            // as models for comparison.  We want to make sure that incoming
            // parameters are correctly passed to full overloads and the return
            // values are correctly passed back.

            char buffer[128];
            char modelBuffer[128];

            for (int i = 0; i <= SOURCE_LENGTH; ++i) {
                const int   LENGTH = i;
                const char *BEGIN  = SOURCE;
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

                int expected = mXModel.convert(modelBuffer,
                                               &numOut,
                                               &numIn,
                                               BEGIN,
                                               END);

                // truncated 'convert'

                int rc = mX.convert(buffer, BEGIN, END);

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(expected, rc, expected  == rc);

                ASSERTV(0 == bsl::strncmp(modelBuffer, buffer, numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());

                numOut = 0;
                bsl::memset(buffer,      0, sizeof(buffer     ));
                bsl::memset(modelBuffer, 0, sizeof(modelBuffer));


                expected = mXModel.endConvert(modelBuffer, &numOut);

                // truncated 'endConvert'

                rc = mX.endConvert(buffer);

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(expected, rc, expected  == rc);

                ASSERTV(0 == bsl::strncmp(modelBuffer, buffer, numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());
            }
        }

        if (verbose) cout << "\tTesting compatibility with standard iterators"
                          << endl;
        {

            bsl::list<char>   input;
            bsl::vector<char> output(128);

            char modelBuffer[128];

            for (int i = 0; i <= SOURCE_LENGTH; ++i) {
                const int   LENGTH = i;
                const char *BEGIN  = SOURCE;
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

                int expected = mXModel.convert(modelBuffer,
                                               &numOutModel,
                                               &numInModel,
                                               BEGIN,
                                               END);

                int rc = mX.convert(output.begin(),
                                    &numOut,
                                    &numIn,
                                    input.begin(),
                                    input.end());

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(expected,    rc,     expected     == rc    );
                ASSERTV(numOutModel, numOut, numOutModel  == numOut);
                ASSERTV(numInModel,  numIn,  numInModel   == numIn );

                ASSERTV(0 == bsl::strncmp(modelBuffer, output.data(), numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());

                numOutModel = 0;
                numOut      = 0;
                bsl::memset(modelBuffer, 0, sizeof(modelBuffer));

                expected = mXModel.endConvert(modelBuffer, &numOutModel);

                rc = mX.endConvert(output.begin(), &numOut);

                ASSERTV(XModel.isAcceptable() == X.isAcceptable());
                ASSERTV(XModel.isDone()       == X.isDone()      );
                ASSERTV(XModel.isError()      == X.isError()     );

                ASSERTV(expected, rc,        expected     == rc    );
                ASSERTV(numOutModel, numOut, numOutModel  == numOut);

                ASSERTV(0 == bsl::strncmp(modelBuffer, output.data(), numOut));
                ASSERTV(XModel.outputLength() == X.outputLength());

                input.clear();
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            char  buffer[] = "00";
            int   num      = 0;
            char *begin    = buffer;
            char *end      = buffer + 1;
            int  *nullI    = 0;

            ASSERT_SAFE_FAIL(mX.convert(buffer, nullI, &num,  begin, end  ));
            ASSERT_SAFE_FAIL(mX.convert(buffer, &num,  nullI, begin, end  ));
            ASSERT_SAFE_PASS(mX.convert(buffer, &num,  &num,  begin, end  ));

            ASSERT_SAFE_FAIL(mX.endConvert(buffer, nullI));
            ASSERT_SAFE_PASS(mX.endConvert(buffer, &num ));
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

        const char *INPUT            = "@ABCDEFGHIJKLMNO";
        const char *EXPECTED         = "404142434445464748494A4B4C4D4E4F";
        const int   EXPECTED_NUM_IN  = 16;
        const int   EXPECTED_NUM_OUT = 32;
        char        buffer[EXPECTED_NUM_OUT + 1];
        int         numIn;
        int         numOut;

        bsl::memset(buffer, 0, sizeof(buffer));

        Obj         mX;
        const Obj&  X = mX;

        ASSERT( X.isUpperCase()   );
        ASSERT( X.isInitialState());
        ASSERT( X.isAcceptable()  );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        int rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            INPUT,
                            INPUT + EXPECTED_NUM_IN);

        ASSERTV(rc,     0                == rc                            );
        ASSERTV(numOut, EXPECTED_NUM_OUT == numOut                        );
        ASSERTV(numIn,  EXPECTED_NUM_IN  == numIn                         );
        ASSERTV(buffer, 0                == bsl::strncmp(EXPECTED,
                                                         buffer,
                                                         EXPECTED_NUM_OUT));

        ASSERTV(X.outputLength(),     EXPECTED_NUM_OUT == X.outputLength());
        ASSERTV(X.numOutputPending(), 0 == X.numOutputPending());

        ASSERT(!X.isInitialState());
        ASSERT( X.isAcceptable()  );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        numOut = 0;
        rc = mX.endConvert(buffer, &numOut);

        ASSERTV(rc,     0 == rc    );
        ASSERTV(numOut, 0 == numOut);
        ASSERT(!X.isInitialState() );
        ASSERT(!X.isAcceptable()   );
        ASSERT( X.isDone()         );
        ASSERT(!X.isError()        );

        mX.reset();

        ASSERT( X.isInitialState());
        ASSERT( X.isAcceptable()  );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        rc = mX.convert(buffer,
                            &numOut,
                            &numIn,
                            INPUT,
                            INPUT + EXPECTED_NUM_IN,
                            EXPECTED_NUM_OUT - 1);

        ASSERTV(rc,     1                    == rc                           );
        ASSERTV(numOut, EXPECTED_NUM_OUT - 1 == numOut                       );
        ASSERTV(numIn,  EXPECTED_NUM_IN      == numIn                        );
        ASSERTV(buffer, 0                    == bsl::strncmp(
                                                        EXPECTED,
                                                        buffer,
                                                        EXPECTED_NUM_OUT - 1));

        ASSERTV(X.outputLength(),
                EXPECTED_NUM_OUT - 1 == X.outputLength());
        ASSERTV(X.numOutputPending(), 1 == X.numOutputPending());

        ASSERT(!X.isInitialState());
        ASSERT(!X.isAcceptable()   );
        ASSERT(!X.isDone()        );
        ASSERT(!X.isError()       );

        numOut = 0;
        rc = mX.endConvert(buffer, &numOut);

        ASSERTV(rc,     0 == rc    );
        ASSERTV(numOut, 1 == numOut);
        ASSERT(!X.isInitialState() );
        ASSERT(!X.isAcceptable()   );
        ASSERT( X.isDone()         );
        ASSERT(!X.isError()        );
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
