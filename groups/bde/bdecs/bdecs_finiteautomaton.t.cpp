// bdecs_finiteautomaton.t.cpp         -*-C++-*-

#include <bdecs_finiteautomaton.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <cstdlib>     // atoi()
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()
#include <new>         // placement syntax
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'append' and 'removeAll' methods to be used by the generator functions
// 'g' and 'gg'.  Note that each manipulator that performs memory allocation
// must be tested for exception neutrality via the 'bdema_testallocator'
// component.  Exception neutrality involving streaming is verified using
// 'bdex_testinstream' (and 'bdex_testoutstream').
//-----------------------------------------------------------------------------
// [ 2] bdecs_FiniteAutomaton(bdema_Allocator *ba = 0);
// [ 7] bdecs_FiniteAutomaton(const bdecs_FiniteAutomaton& original, *ba = 0);
// [ 2] ~bdecs_FiniteAutomaton();
// [ 9] bdecs_FiniteAutomaton& operator=(const bdecs_FiniteAutomaton& rhs);
// [15] bdecs_FiniteAutomaton& operator+=(const bdecs_FiniteAutomaton& rhs);
// [16] bdecs_FiniteAutomaton& operator|=(const bdecs_FiniteAutomaton& rhs);
// [11] void append(char accept);
// [13] void appendSet(const char *accept, int length);
// [12] void appendSequence(const char *accept, int length);
// [14] void assignKleeneStar();
// [14] bdecs_FiniteAutomaton kleeneStar();
// [ 2] void removeAll();
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [10] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [ 5] void print() const;
// [10] int maxSupportedVersion() const;
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream, int version) const;
//
// [10] bdex_InStream& operator>>(bdex_InStream&, Obj&);
// [10] bdex_OutStream& operator<<(bdex_OutStream&, const Obj&);
// [ 5] ostream& operator<<(ostream&, const Obj&);
// [15] Obj operator+(const Obj&, const Obj&);
// [16] Obj operator|(const Obj&, const Obj&);
//-----------------------------------------------------------------------------
// [ 4] bdecs_FiniteAutomaton_State();
//-----------------------------------------------------------------------------
// [ 4] bdecs_FiniteAutomatonAccumulator(const bdecs_FiniteAutomaton& fa);
// [ 4] ~bdecs_FiniteAutomatonAccumulator();
// [ 4] void transition(char data);
// [ 4] int isAccepting();
// [ 4] int isError();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void append(char accept);
//
// [ 3] int ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] Obj& gg(Obj* object, const char *spec);
// [ 8] Obj   g(const char *spec);

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdecs_FiniteAutomaton Obj;
typedef bdecs_FiniteAutomatonAccumulator ObjAcc;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int accepts(const Obj& fa, const char *s)
{
    ObjAcc faa(fa);

    while (*s) {
        faa.transition(*s);
        if (faa.isError()) { return 0; }
        ++s;
    }

    return faa.isAccepting();
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) double values to be
// added to the 'bdecs_FiniteAutomaton' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Add the value corresponding to A.
// "AA"         Add two values both corresponding to A.
// "ABC"        Add three values corresponding to A, B and C.
// "ABC~"       Add three values corresponding to A, B and C and then
//              remove all the elements (set set length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Add three values corresponding to A, B, and C; empty
//              the object; and add values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(bdecs_FiniteAutomaton *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'add' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
   return SUCCESS;
}

bdecs_FiniteAutomaton& gg(bdecs_FiniteAutomaton *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdecs_FiniteAutomaton g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdecs_FiniteAutomaton object((bdema_Allocator *)0);
    return gg(&object, spec);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 32: {
        {
            Obj gobble(&testAllocator);
            gobble.appendSet("ab", 2);
            gobble.assignKleeneStar();

            {
                if (verbose) cout << endl
                    << "Tests for '[ab]*[ab]'" << endl
                    << "=====================" << endl;

                Obj fa(&testAllocator);
                fa = gobble;

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(1 == accepts(fa, "a"));
                ASSERT(1 == accepts(fa, "b"));

                ASSERT(1 == accepts(fa, "aa"));
                ASSERT(1 == accepts(fa, "bb"));
                ASSERT(1 == accepts(fa, "ab"));
                ASSERT(1 == accepts(fa, "ba"));

                ASSERT(1 == accepts(fa, "aaa"));
                ASSERT(1 == accepts(fa, "aba"));
                ASSERT(1 == accepts(fa, "aab"));
                ASSERT(1 == accepts(fa, "abb"));
                ASSERT(1 == accepts(fa, "baa"));
                ASSERT(1 == accepts(fa, "bba"));
                ASSERT(1 == accepts(fa, "bab"));
                ASSERT(1 == accepts(fa, "bbb"));
            }
        }
      } break;
      case 31: {
        {
            Obj gobble(&testAllocator);
            gobble.appendSet("ab", 2);
            gobble.assignKleeneStar();

            {
                Obj fa(&testAllocator);
                fa = gobble;

                fa.appendSequence("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));

                ASSERT(0 == accepts(fa, "aa"));
                ASSERT(0 == accepts(fa, "bb"));
                ASSERT(1 == accepts(fa, "ab"));
                ASSERT(0 == accepts(fa, "ba"));

                ASSERT(0 == accepts(fa, "aaa"));
                ASSERT(0 == accepts(fa, "aba"));
                ASSERT(1 == accepts(fa, "aab"));
                ASSERT(0 == accepts(fa, "abb"));
                ASSERT(0 == accepts(fa, "baa"));
                ASSERT(0 == accepts(fa, "bba"));
                ASSERT(1 == accepts(fa, "bab"));
                ASSERT(0 == accepts(fa, "bbb"));

                ASSERT(0 == accepts(fa, "aaaa"));
                ASSERT(0 == accepts(fa, "aaba"));
                ASSERT(1 == accepts(fa, "aaab"));
                ASSERT(0 == accepts(fa, "aabb"));
                ASSERT(0 == accepts(fa, "abaa"));
                ASSERT(0 == accepts(fa, "abba"));
                ASSERT(1 == accepts(fa, "abab"));
                ASSERT(0 == accepts(fa, "abbb"));
                ASSERT(0 == accepts(fa, "baaa"));
                ASSERT(0 == accepts(fa, "baba"));
                ASSERT(1 == accepts(fa, "baab"));
                ASSERT(0 == accepts(fa, "babb"));
                ASSERT(0 == accepts(fa, "bbaa"));
                ASSERT(0 == accepts(fa, "bbba"));
                ASSERT(1 == accepts(fa, "bbab"));
                ASSERT(0 == accepts(fa, "bbbb"));
            }
        }
      } break;
      case 30: {
        {
            Obj gobble(&testAllocator);
            gobble.appendSet("ab", 2);
            gobble.assignKleeneStar();

            {
                Obj fa(&testAllocator);
                fa = gobble;

                fa.append('a');
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(1 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));

                ASSERT(1 == accepts(fa, "aa"));
                ASSERT(0 == accepts(fa, "bb"));
                ASSERT(0 == accepts(fa, "ab"));
                ASSERT(1 == accepts(fa, "ba"));

                ASSERT(1 == accepts(fa, "aaa"));
                ASSERT(1 == accepts(fa, "aba"));
                ASSERT(0 == accepts(fa, "aab"));
                ASSERT(0 == accepts(fa, "abb"));
                ASSERT(1 == accepts(fa, "baa"));
                ASSERT(1 == accepts(fa, "bba"));
                ASSERT(0 == accepts(fa, "bab"));
                ASSERT(0 == accepts(fa, "bbb"));
            }
        }
      } break;
      case 29: {
        {
            Obj gobble(&testAllocator);
            gobble.appendSet("abc", 3);
            gobble.assignKleeneStar();

            {
                if (verbose) cout << endl
                    << "Tests for '[abc]*[ab]'" << endl
                    << "======================" << endl;

                Obj fa(&testAllocator);
                fa = gobble;

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(1 == accepts(fa, "a"));
                ASSERT(1 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "aa"));
                ASSERT(1 == accepts(fa, "bb"));
                ASSERT(0 == accepts(fa, "cc"));
                ASSERT(1 == accepts(fa, "ab"));
                ASSERT(1 == accepts(fa, "ba"));
                ASSERT(0 == accepts(fa, "ac"));
                ASSERT(1 == accepts(fa, "ca"));
                ASSERT(0 == accepts(fa, "bc"));
                ASSERT(1 == accepts(fa, "cb"));

                ASSERT(1 == accepts(fa, "aaa"));
                ASSERT(1 == accepts(fa, "aba"));
                ASSERT(1 == accepts(fa, "aab"));
                ASSERT(1 == accepts(fa, "abb"));
                ASSERT(1 == accepts(fa, "aca"));
                ASSERT(0 == accepts(fa, "aac"));
                ASSERT(0 == accepts(fa, "acc"));
                ASSERT(0 == accepts(fa, "abc"));
                ASSERT(1 == accepts(fa, "acb"));
                ASSERT(1 == accepts(fa, "baa"));
                ASSERT(1 == accepts(fa, "bba"));
                ASSERT(1 == accepts(fa, "bab"));
                ASSERT(1 == accepts(fa, "bbb"));
                ASSERT(1 == accepts(fa, "bca"));
                ASSERT(0 == accepts(fa, "bac"));
                ASSERT(0 == accepts(fa, "bcc"));
                ASSERT(0 == accepts(fa, "bbc"));
                ASSERT(1 == accepts(fa, "bcb"));
                ASSERT(1 == accepts(fa, "caa"));
                ASSERT(1 == accepts(fa, "cba"));
                ASSERT(1 == accepts(fa, "cab"));
                ASSERT(1 == accepts(fa, "cbb"));
                ASSERT(1 == accepts(fa, "cca"));
                ASSERT(0 == accepts(fa, "cac"));
                ASSERT(0 == accepts(fa, "ccc"));
                ASSERT(0 == accepts(fa, "cbc"));
                ASSERT(1 == accepts(fa, "ccb"));
            }
        }
      } break;
      case 28: {
        {
            Obj gobble(&testAllocator);
            gobble.appendSet("abc", 3);
            gobble.assignKleeneStar();

            {
                if (verbose) cout << endl
                    << "Tests for '[ab]c'" << endl
                    << "=================" << endl;

                Obj fa(&testAllocator);

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                fa.append('c');
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "ac"));
                ASSERT(1 == accepts(fa, "bc"));
                ASSERT(0 == accepts(fa, "cc"));

                ASSERT(0 == accepts(fa, "acc"));
                ASSERT(0 == accepts(fa, "bcc"));
            }
            {
                if (verbose) cout << endl
                    << "Tests for '[abc]*[ab]c'" << endl
                    << "=======================" << endl;

                Obj fa(&testAllocator);
                fa = gobble;

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                fa.append('c');
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "ac"));
                ASSERT(1 == accepts(fa, "bc"));
                ASSERT(0 == accepts(fa, "cc"));

                ASSERT(1 == accepts(fa, "cac"));
                ASSERT(1 == accepts(fa, "cbc"));

                ASSERT(1 == accepts(fa, "cccac"));
                ASSERT(1 == accepts(fa, "cccbc"));

                ASSERT(0 == accepts(fa, "cccacc"));
                ASSERT(0 == accepts(fa, "cccbcc"));
                ASSERT(0 == accepts(fa, "cccaca"));
                ASSERT(0 == accepts(fa, "cccbcb"));
            }
            {
                if (verbose) cout << endl
                    << "Tests for '[ab]c[abc]*'" << endl
                    << "=======================" << endl;

                Obj fa(&testAllocator);

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                fa.append('c');
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                fa += gobble;
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "ac"));
                ASSERT(1 == accepts(fa, "bc"));
                ASSERT(0 == accepts(fa, "cc"));

                ASSERT(1 == accepts(fa, "acc"));
                ASSERT(1 == accepts(fa, "bcc"));
                ASSERT(1 == accepts(fa, "acb"));
                ASSERT(1 == accepts(fa, "bca"));
                ASSERT(0 == accepts(fa, "cac"));
                ASSERT(0 == accepts(fa, "cbc"));

                ASSERT(0 == accepts(fa, "ccac"));
                ASSERT(0 == accepts(fa, "ccbc"));

                ASSERT(1 == accepts(fa, "acccc"));
                ASSERT(1 == accepts(fa, "bcccc"));
                ASSERT(0 == accepts(fa, "cccac"));
                ASSERT(0 == accepts(fa, "cccbc"));
            }
            {
                if (verbose) cout << endl
                    << "Tests for '[abc]*[ab]c[abc]*'" << endl
                    << "=============================" << endl;

                Obj fa(&testAllocator);
                Obj left(&testAllocator);
                Obj right(&testAllocator);

                left.appendSet("abc", 3);
                if (veryVerbose) { cout << "Line: " << L_ << endl << left; }

                left.assignKleeneStar();
                if (veryVerbose) { cout << "Line: " << L_ << endl << left; }

                left.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << left; }

                right.append('c');
                if (veryVerbose) { cout << "Line: " << L_ << endl << right; }

                Obj gobble(&testAllocator);
                gobble.appendSet("abc", 3);
                gobble.assignKleeneStar();

                right += gobble;
                if (veryVerbose) { cout << "Line: " << L_ << endl << right; }

                left += right;
                if (veryVerbose) { cout << "Line: " << L_ << endl << left; }

                fa += left;
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "ac"));
                ASSERT(1 == accepts(fa, "bc"));
                ASSERT(0 == accepts(fa, "cc"));

                ASSERT(1 == accepts(fa, "acc"));
                ASSERT(1 == accepts(fa, "bcc"));
                ASSERT(1 == accepts(fa, "acb"));
                ASSERT(1 == accepts(fa, "bca"));
                ASSERT(1 == accepts(fa, "cac"));
                ASSERT(1 == accepts(fa, "cbc"));
                ASSERT(1 == accepts(fa, "aac"));
                ASSERT(1 == accepts(fa, "abc"));
                ASSERT(1 == accepts(fa, "bac"));
                ASSERT(1 == accepts(fa, "bbc"));
                ASSERT(1 == accepts(fa, "cac"));
                ASSERT(1 == accepts(fa, "cbc"));
                ASSERT(0 == accepts(fa, "cba"));

                ASSERT(1 == accepts(fa, "ccac"));
                ASSERT(1 == accepts(fa, "ccbc"));

                ASSERT(1 == accepts(fa, "cccac"));
                ASSERT(1 == accepts(fa, "cccbc"));
                ASSERT(1 == accepts(fa, "acccc"));
                ASSERT(1 == accepts(fa, "bcccc"));
                ASSERT(1 == accepts(fa, "cccac"));
                ASSERT(1 == accepts(fa, "cccbc"));
                ASSERT(1 == accepts(fa, "aaaac"));
                ASSERT(1 == accepts(fa, "aaccc"));
                ASSERT(1 == accepts(fa, "aaabc"));
                ASSERT(1 == accepts(fa, "abccc"));
                ASSERT(1 == accepts(fa, "caccc"));
                ASSERT(1 == accepts(fa, "cbccc"));

                ASSERT(1 == accepts(fa, "aaaaccc"));
                ASSERT(1 == accepts(fa, "aaabccc"));
                ASSERT(1 == accepts(fa, "aacaccc"));
                ASSERT(1 == accepts(fa, "aacbccc"));
            }
            {
                if (verbose) cout << endl
                    << "Tests for 'c[ab]'" << endl
                    << "=================" << endl;

                Obj fa(&testAllocator);

                fa.append('c');
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                fa.appendSet("ab", 2);
                if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

                ASSERT(0 == accepts(fa, "a"));
                ASSERT(0 == accepts(fa, "b"));
                ASSERT(0 == accepts(fa, "c"));

                ASSERT(1 == accepts(fa, "ca"));
                ASSERT(1 == accepts(fa, "cb"));
                ASSERT(0 == accepts(fa, "cc"));

                ASSERT(0 == accepts(fa, "acc"));
                ASSERT(0 == accepts(fa, "bcc"));
            }
        }
      } break;
      case 27: {
        {
            Obj fa(&testAllocator);

            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            fa.assignKleeneStar();
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            fa.append('b');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            {
                bdecs_FiniteAutomatonAccumulator a(fa);
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
            }
        }
      } break;
      case 26: {
        {
            Obj fa(&testAllocator);

            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            fa.assignKleeneStar();
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            {
                bdecs_FiniteAutomatonAccumulator a(fa);
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
            }
        }
      } break;
      case 25: {
        {
            Obj fa(&testAllocator);

            char set[256];
            for (int i = 0; i < 256; ++i) {
                set[i] = i;
            }
            fa.appendSet(set, 256);
            fa.assignKleeneStar();
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            Obj gobble(&testAllocator);
            gobble.appendSet(set, 256);
            gobble.assignKleeneStar();
            fa += gobble;
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

            {
                bdecs_FiniteAutomatonAccumulator a(fa);
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
            }

            {
                bdecs_FiniteAutomatonAccumulator a(fa);
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
        }
      } break;
      case 24: {
        {
            Obj fa(&testAllocator);
            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            fa.assignKleeneStar();
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        }
        {
            Obj fa(&testAllocator);
            fa.appendSequence("aa", 2);
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            fa.assignKleeneStar();
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
            fa.append('a');
            if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        {
            bdecs_FiniteAutomaton f(&testAllocator);
            {
                bdecs_FiniteAutomatonAccumulator a(f);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }

            f.appendSequence("abc", 3);
            {
                bdecs_FiniteAutomatonAccumulator a(f);
                ASSERT(0 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                a.transition('b');
                a.transition('c');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }

            f.assignKleeneStar();
            {
                bdecs_FiniteAutomatonAccumulator a(f);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                a.transition('b');
                a.transition('c');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                a.transition('b');
                a.transition('c');
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
        }
      } break;
      case 22: {
        Obj fa(&testAllocator);
        fa.appendSequence("ab", 2);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.append('a');
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
      } break;
      case 21: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abcabc", 6);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        Obj fc(&testAllocator);
        fc.appendSequence("abcabab", 7);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fc; }
        fb |= fc;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
      } break;
      case 20: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
      } break;
      case 19: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa += fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
      } break;
      case 18: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        fa.removeAll();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(0 == a.isAccepting());
        }

      } break;
      case 17: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(&testAllocator);
        fa = fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            for (int i = 0; i < 10; ++i) {
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isAccepting());
            }
        }

      } break;
      case 16: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(fb, &testAllocator);

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            for (int i = 0; i < 10; ++i) {
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isAccepting());
            }
        }

      } break;
      case 15: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(&testAllocator);
        fa.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }


        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int i = 0; i < 10; ++i) {
                ObjAcc a(fa);
                for (int j = 0; j < i; ++j) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('c');
                    ASSERT(1 == a.isAccepting());
                }
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT((0 == i) == a.isAccepting());
            }
        }

        {
            for (int i = 0; i < 10; ++i) {
                ObjAcc a(fa);
                for (int j = 0; j < i; ++j) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(1 == a.isAccepting());
                }
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('c');
                ASSERT((0 == i) == a.isAccepting());
            }
        }

      } break;
      case 14: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int i = 0; i < 10; ++i) {
                ObjAcc a(fa);
                for (int j = 0; j < i; ++j) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('c');
                    ASSERT(1 == a.isAccepting());
                }
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT((0 == i) == a.isAccepting());
            }
        }

        {
            for (int i = 0; i < 10; ++i) {
                ObjAcc a(fa);
                for (int j = 0; j < i; ++j) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(1 == a.isAccepting());
                }
                ASSERT(1 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('c');
                ASSERT((0 == i) == a.isAccepting());
            }
        }

      } break;
      case 13: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(&testAllocator);
        fa.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(1 == a.isAccepting());
        }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(1 == a.isAccepting());
        }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
        }

      } break;
      case 12: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(1 == a.isAccepting());
        }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(1 == a.isAccepting());
        }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(1 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
        }

      } break;
      case 11: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa |= fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('c');
            ASSERT(1 == a.isAccepting());
        }

        {
            ObjAcc a(fa);
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(1 == a.isAccepting());
        }

      } break;
      case 10: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(&testAllocator);
        fa.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        fa += fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int k = 0; k < 10; ++k) {
                for (int j = 0; j < 10; ++j) {
                    ObjAcc a(fa);
                    for (int i = 0; i < j; ++i) {
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(1 == a.isAccepting());
                    }
                    for (int i = 0; i < k; ++i) {
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                        a.transition('c');
                        ASSERT(1 == a.isAccepting());
                    }
                    ASSERT(1 == a.isAccepting());
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT((0 == k) == a.isAccepting());
                }
            }
        }

      } break;
      case 9: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa += fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int k = 0; k < 10; ++k) {
                for (int j = 0; j < 10; ++j) {
                    ObjAcc a(fa);
                    for (int i = 0; i < j; ++i) {
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                        a.transition('c');
                        ASSERT(1 == a.isAccepting());
                    }
                    for (int i = 0; i < k; ++i) {
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(1 == a.isAccepting());
                    }
                    ASSERT(1 == a.isAccepting());
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('c');
                    ASSERT((0 == k) == a.isAccepting());
                }
            }
        }

      } break;
      case 8: {
        Obj fb(&testAllocator);
        fb.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }
        fb.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        Obj fa(&testAllocator);
        fa.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        fa += fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int j = 0; j < 10; ++j) {
                ObjAcc a(fa);
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(1 == a.isAccepting());
                for (int i = 0; i < j; ++i) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('c');
                    ASSERT(1 == a.isAccepting());
                }
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
            }
        }

      } break;
      case 7: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }
        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        Obj fb(&testAllocator);
        fb.appendSequence("abab", 4);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fb; }

        fa += fb;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int j = 0; j < 10; ++j) {
                ObjAcc a(fa);
                ASSERT(0 == a.isAccepting());
                for (int i = 0; i < j; ++i) {
                    a.transition('a');
                    ASSERT(0 == a.isAccepting());
                    a.transition('b');
                    ASSERT(0 == a.isAccepting());
                    a.transition('c');
                    ASSERT(0 == a.isAccepting());
                }
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(1 == a.isAccepting());
            }
        }

        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int l = 0; l < 10; ++l) {
                ObjAcc a(fa);
                for (int k = 0; k < l; ++k) {
                    for (int j = 0; j < 10; ++j) {
                        for (int i = 0; i < j; ++i) {
                          a.transition('a');
                          ASSERT(0 == a.isAccepting());
                          a.transition('b');
                          ASSERT(0 == a.isAccepting());
                          a.transition('c');
                          ASSERT(0 == a.isAccepting());
                        }
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(1 == a.isAccepting());
                    }
                }
                ASSERT(1 == a.isAccepting());
            }
        }

        Obj fc(&testAllocator);
        fc.appendSequence("ab", 2);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fc; }

        fa += fc;
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            for (int l = 0; l < 10; ++l) {
                ObjAcc a(fa);
                for (int k = 0; k < l; ++k) {
                    for (int j = 0; j < 10; ++j) {
                        for (int i = 0; i < j; ++i) {
                          a.transition('a');
                          ASSERT(0 == a.isAccepting());
                          a.transition('b');
                          ASSERT((i == 0) == a.isAccepting());
                          a.transition('c');
                          ASSERT(0 == a.isAccepting());
                        }
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT((j == 0) == a.isAccepting());
                        a.transition('a');
                        ASSERT(0 == a.isAccepting());
                        a.transition('b');
                        ASSERT(0 == a.isAccepting());
                    }
                }
                ASSERT(0 == a.isAccepting());
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(1 == a.isAccepting());
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING STREAMING
        // TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing streaming" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting 'streaming'." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str1;     // first sequence source string
                const char *d_str2;     // first set source string
                const char *d_str3;     // second sequence source string
                const char *d_str4;     // second set source string
            } DATA[] = {
                //line    str1      str2      str3      str4
                //----  --------  --------  --------  --------
                { L_,   "",       "",       "",       ""     },
                { L_,   "a",      "a",      "",       ""     },
                { L_,   "a",      "ab",     "a",      ""     },
                { L_,   "a",      "ab",     "a",      "ab"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int ks = 0; ks < 2; ++ks) {
                    // const int   LINE = DATA[ti].d_lineNum;
                    const char *STR1 = DATA[ti].d_str1;
                    const int   LEN1 = (int)strlen(STR1);
                    const char *STR2 = DATA[ti].d_str2;
                    const int   LEN2 = (int)strlen(STR2);
                    const char *STR3 = DATA[ti].d_str3;
                    const int   LEN3 = (int)strlen(STR3);
                    const char *STR4 = DATA[ti].d_str4;
                    const int   LEN4 = (int)strlen(STR4);
                    const int   LEN  = LEN1 + !!LEN2 + LEN3 + !!LEN4;

                    const int   LEN123 = LEN1 + !!LEN2 + LEN3;
                    const int   LEN12  = LEN1 + !!LEN2;

                    Obj mY(&testAllocator);  const Obj& Y = mY;
                    mY.removeAll();
                    mY.appendSequence(STR1, LEN1);
                    mY.appendSet(STR2, LEN2);
                    mY.appendSequence(STR3, LEN3);
                    mY.appendSet(STR4, LEN4);
                    if (ks) {
                        mY.assignKleeneStar();
                    }
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    BEGIN_BDEMA_EXCEPTION_TEST {
                        mX.removeAll();

                        bdex_TestOutStream out;  out << Y;

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdex_TestInStream testInStream(OD, LOD);
                        ASSERT(testInStream);  ASSERT(!testInStream.isEmpty());

                        BEGIN_BDEX_EXCEPTION_TEST {
                          testInStream.reset();
                          testInStream >> mX;
                        } END_BDEX_EXCEPTION_TEST
                    } END_BDEMA_EXCEPTION_TEST

                    if (verbose) {
                        cout << "\t";
                        cout << "\t"; P_(STR1);
                        cout << "\t"; P_(STR2);
                        cout << "\t"; P_(STR3);
                        cout << "\t"; P(STR4);
                    }

                    if (0 == LEN) {
                        for (int value = 0; value < 256; ++value) {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(1 == a.isAccepting());
                            a.transition(value);
                            ASSERT(1 == a.isError());
                            ASSERT(0 == a.isAccepting());
                        }
                    }
                    else {
                        {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(ks == a.isAccepting());
                        }
                        for (int iter = 0; iter < 9 && (!iter || ks); ++iter) {
                            for (int i = 0; i < LEN; ++i) {
                                for (int value = 0; value < 256; ++value) {
                                    ObjAcc a(X);
                                    ASSERT(0 == a.isError());
                                    ASSERT(ks == a.isAccepting());
                                    for (int k = 0; k < iter; ++k) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        {
                                            for (int j = 0; j < LEN3; ++j) {
                                                a.transition(STR3[j]);
                                            }
                                        }
                                        if (LEN4) {
                                            a.transition(STR4[0]);
                                        }
                                        ASSERT(0 == a.isError());
                                        ASSERT(1 == a.isAccepting());
                                    }
                                    if (i < LEN1) {
                                        for (int j = 0; j < i; ++j) {
                                            a.transition(STR1[j]);
                                            ASSERT(0 == a.isError());
                                            ASSERT(0 == a.isAccepting());
                                        }
                                        a.transition(value);
                                        const int E = (unsigned char)STR1[i]
                                                                      != value;
                                        const int A = !E && (LEN - 1 == i);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else if (i < LEN12) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        int E = 1;
                                        for (int j = 0; j < LEN2; ++j) {
                                            if ((unsigned char)STR2[j] ==
                                                                       value) {
                                                E = 0;
                                            }
                                        }
                                        const int A = !E && (LEN - 1 == i);
                                        a.transition(value);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else if (i < LEN123) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        for (int j = LEN12; j < i; ++j) {
                                            a.transition(STR3[j - LEN12]);
                                            ASSERT(0 == a.isError());
                                            ASSERT(0 == a.isAccepting());
                                        }
                                        a.transition(value);
                                        const int E = (unsigned char)
                                                      STR3[i - LEN12] != value;
                                        const int A = !E && (LEN - 1 == i);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        {
                                            for (int j = 0; j < LEN3; ++j) {
                                                a.transition(STR3[j]);
                                            }
                                        }
                                        int E = 1;
                                        for (int j = 0; j < LEN4; ++j) {
                                            if ((unsigned char)STR4[j] ==
                                                                       value) {
                                                E = 0;
                                            }
                                        }
                                        const int A = !E && (LEN - 1 == i);
                                        a.transition(value);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR=
        // TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing operator=" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting 'operator='." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str1;     // first sequence source string
                const char *d_str2;     // first set source string
                const char *d_str3;     // second sequence source string
                const char *d_str4;     // second set source string
            } DATA[] = {
                //line    str1      str2      str3      str4
                //----  --------  --------  --------  --------
                { L_,   "",       "",       "",       ""     },
                { L_,   "a",      "a",      "",       ""     },
                { L_,   "a",      "ab",     "a",      ""     },
                { L_,   "a",      "ab",     "a",      "ab"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int ks = 0; ks < 2; ++ks) {
                    // const int   LINE = DATA[ti].d_lineNum;
                    const char *STR1 = DATA[ti].d_str1;
                    const int   LEN1 = (int)strlen(STR1);
                    const char *STR2 = DATA[ti].d_str2;
                    const int   LEN2 = (int)strlen(STR2);
                    const char *STR3 = DATA[ti].d_str3;
                    const int   LEN3 = (int)strlen(STR3);
                    const char *STR4 = DATA[ti].d_str4;
                    const int   LEN4 = (int)strlen(STR4);
                    const int   LEN  = LEN1 + !!LEN2 + LEN3 + !!LEN4;

                    const int   LEN123 = LEN1 + !!LEN2 + LEN3;
                    const int   LEN12  = LEN1 + !!LEN2;

                    Obj mY(&testAllocator);  const Obj& Y = mY;
                    mY.removeAll();
                    mY.appendSequence(STR1, LEN1);
                    mY.appendSet(STR2, LEN2);
                    mY.appendSequence(STR3, LEN3);
                    mY.appendSet(STR4, LEN4);
                    if (ks) {
                        mY.assignKleeneStar();
                    }
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    BEGIN_BDEMA_EXCEPTION_TEST {
                        mX.removeAll();
                        mX = Y;
                    } END_BDEMA_EXCEPTION_TEST

                    if (verbose) {
                        cout << "\t";
                        cout << "\t"; P_(STR1);
                        cout << "\t"; P_(STR2);
                        cout << "\t"; P_(STR3);
                        cout << "\t"; P(STR4);
                    }

                    if (0 == LEN) {
                        for (int value = 0; value < 256; ++value) {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(1 == a.isAccepting());
                            a.transition(value);
                            ASSERT(1 == a.isError());
                            ASSERT(0 == a.isAccepting());
                        }
                    }
                    else {
                        {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(ks == a.isAccepting());
                        }
                        for (int iter = 0; iter < 9 && (!iter || ks); ++iter) {
                            for (int i = 0; i < LEN; ++i) {
                                for (int value = 0; value < 256; ++value) {
                                    ObjAcc a(X);
                                    ASSERT(0 == a.isError());
                                    ASSERT(ks == a.isAccepting());
                                    for (int k = 0; k < iter; ++k) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        {
                                            for (int j = 0; j < LEN3; ++j) {
                                                a.transition(STR3[j]);
                                            }
                                        }
                                        if (LEN4) {
                                            a.transition(STR4[0]);
                                        }
                                        ASSERT(0 == a.isError());
                                        ASSERT(1 == a.isAccepting());
                                    }
                                    if (i < LEN1) {
                                        for (int j = 0; j < i; ++j) {
                                            a.transition(STR1[j]);
                                            ASSERT(0 == a.isError());
                                            ASSERT(0 == a.isAccepting());
                                        }
                                        a.transition(value);
                                        const int E = (unsigned char)STR1[i]
                                                                      != value;
                                        const int A = !E && (LEN - 1 == i);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else if (i < LEN12) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        int E = 1;
                                        for (int j = 0; j < LEN2; ++j) {
                                            if ((unsigned char)STR2[j] ==
                                                                       value) {
                                                E = 0;
                                            }
                                        }
                                        const int A = !E && (LEN - 1 == i);
                                        a.transition(value);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else if (i < LEN123) {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        for (int j = LEN12; j < i; ++j) {
                                            a.transition(STR3[j - LEN12]);
                                            ASSERT(0 == a.isError());
                                            ASSERT(0 == a.isAccepting());
                                        }
                                        a.transition(value);
                                        const int E = (unsigned char)
                                                      STR3[i - LEN12] != value;
                                        const int A = !E && (LEN - 1 == i);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                    else {
                                        {
                                            for (int j = 0; j < LEN1; ++j) {
                                                a.transition(STR1[j]);
                                            }
                                        }
                                        if (LEN2) {
                                            a.transition(STR2[0]);
                                        }
                                        {
                                            for (int j = 0; j < LEN3; ++j) {
                                                a.transition(STR3[j]);
                                            }
                                        }
                                        int E = 1;
                                        for (int j = 0; j < LEN4; ++j) {
                                            if ((unsigned char)STR4[j] ==
                                                                       value) {
                                                E = 0;
                                            }
                                        }
                                        const int A = !E && (LEN - 1 == i);
                                        a.transition(value);
                                        ASSERT(E == a.isError());
                                        ASSERT(A == a.isAccepting());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNKLEENESTAR
        // TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing assignKleeneStar" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting 'assignKleeneStar'." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str1;     // first sequence source string
                const char *d_str2;     // first set source string
                const char *d_str3;     // second sequence source string
                const char *d_str4;     // second set source string
            } DATA[] = {
                //line    str1      str2      str3      str4
                //----  --------  --------  --------  --------
                { L_,   "",       "",       "",       ""     },
                { L_,   "a",      "a",      "",       ""     },
                { L_,   "a",      "ab",     "a",      ""     },
                { L_,   "a",      "ab",     "a",      "ab"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                // const int   LINE = DATA[ti].d_lineNum;
                const char *STR1 = DATA[ti].d_str1;
                const int   LEN1 = (int)strlen(STR1);
                const char *STR2 = DATA[ti].d_str2;
                const int   LEN2 = (int)strlen(STR2);
                const char *STR3 = DATA[ti].d_str3;
                const int   LEN3 = (int)strlen(STR3);
                const char *STR4 = DATA[ti].d_str4;
                const int   LEN4 = (int)strlen(STR4);
                const int   LEN  = LEN1 + !!LEN2 + LEN3 + !!LEN4;

                const int   LEN123 = LEN1 + !!LEN2 + LEN3;
                const int   LEN12  = LEN1 + !!LEN2;

                Obj mX(&testAllocator);  const Obj& X = mX;
                BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    {
                        mX.removeAll();
                        mX.appendSequence(STR1, LEN1);
                        mX.appendSet(STR2, LEN2);
                        mX.appendSequence(STR3, LEN3);
                        mX.appendSet(STR4, LEN4);
                    }
                    testAllocator.setAllocationLimit(AL);
                    mX.assignKleeneStar();
                } END_BDEMA_EXCEPTION_TEST

                if (verbose) {
                    cout << "\t";
                    cout << "\t"; P_(STR1);
                    cout << "\t"; P_(STR2);
                    cout << "\t"; P_(STR3);
                    cout << "\t"; P(STR4);
                }

                if (0 == LEN) {
                    for (int value = 0; value < 256; ++value) {
                        ObjAcc a(X);
                        ASSERT(0 == a.isError());
                        ASSERT(1 == a.isAccepting());
                        a.transition(value);
                        ASSERT(1 == a.isError());
                        ASSERT(0 == a.isAccepting());
                    }
                }
                else {
                    {
                        ObjAcc a(X);
                        ASSERT(0 == a.isError());
                        ASSERT(1 == a.isAccepting());
                    }
                    for (int iter = 0; iter < 10; ++iter) {
                        for (int i = 0; i < LEN; ++i) {
                            for (int value = 0; value < 256; ++value) {
                                ObjAcc a(X);
                                ASSERT(0 == a.isError());
                                ASSERT(1 == a.isAccepting());
                                for (int k = 0; k < iter; ++k) {
                                    {
                                        for (int j = 0; j < LEN1; ++j) {
                                            a.transition(STR1[j]);
                                        }
                                    }
                                    if (LEN2) {
                                        a.transition(STR2[0]);
                                    }
                                    {
                                        for (int j = 0; j < LEN3; ++j) {
                                            a.transition(STR3[j]);
                                        }
                                    }
                                    if (LEN4) {
                                        a.transition(STR4[0]);
                                    }
                                    ASSERT(0 == a.isError());
                                    ASSERT(1 == a.isAccepting());
                                }
                                if (i < LEN1) {
                                    for (int j = 0; j < i; ++j) {
                                        a.transition(STR1[j]);
                                        ASSERT(0 == a.isError());
                                        ASSERT(0 == a.isAccepting());
                                    }
                                    a.transition(value);
                                    const int E = (unsigned char)STR1[i]
                                                                      != value;
                                    const int A = !E && (LEN - 1 == i);
                                    ASSERT(E == a.isError());
                                    ASSERT(A == a.isAccepting());
                                }
                                else if (i < LEN12) {
                                    {
                                        for (int j = 0; j < LEN1; ++j) {
                                            a.transition(STR1[j]);
                                        }
                                    }
                                    int E = 1;
                                    for (int j = 0; j < LEN2; ++j) {
                                        if ((unsigned char)STR2[j] == value) {
                                            E = 0;
                                        }
                                    }
                                    const int A = !E && (LEN - 1 == i);
                                    a.transition(value);
                                    ASSERT(E == a.isError());
                                    ASSERT(A == a.isAccepting());
                                }
                                else if (i < LEN123) {
                                    {
                                        for (int j = 0; j < LEN1; ++j) {
                                            a.transition(STR1[j]);
                                        }
                                    }
                                    if (LEN2) {
                                        a.transition(STR2[0]);
                                    }
                                    for (int j = LEN12; j < i; ++j) {
                                        a.transition(STR3[j - LEN12]);
                                        ASSERT(0 == a.isError());
                                        ASSERT(0 == a.isAccepting());
                                    }
                                    a.transition(value);
                                    const int E = (unsigned char)
                                                      STR3[i - LEN12] != value;
                                    const int A = !E && (LEN - 1 == i);
                                    ASSERT(E == a.isError());
                                    ASSERT(A == a.isAccepting());
                                }
                                else {
                                    {
                                        for (int j = 0; j < LEN1; ++j) {
                                            a.transition(STR1[j]);
                                        }
                                    }
                                    if (LEN2) {
                                        a.transition(STR2[0]);
                                    }
                                    {
                                        for (int j = 0; j < LEN3; ++j) {
                                            a.transition(STR3[j]);
                                        }
                                    }
                                    int E = 1;
                                    for (int j = 0; j < LEN4; ++j) {
                                        if ((unsigned char)STR4[j] == value) {
                                            E = 0;
                                        }
                                    }
                                    const int A = !E && (LEN - 1 == i);
                                    a.transition(value);
                                    ASSERT(E == a.isError());
                                    ASSERT(A == a.isAccepting());
                                }
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING APPENDSEQUENCE and APPENDSET
        // TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing appendSequence and appendSet" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nTesting 'appendSequence' (initial)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str;      // source string
            } DATA[] = {
                //line     str
                //----  -------------
                { L_,   ""            },
                { L_,   "a"           },
                { L_,   "b"           },
                { L_,   "aa"          },
                { L_,   "ab"          },
                { L_,   "aaa"         },
                { L_,   "aba"         },
                { L_,   "abc"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                // const int   LINE = DATA[ti].d_lineNum;
                const char *STR  = DATA[ti].d_str;
                const int   LEN  = (int)strlen(STR);

                Obj mX(&testAllocator);  const Obj& X = mX;
                BEGIN_BDEMA_EXCEPTION_TEST {
                    mX.removeAll();
                    mX.appendSequence(STR, LEN);
                } END_BDEMA_EXCEPTION_TEST

                if (verbose) { cout << "\t\t"; P(STR); }

                if (0 == LEN) {
                    for (int value = 0; value < 256; ++value) {
                        ObjAcc a(X);
                        ASSERT(0 == a.isError());
                        ASSERT(1 == a.isAccepting());
                        a.transition(value);
                        ASSERT(1 == a.isError());
                        ASSERT(0 == a.isAccepting());
                    }
                }
                else {
                    for (int i = 0; i < LEN; ++i) {
                        if (veryVerbose) {
                            cout << "\t\t\tinitial traversal: ";
                            for (int j = 0; j < i; ++j) {
                                cout << STR[j];
                            }
                            cout << endl;
                        }
                        for (int value = 0; value < 256; ++value) {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(0 == a.isAccepting());
                            for (int j = 0; j < i; ++j) {
                                a.transition(STR[j]);
                                ASSERT(0 == a.isError());
                                ASSERT(0 == a.isAccepting());
                            }
                            a.transition(value);
                            const int E = (unsigned char)STR[i] != value;
                            const int A = !E && (LEN - 1 == i);
                            ASSERT(E == a.isError());
                            ASSERT(A == a.isAccepting());
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'appendSet' (initial)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str;      // source string
            } DATA[] = {
                //line     str
                //----  -------------
                { L_,   ""            },
                { L_,   "a"           },
                { L_,   "b"           },
                { L_,   "aa"          },
                { L_,   "ab"          },
                { L_,   "aaa"         },
                { L_,   "aba"         },
                { L_,   "abc"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                // const int   LINE = DATA[ti].d_lineNum;
                const char *STR  = DATA[ti].d_str;
                const int   LEN  = (int)strlen(STR);

                Obj mX(&testAllocator);  const Obj& X = mX;
                BEGIN_BDEMA_EXCEPTION_TEST {
                    mX.removeAll();
                    mX.appendSet(STR, LEN);
                } END_BDEMA_EXCEPTION_TEST

                if (verbose) { cout << "\t\t"; P(STR); }

                for (int value = 0; value < 256; ++value) {
                    int E = 1;
                    for (int i = 0; i < LEN; ++i) {
                        if ((unsigned char)STR[i] == value) {
                            E = 0;
                        }
                    }
                    const int A = !E;
                    ObjAcc a(X);
                    ASSERT(0 == a.isError());
                    ASSERT(!LEN == a.isAccepting());
                    a.transition(value);
                    ASSERT(E == a.isError());
                    ASSERT(A == a.isAccepting());
                }
            }
        }

        if (verbose) cout << "\nTesting 'appendSequence' and 'appendSet'."
                          << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_str1;     // first sequence source string
                const char *d_str2;     // first set source string
                const char *d_str3;     // second sequence source string
                const char *d_str4;     // second set source string
            } DATA[] = {
                //line    str1      str2      str3      str4
                //----  --------  --------  --------  --------
                { L_,   "",       "",       "",       ""     },
                { L_,   "a",      "a",      "",       ""     },
                { L_,   "a",      "ab",     "a",      ""     },
                { L_,   "a",      "ab",     "a",      "ab"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                // const int   LINE = DATA[ti].d_lineNum;
                const char *STR1 = DATA[ti].d_str1;
                const int   LEN1 = (int)strlen(STR1);
                const char *STR2 = DATA[ti].d_str2;
                const int   LEN2 = (int)strlen(STR2);
                const char *STR3 = DATA[ti].d_str3;
                const int   LEN3 = (int)strlen(STR3);
                const char *STR4 = DATA[ti].d_str4;
                const int   LEN4 = (int)strlen(STR4);
                const int   LEN  = LEN1 + !!LEN2 + LEN3 + !!LEN4;

                const int   LEN123 = LEN1 + !!LEN2 + LEN3;
                const int   LEN12  = LEN1 + !!LEN2;

                Obj mX(&testAllocator);  const Obj& X = mX;
                BEGIN_BDEMA_EXCEPTION_TEST {
                    mX.removeAll();
                    mX.appendSequence(STR1, LEN1);
                    mX.appendSet(STR2, LEN2);
                    mX.appendSequence(STR3, LEN3);
                    mX.appendSet(STR4, LEN4);
                } END_BDEMA_EXCEPTION_TEST

                if (verbose) {
                    cout << "\t";
                    cout << "\t"; P_(STR1);
                    cout << "\t"; P_(STR2);
                    cout << "\t"; P_(STR3);
                    cout << "\t"; P(STR4);
                }

                if (0 == LEN) {
                    for (int value = 0; value < 256; ++value) {
                        ObjAcc a(X);
                        ASSERT(0 == a.isError());
                        ASSERT(1 == a.isAccepting());
                        a.transition(value);
                        ASSERT(1 == a.isError());
                        ASSERT(0 == a.isAccepting());
                    }
                }
                else {
                    for (int i = 0; i < LEN; ++i) {
                        for (int value = 0; value < 256; ++value) {
                            ObjAcc a(X);
                            ASSERT(0 == a.isError());
                            ASSERT(0 == a.isAccepting());
                            if (i < LEN1) {
                                for (int j = 0; j < i; ++j) {
                                    a.transition(STR1[j]);
                                    ASSERT(0 == a.isError());
                                    ASSERT(0 == a.isAccepting());
                                }
                                a.transition(value);
                                const int E = (unsigned char)STR1[i] != value;
                                const int A = !E && (LEN - 1 == i);
                                ASSERT(E == a.isError());
                                ASSERT(A == a.isAccepting());
                            }
                            else if (i < LEN12) {
                                {
                                    for (int j = 0; j < LEN1; ++j) {
                                        a.transition(STR1[j]);
                                    }
                                }
                                int E = 1;
                                for (int j = 0; j < LEN2; ++j) {
                                    if ((unsigned char)STR2[j] == value) {
                                        E = 0;
                                    }
                                }
                                const int A = !E && (LEN - 1 == i);
                                a.transition(value);
                                ASSERT(E == a.isError());
                                ASSERT(A == a.isAccepting());
                            }
                            else if (i < LEN123) {
                                {
                                    for (int j = 0; j < LEN1; ++j) {
                                        a.transition(STR1[j]);
                                    }
                                }
                                if (LEN2) {
                                    a.transition(STR2[0]);
                                }
                                for (int j = LEN12; j < i; ++j) {
                                    a.transition(STR3[j - LEN12]);
                                    ASSERT(0 == a.isError());
                                    ASSERT(0 == a.isAccepting());
                                }
                                a.transition(value);
                                const int E = (unsigned char)STR3[i - LEN12]
                                                                      != value;
                                const int A = !E && (LEN - 1 == i);
                                ASSERT(E == a.isError());
                                ASSERT(A == a.isAccepting());
                            }
                            else {
                                {
                                    for (int j = 0; j < LEN1; ++j) {
                                        a.transition(STR1[j]);
                                    }
                                }
                                if (LEN2) {
                                    a.transition(STR2[0]);
                                }
                                {
                                    for (int j = 0; j < LEN3; ++j) {
                                        a.transition(STR3[j]);
                                    }
                                }
                                int E = 1;
                                for (int j = 0; j < LEN4; ++j) {
                                    if ((unsigned char)STR4[j] == value) {
                                        E = 0;
                                    }
                                }
                                const int A = !E && (LEN - 1 == i);
                                a.transition(value);
                                ASSERT(E == a.isError());
                                ASSERT(A == a.isAccepting());
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT CONSTRUCTOR:
        // TBD need append and accumulator here as well
        //   The basic concern is that the default constructor operates as
        //   expected.  We have the following specific concerns:
        //        - creates the correct initial value.
        //        - is exception neutral with respect to memory allocation.
        //        - has the internal memory management system hooked up
        //            properly so that *all* internally allocated memory draws
        //            from the same user-supplied allocator whenever one is
        //            specified.
        //
        // Plan:
        //   To address these concerns, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //
        // Testing:
        //   bdecs_FiniteAutomaton(bdema_Allocator *ba);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Default Constructor" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bdema_Allocator *)0);
            for (int i = 0; i < 256; ++i) {
                ObjAcc a(X);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            for (int i = 0; i < 256; ++i) {
                ObjAcc a(X);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
        }

        if (verbose) cout << "\t\tWith execeptions." << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            const Obj X(&testAllocator);
            for (int i = 0; i < 256; ++i) {
                ObjAcc a(X);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
          } END_BDEMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[4096];
            bdema_BufferAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);

            for (int i = 0; i < 256; ++i) {
                ObjAcc a(*(Obj *)doNotDelete);
                ASSERT(0 == a.isError());
                ASSERT(1 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
                a.transition(i);
                ASSERT(1 == a.isError());
                ASSERT(0 == a.isAccepting());
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

      } break;
      case 1: {
        Obj fa(&testAllocator);
        fa.appendSequence("abc", 3);
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            ASSERT(0 == a.isAccepting());
        }
        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('b');
            ASSERT(0 == a.isAccepting());
        }
        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            a.transition('b');
            ASSERT(0 == a.isAccepting());
        }
        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            a.transition('b');
            a.transition('a');
            ASSERT(0 == a.isAccepting());
        }
        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            a.transition('b');
            a.transition('c');
            ASSERT(1 == a.isAccepting());
        }
        {
            ObjAcc a(fa);
            ASSERT(0 == a.isAccepting());
            a.transition('a');
            a.transition('b');
            a.transition('c');
            a.transition('d');
            ASSERT(0 == a.isAccepting());
        }

        fa.assignKleeneStar();
        if (veryVerbose) { cout << "Line: " << L_ << endl << fa; }

        {
            ObjAcc a(fa);
            ASSERT(1 == a.isAccepting());
            for (int i = 0; i < 10; ++i) {
                a.transition('a');
                ASSERT(0 == a.isAccepting());
                a.transition('b');
                ASSERT(0 == a.isAccepting());
                a.transition('c');
                ASSERT(1 == a.isAccepting());
            }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
