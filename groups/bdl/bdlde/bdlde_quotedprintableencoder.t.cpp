// bdlde_quotedprintableencoder.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_quotedprintableencoder.h>

#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // memset()
#include <bsl_cctype.h>    // isprint(), toupper(), etc.
#include <bsl_iostream.h>
#include <bsl_limits.h>    // INT_MAX
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


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
//  bdlde_quotedprintableencoder(int maxLineLength);
//..
// The primary manipulators are those needed to reach every attainable state.
// Clearly 'convert' is one, but 'endConvert' is also needed in order to reach
// the 'DONE' state.  Hence, the primary manipulators are
//..
//  int convert(char *out, int *numOut, int *ni, const char *b, const char *e);
//  int endConvert(char *out, int *numOut);
//..
// Though not strictly primary, the 'reset' method resets the object to its
// initial state (i.e., to that immediately following construction).  It will
// turn out to be convenient to test 'reset' along with all the other state
// transitions imposed by 'convert' and 'endConvert' early in the testing
// process.
//..
// The basic accessers for the encoder are all the functions that return
// information about the customization and/or execution state:
//..
//  bool isAccepting() const;
//  bool isError() const;
//  bool isInitialState() const;
//  int maxLineLength() const;
//  char* lineBreakMode const;
//  int outputLength() const;
//..
// The following table illustrates major state transitions for all three of the
// processing manipulators:
//..
//                      convert (1)     endConvert      reset
//                      -----------     ----------      ----------
//      INITIAL_STATE:  State 1         DONE_STATE      INITIAL_STATE
//      State 1:        State 2         DONE_STATE      INITIAL_STATE
//      State 2:        State 3         DONE_STATE      INITIAL_STATE
//      State 3:        State 1         DONE_STATE      INITIAL_STATE
//      DONE_STATE:     ERROR_STATE     ERROR_STATE     INITIAL_STATE
//      ERROR_STATE:    ERROR_STATE     ERROR_STATE     INITIAL_STATE
//..
// Our first step will be to ensure that each of these states can be reached
// ('::setState'), that an anticipated state can be verified ('::isState'),
// and that each of the above state transitions (including 'reset') is
// verified.  Next, we will ensure that each internal table is correct.
// Finally, using category partitioning,  we enumerate a representative
// collection of inputs ordered by length (plus MaxLineLength) that will be
// sufficient to prove that the logic associated with the state machine is
// performing as desired.
//
// Note that Because the 'convert' and 'endConvert' methods are parametrized
// based on iterator types, we will want to ensure (at compile time) that their
// respective implementations do not depend on more than minimal iterator
// functionality.  We will accomplish this goal by supplying, as template
// arguments, 'bdeut::InputIterator' for 'convert' and 'bdeut::OutputIterator'
// for both of these template methods.
//
// ----------------------------------------------------------------------------
// [ 7] static int encodedLength(int numInputBytes, int maxLineLength); [ 8]
// bdlde::QuotedPrintableEncoder(); [ 2] bdlde::QuotedPrintableEncoder(int
// maxLineLength); [ 3] ~bdlde::QuotedPrintableEncoder(); [ 7] int convert(char
// *o, int *no, int*ni, const char*b, const char*e); [ 7] int endConvert(char
// *out, int *numOut); [ 3] void reset(); [ 3] bool isAccepting() const; [ 3]
// bool isError() const; [ 3] bool isInitialState() const; [ 2] int
// maxLineLength() const; [ 3] int outputLength() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox) [ ] USAGE EXAMPLE [ ?] That the
// input iterator can have *minimal* functionality.  [ ?] That the output
// iterator can have *minimal* functionality.  [ 5] BOOTSTRAP: 'convert' -
// transitions [ 4] BOOTSTRAP: 'endConvert'- transitions [ 3] That we can reach
// each of the major processing states.  [ 1] ::myMin(const T& a, const T& b);
// [ 1] ::printCharN(ostream& output, const char* sequence, int length) [ 3]
// void ::setState(Obj *obj, int state, const char *input); [ 3] bool
// ::isState(Obj *obj, int state); [ 6] That each internal table has no
// defective entries.  [ 7] That each bit of a 3-byte quantum finds its
// appropriate spot.  [ 7] That each bit of a 2-byte quantum finds its
// appropriate spot.  [ 7] That each bit of a 1-byte quantum finds its
// appropriate spot.  [ 7] That output length is calculated properly.
//
// ----------------------------------------------------------------------------

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

typedef bdlde::QuotedPrintableEncoder Obj;

                        // ==================
                        // Named STATE Values
                        // ==================
enum State {
    // Enumeration of logical states described in the test plan overview.
    // These logical states must range in value from INITIAL_STATE = 0 to
    // ERROR_STATE = NUM_STATES - 1.  Note that the number and values of
    // these logical states need not coincide with those defined explicitly
    // in the implementation.

    INITIAL_STATE = 0,
    SAW_RETURN,
    SAW_WHITE,
    STATE_ZERO,         // Same as INITIAL_STATE except outputLength() > 0
    DONE_STATE,
    ERROR_STATE         // must be last of valid states
};

// The following is a list of strings to be used when printing state values.

const char *STATE_NAMES[] = {
    "INTITAL_STATE",
    "SAW_RETURN",
    "SAW_WHITE",
    "STATE_ZERO",
    "DONE_STATE",
    "ERROR_STATE"
};

const int NUM_STATES = sizeof STATE_NAMES / sizeof *STATE_NAMES;

char assertion[ERROR_STATE + 1 == NUM_STATES];

                            // ===================
                            // class StateAccessor
                            // ===================

class StateAccessor {
  private:
    bdlde::QuotedPrintableEncoder *d_object_p;

  public:
    explicit
    StateAccessor(bdlde::QuotedPrintableEncoder *object)
        // Construct an instance of 'StateAccessor' which sets and examine the
        // internal states of the specified 'object'.  Note that the 'object'
        // must be newly constructed and remain in its initial state if the
        // initial state needs to be one of the accessible states.
    : d_object_p(object)
    { }

    void makeStateTransitionOnInput(char input);
        // Make a state transition by passing the specified 'input' character
        // to the encapsulated encoder.

    void makeStateTransitionOnEndInput();
        // Make a state transition by passing the end of input signal to the
        // encapsulated encoder.

    void setState(State state);
        // Move the specified 'object' from its initial (i.e., newly
        // constructed) state to the specified 'state' using '\0' characters
        // for input as needed.  The behavior is undefined if 'object' is not
        // in its newly-constructed initial state.  Note that when this
        // function is invoked on a newly constructed object, it is presumed
        // that 'isInitialState' has been sufficiently tested to ensure that it
        // returns 'true'.

    bool isState(State state) const;
        // Return 'true' if the specified 'object' was initially in the
        // specified 'state', and 'false' otherwise.  Setting the global
        // variable 'globalAssertsEnabled' to 'true' enables individual
        // sub-conditions to be ASSERTed, which can be used to facilitate test
        // driver debugging.  Note that the final state of 'object' may (and
        // probably will) be modified arbitrarily from its initial state in
        // order to distinguish similar states.

    static int numState()
    {
        return NUM_STATES;
    }
};

                        // ===========================================
                        // Public Manipulators for class StateAccessor
                        // ===========================================

void StateAccessor::makeStateTransitionOnInput(char input)
{
    ASSERT(d_object_p);

    char  buf[10];
    const char *begin = &input;
    const char *end   = begin + 1;
    int numOut;
    int numIn;

    d_object_p->convert(buf, &numOut, &numIn, begin, end);
}

void StateAccessor::makeStateTransitionOnEndInput()
{
    ASSERT(d_object_p);

    char  buf[10];
    int numOut;

    d_object_p->endConvert(buf, &numOut);
}

void StateAccessor::setState(State state)
{
    ASSERT(d_object_p);
    ASSERT(INITIAL_STATE <= (int)state && state <= (int)ERROR_STATE);
    ASSERT(state < NUM_STATES);

    if (!d_object_p->isInitialState()) { cout
     << "You must not call 'setState' from other than 'INITIAL_STATE'!" << endl
     << "\tNote that '::isState' *will* alter from the initial state." << endl;
    }

    // If 'd_object_p' is "just created" then this assertion should be true!
    ASSERT(d_object_p->isInitialState());

    char b[4];
    int numOut = -1;
    int numIn = -1;
    char input;
    char *const begin = &input;
    char *const end = &input + 1;

    switch (state) {
      case INITIAL_STATE: {
        ASSERT(1 == d_object_p->isAccepting());
        ASSERT(0 == d_object_p->isDone());
        ASSERT(0 == d_object_p->isError());
        ASSERT(1 == d_object_p->isInitialState());
        ASSERT(0 == d_object_p->outputLength());
      } break;
      case STATE_ZERO: {
        input = 'A';
        ASSERT(0 == d_object_p->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == d_object_p->isAccepting());
        ASSERT(0 == d_object_p->isDone());
        ASSERT(0 == d_object_p->isError());
        ASSERT(0 == d_object_p->isInitialState());
        ASSERT(0 < d_object_p->outputLength());
      } break;
      case SAW_RETURN: {
        input = '\r';
        ASSERT(3 == d_object_p->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == d_object_p->isAccepting());
        ASSERT(0 == d_object_p->isDone());
        ASSERT(0 == d_object_p->isError());
        ASSERT(0 == d_object_p->isInitialState());
        ASSERT(0 == d_object_p->outputLength());
      } break;
      case SAW_WHITE: {
        input = ' ';
        ASSERT(3 == d_object_p->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == d_object_p->isAccepting());
        ASSERT(0 == d_object_p->isDone());
        ASSERT(0 == d_object_p->isError());
        ASSERT(0 == d_object_p->isInitialState());
        ASSERT(0 == d_object_p->outputLength());
      } break;
      case DONE_STATE: {
        ASSERT(0 == d_object_p->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(1 == d_object_p->isAccepting());
        ASSERT(1 == d_object_p->isDone());
        ASSERT(0 == d_object_p->isError());
        ASSERT(0 == d_object_p->isInitialState());
        ASSERT(0 == d_object_p->outputLength()); // In this case, we know!
      } break;
      case ERROR_STATE: {
        ASSERT(0 == d_object_p->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(-1 == d_object_p->endConvert(b, &numOut));
        ASSERT(0 == numOut);

        ASSERT(0 == d_object_p->isAccepting());
        ASSERT(0 == d_object_p->isDone());
        ASSERT(1 == d_object_p->isError());
        ASSERT(0 == d_object_p->isInitialState());
        ASSERT(0 == d_object_p->outputLength());
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
    explicit
    EnabledGuard(bool flag)
        // Create a guard to control the activation of individual assertions
        // in the '::isState' test helper function using the specified
        // enable 'flag' value.  If 'flag' is 'true' individual false values
        // we be reported as assertion errors.
    : d_state(globalAssertsEnabled) { globalAssertsEnabled = flag; }

    ~EnabledGuard() { globalAssertsEnabled = d_state; }
};

bool StateAccessor::isState(State state) const
{
    ASSERT(d_object_p);
    ASSERT(INITIAL_STATE <= (int)state && (int)state <= ERROR_STATE);
    ASSERT(state < NUM_STATES);

    int enabled = globalAssertsEnabled;

    char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
    int numOut = -1;

    bool rv = false;

    switch (state) {
      case INITIAL_STATE: {
        bool a0 = 1 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 0 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 0 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 1 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 1 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 0 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);
        bool b4 = 0 == d_object_p->outputLength();      ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;

      } break;
      case STATE_ZERO: {
        bool a0 = 1 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 0 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 0 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 0 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 1 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 0 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);
        bool b4 = 0 <  d_object_p->outputLength();      ASSERT(b4 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 0 == numOut;                          ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
          && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;

      } break;
      case SAW_RETURN: {
        bool a0 = 0 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 0 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 0 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 0 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 1 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 0 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == numOut;
        bool c2 = 6 == numOut;
        bool c3 = c1 || c2;                             ASSERT(c3 || !enabled);

        bool d0, d1, d2, d3, d4, d5, d6;

        if (numOut == 6) {
            d0 = '='  == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\r' == b[1];                          ASSERT(d1 || !enabled);
            d2 = '\n' == b[2];                          ASSERT(d2 || !enabled);
            d3 = '='  == b[0];                          ASSERT(d3 || !enabled);
            d4 = '0'  == b[1];                          ASSERT(d4 || !enabled);
            d5 = 'D'  == b[2];                          ASSERT(d5 || !enabled);
            d6 = -1   == b[3];                          ASSERT(d6 || !enabled);
        }
        else {
            d0 = '=' == b[0];                           ASSERT(d0 || !enabled);
            d1 = '0' == b[1];                           ASSERT(d1 || !enabled);
            d2 = 'D' == b[2];                           ASSERT(d2 || !enabled);
            d3 = -1  == b[3];                           ASSERT(d3 || !enabled);
            d4 = -1  == b[4];                           ASSERT(d4 || !enabled);
            d5 = -1  == b[5];                           ASSERT(d5 || !enabled);
            d6 = -1  == b[6];                           ASSERT(d6 || !enabled);
        }

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c3 && d0 && d1 && d2 && d3 && d4 && d5 && d6;

      } break;
      case SAW_WHITE: {
        bool a0 = 0 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 0 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 0 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 0 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // DONE_STATE
        bool b0 = 1 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 1 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 0 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);

        bool c0 = 0 == result;                          ASSERT(c0 || !enabled);
        bool c1 = 3 == numOut;
        bool c2 = 6 == numOut;
        bool c3 = c1 || c2;                             ASSERT(c3 || !enabled);

        const char *bb = b;
        bool d0, d1, d2;

        if (numOut == 6) {
            d0 = '='  == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\r' == b[1];                          ASSERT(d1 || !enabled);
            d2 = '\n' == b[2];                          ASSERT(d2 || !enabled);
            bb += 3;
        }
        else {
            d0 = -1  == b[4];                           ASSERT(d0 || !enabled);
            d1 = -1  == b[5];                           ASSERT(d1 || !enabled);
            d2 = -1  == b[6];                           ASSERT(d2 || !enabled);
        }

        bool d3 = '='  == bb[0];                        ASSERT(d3 || !enabled);

        bool d4 = '2' == bb[1];   // First the possibility of a space
        bool d5 = '0' == bb[2];
        bool d6 = '0' == bb[1];   // Then the possibility of a tab
        bool d7 = '9' == bb[2];
        bool d8 = (d4 && d5 || d6 && d7);               ASSERT(d8 || !enabled);

        bool d9 = -1  == bb[3];                         ASSERT(d9 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c3 && d0 && d1 && d2 && d3 && d8 && d9;

      } break;
      case DONE_STATE: {
        bool a0 = 1 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 1 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 0 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 0 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // ERROR_STATE
        bool b0 = 0 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 0 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 1 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;

      } break;
      case ERROR_STATE: {
        bool a0 = 0 == d_object_p->isAccepting();       ASSERT(a0 || !enabled);
        bool a1 = 0 == d_object_p->isDone();            ASSERT(a1 || !enabled);
        bool a2 = 1 == d_object_p->isError();           ASSERT(a2 || !enabled);
        bool a3 = 0 == d_object_p->isInitialState();    ASSERT(a3 || !enabled);

        int result = d_object_p->endConvert(b, &numOut);

        // ERROR_STATE
        bool b0 = 0 == d_object_p->isAccepting();       ASSERT(b0 || !enabled);
        bool b1 = 0 == d_object_p->isDone();            ASSERT(b1 || !enabled);
        bool b2 = 1 == d_object_p->isError();           ASSERT(b2 || !enabled);
        bool b3 = 0 == d_object_p->isInitialState();    ASSERT(b3 || !enabled);

        bool c0 = -1 == result;                         ASSERT(c0 || !enabled);
        bool c1 =  0 == numOut;                         ASSERT(c1 || !enabled);

        bool d0 = -1 == b[0];                           ASSERT(d0 || !enabled);
        bool d1 = -1 == b[1];                           ASSERT(d1 || !enabled);
        bool d2 = -1 == b[2];                           ASSERT(d2 || !enabled);
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        rv = a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
          && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;

      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
    return rv;
}

                        // ====================================
                        // Equivalence-Class Related Data Types
                        // ====================================

struct Range {
    // Range is made inclusive to support a point.

    unsigned char start;
    unsigned char end;
};

struct EquivalenceClass {
    //

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
};

Range printableRanges[]      = { {33, 60}, {62, 126} };
Range whitespaceRanges[]     = { {9, 9}, {32, 32} };
Range newlineRanges[]        = { {10, 10} };
Range carriageReturnRanges[] = { {13, 13} };
Range controlCharRanges[]    = { {0, 8}, {11, 12}, {14, 31}, {61, 61},
                                 {127, 255} };

int numPrintableRanges       = sizeof(printableRanges) / sizeof(Range);
int numWhitespaceRanges      = sizeof(whitespaceRanges) / sizeof(Range);
int numNewlineRanges         = 1;
int numCarriageReturnRanges  = 1;
int numControlCharRanges     = sizeof(controlCharRanges) / sizeof(Range);

const EquivalenceClass printable("Printable",
                                 printableRanges,
                                 numPrintableRanges);
const EquivalenceClass whitespace("Whitespace",
                                  whitespaceRanges,
                                  numWhitespaceRanges);
const EquivalenceClass newline("Newline",
                               newlineRanges,
                               numNewlineRanges);
const EquivalenceClass carriageReturn("Carriage Return",
                                      carriageReturnRanges,
                                      numCarriageReturnRanges);
const EquivalenceClass controlChar("Control Chararacter",
                                   controlCharRanges,
                                   numControlCharRanges);

const EquivalenceClass *const EquivalenceClass_p[] = {
    &printable,
    &whitespace,
    &newline,
    &carriageReturn,
    &controlChar
};


// enum EquivalenceClassId = {
//     PRINTABLE = 0,
//     WHITESPACE,
//     NEWLINE,
//     RETURN,
//     CONTROL
// };

                        // =======================
                        // State Transition Matrix
                        // =======================
class StateTransitionMatrix {
    const int               **d_matrix_p;
    StateAccessor            *d_stateAccessor_p;
    int                       d_numStates;
    const EquivalenceClass  **d_inputTypes_p;
    int                       d_numInputTypes;

    StateTransitionMatrix(const int              **matrix,
                          StateAccessor           *stateAccessor,
                          const EquivalenceClass **inputTypes,
                          int                      numTypes)
    : d_matrix_p(matrix)
    , d_stateAccessor_p(stateAccessor)
    , d_numStates(NUM_STATES)
    , d_inputTypes_p(inputTypes)
    , d_numInputTypes(numTypes)
    { }

    bool Test()
    {
        for (int i = 0; i < d_numStates; ++i) {
            for (int j = 0; j < d_numInputTypes; ++j) {
                const EquivalenceClass *type = d_inputTypes_p[j];
                for (int k = 0; k < (int)type->d_ranges.size(); ++k) {
                    const Range &range = type->d_ranges[k];
                    for (unsigned char m = range.start; m < range.end; ++m) {
                        d_stateAccessor_p->setState((State) i);
                        d_stateAccessor_p->makeStateTransitionOnInput(m);
                        if (!d_stateAccessor_p->isState(
                                (State) (d_matrix_p[i][j])))
                            return false;                             // RETURN
                    }
                }
            }
        }
        return true;
    }
};

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

                        // =============================
                        // Function findEquivalenceClass
                        // =============================

const EquivalenceClass& findEquivalenceClass(char ch)
    // Find the equivalence class to which the specified 'ch' belongs.
{
    if (ch == '\t' || ch == ' ') {
        return whitespace;                                            // RETURN
    }
    else if (ch == '\n') {
        return newline;                                               // RETURN
    }
    else if (ch == '\r') {
        return carriageReturn;                                        // RETURN
    }
    else if (33 <= ch && ch <= 60 || 62 <= ch && ch <= 126)   {
        return printable;                                             // RETURN
    }
    else {
        return controlChar;                                           // RETURN
    }
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

                        // ==============
                        // Function isHex
                        // ==============

inline
bool isHex(char ch)
    // Return 'true' is the specified 'ch' is a hexadecimal digit.  Note that
    // only uppercase letters are allowed since this function is only used to
    // check output from the converter being tested.
{
    return '0' <= ch && ch <= '9' || 'A' <= ch && ch <= 'F';
}

                        // ===================
                        // Function printCharN
                        // ===================

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

string printHex(const char* sequence, int length)
    // Print the specified character 'sequence' of specified 'length' to the
    // specified 'stream' and return a reference to the modifiable 'stream'
    // (if a character is not printable, its hexadecimal code is printed
    // instead).  The behavior is undefined unless 0 <= 'length' and sequence
    // refers to a valid area of memory of size at least 'length'.
{
    static char HEX[] = "0123456789ABCDEF";
    string str;

    for (int i = 0; i < length; ++i) {
        unsigned char u = static_cast<unsigned char>(sequence[i]);

        if (isprint(u)) {
            str += u;
        }
        else {
            str += '<';
            str += HEX[u/16];
            str += HEX[u%16];
            str += '>';
        }
    }
    return str;
}

void charToHex(char* hex, unsigned char ch, int numOut)
    // Place in the specified 'hex' the hexadecimal representation of the
    // specified 'ch' of the 'unsigned char' type, up to the specified 'numOut'
    // characters.
{
    static char HEX[] = "0123456789ABCDEF";

    if (numOut <= 1) {
        return;                                                       // RETURN
    }
    else if (numOut > 2) {
        hex[2] = 0;
    }

    hex[0] = HEX[ch >> 4];
    hex[1] = HEX[ch & 0x0F];
}

#if 1
                        // =================
                        // Function setState
                        // =================

void setState(bdlde::QuotedPrintableEncoder *object, int state)
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
    char input;
    char *const begin = &input;
    char *const end = &input + 1;

    switch (state) {
      case INITIAL_STATE: {
        ASSERT(1 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(1 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case STATE_ZERO: {
        input = 'A';
        ASSERT(0 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(1 == numOut); ASSERT(1 == numIn);

        ASSERT(1 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 < object->outputLength());
      } break;
      case SAW_RETURN: {
        input = '\r';
        ASSERT(3 == object->convert(b, &numOut, &numIn, begin, end));
        ASSERT(0 == numOut); ASSERT(1 == numIn);

        ASSERT(0 == object->isAccepting());
        ASSERT(0 == object->isDone());
        ASSERT(0 == object->isError());
        ASSERT(0 == object->isInitialState());
        ASSERT(0 == object->outputLength());
      } break;
      case SAW_WHITE: {
        input = ' ';
        ASSERT(3 == object->convert(b, &numOut, &numIn, begin, end));
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
    };
}
                        // ================
                        // Function isState
                        // ================
#if 0
static bool globalAssertsEnabled = false;
    // If set to true, will enable ASSERTs in '::isState' (for debugging).

class EnabledGuard {
    // Enable/Disable '::isState' ASSERTs for current scope; restore status at
    // end.  Note that guards can be nested.

    bool d_state;

  public:
    EnabledGuard(bool flag)
        // Create a guard to control the activation of individual assertions
        // in the '::isState' test helper function using the specified
        // enable 'flag' value.  If 'flag' is 'true' individual false values
        // we be reported as assertion errors.
    : d_state(globalAssertsEnabled) { globalAssertsEnabled = flag; }

    ~EnabledGuard() { globalAssertsEnabled = d_state; }
};
#endif

bool isState(bdlde::QuotedPrintableEncoder *object, int state)
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

    char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
    int numOut = -1;

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
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
            && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;    // RETURN
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
        bool c3 = 2 == numOut;
        bool c4 = 3 == numOut;
        bool c5 = 5 == numOut;
        bool c6 = c1 || c2 || c3 || c4 || c5;           ASSERT(c6 || !enabled);

        bool d0, d1, d2, d3, d4, d5, d6;

        switch (numOut) {
          case 5: {
            d0 = '\r' == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\n' == b[1];                          ASSERT(d1 || !enabled);
            d2 = '='  == b[2];                          ASSERT(d2 || !enabled);
            d3 =  isHex( b[3]);                         ASSERT(d3 || !enabled);
            d4 =  isHex( b[4]);                         ASSERT(d4 || !enabled);
            d5 =  -1  == b[5];                          ASSERT(d5 || !enabled);
            d6 =  -1  == b[6];                          ASSERT(d6 || !enabled);
          } break;
          case 3: {
            d0 = '\r' == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\n' == b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  != b[2];                          ASSERT(d2 || !enabled);
            d3 =  -1  == b[3];                          ASSERT(d3 || !enabled);
            d4 =  -1  == b[4];                          ASSERT(d4 || !enabled);
            d5 =  -1  == b[5];                          ASSERT(d5 || !enabled);
            d6 =  -1  == b[6];                          ASSERT(d6 || !enabled);
          } break;
          case 2: {
            d0 =  -1  != b[0];                          ASSERT(d0 || !enabled);
            d1 =  -1  != b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  == b[2];                          ASSERT(d2 || !enabled);
            d3 =  -1  == b[3];                          ASSERT(d3 || !enabled);
            d4 =  -1  == b[4];                          ASSERT(d4 || !enabled);
            d5 =  -1  == b[5];                          ASSERT(d5 || !enabled);
            d6 =  -1  == b[6];                          ASSERT(d6 || !enabled);
          } break;
          case 1: {
            d0 =  -1  != b[0];                          ASSERT(d0 || !enabled);
            d1 =  -1  == b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  == b[2];                          ASSERT(d2 || !enabled);
            d3 =  -1  == b[3];                          ASSERT(d3 || !enabled);
            d4 =  -1  == b[4];                          ASSERT(d4 || !enabled);
            d5 =  -1  == b[5];                          ASSERT(d5 || !enabled);
            d6 =  -1  == b[6];                          ASSERT(d6 || !enabled);
          } break;
          case 0: {
            d0 =  -1  == b[0];                          ASSERT(d0 || !enabled);
            d1 =  -1  == b[1];                          ASSERT(d1 || !enabled);
            d2 =  -1  == b[2];                          ASSERT(d2 || !enabled);
            d3 =  -1  == b[3];                          ASSERT(d3 || !enabled);
            d4 =  -1  == b[4];                          ASSERT(d4 || !enabled);
            d5 =  -1  == b[5];                          ASSERT(d5 || !enabled);
            d6 =  -1  == b[6];                          ASSERT(d6 || !enabled);
          } break;
        }

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3 && b4
            && c0 && c6 && d0 && d1 && d2 && d3 && d4 && d5 && d6;    // RETURN
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
        bool c1 = 3 == numOut;
        bool c2 = 6 == numOut;
        bool c3 = c1 || c2;                             ASSERT(c3 || !enabled);

        bool d0, d1, d2, d3, d4, d5, d6;

        if (numOut == 6) {
            d0 = '='  == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\r' == b[1];                          ASSERT(d1 || !enabled);
            d2 = '\n' == b[2];                          ASSERT(d2 || !enabled);
            d3 = '='  == b[0];                          ASSERT(d3 || !enabled);
            d4 = '0'  == b[1];                          ASSERT(d4 || !enabled);
            d5 = 'D'  == b[2];                          ASSERT(d5 || !enabled);
            d6 = -1   == b[3];                          ASSERT(d6 || !enabled);
        }
        else {
            d0 = '=' == b[0];                           ASSERT(d0 || !enabled);
            d1 = '0' == b[1];                           ASSERT(d1 || !enabled);
            d2 = 'D' == b[2];                           ASSERT(d2 || !enabled);
            d3 = -1  == b[3];                           ASSERT(d3 || !enabled);
            d4 = -1  == b[4];                           ASSERT(d4 || !enabled);
            d5 = -1  == b[5];                           ASSERT(d5 || !enabled);
            d6 = -1  == b[6];                           ASSERT(d6 || !enabled);
        }

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
            && c0 && c3 && d0 && d1 && d2 && d3 && d4 && d5 && d6;    // RETURN
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
        bool c1 = 3 == numOut;
        bool c2 = 6 == numOut;
        bool c3 = c1 || c2;                             ASSERT(c3 || !enabled);

        const char *bb = b;
        bool d0, d1, d2;

        if (numOut == 6) {
            d0 = '='  == b[0];                          ASSERT(d0 || !enabled);
            d1 = '\r' == b[1];                          ASSERT(d1 || !enabled);
            d2 = '\n' == b[2];                          ASSERT(d2 || !enabled);
            bb += 3;
        }
        else {
            d0 = -1  == b[4];                           ASSERT(d0 || !enabled);
            d1 = -1  == b[5];                           ASSERT(d1 || !enabled);
            d2 = -1  == b[6];                           ASSERT(d2 || !enabled);
        }

        bool d3 = '='  == bb[0];                        ASSERT(d3 || !enabled);

        bool d4 = '2' == bb[1];   // First the possibility of a space
        bool d5 = '0' == bb[2];
        bool d6 = '0' == bb[1];   // Then the possibility of a tab
        bool d7 = '9' == bb[2];
        bool d8 = (d4 && d5 || d6 && d7);               ASSERT(d8 || !enabled);

        bool d9 = -1  == bb[3];                         ASSERT(d9 || !enabled);

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
            && c0 && c3 && d0 && d1 && d2 && d3 && d8 && d9;          // RETURN
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
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
            && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;    // RETURN
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
        bool d3 = -1 == b[3];                           ASSERT(d3 || !enabled);
        bool d4 = -1 == b[4];                           ASSERT(d4 || !enabled);
        bool d5 = -1 == b[5];                           ASSERT(d5 || !enabled);
        bool d6 = -1 == b[6];                           ASSERT(d6 || !enabled);

        return a0 && a1 && a2 && a3 && b0 && b1 && b2 && b3
            && c0 && c1 && d0 && d1 && d2 && d3 && d4 && d5 && d6;    // RETURN
      } break;
      default: {
        ASSERT("Unknown State" && 0);
      } break;
    }
    return false;
}

const char* getStateInText(bdlde::QuotedPrintableEncoder *object)
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

#endif

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
    explicit
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
    explicit
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
        // Return a reference to the modifiable character at the current
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
    // int veryVeryVerbose = argc > 4;
    // int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
#if 0
      case 9: {
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

        V("In CRLF_MODE and 'maxLineLength' = 0 (actually INT_MAX)");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(Obj::CRLF_MODE, 0);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::CRLF_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, INT_MAX == obj.maxLineLength());
            }
        }

        V("In MIXED_MODE and with 'maxLineLength' = 5.");
        {
            for (int i = 0; i < NUM_STATES; ++i) {
                Obj obj(Obj::MIXED_MODE, 5);
                if (verbose) cout << "\t\t" << STATE_NAMES[i] << '.' << endl;
                setState(&obj, i);
                const bool SAME = INITIAL_STATE == i;
                if (veryVerbose) { T_ T_ T_ P(SAME) }
                LOOP_ASSERT(i, SAME == isState(&obj, INITIAL_STATE));
                obj.reset();
                LOOP_ASSERT(i, 1 == isState(&obj, INITIAL_STATE));

                LOOP_ASSERT(i, Obj::MIXED_MODE == obj.lineBreakMode());
                LOOP_ASSERT(i, 5 == obj.maxLineLength());
            }
        }

      } break;
      case 8: {
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
        // 0 = STATE_ZERO
        const int I = INITIAL_STATE;
        const int R = SAW_RETURN;
        const int W = SAW_WHITE;
        const int D = DONE_STATE;
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
#if 1
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
{ L_, "A",       -1, 0, 3, 1, 1, N,         -1, 0, D, 0, _,"A"               },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  1
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          2, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         1, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "A",        0, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "AB",      -1, 0, 3, 2, 2, N,         -1, 0, D, 0, _,"AB"              },
{ L_, "\r\n",    -1, 0, 3, 2, 2, N,         -1, 0, D, 0, _,"\r\n"            },

// *** Testing 'convert' following 'convert'.
{ L_, "A",        0, 0, 0, 0, 0, "",        -1, 0, 0, 0, 0,""                },

{ L_, "A",        0, 0, 0, 0, 0, "",         0, 0, 0, 0, 0,""                },
{ L_, "A",        0, 0, 0, 0, 0, "a",       -1, 0, 3, 1, 1,"a"               },

{ L_, "A",        0, 0, 0, 0, 0, "",         1, 0, 0, 0, 0,""                },
{ L_, "A",        0, 0, 0, 0, 0, "a",        0, 0, 0, 0, 0,""                },
{ L_, "A",        0, 0, 0, 0, 0, "ab",      -1, 0, 3, 2, 2,"ab"              },

{ L_, "A",        0, 0, 0, 0, 0, "",         2, 0, 0, 0, 0,""                },
{ L_, "A",        0, 0, 0, 0, 0, "a",        1, 0, 3, 1, 1,"a"               },
{ L_, "A",        0, 0, 0, 0, 0, "\n",       1, 2, 3, 1, 1,"="               },
{ L_, "A",        0, 0, 0, 0, 0, "ab",       0, 0, 0, 0, 0,""                },
{ L_, "A",        0, 0, 0, 0, 0, "abc",     -1, 0, 3, 3, 3,"abc"             },
{ L_, "A",        0, 0, 0, 0, 0, "a\nb",    -1, 0, 3, 5, 3,"a=0Ab"           },
{ L_, "\n",       0, 0, 0, 0, 0, "abc",     -1, 0, 3, 3, 3,"abc"             },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  2
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          3, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         2, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "A",        1, 0, 3, 1, 1, N,         -1, 0, D, 0, _,"A"               },
{ L_, "\n",       1, 2, 3, 1, 1, N,         -1, 0, D, 2, _,"=0A"             },
{ L_, "AB",       0, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "ABC",     -1, 0, 3, 3, 3, N,         -1, 0, D, 0, _,"ABC"             },
{ L_, "A\nB",    -1, 0, 3, 5, 3, N,         -1, 0, D, 0, _,"A=0AB"           },

//lin InputData1 L1 R1 S1 o1 i1  InputData2 L2 R2 S2 o2 i2 Output Data   D =  3
//--- ---------- -- -- -- -- --  ---------- -- -- -- -- -- ------------------
{ L_, N,          4, 0, D, 0, _, N,         -1,-1, E, 0, _,""                },
{ L_, "",         3, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "A",        2, 0, 3, 1, 1, N,         -1, 0, D, 0, _,"A"               },
{ L_, "\r",       2, 3, 1, 0, 1, N,         -1, 0, D, 3, _,"=0D"             },
{ L_, " ",        2, 3, 2, 0, 1, N,         -1, 0, D, 3, _,"=20"             },
{ L_, "AB",       1, 0, 3, 1, 1, N,         -1, 0, D, 0, _,"A"               },
{ L_, "A\n",      1, 0, 3, 1, 1, N,         -1, 0, D, 0, _,"A"               },
{ L_, "\nA",      1, 2, 3, 1, 1, N,         -1, 0, D, 2, _,"=0A"             },
{ L_, "\r\n",     1, 1, 3, 1, 2, N,         -1, 0, D, 1, _,"\r\n"            },
{ L_, "ABC",      0, 0, 0, 0, 0, N,         -1, 0, D, 0, _,""                },
{ L_, "ABCD",    -1, 0, 3, 4, 4, N,         -1, 0, D, 0, _,"ABCD"            },
{ L_, "A\nBC",   -1, 0, 3, 6, 4, N,         -1, 0, D, 0, _,"A=0ABC"          },
#endif
    };
#if 0
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
#endif

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

                const bool MODE = Obj::CRLF_MODE; // Relaxed Mode

                Obj obj(MODE);  // object under test.
                Obj obj1(MODE); // control for validating S1
                Obj obj2(MODE); // control for validating S2

                const int newDepth = LEN1 + LIMIT1;

                // The following partitions the table in verbose mode.

                if (depth != newDepth) {
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

                // *** Prepare to process first 'convert' or 'endConvert'. ***

                int totalOut = 0;
                int totalIn = 0;
                char *b = outputBuffer;

                int result1;
                int nOut1 = -1;
                int nIn1 = -1;

                if (IN1) {
                    const char *const B = IN1, *const E = IN1 + LEN1;
                    result1 = obj.convert(b, &nOut1, &nIn1, B, E, LIMIT1);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int no = -1;
                        int ni = -1;
                        int r1 = obj1.convert(z, &no, &ni, B, E, LIMIT1);
                        int r2 = obj2.convert(z, &no, &ni, B, E, LIMIT1);
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
                        int no = -1;
                        int r1 = obj1.endConvert(z, &no, LIMIT1);
                        int r2 = obj2.endConvert(z, &no, LIMIT1);
                        LOOP3_ASSERT(LINE, result1, r1, result1 == r1);
                        LOOP3_ASSERT(LINE, result1, r2, result1 == r2);
                    }
                    totalOut += nOut1;
                }

                LOOP3_ASSERT(LINE, R1, result1, R1 == result1);
                LOOP3_ASSERT(LINE, S1, getStateInText(&obj),
                             isState(&obj1, S1));
                LOOP3_ASSERT(LINE, N_OUT1, nOut1, N_OUT1 == nOut1);
                LOOP3_ASSERT(LINE, N_IN1, nIn1, N_IN1 == nIn1);

                // *** Prepare to process second 'convert' or 'endConvert'. ***

                b += totalOut;

                int result2;
                int nOut2 = -1;
                int nIn2 = -1;

                if (IN2) {
                    const char *const B = IN2, *const E = IN2 + LEN2;
                    result2 = obj.convert(b, &nOut2, &nIn2, B, E, LIMIT2);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int no = -1;
                        int ni = -1;
                        int r2 = obj2.convert(z, &no, &ni, B, E, LIMIT2);
                        LOOP3_ASSERT(LINE, result2, r2, result2 == r2);
                    }
                    totalOut += nOut2;
                    totalIn += nIn2;
                }
                else {
                    result2 = obj.endConvert(b, &nOut2, LIMIT2);
                    {
                        char z[OUTPUT_BUFFER_SIZE];
                        int no = -1;
                        int r2 = obj2.endConvert(z, &no, LIMIT2);
                        LOOP3_ASSERT(LINE, result2, r2, result2 == r2);
                    }
                    totalOut += nOut2;
                }

                LOOP3_ASSERT(LINE, R2, result2, R2 == result2);
                LOOP3_ASSERT(LINE, S2, getStateInText(&obj2),
                             isState(&obj2, S2));
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
            } // end for ti

        } // end block

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS WITH DEFAULT ARGUMENTS.
        //   Continue testing 'convert' and 'endConvert' with defaults
        //   arguments.
        //
        // Concerns:
        //   - That the conversion logic is correct.
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
        //
        // Testing:
        //   static int encodedLength(int numInputBytes, int maxLineLength);
        //
        //   DFLT convert(char *o, int *no, int *ni, begin, end, int mno = -1);
        //   DFLT endConvert(char *out, int *numOut, int maxNumOut = -1);
        //
        //   That output length is calculated and stored properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << endl << "\nVerifying Conversion Logic." << endl;
        {
            static const struct {
                int d_lineNum;          // source line number
                int d_maxLineLength;    // maximum length of output line
                int d_inputLength;      // number of input characters
                const char *d_input_p;  // input characters
                int d_outputLength;     // total length of output
                const char *d_output_p; // expected output data
            } DATA[] = {
//--------------^

  // *** DEPTH-ORDERED ENUMERATION: Depth = inputLength; (0 == maxLineLen) ***

//    v---------------------INPUT--------------------v  v-------OUTPUT------v
//lin LL #i Input Data                                  #o Output Data
//--- -- -- ------------------------------------------  -- ------------------

// DEPTH 0
{ L_,  0, 0,"",                                          0,""                },

// DEPTH 1
{ L_,  0, 1,"a",                                         1,"a"               },
{ L_,  0, 1,"A",                                         1,"A"               },
{ L_,  0, 1,"\"",                                        1,"\""              },
{ L_,  0, 1,"~",                                         1,"~"               },
{ L_,  0, 1,"0",                                         1,"0"               },
{ L_,  0, 1,"\x00",                                      3,"=00"             },
{ L_,  0, 1,"\x1F",                                      3,"=1F"             },
{ L_,  0, 1,"\t",                                        3,"=09"             },
{ L_,  0, 1," ",                                         3,"=20"             },
{ L_,  0, 1,"\r",                                        3,"=0D"             },
{ L_,  0, 1,"\n",                                        3,"=0A"             },
{ L_,  0, 1,"\x7F",                                      3,"=7F"             },

// DEPTH 2
// 2 of the same equivalence class (EC)
{ L_,  0, 2,"aa",                                        2,"aa"              },
{ L_,  0, 2,"AA",                                        2,"AA"              },
{ L_,  0, 2,"\"\"",                                      2,"\"\""            },
{ L_,  0, 2,"~~",                                        2,"~~"              },
{ L_,  0, 2,"00",                                        2,"00"              },
{ L_,  0, 2,"\x00\x00",                                  6,"=00=00"          },
{ L_,  0, 2,"\x1F\x1F",                                  6,"=1F=1F"          },
{ L_,  0, 2,"\t\t",                                      4,"\t=09"           },
{ L_,  0, 2,"  ",                                        4," =20"            },
{ L_,  0, 2,"\r\r",                                      6,"=0D=0D"          },
{ L_,  0, 2,"\n\n",                                      6,"=0A=0A"          },
{ L_,  0, 2,"\x7F""\x7F",                                6,"=7F=7F"          },
{ L_,  0, 2,"\xFE""\xFE",                                6,"=FE=FE"          },
{ L_,  0, 2,"\xFF""\xFF",                                6,"=FF=FF"          },

// 2 of different ECs involving printable characters
{ L_,  0, 2,"a ",                                        4,"a=20"            },
{ L_,  0, 2," a",                                        2," a"              },
{ L_,  0, 2,"a\r",                                       4,"a=0D"            },
{ L_,  0, 2,"\ra",                                       4,"=0Da"            },
{ L_,  0, 2,"a\n",                                       4,"a=0A"            },
{ L_,  0, 2,"\na",                                       4,"=0Aa"            },
{ L_,  0, 2,"a""\xFE",                                   4,"a=FE"            },
{ L_,  0, 2,"\xFE""a",                                   4,"=FEa"            },

// 2 of different ECs involving whitespace
{ L_,  0, 2," \r",                                       6,"=20=0D"          },
{ L_,  0, 2,"\r ",                                       6,"=0D=20"          },
{ L_,  0, 2," \n",                                       6,"=20=0A"          },
{ L_,  0, 2,"\n ",                                       6,"=0A=20"          },
{ L_,  0, 2," ""\xFE",                                   4," =FE"            },
{ L_,  0, 2,"\xFE"" ",                                   6,"=FE=20"          },

// 2 of different ECs involving carriage return
{ L_,  0, 2,"\r\n",                                      2,"\r\n"            },
{ L_,  0, 2,"\n\r",                                      6,"=0A=0D"          },
{ L_,  0, 2,"\r\xFE",                                    6,"=0D=FE"          },
{ L_,  0, 2,"\xFE""\r",                                  6,"=FE=0D"          },

// 2 of different ECs involving newline
{ L_,  0, 2,"\n""\xFE",                                  6,"=0A=FE"          },
{ L_,  0, 2,"\xFE""\n",                                  6,"=FE=0A"          },

// DEPTH 3
// 3 of the same ECs - 5 combinations
{ L_,  0, 3,"aaa",                                       3,"aaa"             },
{ L_,  0, 3,"   ",                                       5,"  =20"           },
{ L_,  0, 3,"\r\r\r",                                    9,"=0D=0D=0D"       },
{ L_,  0, 3,"\n\n\n",                                    9,"=0A=0A=0A"       },
{ L_,  0, 3,"\xFE""\xFE""\xFE",                          9,"=FE=FE=FE"       },

// 3 of different ECs involving 2 printable characters
// 3 * (5-1) = 12 combinations
{ L_,  0, 3,"aa ",                                       5,"aa=20"           },
{ L_,  0, 3," aa",                                       3," aa"             },
{ L_,  0, 3,"a a",                                       3,"a a"             },
{ L_,  0, 3,"aa\r",                                      5,"aa=0D"           },
{ L_,  0, 3,"\raa",                                      5,"=0Daa"           },
{ L_,  0, 3,"a\ra",                                      5,"a=0Da"           },
{ L_,  0, 3,"aa\n",                                      5,"aa=0A"           },
{ L_,  0, 3,"\naa",                                      5,"=0Aaa"           },
{ L_,  0, 3,"a\na",                                      5,"a=0Aa"           },
{ L_,  0, 3,"aa""\xFE",                                  5,"aa=FE"           },
{ L_,  0, 3,"\xFE""aa",                                  5,"=FEaa"           },
{ L_,  0, 3,"a""\xFE""a",                                5,"a=FEa"           },

// 3 of different ECs involving 2 whitespace characters
// 3 * (4 - 1) = 9 combinations
{ L_,  0, 3,"  \r",                                      7," =20=0D"         },
{ L_,  0, 3,"\r  ",                                      7,"=0D =20"         },
{ L_,  0, 3," \r ",                                      9,"=20=0D=20"       },
{ L_,  0, 3,"  \n",                                      7," =20=0A"         },
{ L_,  0, 3,"\n  ",                                      7,"=0A =20"         },
{ L_,  0, 3," \n ",                                      9,"=20=0A=20"       },
{ L_,  0, 3,"  ""\xFE",                                  5,"  =FE"           },
{ L_,  0, 3,"\xFE""  ",                                  7,"=FE =20"         },
{ L_,  0, 3," ""\xFE"" ",                                7," =FE=20"         },

// 3 of different ECs involving 2 carriage returns
// 3 * (3 - 1) = 6 combinations
{ L_,  0, 3,"\r\r\n",                                    5,"=0D\r\n"         },
{ L_,  0, 3,"\n\r\r",                                    9,"=0A=0D=0D"       },
{ L_,  0, 3,"\r\n\r",                                    5,"\r\n=0D"         },
{ L_,  0, 3,"\r\r""\xFE",                                9,"=0D=0D=FE"       },
{ L_,  0, 3,"\xFE""\r\r",                                9,"=FE=0D=0D"       },
{ L_,  0, 3,"\r""\xFE""\r",                              9,"=0D=FE=0D"       },

// 3 of different ECs involving 2 newlines
// 3 * (2 - 1) = 3 combinations
{ L_,  0, 3,"\n\n""\xFE",                                9,"=0A=0A=FE"       },
{ L_,  0, 3,"\xFE""\n\n",                                9,"=FE=0A=0A"       },
{ L_,  0, 3,"\n""\xFE""\n",                              9,"=0A=FE=0A"       },

// 3 of different ECs involving no 2 of a kind and only 1 printable
// 3 * 4 * 3 = 36 combinations
{ L_,  0, 3,"a \r",                                      7,"a=20=0D"         },
{ L_,  0, 3,"a \n",                                      7,"a=20=0A"         },
{ L_,  0, 3,"a ""\xFE",                                  5,"a =FE"           },
{ L_,  0, 3,"a\r ",                                      7,"a=0D=20"         },
{ L_,  0, 3,"a\n ",                                      7,"a=0A=20"         },
{ L_,  0, 3,"a""\xFE"" ",                                7,"a=FE=20"         },

{ L_,  0, 3,"a\r\n",                                     3,"a\r\n"           },
{ L_,  0, 3,"a\r""\xFE",                                 7,"a=0D=FE"         },
{ L_,  0, 3,"a\n\r",                                     7,"a=0A=0D"         },
{ L_,  0, 3,"a""\xFE""\r",                               7,"a=FE=0D"         },

{ L_,  0, 3,"a\n""\xFE",                                 7,"a=0A=FE"         },
{ L_,  0, 3,"a""\xFE""\n",                               7,"a=FE=0A"         },

{ L_,  0, 3," a\r",                                      5," a=0D"           },
{ L_,  0, 3," a\n",                                      5," a=0A"           },
{ L_,  0, 3," a""\xFE",                                  5," a=FE"           },
{ L_,  0, 3,"\ra ",                                      7,"=0Da=20"         },
{ L_,  0, 3,"\na ",                                      7,"=0Aa=20"         },
{ L_,  0, 3,"\xFE""a ",                                  7,"=FEa=20"         },

{ L_,  0, 3,"\ra\n",                                     7,"=0Da=0A"         },
{ L_,  0, 3,"\ra""\xFE",                                 7,"=0Da=FE"         },
{ L_,  0, 3,"\na\r",                                     7,"=0Aa=0D"         },
{ L_,  0, 3,"\xFE""a\r",                                 7,"=FEa=0D"         },

{ L_,  0, 3,"\na""\xFE",                                 7,"=0Aa=FE"         },
{ L_,  0, 3,"\xFE""a\n",                                 7,"=FEa=0A"         },

{ L_,  0, 3," \ra",                                      7,"=20=0Da"         },
{ L_,  0, 3," \na",                                      7,"=20=0Aa"         },
{ L_,  0, 3," \xFE""a",                                  5," =FEa"           },
{ L_,  0, 3," \ra",                                      7,"=20=0Da"         },
{ L_,  0, 3," \na",                                      7,"=20=0Aa"         },
{ L_,  0, 3," \xFE""a",                                  5," =FEa"           },

{ L_,  0, 3,"\r\na",                                     3,"\r\na"           },
{ L_,  0, 3,"\r""\xFE""a",                               7,"=0D=FEa"         },
{ L_,  0, 3,"\n\ra",                                     7,"=0A=0Da"         },
{ L_,  0, 3,"\xFE""\ra",                                 7,"=FE=0Da"         },

{ L_,  0, 3,"\n""\xFE""a",                               7,"=0A=FEa"         },
{ L_,  0, 3,"\xFE""\na",                                 7,"=FE=0Aa"         },

// 3 of different ECs involving no 2 of a kind, only 1 whitespace and no
// printable - 3 * 3 * 2 = 18 combinations
{ L_,  0, 3," \r\n",                                     5,"=20\r\n"         },
{ L_,  0, 3," \r""\xFE",                                 9,"=20=0D=FE"       },
{ L_,  0, 3," \n\r",                                     9,"=20=0A=0D"       },
{ L_,  0, 3," \xFE""\r",                                 7," =FE=0D"         },

{ L_,  0, 3," \n\xFE",                                   9,"=20=0A=FE"       },
{ L_,  0, 3," \xFE\n",                                   7," =FE=0A"         },

{ L_,  0, 3,"\r \n",                                     9,"=0D=20=0A"       },
{ L_,  0, 3,"\r ""\xFE",                                 7,"=0D =FE"         },
{ L_,  0, 3,"\n \r",                                     9,"=0A=20=0D"       },
{ L_,  0, 3,"\xFE"" \r",                                 9,"=FE=20=0D"       },

{ L_,  0, 3,"\n ""\xFE",                                 7,"=0A =FE"         },
{ L_,  0, 3,"\xFE"" \n",                                 9,"=FE=20=0A"       },

{ L_,  0, 3,"\r\n ",                                     5,"\r\n=20"         },
{ L_,  0, 3,"\r""\xFE ",                                 9,"=0D=FE=20"       },
{ L_,  0, 3,"\n\r ",                                     9,"=0A=0D=20"       },
{ L_,  0, 3,"\xFE""\r ",                                 9,"=FE=0D=20"       },

{ L_,  0, 3,"\n""\xFE ",                                 9,"=0A=FE=20"       },
{ L_,  0, 3,"\xFE""\n ",                                 9,"=FE=0A=20"       },

// 3 of different ECs involving no 2 of a kind, only 1 carriage return and no
// printable or whitespace - 3 * 2 * 1 = 6 combinations
{ L_,  0, 3,"\r\n""\xFE",                                5,"\r\n=FE"         },
{ L_,  0, 3,"\r""\xFE""\n",                              9,"=0D=FE=0A"       },

{ L_,  0, 3,"\n\r""\xFE",                                9,"=0A=0D=FE"       },
{ L_,  0, 3,"\xFE""\r\n",                                5,"=FE\r\n"         },

{ L_,  0, 3,"\n""\xFE""\r",                              9,"=0A=FE=0D"       },
{ L_,  0, 3,"\xFE""\n\r",                                9,"=FE=0A=0D"       },

// DEPTH 4
// 4 of the same ECs - 5 combinations
{ L_,  0, 4,"aaaa",                                      4,"aaaa"            },
{ L_,  0, 4,"    ",                                      6,"   =20"          },
{ L_,  0, 4,"\r\r\r\r",                                 12,"=0D=0D=0D=0D"    },
{ L_,  0, 4,"\n\n\n\n",                                 12,"=0A=0A=0A=0A"    },
{ L_,  0, 4,"\xFE""\xFE""\xFE""\xFE",                   12,"=FE=FE=FE=FE"    },

// Only consider the interesting cases.
{ L_,  0, 4,"   a",                                      4,"   a"            },
{ L_,  0, 4,"  a ",                                      6,"  a=20"          },
{ L_,  0, 4," a  ",                                      6," a =20"          },
{ L_,  0, 4,"a   ",                                      6,"a  =20"          },

{ L_,  0, 4,"\r\n\r\n",                                  4,"\r\n\r\n"        },
{ L_,  0, 4,"\r\naa",                                    4,"\r\naa"          },
{ L_,  0, 4,"a\r\na",                                    4,"a\r\na"          },
{ L_,  0, 4,"aa\r\n",                                    4,"aa\r\n"          },
{ L_,  0, 4,"\ra\na",                                    8,"=0Da=0Aa"        },


    // *** DEPTH-ORDERED ENUMERATION: Depth = inputLength + maxLineLength ***

//    v----------INPUT-----------v  v-----------------OUTPUT---------------v
//lin LL #i input data              #o output data
//--- -- -- ----------------------  -- -------------------------------------
{ L_,  0, 0,"",                      0,""                                    },

// Note that LL must be >= 4 or LL == 0.
//
// Note that the cases corresponding to LL=0 have been studied quite
// extensively above.  So for the study of each depth below, select only 1 case
// for completeness.
//

// DEPTH 1
{ L_,  0, 1,"a",                     1,"a"                                   },

// DEPTH 2
{ L_,  0, 2,"aa",                    2,"aa"                                  },

// DEPTH 3
{ L_,  0, 3,"aaa",                   3,"aaa"                                 },

// DEPTH 4
{ L_,  4, 0,"",                      0,""                                    },
{ L_,  0, 4,"aaaa",                  4,"aaaa"                                },

// DEPTH 5
{ L_,  5, 0,"",                      0,""                                    },
{ L_,  4, 1,"a",                     1,"a"                                   },
{ L_,  4, 1," ",                     3,"=20"                                 },
{ L_,  4, 1,"\r",                    3,"=0D"                                 },
{ L_,  4, 1,"\n",                    3,"=0A"                                 },
{ L_,  4, 1,"\xFE",                  3,"=FE"                                 },
{ L_,  0, 5,"aaaaa",                 5,"aaaaa"                               },

// DEPTH 6
{ L_,  6, 0,"",                      0,""                                    },
{ L_,  5, 1,"a",                     1,"a"                                   },
{ L_,  5, 1," ",                     3,"=20"                                 },
{ L_,  5, 1,"\r",                    3,"=0D"                                 },
{ L_,  5, 1,"\n",                    3,"=0A"                                 },
{ L_,  5, 1,"\xFE",                  3,"=FE"                                 },

// 2 of the same equivalence class (EC)
{ L_,  4, 2,"aa",                    2,"aa"                                  },
{ L_,  4, 2,"  ",                    7," =\r\n=20"                           },
{ L_,  4, 2,"\r\r",                  9,"=0D=\r\n=0D"                         },
{ L_,  4, 2,"\n\n",                  9,"=0A=\r\n=0A"                         },
{ L_,  4, 2,"\xFE""\xFE",            9,"=FE=\r\n=FE"                         },

// 2 of different ECs involving printable characters
{ L_,  4, 2,"a ",                    7,"a=\r\n=20"                           },
{ L_,  4, 2," a",                    2," a"                                  },
{ L_,  4, 2,"a\r",                   7,"a=\r\n=0D"                           },
{ L_,  4, 2,"\ra",                   7,"=0D=\r\na"                           },
{ L_,  4, 2,"a\n",                   7,"a=\r\n=0A"                           },
{ L_,  4, 2,"\na",                   7,"=0A=\r\na"                           },
{ L_,  4, 2,"a""\xFE",               7,"a=\r\n=FE"                           },
{ L_,  4, 2,"\xFE""a",               7,"=FE=\r\na"                           },

// 2 of different ECs involving whitespace
{ L_,  4, 2," \r",                   9,"=20=\r\n=0D"                         },
{ L_,  4, 2,"\r ",                   9,"=0D=\r\n=20"                         },
{ L_,  4, 2," \n",                   9,"=20=\r\n=0A"                         },
{ L_,  4, 2,"\n ",                   9,"=0A=\r\n=20"                         },
{ L_,  4, 2," \xFE",                 7," =\r\n=FE"                           },
{ L_,  4, 2,"\xFE"" ",               9,"=FE=\r\n=20"                         },

// 2 of different ECs involving carriage return
{ L_,  4, 2,"\r\n",                  2,"\r\n"                                },
{ L_,  4, 2,"\n\r",                  9,"=0A=\r\n=0D"                         },
{ L_,  4, 2,"\r""\xFE",              9,"=0D=\r\n=FE"                         },
{ L_,  4, 2,"\xFE""\r",              9,"=FE=\r\n=0D"                         },

// 2 of different ECs involving newline
{ L_,  4, 2,"\n""\xFE",              9,"=0A=\r\n=FE"                         },
{ L_,  4, 2,"\xFE""\n",              9,"=FE=\r\n=0A"                         },

{ L_,  0, 6,"aaaaaa",                6,"aaaaaa"                              },

// DEPTH 7
{ L_,  7, 0,"",                      0,""                                    },
{ L_,  6, 1,"a",                     1,"a"                                   },
{ L_,  6, 1," ",                     3,"=20"                                 },
{ L_,  6, 1,"\r",                    3,"=0D"                                 },
{ L_,  6, 1,"\n",                    3,"=0A"                                 },
{ L_,  6, 1,"\xFE",                  3,"=FE"                                 },

// MAX LINE LENGTH = 5   INPUT LENGTH = 2
// 2 of the same equivalence class (EC)
{ L_,  5, 2,"aa",                    2,"aa"                                  },
{ L_,  5, 2,"  ",                    4," =20"                                },
{ L_,  5, 2,"\r\r",                  9,"=0D=\r\n=0D"                         },
{ L_,  5, 2,"\n\n",                  9,"=0A=\r\n=0A"                         },
{ L_,  5, 2,"\xFE""\xFE",            9,"=FE=\r\n=FE"                         },

// 2 of different ECs involving printable characters
{ L_,  5, 2,"a ",                    4,"a=20"                                },
{ L_,  5, 2," a",                    2," a"                                  },
{ L_,  5, 2,"a\r",                   4,"a=0D"                                },
{ L_,  5, 2,"\ra",                   4,"=0Da"                                },
{ L_,  5, 2,"a\n",                   4,"a=0A"                                },
{ L_,  5, 2,"\na",                   4,"=0Aa"                                },
{ L_,  5, 2,"a""\xFE",               4,"a=FE"                                },
{ L_,  5, 2,"\xFE""a",               4,"=FEa"                                },

// 2 of different ECs involving whitespace
{ L_,  5, 2," \r",                   9,"=20=\r\n=0D"                         },
{ L_,  5, 2,"\r ",                   9,"=0D=\r\n=20"                         },
{ L_,  5, 2," \n",                   9,"=20=\r\n=0A"                         },
{ L_,  5, 2,"\n ",                   9,"=0A=\r\n=20"                         },
{ L_,  5, 2," ""\xFE",               4," =FE"                                },
{ L_,  5, 2,"\xFE"" ",               9,"=FE=\r\n=20"                         },

// 2 of different ECs involving carriage return
{ L_,  5, 2,"\r\n",                  2,"\r\n"                                },
{ L_,  5, 2,"\n\r",                  9,"=0A=\r\n=0D"                         },
{ L_,  5, 2,"\r""\xFE",              9,"=0D=\r\n=FE"                         },
{ L_,  5, 2,"\xFE""\r",              9,"=FE=\r\n=0D"                         },

// 2 of different ECs involving newline
{ L_,  5, 2,"\n""\xFE",              9,"=0A=\r\n=FE"                         },
{ L_,  5, 2,"\xFE""\n",              9,"=FE=\r\n=0A"                         },

// MAX LINE LENGTH = 4   INPUT LENGTH = 3
// 3 of the same ECs - 5 combinations
{ L_,  4, 3,"aaa",                   3,"aaa"                                 },
{ L_,  4, 3,"   ",                   8,"  =\r\n=20"                          },
{ L_,  4, 3,"\r\r\r",               15,"=0D=\r\n=0D=\r\n=0D"                 },
{ L_,  4, 3,"\n\n\n",               15,"=0A=\r\n=0A=\r\n=0A"                 },
{ L_,  4, 3,"\xFE""\xFE""\xFE",     15,"=FE=\r\n=FE=\r\n=FE"                 },

// 3 of different ECs involving 2 printable characters
// 3 * (5-1) = 12 combinations
{ L_,  4, 3,"aa ",                   8,"aa=\r\n=20"                          },
{ L_,  4, 3," aa",                   3," aa"                                 },
{ L_,  4, 3,"a a",                   3,"a a"                                 },
{ L_,  4, 3,"aa\r",                  8,"aa=\r\n=0D"                          },
{ L_,  4, 3,"\raa",                  8,"=0D=\r\naa"                          },
{ L_,  4, 3,"a\ra",                 11,"a=\r\n=0D=\r\na"                     },
{ L_,  4, 3,"aa\n",                  8,"aa=\r\n=0A"                          },
{ L_,  4, 3,"\naa",                  8,"=0A=\r\naa"                          },
{ L_,  4, 3,"a\na",                 11,"a=\r\n=0A=\r\na"                     },
{ L_,  4, 3,"aa""\xFE",              8,"aa=\r\n=FE"                          },
{ L_,  4, 3,"\xFE""aa",              8,"=FE=\r\naa"                          },
{ L_,  4, 3,"a""\xFE""a",           11,"a=\r\n=FE=\r\na"                     },

// 3 of different ECs involving 2 whitespace characters
// 3 * (4 - 1) = 9 combinations
{ L_,  4, 3,"  \r",                 13," =\r\n=20=\r\n=0D"                   },
{ L_,  4, 3,"\r  ",                 13,"=0D=\r\n =\r\n=20"                   },
{ L_,  4, 3," \r ",                 15,"=20=\r\n=0D=\r\n=20"                 },
{ L_,  4, 3,"  \n",                 13," =\r\n=20=\r\n=0A"                   },
{ L_,  4, 3,"\n  ",                 13,"=0A=\r\n =\r\n=20"                   },
{ L_,  4, 3," \n ",                 15,"=20=\r\n=0A=\r\n=20"                 },
{ L_,  4, 3,"  ""\xFE",              8,"  =\r\n=FE"                          },
{ L_,  4, 3,"\xFE""  ",             13,"=FE=\r\n =\r\n=20"                   },
{ L_,  4, 3," ""\xFE"" ",           13," =\r\n=FE=\r\n=20"                   },

// 3 of different ECs involving 2 carriage returns
// 3 * (3 - 1) = 6 combinations
{ L_,  4, 3,"\r\r\n",                5,"=0D\r\n"                             },
{ L_,  4, 3,"\n\r\r",               15,"=0A=\r\n=0D=\r\n=0D"                 },
{ L_,  4, 3,"\r\n\r",                5,"\r\n=0D"                             },
{ L_,  4, 3,"\r\r\xFE",             15,"=0D=\r\n=0D=\r\n=FE"                 },
{ L_,  4, 3,"\xFE""\r\r",           15,"=FE=\r\n=0D=\r\n=0D"                 },
{ L_,  4, 3,"\r""\xFE""\r",         15,"=0D=\r\n=FE=\r\n=0D"                 },

// 3 of different ECs involving 2 newlines
// 3 * (2 - 1) = 3 combinations
{ L_,  4, 3,"\n\n""\xFE",           15,"=0A=\r\n=0A=\r\n=FE"                 },
{ L_,  4, 3,"\xFE""\n\n",           15,"=FE=\r\n=0A=\r\n=0A"                 },
{ L_,  4, 3,"\n""\xFE""\n",         15,"=0A=\r\n=FE=\r\n=0A"                 },

// 3 of different ECs involving no 2 of a kind and only 1 printable
// 3 * 4 * 3 = 36 combinations
{ L_,  4, 3,"a \r",                 13,"a=\r\n=20=\r\n=0D"                   },
{ L_,  4, 3,"a \n",                 13,"a=\r\n=20=\r\n=0A"                   },
{ L_,  4, 3,"a ""\xFE",              8,"a =\r\n=FE"                          },
{ L_,  4, 3,"a\r ",                 13,"a=\r\n=0D=\r\n=20"                   },
{ L_,  4, 3,"a\n ",                 13,"a=\r\n=0A=\r\n=20"                   },
{ L_,  4, 3,"a""\xFE"" ",           13,"a=\r\n=FE=\r\n=20"                   },

{ L_,  4, 3,"a\r\n",                 3,"a\r\n"                               },
{ L_,  4, 3,"a\r""\xFE",            13,"a=\r\n=0D=\r\n=FE"                   },
{ L_,  4, 3,"a\n\r",                13,"a=\r\n=0A=\r\n=0D"                   },
{ L_,  4, 3,"a""\xFE""\r",          13,"a=\r\n=FE=\r\n=0D"                   },

{ L_,  4, 3,"a\n""\xFE",            13,"a=\r\n=0A=\r\n=FE"                   },
{ L_,  4, 3,"a""\xFE""\n",          13,"a=\r\n=FE=\r\n=0A"                   },

{ L_,  4, 3," a\r",                  8," a=\r\n=0D"                          },
{ L_,  4, 3," a\n",                  8," a=\r\n=0A"                          },
{ L_,  4, 3," a""\xFE",              8," a=\r\n=FE"                          },
{ L_,  4, 3,"\ra ",                 13,"=0D=\r\na=\r\n=20"                   },
{ L_,  4, 3,"\na ",                 13,"=0A=\r\na=\r\n=20"                   },
{ L_,  4, 3,"\xFE""a ",             13,"=FE=\r\na=\r\n=20"                   },

{ L_,  4, 3,"\ra\n",                13,"=0D=\r\na=\r\n=0A"                   },
{ L_,  4, 3,"\ra""\xFE",            13,"=0D=\r\na=\r\n=FE"                   },
{ L_,  4, 3,"\na\r",                13,"=0A=\r\na=\r\n=0D"                   },
{ L_,  4, 3,"\xFE""a\r",            13,"=FE=\r\na=\r\n=0D"                   },

{ L_,  4, 3,"\na""\xFE",            13,"=0A=\r\na=\r\n=FE"                   },
{ L_,  4, 3,"\xFE""a\n",            13,"=FE=\r\na=\r\n=0A"                   },

{ L_,  4, 3," \ra",                 13,"=20=\r\n=0D=\r\na"                   },
{ L_,  4, 3," \na",                 13,"=20=\r\n=0A=\r\na"                   },
{ L_,  4, 3," ""\xFE""a",           11," =\r\n=FE=\r\na"                     },
{ L_,  4, 3," \ra",                 13,"=20=\r\n=0D=\r\na"                   },
{ L_,  4, 3," \na",                 13,"=20=\r\n=0A=\r\na"                   },
{ L_,  4, 3," ""\xFE""a",           11," =\r\n=FE=\r\na"                     },

{ L_,  4, 3,"\r\na",                 3,"\r\na"                               },
{ L_,  4, 3,"\r""\xFE""a",          13,"=0D=\r\n=FE=\r\na"                   },
{ L_,  4, 3,"\n\ra",                13,"=0A=\r\n=0D=\r\na"                   },
{ L_,  4, 3,"\xFE""\ra",            13,"=FE=\r\n=0D=\r\na"                   },

{ L_,  4, 3,"\n""\xFE""a",          13,"=0A=\r\n=FE=\r\na"                   },
{ L_,  4, 3,"\xFE""\na",            13,"=FE=\r\n=0A=\r\na"                   },

// 3 of different ECs involving no 2 of a kind, only 1 whitespace and no
// printable - 3 * 3 * 2 = 18 combinations
{ L_,  4, 3," \r\n",                 5,"=20\r\n"                             },
{ L_,  4, 3," \r""\xFE",            15,"=20=\r\n=0D=\r\n=FE"                 },
{ L_,  4, 3," \n\r",                15,"=20=\r\n=0A=\r\n=0D"                 },
{ L_,  4, 3," \xFE""\r",            13," =\r\n=FE=\r\n=0D"                   },

{ L_,  4, 3," \n""\xFE",            15,"=20=\r\n=0A=\r\n=FE"                 },
{ L_,  4, 3," \xFE""\n",            13," =\r\n=FE=\r\n=0A"                   },

{ L_,  4, 3,"\r \n",                15,"=0D=\r\n=20=\r\n=0A"                 },
{ L_,  4, 3,"\r ""\xFE",            13,"=0D=\r\n =\r\n=FE"                   },
{ L_,  4, 3,"\n \r",                15,"=0A=\r\n=20=\r\n=0D"                 },
{ L_,  4, 3,"\xFE"" \r",            15,"=FE=\r\n=20=\r\n=0D"                 },

{ L_,  4, 3,"\n ""\xFE",            13,"=0A=\r\n =\r\n=FE"                   },
{ L_,  4, 3,"\xFE"" \n",            15,"=FE=\r\n=20=\r\n=0A"                 },

{ L_,  4, 3,"\r\n ",                 5,"\r\n=20"                             },
{ L_,  4, 3,"\r\xFE"" ",            15,"=0D=\r\n=FE=\r\n=20"                 },
{ L_,  4, 3,"\n\r ",                15,"=0A=\r\n=0D=\r\n=20"                 },
{ L_,  4, 3,"\xFE""\r ",            15,"=FE=\r\n=0D=\r\n=20"                 },

{ L_,  4, 3,"\n\xFE"" ",            15,"=0A=\r\n=FE=\r\n=20"                 },
{ L_,  4, 3,"\xFE""\n ",            15,"=FE=\r\n=0A=\r\n=20"                 },

// 3 of different ECs involving no 2 of a kind, only 1 carriage return and no
// printable or whitespace - 3 * 2 * 1 = 6 combinations
{ L_,  4, 3,"\r\n""\xFE",            5,"\r\n=FE"                             },
{ L_,  4, 3,"\r""\xFE""\n",         15,"=0D=\r\n=FE=\r\n=0A"                 },

{ L_,  4, 3,"\n\r""\xFE",           15,"=0A=\r\n=0D=\r\n=FE"                 },
{ L_,  4, 3,"\xFE""\r\n",            5,"=FE\r\n"                             },

{ L_,  4, 3,"\n""\xFE""\r",         15,"=0A=\r\n=FE=\r\n=0D"                 },
{ L_,  4, 3,"\xFE""\n\r",           15,"=FE=\r\n=0A=\r\n=0D"                 },

{ L_,  0, 7,"aaaaaaa",               7,"aaaaaaa"                             },

// DEPTH 8
{ L_,  8, 0,"",                      0,""                                    },
{ L_,  7, 1,"a",                     1,"a"                                   },
{ L_,  7, 1," ",                     3,"=20"                                 },
{ L_,  7, 1,"\r",                    3,"=0D"                                 },
{ L_,  7, 1,"\n",                    3,"=0A"                                 },
{ L_,  7, 1,"\xFE",                  3,"=FE"                                 },

// MAX LINE LENGTH = 6   INPUT LENGTH = 2
// 2 of the same equivalence class (EC)
{ L_,  6, 2,"aa",                    2,"aa"                                  },
{ L_,  6, 2,"  ",                    4," =20"                                },
{ L_,  6, 2,"\r\r",                  9,"=0D=\r\n=0D"                         },
{ L_,  6, 2,"\n\n",                  9,"=0A=\r\n=0A"                         },
{ L_,  6, 2,"\xFE""\xFE",            9,"=FE=\r\n=FE"                         },

// 2 of different ECs involving printable characters
{ L_,  6, 2,"a ",                    4,"a=20"                                },
{ L_,  6, 2," a",                    2," a"                                  },
{ L_,  6, 2,"a\r",                   4,"a=0D"                                },
{ L_,  6, 2,"\ra",                   4,"=0Da"                                },
{ L_,  6, 2,"a\n",                   4,"a=0A"                                },
{ L_,  6, 2,"\na",                   4,"=0Aa"                                },
{ L_,  6, 2,"a""\xFE",               4,"a=FE"                                },
{ L_,  6, 2,"\xFE""a",               4,"=FEa"                                },

// 2 of different ECs involving whitespace
{ L_,  6, 2," \r",                   9,"=20=\r\n=0D"                         },
{ L_,  6, 2,"\r ",                   9,"=0D=\r\n=20"                         },
{ L_,  6, 2," \n",                   9,"=20=\r\n=0A"                         },
{ L_,  6, 2,"\n ",                   9,"=0A=\r\n=20"                         },
{ L_,  6, 2," ""\xFE",               4," =FE"                                },
{ L_,  6, 2,"\xFE"" ",               9,"=FE=\r\n=20"                         },

// 2 of different ECs involving carriage return
{ L_,  6, 2,"\r\n",                  2,"\r\n"                                },
{ L_,  6, 2,"\n\r",                  9,"=0A=\r\n=0D"                         },
{ L_,  6, 2,"\r""\xFE",              9,"=0D=\r\n=FE"                         },
{ L_,  6, 2,"\xFE""\r",              9,"=FE=\r\n=0D"                         },

// 2 of different ECs involving newline
{ L_,  6, 2,"\n""\xFE",              9,"=0A=\r\n=FE"                         },
{ L_,  6, 2,"\xFE""\n",              9,"=FE=\r\n=0A"                         },

// MAX LINE LENGTH = 5   INPUT LENGTH = 3
// 3 of the same ECs - 5 combinations
{ L_,  5, 3,"aaa",                   3,"aaa"                                 },
{ L_,  5, 3,"   ",                   8,"  =\r\n=20"                          },
{ L_,  5, 3,"\r\r\r",               15,"=0D=\r\n=0D=\r\n=0D"                 },
{ L_,  5, 3,"\n\n\n",               15,"=0A=\r\n=0A=\r\n=0A"                 },
{ L_,  5, 3,"\xFE""\xFE""\xFE",     15,"=FE=\r\n=FE=\r\n=FE"                 },

// 3 of different ECs involving 2 printable characters
// 3 * (5-1) = 12 combinations
{ L_,  5, 3,"aa ",                   8,"aa=\r\n=20"                          },
{ L_,  5, 3," aa",                   3," aa"                                 },
{ L_,  5, 3,"a a",                   3,"a a"                                 },
{ L_,  5, 3,"aa\r",                  8,"aa=\r\n=0D"                          },
{ L_,  5, 3,"\raa",                  8,"=0Da=\r\na"                          },
{ L_,  5, 3,"a\ra",                  8,"a=0D=\r\na"                          },
{ L_,  5, 3,"aa\n",                  8,"aa=\r\n=0A"                          },
{ L_,  5, 3,"\naa",                  8,"=0Aa=\r\na"                          },
{ L_,  5, 3,"a\na",                  8,"a=0A=\r\na"                          },
{ L_,  5, 3,"aa""\xFE",              8,"aa=\r\n=FE"                          },
{ L_,  5, 3,"\xFE""aa",              8,"=FEa=\r\na"                          },
{ L_,  5, 3,"a""\xFE""a",            8,"a=FE=\r\na"                          },

// 3 of different ECs involving 2 whitespace characters
// 3 * (4 - 1) = 9 combinations
{ L_,  5, 3,"  \r",                 10," =20=\r\n=0D"                        },
{ L_,  5, 3,"\r  ",                 10,"=0D =\r\n=20"                        },
{ L_,  5, 3," \r ",                 15,"=20=\r\n=0D=\r\n=20"                 },
{ L_,  5, 3,"  \n",                 10," =20=\r\n=0A"                        },
{ L_,  5, 3,"\n  ",                 10,"=0A =\r\n=20"                        },
{ L_,  5, 3," \n ",                 15,"=20=\r\n=0A=\r\n=20"                 },
{ L_,  5, 3,"  ""\xFE",              8,"  =\r\n=FE"                          },
{ L_,  5, 3,"\xFE""  ",             10,"=FE =\r\n=20"                        },
{ L_,  5, 3," ""\xFE"" ",           10," =FE=\r\n=20"                        },

// 3 of different ECs involving 2 carriage returns
// 3 * (3 - 1) = 6 combinations
{ L_,  5, 3,"\r\r\n",                5,"=0D\r\n"                             },
{ L_,  5, 3,"\n\r\r",               15,"=0A=\r\n=0D=\r\n=0D"                 },
{ L_,  5, 3,"\r\n\r",                5,"\r\n=0D"                             },
{ L_,  5, 3,"\r\r""\xFE",           15,"=0D=\r\n=0D=\r\n=FE"                 },
{ L_,  5, 3,"\xFE""\r\r",           15,"=FE=\r\n=0D=\r\n=0D"                 },
{ L_,  5, 3,"\r""\xFE""\r",         15,"=0D=\r\n=FE=\r\n=0D"                 },

// 3 of different ECs involving 2 newlines
// 3 * (2 - 1) = 3 combinations
{ L_,  5, 3,"\n\n""\xFE",           15,"=0A=\r\n=0A=\r\n=FE"                 },
{ L_,  5, 3,"\xFE""\n\n",           15,"=FE=\r\n=0A=\r\n=0A"                 },
{ L_,  5, 3,"\n""\xFE""\n",         15,"=0A=\r\n=FE=\r\n=0A"                 },

// 3 of different ECs involving no 2 of a kind and only 1 printable
// 3 * 4 * 3 = 36 combinations
{ L_,  5, 3,"a \r",                 10,"a=20=\r\n=0D"                        },
{ L_,  5, 3,"a \n",                 10,"a=20=\r\n=0A"                        },
{ L_,  5, 3,"a ""\xFE",              8,"a =\r\n=FE"                          },
{ L_,  5, 3,"a\r ",                 10,"a=0D=\r\n=20"                        },
{ L_,  5, 3,"a\n ",                 10,"a=0A=\r\n=20"                        },
{ L_,  5, 3,"a""\xFE"" ",           10,"a=FE=\r\n=20"                        },

{ L_,  5, 3,"a\r\n",                 3,"a\r\n"                               },
{ L_,  5, 3,"a\r""\xFE",            10,"a=0D=\r\n=FE"                        },
{ L_,  5, 3,"a\n\r",                10,"a=0A=\r\n=0D"                        },
{ L_,  5, 3,"a""\xFE""\r",          10,"a=FE=\r\n=0D"                        },

{ L_,  5, 3,"a\n""\xFE",            10,"a=0A=\r\n=FE"                        },
{ L_,  5, 3,"a""\xFE""\n",          10,"a=FE=\r\n=0A"                        },

{ L_,  5, 3," a\r",                  8," a=\r\n=0D"                          },
{ L_,  5, 3," a\n",                  8," a=\r\n=0A"                          },
{ L_,  5, 3," a""\xFE",              8," a=\r\n=FE"                          },
{ L_,  5, 3,"\ra ",                 10,"=0Da=\r\n=20"                        },
{ L_,  5, 3,"\na ",                 10,"=0Aa=\r\n=20"                        },
{ L_,  5, 3,"\xFE""a ",             10,"=FEa=\r\n=20"                        },

{ L_,  5, 3,"\ra\n",                10,"=0Da=\r\n=0A"                        },
{ L_,  5, 3,"\ra""\xFE",            10,"=0Da=\r\n=FE"                        },
{ L_,  5, 3,"\na\r",                10,"=0Aa=\r\n=0D"                        },
{ L_,  5, 3,"\xFE""a\r",            10,"=FEa=\r\n=0D"                        },

{ L_,  5, 3,"\na""\xFE",            10,"=0Aa=\r\n=FE"                        },
{ L_,  5, 3,"\xFE""a\n",            10,"=FEa=\r\n=0A"                        },

{ L_,  5, 3," \ra",                 10,"=20=\r\n=0Da"                        },
{ L_,  5, 3," \na",                 10,"=20=\r\n=0Aa"                        },
{ L_,  5, 3," \xFE""a",              8," =FE=\r\na"                          },
{ L_,  5, 3," \ra",                 10,"=20=\r\n=0Da"                        },
{ L_,  5, 3," \na",                 10,"=20=\r\n=0Aa"                        },
{ L_,  5, 3," \xFE""a",              8," =FE=\r\na"                          },

{ L_,  5, 3,"\r\na",                 3,"\r\na"                               },
{ L_,  5, 3,"\r""\xFE""a",          10,"=0D=\r\n=FEa"                        },
{ L_,  5, 3,"\n\ra",                10,"=0A=\r\n=0Da"                        },
{ L_,  5, 3,"\xFE""\ra",            10,"=FE=\r\n=0Da"                        },

{ L_,  5, 3,"\n""\xFE""a",          10,"=0A=\r\n=FEa"                        },
{ L_,  5, 3,"\xFE""\na",            10,"=FE=\r\n=0Aa"                        },

// 3 of different ECs involving no 2 of a kind, only 1 whitespace and no
// printable - 3 * 3 * 2 = 18 combinations
{ L_,  5, 3," \r\n",                 5,"=20\r\n"                             },
{ L_,  5, 3," \r""\xFE",            15,"=20=\r\n=0D=\r\n=FE"                 },
{ L_,  5, 3," \n\r",                15,"=20=\r\n=0A=\r\n=0D"                 },
{ L_,  5, 3," \xFE""\r",            10," =FE=\r\n=0D"                        },

{ L_,  5, 3," \n""\xFE",            15,"=20=\r\n=0A=\r\n=FE"                 },
{ L_,  5, 3," \xFE""\n",            10," =FE=\r\n=0A"                        },

{ L_,  5, 3,"\r \n",                15,"=0D=\r\n=20=\r\n=0A"                 },
{ L_,  5, 3,"\r ""\xFE",            10,"=0D =\r\n=FE"                        },
{ L_,  5, 3,"\n \r",                15,"=0A=\r\n=20=\r\n=0D"                 },
{ L_,  5, 3,"\xFE"" \r",            15,"=FE=\r\n=20=\r\n=0D"                 },

{ L_,  5, 3,"\n ""\xFE",            10,"=0A =\r\n=FE"                        },
{ L_,  5, 3,"\xFE"" \n",            15,"=FE=\r\n=20=\r\n=0A"                 },

{ L_,  5, 3,"\r\n ",                 5,"\r\n=20"                             },
{ L_,  5, 3,"\r""\xFE ",            15,"=0D=\r\n=FE=\r\n=20"                 },
{ L_,  5, 3,"\n\r ",                15,"=0A=\r\n=0D=\r\n=20"                 },
{ L_,  5, 3,"\xFE""\r ",            15,"=FE=\r\n=0D=\r\n=20"                 },

{ L_,  5, 3,"\n""\xFE ",            15,"=0A=\r\n=FE=\r\n=20"                 },
{ L_,  5, 3,"\xFE""\n ",            15,"=FE=\r\n=0A=\r\n=20"                 },

// 3 of different ECs involving no 2 of a kind, only 1 carriage return and no
// printable or whitespace - 3 * 2 * 1 = 6 combinations
{ L_,  5, 3,"\r\n""\xFE",            5,"\r\n=FE"                             },
{ L_,  5, 3,"\r\xFE""\n",           15,"=0D=\r\n=FE=\r\n=0A"                 },

{ L_,  5, 3,"\n\r""\xFE",           15,"=0A=\r\n=0D=\r\n=FE"                 },
{ L_,  5, 3,"\xFE""\r\n",            5,"=FE\r\n"                             },

{ L_,  5, 3,"\n\xFE""\r",           15,"=0A=\r\n=FE=\r\n=0D"                 },
{ L_,  5, 3,"\xFE""\n\r",           15,"=FE=\r\n=0A=\r\n=0D"                 },

// MAX LINE LENGTH = 4   INPUT LENGTH = 4
// 4 of the same ECs - 5 combinations
{ L_,  4, 4,"aaaa",                  7,"aaa=\r\na"                           },
{ L_,  4, 4,"    ",                  9,"   =\r\n=20"                         },
{ L_,  4, 4,"\r\r\r\r",             21,"=0D=\r\n=0D=\r\n=0D=\r\n=0D"         },
{ L_,  4, 4,"\n\n\n\n",             21,"=0A=\r\n=0A=\r\n=0A=\r\n=0A"         },
{ L_,  4, 4,"\xFE""\xFE""\xFE""\xFE",
                                    21,"=FE=\r\n=FE=\r\n=FE=\r\n=FE"         },

// Only consider the interesting cases.
{ L_,  4, 4,"   a",                  7,"   =\r\na"                           },
{ L_,  4, 4,"  a ",                  9,"  a=\r\n=20"                         },
{ L_,  4, 4," a  ",                  9," a =\r\n=20"                         },
{ L_,  4, 4,"a   ",                  9,"a  =\r\n=20"                         },

{ L_,  4, 4,"\r\n\r\n",              4,"\r\n\r\n"                            },
{ L_,  4, 4,"\r\naa",                4,"\r\naa"                              },
{ L_,  4, 4,"a\r\na",                4,"a\r\na"                              },
{ L_,  4, 4,"aa\r\n",                4,"aa\r\n"                              },
{ L_,  4, 4,"\ra\na",               17,"=0D=\r\na=\r\n=0A=\r\na"             },

{ L_,  0, 8,"aaaaaaaa",              8,"aaaaaaaa"                            },

// DEPTH 9
{ L_,  9, 0,"",                      0,""                                    },
{ L_,  8, 1,"a",                     1,"a"                                   },
{ L_,  8, 1," ",                     3,"=20"                                 },
{ L_,  8, 1,"\r",                    3,"=0D"                                 },
{ L_,  8, 1,"\n",                    3,"=0A"                                 },
{ L_,  8, 1,"\xFE",                  3,"=FE"                                 },

// MAX LINE LENGTH = 7   INPUT LENGTH = 2
// 2 of the same equivalence class (EC)
{ L_,  7, 2,"aa",                    2,"aa"                                  },
{ L_,  7, 2,"  ",                    4," =20"                                },
{ L_,  7, 2,"\r\r",                  6,"=0D=0D"                              },
{ L_,  7, 2,"\n\n",                  6,"=0A=0A"                              },
{ L_,  7, 2,"\xFE""\xFE",            6,"=FE=FE"                              },

// 2 of different ECs involving printable characters
{ L_,  7, 2,"a ",                    4,"a=20"                                },
{ L_,  7, 2," a",                    2," a"                                  },
{ L_,  7, 2,"a\r",                   4,"a=0D"                                },
{ L_,  7, 2,"\ra",                   4,"=0Da"                                },
{ L_,  7, 2,"a\n",                   4,"a=0A"                                },
{ L_,  7, 2,"\na",                   4,"=0Aa"                                },
{ L_,  7, 2,"a""\xFE",               4,"a=FE"                                },
{ L_,  7, 2,"\xFE""a",               4,"=FEa"                                },

// 2 of different ECs involving whitespace
{ L_,  7, 2," \r",                   6,"=20=0D"                              },
{ L_,  7, 2,"\r ",                   6,"=0D=20"                              },
{ L_,  7, 2," \n",                   6,"=20=0A"                              },
{ L_,  7, 2,"\n ",                   6,"=0A=20"                              },
{ L_,  7, 2," ""\xFE",               4," =FE"                                },
{ L_,  7, 2,"\xFE"" ",               6,"=FE=20"                              },

// 2 of different ECs involving carriage return
{ L_,  7, 2,"\r\n",                  2,"\r\n"                                },
{ L_,  7, 2,"\n\r",                  6,"=0A=0D"                              },
{ L_,  7, 2,"\r""\xFE",              6,"=0D=FE"                              },
{ L_,  7, 2,"\xFE""\r",              6,"=FE=0D"                              },

// 2 of different ECs involving newline
{ L_,  7, 2,"\n""\xFE",              6,"=0A=FE"                              },
{ L_,  7, 2,"\xFE""\n",              6,"=FE=0A"                              },

// MAX LINE LENGTH = 6   INPUT LENGTH = 3
// 3 of the same ECs - 5 combinations
{ L_,  6, 3,"aaa",                   3,"aaa"                                 },
{ L_,  6, 3,"   ",                   5,"  =20"                               },
{ L_,  6, 3,"\r\r\r",               15,"=0D=\r\n=0D=\r\n=0D"                 },
{ L_,  6, 3,"\n\n\n",               15,"=0A=\r\n=0A=\r\n=0A"                 },
{ L_,  6, 3,"\xFE""\xFE""\xFE",     15,"=FE=\r\n=FE=\r\n=FE"                 },

// 3 of different ECs involving 2 printable characters
// 3 * (5-1) = 12 combinations
{ L_,  6, 3,"aa ",                   5,"aa=20"                               },
{ L_,  6, 3," aa",                   3," aa"                                 },
{ L_,  6, 3,"a a",                   3,"a a"                                 },
{ L_,  6, 3,"aa\r",                  5,"aa=0D"                               },
{ L_,  6, 3,"\raa",                  5,"=0Daa"                               },
{ L_,  6, 3,"a\ra",                  5,"a=0Da"                               },
{ L_,  6, 3,"aa\n",                  5,"aa=0A"                               },
{ L_,  6, 3,"\naa",                  5,"=0Aaa"                               },
{ L_,  6, 3,"a\na",                  5,"a=0Aa"                               },
{ L_,  6, 3,"aa""\xFE",              5,"aa=FE"                               },
{ L_,  6, 3,"\xFE""aa",              5,"=FEaa"                               },
{ L_,  6, 3,"a""\xFE""a",            5,"a=FEa"                               },

// 3 of different ECs involving 2 whitespace characters
// 3 * (4 - 1) = 9 combinations
{ L_,  6, 3,"  \r",                 10," =20=\r\n=0D"                        },
{ L_,  6, 3,"\r  ",                 10,"=0D =\r\n=20"                        },
{ L_,  6, 3," \r ",                 15,"=20=\r\n=0D=\r\n=20"                 },
{ L_,  6, 3,"  \n",                 10," =20=\r\n=0A"                        },
{ L_,  6, 3,"\n  ",                 10,"=0A =\r\n=20"                        },
{ L_,  6, 3," \n ",                 15,"=20=\r\n=0A=\r\n=20"                 },
{ L_,  6, 3,"  ""\xFE",              5,"  =FE"                               },
{ L_,  6, 3,"\xFE""  ",             10,"=FE =\r\n=20"                        },
{ L_,  6, 3," ""\xFE"" ",           10," =FE=\r\n=20"                        },

// 3 of different ECs involving 2 carriage returns
// 3 * (3 - 1) = 6 combinations
{ L_,  6, 3,"\r\r\n",                5,"=0D\r\n"                             },
{ L_,  6, 3,"\n\r\r",               15,"=0A=\r\n=0D=\r\n=0D"                 },
{ L_,  6, 3,"\r\n\r",                5,"\r\n=0D"                             },
{ L_,  6, 3,"\r\r""\xFE",           15,"=0D=\r\n=0D=\r\n=FE"                 },
{ L_,  6, 3,"\xFE""\r\r",           15,"=FE=\r\n=0D=\r\n=0D"                 },
{ L_,  6, 3,"\r""\xFE""\r",         15,"=0D=\r\n=FE=\r\n=0D"                 },

// 3 of different ECs involving 2 newlines
// 3 * (2 - 1) = 3 combinations
{ L_,  6, 3,"\n\n""\xFE",           15,"=0A=\r\n=0A=\r\n=FE"                 },
{ L_,  6, 3,"\xFE""\n\n",           15,"=FE=\r\n=0A=\r\n=0A"                 },
{ L_,  6, 3,"\n""\xFE""\n",         15,"=0A=\r\n=FE=\r\n=0A"                 },

// 3 of different ECs involving no 2 of a kind and only 1 printable
// 3 * 4 * 3 = 36 combinations
{ L_,  6, 3,"a \r",                 10,"a=20=\r\n=0D"                        },
{ L_,  6, 3,"a \n",                 10,"a=20=\r\n=0A"                        },
{ L_,  6, 3,"a ""\xFE",              5,"a =FE"                               },
{ L_,  6, 3,"a\r ",                 10,"a=0D=\r\n=20"                        },
{ L_,  6, 3,"a\n ",                 10,"a=0A=\r\n=20"                        },
{ L_,  6, 3,"a""\xFE ",             10,"a=FE=\r\n=20"                        },

{ L_,  6, 3,"a\r\n",                 3,"a\r\n"                               },
{ L_,  6, 3,"a\r""\xFE",            10,"a=0D=\r\n=FE"                        },
{ L_,  6, 3,"a\n\r",                10,"a=0A=\r\n=0D"                        },
{ L_,  6, 3,"a""\xFE""\r",          10,"a=FE=\r\n=0D"                        },

{ L_,  6, 3,"a\n""\xFE",            10,"a=0A=\r\n=FE"                        },
{ L_,  6, 3,"a""\xFE""\n",          10,"a=FE=\r\n=0A"                        },

{ L_,  6, 3," a\r",                  5," a=0D"                               },
{ L_,  6, 3," a\n",                  5," a=0A"                               },
{ L_,  6, 3," a""\xFE",              5," a=FE"                               },
{ L_,  6, 3,"\ra ",                 10,"=0Da=\r\n=20"                        },
{ L_,  6, 3,"\na ",                 10,"=0Aa=\r\n=20"                        },
{ L_,  6, 3,"\xFE""a ",             10,"=FEa=\r\n=20"                        },

{ L_,  6, 3,"\ra\n",                10,"=0Da=\r\n=0A"                        },
{ L_,  6, 3,"\ra""\xFE",            10,"=0Da=\r\n=FE"                        },
{ L_,  6, 3,"\na\r",                10,"=0Aa=\r\n=0D"                        },
{ L_,  6, 3,"\xFE""a\r",            10,"=FEa=\r\n=0D"                        },

{ L_,  6, 3,"\na""\xFE",            10,"=0Aa=\r\n=FE"                        },
{ L_,  6, 3,"\xFE""a\n",            10,"=FEa=\r\n=0A"                        },

{ L_,  6, 3," \ra",                 10,"=20=\r\n=0Da"                        },
{ L_,  6, 3," \na",                 10,"=20=\r\n=0Aa"                        },
{ L_,  6, 3," ""\xFE""a",            5," =FEa"                               },
{ L_,  6, 3," \ra",                 10,"=20=\r\n=0Da"                        },
{ L_,  6, 3," \na",                 10,"=20=\r\n=0Aa"                        },
{ L_,  6, 3," ""\xFE""a",            5," =FEa"                               },

{ L_,  6, 3,"\r\na",                 3,"\r\na"                               },
{ L_,  6, 3,"\r""\xFE""a",          10,"=0D=\r\n=FEa"                        },
{ L_,  6, 3,"\n\ra",                10,"=0A=\r\n=0Da"                        },
{ L_,  6, 3,"\xFE""\ra",            10,"=FE=\r\n=0Da"                        },

{ L_,  6, 3,"\n""\xFE""a",          10,"=0A=\r\n=FEa"                        },
{ L_,  6, 3,"\xFE""\na",            10,"=FE=\r\n=0Aa"                        },

// 3 of different ECs involving no 2 of a kind, only 1 whitespace and no
// printable - 3 * 3 * 2 = 18 combinations
{ L_,  6, 3," \r\n",                 5,"=20\r\n"                             },
{ L_,  6, 3," \r""\xFE",            15,"=20=\r\n=0D=\r\n=FE"                 },
{ L_,  6, 3," \n\r",                15,"=20=\r\n=0A=\r\n=0D"                 },
{ L_,  6, 3," \xFE""\r",            10," =FE=\r\n=0D"                        },

{ L_,  6, 3," \n""\xFE",            15,"=20=\r\n=0A=\r\n=FE"                 },
{ L_,  6, 3," \xFE""\n",            10," =FE=\r\n=0A"                        },

{ L_,  6, 3,"\r \n",                15,"=0D=\r\n=20=\r\n=0A"                 },
{ L_,  6, 3,"\r ""\xFE",            10,"=0D =\r\n=FE"                        },
{ L_,  6, 3,"\n \r",                15,"=0A=\r\n=20=\r\n=0D"                 },
{ L_,  6, 3,"\xFE"" \r",            15,"=FE=\r\n=20=\r\n=0D"                 },

{ L_,  6, 3,"\n ""\xFE",            10,"=0A =\r\n=FE"                        },
{ L_,  6, 3,"\xFE"" \n",            15,"=FE=\r\n=20=\r\n=0A"                 },

{ L_,  6, 3,"\r\n ",                 5,"\r\n=20"                             },
{ L_,  6, 3,"\r""\xFE"" ",          15,"=0D=\r\n=FE=\r\n=20"                 },
{ L_,  6, 3,"\n\r ",                15,"=0A=\r\n=0D=\r\n=20"                 },
{ L_,  6, 3,"\xFE""\r ",            15,"=FE=\r\n=0D=\r\n=20"                 },

{ L_,  6, 3,"\n""\xFE"" ",          15,"=0A=\r\n=FE=\r\n=20"                 },
{ L_,  6, 3,"\xFE""\n ",            15,"=FE=\r\n=0A=\r\n=20"                 },

// 3 of different ECs involving no 2 of a kind, only 1 carriage return and no
// printable or whitespace - 3 * 2 * 1 = 6 combinations
{ L_,  6, 3,"\r\n""\xFE",            5,"\r\n=FE"                             },
{ L_,  6, 3,"\r""\xFE""\n",         15,"=0D=\r\n=FE=\r\n=0A"                 },

{ L_,  6, 3,"\n\r""\xFE",           15,"=0A=\r\n=0D=\r\n=FE"                 },
{ L_,  6, 3,"\xFE""\r\n",            5,"=FE\r\n"                             },

{ L_,  6, 3,"\n""\xFE""\r",         15,"=0A=\r\n=FE=\r\n=0D"                 },
{ L_,  6, 3,"\xFE""\n\r",           15,"=FE=\r\n=0A=\r\n=0D"                 },

// MAX LINE LENGTH = 5   INPUT LENGTH = 4
// 4 of the same ECs - 5 combinations
{ L_,  5, 4,"aaaa",                  4,"aaaa"                                },
{ L_,  5, 4,"    ",                  9,"   =\r\n=20"                         },
{ L_,  5, 4,"\r\r\r\r",             21,"=0D=\r\n=0D=\r\n=0D=\r\n=0D"         },
{ L_,  5, 4,"\n\n\n\n",             21,"=0A=\r\n=0A=\r\n=0A=\r\n=0A"         },
{ L_,  5, 4,"\xFE""\xFE""\xFE""\xFE",
                                    21,"=FE=\r\n=FE=\r\n=FE=\r\n=FE"         },

// Only consider the interesting cases.
{ L_,  5, 4,"   a",                  4,"   a"                                },
{ L_,  5, 4,"  a ",                  9,"  a=\r\n=20"                         },
{ L_,  5, 4," a  ",                  9," a =\r\n=20"                         },
{ L_,  5, 4,"a   ",                  9,"a  =\r\n=20"                         },

{ L_,  5, 4,"\r\n\r\n",              4,"\r\n\r\n"                            },
{ L_,  5, 4,"\r\naa",                4,"\r\naa"                              },
{ L_,  5, 4,"a\r\na",                4,"a\r\na"                              },
{ L_,  5, 4,"aa\r\n",                4,"aa\r\n"                              },
{ L_,  5, 4,"\ra\na",               11,"=0Da=\r\n=0Aa"                       },

// MAX LINE LENGTH = 4   INPUT LENGTH = 5
// 5 of the same ECs - 5 combinations
{ L_,  4, 5,"aaaaa",                 8,"aaa=\r\naa"                          },
{ L_,  4, 5,"     ",                13,"   =\r\n =\r\n=20"                   },
{ L_,  4, 5,"\r\r\r\r\r\r",         27,"=0D=\r\n=0D=\r\n=0D=\r\n=0D=\r\n=0D" },
{ L_,  4, 5,"\n\n\n\n\n",           27,"=0A=\r\n=0A=\r\n=0A=\r\n=0A=\r\n=0A" },
{ L_,  4, 5,"\xFE""\xFE""\xFE""\xFE""\xFE",
                                    27,"=FE=\r\n=FE=\r\n=FE=\r\n=FE=\r\n=FE" },
// Only consider the interesting cases.
{ L_,  4, 5,"    a",                 8,"   =\r\n a"                          },
{ L_,  4, 5,"   a ",                13,"   =\r\na=\r\n=20"                   },
{ L_,  4, 5,"  a  ",                13,"  a=\r\n =\r\n=20"                   },
{ L_,  4, 5," a   ",                13," a =\r\n =\r\n=20"                   },
{ L_,  4, 5,"a    ",                13,"a  =\r\n =\r\n=20"                   },

{ L_,  4, 5,"\r\n\r\na",             5,"\r\n\r\na"                           },
{ L_,  4, 5,"\r\naaa",               5,"\r\naaa"                             },
{ L_,  4, 5,"a\r\naa",               5,"a\r\naa"                             },
{ L_,  4, 5,"aa\r\na",               5,"aa\r\na"                             },
{ L_,  4, 5,"\ra\naa",              18,"=0D=\r\na=\r\n=0A=\r\naa"            },

{ L_,  0, 9,"aaaaaaaaa",             9,"aaaaaaaaa"                           },

// DEPTH 10
{ L_, 10, 0,"",                      0,""                                    },
{ L_,  9, 1,"a",                     1,"a"                                   },
{ L_,  9, 1," ",                     3,"=20"                                 },
{ L_,  9, 1,"\r",                    3,"=0D"                                 },
{ L_,  9, 1,"\n",                    3,"=0A"                                 },
{ L_,  9, 1,"\xFE",                  3,"=FE"                                 },

// MAX LINE LENGTH = 8   INPUT LENGTH = 2
// 2 of the same equivalence class (EC)
{ L_,  8, 2,"aa",                    2,"aa"                                  },
{ L_,  8, 2,"  ",                    4," =20"                                },
{ L_,  8, 2,"\r\r",                  6,"=0D=0D"                              },
{ L_,  8, 2,"\n\n",                  6,"=0A=0A"                              },
{ L_,  8, 2,"\xFE""\xFE",            6,"=FE=FE"                              },

// 2 of different ECs involving printable characters
{ L_,  8, 2,"a ",                    4,"a=20"                                },
{ L_,  8, 2," a",                    2," a"                                  },
{ L_,  8, 2,"a\r",                   4,"a=0D"                                },
{ L_,  8, 2,"\ra",                   4,"=0Da"                                },
{ L_,  8, 2,"a\n",                   4,"a=0A"                                },
{ L_,  8, 2,"\na",                   4,"=0Aa"                                },
{ L_,  8, 2,"a""\xFE",               4,"a=FE"                                },
{ L_,  8, 2,"\xFE""a",               4,"=FEa"                                },

// 2 of different ECs involving whitespace
{ L_,  8, 2," \r",                   6,"=20=0D"                              },
{ L_,  8, 2,"\r ",                   6,"=0D=20"                              },
{ L_,  8, 2," \n",                   6,"=20=0A"                              },
{ L_,  8, 2,"\n ",                   6,"=0A=20"                              },
{ L_,  8, 2," ""\xFE",               4," =FE"                                },
{ L_,  8, 2,"\xFE"" ",               6,"=FE=20"                              },

// 2 of different ECs involving carriage return
{ L_,  8, 2,"\r\n",                  2,"\r\n"                                },
{ L_,  8, 2,"\n\r",                  6,"=0A=0D"                              },
{ L_,  8, 2,"\r\xFE",                6,"=0D=FE"                              },
{ L_,  8, 2,"\xFE\r",                6,"=FE=0D"                              },

// 2 of different ECs involving newline
{ L_,  8, 2,"\n""\xFE",              6,"=0A=FE"                              },
{ L_,  8, 2,"\xFE""\n",              6,"=FE=0A"                              },

// MAX LINE LENGTH = 7   INPUT LENGTH = 3
// 3 of the same ECs - 5 combinations
{ L_,  7, 3,"aaa",                   3,"aaa"                                 },
{ L_,  7, 3,"   ",                   5,"  =20"                               },
{ L_,  7, 3,"\r\r\r",               12,"=0D=0D=\r\n=0D"                      },
{ L_,  7, 3,"\n\n\n",               12,"=0A=0A=\r\n=0A"                      },
{ L_,  7, 3,"\xFE""\xFE""\xFE",     12,"=FE=FE=\r\n=FE"                      },

// 3 of different ECs involving 2 printable characters
// 3 * (5-1) = 12 combinations
{ L_,  7, 3,"aa ",                   5,"aa=20"                               },
{ L_,  7, 3," aa",                   3," aa"                                 },
{ L_,  7, 3,"a a",                   3,"a a"                                 },
{ L_,  7, 3,"aa\r",                  5,"aa=0D"                               },
{ L_,  7, 3,"\raa",                  5,"=0Daa"                               },
{ L_,  7, 3,"a\ra",                  5,"a=0Da"                               },
{ L_,  7, 3,"aa\n",                  5,"aa=0A"                               },
{ L_,  7, 3,"\naa",                  5,"=0Aaa"                               },
{ L_,  7, 3,"a\na",                  5,"a=0Aa"                               },
{ L_,  7, 3,"aa""\xFE",              5,"aa=FE"                               },
{ L_,  7, 3,"\xFE""aa",              5,"=FEaa"                               },
{ L_,  7, 3,"a""\xFE""a",            5,"a=FEa"                               },

// 3 of different ECs involving 2 whitespace characters
// 3 * (4 - 1) = 9 combinations
{ L_,  7, 3,"  \r",                 10," =20=\r\n=0D"                        },
{ L_,  7, 3,"\r  ",                 10,"=0D =\r\n=20"                        },
{ L_,  7, 3," \r ",                 12,"=20=0D=\r\n=20"                      },
{ L_,  7, 3,"  \n",                 10," =20=\r\n=0A"                        },
{ L_,  7, 3,"\n  ",                 10,"=0A =\r\n=20"                        },
{ L_,  7, 3," \n ",                 12,"=20=0A=\r\n=20"                      },
{ L_,  7, 3,"  ""\xFE",              5,"  =FE"                               },
{ L_,  7, 3,"\xFE""  ",             10,"=FE =\r\n=20"                        },
{ L_,  7, 3," \xFE"" ",             10," =FE=\r\n=20"                        },

// 3 of different ECs involving 2 carriage returns
// 3 * (3 - 1) = 6 combinations
{ L_,  7, 3,"\r\r\n",                5,"=0D\r\n"                             },
{ L_,  7, 3,"\n\r\r",               12,"=0A=0D=\r\n=0D"                      },
{ L_,  7, 3,"\r\n\r",                5,"\r\n=0D"                             },
{ L_,  7, 3,"\r\r""\xFE",           12,"=0D=0D=\r\n=FE"                      },
{ L_,  7, 3,"\xFE""\r\r",           12,"=FE=0D=\r\n=0D"                      },
{ L_,  7, 3,"\r""\xFE""\r",         12,"=0D=FE=\r\n=0D"                      },

// 3 of different ECs involving 2 newlines
// 3 * (2 - 1) = 3 combinations
{ L_,  7, 3,"\n\n""\xFE",           12,"=0A=0A=\r\n=FE"                      },
{ L_,  7, 3,"\xFE""\n\n",           12,"=FE=0A=\r\n=0A"                      },
{ L_,  7, 3,"\n""\xFE""\n",         12,"=0A=FE=\r\n=0A"                      },

// 3 of different ECs involving no 2 of a kind and only 1 printable
// 3 * 4 * 3 = 36 combinations
{ L_,  7, 3,"a \r",                 10,"a=20=\r\n=0D"                        },
{ L_,  7, 3,"a \n",                 10,"a=20=\r\n=0A"                        },
{ L_,  7, 3,"a ""\xFE",              5,"a =FE"                               },
{ L_,  7, 3,"a\r ",                 10,"a=0D=\r\n=20"                        },
{ L_,  7, 3,"a\n ",                 10,"a=0A=\r\n=20"                        },
{ L_,  7, 3,"a""\xFE"" ",           10,"a=FE=\r\n=20"                        },

{ L_,  7, 3,"a\r\n",                 3,"a\r\n"                               },
{ L_,  7, 3,"a\r""\xFE",            10,"a=0D=\r\n=FE"                        },
{ L_,  7, 3,"a\n\r",                10,"a=0A=\r\n=0D"                        },
{ L_,  7, 3,"a""\xFE""\r",          10,"a=FE=\r\n=0D"                        },

{ L_,  7, 3,"a\n""\xFE",            10,"a=0A=\r\n=FE"                        },
{ L_,  7, 3,"a""\xFE""\n",          10,"a=FE=\r\n=0A"                        },

{ L_,  7, 3," a\r",                  5," a=0D"                               },
{ L_,  7, 3," a\n",                  5," a=0A"                               },
{ L_,  7, 3," a""\xFE",              5," a=FE"                               },
{ L_,  7, 3,"\ra ",                 10,"=0Da=\r\n=20"                        },
{ L_,  7, 3,"\na ",                 10,"=0Aa=\r\n=20"                        },
{ L_,  7, 3,"\xFE""a ",             10,"=FEa=\r\n=20"                        },

{ L_,  7, 3,"\ra\n",                10,"=0Da=\r\n=0A"                        },
{ L_,  7, 3,"\ra""\xFE",            10,"=0Da=\r\n=FE"                        },
{ L_,  7, 3,"\na\r",                10,"=0Aa=\r\n=0D"                        },
{ L_,  7, 3,"\xFE""a\r",            10,"=FEa=\r\n=0D"                        },

{ L_,  7, 3,"\na""\xFE",            10,"=0Aa=\r\n=FE"                        },
{ L_,  7, 3,"\xFE""a\n",            10,"=FEa=\r\n=0A"                        },

{ L_,  7, 3," \ra",                 10,"=20=0D=\r\na"                        },
{ L_,  7, 3," \na",                 10,"=20=0A=\r\na"                        },
{ L_,  7, 3," \xFE""a",              5," =FEa"                               },
{ L_,  7, 3," \ra",                 10,"=20=0D=\r\na"                        },
{ L_,  7, 3," \na",                 10,"=20=0A=\r\na"                        },
{ L_,  7, 3," \xFE""a",              5," =FEa"                               },

{ L_,  7, 3,"\r\na",                 3,"\r\na"                               },
{ L_,  7, 3,"\r\xFE""a",            10,"=0D=FE=\r\na"                        },
{ L_,  7, 3,"\n\ra",                10,"=0A=0D=\r\na"                        },
{ L_,  7, 3,"\xFE""\ra",            10,"=FE=0D=\r\na"                        },

{ L_,  7, 3,"\n""\xFE""a",          10,"=0A=FE=\r\na"                        },
{ L_,  7, 3,"\xFE""\na",            10,"=FE=0A=\r\na"                        },

// 3 of different ECs involving no 2 of a kind, only 1 whitespace and no
// printable - 3 * 3 * 2 = 18 combinations
{ L_,  7, 3," \r\n",                 5,"=20\r\n"                             },
{ L_,  7, 3," \r""\xFE",            12,"=20=0D=\r\n=FE"                      },
{ L_,  7, 3," \n\r",                12,"=20=0A=\r\n=0D"                      },
{ L_,  7, 3," ""\xFE""\r",          10," =FE=\r\n=0D"                        },

{ L_,  7, 3," \n""\xFE",            12,"=20=0A=\r\n=FE"                      },
{ L_,  7, 3," ""\xFE""\n",          10," =FE=\r\n=0A"                        },

{ L_,  7, 3,"\r \n",                12,"=0D=20=\r\n=0A"                      },
{ L_,  7, 3,"\r ""\xFE",            10,"=0D =\r\n=FE"                        },
{ L_,  7, 3,"\n \r",                12,"=0A=20=\r\n=0D"                      },
{ L_,  7, 3,"\xFE"" \r",            12,"=FE=20=\r\n=0D"                      },

{ L_,  7, 3,"\n ""\xFE",            10,"=0A =\r\n=FE"                        },
{ L_,  7, 3,"\xFE"" \n",            12,"=FE=20=\r\n=0A"                      },

{ L_,  7, 3,"\r\n ",                 5,"\r\n=20"                             },
{ L_,  7, 3,"\r""\xFE"" ",          12,"=0D=FE=\r\n=20"                      },
{ L_,  7, 3,"\n\r ",                12,"=0A=0D=\r\n=20"                      },
{ L_,  7, 3,"\xFE""\r ",            12,"=FE=0D=\r\n=20"                      },

{ L_,  7, 3,"\n\xFE"" ",            12,"=0A=FE=\r\n=20"                      },
{ L_,  7, 3,"\xFE""\n ",            12,"=FE=0A=\r\n=20"                      },

// 3 of different ECs involving no 2 of a kind, only 1 carriage return and no
// printable or whitespace - 3 * 2 * 1 = 6 combinations
{ L_,  7, 3,"\r\n""\xFE",            5,"\r\n=FE"                             },
{ L_,  7, 3,"\r""\xFE""\n",         12,"=0D=FE=\r\n=0A"                      },

{ L_,  7, 3,"\n\r""\xFE",           12,"=0A=0D=\r\n=FE"                      },
{ L_,  7, 3,"\xFE""\r\n",            5,"=FE\r\n"                             },

{ L_,  7, 3,"\n""\xFE""\r",         12,"=0A=FE=\r\n=0D"                      },
{ L_,  7, 3,"\xFE""\n\r",           12,"=FE=0A=\r\n=0D"                      },

// MAX LINE LENGTH = 6   INPUT LENGTH = 4
// 4 of the same ECs - 5 combinations
{ L_,  6, 4,"aaaa",                  4,"aaaa"                                },
{ L_,  6, 4,"    ",                  9,"   =\r\n=20"                         },
{ L_,  6, 4,"\r\r\r\r",              21,"=0D=\r\n=0D=\r\n=0D=\r\n=0D"        },
{ L_,  6, 4,"\n\n\n\n",              21,"=0A=\r\n=0A=\r\n=0A=\r\n=0A"        },
{ L_,  6, 4,"\xFE""\xFE""\xFE""\xFE",21,"=FE=\r\n=FE=\r\n=FE=\r\n=FE"        },

// Only consider the interesting cases.
{ L_,  6, 4,"   a",                  4,"   a"                                },
{ L_,  6, 4,"  a ",                  9,"  a=\r\n=20"                         },
{ L_,  6, 4," a  ",                  9," a =\r\n=20"                         },
{ L_,  6, 4,"a   ",                  9,"a  =\r\n=20"                         },

{ L_,  6, 4,"\r\n\r\n",              4,"\r\n\r\n"                            },
{ L_,  6, 4,"\r\naa",                4,"\r\naa"                              },
{ L_,  6, 4,"a\r\na",                4,"a\r\na"                              },
{ L_,  6, 4,"aa\r\n",                4,"aa\r\n"                              },
{ L_,  6, 4,"\ra\na",               11,"=0Da=\r\n=0Aa"                       },

// MAX LINE LENGTH = 5   INPUT LENGTH = 5
// 5 of the same ECs - 5 combinations
{ L_,  5, 5,"aaaaa",                 8,"aaaa=\r\na"                          },
{ L_,  5, 5,"     ",                10,"    =\r\n=20"                        },
{ L_,  5, 5,"\r\r\r\r\r",           27,"=0D=\r\n=0D=\r\n=0D=\r\n=0D=\r\n=0D" },
{ L_,  5, 5,"\n\n\n\n\n",           27,"=0A=\r\n=0A=\r\n=0A=\r\n=0A=\r\n=0A" },
{ L_,  5, 5,"\xFE""\xFE""\xFE""\xFE""\xFE",
                                    27,"=FE=\r\n=FE=\r\n=FE=\r\n=FE=\r\n=FE" },

// Only consider the interesting cases.
{ L_,  5, 5,"    a",                 8,"    =\r\na"                          },
{ L_,  5, 5,"   a ",                10,"   a=\r\n=20"                        },
{ L_,  5, 5,"  a  ",                10,"  a =\r\n=20"                        },
{ L_,  5, 5," a   ",                10," a  =\r\n=20"                        },
{ L_,  5, 5,"a    ",                10,"a   =\r\n=20"                        },

{ L_,  5, 5,"\r\n\r\na",             5,"\r\n\r\na"                           },
{ L_,  5, 5,"\r\naaa",               5,"\r\naaa"                             },
{ L_,  5, 5,"a\r\naa",               5,"a\r\naa"                             },
{ L_,  5, 5,"aa\r\na",               5,"aa\r\na"                             },
{ L_,  5, 5,"\ra\naa",              15,"=0Da=\r\n=0Aa=\r\na"                 },

// MAX LINE LENGTH = 4   INPUT LENGTH = 6
// 5 of the same ECs - 5 combinations
{ L_,  4, 6,"aaaaaa",                9,"aaa=\r\naaa"                         },
{ L_,  4, 6,"      ",               14,"   =\r\n  =\r\n=20"                  },
{ L_,  4, 6,"\r\r\r\r\r\r",         33,"=0D=\r\n=0D=\r\n=0D=\r\n=0D=\r\n=0D"
                                       "=\r\n=0D"                            },
{ L_,  4, 6,"\n\n\n\n\n\n",         33,"=0A=\r\n=0A=\r\n=0A=\r\n=0A=\r\n=0A"
                                       "=\r\n=0A"                            },
{ L_,  4, 6,"\xFE""\xFE""\xFE""\xFE""\xFE""\xFE",
                                    33,"=FE=\r\n=FE=\r\n=FE=\r\n=FE=\r\n=FE"
                                       "=\r\n=FE"                            },

// Only consider the interesting cases.
{ L_,  4, 6,"     a",                9,"   =\r\n  a"                         },
{ L_,  4, 6,"    a ",               14,"   =\r\n a=\r\n=20"                  },
{ L_,  4, 6,"   a  ",               14,"   =\r\na =\r\n=20"                  },
{ L_,  4, 6,"  a   ",               14,"  a=\r\n  =\r\n=20"                  },
{ L_,  4, 6," a    ",               14," a =\r\n  =\r\n=20"                  },
{ L_,  4, 6,"a     ",               14,"a  =\r\n  =\r\n=20"                  },

{ L_,  4, 6,"\r\n\r\n\r\n",          6,"\r\n\r\n\r\n"                        },
{ L_,  4, 6,"\r\n\r\naa",            6,"\r\n\r\naa"                          },
{ L_,  4, 6,"\r\naa\r\n",            6,"\r\naa\r\n"                          },
{ L_,  4, 6,"aa\r\n\r\n",            6,"aa\r\n\r\n"                          },
{ L_,  4, 6,"\r\naaaa",              9,"\r\naaa=\r\na"                       },
{ L_,  4, 6,"a\r\naaa",              6,"a\r\naaa"                            },
{ L_,  4, 6,"aa\r\naa",              6,"aa\r\naa"                            },
{ L_,  4, 6,"aaa\r\na",              6,"aaa\r\na"                            },
{ L_,  4, 6,"aaaa\r\n",              9,"aaa=\r\na\r\n"                       },
{ L_,  4, 6,"\ra\na\ra",            27,"=0D=\r\na=\r\n=0A=\r\na=\r\n=0D"
                                       "=\r\na"                              },
{ L_,  0, 10,"aaaaaaaaaa",          10,"aaaaaaaaaa"                          }

//--------------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;


            int depth = -1;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_lineNum;
                const int         MAX_LEN = DATA[ti].d_maxLineLength;
                const int         IN_LEN  = DATA[ti].d_inputLength;
                const char *const INPUT   = DATA[ti].d_input_p;
                const int         OUT_LEN = DATA[ti].d_outputLength;
                const char *const OUTPUT  = DATA[ti].d_output_p;

                const char *const B = INPUT;
                const char *const E = INPUT + IN_LEN;

                const int OUTPUT_BUFFER_SIZE = 100; // overrun will be detected
                const int TRAILING_OUTPUT_WINDOW = 30; // detect extra output

                Obj obj(Obj::CRLF_MODE, MAX_LEN);

                const int newDepth = IN_LEN + MAX_LEN;

                // The following partitions the table in verbose mode.
                if (newDepth < depth) {             // table entires "Part Two"
                    if (verbose) cout <<
                                    "\nVerifying Maximum Line Length." << endl;
                }
                if (depth != newDepth) {
                    if (verbose) cout << "\tDepth = " << newDepth << endl;
                    depth = newDepth;
                }
                if (veryVerbose) {
                    T_ T_ P_(ti)
                    P_(LINE) printCharN(cout, INPUT, IN_LEN) << endl;
                }

                // The first thing to do is to check expected output length.
                //const int CALC_LEN = Obj::encodedLength(IN_LEN, MAX_LEN);
                //LOOP3_ASSERT(LINE, OUT_LEN, CALC_LEN, OUT_LEN == CALC_LEN);

                // Define the output buffer and initialize it.
                char outputBuffer[OUTPUT_BUFFER_SIZE];
                memset(outputBuffer, '?', sizeof outputBuffer);

                char *b = outputBuffer;
                int nOut = -1;
                int nIn = -1;

                int res = obj.convert(b, &nOut, &nIn, B, E);
                if (B[nIn - 1] == ' '  ||
                    B[nIn - 1] == '\t' ||
                    B[nIn - 1] == '\r') {
                    LOOP_ASSERT(LINE, 3 == res);
                }
                else {
                    LOOP_ASSERT(LINE, 0 == res);
                }

                LOOP_ASSERT(LINE, IN_LEN == nIn);

                // Prepare to call 'endConvert'.
                int totalOut = nOut;
                b += nOut;
                LOOP_ASSERT(LINE, 0 == obj.endConvert(b, &nOut));
                totalOut += nOut;
                LOOP3_ASSERT(LINE, OUT_LEN, totalOut, OUT_LEN == totalOut);

                // Capture and verify internal output length.
                const int internalLen = obj.outputLength();
                LOOP2_ASSERT(LINE, internalLen, OUT_LEN == internalLen);

                // Confirm final state is DONE_STATE.
                LOOP_ASSERT(LINE, isState(&obj, DONE_STATE));

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
                // For each index in [0, IN_LEN], partition the input
                // into two sequences, apply these sequences, in turn, to a
                // newly created instance, and verify that the result is
                // identical to that of the original (unpartitioned) sequence.

                for (int index = 0; index <= IN_LEN; ++index) {
                    if (veryVeryVerbose) { T_ T_ T_ T_ P(index) }

                    Obj localObj(Obj::CRLF_MODE, MAX_LEN);
                    const char *const M = B + index;
                    char localBuf[sizeof outputBuffer];
                    memset(localBuf, '$', sizeof localBuf);
                    char *lb = localBuf;
                    int localNumIn;
                    int localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 1: ";
                        printCharN(cout, B, M - B) << endl;
                    }

                    int res1 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         B, M);

                    if (B[localNumIn - 1] == ' '  ||
                        B[localNumIn - 1] == '\t' ||
                        B[localNumIn - 1] == '\r') {
                        LOOP2_ASSERT(LINE, index, 3 == res1);
                    }
                    else {
                        LOOP2_ASSERT(LINE, index, 0 == res1);
                    }

                    // Prepare for second call to 'convert'.
                    int localTotalIn = localNumIn;
                    int localTotalOut = localNumOut;
                    lb += localNumOut;

                    if (veryVeryVeryVerbose) {
                        cout << "\t\t\t\t\t" << "Input 2: ";
                        printCharN(cout, M, E - M) << endl;
                    }

                    int res2 = localObj.convert(lb, &localNumOut, &localNumIn,
                                                                         M, E);

                    if (M[localNumIn - 1] == ' '  ||
                        M[localNumIn - 1] == '\t' ||
                        M[localNumIn - 1] == '\r') {
                        LOOP2_ASSERT(LINE, index, 3 == res2);
                    }
                    else {
                        LOOP2_ASSERT(LINE, index, 0 == res2);
                    }

                    // Prepare to call 'endConvert'.
                    localTotalIn  += localNumIn;
                    localTotalOut += localNumOut;
                    lb += localNumOut;

                    int res3 = localObj.endConvert(lb, &localNumOut);
                    LOOP2_ASSERT(LINE, index, 0 == res3);
                    localTotalOut += localNumOut;

                    // Compare internal output lengths.
                    const int localLen = localObj.outputLength();
            // -----^
            LOOP3_ASSERT(LINE, internalLen, localLen, internalLen == localLen);
            // -----v

                    // Confirm final state is DONE_STATE.
                    LOOP2_ASSERT(LINE, index, isState(&localObj, DONE_STATE));

                    // Verify total amount of input consumed is the same.
            // -----^
            LOOP4_ASSERT(LINE, index, nIn, localTotalIn, nIn == localTotalIn);
            // -----v

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

        } // end block

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Verify THE CONSTRUCTOR WITH 3 PARAMETERS AND INTERNAL TABLES
        //   Ensure that each internal table has the appropriate entries and
        //   that the constructor with 3 parameters can be used to specify
        //   which ASCII characters are to be encoded into the Quoted Printable
        //   representation (i.e., "=XX").
        //
        // Concerns:
        //   - That there is a typo in some internal table.
        //   - That the constructor does not change the internal opcode table
        //     properly
        //
        // Plan:
        //   Using a pseudo-piecewise-continuous implementation technique
        //   i.e., Loop-Based), provide an area test that will sample each of
        //   the table entires.  The goal is that if any table entry is bad,
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

        if (verbose) cout << endl
                          << "VERIFY INTERNAL TABLES" << endl
                          << "======================" << endl;

        V("\nVerify Encoding Table and Opcode Table (256 entires).\n");

        char input;
        const char *const B = &input, *const E = B + 1;
        char encode[4] = { 0, 0, 0, 0 };
        char hex[2] = { 0, 0 };
        int nOut;
        int nIn;

        V("Verify Printable Characters: Entries [33-60], [62-126].");
        {
            const EquivalenceClass &inputType = printable;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input = i;

                    VVV("Verify printable characters are printed and not \
encoded.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 1 == nOut);
                        LOOP_ASSERT(i, input == b[0]);
                        LOOP_ASSERT(i, -1    == b[1]);
                        LOOP_ASSERT(i, -1    == b[2]);
                        LOOP_ASSERT(i, -1    == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, input == b[0]);
                        LOOP_ASSERT(i, -1    == b[1]);
                        LOOP_ASSERT(i, -1    == b[2]);
                        LOOP_ASSERT(i, -1    == b[3]);
                    }

                    VVV("Verify if a single char '" << input <<
                        "' can be specified to be encoded");
                    {
                        encode[0] = i;
                        encode[1] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if char '" << input << "' can be specified");
                    VVV("among other chars to be encoded");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if preceding chararcters specified to be");
                    VVV("encoded are encoded.");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        input = 'a';
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if trailing chararcters specified to be");
                    VVV("encoded are encoded.");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        input = 'A';
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }
                }
            }
        }

        V("Verify Whitespace Characters: Entries [9, 32]");
        {
            const EquivalenceClass &inputType = whitespace;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input = i;

                    VVV("Verify delayed encoding of char '" <<
                        printHex(B, 1) << "'");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if a single char '" << printHex(B, 1) <<
                        "' can be specified to be encoded");
                    {
                        encode[0] = i;
                        encode[1] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if char '" << printHex(B, 1) << "' can be");
                    VVV("specified among other chars to be encoded");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if precediing chararcters specified to be");
                    VVV("encoded are encoded.");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        input = 'a';
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify if trailing chararcters specified to be");
                    VVV("encoded are encoded.");
                    {
                        encode[0] = 'a';
                        encode[1] = i;
                        encode[2] = 'A';
                        encode[3] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        input = 'A';
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
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
                    input = i;

                    VVV("Verify '\\r' is encoded in LF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::LF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify delayed encoding of a stand-alone '\\r'");
                    VVV("in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify delayed encoding of a stand-alone '\\r'");
                    VVV("in MIXED_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::MIXED_MODE, 0);
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    // Unlike whitespace characters which can be specified to
                    // be encoded, hard line breaks in Quoted-Printable
                    // encoding are to be printed as hard line breaks.  Hence,
                    // upon reception of a '\r' input, the encoder must wait
                    // for an '\n' input.  If the input character to follow is
                    // not a '\n', the encoder will encode '\r' anyway,
                    // obviating the need to specify '\r' encoding at
                    // construction.

                    VVV("Repeat all 3 tests above to verify there is no");
                    VVV("effect of specifying '\\n' to be encoded at");
                    VVV("construction.");

                    encode[0] = i;
                    encode[1] = 0;

                    VVV("Verify '\\r' is encoded in LF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::LF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify delayed encoding of a stand-alone '\\r'");
                    VVV("in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify delayed encoding of a stand-alone '\\r'");
                    VVV("in MIXED_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::MIXED_MODE, 0);
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    // The case of a '\r' followed immediately by a '\n'
                    // is verified when the '\n' test cases are verified.
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
                    input = i;

                    VVV("Verify a stand-alone '\\n' is encoded in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify \"\\r\\n\" are output as CRLF in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        input = '\r';
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                        LOOP_ASSERT(i, -1 == b[3]);
                        input = i;  // i.e., '\n'
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }

                    VVV("Verify '\\n' is output as CRLF in LF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::LF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }

                    VVV("Verify '\\n' is output as CRLF in MIXED_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::MIXED_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }

                    VVV("Repeat all 4 tests above to verify there is no");
                    VVV("effect of specifying '\\n' to be encoded at");
                    VVV("construction.");

                    encode[0] = i;
                    encode[1] = 0;

                    VVV("Verify a stand-alone '\\n' is encoded in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify \"\\r\\n\" are output as CRLF in CRLF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        input = '\r';
                        LOOP_ASSERT(i, 3 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, -1 == b[0]);
                        LOOP_ASSERT(i, -1 == b[1]);
                        LOOP_ASSERT(i, -1 == b[2]);
                        LOOP_ASSERT(i, -1 == b[3]);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }

                    VVV("Verify '\\n' is output as CRLF in LF_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::LF_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }

                    VVV("Verify '\\n' is output as CRLF in MIXED_MODE.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::MIXED_MODE, 0);
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 2 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '\r' == b[0]);
                        LOOP_ASSERT(i, '\n' == b[1]);
                        LOOP_ASSERT(i, -1   == b[2]);
                        LOOP_ASSERT(i, -1   == b[3]);
                    }
                }
            }
        }

        V("Verify Control Characters: Entries [0-8], [11-12], [14-31], [61],");
        V("[127-256]");
        {
            const EquivalenceClass &inputType = controlChar;
            for (int j = 0; j < inputType.d_ranges.size(); ++j) {
                for (int i = inputType.d_ranges[j].start;
                     i <= inputType.d_ranges[j].end;
                     ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    input = i;

                    VVV("Verify control characters are encoded.");
                    {
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(Obj::CRLF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }

                    VVV("Verify there is no effect of specifying character");
                    VVV("to be encoded at construction.");
                    {
                        encode[0] = i;
                        encode[1] = 0;
                        char b[4] = { -1, -1, -1, -1 };
                        Obj obj(encode, Obj::CRLF_MODE, 0);
                        input = i;
                        LOOP_ASSERT(i, 0 == obj.convert(b, &nOut, &nIn, B, E));
                        LOOP_ASSERT(i, 1 == nIn);
                        LOOP_ASSERT(i, 3 == nOut);
                        charToHex(hex, input, sizeof hex);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                        LOOP_ASSERT(i, 0 == obj.endConvert(b + nOut, &nOut));
                        LOOP_ASSERT(i, 0 == nOut);
                        LOOP_ASSERT(i, '='    == b[0]);
                        LOOP_ASSERT(i, hex[0] == b[1]);
                        LOOP_ASSERT(i, hex[1] == b[2]);
                        LOOP_ASSERT(i, -1     == b[3]);
                    }
                }
            }
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
                { L_,  INITIAL_STATE,  0,       INITIAL_STATE },
                { L_,  STATE_ZERO,     0,       STATE_ZERO    },
                { L_,  SAW_RETURN,     0,       SAW_RETURN    },
                { L_,  SAW_WHITE,      0,       SAW_WHITE     },
                { L_,  DONE_STATE,     0,       ERROR_STATE   },
                { L_,  ERROR_STATE,    0,       ERROR_STATE   },

                { L_,  INITIAL_STATE,  1,       STATE_ZERO    },
                { L_,  STATE_ZERO,     1,       STATE_ZERO    },
                { L_,  SAW_RETURN,     1,       STATE_ZERO    },
                { L_,  SAW_WHITE,      1,       STATE_ZERO    },
                { L_,  DONE_STATE,     1,       ERROR_STATE   },
                { L_,  ERROR_STATE,    1,       ERROR_STATE   },

                { L_,  INITIAL_STATE,  2,       STATE_ZERO    },
                { L_,  STATE_ZERO,     2,       STATE_ZERO    },
                { L_,  SAW_RETURN,     2,       STATE_ZERO    },
                { L_,  SAW_WHITE,      2,       STATE_ZERO    },
                { L_,  DONE_STATE,     2,       ERROR_STATE   },
                { L_,  ERROR_STATE,    2,       ERROR_STATE   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const char INPUT[] = "ABCDEF";
            char b[4];
            int nOut;
            int nIn;
            const char *const B = INPUT;

            int lastNumInputs = -1;

            // MAIN TEST-TABLE LOOP
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int START = DATA[ti].d_startState;
                const int COUNT = DATA[ti].d_numInputs;
                const int END   = DATA[ti].d_endState;
                const char *const E = B + COUNT;
                int RTN;

                Obj obj(Obj::CRLF_MODE, 0);

                if (lastNumInputs != COUNT) {
                    if (verbose) cout << '\t' << COUNT << " input character"
                                      << (1 == COUNT ? "." : "s.") << endl;
                    lastNumInputs = COUNT;
                }
                setState(&obj, START);

                if (ERROR_STATE == END) {
                    RTN = -1;
                }
                else if (SAW_RETURN == END  || SAW_WHITE  == END) {
                    RTN = 3;
                }
                else {
                    RTN = 0;
                }

                if (veryVerbose) cout << "\t\t" << STATE_NAMES[START] << endl;

                if (veryVeryVerbose) { cout
                    << "\t\t\tExpected end state: " << STATE_NAMES[END] << endl
                    << "\t\t\tExpected return status: " << RTN << endl;
                }

                LOOP_ASSERT(LINE, RTN == obj.convert(b, &nOut, &nIn, B, E));

                LOOP_ASSERT(LINE, isState(&obj, END));

                // Verify amount of input consumed: all or none.
                const bool VALID = START != DONE_STATE && START != ERROR_STATE;
                const int EXP_NUM_IN = VALID ? COUNT : 0;
                LOOP_ASSERT(LINE, EXP_NUM_IN == nIn);

            } // end for ti
        } // end block

      } break;
      case 4: {
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
                { L_,  STATE_ZERO,     DONE_STATE  },
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

                Obj obj(Obj::CRLF_MODE, 0);

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
        //   ~Obj();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SET-STATE, IS-STATE, RESET, AND BASIC ACCESSORS"
                          << endl
                          << "==============================================="
                          << endl;

        if (verbose) cout <<
                        "\nMake sure we can detect the initial state." << endl;
        {
            if (verbose) cout << "\tINITIAL_STATE." << endl;

            Obj obj(Obj::CRLF_MODE, 9);

            ASSERT(9 == obj.maxLineLength());
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\nVerify ::setState." << endl;
        {

            if (verbose) cout << "\tINITIAL_STATE." << endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, INITIAL_STATE);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(1 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
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
                ASSERT(-1 == b[3]);
                ASSERT(-1 == b[4]);
                ASSERT(-1 == b[5]);
                ASSERT(-1 == b[6]);
            }

            if (verbose) cout << "\tSTATE_ZERO." << endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, STATE_ZERO);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(1 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 <  obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
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
                ASSERT(-1 == b[3]);
                ASSERT(-1 == b[4]);
                ASSERT(-1 == b[5]);
                ASSERT(-1 == b[6]);
            }

            if (verbose) cout << "\tSAW_RETURN." < endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, SAW_RETURN);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(3 == numOut);
                ASSERT('=' == b[0]);
                ASSERT('0' == b[1]);
                ASSERT('D' == b[2]);
                ASSERT(-1  == b[3]);
                ASSERT(-1  == b[4]);
                ASSERT(-1  == b[5]);
                ASSERT(-1  == b[6]);
            }

            if (verbose) cout << "\tSAW_WHITE." << endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, SAW_WHITE);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // DONE_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(3 == obj.outputLength());
                ASSERT(0 == result);
                ASSERT(3 == numOut);
                ASSERT('=' == b[0]);
                ASSERT('2' == b[1]);
                ASSERT('0' == b[2]);
                ASSERT(-1  == b[3]);
                ASSERT(-1  == b[4]);
                ASSERT(-1  == b[5]);
                ASSERT(-1  == b[6]);
            }

            if (verbose) cout << "\tDONE_STATE." << endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, DONE_STATE);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(1 == obj.isAccepting());
                ASSERT(1 == obj.isDone());
                ASSERT(0 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // ERROR_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
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
                ASSERT(-1 == b[3]);
                ASSERT(-1 == b[4]);
                ASSERT(-1 == b[5]);
                ASSERT(-1 == b[6]);
            }

            if (verbose) cout << "\tERROR_STATE." << endl;
            {
                Obj obj(Obj::CRLF_MODE, 9);
                setState(&obj, ERROR_STATE);

                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
                ASSERT(0 == obj.isAccepting());
                ASSERT(0 == obj.isDone());
                ASSERT(1 == obj.isError());
                ASSERT(0 == obj.isInitialState());
                ASSERT(0 == obj.outputLength());

                char b[7] = { -1, -1, -1, -1, -1, -1, -1 };
                int numOut = -1;
                int result = obj.endConvert(b, &numOut);

                // ERROR_STATE
                ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
                ASSERT(9 == obj.maxLineLength());
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
                ASSERT(-1 == b[3]);
                ASSERT(-1 == b[4]);
                ASSERT(-1 == b[5]);
                ASSERT(-1 == b[6]);
            }
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

                    Obj obj(Obj::CRLF_MODE, 0);
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
        //   Note that the effect of extraCharsToEncode parameter is tested
        //   separately in test case 6 (when internal tables are verified).
        //
        // Plan:
        //   Create the object in all 3 LineBreakMode's and with various
        //   maxLineLengths at the boundaries, and verify using all of the (as
        //   yet untested) direct accessors.  After this test case, we can
        //   declare the accessors returning configuration state to be
        //   thoroughly tested.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Orthogonality
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlde::QuotedPrintableEncoder(LineBreakMode mode = CRLF_MODE,
        //                                int maxLineLen=DEFAULT_MAX_LINELEN);
        //   bdlde::QuotedPrintableEncoder(const char*   extraCharsToEncode,
        //                                LineBreakMode mode = CRLF_MODE,
        //                                int maxLineLen=DEFAULT_MAX_LINELEN);
        //   int maxLineLength() const;
        //   LineBreakMode lineBreakMode() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nFirst test the constructor with 2 parameters"
                          << endl;

        if (verbose) cout
            << "\nVerify the setting of maxLineLength with mode = CRLF_MODE."
            << endl;

        if (verbose) cout << "\tmaxLineLength = default" << endl;
        {
            Obj obj(Obj::CRLF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(76 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = -1" << endl;
        {
            Obj obj(Obj::CRLF_MODE, -1);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(76 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 0 (single line)" << endl;
        {
            Obj obj(Obj::CRLF_MODE, 0);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(INT_MAX == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 4 (the minimum)" << endl;
        {
            Obj obj(Obj::CRLF_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 5" << endl;
        {
            Obj obj(Obj::CRLF_MODE, 5);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(5 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = INT_MAX" << endl;
        {
            Obj obj(Obj::CRLF_MODE, INT_MAX);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(INT_MAX == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmode = LF_MODE" << endl;
        {
            Obj obj(Obj::LF_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::LF_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmode = MIXED_MODE" << endl;
        {
            Obj obj(Obj::MIXED_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::MIXED_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout
            << "\nNow repeat the test for the constructor with 3 parameters."
            << endl;

        if (verbose) cout
            << "\nVerify the setting of maxLineLength with mode = CRLF_MODE."
            << endl;

        char testVector[] = "ACdz03";

        if (verbose) cout << "\tmaxLineLength = default" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(76 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = -1" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE, -1);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(76 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 0 (single line)" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE, 0);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(INT_MAX == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 4 (the minimum)" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = 5" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE, 5);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(5 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmaxLineLength = INT_MAX" << endl;
        {
            Obj obj(testVector, Obj::CRLF_MODE, INT_MAX);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::CRLF_MODE == obj.lineBreakMode());
            ASSERT(INT_MAX == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmode = LF_MODE" << endl;
        {
            Obj obj(testVector, Obj::LF_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::LF_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

        if (verbose) cout << "\tmode = MIXED_MODE" << endl;
        {
            Obj obj(testVector, Obj::MIXED_MODE, 4);
            ASSERT(1 == obj.isAccepting());
            ASSERT(0 == obj.isDone());
            ASSERT(0 == obj.isError());
            ASSERT(1 == obj.isInitialState());
            ASSERT(Obj::MIXED_MODE == obj.lineBreakMode());
            ASSERT(4 == obj.maxLineLength());
            ASSERT(0 == obj.outputLength());
        }

      } break;
#endif
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

        if (veryVerbose) {
            P_(int('+')); P((void*)'+');
            P_(int('/')); P((void*)'/');
            P_(int('=')); P((void*)'=');
        }

        if (verbose) cout << "\nTry 'printCharN' test helper function." <<endl;
        {
            ostringstream out;

            const char in[] = "a" "\x00" "b" "\x07" "c" "\x08" "d" "\x0F"
                              "e" "\x10" "f" "\x80" "g" "\xFF";

            printCharN(out, in, sizeof in) << ends;

            const char EXP[] = "a<00>b<07>c<08>d<0F>e<10>f<80>g<FF><00>";

            if (veryVerbose) {
                cout << "\tRESULT = " << out.str() << endl;
                cout << "\tEXPECT = " << EXP << endl;
            }
            ASSERT(0 == strncmp(EXP, out.str().c_str(), sizeof EXP));
        }

        if (verbose) cout << "\nTry instantiating an encoder." << endl;
        {
            bdlde::QuotedPrintableEncoder encoder;

            if (veryVerbose) {
                T_ P(encoder.isAccepting());
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

            int numInChkPt[]  = {0, 6, 80, 110, 110};
            int numOutChkPt[] = {0, 6, 79, 31, 3};
            int outIdxChkPt[] = {0, 6, 85, 116, 119};

            //begin:   0      7       80          END
            //end:     7     80      110          INPUT
            const char*const input = "Hello, "
"this is a test using an input line of text containing a space at pos = 76"
                " (also having a \r\n linebreak) ";

            ASSERT(numOutChkPt[0] == numOut);
            ASSERT(numInChkPt[0] == numIn);
            ASSERT(outIdxChkPt[0] == outIdx);
            for (int i = numOutChkPt[0]; i < numOutChkPt[1]; ++i)
                ASSERT(0 == out[i]);

            if (verbose) cout << "\tEncode: \"Hello, \"" << endl;
            {
                const char *const begin = input + numInChkPt[0];
                const char *const   end = input + numInChkPt[1];
                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(numOutChkPt[1] == numOut);
            ASSERT(numInChkPt[1] - numInChkPt[0] == numIn);
            ASSERT(outIdxChkPt[1] == outIdx);
            ASSERT(0 == strcmp(out, "Hello,"));
            for (int i = numOutChkPt[1]; i < numOutChkPt[2]; ++i)
                ASSERT(0 == out[i]);

            if (verbose) cout << "\tEncode: " <<
"\"this is a test using an input line of text containing a space at pos = 76\""
                              << endl;
            {
                const char *const begin = input + numInChkPt[1];
                const char *const   end = input + numInChkPt[2];

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(numOutChkPt[2] == numOut);
            ASSERT(numInChkPt[2] - numInChkPt[1] == numIn);
            ASSERT(outIdxChkPt[2] == outIdx);
            ASSERT(0 == strcmp(out, "Hello, this is a test using an input \
line of text containing a space at pos=\r\n =3D 76"));
            for (int i = numOutChkPt[2]; i < numOutChkPt[3]; ++i)
                ASSERT(0 == out[i]);

            if (verbose) cout << "\tEncode: " <<
                             "\" (also having a \r\n linebreak) \"" << endl;
            {
                const char *const begin = input + numInChkPt[2];
                const char *const   end = input + numInChkPt[3];

                encoder.convert(out + outIdx, &numOut, &numIn, begin, end);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(numOutChkPt[3] == numOut);
            ASSERT(numInChkPt[3] - numInChkPt[2] == numIn);
            ASSERT(outIdxChkPt[3] == outIdx);
            ASSERT(0 == strcmp(out, "Hello, this is a test using an input \
line of text containing a space at pos=\r\n =3D 76\
 (also having a=20\r\n linebreak)"));
            for (int i = numOutChkPt[3]; i < numOutChkPt[4]; ++i)
                ASSERT(0 == out[i]);

            if (verbose) cout << "\tEnd of Input: " << endl;
            {
                numIn = 0;
                encoder.endConvert(out + outIdx, &numOut);
                outIdx += numOut;

                if (veryVerbose) {
                    T_ T_ P_(numIn) P_(numOut) P_(outIdx) cout << "output: \"";
                    printCharN(cout, out, outIdx) << '"' << endl;
                }
            }

            ASSERT(numOutChkPt[4] == numOut);
            ASSERT(outIdxChkPt[4] == outIdx);
            ASSERT(0 == strcmp(out, "Hello, this is a test using an input \
line of text containing a space at pos=\r\n =3D 76\
 (also having a=20\r\n linebreak)=20"));
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
