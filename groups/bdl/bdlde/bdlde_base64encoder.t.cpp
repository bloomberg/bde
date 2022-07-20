// bdlde_base64encoder.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//                            U_ENABLE_DEPRECATIONS
//
// Set 'U_ENABLE_DEPRECATIONS' to 1 get warnings about uses of deprecated
// methods.  These warnings are quite voluminous.  Test case 14 will fail
// unless '0 == U_ENABLE_DEPRECATIONS' to make sure we don't ship with these
// warnings enabled.
// ----------------------------------------------------------------------------

#undef  U_ENABLE_DEPRECATIONS
#define U_ENABLE_DEPRECATIONS 0
#if U_ENABLE_DEPRECATIONS
# define BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING 1
# include <bsls_deprecatefeature.h>
#endif

#include <bdlde_base64encoder.h>

#include <bslim_fuzzutil.h>
#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>    // isgraph(), isalpha()
#include <bsl_cstdint.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // memset()
#include <bsl_climits.h>   // INT_MAX
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;
using bsl::size_t;
using bsl::ptrdiff_t;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This encoder is a kind of 'mechanism' that can be customized at
// construction.  Since there is currently no prevision to change the
// customization after construction, the choice for primary constructor must
// itself permit full customization:
//..
//  bdlde::Base64Encoder(int maxLineLength);
//..
// The primary manipulators are those needed to reach every attainable state
// from the initial state (i.e., that of a newly constructed object).  Clearly
// 'convert' is one, but 'endConvert' is also needed in order to reach the
// 'DONE' state.  Hence, the primary manipulators are
//..
//  int convert(char *out, int *numOut, int *ni, begin, end, int maxNumOut);
//  int endConvert(char *out, int *numOut, int maxNumOut);
//..
// The basic accessors for the encoder are all the functions that return
// information about the customization and/or execution state:
//..
//  bool isAcceptable() const;
//  bool isDone() const;
//  bool isError() const;
//  bool isInitialState() const;
//  int maxLineLength() const;
//  int outputLength() const;
//..
// The following tables characterize the major logical states and illustrates
// transitions for the primary manipulators (as implemented under the heading
// "Named STATE Values" in the "GLOBAL TYPEDEFS/CONSTANTS" section below):
//..
//  MAJOR STATE NAME       CHARACTERIZATION
//  ----------------       ----------------------------------------------------
// *INITIAL_STATE          nothing in accumulator; output length is 0
//  State 1                one char in accumulator
//  State 2                two chars in accumulator
// *State 3                nothing in accumulator; output length is NOT 0
// *DONE_STATE:            end of input noted
//  ERROR_STATE            error state (must be last)
//
// '*' = Accepting State
//                              convert (1)     endConvert
//                              -----------     ----------
//             *INITIAL_STATE:  State 1         DONE_STATE
//              State 1:        State 2         DONE_STATE
//              State 2:        State 3         DONE_STATE
//             *State 3:        State 1         DONE_STATE
//             *DONE_STATE:     ERROR_STATE     ERROR_STATE
//              ERROR_STATE:    ERROR_STATE     ERROR_STATE
//..
// Our first step will be to ensure that each of these states can be reached
// ('::setState'), that an anticipated state can be verified ('::isState'), and
// that each of the above state transitions is verified.  Next, we will ensure
// that each internal table is correct.  Then, using Category Partitioning, we
// enumerate a representative collection of inputs ordered by increasing
// *depth* that will be sufficient to prove that the logic associated with the
// state machine is performing as desired.
//
// Note that because the 'convert' and 'endConvert' methods are parametrized
// based on iterator types, we will want to ensure (at compile time) that
// their respective implementations do not depend on more than minimal iterator
// functionality.  We will accomplish this goal by supplying, as template
// arguments, 'bdeut::InputIterator' for 'convert' and 'bdeut::OutputIterator'
// for both of these template methods.
//-----------------------------------------------------------------------------
// [ 8] static int encodedLength(int numInputBytes, int maxLineLength);
// [11] bdlde::Base64Encoder(Alphabet alphabet);
// [ 2] bdlde::Base64Encoder(int maxLineLength, Alphabet alphabet);
// [ 3] ~bdlde::Base64Encoder();
// [ 9] int convert(char *o, int *no, int *ni, begin, end, int mno);
// [ 9] int endConvert(char *out, int *numOut, int maxNumOut);
// [ 8] int convert(char *o, int *no, int*ni, const char*b, const char*e);
// [ 8] int endConvert(char *out, int *numOut);
// [ 7] TEST MACHINERY
// [ 7] int LLVMFuzzerTestOneInput(const uint8_t *, size_t);
// [10] void resetState();
// [ 3] bool isAcceptable() const;
// [ 3] bool isDone() const;
// [ 3] bool isError() const;
// [ 3] bool isInitialState() const;
// [ 2] int maxLineLength() const;
// [ 3] int outputLength() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [15] USAGE EXAMPLE
// [14] 0 == U_ENABLE_DEPRECATIONS
// [ ?] That the input iterator can have *minimal* functionality.
// [ ?] That the output iterator can have *minimal* functionality.
// [ 1] ::myMin(const T& a, const T& b);
// [ 1] ::printCharN(ostream& output, const char* sequence, int length)
// [ 3] That we can reach each of the major processing states.
// [ 3] void ::setState(Obj *obj, int state, const char *input);
// [ 3] bool ::isState(Obj *obj, int state);
// [ 5] BOOTSTRAP: 'convert' - transitions
// [ 4] BOOTSTRAP: 'endConvert'- transitions
// [ 6] That each internal table has no defective entries.
// [ 8] DFLT convert(char *o, int *no, int *ni, begin, end, int mno = -1);
// [ 8] DFLT endConvert(char *out, int *numOut, int maxNumOut = -1);
// [ 8] That each bit of a 3-byte quantum finds its appropriate spot.
// [ 8] That each bit of a 2-byte quantum finds its appropriate spot.
// [ 8] That each bit of a 1-byte quantum finds its appropriate spot.
// [ 8] That output length is calculated properly.
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
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

#define V(X) { if (verbose) { cout << "\t" << X << endl; } } // no () to allow
                                                             // stringing <<
#define VV(X) { if (veryVerbose) { cout << "\t\t" << X << endl; } }
#define VVV(X) { if (veryVeryVerbose) { cout << "\t\t\t" << X << endl; } }
#define VVVV(X) { if (veryVeryVeryVerbose) {cout << "\t\t\t\t" << X << endl;} }

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                         GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

typedef bdlde::Base64Encoder        Obj;
typedef bdlde::Base64Alphabet       Alphabet;
typedef bdlde::Base64EncoderOptions EncoderOptions;
using bslim::FuzzDataView;
using bslim::FuzzUtil;

enum { k_DEFAULT_MAX_LINE_LENGTH = 76 };

                        // ==================
                        // Named STATE Values
                        // ==================
enum {
    // Enumeration of logical states described in the test plan overview.
    // These logical states must range in value from e_INITIAL_STATE = 0 to
    // e_ERROR_STATE = NUM_STATES - 1.  Note that the number and values of
    // these logical states need not coincide with those defined explicitly in
    // the implementation.

    e_INITIAL_STATE,
    e_STATE_ONE,
    e_STATE_TWO,
    e_STATE_THREE,
    e_DONE_STATE,
    e_ERROR_STATE    // must be last
};

// The following is a list of strings to be used when printing state values.

const char *STATE_NAMES[] = {
    "INITIAL_STATE",
    "State 1",
    "State 2",
    "State 3",
    "DONE_STATE",
    "ERROR_STATE"
};

const int NUM_STATES = sizeof STATE_NAMES / sizeof *STATE_NAMES;

char assertion[e_ERROR_STATE + 1 == NUM_STATES];

// The following is a very long text to use both in the stress test and in the
// usage example.

const char BLOOMBERG_NEWS[] =
"        (Commentary. Chet Currier is a Bloomberg News \n\
columnist. The opinions expressed are his own.) \n\
\n\
By Chet Currier\n\
     Sept. 14 (Bloomberg) -- The trouble with college finance in\n\
21st Century America is way too much homework.\n\
     Study up on UGMAs, ESAs, Section 529 savings programs that\n\
come in more than 50 different versions, and prepaid tuition\n\
plans. Learn when interest on U.S. Savings Bonds may, or may not,\n\
be exempt from taxes when the money is used to pay educational\n\
expenses.\n\
     Comb through some of the 117,687 items that turn up when you\n\
do a ``pay for college'' search on the Web site of Amazon.com.\n\
Read articles like ``Understanding the Aid Form,'' a discussion\n\
of something called the FAFSA that ran as part of a 28-page\n\
special ``Paying for College'' section in the Sept. 6 U.S. News &\n\
World Report.\n\
     ``A college education is a worthwhile, but expensive,\n\
investment,'' says the Vanguard Group in a current bulletin to\n\
investors in its $730 billion stable of mutual funds. ``The right\n\
savings plan can help ease the financial burden, but with the\n\
variety of alternatives available you may need a crash course on\n\
the pros and cons of each one.''\n\
     Please, life already has all the crash courses it needs. Is\n\
this the best we can do? Even if each piece of the system was\n\
designed with the best of intentions, the aggregate has become an\n\
absurd monstrosity.\n\
\n\
                         Fails the Test\n\
\n\
     The system's first and most obvious offense is its utter\n\
disregard for the virtues of simplicity, both aesthetic and\n\
practical. That's just the beginning.\n\
     It wastes huge amounts of time, energy and economic\n\
resources. Along the way, it undercuts several of the most\n\
important ideas colleges are supposed to be dealing in.\n\
     Consider the difference between real problems that naturally\n\
arise in life, and artificial obstacles that humans put in their\n\
own path. Dealing with real problems is an unavoidable part of\n\
the human condition -- and quite often turns out to be a\n\
productive exercise that gets you somewhere. Artificial problems,\n\
being both unnecessary and sterile, offer few benefits of that\n\
kind.\n\
     The wish to give young people an education, and the\n\
necessity of paying the cost of that effort, are classic examples\n\
of naturally occurring problems. When you study 529 plans, by\n\
contrast, you learn nothing but an arbitrary set of details that\n\
have no bearing on anything else.\n\
\n\
                          Hypocritical\n\
\n\
     College should teach ways of thinking that a student can\n\
apply and build on after the degree is awarded. The college-\n\
finance system is completely out of synch with that ideal.\n\
     In the few years while the teenagers in a family struggle\n\
through the miasma known as the ``admissions process,'' college-\n\
finance information presents itself as vital. Immediately after\n\
the last child matriculates, it becomes junk to be jettisoned as\n\
fast as the brain cells will allow.\n\
     Also, college ought to be a place to acquire a sense of\n\
ethics -- more now than ever amid the outcry over the moral and\n\
ethical failures of so many in business, government, religion and\n\
the press. Yet a basic part of college planning, usually\n\
presented without the slightest moral compunction, is learning\n\
the slyest ways to game the system.\n\
     Parents saving for a child's tuition are confronted with\n\
issues such as ``can that stash hurt your aid?'' to quote one\n\
headline in the U.S. News college-planning report. Here, son, you\n\
go study Plato and Aristotle, and we'll scrounge up the money to\n\
pay for it by hiding assets in Aunt Adelaide's name.\n\
\n\
                        Lessons to Learn\n\
\n\
     ``Mankind, left free, instinctively complicates life,'' said\n\
the writer Katharine Fullerton Gerould.\n\
     Beyond the portrait it paints of human folly, though, maybe\n\
there is something to be learned from the sorry state of college\n\
finance. It gives us a glimpse into the workings of a so-called\n\
``information economy'' where, instead of education steering us\n\
toward life, life steers us to education.\n\
     Like the computer, education was originally a tool, now is a\n\
product unto itself. Hence the resume of achievements and\n\
experiences that have come to be required of children aspiring to\n\
get into a ``selective'' college.\n\
     The economics of information is pretty new stuff. One sign\n\
we are starting to figure it out will come when we do a better\n\
job of designing mechanisms like college finance.\n\
\n\
--Editors: Ahearn, Wolfson\n\
\n\
Story Illustration: To graph increase in population of U.S.\n\
college graduates, click on  USP CL25 <Index> GP <GO> . To see\n\
additional Currier columns, click on  NI CURRIER <GO> . To\n\
comment on this column, click on  LETT <GO>  and send a letter to\n\
the editor.\n\
\n\
To contact the writer of this column: Chet Currier in New York\n\
(1) (212) 318-2605 or ccurrier@bloomberg.net.";

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace {
namespace u {

typedef bool (*CheckChar)(char);

bool isBasicRaw[256], *isBasicArray = isBasicRaw;
bool isUrlRaw  [256], *isUrlArray   = isUrlRaw;

struct InitRaw {
    InitRaw();
        // Initialize 'isBasicRaw' and 'isUrlRaw'.
} initRaw;

InitRaw::InitRaw()
{
    for (int ii = 0; ii < 256; ++ii) {
        const unsigned char uc = static_cast<unsigned char>(ii);

        isBasicArray[ii] = bsl::isalnum(uc);
        isUrlArray  [ii] = bsl::isalnum(uc);
    }

    isBasicArray['+'] = true;
    isBasicArray['/'] = true;

    isUrlArray['-']   = true;
    isUrlArray['_']   = true;
}

bool isBasic(char c)
{
    return isBasicArray[static_cast<unsigned char>(c)];
}

bool isBasicOrEquals(char c)
{
    return isBasicArray[static_cast<unsigned char>(c)] || '=' == c;
}

bool isBasicOrCrlf(char c)
{
    return isBasicArray[static_cast<unsigned char>(c)] || '\n' == c ||
                                                                     '\r' == c;
}

bool isBasicOrEqualsOrCrlf(char c)
{
    return isBasicArray[static_cast<unsigned char>(c)] || '=' == c ||
                                                        '\n' == c || '\r' == c;
}

bool isUrl(char c)
{
    return isUrlArray[static_cast<unsigned char>(c)];
}

bool isUrlOrEquals(char c)
{
    return isUrlArray[static_cast<unsigned char>(c)] || '=' == c;
}

bool isUrlOrCrlf(char c)
{
    return isUrlArray[static_cast<unsigned char>(c)] || '\n' == c || '\r' == c;
}

bool isUrlOrEqualsOrCrlf(char c)
{
    return isUrlArray[static_cast<unsigned char>(c)] || '=' == c ||
                                                        '\n' == c || '\r' == c;
}

bool checkRange(const EncoderOptions&  options,
                const char            *begin,
                const char            *end)
{
    BSLS_ASSERT(begin <= end);

    const bool noCrlf = 0 == options.maxLineLength();

    const CheckChar checkChar = Alphabet::e_BASIC == options.alphabet()
                              ? (options.isPadded()
                                 ? (noCrlf
                                    ? isBasicOrEquals
                                    : isBasicOrEqualsOrCrlf)
                                 : (noCrlf
                                    ? isBasic
                                    : isBasicOrCrlf))
                              : (options.isPadded()
                                 ? (noCrlf
                                    ? isUrlOrEquals
                                    : isUrlOrEqualsOrCrlf)
                                 : (noCrlf
                                    ? isUrl
                                    : isUrlOrCrlf));

    return end - begin == bsl::count_if(begin, end, checkChar);
}

inline
bool notBasicAndNotEqualsAndNotCrlf(char c)
    // Return 'true' if the specified 'c' is valid base 64 character or part of
    // a CRLF.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("+/=\n\r", uc));
}

inline
bool notBasicAndNotEquals(char c)
    // Return 'true' if the specified 'c' is not a valid base 64 character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("+/=", uc));
}

inline
bool equalsOrNotBasicAndNotCrlf(char c)
    // Return 'true' if the specified 'c' is '=' or not a valid base 64
    // character and not a CRLF character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("+/\n\r", uc));
}

inline
bool equalsOrCrlfOrNotBasic(char c)
    // Return 'true' if the specified 'c' is '=' or not a valid base 64
    // character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("+/", uc));
}

inline
bool notUrlAndNotEqualsAndNotCrlf(char c)
    // Return 'true' if the specified 'c' is valid base 64 character or part of
    // a CRLF.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("-_=\n\r", uc));
}

inline
bool notUrlAndNotEquals(char c)
    // Return 'true' if the specified 'c' is not a valid base 64 character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("-_=", uc));
}

inline
bool equalsOrNotUrlAndNotCrlf(char c)
    // Return 'true' if the specified 'c' is '=' or not a valid base 64
    // character and not a CRLF character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("-_\n\r", uc));
}

inline
bool equalsOrCrlfOrNotUrl(char c)
    // Return 'true' if the specified 'c' is '=' or not a valid base 64
    // character.
{
    unsigned char uc = c;
    return !(bsl::isalnum(uc) || bsl::strchr("-_", uc));
}

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

  public:
    // CREATORS
    explicit
    RandGen(int startSeed = 0);
        // Initialize the generator with the optionally specified 'startSeed'.

    // MANIPULATORS
    unsigned operator()();
        // Return the next random number in the series;

    char getChar();
        // Return a random 'char'.
};

// CREATORS
inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}

// MANIPULATORS
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

inline
char RandGen::getChar()
{
    return static_cast<char>((*this)() & 0xff);
}

}  // close namespace u
}  // close unnamed namespace

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

bsl::ostream& printCharN(bsl::ostream& output,
                         const char*   sequence,
                         size_t        length)
    // Print the specified character 'sequence' of specified 'length' to the
    // specified 'stream' and return a reference to the modifiable 'stream'
    // (if a character is not graphical, its hexadecimal code is printed
    // instead).  The behavior is undefined unless 0 <= 'length' and sequence
    // refers to a valid area of memory of size at least 'length'.
{
    static char HEX[] = "0123456789ABCDEF";

    for (unsigned i = 0; i < length; ++i) {
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

                        // ==================
                        // Function showCharN
                        // ==================

bsl::string showCharN(const char  *sequence,
                      bsl::size_t  length)
{
    bsl::ostringstream oss;
    printCharN(oss, sequence, static_cast<int>(length));

    return oss.str();
}

                        // =================
                        // Function setState
                        // =================

void setState(bdlde::Base64Encoder *object, int state)
    // Move the specified 'object' from its initial (i.e., newly constructed)
    // state to the specified 'state' using '\0' characters for input as
    // needed.  The behavior is undefined if 'object' is not in its
    // newly-constructed initial state.  Note that when this function is
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

    char b[8];
    int  numOut = -1;
    int  numIn = -1;

    const char        input = '\0';
    const char *const begin = &input;
    const char *const end = &input + 1;

    switch (state) {
      case e_INITIAL_STATE: {
        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(1 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case e_STATE_ONE: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 != object->outputLength());
      } break;
      case e_STATE_TWO: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 != object->outputLength());
      } break;
      case e_STATE_THREE: {
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(2 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 != object->outputLength()); // depends on maxLineLength
      } break;
      case e_DONE_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(1 == object->isAcceptable());
        ASSERT(1 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength()); // In this case, we know!
      } break;
      case e_ERROR_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(-1 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(0 == object->isAcceptable());
        ASSERT(0 == object->isDone());
        ASSERT(1 == object->isError());
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
        // Create a guard to control the activation of individual assertions in
        // the '::isState' test helper function using the specified enable
        // 'flag' value.  If 'flag' is 'true' individual false values we be
        // reported as assertion errors.
    : d_state(globalAssertsEnabled) { globalAssertsEnabled = flag; }

    ~EnabledGuard() { globalAssertsEnabled = d_state; }
};

bool isState(bdlde::Base64Encoder *object, int state)
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

    char b[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    int  numOut = -1;
    int  numIn = -1;

    const char        input = '\0';
    const char *const begin = &input;

    bool rv = false;

    switch (state) {
      case e_INITIAL_STATE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 1 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // e_DONE_STATE
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      case e_STATE_ONE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        object->convert(b, &numOut, &numIn, begin, begin);
        int result = object->endConvert(b, &numOut);

        // e_DONE_STATE
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = '=' != b[0];                          ASSERT(d0 || !enabled);
        bool d1 = '=' == b[1];                          ASSERT(d1 || !enabled);
        bool d2 = '=' == b[2];                          ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      case e_STATE_TWO: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        object->convert(b, &numOut, &numIn, begin, begin);
        int result = object->endConvert(b, &numOut);

        // e_DONE_STATE
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 2 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = '=' != b[0];                          ASSERT(d0 || !enabled);
        bool d1 = '=' == b[1];                          ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      case e_STATE_THREE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        object->convert(b, &numOut, &numIn, begin, begin);
        int result = object->endConvert(b, &numOut);

        // e_DONE_STATE
        bool b0 = 1 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      case e_DONE_STATE: {
        bool a0 = 1 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 1 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // e_ERROR_STATE
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      case e_ERROR_STATE: {
        bool a0 = 0 == object->isAcceptable();          ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 1 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // e_ERROR_STATE
        bool b0 = 0 == object->isAcceptable();          ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = (char)-1 == b[0];                     ASSERT(d0 || !enabled);
        bool d1 = (char)-1 == b[1];                     ASSERT(d1 || !enabled);
        bool d2 = (char)-1 == b[2];                     ASSERT(d2 || !enabled);
        bool d3 = (char)-1 == b[3];                     ASSERT(d3 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3;

      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
    return rv;
}

                        // ================================
                        // DUMMY Class u_Base64Decoder_Test
                        // for use in case 12 (stress test)
                        // ================================

namespace {

class u_Base64Decoder_Test {
    // This class implements a simple base64 decoder sufficient for this test
    // driver.  The interface and code is taken from the 'bdlde_base64decoder'
    // component.

    // PRIVATE TYPES
    enum {
        // Symbolic state values.

        e_ERROR_STATE      = -1, // input is irreparably invalid
        e_INPUT_STATE      =  0, // general input state
        e_NEED_EQUAL_STATE =  1, // need an '='
        e_SOFT_DONE_STATE  =  2, // only ignorable input and 'endConvert'
        e_DONE_STATE       =  3  // any additional input is an error
    };

    // CLASS DATA
    static const bool *const s_ignorableStrict_p; // Table identifying
                                                  // ignorable characters in
                                                  // strict mode
    static const bool *const s_ignorableRelaxed_p;// Table identifying
                                                  // ignorable characters in
                                                  // relaxed mode
    static const char *const s_decoding_p;        // a map from numeric Base64
                                                  // encoding characters to the
                                                  // corresponding 6-bit number

    // DATA
    const bool *const d_ignorable_p;   // Selected table of ignorable
                                       // characters based on specified
                                       // error-reporting mode

    signed char       d_state;         // -1 = error state
                                       //  0 = general input state
                                       //  1 = need another '='
                                       //  2 = soft done state - allow only
                                       //      ignorable input
                                       //  3 = done state - no more input
                                       //      allowed

    int               d_outputLength;  // total number of output characters

    unsigned int      d_stack;         // storage of non-emitted input

    int               d_bitsInStack;   // number of bits in 'd_stack'

  public:
    explicit
    u_Base64Decoder_Test(bool unrecognizedIsErrorFlag = true);
        // Create a Base64 decoder in the initial state.  Unrecognized
        // characters (i.e., non-base64 characters other than whitespace) will
        // be treated as errors if the optionally specified
        // 'unrecognizedIsErrorFlag' is 'true', and ignored otherwise.

    static int maxDecodedLength(int encodedLen);
        // Return the maximum number of decoded bytes that could result from an
        // input byte sequence of the specified 'inputLength' provided to the
        // 'convert' method of this decoder.  The behavior is undefined unless
        // 0 <= 'inputLength'.

    int convert(char       *out,
                int        *numOut,
                int        *numIn,
                const char *begin,
                const char *end,
                int         maxNumOut = -1);
        // Decode the sequence of input characters starting at the specified
        // 'begin' position up to, but not including, the specified 'end'
        // position, writing any resulting output characters to the specified
        // 'out' buffer.  Optionally specify the 'maxNumOut' limit on the
        // number of bytes to output; if 'maxNumOut' is negative, no limit is
        // imposed.  If the 'maxNumOut' limit is reached, no further input will
        // be consumed.  Load into the specified 'numOut' and 'numIn' the
        // number of output bytes produced and input bytes consumed,
        // respectively.  Return a non-negative value on success, -1 on an
        // input error, and -2 if the 'endConvert' method has already been
        // called without an intervening 'resetState' call.

    int endConvert(char* out, int *numOut, int maxNumOut = -1);
        // Terminate decoding for this decoder; write any retained output to
        // the specified 'out' buffer; encode any unprocessed input characters
        // that do not complete a 3-byte sequence.  Optionally specify the
        // 'maxNumOut' limit on the number of bytes to output; if 'maxNumOut'
        // is negative, no limit is imposed.  Load into the specified 'numOut'
        // the number of output bytes produced.  Return 0 on success, the
        // positive number of bytes *still* retained by this decoder if the
        // 'maxNumOut' limit was reached, and a negative value otherwise.
};

// CLASS DATA

// The following table identifies characters that can be ignored when
// d_isUnrecognizedAnErrorFlag 'true'.

static const bool CHARACTERS_THAT_CAN_BE_IGNORED_IN_STRICT_MODE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,  // 00  // whitespace
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20  // <space> char
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 80
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 90
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // A0
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // B0
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // C0
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // D0
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // E0
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // F0
};

// The following table identifies characters that can be ignored when
// d_isUnrecognizedAnErrorFlag 'false'.

static const bool CHARACTERS_THAT_CAN_BE_IGNORED_IN_RELAXED_MODE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 00
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0,  // 20  // '+', '/'
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1,  // 30  // '0'..'9', '='
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40  // uppercase
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,  // 50  //      alphabet
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60  // lowercase
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,  // 70  //      alphabet
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 80
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 90
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // A0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // B0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // C0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // D0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // E0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // F0
};

// The following table is a map from numeric Base64 encoding characters to the
// corresponding 6-bit index.

static const char DEC[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 00
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 10
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  // 20
       52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  // 30
       -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,  // 40
       15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,  // 50
       -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  // 60
       41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  // 70
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 80
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // 90
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // A0
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // B0
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // C0
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // D0
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // E0
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // F0
};

const bool *const u_Base64Decoder_Test::s_ignorableStrict_p =
                                 CHARACTERS_THAT_CAN_BE_IGNORED_IN_STRICT_MODE;
const bool *const u_Base64Decoder_Test::s_ignorableRelaxed_p =
                                CHARACTERS_THAT_CAN_BE_IGNORED_IN_RELAXED_MODE;
const char *const u_Base64Decoder_Test::s_decoding_p = DEC;

// CREATORS

u_Base64Decoder_Test::u_Base64Decoder_Test(bool unrecognizedIsErrorFlag)
: d_ignorable_p(unrecognizedIsErrorFlag ? s_ignorableStrict_p
                                        : s_ignorableRelaxed_p)
, d_state(e_INPUT_STATE)
, d_outputLength(0)
, d_stack(0)
, d_bitsInStack(0)
{
}

// ACCESSORS

int u_Base64Decoder_Test::maxDecodedLength(int encodedLen)
{
    return (encodedLen + 3) / 4 * 3;
}

int u_Base64Decoder_Test::convert(char       *out,
                                  int        *numOut,
                                  int        *numIn,
                                  const char *begin,
                                  const char *end,
                                  int         maxNumOut)
{
    if (e_ERROR_STATE == d_state || e_DONE_STATE == d_state) {
        int rv = e_DONE_STATE == d_state ? -2 : -1;
        d_state = e_ERROR_STATE;
        *numOut = 0;
        *numIn = 0;
        return rv;                                                    // RETURN
    }

    int numEmitted = 0;

    // Emit as many output bytes as possible.

    while (8 <= d_bitsInStack && numEmitted != maxNumOut) {
        d_bitsInStack -= 8;
        *out = static_cast<char>((d_stack >> d_bitsInStack) & 0xff);
        ++out;
        ++numEmitted;
    }

    // Consume as many input bytes as possible.

    *numIn = 0;

    if (e_INPUT_STATE == d_state) {
        while (18 >= d_bitsInStack && begin != end) {
            const unsigned char byte = *begin;

            ++begin;
            ++*numIn;

            unsigned char converted = static_cast<unsigned char>(
                                                           s_decoding_p[byte]);

            if (converted < 64) {
                d_stack = (d_stack << 6) | converted;
                d_bitsInStack += 6;
                if (8 <= d_bitsInStack && numEmitted != maxNumOut) {
                    d_bitsInStack -= 8;
                    *out = static_cast<char>((d_stack>>d_bitsInStack) & 0xff);
                    ++out;
                    ++numEmitted;
                }
            }
            else if (!d_ignorable_p[byte]) {
                if ('=' == byte) {
                    const int residualBits =
                                       (((d_outputLength + numEmitted) % 3) * 8
                                                         + d_bitsInStack) % 24;
                    if (12 == residualBits && 0 == (d_stack & 0xf)) {
                        d_stack = d_stack >> 4;
                        d_bitsInStack -= 4;
                        d_state = e_NEED_EQUAL_STATE;
                    }
                    else if (18 == residualBits && 0 == (d_stack & 0x3)) {
                        d_stack = d_stack >> 2;
                        d_bitsInStack -= 2;
                        d_state = e_SOFT_DONE_STATE;
                    }
                    else {
                        d_state = e_ERROR_STATE;
                    }
                }
                else {
                    d_state = e_ERROR_STATE;
                }
                break;
            }
        }
    }
    if (e_NEED_EQUAL_STATE == d_state) {
        while (begin != end) {
            const unsigned char byte = *begin;

            ++begin;
            ++*numIn;

            if (!d_ignorable_p[byte]) {
                if ('=' == byte) {
                    d_state = e_SOFT_DONE_STATE;
                }
                else {
                    d_state = e_ERROR_STATE;
                }
                break;
            }
        }
    }
    if (e_SOFT_DONE_STATE == d_state) {
        while (begin != end) {
            const unsigned char byte = *begin;

            ++begin;
            ++*numIn;

            if (!d_ignorable_p[byte]) {
                d_state = e_ERROR_STATE;
                break;
            }
        }
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;

    return e_ERROR_STATE == d_state ? -1 : d_bitsInStack / 8;
}

int u_Base64Decoder_Test::endConvert(char *out, int *numOut, int maxNumOut)
{
    BSLS_ASSERT(numOut);

    const int residualBits = ((d_outputLength % 3) * 8 + d_bitsInStack) % 24;
    if (e_ERROR_STATE == d_state || e_NEED_EQUAL_STATE == d_state ||
                    (e_DONE_STATE == d_state && 0 == d_bitsInStack) ||
                                (e_INPUT_STATE == d_state && residualBits)) {
        *numOut = 0;
        d_state = e_ERROR_STATE;
        return -1;                                                    // RETURN
    }

    d_state = e_DONE_STATE;

    int numEmitted = 0;
    while (8 <= d_bitsInStack && numEmitted != maxNumOut) {
        d_bitsInStack -= 8;
        *out = static_cast<char>((d_stack >> d_bitsInStack) & 0xff);
        ++out;
        ++numEmitted;
    }

    *numOut = numEmitted;
    d_outputLength += numEmitted;

    return e_ERROR_STATE == d_state ? -1 : d_bitsInStack / 8;
}

int run = 0;

namespace u {

typedef void (*Checker)(const char *, const char *, bsl::size_t);

void checkBasic(const char *begin, const char *end, bsl::size_t lineLength)
    // Check that the specfied sequence '[ begin, end )' is 100% valid base 64
    // characters, with CRLF's exactly where they're expected given the
    // specified 'lineLength'.
{
    if (0 == lineLength) {
        const char *preEqualsEnd = bsl::max(begin, end - 2);
        ASSERT(0 == bsl::count_if(begin,
                                  preEqualsEnd,
                                  &u::equalsOrCrlfOrNotBasic));
        ASSERT(0 == bsl::count_if(preEqualsEnd,
                                  end,
                                  &u::notBasicAndNotEquals));
    }
    else {
        const char *preEqualsEnd = bsl::max(begin, end - 4), *pc = begin;
        while (pc < preEqualsEnd) {
            const char *lineEnd = bsl::min(preEqualsEnd, pc + lineLength);
            ASSERT(0 == bsl::count_if(pc,
                                      lineEnd,
                                      &u::equalsOrCrlfOrNotBasic));
            pc = lineEnd;

            if (pc < preEqualsEnd) {
                ASSERTV(*pc, '\r' == *pc);
                ++pc;
            }
            if (pc < preEqualsEnd) {
                ASSERTV(*pc, '\n' == *pc);
                ++pc;
            }
        }
        ASSERT(0 == bsl::count_if(pc,
                                  end,
                                  &u::notBasicAndNotEqualsAndNotCrlf));
        if (begin < end) {
            ASSERTV(run, showCharN(begin, end - begin),
                                           '\r' != end[-1] && '\n' != end[-1]);
        }
    }
}

void checkUrl(const char *begin, const char *end, bsl::size_t lineLength)
    // Check that the specfied sequence '[ begin, end )' is 100% valid base 64
    // characters, with CRLF's exactly where they're expected given the
    // specified 'lineLength'.
{
    if (0 == lineLength) {
        const char *preEqualsEnd = bsl::max(begin, end - 2);
        ASSERT(0 == bsl::count_if(begin,
                                  preEqualsEnd,
                                  &u::equalsOrCrlfOrNotUrl));
        ASSERT(0 == bsl::count_if(preEqualsEnd,
                                  end,
                                  &u::notUrlAndNotEquals));
    }
    else {
        const char *preEqualsEnd = bsl::max(begin, end - 4), *pc = begin;
        while (pc < preEqualsEnd) {
            const char *lineEnd = bsl::min(preEqualsEnd, pc + lineLength);
            ASSERT(0 == bsl::count_if(pc,
                                      lineEnd,
                                      &u::equalsOrCrlfOrNotUrl));
            pc = lineEnd;

            if (pc < preEqualsEnd) {
                ASSERTV(*pc, '\r' == *pc);
                ++pc;
            }
            if (pc < preEqualsEnd) {
                ASSERTV(*pc, '\n' == *pc);
                ++pc;
            }
        }
        ASSERT(0 == bsl::count_if(pc,
                                  end,
                                  &u::notUrlAndNotEqualsAndNotCrlf));
        if (begin < end) {
            ASSERTV(run, showCharN(begin, end - begin),
                                           '\r' != end[-1] && '\n' != end[-1]);
        }
    }
}

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                              FUZZ TESTING
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, 'LLVMFuzzerTestOneInput', is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
//-----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

using bsl::uint8_t;

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
    // Use the specified 'data' array of 'size' bytes as input to methods of
    // this component and return zero.
{
    FuzzDataView fdv(data, size);    const FuzzDataView& FDV = fdv;
    int          test = FuzzUtil::consumeNumberInRange(&fdv, 1, 2);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // FUZZ TESTING WITHOUT MAXNUMOUT
        //
        // Concerns:
        //: 1 That object under test works properly with a varient of values
        //:   passed to the 'lineLength' parameter, and with tha parameter
        //:   allowed to default to 76.
        //:
        //: 2 That at most 2 '=' characters occur in the output, and that, if
        //:   present, they are at the end (possibly divided by a CRLF).
        //:
        //: 3 That CRLF's are present, or not, exactly as dictated by the
        //:   'lineLength' argument.
        //:
        //: 4 Test only with 'maxNumOut' not specified.
        //
        // Plan:
        //: 1 Test the binary input with a variety of values passed to the
        //:   'lineLength' parameter (and nothing specified for the 'maxNumOut'
        //:   parameter:
        //:   o Test the case where no value is passed to the 'lineLength'
        //:     parameter, interpreted as a value of 76.
        //:   o Test 0, which means no CRLF's are inserted.
        //:   o Test a variety of positive values, including very low values.
        //:
        //: 2 Call 'checkBasic' at the end to verify that that the output
        //:   is a valid base64 encoding, with CRLF's exactly where they're
        //:   expected.
        //:
        //: 3 Allocate the segment of memory to write to using 'malloc' so that
        //:   the address santizer will detect out of bounds access.
        //
        // Testing:
        //   FUZZ TESTING WITHOUT MAXNUMOUT
        //   int encodedLength(int);
        //   int encodedLength(int, int);
        //   int convert(OUT, int *, int *, IN, IN);
        //   int endConvert(OUT, int *);
        // --------------------------------------------------------------------

        // We want 'LINE_LENGTH' to have a random value, max 127, with a
        // relatively high probabilty of 0.

        const int           LINE_LENGTH   = FuzzUtil::consumeBool(&fdv)
                                          ? 0
                                          : FuzzUtil::consumeNumberInRange(
                                                                 &fdv, 1, 127);
        const char          GARBAGE       = static_cast<char>(0xa5);

        for (int ti = 0; ti < 2 * 2; ++ti) {
            int tti = ti;
            const bool           URL   = tti % 2;    tti /= 2;
            const Alphabet::Enum ALPHA = static_cast<Alphabet::Enum>(URL);
            const bool           PAD   = tti % 2;    tti /= 2;
            ASSERT(0 == tti);

            const EncoderOptions& options = EncoderOptions::custom(
                                                                 LINE_LENGTH,
                                                                 ALPHA,
                                                                 PAD);
            const char * const begin =
                                   reinterpret_cast<const char *>(FDV.begin());
            const char * const end   =
                                   reinterpret_cast<const char *>(FDV.end());

            const bool      leftOver      = 0 != fdv.length() % 3;
            const ptrdiff_t outLength     = Obj::encodedLength(options,
                                                               fdv.length());
            ASSERT(0 != LINE_LENGTH || !PAD || 0 == outLength % 4);

            bsl::string outBuf(outLength + 1, GARBAGE);
            const char *outBegin = outBuf.data();

            char *out = outBuf.data();

            bsls::ObjectBuffer<Obj> ob;
            if (EncoderOptions::mime() == options) {
                new (ob.address()) Obj();
            }
            else {
                new (ob.address()) Obj(options);
            }
            Obj& mX = ob.object();    const Obj& X = mX;
            ASSERT(X.options() == options);

            int numOut, numIn;

            int rc = mX.convert(out, &numOut, &numIn, begin, end);
            ASSERT(0 <= rc);
            ASSERT(0 <= numOut);
            ASSERT(numOut <= outLength);

            out += numOut;

            ASSERT(outBegin <= out);
            ASSERT(out <= outBegin + outLength);
            ASSERT(0 <= numIn);
            ASSERT(numIn <= static_cast<int>(FDV.length()));

            ASSERT(!leftOver || numOut < outLength);

            EncoderOptions padLessOptions(options);
            padLessOptions.setIsPadded(false);

            ASSERT(u::checkRange(padLessOptions, outBegin, outBegin + numOut));

            int endNumOut;
            rc = mX.endConvert(out, &endNumOut);
            ASSERT(0 == rc && X.isDone());
            ASSERT(0 <= endNumOut);
            ASSERT(outLength == numOut + endNumOut);

            ASSERT(u::checkRange(options, outBegin, outBegin + numOut));
            ASSERT((!PAD || !leftOver) ==
                u::checkRange(padLessOptions, outBegin, outBegin + outLength));
            ASSERT(GARBAGE == outBegin[outLength]);

            mX.~Obj();
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUZZ TESTING WITH MAXNUMOUT
        //
        // Concerns:
        //: 1 That object under test works properly with a varient of values
        //:   passed to the 'lineLength' parameter, and with tha parameter
        //:   allowed to default to 76.
        //:
        //: 2 That at most 2 '=' characters occur in the output, and that, if
        //:   present, they are at the end (possibly divided by a CRLF).
        //:
        //: 3 That CRLF's are present, or not, exactly as dictated by the
        //:   'lineLength' argument.
        //:
        //: 4 Test with a variety of values of 'maxNumOut', including 0,
        //:   specified to 'convert', and variety not including 0 specified to
        //:   'endConvert'.
        //
        // Plan:
        //: 1 Test the binary input with a variety of values passed to the
        //:   'lineLength' parameter.
        //:   o Test the case where no value is passed to the 'lineLength'
        //:     parameter, interpreted as a value of 76.
        //:   o Test 0, which means no CRLF's are inserted.
        //:   o Test a variety of positive values, including very low values.
        //:   o Test with a variety of values passed to the 'maxNumOut'
        //:     parameter (don't pass 0 to 'endNumOut', that would not be in
        //:     contract.
        //:
        //: 2 Call 'checkBasic' at the end to verify that that the output
        //:   is a valid base64 encoding, with CRLF's exactly where they're
        //:   expected.
        //:
        //: 3 Allocate the segment of memory to write to using 'malloc' so that
        //:   the address santizer will detect out of bounds access.
        //
        // Testing:
        //   FUZZ TESTING WITH MAXNUMOUT
        //   int encodedLength(int);
        //   int encodedLength(int, int);
        //   int convert(OUT, int *, int *, IN, IN, int);
        //   int endConvert(OUT, int *, int);
        // --------------------------------------------------------------------

        // We want 'LINE_LENGTH' to have a random value, max 127, with a
        // relatively high probabilty of 0.

        const int           LINE_LENGTH   = FuzzUtil::consumeBool(&fdv)
                                          ? 0
                                          : FuzzUtil::consumeNumberInRange(
                                                                 &fdv, 1, 127);
        const char          GARBAGE       = static_cast<char>(0xa5);

        bool doneTi = false;
        for (int ti = 0; ti < 2 * 2; ++ti) {
            int tti = ti;
            const bool           URL   = tti % 2;    tti /= 2;
            const Alphabet::Enum ALPHA = static_cast<Alphabet::Enum>(URL);
            const bool           PAD   = tti % 2;    tti /= 2;
            ASSERT(0 == tti);
            doneTi |= URL && PAD;

            const EncoderOptions& options = EncoderOptions::custom(
                                                                 LINE_LENGTH,
                                                                 ALPHA,
                                                                 PAD);
            EncoderOptions padLessOptions(options);
            padLessOptions.setIsPadded(false);

            const char * const begin =
                                   reinterpret_cast<const char *>(FDV.begin());
            const char * const end   =
                                   reinterpret_cast<const char *>(FDV.end());

            const bool      leftOver        = 0 != fdv.length() % 3;
            const ptrdiff_t outLength       = Obj::encodedLength(options,
                                                                 fdv.length());
            ASSERT(0 != LINE_LENGTH || !PAD || 0 == outLength % 4);

            for (int maxNumOutTi = -1; maxNumOutTi <= outLength + 1;
                                                               ++maxNumOutTi) {
                int maxNumOut = maxNumOutTi;

                const char *beginIn = begin;

                bsl::string outBuf(outLength + 1, GARBAGE);
                const char *outBegin = outBuf.data();

                char *out = outBuf.data();

                bsls::ObjectBuffer<Obj> ob;
                if (EncoderOptions::mime() == options) {
                    new (ob.address()) Obj();
                }
                else {
                    new (ob.address()) Obj(options);
                }
                Obj& mX = ob.object();    const Obj& X = mX;
                ASSERT(X.options() == options);

                int numOut, numIn, numOutSoFar = 0, numInSoFar = 0;

                do {
                    int rc = mX.convert(out,
                                        &numOut,
                                        &numIn,
                                        beginIn,
                                        end,
                                        maxNumOut);
                    ASSERT(0 <= rc);
                    ASSERT(0 <= numOut);
                    ASSERT(numOut <= maxNumOut || maxNumOut < 0);

                    ASSERT(0 <= numIn);
                    ASSERT(numIn <= end - beginIn);

                    beginIn     += numIn;
                    numInSoFar  += numIn;
                    ASSERT(numInSoFar <= static_cast<int>(fdv.length()));

                    out         += numOut;
                    numOutSoFar += numOut;
                    ASSERT(numOutSoFar <= outLength);

                    ASSERT(!leftOver || numOutSoFar < outLength);

                    ASSERT(u::checkRange(padLessOptions,
                                         outBegin,
                                         outBegin + numOutSoFar));

                    if (0 == numIn && 0 == maxNumOut && beginIn < end) {
                        maxNumOut = 1;
                    }
                } while (beginIn < end);

                ASSERTV(padLessOptions, bsl::string(outBegin,
                                                    outBegin + numOutSoFar),
                                        u::checkRange(padLessOptions,
                                                      outBegin,
                                                      outBegin + numOutSoFar));

                maxNumOut = maxNumOutTi;

                bool done = false;
                while (!done) {
                    int rc = mX.endConvert(out, &numOut, maxNumOut);
                    ASSERT(0 <= numOut);
                    done = 0 == rc;
                    ASSERT(X.isDone() == done);
                    ASSERT(done || maxNumOut < (outLength - numOutSoFar));

                    out         += numOut;
                    numOutSoFar += numOut;

                    ASSERTV(options, numOutSoFar, outLength, done, numInSoFar,
                                                                   end - begin,
                                           (numOutSoFar == outLength) == done);
                    ASSERTV(run, beginIn - begin, maxNumOut,
                                           (numOutSoFar == outLength) == done);

                    if (0 == maxNumOut) {
                        maxNumOut = 1;
                    }
                }

                ASSERTV(options, bsl::string(outBegin, outBegin + outLength),
                       u::checkRange(options, outBegin, outBegin + outLength));
                ASSERT(leftOver || u::checkRange(padLessOptions,
                                                 outBegin,
                                                 outBegin + outLength));
                ASSERT(GARBAGE == outBegin[outLength]);

                mX.~Obj();
            } // for maxNumOutTi
        } // for ti
        ASSERT(doneTi);
      } break;
      default: {
      } break;
    }

    if (testStatus > 0) {
        BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
    }

    ++run;

    return 0;
}

// ============================================================================
//                         SUPPORT FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

int streamEncoder(bsl::ostream& os, bsl::istream& is)
{
    enum {
        e_SUCCESS      =  0,
        e_ENCODE_ERROR = -1,
        e_IO_ERROR     = -2
    };

    bdlde::Base64Encoder converter;

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
                return e_ENCODE_ERROR;                                // RETURN
            }

            output += numOut;
            input  += numIn;

            if (output == outputEnd) {  // output buffer full; write data
                os.write(outputBuffer, sizeof outputBuffer);
                if (os.fail()) {
                    return e_IO_ERROR;                                // RETURN
                }
                output = outputBuffer;
            }
        }
    }

    while (1) {

        int numOut;

        int more = converter.endConvert(output,
                                        &numOut,
                                        static_cast<int>(outputEnd - output));
        if (more < 0) {
            return e_ENCODE_ERROR;                                    // RETURN
        }

        output += numOut;

        if (!more) { // no more output
            break;
        }

        ASSERT(output == outputEnd);  // output buffer is full

        os.write (outputBuffer, sizeof outputBuffer);  // write buffer
        if (os.fail()) {
            return e_IO_ERROR;                                        // RETURN
        }
        output = outputBuffer;
    }

    if (output > outputBuffer) {
        os.write (outputBuffer, output - outputBuffer);
    }

    return (is.eof() && os.good()) ? e_SUCCESS : e_IO_ERROR;
}

int streamDecoder(bsl::ostream& os, bsl::istream& is)
{
    enum {
        e_SUCCESS      =  0,
        e_DECODE_ERROR = -1,
        e_IO_ERROR     = -2
    };

    u_Base64Decoder_Test converter;

    const int INBUFFER_SIZE  = 1 << 10;
    const int OUTBUFFER_SIZE = 1 << 10;

    char inputBuffer[INBUFFER_SIZE];
    char outputBuffer[OUTBUFFER_SIZE];

    char *output    = outputBuffer;
    char *outputEnd = outputBuffer + sizeof outputBuffer;

    int numOut;
    int numIn;

    while (is.good()) {  // input stream not exhausted

        is.read(inputBuffer, sizeof inputBuffer);

        const char *input    = inputBuffer;
        const char *inputEnd = input + is.gcount();

        while (input < inputEnd) { // input encoding not complete

            int status = converter.convert(
                                         output,
                                         &numOut,
                                         &numIn,
                                         input,
                                         inputEnd,
                                         static_cast<int>(outputEnd - output));
            if (status < 0) {
                return e_DECODE_ERROR;                                // RETURN
            }

            output += numOut;
            input  += numIn;

            if (output == outputEnd) {  // output buffer full; write data
                os.write(outputBuffer, sizeof outputBuffer);
                if (os.fail()) {
                    return e_IO_ERROR;                                // RETURN
                }
                output = outputBuffer;
            }
        }
    }

    while (1) {

        int more = converter.endConvert(output,
                                        &numOut,
                                        static_cast<int>(outputEnd - output));
        if (more < 0) {
            return e_DECODE_ERROR;                                    // RETURN
        }

        output += numOut;

        if (!more) { // no more output
            break;
        }

        ASSERT(output == outputEnd);  // output buffer is full

        os.write (outputBuffer, sizeof outputBuffer);  // write buffer
        if (os.fail()) {
            return e_IO_ERROR;                                        // RETURN
        }
        output = outputBuffer;
    }

    if (output > outputBuffer) {
        os.write (outputBuffer, output - outputBuffer);
    }

    return (is.eof() && os.good()) ? e_SUCCESS : e_IO_ERROR;
}

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

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Demonstrate that the example compiles, links, and runs.
        //
        // Concerns:
        //   That the example compiles, links, and runs.
        //
        // Plan:
        //   Create the example in a form that can be cut and pasted easily
        //   into the header file.  Then use the stream encoder and decoder on
        //   some string, and compare the output with the original data.
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
// The following example shows how to use a 'bdlde::Base64Encoder' object to
// implement a function, 'streamEncode', that reads text from a 'bsl::istream',
// encodes that text from base 64 representation, and writes the encoded text
// to a 'bsl::ostream'.  'streamEncoder' returns 0 on success and a negative
// value if the input data could not be successfully encoded or if there is an
// I/O error.
//..
// streamencoder.h                      -*-C++-*-
//
// int streamEncoder(bsl::ostream& os, bsl::istream& is);
//     // Read the entire contents of the specified input stream 'is', convert
//     // the input base-64 encoding into plain text, and write the decoded
//     // text to the specified output stream 'os'.  Return 0 on success, and a
//     // negative value otherwise.
//..
        const bsl::string  inStr(BLOOMBERG_NEWS, sizeof(BLOOMBERG_NEWS));
        bsl::istringstream inStream(inStr);

        bsl::stringstream  outStream;
        bsl::stringstream  backInStream;

        ASSERT(0 == streamEncoder(outStream, inStream));
        ASSERT(0 == streamDecoder(backInStream, outStream));

        cout << backInStream.str();

        ASSERT(inStr == backInStream.str());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // ENSURE U_ENABLE_DEPRECATIONS IS DISABLED
        //
        // Concerns:
        //: 1 When we ship, 'U_ENABLE_DEPRECATIONS' is disabled, so that we
        //:   will not get deprecation warnings.
        //
        // Testing:
        //   0 == U_ENABLE_DEPRECATIONS
        // --------------------------------------------------------------------

        ASSERT(0 == U_ENABLE_DEPRECATIONS);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING OPTIONAL NUMIN, NUMOUT
        //   This case is available to be used as a developers' sandbox.
        //
        // Concerns:
        //   That 'convert' will function properly with null passed to
        //   either or both of 'numOut' and 'numIn'.
        //
        // Plan:
        //   Repeat breathing test, only pass null to 'numIn' or 'numOut' in
        //   all cases.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OPTIONAL NUMIN, NUMOUT" << endl
                          << "==============================" << endl;

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
            bsl::ostringstream out;

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

        if (verbose) cout << "\nTry instantiating an encoder." << endl;
        {
            Obj encoder;
            if (veryVerbose) {
                T_ P(encoder.isAcceptable());
                T_ P(encoder.isDone());
                T_ P(encoder.isError());
                T_ P(encoder.isInitialState());
                T_ P(encoder.maxLineLength());
                T_ P(encoder.outputLength());
            }

            if (verbose) cout << "\nEncode something." << endl;

            static char out[1000];

            int outIdx = 0;
            int numIn = 0;
            int numOut = 0;

            //              begin:     0     1     3       4       8    9 END
            //              end  :     1     3     4       8       9   10 INPUT
            const char*const input = "\0" "\0\0" "\0" "\0\0\0\0" "\0" "\0";

            ASSERT(0 == numOut); ASSERT(0 == numIn); ASSERT(0 == outIdx);
            ASSERT(0 == out[0]); ASSERT(0 == out[1]);

            if (verbose) cout << "\tEncode: ^0." << endl;
            {
                const char *const begin = input + 0;
                const char *const end   = input + 1;

                encoder.convert(out + outIdx, 0, 0, begin, end);
                outIdx += 1;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == outIdx);
            ASSERT('A' == out[0]); ASSERT(0 == out[1]);

            if (verbose) cout << "\tEncode: 0^00." << endl;
            {
                const char *const begin = input + 1;
                const char *const end   = input + 3;

                encoder.convert(out + outIdx, 0, 0, begin, end);
                outIdx += 3;

                if (veryVerbose) {
                    T_ T_ P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(4 == outIdx);
            ASSERT('A' == out[3]); ASSERT(0 == out[4]);

            if (verbose) cout << "\tEncode: 000^0." << endl;
            {
                const char *const begin = input + 3;
                const char *const end   = input + 4;

                encoder.convert(out + outIdx, &numOut, 0, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(5 == outIdx);
            ASSERT('A' == out[3]); ASSERT('A' == out[4]); ASSERT(0 == out[5]);

            if (verbose) cout << "\tEncode: 0000^0000." << endl;
            {
                const char *const begin = input + 4;
                const char *const end   = input + 8;

                encoder.convert(out + outIdx, 0, &numIn, begin, end);
                outIdx += 5;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(4 == numIn); ASSERT(10 == outIdx);
            ASSERT('A' == out[7]); ASSERT('A' == out[8]);
            ASSERT('A' == out[9]); ASSERT(0 == out[10]);

            if (verbose) cout << "\tEncode: 00000000^0." << endl;
            {
                const char *const begin = input + 8;
                const char *const end   = input + 9;

                encoder.convert(out + outIdx, &numOut, 0, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(2 == numOut); ASSERT(12 == outIdx);
            ASSERT('A' == out[11]); ASSERT(0 == out[12]);

            if (verbose) cout << "\tEncode: 000000000^0." << endl;
            {
                const char *const begin = input + 9;
                const char *const end   = input + 10;

                encoder.convert(out + outIdx, 0, 0, begin, end);
                outIdx += 1;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(13 == outIdx);
            ASSERT('A' == out[11]); ASSERT('A' == out[12]);
            ASSERT(0 == out[13]);

            if (verbose) cout << "\tEncode: 0000000000^$." << endl;
            {
                encoder.endConvert(out + outIdx, &numOut);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(3 == numOut); ASSERT(16 == outIdx);
            ASSERT('A' == out[12]); ASSERT('A' == out[13]);
            ASSERT('=' == out[14]); ASSERT('=' == out[15]);
            ASSERT(0 == out[16]);
        }

      } break;
      case 12: {
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
        V("but not an even number.");
        {
            const char sample[] = "This is exactly 29 char long.";

            int maxLineLength = 0;

            bdlde::Base64Encoder encoder(maxLineLength);
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
            u_Base64Decoder_Test decoder(true);

            int   origSize = static_cast<int>(strlen(sample));
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
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR:
        //   Verify initial configuration defaults.
        //
        // Concerns:
        //   - That the default line-length is 76.
        //   - That the state is the initial state.
        //
        // Plan:
        //   - Create the object and query it for its information.
        //   - Use '::isState' to verify that the newly created object is in
        //      the initial state.
        //   - Create two lines whose unlimited output lengths would be 76
        //      and 80 characters, respectively.
        //      + Observe the respective output lengths to be 76 and 82.
        //
        // Tactics:
        //   - Ad-HocData Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlde::Base64Encoder(Alphabet alphabet = e_BASIC);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
            "\nVerify default configuration and initial state, default "
            "alhpabet." << endl;
        {
            Obj obj;
            ASSERT(76 == obj.maxLineLength());
            ASSERT(76 == obj.options().maxLineLength());
            ASSERT( 1 == obj.isAcceptable());
            ASSERT( 0 == obj.isDone());
            ASSERT( 0 == obj.isError());
            ASSERT( 1 == obj.isInitialState());
            ASSERT( 0 == obj.outputLength());
            ASSERT(Alphabet::e_BASIC == obj.alphabet());
            ASSERT(true == obj.options().isPadded());

            ASSERT(isState(&obj, e_INITIAL_STATE));
        }

        if (verbose) cout <<
            "\nVerify default configuration and initial state, e_BASIC "
            "alphabet." << endl;
        {
            Obj obj(EncoderOptions::mime());
            ASSERT(76 == obj.maxLineLength());
            ASSERT(76 == obj.options().maxLineLength());
            ASSERT( 1 == obj.isAcceptable());
            ASSERT( 0 == obj.isDone());
            ASSERT( 0 == obj.isError());
            ASSERT( 1 == obj.isInitialState());
            ASSERT( 0 == obj.outputLength());
            ASSERT(Alphabet::e_BASIC == obj.alphabet());
            ASSERT(true == obj.options().isPadded());

            ASSERT(isState(&obj, e_INITIAL_STATE));
        }

        if (verbose) cout <<
            "\nVerify default configuration and initial state, e_URL alphabet."
            << endl;
        {
            Obj obj(EncoderOptions::urlSafe());
            ASSERT( 0 == obj.maxLineLength());
            ASSERT( 0 == obj.options().maxLineLength());
            ASSERT( 1 == obj.isAcceptable());
            ASSERT( 0 == obj.isDone());
            ASSERT( 0 == obj.isError());
            ASSERT( 1 == obj.isInitialState());
            ASSERT( 0 == obj.outputLength());
            ASSERT(Alphabet::e_URL == obj.alphabet());
            ASSERT(false == obj.options().isPadded());

            ASSERT(isState(&obj, e_INITIAL_STATE));
        }

        if (verbose) cout << "\nTest default maximum line length." << endl;

        const char *const input = "1234567890" "1234567890" "1234567890"
                                  "1234567890" "1234567890" "1234567";

        if (verbose) cout << "\tInput 57 bytes; output 76 characters." << endl;
        {
            Obj obj;

            char outputBuffer[100];
            memset(outputBuffer, '?', sizeof outputBuffer);

            int numIn, numOut;
            const char *const B = input, *const E = B + 57;

            obj.convert(outputBuffer, &numOut, &numIn, B, E);
            obj.endConvert(outputBuffer + numOut, &numOut);

            ASSERT(76 == obj.outputLength());

            ASSERT('?' != outputBuffer[ 0]);

            ASSERT('?' != outputBuffer[75]);
            ASSERT('?' == outputBuffer[76]);
            ASSERT('?' == outputBuffer[77]);
            ASSERT('?' == outputBuffer[78]);
            ASSERT('?' == outputBuffer[79]);
            ASSERT('?' == outputBuffer[80]);
            ASSERT('?' == outputBuffer[81]);
            ASSERT('?' == outputBuffer[82]);

            ASSERT('?' == outputBuffer[99]);
        }

        if (verbose) cout << "\tInput 58 bytes; output 82 characters." << endl;
        {
            Obj obj;

            char outputBuffer[100];
            memset(outputBuffer, '?', sizeof outputBuffer);

            int numIn, numOut;
            const char *const B = input, *const E = B + 58;

            obj.convert(outputBuffer, &numOut, &numIn, B, E);
            obj.endConvert(outputBuffer + numOut, &numOut);

            ASSERT(82 == obj.outputLength());

            ASSERT('?'  != outputBuffer[ 0]);

            ASSERT('?'  != outputBuffer[75]);
            ASSERT('\r' == outputBuffer[76]);
            ASSERT('\n' == outputBuffer[77]);
            ASSERT('A'  == outputBuffer[78]);
            ASSERT('A'  == outputBuffer[79]);
            ASSERT('=' == outputBuffer[80]);
            ASSERT('=' == outputBuffer[81]);
            ASSERT('?' == outputBuffer[82]);

            ASSERT('?' == outputBuffer[99]);
        }

      } break;
      case 10: {
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

        if (verbose) cout << "\tWith 'maxLineLength' = 0." << endl;
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(EncoderOptions::standard());
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = e_INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, e_INITIAL_STATE));
                obj.resetState();
                LOOP_ASSERT(i, 1 == isState(&obj, e_INITIAL_STATE));

                LOOP_ASSERT(i, 0 == obj.maxLineLength());
            }
        }

        if (verbose) cout << "\tWith 'maxLineLength' = 5." << endl;
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(EncoderOptions::custom(5,
                                               Alphabet::e_BASIC,
                                               true));
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = e_INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, e_INITIAL_STATE));
                obj.resetState();
                LOOP_ASSERT(i, 1 == isState(&obj, e_INITIAL_STATE));

                LOOP_ASSERT(i, 5 == obj.maxLineLength());
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'endConvert'
        //
        // Concept:
        //: 1 Test 'endConvert'.
        //
        // Plan:
        //: 1 Translate segments full of random garbage with varying values of
        //
        // Testing:
        //   int endConvert(OUTPUT_ITERATOR);
        //   int endConvert(OUTPUT_ITERATOR, int *, int);
        // --------------------------------------------------------------------

        static const int lineLengths[] = { 0, 1, 2, 3, 4, 10, 20, 40, 75,
                                           76, 77, 80, 90, 100 };
        enum { k_NUM_LINE_LENGTHS = sizeof lineLengths / sizeof *lineLengths };

        unsigned   maxNumOutIdx = 0;
        const char GARBAGE      = char(0xaf);

        bsl::vector<char> input;
        input.resize(256);
        for (int ii = 0, jj = 0; ii < 256; ++ii, jj += 7) {
            input[ii] = static_cast<char>(jj);
        }

        for (int ui = 0; ui < 2; ++ui) {
            const Alphabet::Enum alpha   = ui ? Alphabet::e_URL
                                              : Alphabet::e_BASIC;
            const u::Checker     checker = ui ? &u::checkUrl
                                              : &u::checkBasic;

            for (int len = 0; len < 256; ++len) {
                const bool leftOver = len % 3;

                for (int li = 0; li < 2 * k_NUM_LINE_LENGTHS; ++li) {
                    const bool  threeArgs   = k_NUM_LINE_LENGTHS <= li;
                    const int   llIdx       = li % k_NUM_LINE_LENGTHS;
                    const int   LINE_LENGTH = lineLengths[llIdx];

                    const EncoderOptions& options =
                                          EncoderOptions::custom(LINE_LENGTH,
                                                                 alpha,
                                                                 true);
                    const int   outLength   = Obj::encodedLength(len,
                                                                 LINE_LENGTH);
                    bsl::vector<char> outVec;
                    outVec.resize(outLength + 1, GARBAGE);

                    char       *outBuf      = &outVec[0];
                    const char *outBegin    = outBuf;

                    char *out = outBuf;

                    Obj mX(options);
                    const Obj& X = mX;

                    const char *beginIn     = &input[0];
                    const char *end         = beginIn + len;
                    const int   maxNumOuts[] = { 0, 1, 2, 3, 4, 7, 17, 29,
                                                 32, 35 };
                    enum { k_NUM_MAX_NUM_OUTS = sizeof maxNumOuts /
                                                          sizeof *maxNumOuts };
                    int         maxNumOut   = maxNumOuts[maxNumOutIdx];
                    maxNumOutIdx = (maxNumOutIdx + 1) % k_NUM_MAX_NUM_OUTS;

                    int outSoFar = 0;
                    int numOut, numIn;
                    int rc = mX.convert(out,
                                        &numOut,
                                        &numIn,
                                        beginIn,
                                        end);

                    ASSERT(0 == rc);
                    ASSERT(0 <= numOut);

                    out += numOut;

                    ASSERT(outBuf <= out);
                    ASSERT(out <= outBuf + outLength);
                    ASSERT(0 <= numIn);

                    outSoFar += numOut;
                    beginIn  += numIn;

                    ASSERT(outSoFar <= outLength);
                    ASSERT(beginIn <= end);

                    ASSERT(!leftOver || outSoFar < outLength);
                    ASSERT(!leftOver || out < outBuf + outLength);

                    ASSERT(end == beginIn);
                    ASSERT(0 == bsl::count_if(
                                            outBuf,
                                            outBuf + outSoFar,
                                            Alphabet::e_BASIC == alpha
                                            ? &u::equalsOrNotBasicAndNotCrlf
                                            : &u::equalsOrNotUrlAndNotCrlf));

                    if (0 == maxNumOut) {
                        maxNumOut = 1;
                    }

                    int endNumOut;
                    int numEndConvertCalls = 0;
                    if (threeArgs) {
                        int rc;
                        do {
                            ++numEndConvertCalls;

                            rc = mX.endConvert(out, &endNumOut, maxNumOut);
                            if (0 != rc) {
                                ASSERT(0 < endNumOut);
                                ASSERT(endNumOut == maxNumOut);
                            }

                            outSoFar += endNumOut;
                            out      += endNumOut;

                            ASSERT(0 == bsl::count_if(
                                          outBegin,
                                          outBegin + outSoFar,
                                          Alphabet::e_BASIC == alpha
                                          ? &u::notBasicAndNotEqualsAndNotCrlf
                                          : &u::notUrlAndNotEqualsAndNotCrlf));
                        } while (rc);

                        ASSERTV(LINE_LENGTH, maxNumOut, numEndConvertCalls,
                                                        outLength == outSoFar);
                    }
                    else {
                        int rc = mX.endConvert(out);
                        ASSERT(0 == rc);
                    }

                    ASSERT(X.isDone());
                    ASSERT(GARBAGE == outBuf[outLength]);
                    (*checker)(outBuf, outBuf + outLength, LINE_LENGTH);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS WITH DEFAULT ARGUMENTS.
        //   Continue testing 'convert' and 'endConvert' with defaults
        //   arguments.
        //
        // Concerns:
        //   - That the conversion logic is correct in all places:
        //      + quanta of length 1
        //      + quanta of length 2
        //      + quanta of length 3
        //   - That max-line-length is observed and that CRLF's are inserted
        //      accordingly.
        //   - That a relatively long input sequence is treated correctly.
        //   - That a longer input sequence is treated the same as any
        //      equivalent series of shorter subsequences supplied separately.
        //
        // Plan:
        //   - Using Category Partitioning, enumerate input sequences of
        //      increasing length (or length + maxLineLength) to a sufficient
        //      *Depth* that all states in the FSM are exercised thoroughly.
        //      + With Depth = inputLength, make sure that each bit of input
        //         data results in the proper output for the sequence.
        //      + With Depth = inputLength + maxLineLength of sufficient depth
        //         to ensure that all of the output processing mechanisms are
        //         working properly.
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
        //   - All of the encoded sequences in the table are valid in both
        //     'e_BASIC' and 'e_URL' type encodings, so test it in both modes.
        //
        // Testing:
        //   static int encodedLength(int numInputBytes, int maxLineLength);
        //
        //   DFLT convert(char *o, int *no, int *ni, begin, end, int mno = -1);
        //   DFLT endConvert(char *out, int *numOut, int maxNumOut = -1);
        //
        //   That each bit of a 3-byte quantum finds its appropriate spot.
        //   That each bit of a 2-byte quantum finds its appropriate spot.
        //   That each bit of a 1-byte quantum finds its appropriate spot.
        //   That output length is calculated and stored properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        static const struct Data {
            int         d_lineNum;         // source line number
            int         d_maxLineLength;   // maximum length of output line
            int         d_inputLength;     // number of input characters
            const char *d_input_p;         // input characters
            unsigned    d_outputLength;    // total length of output
            const char *d_output_p;        // expected output data
        } DATA[] = {
//--------------^

  // *** DEPTH-ORDERED ENUMERATION: Depth = inputLength; (0 == maxLineLen) ***

//    v---------------------INPUT--------------------v  v-------OUTPUT------v
//lin LL #i Input Data                                  #o Output Data
//--- -- -- ------------------------------------------  -- ------------------
{ L_,  0, 0,"",                                          0,""                },

{ L_,  0, 1,"\x00",                                      4,"AA=="            },
{ L_,  0, 1,"\x01",                                      4,"AQ=="            },
{ L_,  0, 1,"\x02",                                      4,"Ag=="            },
{ L_,  0, 1,"\x04",                                      4,"BA=="            },
{ L_,  0, 1,"\x08",                                      4,"CA=="            },
{ L_,  0, 1,"\x10",                                      4,"EA=="            },
{ L_,  0, 1,"\x20",                                      4,"IA=="            },
{ L_,  0, 1,"\x40",                                      4,"QA=="            },
{ L_,  0, 1,"\x80",                                      4,"gA=="            },

{ L_,  0, 2,"\x00\x00",                                  4,"AAA="            },
{ L_,  0, 2,"\x00\x01",                                  4,"AAE="            },
{ L_,  0, 2,"\x00\x02",                                  4,"AAI="            },
{ L_,  0, 2,"\x00\x04",                                  4,"AAQ="            },
{ L_,  0, 2,"\x00\x08",                                  4,"AAg="            },
{ L_,  0, 2,"\x00\x10",                                  4,"ABA="            },
{ L_,  0, 2,"\x00\x20",                                  4,"ACA="            },
{ L_,  0, 2,"\x00\x40",                                  4,"AEA="            },
{ L_,  0, 2,"\x00\x80",                                  4,"AIA="            },

{ L_,  0, 3,"\x00\x00\x00",                              4,"AAAA"            },
{ L_,  0, 3,"\x00\x00\x01",                              4,"AAAB"            },
{ L_,  0, 3,"\x00\x00\x02",                              4,"AAAC"            },
{ L_,  0, 3,"\x00\x00\x04",                              4,"AAAE"            },
{ L_,  0, 3,"\x00\x00\x08",                              4,"AAAI"            },
{ L_,  0, 3,"\x00\x00\x10",                              4,"AAAQ"            },
{ L_,  0, 3,"\x00\x00\x20",                              4,"AAAg"            },
{ L_,  0, 3,"\x00\x00\x40",                              4,"AABA"            },
{ L_,  0, 3,"\x00\x00\x80",                              4,"AACA"            },

{ L_,  0, 4,"\x00\x00\x00\x00",                          8,"AAAAAA=="        },
{ L_,  0, 4,"\x00\x00\x00\x01",                          8,"AAAAAQ=="        },
{ L_,  0, 4,"\x00\x00\x00\x02",                          8,"AAAAAg=="        },
{ L_,  0, 4,"\x00\x00\x00\x04",                          8,"AAAABA=="        },
{ L_,  0, 4,"\x00\x00\x00\x08",                          8,"AAAACA=="        },
{ L_,  0, 4,"\x00\x00\x00\x10",                          8,"AAAAEA=="        },
{ L_,  0, 4,"\x00\x00\x00\x20",                          8,"AAAAIA=="        },
{ L_,  0, 4,"\x00\x00\x00\x40",                          8,"AAAAQA=="        },
{ L_,  0, 4,"\x00\x00\x00\x80",                          8,"AAAAgA=="        },

{ L_,  0, 5,"\x00\x00\x00\x00\x00",                      8,"AAAAAAA="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x01",                      8,"AAAAAAE="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x02",                      8,"AAAAAAI="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x04",                      8,"AAAAAAQ="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x08",                      8,"AAAAAAg="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x10",                      8,"AAAAABA="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x20",                      8,"AAAAACA="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x40",                      8,"AAAAAEA="        },
{ L_,  0, 5,"\x00\x00\x00\x00\x80",                      8,"AAAAAIA="        },

{ L_,  0, 6,"\x00\x00\x00\x00\x00\x00",                  8,"AAAAAAAA"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x01",                  8,"AAAAAAAB"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x02",                  8,"AAAAAAAC"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x04",                  8,"AAAAAAAE"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x08",                  8,"AAAAAAAI"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x10",                  8,"AAAAAAAQ"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x20",                  8,"AAAAAAAg"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x40",                  8,"AAAAAABA"        },
{ L_,  0, 6,"\x00\x00\x00\x00\x00\x80",                  8,"AAAAAACA"        },

{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x00",             12,"AAAAAAAAAA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x01",             12,"AAAAAAAAAQ=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x02",             12,"AAAAAAAAAg=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x04",             12,"AAAAAAAABA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x08",             12,"AAAAAAAACA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x10",             12,"AAAAAAAAEA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x20",             12,"AAAAAAAAIA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x40",             12,"AAAAAAAAQA=="    },
{ L_,  0, 7,"\x00\x00\x00\x00\x00\x00\x80",             12,"AAAAAAAAgA=="    },

{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x00",         12,"AAAAAAAAAAA="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x01",         12,"AAAAAAAAAAE="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x02",         12,"AAAAAAAAAAI="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x04",         12,"AAAAAAAAAAQ="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x08",         12,"AAAAAAAAAAg="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x10",         12,"AAAAAAAAABA="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x20",         12,"AAAAAAAAACA="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x40",         12,"AAAAAAAAAEA="    },
{ L_,  0, 8,"\x00\x00\x00\x00\x00\x00\x00\x80",         12,"AAAAAAAAAIA="    },

{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x00",     12,"AAAAAAAAAAAA"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x01",     12,"AAAAAAAAAAAB"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x02",     12,"AAAAAAAAAAAC"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x04",     12,"AAAAAAAAAAAE"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x08",     12,"AAAAAAAAAAAI"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x10",     12,"AAAAAAAAAAAQ"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x20",     12,"AAAAAAAAAAAg"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x40",     12,"AAAAAAAAAABA"    },
{ L_,  0, 9,"\x00\x00\x00\x00\x00\x00\x00\x00\x80",     12,"AAAAAAAAAACA"    },

{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16,"AAAAAAAAAAAAAA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", 16,"AAAAAAAAAAAAAQ=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02", 16,"AAAAAAAAAAAAAg=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04", 16,"AAAAAAAAAAAABA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08", 16,"AAAAAAAAAAAACA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10", 16,"AAAAAAAAAAAAEA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20", 16,"AAAAAAAAAAAAIA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40", 16,"AAAAAAAAAAAAQA=="},
{ L_,  0,10,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80", 16,"AAAAAAAAAAAAgA=="},


    // *** DEPTH-ORDERED ENUMERATION: Depth = inputLength + maxLineLength ***

//    v----------INPUT-----------v  v-----------------OUTPUT---------------v
//lin LL #i input data              #o output data
//--- -- -- ----------------------  -- -------------------------------------
{ L_,  0, 0,"",                      0,""                                    },

{ L_,  1, 0,"",                      0,""                                    },
{ L_,  0, 1,"\0",                    4,"AA=="                                },

{ L_,  2, 0,"",                      0,""                                    },
{ L_,  1, 1,"\0",                   10,"A\r\nA\r\n=\r\n="                    },
{ L_,  0, 2,"\0\0",                  4,"AAA="                                },

{ L_,  3, 0,"",                      0,""                                    },
{ L_,  2, 1,"\0",                    6,"AA\r\n=="                            },
{ L_,  1, 2,"\0\0",                 10,"A\r\nA\r\nA\r\n="                    },
{ L_,  0, 3,"\0\0\0",                4,"AAAA"                                },

{ L_,  4, 0,"",                      0,""                                    },
{ L_,  3, 1,"\0",                    6,"AA=\r\n="                            },
{ L_,  2, 2,"\0\0",                  6,"AA\r\nA="                            },
{ L_,  1, 3,"\0\0\0",               10,"A\r\nA\r\nA\r\nA"                    },
{ L_,  0, 4,"\0\0\0\0",              8,"AAAAAA=="                            },

{ L_,  5, 0,"",                      0,""                                    },
{ L_,  4, 1,"\0",                    4,"AA=="                                },
{ L_,  3, 2,"\0\0",                  6,"AAA\r\n="                            },
{ L_,  2, 3,"\0\0\0",                6,"AA\r\nAA"                            },
{ L_,  1, 4,"\0\0\0\0",             22,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\n=\r\n="},
{ L_,  0, 5,"\0\0\0\0\0",            8,"AAAAAAA="                            },

{ L_,  6, 0,"",                      0,""                                    },
{ L_,  5, 1,"\0",                    4,"AA=="                                },
{ L_,  4, 2,"\0\0",                  4,"AAA="                                },
{ L_,  3, 3,"\0\0\0",                6,"AAA\r\nA"                            },
{ L_,  2, 4,"\0\0\0\0",             14,"AA\r\nAA\r\nAA\r\n=="                },
{ L_,  1, 5,"\0\0\0\0\0",           22,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA\r\n="},
{ L_,  0, 6,"\0\0\0\0\0\0",          8,"AAAAAAAA"                            },

{ L_,  7, 0,"",                      0,""                                    },
{ L_,  6, 1,"\0",                    4,"AA=="                                },
{ L_,  5, 2,"\0\0",                  4,"AAA="                                },
{ L_,  4, 3,"\0\0\0",                4,"AAAA"                                },
{ L_,  3, 4,"\0\0\0\0",             12,"AAA\r\nAAA\r\n=="                    },
{ L_,  2, 5,"\0\0\0\0\0",           14,"AA\r\nAA\r\nAA\r\nA="                },
{ L_,  1, 6,"\0\0\0\0\0\0",         22,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA"},
{ L_,  0, 7,"\0\0\0\0\0\0\0",       12,"AAAAAAAAAA=="                        },

{ L_,  8, 0,"",                      0,""                                    },
{ L_,  7, 1,"\0",                    4,"AA=="                                },
{ L_,  6, 2,"\0\0",                  4,"AAA="                                },
{ L_,  5, 3,"\0\0\0",                4,"AAAA"                                },
{ L_,  4, 4,"\0\0\0\0",             10,"AAAA\r\nAA=="                        },
{ L_,  3, 5,"\0\0\0\0\0",           12,"AAA\r\nAAA\r\nA="                    },
{ L_,  2, 6,"\0\0\0\0\0\0",         14,"AA\r\nAA\r\nAA\r\nAA"                },
{ L_,  1, 7,"\0\0\0\0\0\0\0",       34,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA"
                                       "\r\nA\r\nA\r\n=\r\n="                },
{ L_,  0, 8,"\0\0\0\0\0\0\0\0",     12,"AAAAAAAAAAA="                        },

{ L_,  9, 0,"",                      0,""                                    },
{ L_,  8, 1,"\0",                    4,"AA=="                                },
{ L_,  7, 2,"\0\0",                  4,"AAA="                                },
{ L_,  6, 3,"\0\0\0",                4,"AAAA"                                },
{ L_,  5, 4,"\0\0\0\0",             10,"AAAAA\r\nA=="                        },
{ L_,  4, 5,"\0\0\0\0\0",           10,"AAAA\r\nAAA="                        },
{ L_,  3, 6,"\0\0\0\0\0\0",         12,"AAA\r\nAAA\r\nAA"                    },
{ L_,  2, 7,"\0\0\0\0\0\0\0",       22,"AA\r\nAA\r\nAA\r\nAA\r\nAA\r\n=="    },
{ L_,  1, 8,"\0\0\0\0\0\0\0\0",     34,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA"
                                       "\r\nA\r\nA\r\nA\r\n="                },
{ L_,  0, 9,"\0\0\0\0\0\0\0\0\0",   12,"AAAAAAAAAAAA"                        },


{ L_, 10, 0,"",                      0,""                                    },
{ L_,  9, 1,"\0",                    4,"AA=="                                },
{ L_,  8, 2,"\0\0",                  4,"AAA="                                },
{ L_,  7, 3,"\0\0\0",                4,"AAAA"                                },
{ L_,  6, 4,"\0\0\0\0",             10,"AAAAAA\r\n=="                        },
{ L_,  5, 5,"\0\0\0\0\0",           10,"AAAAA\r\nAA="                        },
{ L_,  4, 6,"\0\0\0\0\0\0",         10,"AAAA\r\nAAAA"                        },
{ L_,  3, 7,"\0\0\0\0\0\0\0",       18,"AAA\r\nAAA\r\nAAA\r\nA=="            },
{ L_,  2, 8,"\0\0\0\0\0\0\0\0",     22,"AA\r\nAA\r\nAA\r\nAA\r\nAA\r\nA="    },
{ L_,  1, 9,"\0\0\0\0\0\0\0\0\0",   34,"A\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA\r\nA"
                                       "\r\nA\r\nA\r\nA\r\nA"                },
{ L_,  0,10,"\0\0\0\0\0\0\0\0\0\0", 16,"AAAAAAAAAAAAAA=="                    },
//----------v
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int depth = -1;

        // MAIN TEST-TABLE LOOP
        bool done = false;
        for (int ti = 0; ti < 2 * 2 * NUM_DATA; ++ti) {
            int                    tti     = ti;
            const int              dIdx    = tti % NUM_DATA;   tti /= NUM_DATA;
            const Data&            data    = DATA[dIdx];
            const int              LINE    = data.d_lineNum;
            const int              MAX_LEN = data.d_maxLineLength;
            const int              IN_LEN  = data.d_inputLength;
            const char *const      INPUT   = data.d_input_p;
            bsl::size_t            OUT_LEN = data.d_outputLength;
            const char            *OUTPUT  = data.d_output_p;
            const bool             PADDED  = tti % 2;    tti /= 2;
            const Alphabet::Enum   ALPHA   = static_cast<Alphabet::Enum>(
                                                         tti % 2);    tti /= 2;
            ASSERT(0 == tti);
            done |= NUM_DATA - 1 == dIdx && PADDED && ALPHA;

            const EncoderOptions&  OPTIONS = EncoderOptions::custom(
                                                             MAX_LEN,
                                                             ALPHA,
                                                             PADDED);

            if (!PADDED) {
                // Create string 'unpadded' with the padding stripped off
                // the end, and substitute that to 'OUTPUT and OUT_LEN;

                static bsl::string unpadded;
                unpadded = OUTPUT;
                while (!unpadded.empty() &&
                                       bsl::strchr("\r\n=", unpadded.back())) {
                    unpadded.resize(unpadded.length() - 1);
                }
                OUTPUT  = unpadded.c_str();
                OUT_LEN = unpadded.length();
            }

            const char *const B = INPUT;
            const char *const E = INPUT + IN_LEN;

            const int OUTPUT_BUFFER_SIZE = 100; // overrun will be detected
            const int TRAILING_OUTPUT_WINDOW = 30; // detect extra output

            Obj obj(OPTIONS);

            const int newDepth = IN_LEN + MAX_LEN;

            // The following partitions the table in verbose mode.
            if (newDepth < depth) {                 // table entires "Part Two"
                if (verbose) cout <<
                                    "\nVerifying Maximum Line Length." << endl;
            }
            if (newDepth != depth) {
                if (verbose) cout << "\tDepth = " << newDepth << endl;
                depth = newDepth;
            }
            if (veryVerbose) {
                T_ T_ P_(ti)
                P_(LINE) printCharN(cout, INPUT, IN_LEN) << endl;
            }

            // The first thing to do is to check expected output length.
            const bsl::size_t CALC_LEN = Obj::encodedLength(OPTIONS, IN_LEN);
            ASSERTV(LINE, PADDED, OUTPUT, OUT_LEN, CALC_LEN,
                                                          OUT_LEN == CALC_LEN);

            // Define the output buffer and initialize it.
            char outputBuffer[OUTPUT_BUFFER_SIZE];
            memset(outputBuffer, '?', sizeof outputBuffer);

            char *b = outputBuffer;
            int   nOut = -1;
            int   nIn = -1;
            LOOP_ASSERT(LINE, 0 == obj.convert(b, &nOut, &nIn, B, E));
            LOOP_ASSERT(LINE, IN_LEN == nIn);

            // Prepare to call 'endConvert'.
            unsigned totalOut = nOut;
            b += nOut;
            LOOP_ASSERT(LINE, 0 == obj.endConvert(b, &nOut));
            totalOut += nOut;
            LOOP3_ASSERT(LINE, OUT_LEN, totalOut, OUT_LEN == totalOut);

            // Capture and verify internal output length.
            const unsigned internalLen = obj.outputLength();
            LOOP2_ASSERT(LINE, internalLen, OUT_LEN == internalLen);

            // Confirm final state is e_DONE_STATE.
            LOOP_ASSERT(LINE, isState(&obj, e_DONE_STATE));

            if (veryVeryVerbose) {
                cout << "\t\t\tExpected output: ";
                    printCharN(cout, OUTPUT, OUT_LEN) << endl;
                cout << "\t\t\t  Actual output: ";
                    printCharN(cout, outputBuffer, totalOut) << endl;
            }
            LOOP_ASSERT(LINE, 0 == memcmp(OUTPUT, outputBuffer, OUT_LEN));

            // Verify nothing written past end of actual output.
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

            // For each index in [0, IN_LEN], partition the input into two
            // sequences, apply these sequences, in turn, to a newly
            // created instance, and verify that the result is identical to
            // that of the original (unpartitioned) sequence.

            for (int index = 0; index <= IN_LEN; ++index) {
                if (veryVeryVerbose) { T_ T_ T_ T_ P(index) }

                Obj               localObj(OPTIONS);
                const char *const M = B + index;
                char              localBuf[sizeof outputBuffer];
                memset(localBuf, '$', sizeof localBuf);
                char             *lb = localBuf;
                int               localNumIn;
                int               localNumOut;

                if (veryVeryVeryVerbose) {
                    cout << "\t\t\t\t\t" << "Input 1: ";
                    printCharN(cout, B, static_cast<int>(M - B)) << endl;
                }

                int res1 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         B, M);
                LOOP2_ASSERT(LINE, index, 0 == res1);

                // Prepare for second call to 'convert'.
                int      localTotalIn = localNumIn;
                unsigned localTotalOut = localNumOut;
                lb += localNumOut;

                if (veryVeryVeryVerbose) {
                    cout << "\t\t\t\t\t" << "Input 2: ";
                    printCharN(cout, M, static_cast<int>(E - M)) << endl;
                }

                int res2 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         M, E);
                LOOP2_ASSERT(LINE, index, 0 == res2);

                // Prepare to call 'endConvert'.
                localTotalIn  += localNumIn;
                localTotalOut += localNumOut;
                lb += localNumOut;

                int res3 = localObj.endConvert(lb, &localNumOut);
                LOOP2_ASSERT(LINE, index, 0 == res3);
                localTotalOut += localNumOut;

                // Compare internal output lengths.
                const unsigned localLen = localObj.outputLength();
                ASSERTV(LINE, internalLen, localLen, internalLen == localLen);

                // Confirm final state is e_DONE_STATE.
                LOOP2_ASSERT(LINE, index, isState(&localObj, e_DONE_STATE));

                // Verify total amount of input consumed is the same.
                ASSERTV(LINE, index, nIn, localTotalIn, nIn == localTotalIn);

                int cmpStatus = memcmp(outputBuffer, localBuf, totalOut);
                if (cmpStatus || localTotalOut != totalOut ||
                                                         veryVeryVeryVerbose) {
                    cout << "\t\t\t\t\tExpected output: ";
                    printCharN(cout, outputBuffer, totalOut) << endl;
                    cout << "\t\t\t\t\t  Actual output: ";
                    printCharN(cout, localBuf, localTotalOut) << endl;
                }

                ASSERTV(LINE, index, totalOut, localTotalOut,
                                                    totalOut == localTotalOut);
                LOOP2_ASSERT(LINE, index, 0 == cmpStatus);

                // Verify nothing written past end of actual output.
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
        ASSERT(done);

        if (verbose) cout << "Negative Testing\n";
        {
            bsls::AssertTestHandlerGuard  guard;

            const size_t maxSize_t = bsl::numeric_limits<size_t>::max();

            // 'encodedLength'

            ASSERT_FAIL(Obj::encodedLength(EncoderOptions::urlSafe(), -1));

            size_t limit = (maxSize_t / 4) * 3 + 2;

            ASSERT_PASS(Obj::encodedLength(EncoderOptions::urlSafe(), limit));
            ASSERT_FAIL(Obj::encodedLength(EncoderOptions::urlSafe(),
                                           limit + 1));

            ASSERT_FAIL(Obj::encodedLength(EncoderOptions::mime(), -1));

            limit = ((maxSize_t / 78)) * 76 / 4 * 3 +
                                      (sizeof(int) == sizeof(size_t) ? 15 : 9);
            ASSERT_PASS(Obj::encodedLength(EncoderOptions::mime(), limit));
            ASSERT_FAIL(Obj::encodedLength(EncoderOptions::mime(), limit + 1));

            // 'encodedLines'

            ASSERT_FAIL(Obj::encodedLines(EncoderOptions::mime(), -1));

            limit = ((maxSize_t - 4) / 4 + 1) * 3 - 2 + 2;

            ASSERT_PASS(Obj::encodedLines(EncoderOptions::mime(), limit));
            ASSERT_FAIL(Obj::encodedLines(EncoderOptions::mime(), limit + 1));

            // Result is 1 if line length is 0

            ASSERT_PASS(Obj::encodedLines(EncoderOptions::urlSafe(), 0));
            ASSERT(1 == Obj::encodedLines(EncoderOptions::urlSafe(), 0));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //
        // Concern:
        //: 1 Run the fuzz test routine, but not as a true fuzz test, just feed
        //:   it some random input to do basic debugging.
        //
        // Plan:
        //: 1 Call the fuzz test routine, many times, with a big garbage
        //:   string.
        //:
        //: 2 If 'verbose' is set and 'argv[2]' is a file, read the file to a
        //:   string and feed that into the fuzz test.
        //
        // Testing:
        //   int LLVMFuzzerTestOneInput(const uint8_t *, size_t);
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST MACHINERY\n"
                             "==============\n";

        enum { k_BUF_LEN = 128 };
        bsl::vector<uint8_t> buf, asciiBuf;
        buf.resize(     k_BUF_LEN);
        asciiBuf.resize(k_BUF_LEN);

        u::RandGen rand;

        for (int ii = 0; ii < 64; ++ii) {
            for (unsigned uu = 0; uu < k_BUF_LEN; uu += 4) {
                unsigned r = rand();
                bsl::memcpy(&buf[uu],      &r, sizeof(r));
                r &= 0x7f7f7f7f;
                bsl::memcpy(&asciiBuf[uu], &r, sizeof(r));
            }

            for (unsigned len = 0 != ii; len <= k_BUF_LEN;
                                                     len += 1 + (rand() % 6)) {
                const int gap = k_BUF_LEN - len;
                int rc = LLVMFuzzerTestOneInput(buf.data() + gap, len);
                ASSERT(0 == rc && "buf");
                rc = LLVMFuzzerTestOneInput(asciiBuf.data() + gap, len);
                ASSERT(0 == rc && "asciiBuf");
            }
        }

        if (verbose) {
            bsl::fstream ifs(argv[2],
                             bsl::ios_base::binary | bsl::ios_base::in);
            P(ifs.good());
            if (ifs.good()) {
                bsl::string fileContents;
                while (true) {
                    char c;
                    ifs.get(c);
                    if (ifs.eof()) {
                        break;
                    }
                    fileContents += c;
                }

                if (veryVerbose) P_(fileContents.length()); P(fileContents);

                int rc = LLVMFuzzerTestOneInput(
                        reinterpret_cast<const uint8_t *>(fileContents.data()),
                        fileContents.length());
                ASSERT(0 == rc);
            }
        }
      } break;
      case 6: {
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
        //   - Loop-Based Implementation Technique
        //
        // Testing:
        //   That each internal table has no defective entires.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY INTERNAL TABLES" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nVerify Encoding Table (64 entires)." << endl;
        {
            char              input;
            const char *const B = &input, *const E = B + 1;

            char b[4];
            int  nOut;
            int  nIn;

            int  end = 0;

            // --------------------- Base64 alhabet ---------------------------

            if (verbose) cout << "\t\"base64\": Verify Entries [0-25]."
                              << endl;
            int origStart = end;
            for (int pi = 0; pi < 2; ++pi) {
                const bool pad = pi;
                int start = origStart;
                end = start + 26;
                for (int i = start; i < end; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, pad));
                    bsl::memset(b, '?', sizeof(b));
                    input = char(4 * i);
                    LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                    LOOP_ASSERT(i, 1 == nOut);
                    LOOP_ASSERT(i, 1 == nIn);
                    LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                    LOOP_ASSERT(i, (pad ? 3 : 1) == nOut);
                    LOOP_ASSERT(i, 'A' + i - start == b[0]);
                    LOOP_ASSERT(i, 'A' == b[1]);
                    LOOP_ASSERT(i, (pad ? '=' : '?') == b[2]);
                    LOOP_ASSERT(i, (pad ? '=' : '?') == b[3]);
                }
            }

            if (verbose) cout << "\t\"base64\": Verify Entries [26-51]."
                              << endl;
            origStart = end;
            for (int pi = 0; pi < 2; ++pi) {
                const bool pad = pi;
                int start = origStart;
                end = start + 26;
                for (int i = start; i < end; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, pad));
                    bsl::memset(b, '?', sizeof(b));
                    input = char(4 * i);
                    LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                    LOOP_ASSERT(i, 1 == nOut);
                    LOOP_ASSERT(i, 1 == nIn);
                    LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                    ASSERTV(i, nOut, (pad ? 3 : 1) == nOut);
                    ASSERTV(i, 'a' + i - start == b[0]);
                    ASSERTV(i, 'A' == b[1]);
                    ASSERTV(i, b[2], pad, (pad ? '=' : '?') == b[2]);
                    ASSERTV(i, b[3], pad, (pad ? '=' : '?') == b[3]);
                }
            }

            if (verbose) cout << "\t\"base64\": Verify Entries [52-61]."
                              << endl;
            origStart = end;
            for (int pi = 0; pi < 2; ++pi) {
                const bool pad = pi;
                int start = origStart;
                end = start + 10;
                for (int i = start; i < end; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, pad));
                    bsl::memset(b, '?', sizeof(b));
                    input = char(4 * i);
                    LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                    LOOP_ASSERT(i, 1 == nOut);
                    LOOP_ASSERT(i, 1 == nIn);
                    LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                    ASSERTV(i, nOut, (pad ? 3 : 1) == nOut);
                    ASSERTV(i, '0' + i - start == b[0]);
                    ASSERTV(i, 'A' == b[1]);
                    ASSERTV(i, b[2], pad, (pad ? '=' : '?') == b[2]);
                    ASSERTV(i, b[3], pad, (pad ? '=' : '?') == b[3]);
                }
            }

            if (verbose) cout << "\t\"base64\": Verify Entry [62]."
                              << endl;
            origStart = end;
            for (int pi = 0; pi < 2; ++pi) {
                const bool pad = pi;
                int start = origStart;
                end = start + 1;
                for (int i = start; i < end; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, pad));
                    bsl::memset(b, '?', sizeof(b));
                    input = char(4 * i);
                    LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                    LOOP_ASSERT(i, 1 == nOut);
                    LOOP_ASSERT(i, 1 == nIn);
                    LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                    ASSERTV(i, nOut, (pad ? 3 : 1) == nOut);
                    ASSERTV(i, '+' + i - start == b[0]);
                    ASSERTV(i, 'A' == b[1]);
                    ASSERTV(i, b[2], pad, (pad ? '=' : '?') == b[2]);
                    ASSERTV(i, b[3], pad, (pad ? '=' : '?') == b[3]);
                }
            }

            if (verbose) cout << "\t\"base64\": Verify Entry [63]."
                              << endl;
            origStart = end;
            for (int pi = 0; pi < 2; ++pi) {
                const bool pad = pi;
                int start = origStart;
                end = start + 1;
                for (int i = start; i < end; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, pad));
                    bsl::memset(b, '?', sizeof(b));
                    input = char(4 * i);
                    LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                    LOOP_ASSERT(i, 1 == nOut);
                    LOOP_ASSERT(i, 1 == nIn);
                    LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                    ASSERTV(i, nOut, (pad ? 3 : 1) == nOut);
                    ASSERTV(i, '/' + i - start == b[0]);
                    ASSERTV(i, 'A' == b[1]);
                    ASSERTV(i, b[2], pad, (pad ? '=' : '?') == b[2]);
                    ASSERTV(i, b[3], pad, (pad ? '=' : '?') == b[3]);
                }
            }

            ASSERT(64 == end);  // make sure all entires are accounted for.

            // ------------------- Base64url alhabet --------------------------

            end = 0;

            if (verbose) cout << "\t\"base64url\": Verify Entries [0-25]."
                              << endl;
            int start = end;
            end = start + 26;
            for (int i = start; i < end; ++i) {
                if (veryVerbose) { T_ T_ P(i) }
                Obj obj(EncoderOptions::urlSafe());
                bsl::memset(b, '?', sizeof(b));
                input = char(4 * i);
                LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(i, 1 == nOut);
                LOOP_ASSERT(i, 1 == nIn);
                LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                ASSERTV(i, nOut, 1 == nOut);
                ASSERTV(i, 'A' + i - start == b[0]);
                ASSERTV(i, 'A' == b[1]);
                ASSERTV(i, b[2], '?' == b[2]);
                ASSERTV(i, b[2], '?' == b[3]);
            }

            if (verbose) cout << "\t\"base64url\": Verify Entries [26-51]."
                              << endl;
            origStart = end;
            start = end;
            end = start + 26;
            for (int i = start; i < end; ++i) {
                if (veryVerbose) { T_ T_ P(i) }
                Obj obj(EncoderOptions::urlSafe());
                bsl::memset(b, '?', sizeof(b));
                input = char(4 * i);
                LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(i, 1 == nOut);
                LOOP_ASSERT(i, 1 == nIn);
                LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                ASSERTV(i, nOut, 1 == nOut);
                ASSERTV(i, 'a' + i - start == b[0]);
                ASSERTV(i, 'A' == b[1]);
                ASSERTV(i, b[2], '?' == b[2]);
                ASSERTV(i, b[2], '?' == b[3]);
            }

            if (verbose) cout << "\t\"base64url\": Verify Entries [52-61]."
                              << endl;
            start = end;
            end = start + 10;
            for (int i = start; i < end; ++i) {
                if (veryVerbose) { T_ T_ P(i) }
                Obj obj(EncoderOptions::urlSafe());
                bsl::memset(b, '?', sizeof(b));
                input = char(4 * i);
                LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(i, 1 == nOut);
                LOOP_ASSERT(i, 1 == nIn);
                LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                ASSERTV(i, nOut, 1 == nOut);
                ASSERTV(i, '0' + i - start == b[0]);
                ASSERTV(i, 'A' == b[1]);
                ASSERTV(i, b[2], '?' == b[2]);
                ASSERTV(i, b[3], '?' == b[3]);
            }

            if (verbose) cout << "\t\"base64url\": Verify Entry [62]."
                              << endl;
            start = end;
            end = start + 1;
            for (int i = start; i < end; ++i) {
                if (veryVerbose) { T_ T_ P(i) }
                Obj obj(EncoderOptions::urlSafe());
                bsl::memset(b, '?', sizeof(b));
                input = char(4 * i);
                LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(i, 1 == nOut);
                LOOP_ASSERT(i, 1 == nIn);
                LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                ASSERTV(i, nOut, 1 == nOut);
                ASSERTV(i, '-' + i - start == b[0]);
                ASSERTV(i, 'A' == b[1]);
                ASSERTV(i, b[2], '?' == b[2]);
                ASSERTV(i, b[3], '?' == b[3]);
            }

            if (verbose) cout << "\t\"base64url\": Verify Entry [63]."
                              << endl;
            start = end;
            end = start + 1;
            for (int i = start; i < end; ++i) {
                if (veryVerbose) { T_ T_ P(i) }
                Obj obj(EncoderOptions::urlSafe());
                bsl::memset(b, '?', sizeof(b));
                input = char(4 * i);
                LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                LOOP_ASSERT(i, 1 == nOut);
                LOOP_ASSERT(i, 1 == nIn);
                LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                ASSERTV(i, nOut, 1 == nOut);
                ASSERTV(i, '_' + i - start == b[0]);
                ASSERTV(i, 'A' == b[1]);
                ASSERTV(i, b[2], '?' == b[2]);
                ASSERTV(i, b[3], '?' == b[3]);
            }

            ASSERT(64 == end);  // make sure all entires are accounted for.
        }

      } break;
      case 5: {
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
        //   - That all input characters are consumed.
        //
        // Plan:
        //   For inputs counts of increasing value starting with 0, put the
        //   object in each of the possible states, supply an input of the
        //   current length and, after calling 'convert', verify that the
        //   return code and state are as expected, and that all of the input
        //   was consumed.
        //
        // Tactics:
        //   - Depth-Ordered Enumeration Data Selection Method
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
                int d_lineNum;          // source line number
                int d_startState;       // indicated starting state
                int d_numInputs;        // number of input characters
                int d_endState;         // expected ending state

            } DATA[] = {//-------input-------v  v--output--v
                //lin  Starting State  #inputs  Ending State
                //---  --------------  -------  ------------
                { L_,  e_INITIAL_STATE,  0,       e_INITIAL_STATE },
                { L_,  e_STATE_ONE,      0,       e_STATE_ONE     },
                { L_,  e_STATE_TWO,      0,       e_STATE_TWO     },
                { L_,  e_STATE_THREE,    0,       e_STATE_THREE   },
                { L_,  e_DONE_STATE,     0,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    0,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  1,       e_STATE_ONE     },
                { L_,  e_STATE_ONE,      1,       e_STATE_TWO     },
                { L_,  e_STATE_TWO,      1,       e_STATE_THREE   },
                { L_,  e_STATE_THREE,    1,       e_STATE_ONE     },
                { L_,  e_DONE_STATE,     1,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    1,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  2,       e_STATE_TWO     },
                { L_,  e_STATE_ONE,      2,       e_STATE_THREE   },
                { L_,  e_STATE_TWO,      2,       e_STATE_ONE     },
                { L_,  e_STATE_THREE,    2,       e_STATE_TWO     },
                { L_,  e_DONE_STATE,     2,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    2,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  3,       e_STATE_THREE   },
                { L_,  e_STATE_ONE,      3,       e_STATE_ONE     },
                { L_,  e_STATE_TWO,      3,       e_STATE_TWO     },
                { L_,  e_STATE_THREE,    3,       e_STATE_THREE   },
                { L_,  e_DONE_STATE,     3,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    3,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  4,       e_STATE_ONE     },
                { L_,  e_STATE_ONE,      4,       e_STATE_TWO     },
                { L_,  e_STATE_TWO,      4,       e_STATE_THREE   },
                { L_,  e_STATE_THREE,    4,       e_STATE_ONE     },
                { L_,  e_DONE_STATE,     4,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    4,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  5,       e_STATE_TWO     },
                { L_,  e_STATE_ONE,      5,       e_STATE_THREE   },
                { L_,  e_STATE_TWO,      5,       e_STATE_ONE     },
                { L_,  e_STATE_THREE,    5,       e_STATE_TWO     },
                { L_,  e_DONE_STATE,     5,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    5,       e_ERROR_STATE   },

                { L_,  e_INITIAL_STATE,  6,       e_STATE_THREE   },
                { L_,  e_STATE_ONE,      6,       e_STATE_ONE     },
                { L_,  e_STATE_TWO,      6,       e_STATE_TWO     },
                { L_,  e_STATE_THREE,    6,       e_STATE_THREE   },
                { L_,  e_DONE_STATE,     6,       e_ERROR_STATE   },
                { L_,  e_ERROR_STATE,    6,       e_ERROR_STATE   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int  OUTPUT_SIZE = 16;
            const char INPUT[] = "ABCDEF";

            char b[OUTPUT_SIZE];
            int  nOut;
            int  nIn;

            const char *const B = INPUT;

            int lastNumInputs = -1;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int START = DATA[ti].d_startState;
                const int COUNT = DATA[ti].d_numInputs;
                const int END   = DATA[ti].d_endState;
                const int RTN = -(e_ERROR_STATE == END);
                const char *const E = B + COUNT;

                Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, true));

                if (COUNT != lastNumInputs) {
                    if (verbose) cout << '\t' << COUNT << " input character"
                                      << (1 == COUNT ? "." : "s.") << endl;
                    lastNumInputs = COUNT;
                }
                setState(&obj, START);

                if (veryVerbose) cout << "\t\t" << STATE_NAMES[START] << endl;

                if (veryVeryVerbose) { cout
                    << "\t\t\tExpected end state: " << STATE_NAMES[END] << endl
                    << "\t\t\tExpected return status: " << RTN << endl;
                }

                LOOP_ASSERT(LINE, RTN == obj.convert(b, &nOut, &nIn, B, E));
                LOOP2_ASSERT(LINE, nOut, OUTPUT_SIZE > nOut);
                LOOP_ASSERT(LINE, isState(&obj, END));

                // Verify amount of input consumed: all or none.
                const bool VALID = START != e_DONE_STATE &&
                                                        START != e_ERROR_STATE;
                const int  EXP_NUM_IN = VALID ? COUNT : 0;
                LOOP2_ASSERT(LINE, nIn, EXP_NUM_IN == nIn);

            } // end for ti
        } // end block

      } break;
      case 4: {
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
        //   BOOTSTRAP: 'endConvert' - transitions
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
                { L_,  e_INITIAL_STATE,  e_DONE_STATE  },
                { L_,  e_STATE_ONE,      e_DONE_STATE  },
                { L_,  e_STATE_TWO,      e_DONE_STATE  },
                { L_,  e_STATE_THREE,    e_DONE_STATE  },
                { L_,  e_DONE_STATE,     e_ERROR_STATE },
                { L_,  e_ERROR_STATE,    e_ERROR_STATE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char b[4];
            int  numOut;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int START = DATA[ti].d_startState;
                const int END   = DATA[ti].d_endState;
                const int RTN = -(e_ERROR_STATE == END);

                Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, true));

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

      } break;
      case 3: {
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
        //   test helper function.  Then use of all of the (as yet untested)
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
        //   the primary manipulators which have not yet been fully tested.
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
                if (verbose) cout << "\te_INITIAL_STATE." << endl;

                Obj obj(EncoderOptions::custom(9, Alphabet::e_BASIC, true));

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9 == obj.options().maxLineLength());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(true  == obj.options().isPadded());
                ASSERT(true  == obj.isPadded());
        }

        if (verbose) cout << "\nVerify ::setState." << endl;
        {

            if (verbose) cout << "\te_INITIAL_STATE." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_INITIAL_STATE);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9 == obj.options().maxLineLength());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                // e_DONE_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9 == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
                ASSERT((char)-1 == b[3]);
            }

            if (verbose) cout << "\tState 1." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_STATE_ONE);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9 == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b + 1, &numOut);

                // e_DONE_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERTV(isPadded, 1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT((isPadded ? 4 : 2) == obj.outputLength());
                ASSERTV(isPadded, result, 0 == result);
                ASSERT((isPadded ? 3 : 1) == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT('A' == b[1]);
                ASSERT((isPadded ? '=' : (char)-1) == b[2]);
                ASSERT((isPadded ? '=' : (char)-1) == b[3]);
            }

            if (verbose) cout << "\tState 2." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_STATE_TWO);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b + 2, &numOut);

                // e_DONE_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT((isPadded ? 4 : 3) == obj.outputLength());
                ASSERT(0 == result);
                ASSERT((isPadded ? 2 : 1) == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT('A' == b[2]);
                ASSERT((isPadded ? '=' : (char)-1) == b[3]);
            }

            if (verbose) cout << "\tState 3." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_STATE_THREE);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(4 == obj.outputLength());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                // e_DONE_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(4 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
                ASSERT((char)-1 == b[3]);
            }

            if (verbose) cout << "\te_DONE_STATE." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_DONE_STATE);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(1 == obj.isAcceptable());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                // e_ERROR_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-1 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
                ASSERT((char)-1 == b[3]);
            }

            if (verbose) cout << "\te_ERROR_STATE." << endl;
            for (int tp = 0; tp < 2; ++tp) {
                const bool isPadded = tp;

                Obj obj(EncoderOptions::custom(
                                              9, Alphabet::e_BASIC, isPadded));
                setState(&obj, e_ERROR_STATE);

                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[4] = { -1, -1, -1, -1 };
                int  numOut = -1;
                int  result = obj.endConvert(b, &numOut);

                // e_ERROR_STATE
                ASSERT(9 == obj.maxLineLength());
                ASSERT(9        == obj.options().maxLineLength());
                ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
                ASSERT(Alphabet::e_BASIC == obj.alphabet());
                ASSERT(isPadded == obj.options().isPadded());
                ASSERT(isPadded == obj.isPadded());
                ASSERT(0 == obj.isAcceptable());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-1 == result);
                ASSERT(0 == numOut);
                ASSERT((char)-1 == b[0]);
                ASSERT((char)-1 == b[1]);
                ASSERT((char)-1 == b[2]);
                ASSERT((char)-1 == b[3]);
            }
        }

        if (verbose) cout << "\nVerify ::isState." << endl;
        {
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

                    Obj obj(EncoderOptions::custom(0,
                                                   Alphabet::e_BASIC,
                                                   true));
                    setState(&obj, i);
                    LOOP2_ASSERT(i, j, SAME == isState(&obj, j));
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR AND CONFIGURATION STATE ACCESSORS
        //   Make sure we can bring the object to any attainable state.
        //
        // Concerns:
        //   That we can fully configure the object from the constructor.
        //
        // Plan:
        //   Create the object in several configurations and verify using
        //   all of the (as yet untested) direct accessors.  After this test
        //   case, we can declare the accessors returning configuration state
        //   to be thoroughly tested.
        //
        // Tactics:
        //   - Boundary Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlde::Base64Encoder(int maxLineLength);
        //   int maxLineLength() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;


        if (verbose) cout << "\nTry a few different settings." << endl;

        if (verbose) cout << "\tmaxLineLength = 0, default alphabet" << endl;
        {
            Obj obj(EncoderOptions::custom(0, Alphabet::e_BASIC, true));
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(0 == obj.maxLineLength());
            ASSERT(0 == obj.options().maxLineLength());
            ASSERT(0 == obj.outputLength());
            ASSERT(Alphabet::e_BASIC == obj.alphabet());
            ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
            ASSERT(true  == obj.options().isPadded());
            ASSERT(true  == obj.isPadded());
        }
        if (verbose) cout << "\tmaxLineLength = 1, default alphabet" << endl;
        {
            Obj obj(EncoderOptions::custom(1, Alphabet::e_BASIC, true));
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(1 == obj.maxLineLength());
            ASSERT(1 == obj.options().maxLineLength());
            ASSERT(0 == obj.outputLength());
            ASSERT(Alphabet::e_BASIC == obj.alphabet());
            ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
            ASSERT(true  == obj.options().isPadded());
            ASSERT(true  == obj.isPadded());
        }
        if (verbose) cout << "\tmaxLineLength = 2, \"base64\" alphabet"
                          << endl;
        {
            Obj obj(EncoderOptions::custom(2, Alphabet::e_BASIC, true));
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(2 == obj.maxLineLength());
            ASSERT(2 == obj.options().maxLineLength());
            ASSERT(0 == obj.outputLength());
            ASSERT(Alphabet::e_BASIC == obj.alphabet());
            ASSERT(Alphabet::e_BASIC == obj.options().alphabet());
            ASSERT(true  == obj.options().isPadded());
            ASSERT(true  == obj.isPadded());
        }
        if (verbose) cout << "\tmaxLineLength = INT_MAX, \"base64url\" "
                          << "alphabet" << endl;
        {
            Obj obj(EncoderOptions::custom(INT_MAX, Alphabet::e_URL, true));
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(INT_MAX == obj.maxLineLength());
            ASSERT(INT_MAX == obj.options().maxLineLength());
            ASSERT(0 == obj.outputLength());
            ASSERT(Alphabet::e_URL == obj.alphabet());
            ASSERT(Alphabet::e_URL == obj.options().alphabet());
            ASSERT(true  == obj.options().isPadded());
            ASSERT(true  == obj.isPadded());
        }
        if (verbose) cout << "\turlSafe\n";
        {
            Obj obj(EncoderOptions::urlSafe());
            ASSERT(1 == obj.isAcceptable());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(0 == obj.maxLineLength());
            ASSERT(0 == obj.options().maxLineLength());
            ASSERT(0 == obj.outputLength());
            ASSERT(Alphabet::e_URL == obj.alphabet());
            ASSERT(Alphabet::e_URL == obj.options().alphabet());
            ASSERT(false == obj.options().isPadded());
            ASSERT(false == obj.isPadded());
        }
      } break;
      case 1: {
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
            bsl::ostringstream out;

            const char  in[] = "a" "\x00" "b" "\x07" "c" "\x08" "d" "\x0F"
                               "e" "\x10" "f" "\x80" "g" "\xFF";

            printCharN(out, in, sizeof in) << ends;

            const char EXP[] = "a<00>b<07>c<08>d<0F>e<10>f<80>g<FF><00>";

            if (veryVerbose) {
                cout << "\tRESULT = " << out.str().c_str() << endl;
                cout << "\tEXPECT = " << EXP << endl;
            }
            ASSERT(0 == strncmp(EXP, out.str().c_str(), sizeof EXP));
        }

        if (verbose) cout << "\nTry instantiating an encoder." << endl;
        {
            Obj encoder;
            if (veryVerbose) {
                T_ P(encoder.isAcceptable());
                T_ P(encoder.isDone());
                T_ P(encoder.isError());
                T_ P(encoder.isInitialState());
                T_ P(encoder.maxLineLength());
                T_ P(encoder.outputLength());
            }

            if (verbose) cout << "\nEncode something." << endl;

            static char out[1000];
            int         outIdx = 0;
            int         numIn = 0;
            int         numOut = 0;

            //              begin:     0     1     3       4       8    9 END
            //              end  :     1     3     4       8       9   10 INPUT
            const char*const input = "\0" "\0\0" "\0" "\0\0\0\0" "\0" "\0";

            ASSERT(0 == numOut); ASSERT(0 == numIn); ASSERT(0 == outIdx);
            ASSERT(0 == out[0]); ASSERT(0 == out[1]);

            if (verbose) cout << "\tEncode: ^0." << endl;
            {
                const char *const begin = input + 0;
                const char *const end   = input + 1;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(1 == numIn); ASSERT(1 == outIdx);
            ASSERT('A' == out[0]); ASSERT(0 == out[1]);

            if (verbose) cout << "\tEncode: 0^00." << endl;
            {
                const char *const begin = input + 1;
                const char *const end   = input + 3;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(3 == numOut); ASSERT(2 == numIn); ASSERT(4 == outIdx);
            ASSERT('A' == out[3]); ASSERT(0 == out[4]);

            if (verbose) cout << "\tEncode: 000^0." << endl;
            {
                const char *const begin = input + 3;
                const char *const end   = input + 4;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(1 == numIn); ASSERT(5 == outIdx);
            ASSERT('A' == out[3]); ASSERT('A' == out[4]); ASSERT(0 == out[5]);

            if (verbose) cout << "\tEncode: 0000^0000." << endl;
            {
                const char *const begin = input + 4;
                const char *const end   = input + 8;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(5 == numOut); ASSERT(4 == numIn); ASSERT(10 == outIdx);
            ASSERT('A' == out[7]); ASSERT('A' == out[8]);
            ASSERT('A' == out[9]); ASSERT(0 == out[10]);

            if (verbose) cout << "\tEncode: 00000000^0." << endl;
            {
                const char *const begin = input + 8;
                const char *const end   = input + 9;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(2 == numOut); ASSERT(1 == numIn); ASSERT(12 == outIdx);
            ASSERT('A' == out[11]); ASSERT(0 == out[12]);

            if (verbose) cout << "\tEncode: 000000000^0." << endl;
            {
                const char *const begin = input + 9;
                const char *const end   = input + 10;

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(1 == numOut); ASSERT(1 == numIn); ASSERT(13 == outIdx);
            ASSERT('A' == out[11]); ASSERT('A' == out[12]);
            ASSERT(0 == out[13]);

            if (verbose) cout << "\tEncode: 0000000000^$." << endl;
            {
                encoder.endConvert(out + outIdx, &numOut);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(3 == numOut); ASSERT(16 == outIdx);
            ASSERT('A' == out[12]); ASSERT('A' == out[13]);
            ASSERT('=' == out[14]); ASSERT('=' == out[15]);
            ASSERT(0 == out[16]);
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
