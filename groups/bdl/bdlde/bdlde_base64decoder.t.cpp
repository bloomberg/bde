// bdlde_base64decoder.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_base64decoder.h>

#include <bdlde_base64encoder.h>        // for testing only

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // memset()
#include <bsl_cctype.h>    // isgraph()
#include <bsl_climits.h>   // INT_MIN
#include <bsl_sstream.h>

#include <stdio.h>

#undef SS  // Solaris 5.10/x86 sys/regset.h via stdlib.h
#undef ES
#undef GS

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This decoder is a kind of 'mechanism' that can be customized at
// construction.  Since there is currently no prevision to change the
// customization after construction the choice for primary constructor must
// itself permit full customization:
//..
//  bdlde::Base64Decoder(int unrecognizedIsErrorFlag);
//..
// The primary manipulators are those needed to reach every attainable state
// from the initial state (i.e., that of a newly constructed object).  Clearly
// 'convert' is one, but 'endConvert' is also needed in order to reach the
// 'DONE' state.  Hence, the primary manipulators are
//..
//  int convert(char *out, int *numOut, int *ni, begin, end, int maxNumOut);
//  int endConvert(char *out, int *numOut, int maxNumOut);
//..
// The basic accessers for the encoder are all the functions that return
// information about the customization and/or execution state:
//..
//  bool isAcceptable() const;
//  bool isDone const;
//  bool isError() const;
//  bool isMaximal() const;
//  bool isInitialState() const;
//  bool isUnrecognizedAnError() const;
//  int outputLength() const;
//..
// The following tables characterize the major logical states and illustrates
// transitions for the primary manipulators (as implemented under the heading
// "Named STATE Values" in the "GLOBAL TYPEDEFS/CONSTANTS" section below):
//..
//  INPUT CLASS       SYM  CHARACTERIZATION ('convert' and 'endConvert')
//  -----------       ---  ----------------------------------------------------
//  BASE64 "NUMERIC"  N    [A-Z][a-z][0-9]+/
//  EQUAL             E    =
//  SPACE             S    [\x09-\x0D]\x32
//  UNRECOGNIZED (@)  U    [\x00-08][\x0E-\x31][\x5B-\x5F][\x7B-\xFF]
//  END-OF-INPUT      $    Signaled by a call to 'endConvert'
//
// (@) Note that in Relaxed mode UNRECOGNIZED is treated as if it were SPACE.
//
//  MAJOR STATE NAME  SYM  CHARACTERIZATION
//  ----------------  ---  ----------------------------------------------------
// *INITIAL_STATE     S0   nothing in accumulator; output length is 0
//  State 1           S1   one char in accumulator
//  State 2           S2   two chars in accumulator (not an =)
//  State 3           S3   three chars in accumulator (not an =)
// *State 4           S4   nothing in accumulator; output length is NOT 0
// *DONE_STATE:       DS   end of input noted
// *SOFT_DONE_STATE:  SDS  logical end of input detected
//  SAW_ONE_EQUAL:    SOE  '=' detected in third character position
//  ERROR_STATE       ES/-  error state (must be last)
//
// '*' = Accepting State
//
//                =====================================
//                = SIMPLIFIED STATE TRANSITION TABLE =
//                =====================================
//
//                 STATE    N     E     S     U     $
//                 -----   ---   ---   ---   ---   ---
//                  *S0:   S1     -    S0     -    DS
//                   S1:   S2     -    S1     -     -
//                   S2:   S3     -    S2     -     -
//                   S3:   S4     -    S3     -     -
//                  *S4:   S1     -    S4     -    DS
//                  *DS:    -     -     -     -     -
//                 *SDS:    -     -    SDS    -    DS
//                  SOE:    -    SDS   SOE    -     -
//                   ES:    -     -     -     -     -
//..
// Note that this table represents a simplification of the complete partition.
// The numeric class N can be further divided into the following sub-classes:
//..
//  INPUT SUB CLASS   SYM  CHARACTERIZATION ('convert' and 'endConvert')
//  ---------------   ---  ----------------------------------------------------
//  before 2 equals   N2   AQgw
//  before 1 equal    N1   EIMUYckos048
//  not before an =   N0   \BCDFGHJKLNOPRSTVWXZabdefhijlmnpqrtuvxyz1235679=
//..
// Giving way to the following minor substates:
//
//  MINOR STATE NAME  SYM  CHARACTERIZATION
//  ----------------  ---  ----------------------------------------------------
//  State 2a          S2A  two chars in accumulator cannot follow with an equal
//  State 2b          S2B  two chars in accumulator can follow with an equal
//  State 3a          S3A  3 chars in acc (third not '=') cannot follow w/=
//  State 3b          S3B  3 chars in acc (third not '=') *can* follow w/=
//..
// The complete logical transition table would then be as follows:
//..
//              ========================================
//              =    COMPLETE STATE TRANSITION TABLE   =
//              ========================================
//
//                     ,----N-----.
//
//              STATE  N0   N1   N2    E    S    U    $
//              -----  ---  ---  ---  ---  ---  ---  ---
//               *S0:  S1   S1   S1    -   S0    -   DS
//                S1:  S2A  S2A  S2B   -   S1    -    -
//            /  S2A:  S3A  S3B  S3B   -   S2    -    -
//          S2\  S2B:  S3A  S3B  S3B  SOE  S2    -    -
//            /  S3A:  S4   S4   S4    -   S3    -    -
//          S3\  S3B:  S4   S4   S4   SDS  S3    -    -
//               *S4:  S1   S1   S1    -   S4    -   DS
//               *DS:   -    -    -    -    -    -    -
//              *SDS:   -    -    -    -   SDS   -   DS
//               SOE:   -    -    -   SDS  SOE   -    -
//                ES:   -    -    -    -    -    -    -
//
//..
// It will turn out that distinguishing between states 2A and 2B and states
// 3A and 3B is sufficiently complex as to make it impractical to validate
// prior to case 7; hence, we will use the simplified model in the first 6
// test cases and defer these subtleties until case 7, after the internal
// tables defining these subclasses have themselves been validated (in case 6).
//
// Our first step will be to ensure that each of these states can be reached
// ('::setState'), that an anticipated state can be verified ('::isState'), and
// that each of the above state transitions is verified.  Next, we will ensure
// that each internal table is correct.  Then, using Category Partitioning, we
// enumerate a representative collection of inputs ordered by increasing
// *depth* that will be sufficient to prove that the logic associated with the
// state machine is performing as desired.  Finally, in a separate test case,
// we will verify that the optional output limit for both 'convert' and
// 'endConvert' is handled properly.
//
// Note that because the 'convert' and 'endConvert' methods are parametrized
// based on iterator types, we will want to ensure (at compile time) that their
// respective implementations do not depend on more than minimal iterator
// functionality.  We will accomplish this goal by supplying, as template
// arguments, 'bdeut::InputIterator' for 'convert' and 'bdeut::OutputIterator'
// for both of these template methods.
//
// Note also that, unlike the encoder, there is no "obvious" default mode
// for the decoder; we will therefore ensure (using metafunctions) that no
// default constructor can be instantiated.
//-----------------------------------------------------------------------------
// [ 2] bdlde::Base64Decoder(int unrecognizedIsErrorFlag);
// [ 3] ~bdlde::Base64Decoder();
// [ 8] int convert(char *o, int *no, int *ni, begin, end, int mno);
// [ 8] int endConvert(char *out, int *numOut, int maxNumOut);
// [ 9] void resetState();
// [ 3] bool isAcceptable() const;
// [ 3] bool isDone;
// [ 3] bool isError() const;
// [ 3] bool isMaximal() const;
// [ 3] bool isInitialState() const;
// [ 2] bool isUnrecognizedAnError() const;
// [ 3] int outputLength() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
//*[11] USAGE EXAMPLE
// [ ?] That the input iterator can have *minimal* functionality.
// [ ?] That the output iterator can have *minimal* functionality.
// [ ?] That there is no default constructor.
// [ 1] ::myMin(const T& a, const T& b);
// [ 1] ::printCharN(ostream& output, const char* sequence, int length)
// [ 3] void ::setState(Obj *obj, int state, const char *input);
// [ 3] bool ::isState(Obj *obj, int state);
// [ 3] That we can reach each of the major processing states.
// [ 5] BOOTSTRAP: 'convert' - transitions
// [ 4] BOOTSTRAP: 'endConvert'- transitions
// [ 6] That each internal table has no defective entries.
// [ 7] DFLT convert(char *o, int *no, int *ni, begin, end, int mno = -1);
// [ 7] DFLT endConvert(char *out, int *numOut, int maxNumOut = -1);
// [ 7] That a 3-byte quantum is decoded properly.
// [ 7] That a 2-byte quantum is decoded properly.
// [ 7] That a 1-byte quantum is decoded properly.
// [ 7] That output length is calculated and stored properly.
//*[ 8] That a specified maximum output length is observed.
//*[ 8] That surplus output beyond 'maxNumOut' is buffered properly.
//*[10] STRESS TEST: The decoder properly decodes all encoded output.
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

#define V(X) { if (verbose) { cout << "\t" << X << endl; } } // no () to allow
                                                             // stringing <<
#define VV(X) { if (veryVerbose) { cout << "\t\t" << X << endl; } }
#define VVV(X) { if (veryVeryVerbose) { cout << "\t\t\t" << X << endl; } }
#define VVVV(X) { if (veryVeryVeryVerbose) {cout << "\t\t\t\t" << X << endl;} }

// ============================================================================
//                SEMI-STANDARD HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void loopMeter(unsigned index, unsigned length, unsigned size = 50)
    // Create a visual display for a computation of the specified 'length' and
    // emit updates to 'cerr' as appropriate for the specified 'index'.
    // Optionally specify the 'size' of the display.  The behavior is undefined
    // unless 0 <= index, 0 <= length, 0 < size, and index <= length.  Note
    // that it is expected that indices will be presented in order from 0 to
    // 'length', inclusive, without intervening output to 'stderr'; however,
    // intervening output to 'stdout' may be redirected productively.
{
    ASSERT(0 < size);
    ASSERT(index <= length);

    if (0 == index) {           // We are at the beginning of the loop.
        cerr << "     |";
        for (unsigned i = 1; i < size; ++i) {
            cerr << (i % 5 ? '-' : '+');
        }
        cerr << "|\nBEGIN." << flush;
    }
    else {                      // We are in the middle of the loop.
        int t1 = int((double(index - 1) * size)/length + 0.5);
        int t2 = int((double(index)     * size)/length + 0.5);
        int dt = t2 - t1;       // accumulated ticks (but no accumulated error)

        for (int i = 0; i < dt; ++i) {
           cerr << '.';
        }
        cerr << flush;
    }

    if (index == length) {      // We are at the end of the loop.
        cerr << "END" << endl;
    }
}

// ============================================================================
//                         GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

typedef bdlde::Base64Decoder Obj;

                        // ==================
                        // Named STATE Values
                        // ==================
enum State {
    // Enumeration of logical states described in the test plan overview.
    // These logical states must range in value from INITIAL_STATE = 0 to
    // ERROR_STATE = NUM_STATES - 1.  Note that the number and values of
    // these logical states need not coincide with those defined explicitly
    // in the implementation.

    INITIAL_STATE,
    STATE_ONE,
    STATE_TWO,
    STATE_THREE,
    STATE_FOUR,
    DONE_STATE,
    SOFT_DONE_STATE,
    SAW_ONE_EQUAL,
    ERROR_STATE    // must be last
};

// The following is a list of strings to be used when printing state values.

const char *STATE_NAMES[] = {
    "INTITAL_STATE",
    "State 1",
    "State 2",
    "State 3",
    "State 4",
    "DONE_STATE",
    "SOFT_DONE_STATE",
    "SAW_ONE_EQUAL",
    "ERROR_STATE"
};

const int NUM_STATES = sizeof STATE_NAMES / sizeof *STATE_NAMES;

char assertion[ERROR_STATE + 1 == NUM_STATES];

// The following is a very long text to use both in the stress test and
// in the usage example.

const char BLOOMBERG_NEWS[] =
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

                        // =========================
                        // operator<< for enum State
                        // =========================

ostream& operator<<(ostream& stream, State enumerator)
    // Write the ascii representation of the specified State 'enumerator'
    // to the specified output 'stream'.
{
    ASSERT(0 <= (int)enumerator); ASSERT((int)enumerator <= ERROR_STATE);
    return stream << STATE_NAMES[enumerator] << flush;
}

                        // ==============
                        // Function myMin
                        // ==============

template <class T>
inline
T myMin(const T& a, const T& b)
{
    return a < b ? a : b;
}

                        // ===================
                        // Function printCharN
                        // ===================

ostream& printCharN(ostream& output, const char* sequence, int length)
    // Print the specified character 'sequence' of specified 'length' to the
    // specified 'output' and return a reference to the modifiable 'stream'
    // (if a character is not graphical, its hexadecimal code is printed
    // instead).  The behavior is undefined unless 0 <= 'length' and sequence
    // refers to a valid area of memory of size at least 'length'.
{
    static char HEX[] = "0123456789ABCDEF";

    for (int i = 0; i < length; ++i) {
        unsigned char c = static_cast<unsigned char>(sequence[i]);

        if (isgraph(c)) {
            output << c;
        }
        else {
            output << '<' << HEX[c/16] << HEX[c%16] << '>';
        }
    }
    return output << flush;
}

                        // =================
                        // Function setState
                        // =================

void setState(Obj *object, int state)
    // Move the specified 'object' from its initial (i.e., newly constructed)
    // state to the specified 'state' using 'A' and '=' characters for input
    // as needed.  The behavior is undefined if 'object' is not in its
    // newly-constructed initial state.   Note that when this function is
    // invoked on a newly constructed object, it is presumed that
    // 'isInitialState' has been sufficiently tested to ensure that it returns
    // 'true'.
{
    ASSERT(object); ASSERT(0 <= state); ASSERT(state < NUM_STATES);

    if (!object->isInitialState()) { cout
     << "You must not call 'setState' from other than 'INITIAL_STATE'!" << endl
     << "\tNote that '::isState' *will* alter from the initial state." << endl;
    }

    ASSERT(object->isInitialState()); // If 'object' is "just created" then
                                      // this assertion should be true!

    char b[3];
    int  numOut = -1;
    int  numIn = -1;
    char input = 'A';

    const char *const begin = &input;
    const char *const end = &input + 1;

    switch (state) {
      case INITIAL_STATE: {
        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(1 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case STATE_ONE: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case STATE_TWO: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(1 == object->outputLength());
      } break;
      case STATE_THREE: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(2 == object->outputLength());
      } break;
      case STATE_FOUR: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(3 == object->outputLength());
      } break;
      case DONE_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(1 == object->isAcceptable());
        ASSERT(1 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength()); // In this case, we know!
      } break;
      case SOFT_DONE_STATE: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        input = '=';
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(1 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(2 == object->outputLength());
      } break;
      case SAW_ONE_EQUAL: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        input = '=';
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(1 == object->outputLength());
      } break;

      case ERROR_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(-1 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(1 == object->isError());
        ASSERT(0 == object->isMaximal());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    };
}

                        // ================
                        // Function isState
                        // ================

static bool globalAssertsEnabled = false;
    // If set to true, will enable ASSERTs in '::isState' (for debugging).

class EnabledGuard {
    // Enable/Disable '::isState' ASSERTs for current scope; restore status at
    // end.  Note that guards can be nested.

    bool d_state;

  public:
    explicit
    EnabledGuard(bool flag)
        // Create a guard to control the activation of individual assertions
        // in the '::isState' test helper function using the specified
        // enable 'flag' value.  If 'flag' is 'true' individual false values
        // we be reported as assertion errors.
    : d_state(globalAssertsEnabled) { globalAssertsEnabled = flag; }

    ~EnabledGuard() { globalAssertsEnabled = d_state; }
};

bool isState(Obj *object, int state)
    // Return 'true' if the specified 'object' was initially in the specified
    // 'state', and 'false' otherwise.  Setting the global variable
    // 'globalAssertsEnabled' to 'true' enables individual sub-conditions to
    // be ASSERTed, which can be used to facilitate test driver debugging.
    // Note that the final state of 'object' may (and probably will) be
    // modified arbitrarily from its initial state in order to distinguish
    // similar states.
{
    ASSERT(object); ASSERT(0 <= state); ASSERT(state < NUM_STATES);

    int enabled = globalAssertsEnabled;

    char b[3] = { -1, -1, -1 };
    char input = 'A';
    int  numOut = -1;
    int  numIn = -1;
    int  result = INT_MIN;
    bool rv = false;

    const char *const begin = &input;
    const char *const end = &input + 1;

    switch (state) {
      case INITIAL_STATE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 1 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case STATE_ONE: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->convert(b, &numOut, &numIn, begin, begin);

        int idx = 0;
        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 2.

        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 3.

        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 4.
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == idx;                             ASSERT(c1 || !enabled);

        bool d0 = 0 == (b[0] & 0x3);                    ASSERT(d0 || !enabled);

        bool d1 = 0 == b[1];                            ASSERT(d1 || !enabled);
        bool d2 = 0 == b[2];                            ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case STATE_TWO: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->convert(b, &numOut, &numIn, begin, begin);

        int idx = 1;
        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 3.

        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 4.
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == idx;                             ASSERT(c1 || !enabled);

        bool d0 = !0;                                   ASSERT(d0 || !enabled);
        bool d1 = 0 == (b[1] & 0xF);                    ASSERT(d1 || !enabled);
        bool d2 = 0 == b[2];                            ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case STATE_THREE: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->convert(b, &numOut, &numIn, begin, begin);

        int idx = 2;
        result = object->convert(b + idx, &numOut, &numIn, begin, end);
        idx += numOut;

        // State 4.
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == idx;                             ASSERT(c1 || !enabled);

        bool d0 = !0;                                   ASSERT(d0 || !enabled);
        bool d1 = !0;                                   ASSERT(d1 || !enabled);
        bool d2 = 0 == (b[2] & 0x3F);                   ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case STATE_FOUR: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        int idx = object->convert(b, &numOut, &numIn, begin, begin, 0);
        result = object->convert(b, &numOut, &numIn, begin, end);

        // State 1.
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = idx == numOut;                        ASSERT(c1 || !enabled);

        bool d0 = !0;                                   ASSERT(d0 || !enabled);
        bool d1 = !0;                                   ASSERT(d1 || !enabled);
        bool d2 = !0;                                   ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case DONE_STATE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 1 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->convert(b, &numOut, &numIn, begin, end);

        // ERROR_STATE.
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = -2 == result;                         ASSERT(c0 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0;

      } break;
      case SOFT_DONE_STATE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 1 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        result = object->convert(b, &numOut, &numIn, begin, end);

        // ERROR_STATE
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = 0 > result;                           ASSERT(c0 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0;

      } break;
      case SAW_ONE_EQUAL: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        // The above ensures 'object' is in STATE_ONE, STATE_TWO, STATE_THREE,
        // or SAW_ONE_EQUAL.

        input = 'A';
        result = object->convert(b, &numOut, &numIn, begin, end);

        // SOFT_DONE_STATE
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

#if 0
// TBD remove; changed test
        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0]                      ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);

        return a0 && a1 && a2 && a3 && a4
            && b0 && b1 && b2 && b3 && b4
            && c0 && c1 && d0 && d1 && d2;
#endif

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4;

      } break;
      case ERROR_STATE: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 1 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isMaximal();             ASSERT(a3 || !enabled);
        bool a4 = 0 == object->isInitialState();        ASSERT(a4 || !enabled);

        char b[3] = { -1, -1, -1 };
        int  numOut = -1;
        int  result = object->endConvert(b, &numOut);

        // ERROR_STATE
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isMaximal();             ASSERT(b3 || !enabled);
        bool b4 = 0 == object->isInitialState();        ASSERT(b4 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && a4
          && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;

      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
    return rv;
}

// ============================================================================
//                         SUPPORT FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------
namespace BloombergLP {

int streamEncoder(bsl::ostream& os, bsl::istream& is)
{
    enum {
        SUCCESS      =  0,
        ENCODE_ERROR = -1,
        IO_ERROR     = -2
    };

    bdlde::Base64Encoder converter(false);   // Do not report errors.

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

            int status = converter.convert(output, &numOut, &numIn,
                                           input,   inputEnd,
                                           outputEnd - output);
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

        int numOut;

        int more = converter.endConvert(output, &numOut, outputEnd-output);
        if (more < 0) {
            return ENCODE_ERROR;                                      // RETURN
        }

        output += numOut;

        if (!more) { // no more output
            break;
        }

        ASSERT(output == outputEnd);  // output buffer is full

        os.write (outputBuffer, sizeof outputBuffer);  // write buffer
        if (os.fail()) {
            return IO_ERROR;                                          // RETURN
        }
        output = outputBuffer;
    }

    if (output > outputBuffer) {
        os.write (outputBuffer, output - outputBuffer);
    }

    return is.eof() && os.good() ? SUCCESS : IO_ERROR;
}

int streamDecoder(bsl::ostream& os, bsl::istream& is)
{
    enum {
        SUCCESS      =  0,
        DECODE_ERROR = -1,
        IO_ERROR     = -2
    };

    bdlde::Base64Decoder converter(false);   // Do not report errors.

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

            int status = converter.convert(output, &numOut, &numIn,
                                           input,   inputEnd,
                                           outputEnd - output);
            if (status < 0) {
                return DECODE_ERROR;                                  // RETURN
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

        int numOut;

        int more = converter.endConvert(output, &numOut, outputEnd-output);
        if (more < 0) {
            return DECODE_ERROR;                                      // RETURN
        }

        output += numOut;

        if (!more) { // no more output
            break;
        }

        ASSERT(output == outputEnd);  // output buffer is full

        os.write (outputBuffer, sizeof outputBuffer);  // write buffer
        if (os.fail()) {
            return IO_ERROR;                                          // RETURN
        }
        output = outputBuffer;
    }

    if (output > outputBuffer) {
        os.write (outputBuffer, output - outputBuffer);
    }

    return is.eof() && os.good() ? SUCCESS : IO_ERROR;
}

}  // close enterprise namespace

// ============================================================================
//                                TEST CASES
// ----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose,   \
                                                      bool veryVeryVeryVerbose)

DEFINE_TEST_CASE(11)
{
        // --------------------------------------------------------------------
        // STRESS TEST
        //   Demonstrate that the encoder/decoder can encode/decode a large
        // number of characters continuously.
        //
        // Concerns:
        //   - That the encoder and decoder are not subject to any physical
        //     or software limit.
        //   - That encodedLength returns the right number of output characters
        //     from the encoder including the soft line breaks.
        //
        // Plan:
        //   - First have the encoder convert a large amount of text, created
        //     for example by copying from a Bloomberg news page.  Then use the
        //     decoder to convert the result back into its original form.
        //     Finally, verify that the output from the decoder to the input to
        //     the encoder are identical.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   Stress limits of encoder/decoder.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRESS TEST" << endl
                          << "=============" << endl;

        V("Verify a text sample whose number of characters is 0 mod 3.");
        {
            const char sample[] = "Education is a useful tool.";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("-----------------------------------------------------------");
        V("Verify a text sample whose number of characters is 1 mod 3.");
        {
            const char sample[] = "Education is also a lot of fun.";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("-----------------------------------------------------------");
        V("Verify a text sample whose number of characters is 2 mod 3.");
        V("and also an even number");
        {
            const char sample[] = "Education was originally a tool.";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("-----------------------------------------------------------");
        V("Verify an ad-hoc text sample.");
        {
            const char sample[] =
                "NXTW MSG 1 * 401817628 453493 0<GO>NXTW 97<GO>";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("-----------------------------------------------------------");
        V("Verify the second ad-hoc text sample.");
        {
            const char sample[] = "NXTW PRLS 78358<GO>";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("-----------------------------------------------------------");
        V("Verify the third ad-hoc text sample.");
        {
            const char sample[] = "NXTW BICQ > 11 57 401806044<GO>";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("------------------------------------------------------");
        V("Verify a large text sample consisting of 101 lines and");
        V("(5369 - 202) = 5167 characters.")

        V("Verify encoder with unlimited line length.");
        {
            const char* sample = BLOOMBERG_NEWS;

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }

        V("Verify encoder with maximum line length = 76.");
        {
            const char* sample = BLOOMBERG_NEWS;

            int maxLineLength = 76;

            bdlde::Base64Encoder encoder;
            bdlde::Base64Decoder decoder(true);

            int   origSize = strlen(sample);
            int   encodedLen = encoder.encodedLength(origSize, maxLineLength);
            char *encoded = new char[encodedLen];
            int   maxDecodedLen = decoder.maxDecodedLength(encodedLen);
            char *decoded = new char[maxDecodedLen] ;

            VV("Original input size = " << origSize);
            VV("Encoded length      = " << encodedLen);
            VV("Max decoded length  = " << maxDecodedLen);

            int numOutEncoded   = -1;
            int numOutEndEncode = -1;
            int numInEncoded    = -1;
            int numOutDecoded   = -1;
            int numOutEndDecode = -1;
            int numInDecoded    = -1;

            encoder.convert(encoded, &numOutEncoded, &numInEncoded,
                            sample, sample + origSize);
            encoder.endConvert(encoded + numOutEncoded, &numOutEndEncode);
            numOutEncoded += numOutEndEncode;
            ASSERT(numOutEncoded == encodedLen);

            decoder.convert(decoded, &numOutDecoded, &numInDecoded,
                            encoded, encoded + numOutEncoded);
            decoder.endConvert(decoded + numOutDecoded, &numOutEndDecode);
            numOutDecoded += numOutEndDecode;
            ASSERT(numOutDecoded <= maxDecodedLen);

            VV("NumOut from endEncode     = " << numOutEndEncode);
            VV("Total numOut from encoder = " << numOutEncoded);
            VV("NumOut from endDecode     = " << numOutEndDecode);
            VV("Total numOut from decoder = " << numOutDecoded);
            VV("Encoder input  = \n\"" << sample << "\"");
            VV("Encoder output = \n\"" << encoded << "\"");
            VV("Decoder output = \n\"" << decoded << "\"");

            ASSERT(origSize == numOutDecoded);
            ASSERT(0 == memcmp(decoded, sample, origSize));

            delete[] encoded;
            delete[] decoded;
        }
}

DEFINE_TEST_CASE(10)
{
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Demonstrate that the example compiles, links, and runs.
        //
        // Concerns:
        //   That the example compiles, links, and runs.
        //
        // Plan:
        //   Create the example in a form that can be cut and pasted easily
        //   into the header file.  Then use the stream encoder and decoder
        //   with an example string, and compare the output with the original
        //   string.
        //
        // Tactics:
        //   - Ad-HocData Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage Example
///-------------
// The following example shows how to use a 'bdlde::Base64Decoder' object to
// implement a function, 'streamDecode', that reads text from a
// 'bsl::istream', decodes that text from base 64 representation, and writes
// the decoded text to a 'bsl::ostream'.  'streamDecoder' returns 0 on
// success and a negative value if the input data could not be successfully
// decoded or if there is an I/O error.
//..
// streamencoder.h                      -*-C++-*-
//
// int streamEncoder(bsl::ostream& os, bsl::istream& is);
//     // Read the entire contents of the specified input stream 'is', convert
//     // the input base-64 encoding into plain text, and write the decoded
//     // text to the specified output stream 'os'.  Return 0 on success, and a
//     // negative value otherwise.
//..
        bsl::istringstream inStream(bsl::string(BLOOMBERG_NEWS,
                                                sizeof(BLOOMBERG_NEWS)));
        bsl::stringstream  outStream;
        bsl::stringstream  backInStream;

        ASSERT(0 == streamEncoder(outStream, inStream));
        ASSERT(0 == streamDecoder(backInStream, outStream));

        ASSERT(0 == strcmp(BLOOMBERG_NEWS, backInStream.str().c_str()));
}

DEFINE_TEST_CASE(9)
{
        // --------------------------------------------------------------------
        // RESET STATE
        //   Verify the 'resetState' method.
        //
        // Concerns:
        //   - That resetState returns the object to its initial state (i.e.,
        //      the same as it was immediately after construction.
        //   - That the initial configuration is not altered.
        //
        // Plan:
        //   - Use put the object in each state and verify the expected
        //      state is not/is in the initial state before/after the call to
        //      'resetState'.
        //   - Verify that the initial configuration has not changed.
        //   - Repeat the above with a different configuration.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Loop-Based Implementation Technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESET STATE" << endl
                          << "===========" << endl;

        if (verbose) cout << "\nVerify 'resetState'." << endl;

        if (verbose) cout << "\tWith 'isUnregnizeAndError' = false." << endl;
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(false);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.resetState();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, 0 == obj.isUnrecognizedAnError());
            }
        }

        if (verbose) cout << "\tWith 'isUnregnizeAndError' = true." << endl;
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(true);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.resetState();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, 1 == obj.isUnrecognizedAnError());
            }
        }
}

DEFINE_TEST_CASE(8)
{
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS.
        //   Complete the testing of 'convert' and 'endConvert'.
        //
        // Concerns:
        //   - That the return status is correct.
        //   - That new input is NOT consumed when output is deferred.
        //   - That the call may be repeated for small output limits.
        //   - That the boundary condition of a zero output limit works.
        //
        // Plan:
        //   - Using Category Partitioning, enumerate input sequences of
        //      increasing length to a sufficient *Depth* that all logic
        //      associated with deferring output is exercised thoroughly.
        //   - Use as the depth metric the sum of first input length and
        //      first output limit; the second input and output limit will
        //      be chosen ad hoc.
        //      + Not supplying an output limit has the value -1.
        //      + Not supplying an input (implying 'endConvert') is also valued
        //         at -1 (hence, an unlimited 'endConvert' will value to -2).
        //
        //  - Treat special cases "ad hoc" following the regular sequences
        //     for each depth category:
        //     + terminal sequence involving = and ==.
        //     + sequences where 'convert' follows 'convert'
        //     + sequences where 'convert' must first emit delayed output.
        //     + sequences where 'endConvert' has a limit with delayed output.
        //
        // Tactics:
        //   - Category Partitioning, Depth-Ordered Enumeration, and Ad Hoc
        //      Data Selection Methods
        //   - Table-Based Implementation Technique.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        EnabledGuard g(true); // If state is wrong, provide detailed diagnosis.

        // Major State Aliases:
        const int D = DONE_STATE;
        const int S = SOFT_DONE_STATE;
        const int E = ERROR_STATE;

        const char *const END_INPUT = 0;  // Used to indicate 'endConvert'
                                          // should be called instead of
                                          // 'convert'.
        const int _ = -1;

        const char *N = END_INPUT;        // alias

        if (verbose) cout <<
                "\nVerifying optional output limit argument." << endl;
        {
            static const struct {
                int d_lineNum;          // source line number

                const char *d_input1_p; // input characters (null terminated)
                                        // Note: 0 implies call to 'endConvert'
                int d_limit1;           // output limit 1
                int d_return1;          // status after call to first function
                int d_state1;           // state after call to first function
                int d_numOut1;          // # bytes written by first function
                int d_numIn1;           // # bytes read by first function.

                const char *d_input2_p; // input characters (null terminated)
                                        // Note: 0 implies call to 'endConvert'
                int d_limit2;           // output limit 2
                int d_return2;          // status after call to second function
                int d_state2;           // state after call to second function
                int d_numOut2;          // # bytes written by second function
                int d_numIn2;           // # bytes read by second function.

                const char *d_output_p; // combined expected output data

            } DATA[] = {
//--------------^

  // *** DEPTH-ORDERED ENUMERATION: Depth D = input length 1 + output limit 1

//    v---INPUT---v v-OUTPUT--v  v---INPUT---v v----------OUTPUT------------v
//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = -2
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,         -1, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = -1
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          0, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",        -1, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  0
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          1, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         0, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",       -1, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  1
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          2, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         1, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "A",        0, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",      -1, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },

// *** Testing 'convert' following 'convert'.
{ L_, "A",        0, 0, 1, 0, 1, "",        -1, 0, 1, 0, 0,""                },

{ L_, "A",        0, 0, 1, 0, 1, "",         0, 0, 1, 0, 0,""                },
{ L_, "A",        0, 0, 1, 0, 1, "A",       -1, 0, 2, 1, 1,"\x00"            },

{ L_, "A",        0, 0, 1, 0, 1, "",         1, 0, 1, 0, 0,""                },
{ L_, "A",        0, 0, 1, 0, 1, "A",        0, 1, 2, 0, 1,""                },
{ L_, "A",        0, 0, 1, 0, 1, "AA",      -1, 0, 3, 2, 2,"\x00\x00"        },

{ L_, "A",        0, 0, 1, 0, 1, "",         2, 0, 1, 0, 0,""                },
{ L_, "A",        0, 0, 1, 0, 1, "A",        1, 0, 2, 1, 1,"\x00"            },
{ L_, "A",        0, 0, 1, 0, 1, "AA",       0, 2, 3, 0, 2,""                },
{ L_, "A",        0, 0, 1, 0, 1, "AAQ",     -1, 0, 4, 3, 3,"\x00\x00\x10"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  2
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          3, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         2, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        1, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       0, 1, 2, 0, 2, N,         -1,-1, E, 0, _,""                },
{ L_, "AAQ",     -1, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  3
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          4, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         3, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        2, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       1, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      0, 2, 3, 0, 3, N,         -1,-1, E, 0, _,""                },
{ L_, "AAAQ",    -1, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },

// *** Verify '=' and '==' variants.
{ L_, "AAE=",    -1, 0, S, 2, 4, N,         -1, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",    -1, 0, S, 1, 4, N,         -1, 0, D, 0, _,"\x01"            },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  4
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          5, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         4, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        3, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       2, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      1, 1, 3, 1, 3, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAQ",     0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",   -1, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },

// *** Verify '=' and '==' variants.
                                                                        // 4.-2
{ L_, "AAE=",     0, 2, S, 0, 4, N,         -1, 0, D, 2, _,"\x00\x01"        },
{ L_, "AQ==",     0, 1, S, 0, 4, N,         -1, 0, D, 1, _,"\x01"            },

// *** Testing 'endConvert' following 'convert' with delayed output.
                                                                        // 4.-1
{ L_, "AAAg",     0, 3, 4, 0, 4, N,          0, 3, S, 0, _,""                },
{ L_, "AAE=",     0, 2, S, 0, 4, N,          0, 2, S, 0, _,""                },
{ L_, "AQ==",     0, 1, S, 0, 4, N,          0, 1, S, 0, _,""                },

                                                                        // 4.0
{ L_, "AAAg",     0, 3, 4, 0, 4, N,          1, 2, S, 1, _,"\x00"            },
{ L_, "AAE=",     0, 2, S, 0, 4, N,          1, 1, S, 1, _,"\x00"            },
{ L_, "AQ==",     0, 1, S, 0, 4, N,          1, 0, D, 1, _,"\x01"            },

                                                                        // 4.1
{ L_, "AAAg",     0, 3, 4, 0, 4, N,          2, 1, S, 2, _,"\x00\x00"        },
{ L_, "AAE=",     0, 2, S, 0, 4, N,          2, 0, D, 2, _,"\x00\x01"        },
{ L_, "AQ==",     0, 1, S, 0, 4, N,          2, 0, D, 1, _,"\x01"            },

                                                                        // 4.2
{ L_, "AAAg",     0, 3, 4, 0, 4, N,          3, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAE=",     0, 2, S, 0, 4, N,          3, 0, D, 2, _,"\x00\x01"        },
{ L_, "AQ==",     0, 1, S, 0, 4, N,          3, 0, D, 1, _,"\x01"            },

// *** Testing 'convert' following 'convert' with delayed output.
                                                                        // 4.-1
{ L_, "AAAg",     0, 3, 4, 0, 4, "",        -1, 0, 4, 3, 0,"\x00\x00\x20"    },

                                                                        // 4.0
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",       -1, 0, 1, 3, 1,"\x00\x00\x20"    },

                                                                        // 4.1
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         1, 2, 4, 1, 0,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",      -1, 0, 2, 4, 2,"\x00\x00\x20\x00"},

                                                                        // 4.2
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         2, 1, 4, 2, 0,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",     -1, 0, 3, 5, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 4.3
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         3, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        2, 1, 1, 2, 1,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",    -1, 0, 4, 6, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",    -1, 0, S, 5, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",    -1, 0, S, 4, 4,"\x00\x00\x20\x01"},

                                                                        // 4.4
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         4, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        3, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       2, 2, 2, 2, 2,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     0, 3, 4, 0, 0,""                },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",   -1, 0, 1, 6, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 4.5
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         5, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        4, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       3, 1, 2, 3, 2,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      2, 2, 2, 2, 2,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     1, 2, 1, 1, 1,"\x00"            },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    0, 3, 4, 0, 0,""                },

                                                                        // 4.6
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         6, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        5, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       4, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      3, 2, 3, 3, 3,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     2, 2, 2, 2, 2,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     2, 2, 2, 2, 2,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     2, 2, 2, 2, 2,"\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    1, 2, 1, 1, 1,"\x00"            },

                                                                        // 4.7
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         7, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        6, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       5, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      4, 1, 3, 4, 3,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     3, 3, 4, 3, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     3, 2, S, 3, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     3, 1, S, 3, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    2, 2, 2, 2, 2,"\x00\x00"        },

                                                                        // 4.8
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         8, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        7, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       6, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      5, 0, 3, 5, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     4, 2, 4, 4, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     4, 1, S, 4, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     4, 0, S, 4, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    3, 3, 4, 3, 4,"\x00\x00\x20"    },

                                                                        // 4.9
{ L_, "AAAg",     0, 3, 4, 0, 4, "",         9, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        8, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       7, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      6, 0, 3, 5, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     5, 1, 4, 5, 4,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     5, 0, S, 5, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     5, 0, S, 4, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    4, 2, 1, 4, 5,"\x00\x00\x20\x00"},

                                                                        // 4.10
{ L_, "AAAg",     0, 3, 4, 0, 4, "",        10, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",        9, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       8, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      7, 0, 3, 5, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     6, 0, 4, 6, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     6, 0, S, 5, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     6, 0, S, 4, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    5, 1, 1, 5, 5,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 4.11
{ L_, "AAAg",     0, 3, 4, 0, 4, "",        11, 0, 4, 3, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "A",       10, 0, 1, 3, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AA",       9, 0, 2, 4, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAA",      8, 0, 3, 5, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQ",     7, 0, 4, 6, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAE=",     7, 0, S, 5, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     0, 3, 4, 0, 4, "AQ==",     7, 0, S, 4, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     0, 3, 4, 0, 4, "AAAQA",    6, 0, 1, 6, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  5
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          6, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         5, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        4, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       3, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      2, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     1, 2, 4, 1, 4, N,         -1, 0, D, 2, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",  -1, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},

// *** Verify '=' and '==' variants.
                                                                        // 5.-2
{ L_, "AAE=",     1, 1, S, 1, 4, N,         -1, 0, D, 1, _,"\x00\x01"        },
{ L_, "AQ==",     1, 0, S, 1, 4, N,         -1, 0, D, 0, _,"\x01"            },

// *** Testing 'endConvert' following 'convert' with delayed output.
                                                                        // 5.-1
{ L_, "AAAg",     1, 2, 4, 1, 4, N,          0, 2, S, 0, _,"\x00"            },
{ L_, "AAE=",     1, 1, S, 1, 4, N,          0, 1, S, 0, _,""                },
{ L_, "AQ==",     1, 0, S, 1, 4, N,          0, 0, D, 0, _,"\x01"            },

                                                                        // 5.0
{ L_, "AAAg",     1, 2, 4, 1, 4, N,          1, 1, S, 1, _,"\x00\x00"        },
{ L_, "AAE=",     1, 1, S, 1, 4, N,          1, 0, D, 1, _,"\x00\x01"        },
{ L_, "AQ==",     1, 0, S, 1, 4, N,          1, 0, D, 0, _,"\x01"            },

                                                                        // 5.1
{ L_, "AAAg",     1, 2, 4, 1, 4, N,          2, 0, D, 2, _,"\x00\x00\x20"    },
{ L_, "AAE=",     1, 1, S, 1, 4, N,          2, 0, D, 1, _,"\x00\x01"        },
{ L_, "AQ==",     1, 0, S, 1, 4, N,          2, 0, D, 0, _,"\x01"            },

                                                                        // 5.2
{ L_, "AAAg",     1, 2, 4, 1, 4, N,          3, 0, D, 2, _,"\x00\x00\x20"    },
{ L_, "AAE=",     1, 1, S, 1, 4, N,          3, 0, D, 1, _,"\x00\x01"        },
{ L_, "AQ==",     1, 0, S, 1, 4, N,          3, 0, D, 0, _,"\x01"            },

// *** Testing 'convert' following 'convert' with delayed output.
                                                                        // 5.-1
{ L_, "AAAg",     1, 2, 4, 1, 4, "",        -1, 0, 4, 2, 0,"\x00\x00\x20"    },

                                                                        // 5.0
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         0, 2, 4, 0, 0,""                },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",       -1, 0, 1, 2, 1,"\x00\x00\x20"    },

                                                                        // 5.1
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         1, 1, 4, 1, 0,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",      -1, 0, 2, 3, 2,"\x00\x00\x20\x00"},

                                                                        // 5.2
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         2, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        1, 1, 1, 1, 1,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",     -1, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 5.3
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         3, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        2, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       1, 2, 2, 1, 2,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",    -1, 0, 4, 5, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",    -1, 0, S, 4, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",    -1, 0, S, 3, 4,"\x00\x00\x20\x01"},

                                                                        // 5.4
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         4, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        3, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       2, 1, 2, 2, 2,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      1, 2, 2, 1, 2,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     0, 2, 1, 0, 1,"\x00"            },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",   -1, 0, 1, 5, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 5.5
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         5, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        4, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       3, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      2, 2, 3, 2, 3,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     1, 2, 2, 1, 2,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     1, 2, 2, 1, 2,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     1, 2, 2, 1, 2,"\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    0, 2, 1, 0, 1,"\x00"            },

                                                                        // 5.6
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         6, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        5, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       4, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      3, 1, 3, 3, 3,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     2, 3, 4, 2, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     2, 2, S, 2, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     2, 1, S, 2, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    1, 2, 2, 1, 2,"\x00\x00"        },

                                                                        // 5.7
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         7, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        6, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       5, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      4, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     3, 2, 4, 3, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     3, 1, S, 3, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     3, 0, S, 3, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    2, 3, 4, 2, 4,"\x00\x00\x20\x00"},

                                                                        // 5.8
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         8, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        7, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       6, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      5, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     4, 1, 4, 4, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     4, 0, S, 4, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     4, 0, S, 3, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    3, 2, 1, 3, 5,"\x00\x00\x20\x00"},

                                                                        // 5.9
{ L_, "AAAg",     1, 2, 4, 1, 4, "",         9, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        8, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       7, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      6, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     5, 0, 4, 5, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     5, 0, S, 4, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     5, 0, S, 3, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    4, 1, 1, 4, 5,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 5.10
{ L_, "AAAg",     1, 2, 4, 1, 4, "",        10, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",        9, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       8, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      7, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     6, 0, 4, 5, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     6, 0, S, 4, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     6, 0, S, 3, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    5, 0, 1, 5, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
    // Don't need 5.11 (similar to 5.10)
                                                                        // 5.11
{ L_, "AAAg",     1, 2, 4, 1, 4, "",        11, 0, 4, 2, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "A",       10, 0, 1, 2, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AA",       9, 0, 2, 3, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAA",      8, 0, 3, 4, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQ",     7, 0, 4, 5, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAE=",     7, 0, S, 4, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     1, 2, 4, 1, 4, "AQ==",     7, 0, S, 3, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     1, 2, 4, 1, 4, "AAAQA",    6, 0, 1, 5, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  6
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          7, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         6, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        5, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       4, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      3, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     2, 1, 4, 2, 4, N,         -1, 0, D, 1, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAgAQ",   0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAAgAAQ", -1, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },

// *** Verify '=' and '==' variants.
                                                                        // 6.-2
{ L_, "AAE=",     2, 0, S, 2, 4, N,         -1, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     2, 0, S, 1, 4, N,         -1, 0, D, 0, _,"\x01"            },

// *** Testing 'endConvert' following 'convert' with delayed output.
                                                                        // 6.-1
{ L_, "AAAg",     2, 1, 4, 2, 4, N,          0, 1, S, 0, _,"\x00\x00"        },
{ L_, "AAE=",     2, 0, S, 2, 4, N,          0, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     2, 0, S, 1, 4, N,          0, 0, D, 0, _,"\x01"            },

                                                                        // 6.0
{ L_, "AAAg",     2, 1, 4, 2, 4, N,          1, 0, D, 1, _,"\x00\x00\x20"    },
{ L_, "AAE=",     2, 0, S, 2, 4, N,          1, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     2, 0, S, 1, 4, N,          1, 0, D, 0, _,"\x01"            },

                                                                        // 6.1
{ L_, "AAAg",     2, 1, 4, 2, 4, N,          2, 0, D, 1, _,"\x00\x00\x20"    },
{ L_, "AAE=",     2, 0, S, 2, 4, N,          2, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     2, 0, S, 1, 4, N,          2, 0, D, 0, _,"\x01"            },

                                                                        // 6.2
{ L_, "AAAg",     2, 1, 4, 2, 4, N,          3, 0, D, 1, _,"\x00\x00\x20"    },
{ L_, "AAE=",     2, 0, S, 2, 4, N,          3, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     2, 0, S, 1, 4, N,          3, 0, D, 0, _,"\x01"            },

// *** Testing 'convert' following 'convert' with delayed output.
                                                                        // 6.-1
{ L_, "AAAg",     2, 1, 4, 2, 4, "",        -1, 0, 4, 1, 0,"\x00\x00\x20"    },

                                                                        // 6.0
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         0, 1, 4, 0, 0,"\x00"            },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",       -1, 0, 1, 1, 1,"\x00\x00\x20"    },

                                                                        // 6.1
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         1, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        0, 1, 1, 0, 1,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",      -1, 0, 2, 2, 2,"\x00\x00\x20\x00"},

                                                                        // 6.2
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         2, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        1, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       0, 2, 2, 0, 2,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",     -1, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 6.3
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         3, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        2, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       1, 1, 2, 1, 2,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      0, 2, 2, 0, 2,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",    -1, 0, 4, 4, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",    -1, 0, S, 3, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",    -1, 0, S, 2, 4,"\x00\x00\x20\x01"},

                                                                        // 6.4
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         4, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        3, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       2, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      1, 2, 3, 1, 3,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     0, 2, 2, 0, 2,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     0, 2, 2, 0, 2,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     0, 2, 2, 0, 2,"\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",   -1, 0, 1, 4, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 6.5
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         5, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        4, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       3, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      2, 1, 3, 2, 3,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     1, 3, 4, 1, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     1, 2, S, 1, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     1, 1, S, 1, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    0, 2, 2, 0, 2,"\x00\x00"        },

                                                                        // 6.6
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         6, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        5, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       4, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      3, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     2, 2, 4, 2, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     2, 1, S, 2, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     2, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    1, 3, 4, 1, 4,"\x00\x00\x20"    },

                                                                        // 6.7
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         7, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        6, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       5, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      4, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     3, 1, 4, 3, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     3, 0, S, 3, 4,"\x00\x00\x20\x00"
                                                           "\x01"            },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     3, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    2, 2, 1, 2, 5,"\x00\x00\x20\x00"},

                                                                        // 6.8
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         8, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        7, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       6, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      5, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     4, 0, 4, 4, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     4, 0, S, 3, 4,"\x00\x00\x20\x00"
                                                           "\x01"            },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     4, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    3, 1, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 6.9
{ L_, "AAAg",     2, 1, 4, 2, 4, "",         9, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        8, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       7, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      6, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     5, 0, 4, 4, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     5, 0, S, 3, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     5, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    4, 0, 1, 4, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
    // Don't need 6.10 and 6.11 (similar to 6.9)
                                                                        // 6.10
{ L_, "AAAg",     2, 1, 4, 2, 4, "",        10, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",        9, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       8, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      7, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     6, 0, 4, 4, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     6, 0, S, 3, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     6, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    5, 0, 1, 4, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 6.11
{ L_, "AAAg",     2, 1, 4, 2, 4, "",        11, 0, 4, 1, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "A",       10, 0, 1, 1, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AA",       9, 0, 2, 2, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAA",      8, 0, 3, 3, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQ",     7, 0, 4, 4, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAE=",     7, 0, S, 3, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     2, 1, 4, 2, 4, "AQ==",     7, 0, S, 2, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     2, 1, 4, 2, 4, "AAAQA",    6, 0, 1, 4, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  7
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          8, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         7, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        6, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       5, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      4, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     3, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    2, 1, 1, 2, 5, N,         -1,-1, E, 0, _,"\x00\x00"        },
{ L_, "AAAgAQ",   1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAgAAQ",  0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAAgAAAQ",-1, 0, 4, 6, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
// *** Verify '=' and '==' variants.
                                                                        // 7.-2
{ L_, "AAAgAAE=",-1, 0, S, 5, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAgAQ==",-1, 0, S, 4, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20\x01"},

// *** Testing 'endConvert' following 'convert' with delayed output.

                                                                        // 7.-1
{ L_, "AAAg",     3, 0, 4, 3, 4, N,          0, 0, D, 0, _,"\x00\x00\x20"    },
{ L_, "AAE=",     3, 0, S, 2, 4, N,          0, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     3, 0, S, 1, 4, N,          0, 0, D, 0, _,"\x01"            },
    // Don't need 7.0-7.2 (similar to 7.-1)
                                                                        // 7.0
{ L_, "AAAg",     3, 0, 4, 3, 4, N,          1, 0, D, 0, _,"\x00\x00\x20"    },
{ L_, "AAE=",     3, 0, S, 2, 4, N,          1, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     3, 0, S, 1, 4, N,          1, 0, D, 0, _,"\x01"            },

                                                                        // 7.1
{ L_, "AAAg",     3, 0, 4, 3, 4, N,          2, 0, D, 0, _,"\x00\x00\x20"    },
{ L_, "AAE=",     3, 0, S, 2, 4, N,          2, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     3, 0, S, 1, 4, N,          2, 0, D, 0, _,"\x01"            },

                                                                        // 7.2
{ L_, "AAAg",     3, 0, 4, 3, 4, N,          3, 0, D, 0, _,"\x00\x00\x20"    },
{ L_, "AAE=",     3, 0, S, 2, 4, N,          3, 0, D, 0, _,"\x00\x01"        },
{ L_, "AQ==",     3, 0, S, 1, 4, N,          3, 0, D, 0, _,"\x01"            },

// *** Testing 'convert' following 'convert' with delayed output.
                                                                        // 7.-1
{ L_, "AAAg",     3, 0, 4, 3, 4, "",        -1, 0, 4, 0, 0,"\x00\x00\x20"    },

                                                                        // 7.0
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         0, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",       -1, 0, 1, 0, 1,"\x00\x00\x20"    },

                                                                        // 7.1
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         1, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        0, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",      -1, 0, 2, 1, 2,"\x00\x00\x20\x00"},

                                                                        // 7.2
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         2, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        1, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       0, 1, 2, 0, 2,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",     -1, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 7.3
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         3, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        2, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       1, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      0, 2, 3, 0, 3,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",    -1, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",    -1, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",    -1, 0, S, 1, 4,"\x00\x00\x20\x01"},

                                                                        // 7.4
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         4, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        3, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       2, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      1, 1, 3, 1, 3,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     0, 3, 4, 0, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     0, 2, S, 0, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     0, 1, S, 0, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",   -1, 0, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 7.5
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         5, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        4, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       3, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      2, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     1, 2, 4, 1, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     1, 1, S, 1, 4,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     1, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    0, 3, 4, 0, 4,"\x00\x00\x20"    },

                                                                        // 7.6
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         6, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        5, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       4, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      3, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     2, 1, 4, 2, 4,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     2, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     2, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    1, 2, 1, 1, 5,"\x00\x00\x20\x00"},

                                                                        // 7.7
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         7, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        6, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       5, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      4, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     3, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     3, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     3, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    2, 1, 1, 2, 5,"\x00\x00\x20"
                                                           "\x00\x00"        },

                                                                        // 7.8
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         8, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        7, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       6, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      5, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     4, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     4, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     4, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    3, 0, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
    // Don't need cases 7.9-7.11 (all similar to 7.8)
                                                                        // 7.9
{ L_, "AAAg",     3, 0, 4, 3, 4, "",         9, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        8, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       7, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      6, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     5, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     5, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     5, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    4, 0, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 7.10
{ L_, "AAAg",     3, 0, 4, 3, 4, "",        10, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",        9, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       8, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      7, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     6, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     6, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     6, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    5, 0, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

                                                                        // 7.11
{ L_, "AAAg",     3, 0, 4, 3, 4, "",        11, 0, 4, 0, 0,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "A",       10, 0, 1, 0, 1,"\x00\x00\x20"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AA",       9, 0, 2, 1, 2,"\x00\x00\x20\x00"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAA",      8, 0, 3, 2, 3,"\x00\x00\x20"
                                                           "\x00\x00"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQ",     7, 0, 4, 3, 4,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAE=",     7, 0, S, 2, 4,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAg",     3, 0, 4, 3, 4, "AQ==",     7, 0, S, 1, 4,"\x00\x00\x20\x01"},
{ L_, "AAAg",     3, 0, 4, 3, 4, "AAAQA",    6, 0, 1, 3, 5,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  8
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          9, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         8, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        7, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       6, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      5, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     4, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    3, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },
{ L_, "AAAgAAQ",  1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAgAAAQ", 0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAAgAAAwQ",-1,0, 1, 6, 9, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x30"    },
// *** Verify '=' and '==' variants.
                                                                        // 8.-2
{ L_, "AAAgAAE=", 0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ==", 0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  9
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "",         9, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "Q",        8, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       7, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      6, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     5, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    4, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   3, 1, 2, 3, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAAQ",  2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },
{ L_, "AAAgAAAQ", 1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAgAAAwQ",0, 3, 4, 0, 4, N,         -1, 0, D, 3, _,"\x00\x00\x20"    },

// *** Verify '=' and '==' variants.
                                                                        // 9.-2
{ L_, "AAAgAAE=", 1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },
{ L_, "AAAgAQ==", 1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 10
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "Q",        9, 0, 1, 0, 1, N,         -1,-1, E, 0, _,""                },
{ L_, "AQ",       8, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      7, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     6, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    5, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   4, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  3, 2, 3, 3, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAAAQ", 2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },
{ L_, "AAAgAAAwQ",1, 2, 1, 1, 5, N,         -1,-1, E, 0, _,"\x00"            },

// *** Verify '=' and '==' variants.
                                                                       // 10.-2
{ L_, "AAAgAAE=", 2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },
{ L_, "AAAgAQ==", 2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 11
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AQ",       9, 0, 2, 1, 2, N,         -1,-1, E, 0, _,"\x01"            },
{ L_, "AAQ",      8, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     7, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    6, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   5, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  4, 1, 3, 4, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20\x00"},
{ L_, "AAAgAAAQ", 3, 3, 4, 3, 8, N,         -1, 0, D, 3, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",2, 2, 2, 2, 6, N,         -1,-1, E, 0, _,"\x00\x00"        },

// *** Verify '=' and '==' variants.
                                                                       // 11.-2
{ L_, "AAAgAAE=", 3, 2, S, 3, 8, N,         -1, 0, D, 2, _,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAgAQ==", 3, 1, S, 3, 8, N,         -1, 0, D, 1, _,"\x00\x00\x20\x01"},

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 12
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AAQ",      9, 0, 3, 2, 3, N,         -1,-1, E, 0, _,"\x00\x04"        },
{ L_, "AAAQ",     8, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    7, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   6, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  5, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },
{ L_, "AAAgAAAQ", 4, 2, 4, 4, 8, N,         -1, 0, D, 2, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",3, 3, 4, 3, 8, N,         -1, 0, D, 3, _,"\x00\x00\x20"
                                                           "\x00\x00\x30"    },
// *** Verify '=' and '==' variants.
                                                                       // 12.-2
{ L_, "AAAgAAE=", 4, 1, S, 4, 8, N,         -1, 0, D, 1, _,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAgAQ==", 4, 0, S, 4, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20\x01"},

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 13
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AAAQ",     9, 0, 4, 3, 4, N,         -1, 0, D, 0, _,"\x00\x00\x10"    },
{ L_, "AAAgQ",    8, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   7, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  6, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },
{ L_, "AAAgAAAQ", 5, 1, 4, 5, 8, N,         -1, 0, D, 1, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",4, 2, 1, 4, 9, N,         -1,-1, E, 0, _,"\x00\x00\x20\x00"},

// *** Verify '=' and '==' variants.
                                                                       // 13.-2
{ L_, "AAAgAAE=", 5, 0, S, 5, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x01"        },
{ L_, "AAAgAQ==", 5, 0, S, 4, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20\x01"},

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 14
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AAAgQ",    9, 0, 1, 3, 5, N,         -1,-1, E, 0, _,"\x00\x00\x20"    },
{ L_, "AAAgAQ",   8, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  7, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },
{ L_, "AAAgAAAQ", 6, 0, 4, 6, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",5, 1, 1, 5, 9, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x00"        },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 15
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AAAgAQ",   9, 0, 2, 4, 6, N,         -1,-1, E, 0, _,"\x00\x00\x20\x01"},
{ L_, "AAAgAAQ",  8, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },
{ L_, "AAAgAAAQ", 7, 0, 4, 6, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",6, 0, 1, 6, 9, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x30"    },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D = 16
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, "AAAgAAQ",  9, 0, 3, 5, 7, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x04"        },
{ L_, "AAAgAAAQ", 8, 0, 4, 6, 8, N,         -1, 0, D, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x10"    },
{ L_, "AAAgAAAwQ",7, 0, 1, 6, 9, N,         -1,-1, E, 0, _,"\x00\x00\x20"
                                                           "\x00\x00\x30"    },
//--------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int depth = -3;     // print depth in increasing order.

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;

                const char *const IN1    = DATA[ti].d_input1_p;
                const int         LIMIT1 = DATA[ti].d_limit1;
                const State       S1     = State(DATA[ti].d_state1);
                const int         R1     = DATA[ti].d_return1;
                const int         N_OUT1 = DATA[ti].d_numOut1;
                const int         N_IN1  = DATA[ti].d_numIn1;

                const char *const IN2    = DATA[ti].d_input2_p;
                const int         LIMIT2 = DATA[ti].d_limit2;
                const State       S2     = State(DATA[ti].d_state2);
                const int         R2     = DATA[ti].d_return2;
                const int         N_OUT2 = DATA[ti].d_numOut2;
                const int         N_IN2  = DATA[ti].d_numIn2;

                const char *const OUTPUT = DATA[ti].d_output_p;

                const int LEN1 = IN1 ? (int) strlen(IN1) : -1;
                const int LEN2 = IN2 ? (int) strlen(IN2) : -1;

                if (-1 == LEN1) LOOP_ASSERT(LINE, -1 == N_IN1);
                if (-1 != LEN1) LOOP_ASSERT(LINE, -1 != N_IN1);
                if (-1 == LEN2) LOOP_ASSERT(LINE, -1 == N_IN2);
                if (-1 != LEN2) LOOP_ASSERT(LINE, -1 != N_IN2);

                const int OUT_LEN = N_OUT1 + N_OUT2;

                const int OUTPUT_BUFFER_SIZE = 100; // overrun will be detected
                const int TRAILING_OUTPUT_WINDOW = 30; // detect extra output

                const bool MODE = false; // Relaxed Mode

                Obj obj(MODE);  // object under test.
                Obj obj1(MODE); // control for validating S1
                Obj obj2(MODE); // control for validating S2

                const int newDepth = LEN1 + LIMIT1;

                // The following partitions the table in verbose mode.

                if (newDepth != depth) {
                    if (verbose) cout << "\tDepth = " << newDepth << endl;
                    LOOP_ASSERT(LINE, newDepth > depth);
                    depth = newDepth;
                }

                if (veryVerbose) {
                    T_ T_ P_(LINE)
                    cout << "len 1 = ";
                    if (-1 != LEN1) cout << LEN1; else cout << "-";
                    cout << ", lim 1 = ";
                    if (-1 != LIMIT1) cout << LIMIT1; else cout << "-";
                    cout << ", len 2 = ";
                    if (-1 != LEN2) cout << LEN2; else cout << "-";
                    cout << ", lim 2 = ";
                    if (-1 != LIMIT2) cout << LIMIT2; else cout << "-";
                    cout << endl;

                    // P_(LEN1) P_(LIMIT1) P_(LEN2) P(LIMIT2)

                    if (veryVeryVerbose) {
                        T_ T_ T_
                        if (IN1) {
                            cout << "1. convert: ";
                            printCharN(cout, IN1, LEN1) << endl;
                        }
                        else {
                            cout << "1. endConvert." << endl;
                        }

                        T_ T_ T_
                        if (IN2) {
                            cout << "2. convert: ";
                            printCharN(cout, IN2, LEN2) << endl;
                        }
                        else {
                            cout << "2. endConvert." << endl;
                        }
                    }
                }

                // Define the output buffer and initialize it.

                char outputBuffer[OUTPUT_BUFFER_SIZE];
                memset(outputBuffer, '?', sizeof outputBuffer);

                // **  Prepare to process first 'convert' or 'endConvert'.  **

                int   totalOut = 0;
                int   totalIn = 0;
                char *b = outputBuffer;

                int result1;
                int nOut1 = -1;
                int nIn1 = -1;

                if (IN1) {
                    const char *const B = IN1, *const E = IN1 + LEN1;
                    result1 = obj.convert(b, &nOut1, &nIn1, B, E, LIMIT1);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  ni = -1;
                        int  r1 = obj1.convert(z, &no, &ni, B, E, LIMIT1);
                        int  r2 = obj2.convert(z, &no, &ni, B, E, LIMIT1);
                        LOOP3_ASSERT(LINE, result1, r1, result1 == r1);
                        LOOP3_ASSERT(LINE, result1, r2, result1 == r2);
                    }
                    totalOut += nOut1;
                    totalIn += nIn1;
                }
                else {
                    result1 = obj.endConvert(b, &nOut1, LIMIT1);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  r1 = obj1.endConvert(z, &no, LIMIT1);
                        int  r2 = obj2.endConvert(z, &no, LIMIT1);
                        LOOP3_ASSERT(LINE, result1, r1, result1 == r1);
                        LOOP3_ASSERT(LINE, result1, r2, result1 == r2);
                    }
                    totalOut += nOut1;
                }

                LOOP3_ASSERT(LINE, R1, result1, R1 == result1);
                LOOP2_ASSERT(LINE, S1, isState(&obj1, S1));
                LOOP3_ASSERT(LINE, N_OUT1, nOut1, N_OUT1 == nOut1);
                LOOP3_ASSERT(LINE, N_IN1, nIn1, N_IN1 == nIn1);

                // **  Prepare to process second 'convert' or 'endConvert'.  **

                b += totalOut;

                int result2;
                int nOut2 = -1;
                int nIn2 = -1;

                if (IN2) {
                    const char *const B = IN2, *const E = IN2 + LEN2;
                    result2 = obj.convert(b, &nOut2, &nIn2, B, E, LIMIT2);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  ni = -1;
                        int  r2 = obj2.convert(z, &no, &ni, B, E, LIMIT2);
                        LOOP3_ASSERT(LINE, result2, r2, result2 == r2);
                    }
                    totalOut += nOut2;
                    totalIn += nIn2;
                }
                else {
                    result2 = obj.endConvert(b, &nOut2, LIMIT2);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  r2 = obj2.endConvert(z, &no, LIMIT2);
                        LOOP3_ASSERT(LINE, result2, r2, result2 == r2);
                    }
                    totalOut += nOut2;
                }

                LOOP3_ASSERT(LINE, R2, result2, R2 == result2);
                LOOP2_ASSERT(LINE, S2, isState(&obj2, S2));
                LOOP3_ASSERT(LINE, N_OUT2, nOut2, N_OUT2 == nOut2);
                LOOP3_ASSERT(LINE, N_IN2, nIn2, N_IN2 == nIn2);


                // Verify combined output.

                if (veryVeryVerbose) {
                    cout << "\t\t\tExpected output: ";
                        printCharN(cout, OUTPUT, OUT_LEN) << endl;
                    cout << "\t\t\t  Actual output: ";
                        printCharN(cout, outputBuffer, totalOut) << endl;
                }
                LOOP_ASSERT(LINE, 0 == memcmp(OUTPUT, outputBuffer, OUT_LEN));

                // Verify nothing written past end of indicated output.
                {
                    int start = totalOut;
                    int extra = TRAILING_OUTPUT_WINDOW;
                    int end = myMin(start + extra, int(sizeof outputBuffer));
                    int last = start;
                    for (int i = last; i < end; ++i) {
                        if ('?' != outputBuffer[i]) {
                            last = i;
                        }
                    }
                    if (last != start) {
                        cout << "\t\t\t  Extended View: ";
                        printCharN(cout, outputBuffer, last + 1) << endl;
                    }
                    LOOP3_ASSERT(LINE, start, last, start == last)
                }

                // Detect output buffer overflow.

                LOOP_ASSERT(LINE, '?' == outputBuffer[sizeof outputBuffer - 2])
                LOOP_ASSERT(LINE, '?' == outputBuffer[sizeof outputBuffer - 1])
#if 0

                // ORTHOGONAL PERTURBATION:
                // For each index in [0, LEN1], partition the first input into
                // two sequences, apply these sequences, in turn, to a newly
                // created instance, and verify that the result is identical
                // to that of the original (unpartitioned) sequence.

                for (int index = 0; index <= LEN1; ++index) {
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(index) }

                    Obj  localObj(MODE);
                    char localBuf[sizeof outputBuffer];
                    memset(localBuf, '$', sizeof localBuf);
                    char *lb = localBuf;

                    // Prepare for first operation
                    int localNumIn = 0;
                    int localNumOut = 0;

                    if (IN1) {
                        const char *const B = IN1, *const E = IN1 + LEN1;
                        result1 = obj.convert(b, &nOut1, &nIn1, B, E, LIMIT1);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  ni = -1;
                        int  r1 = obj1.convert(z, &no, &ni, B, E, LIMIT1);
                        int  r2 = obj2.convert(z, &no, &ni, B, E, LIMIT1);
                        LOOP3_ASSERT(LINE, result1, r1, result1 == r1);
                        LOOP3_ASSERT(LINE, result1, r2, result1 == r2);
                    }
                    totalOut += nOut1;
                    totalIn += nIn1;
                }
                else {
                    result1 = obj.endConvert(b, &nOut1, LIMIT1);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int  no = -1;
                        int  r1 = obj1.endConvert(z, &no, LIMIT1);
                        int  r2 = obj2.endConvert(z, &no, LIMIT1);
                        LOOP3_ASSERT(LINE, result1, r1, result1 == r1);
                        LOOP3_ASSERT(LINE, result1, r2, result1 == r2);
                    }
                    totalOut += nOut1;
                }

                LOOP3_ASSERT(LINE, R1, result1, R1 == result1);
                LOOP2_ASSERT(LINE, S1, isState(&obj1, S1));
                LOOP3_ASSERT(LINE, N_OUT1, nOut1, N_OUT1 == nOut1);
                LOOP3_ASSERT(LINE, N_IN1, nIn1, N_IN1 == nIn1);



                    const char *const M = B + index;
                    char localBuf[sizeof outputBuffer];
                    memset(localBuf, '$', sizeof localBuf);
                    char *lb = localBuf;
                    int   localNumIn;
                    int   localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 1: ";
                        printCharN(cout, B, M - B) << endl;
                    }

                    localObj.convert(lb, &localNumOut, &localNumIn, B, M);

                    // Prepare for second call to 'convert'.
                    int localTotalIn = localNumIn;
                    int localTotalOut = localNumOut;
                    lb += localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 2: ";
                        printCharN(cout, M, E - M) << endl;
                    }

                    int res = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         M, E);
                    LOOP4_ASSERT(LINE, index, RES, res, RES == res);

                    // Prepare to apply the additional transition character.
                    localTotalIn += localNumIn;
                    localTotalOut += localNumOut;
                    lb += localNumOut;

                    int res2 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                       BB, EE);
                    LOOP4_ASSERT(LINE, index, RES2, res2, RES2 == res2);

                    // Prepare to call 'endConvert'.
                    localTotalIn  += localNumIn;
                    localTotalOut += localNumOut;
                    lb += localNumOut;

                    int res3 = localObj.endConvert(lb, &localNumOut);
                    LOOP4_ASSERT(LINE, index, RES3, res3, RES3 == res3);
                    localTotalOut += localNumOut;

                    // Compare internal output lengths.
                    const int localLen = localObj.outputLength();
            // -----^
            LOOP3_ASSERT(LINE, internalLen, localLen, internalLen == localLen);
            // -----v

                    // Confirm final state is the same as above.
    // -------------^
    LOOP3_ASSERT(LINE, index, FINAL_STATE, isState(&localObj, FINAL_STATE));
    // -------------v

                    // Verify total amount of input consumed is the same.
    // -------------^
    LOOP4_ASSERT(LINE, index, totalIn, localTotalIn, totalIn == localTotalIn);
    // -------------v

                    int cmpStatus = memcmp(outputBuffer, localBuf, totalOut);
                    if (cmpStatus || localTotalOut != totalOut ||
                                                         veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\tExpected output: ";
                        printCharN(cout, outputBuffer, totalOut) << endl;
                        cout << "\t\t\t\t\t  Actual output: ";
                        printCharN(cout, localBuf, localTotalOut) << endl;
                    }

// -----------------^
LOOP4_ASSERT(LINE, index, totalOut, localTotalOut, totalOut == localTotalOut);
// -----------------v
                    LOOP2_ASSERT(LINE, index, 0 == cmpStatus);

                    // Verify nothing written past end of indicated output.
                    {
                        int start = localTotalOut;
                        int extra = TRAILING_OUTPUT_WINDOW;
                        int end = myMin(start + extra, int(sizeof localBuf));
                        int last = start;
                        for (int i = last; i < end; ++i) {
                            if ('$' != localBuf[i]) {
                                last = i;
                            }
                        }
                        if (last != start) {
                            cout << "\t\t\t\t\t  Extended view: ";
                            printCharN(cout, localBuf, last + 1) << endl;
                        }
                        LOOP3_ASSERT(LINE, start, last, start == last)
                    }

                    // Detect local buffer overflow.
                    const int SIZE = sizeof outputBuffer;
                    LOOP2_ASSERT(LINE, index, '$' == localBuf[SIZE - 2]);
                    LOOP2_ASSERT(LINE, index, '$' == localBuf[SIZE - 1]);
                }
#endif

            } // end for ti

        } // end block
}

DEFINE_TEST_CASE(7)
{
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS WITH DEFAULT ARGUMENTS.
        //   Continue testing 'convert' and 'endConvert' with defaults
        //   arguments.
        //
        // Concerns:
        //   - That the conversion logic is correct in all places:
        //      + quanta of length 2
        //      + quanta of length 3
        //      + quanta of length 4
        //   - That unrecognized characters are ignored (only) in Relaxed mode.
        //   - That a relatively long input sequence is treated correctly.
        //   - That we distinguish States 2A and 2B in the complete table.
        //   - That we distinguish States 3A and 3B in the complete table.
        //   - That a longer input sequence is treated the same as any
        //      equivalent series of shorter subsequences supplied separately.
        //
        // Plan:
        //   - Using Category Partitioning, enumerate input sequences of
        //      increasing length to a sufficient *Depth* that all actions
        //      for each state in the FSM are exercised thoroughly.  In
        //      particular, partition the input category into equivalence
        //      classes and (in Relaxed mode) advance a *frontier* containing
        //      representatives from each equivalence class:
        //..
        //       EQUIV  AN
        //       CLASS ELEM  CHARACTERIZATION OF EQUIVALENCE CLASS
        //       ----- ----  ------------------------------------
        //        N2   'Q'   `010000` Can be followed by two (or one) equals.
        //        N1   'E'   `000100` Can be followed by one equal (state 3).
        //        N0   'V'   `010101` Numeric Char that can't precede an equal.
        //        E    '='   The 65th Base64 character, indicating padding.
        //        S    ' '   Ignorable in both Strict and Relaxed modes.
        //        U    '@'   Ignorable in Relaxed mode only.
        //        $     -    A call to 'endConvert'.
        //..
        //   - At each depth, repeat the exercise in Strict Mode.
        //
        //   - On a second pass, test that each bit position of an encoded
        //      quanta is decoded properly (for the first and subsequent one).
        //
        //   - As an Orthogonal Perturbation, partition each initial sequence
        //      into every possible pair of sequences and repeat the test.
        //      + For each pair of subsequences, verify that the end result is
        //         identical to that of the initial sequence.
        //
        // Tactics:
        //   - Category Partitioning and Depth-Ordered Enumeration Data
        //      Selection Methods
        //   - Table-Based Implementation Technique with Orthogonal
        //      Perturbation
        //
        // Testing:
        //   DFLT convert(char *o, int *no, int *ni, begin, end, int mno = -1);
        //   DFLT endConvert(char *out, int *numOut, int maxNumOut = -1);
        //
        //   That a 3-byte quantum is decoded properly.
        //   That a 2-byte quantum is decoded properly.
        //   That a 1-byte quantum is decoded properly.
        //
        //   That output length is calculated and stored properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "PRIMARY MANIPULATORS WITH DEFAULT ARGUMENTS" << endl
                  << "===========================================" << endl;

        EnabledGuard g(true); // If state is wrong, provide detailed diagnosis.

        // Major State Aliases:
        const int S = SOFT_DONE_STATE;
        const int Z = SAW_ONE_EQUAL;
        const int E = ERROR_STATE;

        if (verbose) cout << endl << "\nVerifying State Transitions." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number

                bool        d_strictMode;   // supplied to primary constructor
                const char *d_input_p;      // input characters (0 terminated)
                int         d_stateOne;     // state after input data
                char        d_char;         // transition character
                int         d_stateTwo;     // state after transition character

                bool        d_isError;      // if true ERROR_STATE else
                                            // DONE_STATE
                int         d_numOut;       // number of output characters
                int         d_numIn;        // number of input characters
                const char *d_output_p;     // expected output data
            } DATA[] = {
//--------------^

// *** DEPTH-ORDERED ENUMERATION: Depth = input length: state transitions ***

//    v--------INPUT-------------v v---------------OUTPUT-------------------v D
//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 0

{ L_,  0,"",             0, 'Q', 1, 1, 0, 1,""                               },
{ L_,  0,"",             0, 'E', 1, 1, 0, 1,""                               },
{ L_,  0,"",             0, 'V', 1, 1, 0, 1,""                               },
{ L_,  0,"",             0, '=', E, 1, 0, 1,""                               },
{ L_,  0,"",             0, ' ', 0, 0, 0, 1,""                               },
{ L_,  0,"",             0, '@', 0, 0, 0, 1,""                               },

//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 0

{ L_,  1,"",             0, 'Q', 1, 1, 0, 1,""                               },
{ L_,  1,"",             0, 'E', 1, 1, 0, 1,""                               },
{ L_,  1,"",             0, 'V', 1, 1, 0, 1,""                               },
{ L_,  1,"",             0, '=', E, 1, 0, 1,""                               },
{ L_,  1,"",             0, ' ', 0, 0, 0, 1,""                               },
{ L_,  1,"",             0, '@', E, 1, 0, 1,""                               },

//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 1
{ L_,  0,"Q",            1, 'Q', 2, 1, 1, 2,"\x41"                           },
{ L_,  0,"Q",            1, 'E', 2, 1, 1, 2,"\x40"                           },
{ L_,  0,"Q",            1, 'V', 2, 1, 1, 2,"\x41"                           },
{ L_,  0,"Q",            1, '=', E, 1, 0, 2,""                               },
{ L_,  0,"Q",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  0,"Q",            1, '@', 1, 1, 0, 2,""                               },

{ L_,  0,"E",            1, 'Q', 2, 1, 1, 2,"\x11"                           },
{ L_,  0,"E",            1, 'E', 2, 1, 1, 2,"\x10"                           },
{ L_,  0,"E",            1, 'V', 2, 1, 1, 2,"\x11"                           },
{ L_,  0,"E",            1, '=', E, 1, 0, 2,""                               },
{ L_,  0,"E",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  0,"E",            1, '@', 1, 1, 0, 2,""                               },

{ L_,  0,"V",            1, 'Q', 2, 1, 1, 2,"\x55"                           },
{ L_,  0,"V",            1, 'E', 2, 1, 1, 2,"\x54"                           },
{ L_,  0,"V",            1, 'V', 2, 1, 1, 2,"\x55"                           },
{ L_,  0,"V",            1, '=', E, 1, 0, 2,""                               },
{ L_,  0,"V",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  0,"V",            1, '@', 1, 1, 0, 2,""                               },
{ L_,  0,"=",            E, 'Q', E, 1, 0, 1,""                               },
{ L_,  0,"=",            E, 'E', E, 1, 0, 1,""                               },
{ L_,  0,"=",            E, 'V', E, 1, 0, 1,""                               },
{ L_,  0,"=",            E, '=', E, 1, 0, 1,""                               },
{ L_,  0,"=",            E, ' ', E, 1, 0, 1,""                               },
{ L_,  0,"=",            E, '@', E, 1, 0, 1,""                               },

{ L_,  0," ",            0, 'Q', 1, 1, 0, 2,""                               },
{ L_,  0," ",            0, 'E', 1, 1, 0, 2,""                               },
{ L_,  0," ",            0, 'V', 1, 1, 0, 2,""                               },
{ L_,  0," ",            0, '=', E, 1, 0, 2,""                               },
{ L_,  0," ",            0, ' ', 0, 0, 0, 2,""                               },
{ L_,  0," ",            0, '@', 0, 0, 0, 2,""                               },

{ L_,  0,"@",            0, 'Q', 1, 1, 0, 2,""                               },
{ L_,  0,"@",            0, 'E', 1, 1, 0, 2,""                               },
{ L_,  0,"@",            0, 'V', 1, 1, 0, 2,""                               },
{ L_,  0,"@",            0, '=', E, 1, 0, 2,""                               },
{ L_,  0,"@",            0, ' ', 0, 0, 0, 2,""                               },
{ L_,  0,"@",            0, '@', 0, 0, 0, 2,""                               },

//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 1
{ L_,  1,"Q",            1, 'Q', 2, 1, 1, 2,"\x41"                           },
{ L_,  1,"Q",            1, 'E', 2, 1, 1, 2,"\x40"                           },
{ L_,  1,"Q",            1, 'V', 2, 1, 1, 2,"\x41"                           },
{ L_,  1,"Q",            1, '=', E, 1, 0, 2,""                               },
{ L_,  1,"Q",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  1,"Q",            1, '@', E, 1, 0, 2,""                               },

{ L_,  1,"E",            1, 'Q', 2, 1, 1, 2,"\x11"                           },
{ L_,  1,"E",            1, 'E', 2, 1, 1, 2,"\x10"                           },
{ L_,  1,"E",            1, 'V', 2, 1, 1, 2,"\x11"                           },
{ L_,  1,"E",            1, '=', E, 1, 0, 2,""                               },
{ L_,  1,"E",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  1,"E",            1, '@', E, 1, 0, 2,""                               },

{ L_,  1,"V",            1, 'Q', 2, 1, 1, 2,"\x55"                           },
{ L_,  1,"V",            1, 'E', 2, 1, 1, 2,"\x54"                           },
{ L_,  1,"V",            1, 'V', 2, 1, 1, 2,"\x55"                           },
{ L_,  1,"V",            1, '=', E, 1, 0, 2,""                               },
{ L_,  1,"V",            1, ' ', 1, 1, 0, 2,""                               },
{ L_,  1,"V",            1, '@', E, 1, 0, 2,""                               },

{ L_,  1,"=",            E, 'Q', E, 1, 0, 1,""                               },
{ L_,  1,"=",            E, 'E', E, 1, 0, 1,""                               },
{ L_,  1,"=",            E, 'V', E, 1, 0, 1,""                               },
{ L_,  1,"=",            E, '=', E, 1, 0, 1,""                               },
{ L_,  1,"=",            E, ' ', E, 1, 0, 1,""                               },
{ L_,  1,"=",            E, '@', E, 1, 0, 1,""                               },

{ L_,  1," ",            0, 'Q', 1, 1, 0, 2,""                               },
{ L_,  1," ",            0, 'E', 1, 1, 0, 2,""                               },
{ L_,  1," ",            0, 'V', 1, 1, 0, 2,""                               },
{ L_,  1," ",            0, '=', E, 1, 0, 2,""                               },
{ L_,  1," ",            0, ' ', 0, 0, 0, 2,""                               },
{ L_,  1," ",            0, '@', E, 1, 0, 2,""                               },

{ L_,  1,"@",            E, 'Q', E, 1, 0, 1,""                               },
{ L_,  1,"@",            E, 'E', E, 1, 0, 1,""                               },
{ L_,  1,"@",            E, 'V', E, 1, 0, 1,""                               },
{ L_,  1,"@",            E, '=', E, 1, 0, 1,""                               },
{ L_,  1,"@",            E, ' ', E, 1, 0, 1,""                               },
{ L_,  1,"@",            E, '@', E, 1, 0, 1,""                               },

//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 2
{ L_,  0,"AQ",           2, 'Q', 3, 1, 2, 3,"\x01\x04"                       },
{ L_,  0,"AQ",           2, 'E', 3, 1, 2, 3,"\x01\x01"                       },
{ L_,  0,"AQ",           2, 'V', 3, 1, 2, 3,"\x01\x05"                       },
{ L_,  0,"AQ",           2, '=', Z, 1, 1, 3,"\x01"                           },
{ L_,  0,"AQ",           2, ' ', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"AQ",           2, '@', 2, 1, 1, 3,"\x01"                           },

{ L_,  0,"AE",           2, 'Q', 3, 1, 2, 3,"\x00\x44"                       },
{ L_,  0,"AE",           2, 'E', 3, 1, 2, 3,"\x00\x41"                       },
{ L_,  0,"AE",           2, 'V', 3, 1, 2, 3,"\x00\x45"                       },
{ L_,  0,"AE",           2, '=', E, 1, 1, 3,"\x00"                           },
{ L_,  0,"AE",           2, ' ', 2, 1, 1, 3,"\x00"                           },
{ L_,  0,"AE",           2, '@', 2, 1, 1, 3,"\x00"                           },

{ L_,  0,"AV",           2, 'Q', 3, 1, 2, 3,"\x01\x54"                       },
{ L_,  0,"AV",           2, 'E', 3, 1, 2, 3,"\x01\x51"                       },
{ L_,  0,"AV",           2, 'V', 3, 1, 2, 3,"\x01\x55"                       },
{ L_,  0,"AV",           2, '=', E, 1, 1, 3,"\x01"                           },
{ L_,  0,"AV",           2, ' ', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"AV",           2, '@', 2, 1, 1, 3,"\x01"                           },

{ L_,  0,"A=",           E, 'Q', E, 1, 0, 2,""                               },
{ L_,  0,"A=",           E, 'E', E, 1, 0, 2,""                               },
{ L_,  0,"A=",           E, 'V', E, 1, 0, 2,""                               },
{ L_,  0,"A=",           E, '=', E, 1, 0, 2,""                               },
{ L_,  0,"A=",           E, ' ', E, 1, 0, 2,""                               },
{ L_,  0,"A=",           E, '@', E, 1, 0, 2,""                               },

{ L_,  0,"A ",           1, 'Q', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"A ",           1, 'E', 2, 1, 1, 3,"\x00"                           },
{ L_,  0,"A ",           1, 'V', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"A ",           1, '=', E, 1, 0, 3,""                               },
{ L_,  0,"A ",           1, ' ', 1, 1, 0, 3,""                               },
{ L_,  0,"A ",           1, '@', 1, 1, 0, 3,""                               },

{ L_,  0,"A@",           1, 'Q', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"A@",           1, 'E', 2, 1, 1, 3,"\x00"                           },
{ L_,  0,"A@",           1, 'V', 2, 1, 1, 3,"\x01"                           },
{ L_,  0,"A@",           1, '=', E, 1, 0, 3,""                               },
{ L_,  0,"A@",           1, ' ', 1, 1, 0, 3,""                               },
{ L_,  0,"A@",           1, '@', 1, 1, 0, 3,""                               },

//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 2
{ L_,  1,"AQ",           2, 'Q', 3, 1, 2, 3,"\x01\x04"                       },
{ L_,  1,"AQ",           2, 'E', 3, 1, 2, 3,"\x01\x01"                       },
{ L_,  1,"AQ",           2, 'V', 3, 1, 2, 3,"\x01\x05"                       },
{ L_,  1,"AQ",           2, '=', Z, 1, 1, 3,"\x01"                           },
{ L_,  1,"AQ",           2, ' ', 2, 1, 1, 3,"\x01"                           },
{ L_,  1,"AQ",           2, '@', E, 1, 1, 3,"\x01"                           },

{ L_,  1,"AE",           2, 'Q', 3, 1, 2, 3,"\x00\x44"                       },
{ L_,  1,"AE",           2, 'E', 3, 1, 2, 3,"\x00\x41"                       },
{ L_,  1,"AE",           2, 'V', 3, 1, 2, 3,"\x00\x45"                       },
{ L_,  1,"AE",           2, '=', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AE",           2, ' ', 2, 1, 1, 3,"\x00"                           },
{ L_,  1,"AE",           2, '@', E, 1, 1, 3,"\x00"                           },

{ L_,  1,"AV",           2, 'Q', 3, 1, 2, 3,"\x01\x54"                       },
{ L_,  1,"AV",           2, 'E', 3, 1, 2, 3,"\x01\x51"                       },
{ L_,  1,"AV",           2, 'V', 3, 1, 2, 3,"\x01\x55"                       },
{ L_,  1,"AV",           2, '=', E, 1, 1, 3,"\x01"                           },
{ L_,  1,"AV",           2, ' ', 2, 1, 1, 3,"\x01"                           },
{ L_,  1,"AV",           2, '@', E, 1, 1, 3,"\x01"                           },

{ L_,  1,"A=",           E, 'Q', E, 1, 0, 2,""                               },
{ L_,  1,"A=",           E, 'E', E, 1, 0, 2,""                               },
{ L_,  1,"A=",           E, 'V', E, 1, 0, 2,""                               },
{ L_,  1,"A=",           E, '=', E, 1, 0, 2,""                               },
{ L_,  1,"A=",           E, ' ', E, 1, 0, 2,""                               },
{ L_,  1,"A=",           E, '@', E, 1, 0, 2,""                               },

{ L_,  1,"A ",           1, 'Q', 2, 1, 1, 3,"\x01"                           },
{ L_,  1,"A ",           1, 'E', 2, 1, 1, 3,"\x00"                           },
{ L_,  1,"A ",           1, 'V', 2, 1, 1, 3,"\x01"                           },
{ L_,  1,"A ",           1, '=', E, 1, 0, 3,""                               },
{ L_,  1,"A ",           1, ' ', 1, 1, 0, 3,""                               },
{ L_,  1,"A ",           1, '@', E, 1, 0, 3,""                               },

{ L_,  1,"A@",           E, 'Q', E, 1, 0, 2,""                               },
{ L_,  1,"A@",           E, 'E', E, 1, 0, 2,""                               },
{ L_,  1,"A@",           E, 'V', E, 1, 0, 2,""                               },
{ L_,  1,"A@",           E, '=', E, 1, 0, 2,""                               },
{ L_,  1,"A@",           E, ' ', E, 1, 0, 2,""                               },
{ L_,  1,"A@",           E, '@', E, 1, 0, 2,""                               },

//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 3
{ L_,  0,"AAQ",          3, 'Q', 4, 0, 3, 4,"\x00\x04\x10"                   },
{ L_,  0,"AAQ",          3, 'E', 4, 0, 3, 4,"\x00\x04\x04"                   },
{ L_,  0,"AAQ",          3, 'V', 4, 0, 3, 4,"\x00\x04\x15"                   },
{ L_,  0,"AAQ",          3, '=', S, 0, 2, 4,"\x00\x04"                       },
{ L_,  0,"AAQ",          3, ' ', 3, 1, 2, 4,"\x00\x04"                       },
{ L_,  0,"AAQ",          3, '@', 3, 1, 2, 4,"\x00\x04"                       },

{ L_,  0,"AAE",          3, 'Q', 4, 0, 3, 4,"\x00\x01\x10"                   },
{ L_,  0,"AAE",          3, 'E', 4, 0, 3, 4,"\x00\x01\x04"                   },
{ L_,  0,"AAE",          3, 'V', 4, 0, 3, 4,"\x00\x01\x15"                   },
{ L_,  0,"AAE",          3, '=', S, 0, 2, 4,"\x00\x01"                       },
{ L_,  0,"AAE",          3, ' ', 3, 1, 2, 4,"\x00\x01"                       },
{ L_,  0,"AAE",          3, '@', 3, 1, 2, 4,"\x00\x01"                       },

{ L_,  0,"AAV",          3, 'Q', 4, 0, 3, 4,"\x00\x05\x50"                   },
{ L_,  0,"AAV",          3, 'E', 4, 0, 3, 4,"\x00\x05\x44"                   },
{ L_,  0,"AAV",          3, 'V', 4, 0, 3, 4,"\x00\x05\x55"                   },
{ L_,  0,"AAV",          3, '=', E, 1, 2, 4,"\x00\x05"                       },
{ L_,  0,"AAV",          3, ' ', 3, 1, 2, 4,"\x00\x05"                       },
{ L_,  0,"AAV",          3, '@', 3, 1, 2, 4,"\x00\x05"                       },

{ L_,  0,"AA=",          Z, 'Q', E, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA=",          Z, 'E', E, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA=",          Z, 'V', E, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA=",          Z, '=', S, 0, 1, 4,"\x00"                           },
{ L_,  0,"AA=",          Z, ' ', Z, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA=",          Z, '@', Z, 1, 1, 4,"\x00"                           },

{ L_,  0,"AA ",          2, 'Q', 3, 1, 2, 4,"\x00\x04"                       },
{ L_,  0,"AA ",          2, 'E', 3, 1, 2, 4,"\x00\x01"                       },
{ L_,  0,"AA ",          2, 'V', 3, 1, 2, 4,"\x00\x05"                       },
{ L_,  0,"AA ",          2, '=', Z, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA ",          2, ' ', 2, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA ",          2, '@', 2, 1, 1, 4,"\x00"                           },

{ L_,  0,"AA@",          2, 'Q', 3, 1, 2, 4,"\x00\x04"                       },
{ L_,  0,"AA@",          2, 'E', 3, 1, 2, 4,"\x00\x01"                       },
{ L_,  0,"AA@",          2, 'V', 3, 1, 2, 4,"\x00\x05"                       },
{ L_,  0,"AA@",          2, '=', Z, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA@",          2, ' ', 2, 1, 1, 4,"\x00"                           },
{ L_,  0,"AA@",          2, '@', 2, 1, 1, 4,"\x00"                           },

//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 3
{ L_,  1,"AAQ",          3, 'Q', 4, 0, 3, 4,"\x00\x04\x10"                   },
{ L_,  1,"AAQ",          3, 'E', 4, 0, 3, 4,"\x00\x04\x04"                   },
{ L_,  1,"AAQ",          3, 'V', 4, 0, 3, 4,"\x00\x04\x15"                   },
{ L_,  1,"AAQ",          3, '=', S, 0, 2, 4,"\x00\x04"                       },
{ L_,  1,"AAQ",          3, ' ', 3, 1, 2, 4,"\x00\x04"                       },
{ L_,  1,"AAQ",          3, '@', E, 1, 2, 4,"\x00\x04"                       },

{ L_,  1,"AAE",          3, 'Q', 4, 0, 3, 4,"\x00\x01\x10"                   },
{ L_,  1,"AAE",          3, 'E', 4, 0, 3, 4,"\x00\x01\x04"                   },
{ L_,  1,"AAE",          3, 'V', 4, 0, 3, 4,"\x00\x01\x15"                   },
{ L_,  1,"AAE",          3, '=', S, 0, 2, 4,"\x00\x01"                       },
{ L_,  1,"AAE",          3, ' ', 3, 1, 2, 4,"\x00\x01"                       },
{ L_,  1,"AAE",          3, '@', E, 1, 2, 4,"\x00\x01"                       },

{ L_,  1,"AAV",          3, 'Q', 4, 0, 3, 4,"\x00\x05\x50"                   },
{ L_,  1,"AAV",          3, 'E', 4, 0, 3, 4,"\x00\x05\x44"                   },
{ L_,  1,"AAV",          3, 'V', 4, 0, 3, 4,"\x00\x05\x55"                   },
{ L_,  1,"AAV",          3, '=', E, 1, 2, 4,"\x00\x05"                       },
{ L_,  1,"AAV",          3, ' ', 3, 1, 2, 4,"\x00\x05"                       },
{ L_,  1,"AAV",          3, '@', E, 1, 2, 4,"\x00\x05"                       },

{ L_,  1,"AA=",          Z, 'Q', E, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA=",          Z, 'E', E, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA=",          Z, 'V', E, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA=",          Z, '=', S, 0, 1, 4,"\x00"                           },
{ L_,  1,"AA=",          Z, ' ', Z, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA=",          Z, '@', E, 1, 1, 4,"\x00"                           },

{ L_,  1,"AA ",          2, 'Q', 3, 1, 2, 4,"\x00\x04"                       },
{ L_,  1,"AA ",          2, 'E', 3, 1, 2, 4,"\x00\x01"                       },
{ L_,  1,"AA ",          2, 'V', 3, 1, 2, 4,"\x00\x05"                       },
{ L_,  1,"AA ",          2, '=', Z, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA ",          2, ' ', 2, 1, 1, 4,"\x00"                           },
{ L_,  1,"AA ",          2, '@', E, 1, 1, 4,"\x00"                           },

{ L_,  1,"AA@",          E, 'Q', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AA@",          E, 'E', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AA@",          E, 'V', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AA@",          E, '=', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AA@",          E, ' ', E, 1, 1, 3,"\x00"                           },
{ L_,  1,"AA@",          E, '@', E, 1, 1, 3,"\x00"                           },

//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 4
{ L_,  0,"AAAQ",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAAQ",         4, 'E', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAAQ",         4, 'V', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAAQ",         4, '=', E, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAAQ",         4, ' ', 4, 0, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAAQ",         4, '@', 4, 0, 3, 5,"\x00\x00\x10"                   },

{ L_,  0,"AAAE",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAAE",         4, 'E', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAAE",         4, 'V', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAAE",         4, '=', E, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAAE",         4, ' ', 4, 0, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAAE",         4, '@', 4, 0, 3, 5,"\x00\x00\x04"                   },

{ L_,  0,"AAAV",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAAV",         4, 'E', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAAV",         4, 'V', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAAV",         4, '=', E, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAAV",         4, ' ', 4, 0, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAAV",         4, '@', 4, 0, 3, 5,"\x00\x00\x15"                   },

{ L_,  0,"AAA=",         S, 'Q', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA=",         S, 'E', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA=",         S, 'V', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA=",         S, '=', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA=",         S, ' ', S, 0, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA=",         S, '@', S, 0, 2, 5,"\x00\x00"                       },

{ L_,  0,"AAA ",         3, 'Q', 4, 0, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAA ",         3, 'E', 4, 0, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAA ",         3, 'V', 4, 0, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAA ",         3, '=', S, 0, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA ",         3, ' ', 3, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA ",         3, '@', 3, 1, 2, 5,"\x00\x00"                       },

{ L_,  0,"AAA@",         3, 'Q', 4, 0, 3, 5,"\x00\x00\x10"                   },
{ L_,  0,"AAA@",         3, 'E', 4, 0, 3, 5,"\x00\x00\x04"                   },
{ L_,  0,"AAA@",         3, 'V', 4, 0, 3, 5,"\x00\x00\x15"                   },
{ L_,  0,"AAA@",         3, '=', S, 0, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA@",         3, ' ', 3, 1, 2, 5,"\x00\x00"                       },
{ L_,  0,"AAA@",         3, '@', 3, 1, 2, 5,"\x00\x00"                       },

//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 4
{ L_,  1,"AAAQ",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAAQ",         4, 'E', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAAQ",         4, 'V', 1, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAAQ",         4, '=', E, 1, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAAQ",         4, ' ', 4, 0, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAAQ",         4, '@', E, 1, 3, 5,"\x00\x00\x10"                   },

{ L_,  1,"AAAE",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAAE",         4, 'E', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAAE",         4, 'V', 1, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAAE",         4, '=', E, 1, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAAE",         4, ' ', 4, 0, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAAE",         4, '@', E, 1, 3, 5,"\x00\x00\x04"                   },

{ L_,  1,"AAAV",         4, 'Q', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAAV",         4, 'E', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAAV",         4, 'V', 1, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAAV",         4, '=', E, 1, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAAV",         4, ' ', 4, 0, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAAV",         4, '@', E, 1, 3, 5,"\x00\x00\x15"                   },

{ L_,  1,"AAA=",         S, 'Q', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA=",         S, 'E', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA=",         S, 'V', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA=",         S, '=', E, 1, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA=",         S, ' ', S, 0, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA=",         S, '@', E, 1, 2, 5,"\x00\x00"                       },

{ L_,  1,"AAA ",         3, 'Q', 4, 0, 3, 5,"\x00\x00\x10"                   },
{ L_,  1,"AAA ",         3, 'E', 4, 0, 3, 5,"\x00\x00\x04"                   },
{ L_,  1,"AAA ",         3, 'V', 4, 0, 3, 5,"\x00\x00\x15"                   },
{ L_,  1,"AAA ",         3, '=', S, 0, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA ",         3, ' ', 3, 1, 2, 5,"\x00\x00"                       },
{ L_,  1,"AAA ",         3, '@', E, 1, 2, 5,"\x00\x00"                       },

{ L_,  1,"AAA@",         E, 'Q', E, 1, 2, 4,"\x00\x00"                       },
{ L_,  1,"AAA@",         E, 'E', E, 1, 2, 4,"\x00\x00"                       },
{ L_,  1,"AAA@",         E, 'V', E, 1, 2, 4,"\x00\x00"                       },
{ L_,  1,"AAA@",         E, '=', E, 1, 2, 4,"\x00\x00"                       },
{ L_,  1,"AAA@",         E, ' ', E, 1, 2, 4,"\x00\x00"                       },
{ L_,  1,"AAA@",         E, '@', E, 1, 2, 4,"\x00\x00"                       },


  // *** DEPTH-ORDERED ENUMERATION: Depth = input length: decoding logic ***

//    v--------INPUT-------------v v---------------OUTPUT-------------------v D
//lin Mo Input Data     S1 Char S2 Er #o #i Output Data
//--- -- -------------- -- ---- -- -- -- -- --------------------------------- 0
{ L_,  0,"gA=",          Z, '=', S, 0, 1, 4,"\x80"                           },
{ L_,  0,"QA=",          Z, '=', S, 0, 1, 4,"\x40"                           },
{ L_,  0,"IA=",          Z, '=', S, 0, 1, 4,"\x20"                           },
{ L_,  0,"EA=",          Z, '=', S, 0, 1, 4,"\x10"                           },
{ L_,  0,"CA=",          Z, '=', S, 0, 1, 4,"\x08"                           },
{ L_,  0,"BA=",          Z, '=', S, 0, 1, 4,"\x04"                           },
{ L_,  0,"AA=",          Z, '=', S, 0, 1, 4,"\x00"                           },
{ L_,  0,"Ag=",          Z, '=', S, 0, 1, 4,"\x02"                           },
{ L_,  0,"AQ=",          Z, '=', S, 0, 1, 4,"\x01"                           },
{ L_,  0,"AA=",          Z, '=', S, 0, 1, 4,"\x00"                           },

{ L_,  0,"gAA",          3, '=', S, 0, 2, 4,"\x80\x00"                       },
{ L_,  0,"QAA",          3, '=', S, 0, 2, 4,"\x40\x00"                       },
{ L_,  0,"IAA",          3, '=', S, 0, 2, 4,"\x20\x00"                       },
{ L_,  0,"EAA",          3, '=', S, 0, 2, 4,"\x10\x00"                       },
{ L_,  0,"CAA",          3, '=', S, 0, 2, 4,"\x08\x00"                       },
{ L_,  0,"BAA",          3, '=', S, 0, 2, 4,"\x04\x00"                       },
{ L_,  0,"AgA",          3, '=', S, 0, 2, 4,"\x02\x00"                       },
{ L_,  0,"AQA",          3, '=', S, 0, 2, 4,"\x01\x00"                       },
{ L_,  0,"AIA",          3, '=', S, 0, 2, 4,"\x00\x80"                       },
{ L_,  0,"AEA",          3, '=', S, 0, 2, 4,"\x00\x40"                       },
{ L_,  0,"ACA",          3, '=', S, 0, 2, 4,"\x00\x20"                       },
{ L_,  0,"ABA",          3, '=', S, 0, 2, 4,"\x00\x10"                       },
{ L_,  0,"AAg",          3, '=', S, 0, 2, 4,"\x00\x08"                       },
{ L_,  0,"AAQ",          3, '=', S, 0, 2, 4,"\x00\x04"                       },
{ L_,  0,"AAI",          3, '=', S, 0, 2, 4,"\x00\x02"                       },
{ L_,  0,"AAE",          3, '=', S, 0, 2, 4,"\x00\x01"                       },
{ L_,  0,"AAA",          3, '=', S, 0, 2, 4,"\x00\x00"                       },

{ L_,  0,"gAA",          3, 'A', 4, 0, 3, 4,"\x80\x00\x00"                   },
{ L_,  0,"QAA",          3, 'A', 4, 0, 3, 4,"\x40\x00\x00"                   },
{ L_,  0,"IAA",          3, 'A', 4, 0, 3, 4,"\x20\x00\x00"                   },
{ L_,  0,"EAA",          3, 'A', 4, 0, 3, 4,"\x10\x00\x00"                   },
{ L_,  0,"CAA",          3, 'A', 4, 0, 3, 4,"\x08\x00\x00"                   },
{ L_,  0,"BAA",          3, 'A', 4, 0, 3, 4,"\x04\x00\x00"                   },
{ L_,  0,"AgA",          3, 'A', 4, 0, 3, 4,"\x02\x00\x00"                   },
{ L_,  0,"AQA",          3, 'A', 4, 0, 3, 4,"\x01\x00\x00"                   },
{ L_,  0,"AIA",          3, 'A', 4, 0, 3, 4,"\x00\x80\x00"                   },
{ L_,  0,"AEA",          3, 'A', 4, 0, 3, 4,"\x00\x40\x00"                   },
{ L_,  0,"ACA",          3, 'A', 4, 0, 3, 4,"\x00\x20\x00"                   },
{ L_,  0,"ABA",          3, 'A', 4, 0, 3, 4,"\x00\x10\x00"                   },
{ L_,  0,"AAg",          3, 'A', 4, 0, 3, 4,"\x00\x08\x00"                   },
{ L_,  0,"AAQ",          3, 'A', 4, 0, 3, 4,"\x00\x04\x00"                   },
{ L_,  0,"AAI",          3, 'A', 4, 0, 3, 4,"\x00\x02\x00"                   },
{ L_,  0,"AAE",          3, 'A', 4, 0, 3, 4,"\x00\x01\x00"                   },
{ L_,  0,"AAC",          3, 'A', 4, 0, 3, 4,"\x00\x00\x80"                   },
{ L_,  0,"AAB",          3, 'A', 4, 0, 3, 4,"\x00\x00\x40"                   },
{ L_,  0,"AAA",          3, 'g', 4, 0, 3, 4,"\x00\x00\x20"                   },
{ L_,  0,"AAA",          3, 'Q', 4, 0, 3, 4,"\x00\x00\x10"                   },
{ L_,  0,"AAA",          3, 'I', 4, 0, 3, 4,"\x00\x00\x08"                   },
{ L_,  0,"AAA",          3, 'E', 4, 0, 3, 4,"\x00\x00\x04"                   },
{ L_,  0,"AAA",          3, 'C', 4, 0, 3, 4,"\x00\x00\x02"                   },
{ L_,  0,"AAA",          3, 'B', 4, 0, 3, 4,"\x00\x00\x01"                   },
{ L_,  0,"AAA",          3, 'A', 4, 0, 3, 4,"\x00\x00\x00"                   },

{ L_,  1,"AAAAgA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x80"               },
{ L_,  1,"AAAAQA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x40"               },
{ L_,  1,"AAAAIA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x20"               },
{ L_,  1,"AAAAEA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x10"               },
{ L_,  1,"AAAACA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x08"               },
{ L_,  1,"AAAABA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x04"               },
{ L_,  1,"AAAAAA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x00"               },
{ L_,  1,"AAAAAg=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x02"               },
{ L_,  1,"AAAAAQ=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x01"               },
{ L_,  1,"AAAAAA=",      Z, '=', S, 0, 4, 8,"\x00\x00\x00\x00"               },

{ L_,  1,"AAAAgAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x80\x00"           },
{ L_,  1,"AAAAQAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x40\x00"           },
{ L_,  1,"AAAAIAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x20\x00"           },
{ L_,  1,"AAAAEAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x10\x00"           },
{ L_,  1,"AAAACAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x08\x00"           },
{ L_,  1,"AAAABAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x04\x00"           },
{ L_,  1,"AAAAAgA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x02\x00"           },
{ L_,  1,"AAAAAQA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x01\x00"           },
{ L_,  1,"AAAAAIA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x80"           },
{ L_,  1,"AAAAAEA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x40"           },
{ L_,  1,"AAAAACA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x20"           },
{ L_,  1,"AAAAABA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x10"           },
{ L_,  1,"AAAAAAg",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x08"           },
{ L_,  1,"AAAAAAQ",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x04"           },
{ L_,  1,"AAAAAAI",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x02"           },
{ L_,  1,"AAAAAAE",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x01"           },
{ L_,  1,"AAAAAAA",      3, '=', S, 0, 5, 8,"\x00\x00\x00\x00\x00"           },

{ L_,  1,"AAAAgAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x80\x00\x00"       },
{ L_,  1,"AAAAQAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x40\x00\x00"       },
{ L_,  1,"AAAAIAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x20\x00\x00"       },
{ L_,  1,"AAAAEAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x10\x00\x00"       },
{ L_,  1,"AAAACAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x08\x00\x00"       },
{ L_,  1,"AAAABAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x04\x00\x00"       },
{ L_,  1,"AAAAAgA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x02\x00\x00"       },
{ L_,  1,"AAAAAQA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x01\x00\x00"       },
{ L_,  1,"AAAAAIA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x80\x00"       },
{ L_,  1,"AAAAAEA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x40\x00"       },
{ L_,  1,"AAAAACA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x20\x00"       },
{ L_,  1,"AAAAABA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x10\x00"       },
{ L_,  1,"AAAAAAg",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x08\x00"       },
{ L_,  1,"AAAAAAQ",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x04\x00"       },
{ L_,  1,"AAAAAAI",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x02\x00"       },
{ L_,  1,"AAAAAAE",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x01\x00"       },
{ L_,  1,"AAAAAAC",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x80"       },
{ L_,  1,"AAAAAAB",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x40"       },
{ L_,  1,"AAAAAAA",      3, 'g', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x20"       },
{ L_,  1,"AAAAAAA",      3, 'Q', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x10"       },
{ L_,  1,"AAAAAAA",      3, 'I', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x08"       },
{ L_,  1,"AAAAAAA",      3, 'E', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x04"       },
{ L_,  1,"AAAAAAA",      3, 'C', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x02"       },
{ L_,  1,"AAAAAAA",      3, 'B', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x01"       },
{ L_,  1,"AAAAAAA",      3, 'A', 4, 0, 6, 8,"\x00\x00\x00\x00\x00\x00"       },
//--------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int depth = -1; // print each new depth once (increasing order)

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_lineNum;
                const bool        MODE    = DATA[ti].d_strictMode;
                const char *const INPUT   = DATA[ti].d_input_p;
                const State       S1      = State(DATA[ti].d_stateOne);
                const char        CHAR    = DATA[ti].d_char;
                const State       S2      = State(DATA[ti].d_stateTwo);

                const bool        ERROR   = DATA[ti].d_isError;
                const int         OUT_LEN = DATA[ti].d_numOut;
                const int         IN_LEN  = DATA[ti].d_numIn;
                const char *const OUTPUT  = DATA[ti].d_output_p;

                const int         LENGTH  = strlen(INPUT);
                const char *const B       = INPUT;
                const char *const E       = INPUT + LENGTH;

                const int OUTPUT_BUFFER_SIZE = 100; // overrun will be detected
                const int TRAILING_OUTPUT_WINDOW = 30; // detect extra output

                Obj obj(MODE);  // MODE ? STRICT : RELAXED.
                Obj obj1(MODE); // control for validating S1
                Obj obj2(MODE); // control for validating S2

                const int newDepth = LENGTH;

                // The following partitions the table in verbose mode.

                if (newDepth < depth) {
                    if (verbose) cout << "\nVerifying Decoding Logic." << endl;
                }
                if (newDepth != depth) {
                    if (verbose) cout << "\tDepth = " << newDepth << endl;
                    depth = newDepth;
                }
                if (veryVerbose) {
                    T_ T_ P_(ti)
                    P_(LINE)
                    cout << "INPUT = ";
                    printCharN(cout, INPUT, LENGTH) << ", CHAR = ";
                    printCharN(cout, &CHAR, 1) << endl;

                }

                // Define the output buffer and initialize it.

                char outputBuffer[OUTPUT_BUFFER_SIZE];
                memset(outputBuffer, '?', sizeof outputBuffer);

                // Prepare to invoke 'convert' on initial input.

                char     *b = outputBuffer;
                int       nOut = -1;
                int       nIn = -1;
                const int RES = obj.convert(b, &nOut, &nIn, B, E);
                int       totalOut = nOut;
                int       totalIn = nIn;
                b += nOut;
                {
                    char b[OUTPUT_BUFFER_SIZE];
                    int  nOut;
                    int  nIn;
                    obj1.convert(b, &nOut, &nIn, B, E); // update S1 control
                    obj2.convert(b, &nOut, &nIn, B, E); // update S2 control

                    // Verify state after initial input.
                    LOOP2_ASSERT(LINE, S1, isState(&obj1, S1));
                }

                // Now apply the additional transition character.

                const char *const BB = &CHAR, *const EE = BB + 1;
                nOut = -1;
                nIn = -1;
                const int RES2 = obj.convert(b, &nOut, &nIn, BB, EE);
                totalIn += nIn;
                totalOut += nOut;
                b += nOut;
                {
                    char b[OUTPUT_BUFFER_SIZE];
                    int  nOut;
                    int  nIn;
                    obj2.convert(b, &nOut, &nIn, BB, EE); // update S2 control

                    // Verify state after additional transition character.
                    LOOP2_ASSERT(LINE, S2, isState(&obj2, S2));
                }

                // Verify total input consumed.

                LOOP3_ASSERT(LINE, IN_LEN, totalIn, IN_LEN == totalIn);

                // Prepare to call 'endConvert'.

                nOut = -1;
                const int RES3 = obj.endConvert(b, &nOut);
                LOOP2_ASSERT(LINE, ERROR, -ERROR == RES3);
                totalOut += nOut;
                LOOP3_ASSERT(LINE, OUT_LEN, totalOut, OUT_LEN == totalOut);

                // Capture and verify internal output length.

                const int internalLen = obj.outputLength();
                LOOP2_ASSERT(LINE, internalLen, OUT_LEN == internalLen);

                // Confirm final state is 'DONE_STATE' or 'ERROR_STATE'.

                const int FINAL_STATE = ERROR ? ERROR_STATE : DONE_STATE;
                LOOP2_ASSERT(LINE, FINAL_STATE, isState(&obj, FINAL_STATE));

                if (veryVeryVerbose) {
                    cout << "\t\t\tExpected output: ";
                        printCharN(cout, OUTPUT, OUT_LEN) << endl;
                    cout << "\t\t\t  Actual output: ";
                        printCharN(cout, outputBuffer, totalOut) << endl;
                }
                LOOP_ASSERT(LINE, 0 == memcmp(OUTPUT, outputBuffer, OUT_LEN));

                // Verify nothing written past end of indicated output.
                {
                    int start = totalOut;
                    int extra = TRAILING_OUTPUT_WINDOW;
                    int end = myMin(start + extra, int(sizeof outputBuffer));
                    int last = start;
                    for (int i = last; i < end; ++i) {
                        if ('?' != outputBuffer[i]) {
                            last = i;
                        }
                    }
                    if (last != start) {
                        cout << "\t\t\t  Extended View: ";
                        printCharN(cout, outputBuffer, last + 1) << endl;
                    }
                    LOOP3_ASSERT(LINE, start, last, start == last)
                }

                // Detect output buffer overflow.

                LOOP_ASSERT(LINE, '?' == outputBuffer[sizeof outputBuffer - 2])
                LOOP_ASSERT(LINE, '?' == outputBuffer[sizeof outputBuffer - 1])

                // ORTHOGONAL PERTURBATION:

                // For each index in [0, LENGTH], partition the input into two
                // sequences, apply these sequences, in turn, to a newly
                // created instance, and verify that the result is identical to
                // that of the original (unpartitioned) sequence.

                for (int index = 0; index <= LENGTH; ++index) {
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(index) }

                    // Prepare for first call to 'convert'.

                    Obj               localObj(MODE);
                    const char *const M = B + index;
                    char              localBuf[sizeof outputBuffer];
                    memset(localBuf, '$', sizeof localBuf);
                    char             *lb = localBuf;
                    int               localNumIn;
                    int               localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 1: ";
                        printCharN(cout, B, M - B) << endl;
                    }

                    localObj.convert(lb, &localNumOut, &localNumIn, B, M);

                    // Prepare for second call to 'convert'.

                    int localTotalIn = localNumIn;
                    int localTotalOut = localNumOut;
                    lb += localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 2: ";
                        printCharN(cout, M, E - M) << endl;
                    }

                    int res = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         M, E);
                    LOOP4_ASSERT(LINE, index, RES, res, RES == res);

                    // Prepare to apply the additional transition character.

                    localTotalIn += localNumIn;
                    localTotalOut += localNumOut;
                    lb += localNumOut;

                    int res2 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                       BB, EE);
                    LOOP4_ASSERT(LINE, index, RES2, res2, RES2 == res2);

                    // Prepare to call 'endConvert'.

                    localTotalIn  += localNumIn;
                    localTotalOut += localNumOut;
                    lb += localNumOut;

                    int res3 = localObj.endConvert(lb, &localNumOut);
                    LOOP4_ASSERT(LINE, index, RES3, res3, RES3 == res3);
                    localTotalOut += localNumOut;

                    // Compare internal output lengths.

                    const int localLen = localObj.outputLength();
            // -----^
            LOOP3_ASSERT(LINE, internalLen, localLen, internalLen == localLen);
            // -----v

                    // Confirm final state is the same as above.
    // -------------^
    LOOP3_ASSERT(LINE, index, FINAL_STATE, isState(&localObj, FINAL_STATE));
    // -------------v

                    // Verify total amount of input consumed is the same.
    // -------------^
    LOOP4_ASSERT(LINE, index, totalIn, localTotalIn, totalIn == localTotalIn);
    // -------------v

                    int cmpStatus = memcmp(outputBuffer, localBuf, totalOut);
                    if (cmpStatus || localTotalOut != totalOut ||
                                                         veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\tExpected output: ";
                        printCharN(cout, outputBuffer, totalOut) << endl;
                        cout << "\t\t\t\t\t  Actual output: ";
                        printCharN(cout, localBuf, localTotalOut) << endl;
                    }

// -----------------^
LOOP4_ASSERT(LINE, index, totalOut, localTotalOut, totalOut == localTotalOut);
// -----------------v
                    LOOP2_ASSERT(LINE, index, 0 == cmpStatus);

                    // Verify nothing written past end of indicated output.
                    {
                        int start = localTotalOut;
                        int extra = TRAILING_OUTPUT_WINDOW;
                        int end = myMin(start + extra, int(sizeof localBuf));
                        int last = start;
                        for (int i = last; i < end; ++i) {
                            if ('$' != localBuf[i]) {
                                last = i;
                            }
                        }
                        if (last != start) {
                            cout << "\t\t\t\t\t  Extended view: ";
                            printCharN(cout, localBuf, last + 1) << endl;
                        }
                        LOOP3_ASSERT(LINE, start, last, start == last)
                    }

                    // Detect local buffer overflow.

                    const int SIZE = sizeof outputBuffer;
                    LOOP2_ASSERT(LINE, index, '$' == localBuf[SIZE - 2]);
                    LOOP2_ASSERT(LINE, index, '$' == localBuf[SIZE - 1]);
                }

            } // end for ti

        } // end block
}

DEFINE_TEST_CASE(6)
{
        // --------------------------------------------------------------------
        // VERIFY INTERNAL TABLES.
        //   Ensure that each internal table has the appropriate entries.
        //
        // Concerns:
        //   - That there is a typo in some internal table.
        //
        // Plan:
        //   Using a pseudo-piecewise-continuous implementation technique
        //   i.e., Loop-Based), provide an area test that will sample each of
        //   the table entires.  The goal is that if any table entry is bad,
        //   the test will fail.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Loop-Based and Table-Based Implementation Techniques
        //
        // Testing:
        //   That each internal table has no defective entires.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY INTERNAL TABLES" << endl
                          << "======================" << endl;

        if (verbose) cout <<
                "\nVerify is numeric Base64 char (256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                int  d_upto;     // indicated ending character code
                bool d_valid;    // true if expected member of set

            } DATA[] = {
                //lin  UpTo Valid
                //---  ---- -----
                { L_,  43,  false },
                { L_,  44,  true  }, // '+'
                { L_,  47,  false },
                { L_,  58,  true  }, // '/' and decimal digits
                { L_,  65,  false },
                { L_,  91,  true  }, // uppercase alphabet
                { L_,  97,  false },
                { L_, 123,  true  }, // lowercase alphabet
                { L_, 256,  false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int  INPUT_LENGTH = 4;
            const int  INPUT_INDEX  = 0;
            char       input[INPUT_LENGTH] = { '_', 'A', 'A', 'A' };

            const char *const B = input, *const E = B + INPUT_LENGTH;
            char              b[3];
            int               nOut;
            int               nIn;

            int end = 0;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int UPTO  = DATA[ti].d_upto;
                const int VALID = DATA[ti].d_valid;
                const int START = end;
                end = UPTO;

                LOOP3_ASSERT(LINE, end, START, end > START);

                if (veryVerbose) {
                    if (end - START > 1) {
                        cout << "\tVerify Entries ["
                             << START << '-' << end - 1 << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << START << "]." << endl;
                    }
                }

                for (int i = START; i < end; ++i) {
                    if (veryVeryVerbose) { T_ T_ P(i) }
                    input[INPUT_INDEX] = char(i);

                    Obj obj(false); // The mode is not relevant.

                    obj.convert(b, &nOut, &nIn, B, E);
                    LOOP2_ASSERT(LINE, i, VALID == (3 == nOut));
                    LOOP2_ASSERT(LINE, i, !VALID || 0 == (b[0] & 0x3));
                }
            }
            ASSERT(256 == end); // make sure all entires are accounted for.
        }

        if (verbose) cout <<
                "\nVerify ignorable chars/Strict mode (256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                int  d_upto;     // indicated ending character code
                bool d_valid;    // true if expected member of set

            } DATA[] = {
                //lin  UpTo  Valid
                //---  ----  -----
                { L_,  9,    false },
                { L_,  14,   true  }, // whitespace
                { L_,  32,   false },
                { L_,  33,   true  }, // space
                { L_, 256,   false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int INPUT_LENGTH = 1;
            const int INPUT_INDEX  = 0;
            char      input[INPUT_LENGTH] = { '_' };

            const char *const B = input, *const E = B + INPUT_LENGTH;

            char b[3];
            int  nOut;
            int  nIn;

            int end = 0;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int UPTO  = DATA[ti].d_upto;
                const int VALID = DATA[ti].d_valid;
                const int START = end;
                end = UPTO;

                LOOP3_ASSERT(LINE, end, START, end > START);

                if (veryVerbose) {
                    if (end - START > 1) {
                        cout << "\tVerify Entries ["
                             << START << '-' << end - 1 << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << START << "]." << endl;
                    }
                }

                for (int i = START; i < end; ++i) {
                    if (veryVeryVerbose) { T_ T_ P(i) }
                    input[INPUT_INDEX] = char(i);

                    Obj obj(true); // Strict Mode.

                    obj.convert(b, &nOut, &nIn, B, E);
                    bool stillInitial = isState(&obj, INITIAL_STATE);
                    LOOP2_ASSERT(LINE, i, VALID == stillInitial);
                }
            }
            ASSERT(256 == end); // make sure all entires are accounted for.
        }

        if (verbose) cout <<
                "\nVerify ignorable chars/Relaxed mode (256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                int  d_upto;     // indicated ending character code
                bool d_valid;    // true if expected member of set

            } DATA[] = {
                { L_,  43,  true  },
                { L_,  44,  false }, // '+'
                { L_,  47,  true  },
                { L_,  58,  false }, // '/' and decimal digits
                { L_,  61,  true  },
                { L_,  62,  false }, // '=' and decimal digits
                { L_,  65,  true  },
                { L_,  91,  false }, // uppercase alphabet
                { L_,  97,  true  },
                { L_, 123,  false }, // lowercase alphabet
                { L_, 256,  true  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int INPUT_LENGTH = 1;
            const int INPUT_INDEX  = 0;
            char      input[INPUT_LENGTH] = { '_' };

            const char *const B = input, *const E = B + INPUT_LENGTH;

            char b[3];
            int  nOut;
            int  nIn;

            int end = 0;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int UPTO  = DATA[ti].d_upto;
                const int VALID = DATA[ti].d_valid;
                const int START = end;
                end = UPTO;

                LOOP3_ASSERT(LINE, end, START, end > START);

                if (veryVerbose) {
                    if (end - START > 1) {
                        cout << "\tVerify Entries ["
                             << START << '-' << end - 1 << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << START << "]." << endl;
                    }
                }

                for (int i = START; i < end; ++i) {
                    if (veryVeryVerbose) { T_ T_ P(i) }
                    input[INPUT_INDEX] = char(i);

                    Obj obj(false); // Relaxed Mode.

                    obj.convert(b, &nOut, &nIn, B, E);
                    bool stillInitial = isState(&obj, INITIAL_STATE);
                    LOOP2_ASSERT(LINE, i, VALID == stillInitial);
                }
            }
            ASSERT(256 == end); // make sure all entires are accounted for.
        }

        if (verbose) cout <<
                "\nVerify chars before '=' in state 2 (256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                int  d_upto;     // indicated ending character code
                bool d_valid;    // true if expected member of set

            } DATA[] = {
                //lin  UpTo  Valid
                //---  ----  -----
                { L_,  65,   false },
                { L_,  66,   true  }, // 'A'
                { L_,  81,   false },
                { L_,  82,   true  }, // 'Q'
                { L_, 103,   false },
                { L_, 104,   true  }, // 'g'
                { L_, 119,   false },
                { L_, 120,   true  }, // 'w'
                { L_, 256,   false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int INPUT_LENGTH = 4;
            const int INPUT_INDEX  = 1;
            char      input[INPUT_LENGTH] = { 'A', '_', '=', '=' };

            const char *const B = input, *const E = B + INPUT_LENGTH;

            int nIn;
            int end = 0;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int UPTO  = DATA[ti].d_upto;
                const int VALID = DATA[ti].d_valid;
                const int START = end;
                end = UPTO;

                LOOP3_ASSERT(LINE, end, START, end > START);

                if (veryVerbose) {
                    if (end - START > 1) {
                        cout << "\tVerify Entries ["
                             << START << '-' << end - 1 << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << START << "]." << endl;
                    }
                }

                for (int i = START; i < end; ++i) {
                    if (veryVeryVerbose) { T_ T_ P(i) }
                    input[INPUT_INDEX] = char(i);

                    int  nOut = -1;
                    char b[1] = { -1 };
                    Obj  obj(false); // Do this test in Relaxed mode.

                    const int res = obj.convert(b, &nOut, &nIn, B, E);
                    if (VALID) {
                        LOOP3_ASSERT(LINE, i, res,  0 == res);
                        LOOP3_ASSERT(LINE, i, nOut, 1 == nOut);
                        LOOP3_ASSERT(LINE, i, b[0], 0 == (b[0] & 0xFC));
                    }
                    else if (1 == nOut) {
                        LOOP3_ASSERT(LINE, i, res, -1 == res);
                        LOOP3_ASSERT(LINE, i, b[0], 0 == (b[0] & 0xFC));
                    }
                    else {
                        LOOP3_ASSERT(LINE, i, res,  -1 == res);
                        LOOP3_ASSERT(LINE, i, b[0], (char)-1 == b[0]);
                    }
                }
            }
            ASSERT(256 == end); // make sure all entires are accounted for.
        }

        if (verbose) cout <<
                "\nVerify chars before '=' in state 3 (256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                int  d_upto;     // indicated ending character code
                bool d_valid;    // true if expected member of set

            } DATA[] = {
                //lin  UpTo  Valid
                //---  ----  -----
                { L_,  48,   false },
                { L_,  49,   true  }, // '0'
                { L_,  52,   false },
                { L_,  53,   true  }, // '4'
                { L_,  56,   false },
                { L_,  57,   true  }, // '8'

                { L_,  65,   false },
                { L_,  66,   true  }, // 'A'
                { L_,  69,   false },
                { L_,  70,   true  }, // 'E'
                { L_,  73,   false },
                { L_,  74,   true  }, // 'I'
                { L_,  77,   false },
                { L_,  78,   true  }, // 'M'
                { L_,  81,   false },
                { L_,  82,   true  }, // 'Q'
                { L_,  85,   false },
                { L_,  86,   true  }, // 'U'
                { L_,  89,   false },
                { L_,  90,   true  }, // 'Y'

                { L_,  99,   false },
                { L_, 100,   true  }, // 'c'
                { L_, 103,   false },
                { L_, 104,   true  }, // 'g'
                { L_, 107,   false },
                { L_, 108,   true  }, // 'k'
                { L_, 111,   false },
                { L_, 112,   true  }, // 'o'
                { L_, 115,   false },
                { L_, 116,   true  }, // 's'
                { L_, 119,   false },
                { L_, 120,   true  }, // 'w'

                { L_, 256,   false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int INPUT_LENGTH = 4;
            const int INPUT_INDEX  = 2;
            char      input[INPUT_LENGTH] = { 'A', 'A', '_', '=' };

            const char *const B = input, *const E = B + INPUT_LENGTH;

            int nIn;
            int end = 0;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int UPTO  = DATA[ti].d_upto;
                const int VALID = DATA[ti].d_valid;
                const int START = end;
                end = UPTO;

                LOOP3_ASSERT(LINE, end, START, end > START);

                if (veryVerbose) {
                    if (end - START > 1) {
                        cout << "\tVerify Entries ["
                             << START << '-' << end - 1 << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << START << "]." << endl;
                    }
                }

                for (int i = START; i < end; ++i) {
                    if (veryVeryVerbose) { T_ T_ P(i) }
                    input[INPUT_INDEX] = char(i);

                    int  nOut = -1;
                    char b[2] = { -1, -1 };
                    Obj  obj(true); // Do this test in Strict Mode.

                    const int res = obj.convert(b, &nOut, &nIn, B, E);
                    LOOP3_ASSERT(LINE, i, nOut,
                                            VALID == (2 == nOut && -1 != res));
                }
            }
            ASSERT(256 == end); // make sure all entires are accounted for.
        }

        if (verbose) cout <<
                "\nVerify encoded char index value (64/256 entries)." << endl;
        {
            static const struct {
                int  d_lineNum;  // source line number
                char d_from;    // starting character
                char d_to;      // ending character
                int  d_offset;  // value of FROM char and count so far

            } DATA[] = {
                //lin  From   To   Offset
                //---  ----  ----  ------
                { L_,  'A',  'Z',  0    },
                { L_,  'a',  'z',  26   },
                { L_,  '0',  '9',  52   },
                { L_,  '+',  '+',  62   },
                { L_,  '/',  '/',  63   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int INPUT_LENGTH = 4;
            const int INPUT_INDEX  = 3;
            char      input[INPUT_LENGTH] = { 'A', 'A', 'A', '_' };

            const char *const B = input, *const E = B + INPUT_LENGTH;

            int count = 0;      // Total count must be 64 when we are done.
            int oldOffset = -1; // Entries are ordered by increasing offset.

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE   = DATA[ti].d_lineNum;
                const char FROM  = DATA[ti].d_from;
                const char TO    = DATA[ti].d_to;
                const int  OFFSET = DATA[ti].d_offset;

                LOOP3_ASSERT(LINE, FROM, TO, TO >= FROM);
                LOOP3_ASSERT(LINE, OFFSET, oldOffset, OFFSET > oldOffset);
                LOOP3_ASSERT(LINE, OFFSET, count, OFFSET == count);

                if (veryVerbose) {
                    if (TO - FROM > 1) {
                        cout << "\tVerify ENTRIES ["
                             << FROM << '-' << TO << "]." << endl;
                    }
                    else {
                        cout << "\tVerify Entry [" << FROM << "]." << endl;
                    }
                }

                for (char c = FROM; c <= TO; ++c) {
                    ++count;
                    if (veryVeryVerbose) { T_ T_ P_(c) P(int(c)) }

                    char b[3] = { -1, -1, -1 };
                    int  nOut = -1;
                    int  nIn = -1;
                    input[INPUT_INDEX] = c;
                    Obj  obj(false); // Do test in Relaxed Mode.

                    const int res = obj.convert(b, &nOut, &nIn, B, E);

                    LOOP3_ASSERT(ti, c, res,       0 == res);
                    LOOP3_ASSERT(ti, c, nIn,       4 == nIn);
                    LOOP3_ASSERT(ti, c, nOut,      3 == nOut);
                    LOOP3_ASSERT(ti, c, int(b[0]), 0 == b[0]);
                    LOOP3_ASSERT(ti, c, int(b[1]), 0 == b[1]);
                    LOOP3_ASSERT(ti, c, int(b[2]), OFFSET+c-FROM == b[2]);
                }
            }
            ASSERT(64 == count);  // make sure all entires are accounted for.
        }
}

DEFINE_TEST_CASE(5)
{
        // --------------------------------------------------------------------
        // BOOTSTRAP: 'convert' - transitions
        //   Verify 'convert' transitions for all states.
        //
        // Concerns:
        //   - That we reach the correct final state after calling 'convert'.
        //   - That convert with empty input does not change state.
        //   - That convert with multiple inputs is identical to multiple
        //      calls with the same input.
        //   - That the return code is successful unless we wind up in the
        //      error state.
        //   - That the appropriate number of input characters are consumed.
        //
        // Plan:
        //   For inputs of increasing length starting with 0, put the object in
        //   each of the possible states and apply appropriate combinations of
        //   input character classes for the given input length.  After each
        //   call to 'convert', make sure that the return code, final state,
        //   and amount of input consumed are as expected.
        //
        // Tactics:
        //   - Category Partitioning and Depth-Ordered Enumeration Data
        //      Selection Methods
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //  BOOTSTRAP: 'convert' - transitions
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BOOTSTRAP: 'convert' - transitions" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\nVerify 'convert' - transitions." << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                int         d_startState;  // indicated starting state
                const char *d_input_p;     // input characters
                int         d_expNumIn;    // expected num input chars consumed
                int         d_endState;    // expected ending state

            } DATA[] = {//-----------input-------v  v---output-----v
                //lin  Starting State  Input Chars  nIn Ending State Depth = 0
                //---  --------------  -----------  --- ------------
                { L_,  INITIAL_STATE,  "",          0,  INITIAL_STATE   },
                { L_,  STATE_ONE,      "",          0,  STATE_ONE       },
                { L_,  STATE_TWO,      "",          0,  STATE_TWO       },
                { L_,  STATE_THREE,    "",          0,  STATE_THREE     },
                { L_,  STATE_FOUR,     "",          0,  STATE_FOUR      },
                { L_,  DONE_STATE,     "",          0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"",          0,  SOFT_DONE_STATE },
                { L_,  SAW_ONE_EQUAL,  "",          0,  SAW_ONE_EQUAL   },
                { L_,  ERROR_STATE,    "",          0,  ERROR_STATE     },

                //lin  Starting State  Input Chars  nIn Ending State Depth = 1
                //---  --------------  -----------  --- ------------
                { L_,  INITIAL_STATE,  "A",         1,  STATE_ONE       },
                { L_,  STATE_ONE,      "A",         1,  STATE_TWO       },
                { L_,  STATE_TWO,      "A",         1,  STATE_THREE     },
                { L_,  STATE_THREE,    "A",         1,  STATE_FOUR      },
                { L_,  STATE_FOUR,     "A",         1,  STATE_ONE       },
                { L_,  DONE_STATE,     "A",         0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"A",         1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "A",         1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "A",         0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "=",         1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "=",         1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "=",         1,  SAW_ONE_EQUAL   },
                { L_,  STATE_THREE,    "=",         1,  SOFT_DONE_STATE },
                { L_,  STATE_FOUR,     "=",         1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "=",         0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"=",         1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "=",         1,  SOFT_DONE_STATE },
                { L_,  ERROR_STATE,    "=",         0,  ERROR_STATE     },

                //lin  Starting State  Input Chars  nIn Ending State Depth = 2
                //---  --------------  -----------  --- ------------
                { L_,  INITIAL_STATE,  "AA",        2,  STATE_TWO       },
                { L_,  STATE_ONE,      "AA",        2,  STATE_THREE     },
                { L_,  STATE_TWO,      "AA",        2,  STATE_FOUR      },
                { L_,  STATE_THREE,    "AA",        2,  STATE_ONE       },
                { L_,  STATE_FOUR,     "AA",        2,  STATE_TWO       },
                { L_,  DONE_STATE,     "AA",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"AA",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "AA",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "AA",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "A=",        2,  ERROR_STATE     },
                { L_,  STATE_ONE,      "A=",        2,  SAW_ONE_EQUAL   },
                { L_,  STATE_TWO,      "A=",        2,  SOFT_DONE_STATE },
                { L_,  STATE_THREE,    "A=",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "A=",        2,  ERROR_STATE     },
                { L_,  DONE_STATE,     "A=",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"A=",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "A=",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "A=",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "=A",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "=A",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "=A",        2,  ERROR_STATE     },
                { L_,  STATE_THREE,    "=A",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "=A",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "=A",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"=A",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "=A",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "=A",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "==",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "==",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "==",        2,  SOFT_DONE_STATE },
                { L_,  STATE_THREE,    "==",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "==",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "==",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"==",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "==",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "==",        0,  ERROR_STATE     },

                //lin  Starting State  Input Chars  nIn Ending State Depth = 3
                //---  --------------  -----------  --- ------------
                { L_,  INITIAL_STATE,  "AAA",        3,  STATE_THREE     },
                { L_,  STATE_ONE,      "AAA",        3,  STATE_FOUR      },
                { L_,  STATE_TWO,      "AAA",        3,  STATE_ONE       },
                { L_,  STATE_THREE,    "AAA",        3,  STATE_TWO       },
                { L_,  STATE_FOUR,     "AAA",        3,  STATE_THREE     },
                { L_,  DONE_STATE,     "AAA",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"AAA",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "AAA",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "AAA",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "AA=",        3,  SAW_ONE_EQUAL   },
                { L_,  STATE_ONE,      "AA=",        3,  SOFT_DONE_STATE },
                { L_,  STATE_TWO,      "AA=",        3,  ERROR_STATE     },
                { L_,  STATE_THREE,    "AA=",        3,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "AA=",        3,  SAW_ONE_EQUAL   },
                { L_,  DONE_STATE,     "AA=",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"AA=",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "AA=",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "AA=",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "A=A",        2,  ERROR_STATE     },
                { L_,  STATE_ONE,      "A=A",        3,  ERROR_STATE     },
                { L_,  STATE_TWO,      "A=A",        3,  ERROR_STATE     },
                { L_,  STATE_THREE,    "A=A",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "A=A",        2,  ERROR_STATE     },
                { L_,  DONE_STATE,     "A=A",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"A=A",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "A=A",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "A=A",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "A==",        2,  ERROR_STATE     },
                { L_,  STATE_ONE,      "A==",        3,  SOFT_DONE_STATE },
                { L_,  STATE_TWO,      "A==",        3,  ERROR_STATE     },
                { L_,  STATE_THREE,    "A==",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "A==",        2,  ERROR_STATE     },
                { L_,  DONE_STATE,     "A==",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"A==",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "A==",        1,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "A==",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "=AA",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "=AA",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "=AA",        2,  ERROR_STATE     },
                { L_,  STATE_THREE,    "=AA",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "=AA",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "=AA",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"=AA",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "=AA",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "=AA",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "=A=",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "=A=",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "=A=",        2,  ERROR_STATE     },
                { L_,  STATE_THREE,    "=A=",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "=A=",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "=A=",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"=A=",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "=A=",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "=A=",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "==A",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "==A",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "==A",        3,  ERROR_STATE     },
                { L_,  STATE_THREE,    "==A",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "==A",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "==A",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"==A",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "==A",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "==A",        0,  ERROR_STATE     },

                { L_,  INITIAL_STATE,  "===",        1,  ERROR_STATE     },
                { L_,  STATE_ONE,      "===",        1,  ERROR_STATE     },
                { L_,  STATE_TWO,      "===",        3,  ERROR_STATE     },
                { L_,  STATE_THREE,    "===",        2,  ERROR_STATE     },
                { L_,  STATE_FOUR,     "===",        1,  ERROR_STATE     },
                { L_,  DONE_STATE,     "===",        0,  ERROR_STATE     },
                { L_,  SOFT_DONE_STATE,"===",        1,  ERROR_STATE     },
                { L_,  SAW_ONE_EQUAL,  "===",        2,  ERROR_STATE     },
                { L_,  ERROR_STATE,    "===",        0,  ERROR_STATE     },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char b[3];
            int  nOut;
            int  nIn;

            int lastInputLength = -1;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE       = DATA[ti].d_lineNum;
                const int         START      = DATA[ti].d_startState;
                const char *const INPUT      = DATA[ti].d_input_p;
                const int         EXP_NUM_IN = DATA[ti].d_expNumIn;
                const int         END        = DATA[ti].d_endState;
                const int         RTN        = ERROR_STATE == END
                                                ? DONE_STATE == START ? -2 : -1
                                                : 0;
                const int         LENGTH     = strlen(INPUT);
                const char *const B          = INPUT;
                const char *const E          = B + LENGTH;

                Obj obj(false);

                if (LENGTH != lastInputLength) {
                    if (verbose) cout << '\t' << LENGTH << " input character"
                                      << (1 == LENGTH ? "." : "s.") << endl;
                    lastInputLength = LENGTH;
                }
                setState(&obj, START);

                if (veryVerbose) cout << "\t\t" << STATE_NAMES[START] << endl;

                if (veryVeryVerbose) { cout
                    << "\t\t\tExpected end state: " << STATE_NAMES[END] << endl
                    << "\t\t\tExpected return status: " << RTN << endl;
                }

                LOOP_ASSERT(LINE, RTN == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(LINE, isState(&obj, END));
                LOOP2_ASSERT(LINE, nIn, EXP_NUM_IN == nIn);

            } // end for ti
        } // end block
}

DEFINE_TEST_CASE(4)
{
        // --------------------------------------------------------------------
        // BOOTSTRAP: 'endConvert' - transitions
        //   Verify 'endConvert' transitions for all states.
        //
        // Concerns:
        //   - Reaching the correct final state after calling 'endConvert'.
        //
        // Plan:
        //   Put the object in each state and verify the expected state is
        //   correct after the call to 'endConvert'.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //  BOOTSTRAP: 'endConvert' - transitions
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BOOTSTRAP: 'endConvert' - transitions" << endl
                          << "===================================" << endl;


        if (verbose) cout << "\nVerify 'endConvert' - transitions." << endl;
        {
            static const struct {
                int d_lineNum;          // source line number
                int d_startState;       // indicated starting state
                int d_endState;         // expected ending state

            } DATA[] = {//-input----v  v--output--v
                //lin  Starting State  Ending State
                //---  --------------  ------------
                { L_,  INITIAL_STATE,  DONE_STATE  },
                { L_,  STATE_ONE,      ERROR_STATE },
                { L_,  STATE_TWO,      ERROR_STATE },
                { L_,  STATE_THREE,    ERROR_STATE },
                { L_,  STATE_FOUR,     DONE_STATE  },
                { L_,  DONE_STATE,     ERROR_STATE },
                { L_,  SOFT_DONE_STATE,DONE_STATE  },
                { L_,  SAW_ONE_EQUAL,  ERROR_STATE },
                { L_,  ERROR_STATE,    ERROR_STATE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char b[3];
            int  numOut;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int START = DATA[ti].d_startState;
                const int END   = DATA[ti].d_endState;
                const int RTN = -(ERROR_STATE == END);

                Obj obj(false);

                if (verbose) cout << '\t' << STATE_NAMES[START] << '.' << endl;
                if (veryVerbose) cout <<
                       "\t\tExpected next state: " << STATE_NAMES[END] << endl;
                setState(&obj, START);

                if (veryVerbose) cout <<
                                 "\t\tExpected return status: " << RTN << endl;

                LOOP_ASSERT(LINE, RTN == obj.endConvert(b, &numOut));
                LOOP_ASSERT(LINE, isState(&obj, END));

            } // end for ti
        } // end block
}

DEFINE_TEST_CASE(3)
{
        // --------------------------------------------------------------------
        // SET-STATE, IS-STATE, AND BASIC ACCESSORS.
        //   Ensure that we can bring an object to any attainable state.
        //
        // Concerns:
        //   - That we can reach all internal states.
        //   - That we can confirm any anticipated state.
        //   - That each direct accessor is implemented properly.
        //
        // Plan:
        //   First make sure that when we are in the initial (i.e., newly
        //   constructed) state, 'isInitialState()' returns 'true' (so that
        //   the assertion that 'isInitialState()' does not fail before we have
        //   had an opportunity to test it explicitly).
        //
        //   Second, using the Brute-Force approach, reach each of the major
        //   states from a newly constructed object as simply as possible
        //   (i.e., by processing one character at a time) via the '::setState'
        //   test helper function.  Then use of all the (as yet untested)
        //   processing accessors to help verify the new state.  Additionally,
        //   call one or more manipulators to change the state (thereby
        //   creating a partial *Distinguishing* *Sequence*) in order to
        //   further verify that the targeted state was reached.
        //
        //   Third, prove that the '::isState' helper function (implemented
        //   using the above strategy) correctly confirms the indicated state
        //   by showing that, for each state, only the corresponding state
        //   setting returns true.
        //
        //   We can now declare the basic accessors to have been tested,
        //   mindful that the state values they return are calculated by the
        //   the primary manipulators that have not yet been fully tested.
        //
        // Tactics:
        //   - Ad-Hoc and Area Data Selection Methods
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   That we can reach each of the major processing states.
        //   void ::setState(Obj *obj, int state);
        //   bool ::istState(Obj *obj, int state);
        //
        //   bool isAcceptable() const;
        //   bool isDone() const;
        //   bool isError() const;
        //   bool isMaximal() const;
        //   bool isInitialState() const;
        //   int outputLength() const;
        //
        //   ~bdlde::Base64Encoder();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "SET-STATE, IS-STATE, RESET-STATE, AND BASIC ACCESSORS" << endl
            << "=====================================================" << endl;

        if (verbose) cout <<
                "\nMake sure we can detect the initial state." << endl;
        {
                if (verbose) cout << "\tINITIAL_STATE." << endl;

                Obj obj(true);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
        }


        if (verbose) cout << "\nVerify ::setState." << endl;
        {

            if (verbose) cout << "\tINITIAL_STATE." << endl;
            {
                Obj obj(true);
                setState(&obj, INITIAL_STATE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
            }

            if (verbose) cout << "\tState 1." << endl;
            {
                Obj obj(true);
                setState(&obj, STATE_ONE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char              b[3] = { -1, -1, -1 };
                int               numOut = -1;
                int               numIn = -1;
                const char        input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b, &numOut, &numIn, B, E);

                // State 2.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0  == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);

                result = obj.convert(b + 1, &numOut, &numIn, B, E);

                // State 3.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0  == b[0]);
                ASSERT(0  == b[1]);
                ASSERT((char)-1 == b[2]);

                result = obj.convert(b + 2, &numOut, &numIn, B, E);

                // State 4.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0 == b[0]);
                ASSERT(0 == b[1]);
                ASSERT(0 == b[2]);
            }

            if (verbose) cout << "\tState 2." << endl;
            {
                Obj obj(true);
                setState(&obj, STATE_TWO);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());

                char              b[3] = { 0, -1, -1 };
                int               numOut = -1;
                int               numIn = -1;
                const char        input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b + 1, &numOut, &numIn, B, E);

                // State 3.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0  == b[0]);
                ASSERT(0  == b[1]);
                ASSERT((char)-1 == b[2]);

                result = obj.convert(b + 2, &numOut, &numIn, B, E);

                // State 4.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0 == b[0]);
                ASSERT(0 == b[1]);
                ASSERT(0 == b[2]);
            }

            if (verbose) cout << "\tState 3." << endl;
            {
                Obj obj(true);
                setState(&obj, STATE_THREE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());

                char              b[3] = { 0, 0, -1 };
                int               numOut = -1;
                int               numIn = -1;
                const char        input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b + 2, &numOut, &numIn, B, E);

                // State 4.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT(0 == b[0]);
                ASSERT(0 == b[1]);
                ASSERT(0 == b[2]);
            }

            if (verbose) cout << "\tState 4." << endl;
            {
                Obj obj(true);
                setState(&obj, STATE_FOUR);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());

                char              b[3] = { -1, -1, -1 };
                int               numOut = -1;
                int               numIn = -1;
                char              input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b, &numOut, &numIn, B, E);

                // State 1.
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
            }

            if (verbose) cout << "\tDONE_STATE." << endl;
            {
                Obj obj(true);
                setState(&obj, DONE_STATE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char              b[3] = { -1, -1, -1 };
                int               numOut = -1;
                int               numIn = -1;
                const char        input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b, &numOut, &numIn, B, E);

                // ERROR_STATE
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-2 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
            }

            if (verbose) cout << "\tSOFT_DONE_STATE." << endl;
            {
                Obj obj(true);
                setState(&obj, SOFT_DONE_STATE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());  // "AAA="

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());  // "AAA="
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
                ASSERT((char)-1 == b[3]);

            }

            if (verbose) cout << "\tSAW_ONE_EQUAL." << endl;
            {
                Obj obj(true);
                setState(&obj, SAW_ONE_EQUAL);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int  numOut = -1;
                int  numIn = -1;

                const char        input = '=';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b, &numOut, &numIn, B, E);

                // SOFT_DONE_STATE
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength()); // "AA=="
                ASSERT(0 == result);
                ASSERT(0 == numOut); // "AA=="
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
            }

            if (verbose) cout << "\tERROR_STATE." << endl;
            {
                Obj obj(true);
                setState(&obj, ERROR_STATE);

                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int  numOut = -1;
                int  numIn = -1;

                const char        input = 'A';
                const char *const B = &input;
                const char *const E = B + 1;

                int result = obj.convert(b, &numOut, &numIn, B, E);

                // ERROR_STATE
                ASSERT(1 == obj.isUnrecognizedAnError());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isMaximal());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-1 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
            }
        }


        if (verbose) cout << "\nVerify ::isState." << endl;
        {
            Obj obj(0);
            for (int i = 0; i < NUM_STATES; ++i) {
                if (verbose) cout <<
                               "\tsetState: " << STATE_NAMES[i] << '.' << endl;

                for (int j = 0; j < NUM_STATES; ++j) {
                    if (veryVerbose) cout <<
                              "\t\tisState: " << STATE_NAMES[j] << '.' << endl;

                    const bool SAME = i == j;
                    if (veryVeryVerbose) { T_ T_ T_ P(SAME) }

                    EnabledGuard Guard(SAME); // Enable individual '::isState'
                                              // ASSERTs in order to facilitate
                                              // debugging.

                    Obj obj(0);
                    setState(&obj, i);
                    LOOP2_ASSERT(i, j, SAME == isState(&obj, j));
                }
            }
        }
}

DEFINE_TEST_CASE(2)
{
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR AND CONFIGURATION STATE ACCESSORS
        //   Make sure we can bring the object to any attainable state.
        //
        // Concerns:
        //   That we can fully configure the object from the constructor.
        //
        // Plan:
        //   Create the object in both configurations and verify using all of
        //   the (as yet untested) direct accessors.  After this test case, we
        //   can declare the accessors returning configuration state to be
        //   thoroughly tested.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlde::Base64Encoder(int unrecognizedIsErrorFlag);
        //   bool isUnrecognizedAnError() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTry both settings." << endl;

        if (verbose) cout << "\tunrecognizedIsErrorFlag = 'true'" << endl;
        {
            bdlde::Base64Decoder obj(true);
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(0 == obj.isMaximal());
            ASSERT(1 == obj.isInitialState());
            ASSERT(1 == obj.isUnrecognizedAnError());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tunrecognizedIsErrorFlag = 'false'" << endl;
        {
            Obj obj(false);
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(0 == obj.isMaximal());
            ASSERT(0 == obj.isUnrecognizedAnError());
            ASSERT(0 == obj.outputLength());
        }
}

DEFINE_TEST_CASE(1)
{
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is available to be used as a developers' sandbox.
        //
        // Concerns:
        //   None.
        //
        // Plan:
        //   Ad hoc.
        //
        // Tactics:
        //   - Ad Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   BREATHING TEST -- (developer's sandbox)
        //   ::myMin(const T& a, const T& b);
        //   ::printCharN(ostream& output, const char* sequence, int length)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (veryVerbose) {
            P_(int('+')); P((void*)'+');
            P_(int('/')); P((void*)'/');
            P_(int('=')); P((void*)'=');

            P((int)(signed char) '\xff');
            P((int)(unsigned char) '\xff');
            P((int)(char) '\xff');
        }

        if (verbose) cout << "\nTry '::myMin' test helper function." << endl;
        {
            ASSERT('a'== myMin('a', 'b'));
            ASSERT(-5 == myMin(3, -5));
            ASSERT(-3 == myMin(-3, 5));
        }

        if (verbose) cout << "\nTry '::printCharN' test helper function."
                                                                       << endl;
        {
            ostringstream out;

            const char in[] = "a" "\x00" "b" "\x07" "c" "\x08" "d" "\x0F"
                              "e" "\x10" "f" "\x80" "g" "\xFF";

            printCharN(out, in, sizeof in) << ends;

            const char EXP[] = "a<00>b<07>c<08>d<0F>e<10>f<80>g<FF><00>";

            if (veryVerbose) {
                cout << "\tRESULT = " << out.str().c_str() << endl;
                cout << "\tEXPECT = " << EXP << endl;
            }
            ASSERT(0 == strncmp(EXP, out.str().c_str(), sizeof EXP));
        }

        if (verbose) cout << "\nTry instantiating a decoder." << endl;
        {
            bdlde::Base64Decoder decoder(true);  // strict mode

            if (verbose) cout << "\nDecode the something." << endl;

            char out[1000];
            memset(out, '@', sizeof out);  // initialize to unusual char
            int  outIdx = 0;
            int  numIn = 0;
            int  numOut = 0;

            //              begin:    0   1  3   4   7   8  10     13   19END
            //                end:    1   3  4   7   8  10  13     19   20INPUT
            const char*const input = "A""AA""A""AAA""A""AA""AAA""AAAAA=""=";

            ASSERT(0 == numOut); ASSERT(0 == numIn); ASSERT(0 == outIdx);
            ASSERT('@' == out[0]); ASSERT('@' == out[1]);

            if (verbose) cout << "\tDecode: ^A." << endl;
            {
                const char *const begin = input + 0;
                const char *const end   = input + 1;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(0 == numOut); ASSERT(1 == numIn); ASSERT(0 == outIdx);
            ASSERT('@' == out[0]); ASSERT('@' == out[1]);

            if (verbose) cout << "\tDecode: A^AA." << endl;
            {
                const char *const begin = input + 1;
                const char *const end   = input + 3;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(2 == numOut); ASSERT(2 == numIn); ASSERT(2 == outIdx);
            ASSERT(0 == out[0]); ASSERT(0 == out[1]); ASSERT('@' == out[2]);

            if (verbose) cout << "\tDecode: AAA^A." << endl;
            {
                const char *const begin = input + 3;
                const char *const end   = input + 4;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(1 == numIn); ASSERT(3 == outIdx);
            ASSERT(0 == out[0]); ASSERT(0 == out[1]); ASSERT(0 == out[2]);
            ASSERT('@' == out[3]); ASSERT('@' == out[4]);

            if (verbose) cout << "\tDecode: AAAA^AAA." << endl;
            {
                const char *const begin = input + 4;
                const char *const end   = input + 7;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(2 == numOut); ASSERT(3 == numIn); ASSERT(5 == outIdx);
            ASSERT(0 == out[0]); ASSERT(0 == out[1]); ASSERT(0 == out[2]);
            ASSERT(0 == out[3]); ASSERT(0 == out[4]); ASSERT('@' == out[5]);

            if (verbose) cout << "\tDecode: AAAAAAA^A." << endl;
            {
                const char *const begin = input + 7;
                const char *const end   = input + 8;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(1 == numIn); ASSERT(6 == outIdx);
            ASSERT(0 == out[2]); ASSERT(0 == out[3]); ASSERT(0 == out[4]);
            ASSERT(0 == out[5]); ASSERT('@' == out[6]);

            if (verbose) cout << "\tDecode: AAAAAAAA^AA." << endl;
            {
                const char *const begin = input + 8;
                const char *const end   = input + 10;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(2 == numIn); ASSERT(7 == outIdx);
            ASSERT(0 == out[2]); ASSERT(0 == out[3]); ASSERT(0 == out[4]);
            ASSERT(0 == out[5]); ASSERT(0 == out[6]); ASSERT('@' == out[7]);

            if (verbose) cout << "\tDecode: AAAAAAAAAA^AAA." << endl;
            {
                const char *const begin = input + 10;
                const char *const end   = input + 13;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(2 == numOut); ASSERT(3 == numIn); ASSERT(9 == outIdx);
            ASSERT(0 == out[5]); ASSERT(0 == out[6]); ASSERT(0 == out[7]);
            ASSERT(0 == out[8]); ASSERT('@' == out[9]);

            if (verbose) cout << "\tDecode: AAAAAAAAAAAAA^AAAAA=." << endl;
            {
                const char *const begin = input + 13;
                const char *const end   = input + 19;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(4 == numOut); ASSERT(6 == numIn); ASSERT(13 == outIdx);
            ASSERT(0 == out[8]); ASSERT(0 == out[9]); ASSERT(0 == out[10]);
            ASSERT(0 == out[11]); ASSERT(0 == out[12]); ASSERT('@' == out[13]);

            if (verbose) cout << "\tDecode: AAAAAAAAAAAAAAAAAA=^=." << endl;
            {
                const char *const begin = input + 19;
                const char *const end   = input + 20;

                decoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(0 == numOut); ASSERT(1 == numIn); ASSERT(13 == outIdx);
            ASSERT(0 == out[9]); ASSERT(0 == out[10]); ASSERT(0 == out[11]);
            ASSERT(0 == out[12]); ASSERT('@' == out[13]);

            if (verbose) cout << "\tDecode: AAAAAAAAAAAAAAAAAA==^$." << endl;
            {
                decoder.endConvert(out + outIdx, &numOut);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "\n\t\t"
                    "output: \""; printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(0 == numOut); ASSERT(13 == outIdx);
            ASSERT(0 == out[9]); ASSERT(0 == out[10]); ASSERT(0 == out[11]);
            ASSERT(0 == out[12]); ASSERT('@' == out[13]);
        }

        if (verbose) cout <<
                        "\nEncode, Decode, and Compare with Original." << endl;

      }

#undef DEFINE_TEST_CASE
// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                          \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose,        \
                                                    veryVeryVeryVerbose); break

        CASE(11);
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
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
