// bdlde_quotedprintabledecoder.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_quotedprintabledecoder.h>

#include <bslmf_assert.h>

#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // memset()
#include <bsl_cctype.h>    // isprint(), toupper(), etc.
#include <bsl_iostream.h>
#include <bsl_limits.h>    // INT_MAX
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;                // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This encoder is a kind of 'mechanism' that can be customized at
// construction.  Since there is currently no prevision to change this
// customization after construction, the choice for primary constructor must
// permit permits this customization:
//..
//  bdepu::Base64Encoder(int maxLineLength);
//..
// The primary manipulators are those needed to reach every attainable state.
// Clearly 'convert' is one, but 'endConvert' is also needed in order to reach
// the 'DONE' state.  Hence, the primary manipulators are
//..
//  int convert(char *out, int *numOut, int *ni, const char *b, const char *e);
//  int endConvert(char *out, int *numOut);
//..
// Though not strictly primary, the 'reset' method resets the object under to
// its initial state (i.e., to that immediately following construction).  It
// will turn out to be convenient to test 'reset' along with all the other
// state transitions imposed by 'convert' and 'endConvert' early in the testing
// process.
//..
// The basic accessers for the encoder are all the functions that return
// information about the customization and/or execution state:
//..
//  bool isAccepting() const;
//  bool isError() const;
//  bool isInitialState() const;
//  int maxLineLength() const;
//  int outputLength() const;
//..
// The following table illustrates major state transitions for all three of the
// processing manipulators:
//..
//                      convert (1)     endConvert      reset
//                      -----------     ----------      -----
//      INITIAL_STATE:  State 1         DONE_STATE      INITIAL_STATE
//      State 1:        State 2         DONE_STATE      INITIAL_STATE
//      State 2:        State 3         DONE_STATE      INITIAL_STATE
//      State 3:        State 1         DONE_STATE      INITIAL_STATE
//      DONE_STATE:     ERROR_STATE     ERROR_STATE     INITIAL_STATE
//      ERROR_STATE:    ERROR_STATE     ERROR_STATE     INITIAL_STATE
//..
// Our first step will be to ensure that each of these states can be reached
// ('::setState'), that an anticipated state can be verified ('::isState'), and
// that each of the above state transitions (including 'reset') is verified.
// Next, we will ensure that each internal table is correct.  Finally, using
// category partitioning, we enumerate a representative collection of inputs
// ordered by length (plus MaxLineLength) that will be sufficient to prove that
// the logic associated with the state machine is performing as desired.
//
// Note that Because the 'convert' and 'endConvert' methods are parametrized
// based on iterator types, we will want to ensure (at compile time) that their
// respective implementations do not depend on more than minimal iterator
// functionality.  We will accomplish this goal by supplying, as template
// arguments, 'bdeut::InputIterator' for 'convert' and 'bdeut::OutputIterator'
// for both of these template methods.
//-----------------------------------------------------------------------------
// [ 7] static int encodedLength(int numInputBytes, int maxLineLength);
// [ 8] bdepu::Base64Encoder();
// [ 2] bdepu::Base64Encoder(int maxLineLength);
// [ 3] ~bdepu::Base64Encoder();
// [ 7] int convert(char *o, int *no, int*ni, const char*b, const char*e);
// [ 7] int endConvert(char *out, int *numOut);
// [ 3] void reset();
// [ 3] bool isAccepting() const;
// [ 3] bool isError() const;
// [ 3] bool isInitialState() const;
// [ 2] int maxLineLength() const;
// [ 3] int outputLength() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [  ] USAGE EXAMPLE
// [ ?] That the input iterator can have *minimal* functionality.
// [ ?] That the output iterator can have *minimal* functionality.
// [ 5] BOOTSTRAP: 'convert' - transitions
// [ 4] BOOTSTRAP: 'endConvert'- transitions
// [ 3] That we can reach each of the major processing states.
// [ 1] ::myMin(const T& a, const T& b);
// [ 1] ::printCharN(ostream& output, const char* sequence, int length)
// [ 3] void ::setState(Obj *obj, int state, const char *input);
// [ 3] bool ::isState(Obj *obj, int state);
// [ 6] That each internal table has no defective entries.
// [ 7] That each bit of a 3-byte quantum finds its appropriate spot.
// [ 7] That each bit of a 2-byte quantum finds its appropriate spot.
// [ 7] That each bit of a 1-byte quantum finds its appropriate spot.
// [ 7] That output length is calculated properly.
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)
#define V(X) { if (verbose) { cout << "\t" << X << endl; } } // no () to allow
                                                             // stringing <<
#define VV(X) { if (veryVerbose) { cout << "\t\t" << X << endl; } }
#define VVV(X) { if (veryVeryVerbose) { cout << "\t\t\t" << X << endl; } }
#define VVVV(X) { if (veryVeryVeryVerbose) {cout << "\t\t\t\t" << X << endl;} }

// ============================================================================
//                         GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

typedef bdlde::QuotedPrintableDecoder Obj;

                        // ==================
                        // Named STATE Values
                        // ==================
enum State {
    // Enumeration of logical states described in the test plan overview.
    // These logical states must range in value from INITIAL_STATE = 0 to
    // ERROR_STATE = NUM_STATES - 1.  Note that the number and values of these
    // logical states need not coincide with those defined explicitly in the
    // implementation.

    INITIAL_STATE = 0,
    SAW_EQUAL,
    SAW_EQ_HEX,
    SAW_EQ_RET,
    SAW_EQ_WHITE,
    SAW_RETURN,
    SAW_WHITE,
    STATE_ZERO,         // Same as INITIAL_STATE except outputLength() > 0
    DONE_STATE,
    ERROR_STATE         // must be last of valid states
};

// The following is a list of strings to be used when printing state values.

const char *STATE_NAMES[] = {
    "INTITAL_STATE",
    "SAW_EQUAL",
    "SAW_EQ_HEX",
    "SAW_EQ_RET",
    "SAW_EQ_WHITE",
    "SAW_RETURN",
    "SAW_WHITE",
    "STATE_ZERO",
    "DONE_STATE",
    "ERROR_STATE"
};

const int NUM_STATES = sizeof STATE_NAMES / sizeof *STATE_NAMES;
BSLMF_ASSERT(ERROR_STATE + 1 == NUM_STATES);

                        // ====================================
                        // Equivalence-Class Related Data Types
                        // ====================================

struct Range {
    // Range is made inclusive to support a point.

    unsigned char start;
    unsigned char end;
};

struct EquivalenceClass {
    string        d_name;
    vector<Range> d_ranges;
    int           d_numChars;

    EquivalenceClass(const char  *inName,
                     const Range *inRanges,
                     int          inNumRanges)
    {
        d_name = inName;
        for (int i = 0; i < inNumRanges; ++i) {
            d_ranges.push_back(inRanges[i]);
        }

        d_numChars = 0;
        for (int i = 0; i < inNumRanges; ++i) {
            d_numChars += d_ranges[i].end - d_ranges[i].start + 1;
        }
    }

    string printRanges() const {
        string str("[");
        str += d_ranges[0].start;
        str += string(" - ");
        str += d_ranges[0].end;
        str += ']';
        for (int i = 1; i < (int)d_ranges.size(); ++i) {
            str += string(", [");
            str += d_ranges[i].start;
            str += string(" - ");
            str += d_ranges[i].end;
            str += ']';
        }
        return str;
    }
};

// Strict-mode equivalence classes
Range regularCharStrictRanges[]    = { {33, 47}, {58, 60}, {62, 64},
                                       {71, 126} };
Range hexadecimalStrictRanges[]    = { {48, 57}, {65, 70} };
Range equalSignStrictRanges[]      = { {61, 61} };
Range whitespaceStrictRanges[]     = { {9, 9}, {32, 32} };
Range newlineStrictRanges[]        = { {10, 10} };
Range carriageReturnStrictRanges[] = { {13, 13} };
Range unrecognizedStrictRanges[]   = { {0, 8}, {11, 12}, {14, 31},
                                       {127, 255} };

int numRegularCharStrictRanges     =
                               sizeof(regularCharStrictRanges) / sizeof(Range);
int numHexadecimalStrictRanges     = sizeof(hexadecimalStrictRanges) /
                                                                 sizeof(Range);
int numEqualSignStrictRanges       = 1;
int numWhitespaceStrictRanges      =
                                sizeof(whitespaceStrictRanges) / sizeof(Range);
int numNewlineStrictRanges         = 1;
int numCarriageReturnStrictRanges  = 1;
int numUnrecognizedStrictRanges    =
                              sizeof(unrecognizedStrictRanges) / sizeof(Range);

const EquivalenceClass regularCharStrict("Regular Char (Strict Mode)",
                                         regularCharStrictRanges,
                                         numRegularCharStrictRanges);
const EquivalenceClass hexadecimalStrict("Hexadecimal (Strict Mode)",
                                         hexadecimalStrictRanges,
                                         numHexadecimalStrictRanges);
const EquivalenceClass equalSignStrict("Equal sign (Strict Mode)",
                                       equalSignStrictRanges,
                                       numEqualSignStrictRanges);
const EquivalenceClass whitespaceStrict("Whitespace (Strict Mode)",
                                        whitespaceStrictRanges,
                                        numWhitespaceStrictRanges);
const EquivalenceClass newlineStrict("Newline (Strict Mode)",
                                     newlineStrictRanges,
                                     numNewlineStrictRanges);
const EquivalenceClass carriageReturnStrict("Carriage Return (Strict Mode)",
                                            carriageReturnStrictRanges,
                                            numCarriageReturnStrictRanges);
const EquivalenceClass unrecognizedCharStrict(
                                      "Unrecognized Chararacter (Strict Mode)",
                                      unrecognizedStrictRanges,
                                      numUnrecognizedStrictRanges);

const EquivalenceClass *const EquivalenceClassStrict_p[] = {
    &regularCharStrict,
    &hexadecimalStrict,
    &equalSignStrict,
    &whitespaceStrict,
    &newlineStrict,
    &carriageReturnStrict,
    &unrecognizedCharStrict
};

// Relaxed-mode equivalence classes
Range regularCharRelaxedRanges[]    = { {0, 8},   {11, 12},   {14, 31},
                                        {33, 47}, {58, 60},   {62, 64},
                                        {71, 96}, {103, 126}, {127, 255} };
Range hexadecimalRelaxedRanges[]    = { {48, 57}, {65, 70}, {97, 102} };
Range equalSignRelaxedRanges[]      = { {61, 61} };
Range whitespaceRelaxedRanges[]     = { {9, 9}, {32, 32} };
Range newlineRelaxedRanges[]        = { {10, 10} };
Range carriageReturnRelaxedRanges[] = { {13, 13} };

int numRegularCharRelaxedRanges     =
                              sizeof(regularCharRelaxedRanges) / sizeof(Range);
int numHexadecimalRelaxedRanges     = sizeof(hexadecimalRelaxedRanges)
                                                               / sizeof(Range);
int numEqualSignRelaxedRanges       = 1;
int numWhitespaceRelaxedRanges      =
                               sizeof(whitespaceRelaxedRanges) / sizeof(Range);
int numNewlineRelaxedRanges         = 1;
int numCarriageReturnRelaxedRanges  = 1;

const EquivalenceClass regularCharRelaxed("Regular Char (Relaxed Mode)",
                                          regularCharRelaxedRanges,
                                          numRegularCharRelaxedRanges);
const EquivalenceClass hexadecimalRelaxed("Hexadecimal (Relaxed Mode)",
                                          hexadecimalRelaxedRanges,
                                          numHexadecimalRelaxedRanges);
const EquivalenceClass equalSignRelaxed("Equal sign (Relaxed Mode)",
                                        equalSignRelaxedRanges,
                                        numEqualSignRelaxedRanges);
const EquivalenceClass whitespaceRelaxed("Whitespace (Relaxed Mode)",
                                         whitespaceRelaxedRanges,
                                         numWhitespaceRelaxedRanges);
const EquivalenceClass newlineRelaxed("Newline (Relaxed Mode)",
                                      newlineRelaxedRanges,
                                      numNewlineRelaxedRanges);
const EquivalenceClass carriageReturnRelaxed("Carriage Return (Relaxed Mode)",
                                             carriageReturnRelaxedRanges,
                                             numCarriageReturnRelaxedRanges);

const EquivalenceClass *const EquivalenceClassRelaxed_p[] = {
    &regularCharRelaxed,
    &hexadecimalRelaxed,
    &equalSignRelaxed,
    &whitespaceRelaxed,
    &newlineRelaxed,
    &carriageReturnRelaxed,
    // No &unrecognizedRelaxed equivalence class in relaxed mode.  The
    // characters that are in this class in the strict mode are all placed in
    // the regularChar equivalence class.
};

// enum EquivalenceClassId = {
//     PRINTABLE = 0,
//     WHITESPACE,
//     NEWLINE,
//     RETURN,
//     CONTROL
// };

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

#if 0  // TBD
                        // =============================
                        // Function findEquivalenceClass
                        // =============================

const EquivalenceClass& findEquivalenceClass(char ch)
    // Find the equivalence class to which the specified 'ch' belongs.
{
    if (ch == '=') {
        return equalSign;
    }
    else if (ch == '\t' || ch == ' ') {
        return whitespace;
    }
    else if (ch == '\n') {
        return newline;
    }
    else if (ch == '\r') {
        return carriageReturn;
    }
    else if (33 <= ch && ch <= 47 ||
             58 <= ch && ch <= 60 ||
             62 <= ch && ch <= 126)   {
        return regularChar;
    }
    else if (48 <= ch && ch <= 57) {
        return hexadecimal;
    }
    else {
        return controlChar;
    }
}
#endif

                        // ==============
                        // Function myMin
                        // ==============

template <class T>
inline
T myMin(const T& a, const T& b)
{
    return a < b ? a : b;
}

                        // ==============
                        // Function isHex
                        // ==============

inline
bool isHex(char ch)
    // Return 'true' is the specified 'ch' is a hexadecimal digit.  Note that
    // only uppercase letters are allowed since this function is only used to
    // check output from the converter being tested.
{
    return ('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F');
}


inline
int hexToChar(char hex, bool lowercase = false)
    // Return the hexadecimal number converted from the hexadecimal digit the
    // specified 'hex' character represents, accepting lowercase 'a' - 'f' if
    // the optionally specified 'lowercase' is 'true'; return -1 if hex is not
    // within the hexadecimal range (conditioned by 'lowercase').
{
    if ('0' <= hex && hex <= '9') {
        return hex - '0';                                             // RETURN
    }
    else if ('A' <= hex && hex <= 'F') {
        return hex - 'A';                                             // RETURN
    }
    else if (lowercase && 'a' <= hex && hex <= 'f') {
        return hex - 'a';                                             // RETURN
    }
    else {
        return -1;                                                    // RETURN
    }
}

inline
int hexStrToChar(char* character, char string[2], bool lowercase = false)
    // Place into the specified 'character' the ASCII value obtained by
    // interpreting the 2 characters in the specified 'string' as hexadecimal
    // digits, including lowercase 'a' - 'f' if the optionally specified
    // 'lowercase' is 'true'.  Return 0 if 'string' contains characters within
    // the hexadecimal range (conditioned by 'lowercase'), and -1 otherwise.
{
    int d1 = hexToChar(string[0], lowercase);
    if (d1 == -1)
        return -1;                                                    // RETURN

    int d2 = hexToChar(string[1], lowercase);
    if (d2 == -1)
        return -1;                                                    // RETURN

    *character = (d1 << 4) | d2;
    return 0;
}

                        // ================
                        // Function isWhite
                        // ================

inline
bool isWhite(char ch)
    // Return 'true' is the specified 'ch' is either a space or a tab.
{
    return ' ' == ch || '\t' == ch;
}

ostream& printCharN(ostream& output, const char* sequence, int length)
    // Print the specified character 'sequence' of specified 'length' to the
    // specified 'stream' and return a reference to the modifiable 'stream'
    // (if a character is not printable, its hexadecimal code is printed
    // instead).  The behavior is undefined unless 0 <= 'length' and sequence
    // refers to a valid area of memory of size at least 'length'.
{
    static char HEX[] = "0123456789ABCDEF";


    for (int i = 0; i < length; ++i) {
        unsigned char u = static_cast<unsigned char>(sequence[i]);

        if (isprint(u)) {
            output << u;
        }
        else {
            output << '<' << HEX[u/16] << HEX[u%16] << '>';
        }
    }
    return output << flush;
}

                        // =================
                        // Function setState
                        // =================

void setState(bdlde::QuotedPrintableDecoder *object, int state)
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

    char b[4];
    int numOut = -1;
    int numIn = -1;
    char input[2];
    char *const begin = input;
    char *end = input + 1;

    switch (state) {
      case INITIAL_STATE: {
        ASSERT(1 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(1 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case STATE_ZERO: {
        input[0] = 'A';
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 < object->outputLength());
      } break;
      case SAW_EQUAL: {
        input[0] = '=';
        ASSERT(1 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_EQ_HEX: {
        input[0] = '=';
        input[1] = '3';
        ++end;
        ASSERT(2 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(2 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_EQ_RET: {
        input[0] = '=';
        input[1] = '\r';
        ++end;
        ASSERT(2 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(2 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_EQ_WHITE: {
        input[0] = '=';
        input[1] = ' ';
        ++end;
        ASSERT(1 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(2 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_RETURN: {
        input[0] = '\r';
        ASSERT(1 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_WHITE: {
        input[0] = ' ';
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case DONE_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(1 == object->isAccepting());
        ASSERT(1 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength()); // In this case, we know!
      } break;
      case ERROR_STATE: {
        ASSERT(0 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(-1 == object->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(1 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
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
    EnabledGuard(bool flag)
        // Create a guard to control the activation of individual assertions in
        // the '::isState' test helper function using the specified enable
        // 'flag' value.  If 'flag' is 'true' individual false values we be
        // reported as assertion errors.
    : d_state(globalAssertsEnabled)
    {
        globalAssertsEnabled = flag;
    }

    ~EnabledGuard() { globalAssertsEnabled = d_state; }
};

bool isState(bdlde::QuotedPrintableDecoder *object, int state)
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
    int numOut = -1;

    bool rv = false;

    switch (state) {
      case INITIAL_STATE: {
        bool a0 = 1 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 1 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);
        bool b4 = 0 == object->outputLength();          ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case STATE_ZERO: {
        bool a0 = 1 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);
        bool b4 = 0 <  object->outputLength();          ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;
        bool c2 = 1 == numOut;
        bool c3 = c1 || c2;                             ASSERT(c3 || !enabled);

        bool d0, d1, d2;

        switch (numOut) {
          case 1: {
            d0 =  -1  != b[0];                          ASSERT(d0 || !enabled);
            d1 =  -1  == b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  == b[2];                          ASSERT(d2 || !enabled);
          } break;
          case 0: {
            d0 =  -1  == b[0];                          ASSERT(d0 || !enabled);
            d1 =  -1  == b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  == b[2];                          ASSERT(d2 || !enabled);
          } break;
        }

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
          && c0 && c3 && d0 && d1 && d2;

      } break;
      case SAW_EQUAL: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 1 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        d0 = '=' == b[0];                               ASSERT(d0 || !enabled);
        d1 =  -1 == b[1];                               ASSERT(d1 || !enabled);
        d2 =  -1 == b[2];                               ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case SAW_EQ_HEX: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 2 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        d0 = '='  == b[0];                              ASSERT(d0 || !enabled);
        d1 =  isHex( b[1]);                             ASSERT(d1 || !enabled);
        d2 =  -1  == b[2];                              ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case SAW_EQ_RET: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 2 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        if (numOut == 6) {
            d0 = '='  == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\r' == b[1];                          ASSERT(d1 || !enabled);
            d2 = -1   == b[2];                          ASSERT(d2 || !enabled);
        }

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case SAW_EQ_WHITE: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 1 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        d0 = '=' == b[0];                               ASSERT(d0 || !enabled);
        d1 = -1  == b[1];                               ASSERT(d1 || !enabled);
        d2 = -1  == b[2];                               ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case SAW_RETURN: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 1 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        d0 = '\r' == b[0];                              ASSERT(d0 || !enabled);
        d1 = -1   == b[1];                              ASSERT(d1 || !enabled);
        d2 = -1   == b[2];                              ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case SAW_WHITE: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 1 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 0 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0, d1, d2;

        d0 = -1 == b[0];                                ASSERT(d0 || !enabled);
        d1 = -1 == b[1];                                ASSERT(d1 || !enabled);
        d2 = -1 == b[2];                                ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case DONE_STATE: {
        bool a0 = 1 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 1 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 0 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // ERROR_STATE
        bool b0 = 0 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2;

      } break;
      case ERROR_STATE: {
        bool a0 = 0 == object->isAccepting();           ASSERT(a0 || !enabled);
        bool a1 = 0 == object->isDone();                ASSERT(a1 || !enabled);
        bool a2 = 1 == object->isError();               ASSERT(a2 || !enabled);
        bool a3 = 0 == object->isInitialState();        ASSERT(a3 || !enabled);

        int result = object->endConvert(b, &numOut);

        // ERROR_STATE
        bool b0 = 0 == object->isAccepting();           ASSERT(b0 || !enabled);
        bool b1 = 0 == object->isDone();                ASSERT(b1 || !enabled);
        bool b2 = 1 == object->isError();               ASSERT(b2 || !enabled);
        bool b3 = 0 == object->isInitialState();        ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
            && c0 && c1 && d0 && d1 && d2;

      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
    return rv;
}

const char* getStateInText(bdlde::QuotedPrintableDecoder *object)
{
    int oldGlobalAssertsEnabled = globalAssertsEnabled;
    globalAssertsEnabled = 0;

    for (int i = 0; i < NUM_STATES; ++i) {
        if (isState(object, (State) i)) {
            globalAssertsEnabled = oldGlobalAssertsEnabled;
            return STATE_NAMES[i];                                    // RETURN
        }
    }
    globalAssertsEnabled = oldGlobalAssertsEnabled;
    return "UNKNOWN_STATE";
}

// ============================================================================
//                            TEST HELPER CLASSES
// ----------------------------------------------------------------------------

                            // ===================
                            // class InputIterator
                            // ===================

class InputIterator {
    // This class provides an minimal iterator-like interface that can be used
    // to test encoding/decoding automata that cannot rely on input iterators
    // having random-access semantics.  The postfix increment operator is
    // deliberately omitted (but may be added locally where desired).

    const char *d_pointer_p;

    friend bool operator==(const InputIterator&, const InputIterator&);

  public:
    // CREATORS
    InputIterator(const char *source);
        // Create an iterator referring the specified 'source' character.

    InputIterator(const InputIterator& original);
        // Crate an iterator having the same value as that of the specified
        // 'original' iterator.

    // MANIPULATORS
    InputIterator& operator=(const InputIterator& rhs);
        // Assign the value of the specified 'rhs' iterator to this one.

    void operator++();
        // Advance this iterator to refer to the next position in the sequence;
        // the behavior is undefined if this iterator does not refer initially
        // to a valid element position.

    // ACCESSORS
    char operator*() const;
        // Return the character at the current position; the behavior is
        // undefined if this iterator does not refer initially to a valid
        // element position.
};

// FREE OPERATORS

inline
bool operator==(const InputIterator& lhs, const InputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to the
    // same element, and 'false' otherwise.

inline
bool operator!=(const InputIterator& lhs, const InputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do note refer
    // to same element, and 'false' otherwise.

InputIterator::InputIterator(const char *source)
: d_pointer_p(source)
{
}

InputIterator::InputIterator(const InputIterator& original)
: d_pointer_p(original.d_pointer_p)
{
}

InputIterator& InputIterator::operator=(const InputIterator& rhs)
{
    d_pointer_p = rhs.d_pointer_p; return *this;
}

void InputIterator::operator++()
{
    ++d_pointer_p;
}

char InputIterator::operator*() const
{
    return *d_pointer_p;
}

bool operator==(const InputIterator& lhs, const InputIterator& rhs)
{
    return lhs.d_pointer_p == rhs.d_pointer_p;
}

bool operator!=(const InputIterator& lhs, const InputIterator& rhs)
{
    return !(lhs == rhs);
}

                            // ====================
                            // class OutputIterator
                            // ====================

class OutputIterator {
    // This class provides an minimal iterator-like interface that can be used
    // to test encoding/decoding automata that cannot rely on output iterators
    // having random-access semantics.  The postfix increment operator is
    // deliberately omitted (but may be added locally where desired).

    char *d_pointer_p;

    friend bool operator==(const OutputIterator&, const OutputIterator&);

  public:
    // CREATORS
    OutputIterator(char *source);
        // Create an iterator referring the specified 'source' character.

    OutputIterator(const OutputIterator& original);
        // Crate an iterator having the same value as that of the specified
        // 'original' iterator.

    // MANIPULATORS
    OutputIterator& operator=(const OutputIterator& rhs);
        // Assign the value of the specified 'rhs' iterator to this one.

    void operator++();
        // Advance this iterator to refer to the next position in the sequence;
        // the behavior is undefined if this iterator does not refer initially
        // to a valid element position.

    // ACCESSORS
    char& operator*() const;
        // Return a reference to the modifiable character at the current i
        // position; the behavior is undefined if this iterator does not refer
        // initially to a valid element position.
};

// FREE OPERATORS

inline
bool operator==(const OutputIterator& lhs, const OutputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to the
    // same element, and 'false' otherwise.

inline
bool operator!=(const OutputIterator& lhs, const OutputIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do note refer
    // to same element, and 'false' otherwise.

OutputIterator::OutputIterator(char *source)
: d_pointer_p(source)
{
}

OutputIterator::OutputIterator(const OutputIterator& original)
: d_pointer_p(original.d_pointer_p)
{
}

OutputIterator& OutputIterator::operator=(const OutputIterator& rhs)
{
    d_pointer_p = rhs.d_pointer_p; return *this;
}

void OutputIterator::operator++()
{
    ++d_pointer_p;
}

char& OutputIterator::operator*() const
{
    return *d_pointer_p;
}

bool operator==(const OutputIterator& lhs, const OutputIterator& rhs)
{
    return lhs.d_pointer_p == rhs.d_pointer_p;
}

bool operator!=(const OutputIterator& lhs, const OutputIterator& rhs)
{
    return !(lhs == rhs);
}

// ============================================================================
//                         SUPPORT FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
      case 9: {
#if 0 // TBD
        // --------------------------------------------------------------------
        // RESET
        //   Verify the 'reset' method.
        //
        // Concerns:
        //   - That 'reset' method returns the object to its initial state
        //     (i.e., the same as it was immediately after construction).
        //   - That the initial configuration is not altered.
        //
        // Plan:
        //   - Use put the object in each state and verify the expected
        //      state is not/is in the initial state before/after the call to
        //      'reset'.
        //   - Verify that the initial configuration has not changed.
        //   - Repeat the above with a different configuration.
        //   - Verify that if there are characters specified to be encoded,
        //     they have not changed after 'reset' until the 3-parameter
        //     constructor is verified (in case 7).
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Loop-Based Implementation Technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESET" << endl
                          << "===========" << endl;

        if (verbose) cout << "\nVerify 'reset'." << endl;

        V("In CRLF_MODE and error-reporting mode");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(true, Obj::e_CRLF_MODE);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::e_CRLF_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, obj.isUnrecognizedAnError());
            }
        }

        V("In LF_MODE and error-reporting mode");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(true, Obj::e_LF_MODE);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::e_LF_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, obj.isUnrecognizedAnError());
            }
        }

        V("In CRLF_MODE and non-error-reporting mode");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(false, Obj::e_CRLF_MODE);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::e_CRLF_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, !obj.isUnrecognizedAnError());
            }
        }

        V("In LF_MODE and non-error-reporting mode");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(false, Obj::e_LF_MODE);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::e_LF_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, !obj.isUnrecognizedAnError());
            }
        }
#endif
      } break;
      case 8: {
        // TBD ???
      } break;
      case 7: {
        // TBD ???
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Verify THE CONSTRUCTOR IN ALL CONFIGURATIONS AND INTERNAL TABLES
        //   Ensure that each internal table has the appropriate entries and
        //   that the constructor in various configurations can be used to
        //   specify whether to halt decoding and report error and also to
        //   specify how a line break is to be decoded.
        //
        // Concerns:
        //   - That there is a typo in some internal table.
        //   - That the constructor does not change the internal opcode table
        //     properly
        //
        // Plan:
        //   Using a pseudo-piecewise-continuous implementation technique
        //   i.e., Loop-Based), provide an area test that will sample each of
        //   the table entries.  The goal is that if any table entry is bad,
        //   the test will fail.
        //
        // Tactics:
        //   - Examine input by the Equivalence Classes
        //   - Area Data Selection Method
        //   - Loop-Based Implementation Technique
        //
        // Testing:
        //   That each internal table has no defective entries.
        //   That the constructor changes the opcode table properly.
        // --------------------------------------------------------------------
#if 0  // TBD
        if (verbose) cout << endl
                          << "VERIFY INTERNAL TABLES" << endl
                          << "======================" << endl;

        V("\nVerify Decoding Table and Opcode Table (256 entires).\n");

        char input[10];
        char* const I = input;
        char *B, *E;
        int nOut;
        int nIn;

        char output[10];
        char *D;
        const bool S = true;   // strict mode
        const bool R = false;  // relaxed mode
        bool errorMode;

        V("Verify Strict-Mode Mapping.");
        errorMode = S;

        V("Verify Regular Characters: Entries "
          << regularCharStrictRanges.printRanges() << ".");
        {
            const EquivalenceClass &inputType = regularChar;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    VVV("Verify regular characters are printed and not");
                    VVV("decoded");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }
                }
            }
        }

        V("Verify Hexadecimals: Entries "
          << hexadecimalStrictRanges.printRanges() << ".");
        {
            const EquivalenceClass &inputType = hexadecimal;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input[0] = i;

                    VVV("Verify stand-alone hexadecimals are printed and");
                    VVV("decoded.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }

                    VVV("Verify delayed decoding of a hexadecimal following");
                    VVV("an '='.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '=';
                        I[1] = i;
                        I[2] = '0';
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        char out;
                        LOOP_ASSERT(i, 0 ==
                                    hexStrToChar(&out, I + 1, !errorMode));

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, out == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, out == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);
                    }

                    VVV("Verify error reporting of a hexadecimal following");
                    VVV("an '=Y' sequence where Y is not a hexadecimal.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '=';
                        I[1] = '\r';
                        I[2] = i;
                        B = I;
                        E = B + 2;
                        D = output;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 2 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, -1==obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 0 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, I[1] == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }
                }
            }
        }

        V("Verify Equal Sign: Entry [61].");
        {
            const EquivalenceClass &inputType = hexadecimal;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input[0] = i;

                    VVV("Verify delayed decoding of a '=' followed by");
                    VVV("2 hexadecimal digits.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = '6';
                        I[2] = '5';
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        char out;
                        LOOP_ASSERT(i, 0 ==
                                    hexStrToChar(&out, I + 1, !errorMode));

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, out == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, out == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);
                    }

                    VVV("Verify decoding of a soft line break \"=\\r\\n\"");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = '\r';
                        I[2] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i,  -1  == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i,  -1  == b[2]);
                    }

                    VVV("Verify error reporting of a '=' followed by an");
                    VVV("illegal character.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = 'A';
                        B = I;
                        E = B + 2;
                        D = output;
                        LOOP_ASSERT(i, -1==obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 2 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1  == b[0]);
                        LOOP_ASSERT(i, -1  == b[1]);
                        LOOP_ASSERT(i, -1  == b[2]);
                    }
                }
            }
        }

        V("Verify Whitespace Characters: Entries "
          << whitespaceStrictRanges.printRanges() << ".");
        {
            const EquivalenceClass &inputType = whitespace;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    VVV("Verify whitespace characters not at the end of");
                    VVV("input are printed and not decoded.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = 'A';
                        B = I;
                        E = B + 1;
                        D = output;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1   == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, I[1] == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, I[1] == b[1]);
                        LOOP_ASSERT(i, -1   == b[2])
                    }

                    VVV("Verify whitespace characters at the end of input");
                    VVV("are ignored.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = i;
                        I[2] = i;
                        B = I;
                        E = B + 3;
                        D = output;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1   == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                    }

                    VVV("Verify whitespace characters at the end of line");
                    VVV("just before a '\r' are ignored.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = i;
                        I[2] = i;
                        I[3] = '\r';
                        B = I;
                        E = B + 4;
                        D = output;
                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 4 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D, &nOut));
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i,  -1  == b[1]);
                        LOOP_ASSERT(i,  -1  == b[2]);
                    }

                    // Whitespace before '\n' is encoded to "=20" only to
                    // prevent the true whitespace in the data from being mixed
                    // up with the transport padding added before a line break
                    // during transmission.  During decoding, however, it
                    // should be assumed that that encoder was used to generate
                    // input to this decoder.  The whitespace is therefore
                    // preserved and output (while the '\n' character is
                    // dropped from output).

                    VVV("Verify whitespace characters at the end of line");
                    VVV("(just before a '\n' are not ignored.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = i;
                        I[2] = i;
                        I[3] = '\n';
                        B = I;
                        E = B + 3;
                        D = output;
                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 4 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                    }

                    VVV("Verify whitespace characters between a '=' and");
                    VVV("a '\r' within a soft line break are ignored.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '=';
                        I[1] = i;
                        I[2] = i;
                        I[3] = i;
                        I[4] = '\r';
                        I[5] = '\n';
                        B = I;
                        E = B + 5;
                        D = output;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 5 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                    }
                }
            }
        }

        V("Verify Carriage Return: Entry [13]");
        {
            const EquivalenceClass &inputType = carriageReturn;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    VVV("Verify delayed decoding of \"\\r\\n\" to \"\\r\\n\"");
                    VVV("in CRLF_MODE.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        I[1] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }

                    VVV("Verify delayed decoding of \"\\r\\n\" to \"\\n\"");
                    VVV("in LF_MODE.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_LF_MODE);
                        I[0] = i;
                        I[1] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\n' == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\n' == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }

                    VVV("Verify error reporting of a stand-alone '\r'");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_LF_MODE);
                        I[0] = i;
                        I[1] = 'A';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, -1==obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 0 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i,  -1  == b[1]);
                        LOOP_ASSERT(i,  -1  == b[2]);
                    }

                    VVV("Verify delayed dropping of \"=\\r\\n\"");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '=';
                        I[1] = '\r';
                        I[1] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 1 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                    }

                    VVV("Verify error reporting of \"=\\r\"");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '=';
                        I[1] = '\r';
                        I[1] = 'A';
                        B = I;
                        E = B + 2;
                        D = output;

                        LOOP_ASSERT(i, 2 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 2 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);

                        B = E;
                        E = B + 1;
                        D += nOut;
                        LOOP_ASSERT(i, -1==obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, I[0] == b[0]);
                        LOOP_ASSERT(i, I[1] == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }
                }
            }
        }

        V("Verify Newline: Entry [10]");
        {
            const EquivalenceClass &inputType = newline;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input[0] = i;

                    VVV("Verify a stand-alone '\\n' is decoded in CRLF_MODE.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }

                    VVV("Verify a stand-alone '\\n' is output as '\n'");
                    VVV("in LF_MODE.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = '\n';
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, 0 == obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\n' == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\n' == b[0]);
                        LOOP_ASSERT(i, -1   == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }
                }
            }
        }

        V("Verify Unrecognized Characters: Entries "
          << unrecognizedStrictRanges.printRanges() << ".");
        {
            const EquivalenceClass &inputType = unrecognized;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    VVV("Verify error reporting and skipping of unrecognized");
                    VVV("characters in strict mode.");
                    {
                        memset(output, -1, sizeof(output));
                        Obj obj(errorMode, Obj::e_CRLF_MODE);
                        I[0] = i;
                        B = I;
                        E = B + 1;
                        D = output;

                        LOOP_ASSERT(i, -1==obj.convert(D, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);

                        LOOP_ASSERT(i, 0 == obj.endConvert(D + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                    }


                    VVV("Verify unrecognized characters are ignored in");
                    VVV("relaxed mode.");



                }
            }
        }
#endif

      } break;
      case 5: {
      } break;
      case 4: {
#if 0 // TBD
        // --------------------------------------------------------------------
        // BOOTSTRAP: 'endConvert' - transitions
        //   Verify 'endConvert' transitions for all states.
        //
        // Concerns:
        //   - That we reach the correct final state after calling
        //     'endConvert'.
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
                          << "=====================================" << endl;

        V("Verify 'endConvert' transitions for decoder.");
        {
            static const struct {
                int d_lineNum;          // source line number
                int d_startState;       // indicated starting state
                int d_endState;         // expected ending state

            } DATA[] = {//-input----v  v--output--v
                //lin  Starting State  Ending State
                //---  --------------  ------------
                { L_,  INITIAL_STATE,  DONE_STATE  },
                { L_,  STATE_ZERO,     DONE_STATE  },
                { L_,  SAW_EQUAL,      DONE_STATE  },
                { L_,  SAW_EQ_HEX,     DONE_STATE  },
                { L_,  SAW_EQ_RET,     DONE_STATE  },
                { L_,  SAW_EQ_WHITE,   DONE_STATE  },
                { L_,  SAW_RETURN,     DONE_STATE  },
                { L_,  SAW_WHITE,      DONE_STATE  },
                { L_,  DONE_STATE,     ERROR_STATE },
                { L_,  ERROR_STATE,    ERROR_STATE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char b[3];
            int numOut;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int START = DATA[ti].d_startState;
                const int END   = DATA[ti].d_endState;
                const int RTN = -(ERROR_STATE == END);

                // Use non-error-reporting decoder as the 'endConvert' method
                // is used within 'isState' and it will report an error if the
                // decoder is already done or in error.

                Obj obj(false, Obj::e_CRLF_MODE);

                if (verbose) cout << '\t' << STATE_NAMES[START] << '.' << endl;
                if (veryVerbose) cout <<
                       "\t\tExpected next state: " << STATE_NAMES[END] << endl;
                setState(&obj, START);

                if (veryVerbose) cout <<
                                 "\t\tExpected return status: " << RTN << endl;

                int rtn = obj.endConvert(b, &numOut);

                if (veryVerbose) cout << "\t\tActual next state: "
                                      << getStateInText(&obj) << endl;

                if (veryVerbose) cout <<
                                 "\t\tActual return status: " << rtn << endl;

                LOOP_ASSERT(LINE, RTN == rtn);
                LOOP_ASSERT(LINE, isState(&obj, END));

            } // end for ti
        } // end block
#endif
      } break;
      case 3: {
#if 0 // TBD
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
        //   test helper function.  Then use all of the (as yet untested)
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
        //   bool isAccepting() const;
        //   bool isDone() const;
        //   bool isError() const;
        //   bool isInitialState() const;
        //   int outputLength() const;
        //
        //   ~bdlde::QuotedPrintableDecoder();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SET-STATE, IS-STATE, RESET, AND BASIC ACCESSORS"
                          << endl
                          << "==============================================="
                          << endl;

        if (verbose) cout <<
                         "\nMake sure we can detect the initial state."
                          << endl;
        {
            if (verbose) cout << "\tINITIAL_STATE." << endl;

            Obj obj(true, Obj::e_CRLF_MODE);

            ASSERT(obj.isUnrecognizedAnError());
            ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\nVerify ::setState." << endl;
        {
#if 0
            if (verbose) cout << "\tINITIAL_STATE." << endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, INITIAL_STATE);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT(-1 == b[0]);
                ASSERT(-1 == b[1]);
                ASSERT(-1 == b[2]);
            }

            if (verbose) cout << "\tSTATE_ZERO." << endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, STATE_ZERO);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 <  obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 <  obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT(-1 == b[0]);
                ASSERT(-1 == b[1]);
                ASSERT(-1 == b[2]);
            }

            if (verbose) cout << "\tSAW_EQUAL." < endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_EQUAL);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                // Need to convert more input to distinguish SAW_EQUAL from
                // SAW_EQ_WHITE.

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int numIn  = -1;
                const char input[] = "41";  // 'A' in hex
                const char *const begin = input;
                const char *const end   = input + 2;
                int result = obj.convert(b, &numOut, &numIn, begin, end);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT('A' == b[0]);
                ASSERT(-1  == b[1]);
                ASSERT(-1  == b[2]);
            }

            if (verbose) cout << "\tSAW_EQ_HEX." < endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_EQ_HEX);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(2 == numOut);
                ASSERT('=' == b[0]);
                ASSERT( isHex(b[1]));
                ASSERT( -1 == b[2]);
            }

            if (verbose) cout << "\tSAW_EQ_RET." < endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_EQ_RET);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(2 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(2 == numOut);
                ASSERT('='  == b[0]);
                ASSERT('\r' == b[1]);
                ASSERT( -1  == b[2]);
            }
#endif
            if (verbose) cout << "\tSAW_EQ_WHITE." < endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_EQ_WHITE);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                // Unlike the SAW_EQUAL state, where a further input sequence
                // of "41" produces an output character 'A', a decoder in the
                // SAW_EQ_WHITE state would interpret the whitespace gap
                // between '=' and "41" as an error and output it in the
                // relaxed mode.

                // Use 'convert' with zero output limit to obtain the buffer
                // size needed to hold output.

                char *b;
                const char *begin, *end;
                int numOut = -1;
                int numIn  = -1;

                int len = obj.convert(b, &numOut, &numIn, begin, end, 0);
                len += 2;               // include 2 further input chars
                b = new char[len + 1];  // Add a sentinel
                memset(b, 0, len + 1);

                const char input[] = "41";  // 'A' in hex
                begin = input;
                end   = input + 2;
                int result = obj.convert(b, &numOut, &numIn, begin, end);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(len == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(len == numOut);
                VV("len = " << len);
                VV("output = \"" << b << "\"");
                ASSERT('=' == b[0]);
                for (int i = 1; i < len - 2; ++i) {
                    ASSERT( isWhite(b[i]));
                }
                ASSERT('4' == b[len - 2]);
                ASSERT('1' == b[len - 1]);
                ASSERT( 0 == b[len]);

                delete[] b;
            }
#if 0

            if (verbose) cout << "\tSAW_RETURN." < endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_RETURN);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(1 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(1 == numOut);
                ASSERT('\r' == b[0]);
                ASSERT( -1  == b[1]);
                ASSERT( -1  == b[2]);
            }

            if (verbose) cout << "\tSAW_WHITE." << endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, SAW_WHITE);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(0 == numOut);
                ASSERT(-1 == b[0]);
                ASSERT(-1 == b[1]);
                ASSERT(-1 == b[2]);
            }

            if (verbose) cout << "\tDONE_STATE." << endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, DONE_STATE);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // ERROR_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-1 == result);
                ASSERT(0 == numOut);
                ASSERT(-1 == b[0]);
                ASSERT(-1 == b[1]);
                ASSERT(-1 == b[2]);
            }

            if (verbose) cout << "\tERROR_STATE." << endl;
            {
                Obj obj(false, Obj::e_CRLF_MODE);
                setState(&obj, ERROR_STATE);

                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[3] = { -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // ERROR_STATE
                ASSERT(!obj.isUnrecognizedAnError());
                ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());
                ASSERT(-1 == result);
                ASSERT(0 == numOut);
                ASSERT(-1 == b[0]);
                ASSERT(-1 == b[1]);
                ASSERT(-1 == b[2]);
            }
#endif
        }

        if (verbose) cout << "\nVerify ::isState." << endl;
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                if (verbose) cout << "\t\tState: "
                                  << STATE_NAMES[i] << '.' << endl;

                for (int j = 0; j < NUM_STATES; ++j) {
                    if (veryVerbose) cout << "\t\tisState: "
                                          << STATE_NAMES[j] << '.' << endl;

                    const bool SAME = i == j;
                    if (veryVeryVerbose) { T_ T_ T_ P(SAME) }

                    EnabledGuard Guard(SAME); // Enable individual '::isState'
                                              // ASSERTs in order to facilitate
                                              // debugging.

                    Obj obj(false, Obj::e_CRLF_MODE);
                    setState(&obj, i);
                    LOOP2_ASSERT(i, j, SAME == isState(&obj, j));
                }
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR AND CONFIGURATION STATE ACCESSORS
        //   Make sure we can bring the object to any attainable state.
        //
        // Concerns:
        //   That we can fully configure the object from the constructor.
        //   That the constructor defaults to the right configuration.
        //
        // Plan:
        //   Create the object in both the LineBreakMode's and in either the
        //   strict or the relaxed error reporting configuration, and verify
        //   using all of the (as yet untested) direct accessors.  After this
        //   test case, we can declare the accessors returning configuration
        //   state to be thoroughly tested.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Orthogonality
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlde::QuotedPrintableDecoder(
        //                            bool          detectError,
        //                            LineBreakMode lineBreakMode = CRLF_MODE);
        //   int maxLineLength() const;
        //   LineBreakMode lineBreakMode() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout << "\tDefault line break mode and error reporting"
                          << endl;
        {
            Obj obj(true);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(obj.isUnrecognizedAnError());
            ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tDefault line break mode and no error reporting"
                          << endl;
        {
            Obj obj(false);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(!obj.isUnrecognizedAnError());
            ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tCRLF_MODE and error reporting" << endl;
        {
            Obj obj(true, Obj::e_CRLF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(obj.isUnrecognizedAnError());
            ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tCRLF_MODE and no error reporting" << endl;
        {
            Obj obj(false, Obj::e_CRLF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(!obj.isUnrecognizedAnError());
            ASSERT(Obj::e_CRLF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tLF_MODE and error reporting" << endl;
        {
            Obj obj(true, Obj::e_LF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(obj.isUnrecognizedAnError());
            ASSERT(Obj::e_LF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tLF_MODE and no error reporting" << endl;
        {
            Obj obj(false, Obj::e_LF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(!obj.isUnrecognizedAnError());
            ASSERT(Obj::e_LF_MODE == obj.lineBreakMode());
            ASSERT(0 == obj.outputLength());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is available to be used as a developers sandbox.
        //
        // Concerns:
        //    None.
        //
        // Plan:
        //    Ad hoc.
        //
        // Tactics:
        //    - Ad Hoc Data Selection Method
        //    - Brute Force Implementation Technique
        //
        // Testing:
        //     BREATHING TEST -- (developer's sandbox)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        {
            const char *hex = "0123456789ABCDEF";
            char buffer[4];
            char input[4];
            input[0] = '=';
            input[3] = '\0';
            for (int i = 0; i < 256; ++i) {
                input[1] = hex[i / 16];
                input[2] = hex[i % 16];

                Obj obj(false);
                int numOut = -1;
                int numIn = -1;

                buffer[0] = 'z';
                buffer[1] = 'z';
                buffer[2] = 'z';
                buffer[3] = 'z';

                obj.convert(buffer, &numOut, &numIn, input, input + 3);
                LOOP_ASSERT(i, (char)i == buffer[0]);
                LOOP_ASSERT(i, 'z' == buffer[1]);
                LOOP_ASSERT(i, 'z' == buffer[2]);
                LOOP_ASSERT(i, 'z' == buffer[3]);
                LOOP_ASSERT(i, 3 == numIn);
                LOOP_ASSERT(i, 1 == numOut);
            }
        }

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_length;   // input length
                const char *d_exp;      // expected string
                int         d_expNum;   // expected length of output
            } DATA[] = {
                //lin  input           len  output        num
                //---  --------------  ---  ------------  ---
                { L_,  "",               0, "",             0 },
                { L_,  "a",              1, "a",            1 },
                { L_,  "ab",             2, "ab",           2 },
                { L_,  " a",             2, " a",           2 },
                { L_,  "\r\n",           2, "\r\n",         2 },
                { L_,  "abc",            3, "abc",          3 },
                { L_,  "=41",            3, "A",            1 },
                { L_,  " a ",            3, " a",           2 },
                { L_,  " a a",           4, " a a",         4 },
                { L_,  "\r\n\r\n",       4, "\r\n\r\n",     4 },
                { L_,  " =\r\n",         4, " ",            1 },
                { L_,  " a \r\n",        5, " a\r\n",       4 },
                { L_,  "\r\n=\r\na",     6, "\r\na",        3 },
                { L_,  " a =41",         6, " a A",         4 },
                { L_,  " a =\r\n",       6, " a ",          3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char buffer[1024];
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *INPUT = DATA[ti].d_input;
                const int   LEN   = DATA[ti].d_length;
                const char *EXP   = DATA[ti].d_exp;
                const int   NUM   = DATA[ti].d_expNum;

                LOOP_ASSERT(LINE, LEN <= (int)strlen(INPUT));
                LOOP_ASSERT(LINE, NUM <= (int)strlen(EXP));

                Obj obj(false);
                int numOut = -1;
                int numIn = -1;

                obj.convert(buffer, &numOut, &numIn, INPUT, INPUT + LEN);
                if (veryVerbose) {
                    P(LINE);
                    cout << "    ";
                    P_(INPUT);
                    P_(LEN);
                    P_(EXP);
                    P(NUM);
                    cout << "    ";
                    P_(numIn);
                    P_(numOut);
                    cout << "buffer = ";
                    for (int i = 0; i < numOut; ++i) cout << buffer[i];
                    cout << endl;
                }
                LOOP_ASSERT(LINE, 0 == memcmp(buffer, EXP, NUM));
                LOOP_ASSERT(LINE, LEN == numIn);
                LOOP_ASSERT(LINE, NUM == numOut);

                { // for debugging
                    int i;
                    for (i = 0; i < NUM && buffer[i] == EXP[i]; ++i);
                    if (i < NUM) {
                        cout << "###" << endl;
                        for (int j = i; j < i + 5 && j < NUM; ++j) {
                            cout << buffer[j] << ' ' << EXP[j] << "   " << j
                                 << endl;
                        }
                        cout << "###" << endl;
                        cout << endl;
                    }
                }
            } // end for ti
        } // end block

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_length;   // input length
                const char *d_exp;      // expected string
                int         d_expNum;   // expected length of output
            } DATA[] = {
                //lin  input           len  output        num
                //---  --------------  ---  ------------  ---
                { L_,  "",               0, "",             0 },
                { L_,  "a",              1, "a",            1 },
                { L_,  "(",              1, "(",            1 },
                { L_,  "ab",             2, "ab",           2 },
                { L_,  " a",             2, " a",           2 },
                { L_,  " (",             2, " (",           2 },
                { L_,  "\r\n",           2, "\n",           1 },
                { L_,  "abc",            3, "abc",          3 },
                { L_,  "=41",            3, "A",            1 },
                { L_,  "=28",            3, "(",            1 },
                { L_,  " a ",            3, " a",           2 },
                { L_,  " =28",           4, " (",           2 },
                { L_,  " a a",           4, " a a",         4 },
                { L_,  "\r\n=\r\na",     6, "\na",          2 },
                { L_,  "\r\n=\r\n(",     6, "\n(",          2 },
                { L_,  " a =41",         6, " a A",         4 },
                { L_,  " a =\r\n",       6, " a ",          3 },
                { L_,  "       a",       8, "       a",     8 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            char buffer[1024];
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *INPUT = DATA[ti].d_input;
                const int   LEN   = DATA[ti].d_length;
                const char *EXP   = DATA[ti].d_exp;
                const int   NUM   = DATA[ti].d_expNum;

                Obj obj(false, bdlde::QuotedPrintableDecoder::e_LF_MODE);
                int numOut = -1;
                int numIn = -1;

                obj.convert(buffer, &numOut, &numIn, INPUT, INPUT + LEN);
                if (veryVerbose) {
                    P(LINE);
                    cout << "    ";
                    P_(INPUT);
                    P_(LEN);
                    P_(EXP);
                    P(NUM);
                    cout << "    ";
                    P_(numIn);
                    P_(numOut);
                    cout << "buffer = ";
                    for (int i = 0; i < numOut; ++i) cout << buffer[i];
                    cout << endl;
                }
                LOOP_ASSERT(LINE, 0 == memcmp(buffer, EXP, NUM));
                LOOP_ASSERT(LINE, LEN == numIn);
                LOOP_ASSERT(LINE, NUM == numOut);
            } // end for ti
        } // end block


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
