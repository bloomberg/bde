// bdecs_regex.t.cpp         -*-C++-*-

#include <bdecs_regex.h>

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
// 'add' and 'removeAll' methods to be used by the generator functions
// 'g' and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that each
// manipulator that performs memory allocation
// must be tested for exception neutrality via the 'bdema_testallocator'
// component.  Exception neutrality involving streaming is verified using
// 'bdex_testinstream' (and 'bdex_testoutstream').
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 2] bdec_DoubleSet(bdema_Allocator *ba = 0);
// [15] bdec_DoubleSet(const InitialCapacity& ne, *ba = 0);
// [ 7] bdec_DoubleSet(const bdec_DoubleSet& original, *ba = 0);
// [ 2] ~bdec_DoubleSet();
// [ 9] bdec_DoubleSet& operator=(const bdec_DoubleSet& rhs);
// [14] bdec_DoubleSet& operator|=(const bdec_DoubleSet& rhs);
// [14] bdec_DoubleSet& operator&=(const bdec_DoubleSet& rhs);
// [14] bdec_DoubleSet& operator^=(const bdec_DoubleSet& rhs);
// [14] bdec_DoubleSet& operator-=(const bdec_DoubleSet& rhs);
// [12] const double *add(double value);
// [12] int remove(double value);
// [ 2] void removeAll();
// [15] void reserveCapacity(int ne);
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [13] int isContainedBy(const bdec_DoubleSet& set) const;
// [ 4] int isMember(double value) const;
// [ 4] int length() const;
// [ 4] const double *lookup(double value) const;
// [11] ostream& print(ostream& stream, int level, int spl);
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
//
// [14] operator|(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [14] operator&(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [14] operator^(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [14] operator-(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [ 6] operator==(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [ 6] operator!=(const bdec_DoubleSet&, const bdec_DoubleSet&);
// [ 5] operator<<(ostream&, const bdec_DoubleSet&);
// [10] operator>>(bdex_InStream&, bdec_DoubleSet&);
// [10] operator<<(bdex_OutStream&, const bdec_DoubleSet&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void add(double value);
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] int ggg(bdec_DoubleSet *object, const char *spec, int vF = 1);
// [ 3] bdec_DoubleSet& gg(bdec_DoubleSet* object, const char *spec);
// [ 8] bdec_DoubleSet   g(const char *spec);

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

typedef bdecs_Regex Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) double values to be
// added to the 'bdecs_Regex' object.  A tilde ('~') indicates that
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

int ggg(bdecs_Regex *object, const char *spec, int verboseFlag = 1)
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

bdecs_Regex& gg(bdecs_Regex *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

//bdecs_Regex g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
//{
  //    bdecs_Regex object((bdema_Allocator *)0);
  //return gg(&object, spec);
//}

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
      case 29: {
        {
            Obj r("^a[ab]{20}$", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
      } break;
      case 28: {
        {
            Obj r("^a[ab]{20}", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
      } break;
      case 27: {
// TBD SLOW!!!!
#if 0
        {
            Obj r("a[ab]{20}$", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
#endif
      } break;
      case 26: {
        {
          /* TBD way too slow
            Obj r("^((a{1, 10}){1, 10}){1, 10}$", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("aaaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("aaaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaa", 18));
            */
        }
      } break;
      case 25: {
        {
          /* TBD way too slow
            Obj r("^((a{1, 10}){1, 10}){1, 10}", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("aaaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("aaaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaa", 18));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaa", 19));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaa", 20));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaa", 21));
            */
        }
      } break;
      case 24: {
// TBD SLOW!!!!
#if 0
        {
            Obj r("((a{1, 10}){1, 10}){1, 10}$", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("aaaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("aaaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaa", 18));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaa", 19));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaa", 20));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaa", 21));
        }
#endif
      } break;
      case 23: {
        {
            Obj r("^a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab]$", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
      } break;
      case 22: {
        {
            Obj r("^a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab]", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
      } break;
      case 21: {
// TBD SLOW!!!!
#if 0
        {
            Obj r("a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab]$", &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaab", 21));
        }
#endif
      } break;
      case 20: {
        {
            Obj r("^a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab](wee|week)(knights|night)$",
                  &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaabweeknights", 31));
        }
      } break;
      case 19: {
        {
            Obj r("^a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab](wee|week)(knights|night)",
                  &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaabweeknights", 31));
        }
      } break;
      case 18: {
// TBD SLOW!!!!
#if 0
        {
            Obj r("a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
                  "[ab][ab][ab][ab][ab][ab](wee|week)(knights|night)$",
                  &testAllocator);

            ASSERT(1 == r.isMatch("aaaaabaaaabaaaabaaaabweeknights", 31));
        }
#endif
      } break;
      case 17: {
        {
            Obj r("A[a\\b\nc]Z", &testAllocator);

            ASSERT(0 == r.isMatch("AZ", 2));
            ASSERT(0 == r.isMatch("AnZ", 3));
            ASSERT(1 == r.isMatch("AaZ", 3));
            ASSERT(1 == r.isMatch("A\\Z", 3));
            ASSERT(1 == r.isMatch("AbZ", 3));
            ASSERT(1 == r.isMatch("A\nZ", 3));
            ASSERT(1 == r.isMatch("AcZ", 3));
        }
      } break;
      case 16: {
        {
            {
                Obj r("", &testAllocator);

                ASSERT(1 == r.isMatch("", 0));
                ASSERT(1 == r.isMatch("a", 1));
                ASSERT(1 == r.isMatch("abcdef", 1));
            }
            {
                Obj r("()", &testAllocator);

                ASSERT(1 == r.isMatch("", 0));
                ASSERT(1 == r.isMatch("a", 1));
                ASSERT(1 == r.isMatch("abcdef", 1));
            }
            {
                Obj r("((()(())))", &testAllocator);

                ASSERT(1 == r.isMatch("", 0));
                ASSERT(1 == r.isMatch("a", 1));
                ASSERT(1 == r.isMatch("abcdef", 1));
            }
        }
      } break;
      case 15: {
        {
            {
                // [ab]c - no gobble

                Obj r("^[ab]c$", &testAllocator);

                ASSERT(0 == r.isMatch("a", 1));
                ASSERT(0 == r.isMatch("b", 1));
                ASSERT(0 == r.isMatch("c", 1));
                ASSERT(1 == r.isMatch("ac", 2));
                ASSERT(1 == r.isMatch("bc", 2));
                ASSERT(0 == r.isMatch("cc", 2));
                ASSERT(0 == r.isMatch("acc", 3));
                ASSERT(0 == r.isMatch("bcc", 3));
            }
            {
                // [ab]c - pre-gobble

                Obj r("^[abc]*[ab]c$", &testAllocator);

                ASSERT(0 == r.isMatch("a", 1));
                ASSERT(0 == r.isMatch("b", 1));
                ASSERT(0 == r.isMatch("c", 1));
                ASSERT(1 == r.isMatch("ac", 2));
                ASSERT(1 == r.isMatch("bc", 2));
                ASSERT(0 == r.isMatch("cc", 2));
                ASSERT(1 == r.isMatch("cac", 3));
                ASSERT(1 == r.isMatch("cbc", 3));
                ASSERT(1 == r.isMatch("cccac", 5));
                ASSERT(1 == r.isMatch("cccbc", 5));
                ASSERT(0 == r.isMatch("cccacc", 6));
                ASSERT(0 == r.isMatch("cccbcc", 6));
                ASSERT(0 == r.isMatch("cccaca", 6));
                ASSERT(0 == r.isMatch("cccbcb", 6));
            }
            {
                // [ab]c - post-gobble

                Obj r("^[ab]c[abc]*$", &testAllocator);

                ASSERT(0 == r.isMatch("a", 1));
                ASSERT(0 == r.isMatch("b", 1));
                ASSERT(0 == r.isMatch("c", 1));
                ASSERT(1 == r.isMatch("ac", 2));
                ASSERT(1 == r.isMatch("bc", 2));
                ASSERT(0 == r.isMatch("cc", 2));
                ASSERT(1 == r.isMatch("acc", 3));
                ASSERT(1 == r.isMatch("bcc", 3));
                ASSERT(1 == r.isMatch("acccc", 5));
                ASSERT(1 == r.isMatch("bcccc", 5));
                ASSERT(1 == r.isMatch("acb", 3));
                ASSERT(1 == r.isMatch("bca", 3));
                ASSERT(0 == r.isMatch("cac", 3));
                ASSERT(0 == r.isMatch("cbc", 3));
                ASSERT(0 == r.isMatch("ccac", 4));
                ASSERT(0 == r.isMatch("ccbc", 4));
                ASSERT(0 == r.isMatch("cccac", 5));
                ASSERT(0 == r.isMatch("cccbc", 5));
            }
            {
                // [ab]c - pre-gobble and post-gobble

                Obj r("^[abc]*[ab]c[abc]*$", &testAllocator);

                ASSERT(0 == r.isMatch("a", 1));
                ASSERT(0 == r.isMatch("b", 1));
                ASSERT(0 == r.isMatch("c", 1));
                ASSERT(1 == r.isMatch("ac", 2));
                ASSERT(1 == r.isMatch("bc", 2));
                ASSERT(0 == r.isMatch("cc", 2));
                ASSERT(1 == r.isMatch("acc", 3));
                ASSERT(1 == r.isMatch("bcc", 3));
                ASSERT(1 == r.isMatch("acccc", 5));
                ASSERT(1 == r.isMatch("bcccc", 5));
                ASSERT(1 == r.isMatch("acb", 3));
                ASSERT(1 == r.isMatch("bca", 3));
                ASSERT(1 == r.isMatch("cac", 3));
                ASSERT(1 == r.isMatch("cbc", 3));
                ASSERT(1 == r.isMatch("ccac", 4));
                ASSERT(1 == r.isMatch("ccbc", 4));
                ASSERT(1 == r.isMatch("cccac", 5));
                ASSERT(1 == r.isMatch("cccbc", 5));
                ASSERT(1 == r.isMatch("ccacc", 5));
                ASSERT(1 == r.isMatch("ccbcc", 5));
                ASSERT(1 == r.isMatch("caccc", 5));
                ASSERT(1 == r.isMatch("cbccc", 5));
            }
            {
                // c[ab] - no gobble

                Obj r("^c[ab]$", &testAllocator);

                ASSERT(0 == r.isMatch("a", 1));
                ASSERT(0 == r.isMatch("b", 1));
                ASSERT(0 == r.isMatch("c", 1));
                ASSERT(1 == r.isMatch("ca", 2));
                ASSERT(1 == r.isMatch("cb", 2));
                ASSERT(0 == r.isMatch("cc", 2));
                ASSERT(0 == r.isMatch("acc", 3));
                ASSERT(0 == r.isMatch("bcc", 3));
            }
        }
      } break;
      case 14: {
        if (verbose) cout << endl
            << "Tests for '{...}'" << endl
            << "=================" << endl;
        {
            Obj r("a{1,2}", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("baa", 3));
            ASSERT(1 == r.isMatch("ba", 2));
            ASSERT(0 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("bb", 2));
        }
        {
            Obj r("^a{1,2}$", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(0 == r.isMatch("aaa", 3));
            ASSERT(0 == r.isMatch("aaaa", 4));
            ASSERT(0 == r.isMatch("bb", 2));
            ASSERT(0 == r.isMatch("b", 1));
        }
        {
            Obj r("(a{1,2}){1,3}", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("baaaaaaa", 8));
            ASSERT(1 == r.isMatch("baaaaaa", 7));
            ASSERT(1 == r.isMatch("baaaaa", 6));
            ASSERT(1 == r.isMatch("baaaa", 5));
            ASSERT(1 == r.isMatch("baaa", 4));
            ASSERT(1 == r.isMatch("baa", 3));
            ASSERT(1 == r.isMatch("ba", 2));
            ASSERT(0 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("bb", 2));
            ASSERT(0 == r.isMatch("bbb", 3));
        }
        {
            Obj r("^(a{1,2}){1,3}$", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(0 == r.isMatch("aaaaaaa", 7));
            ASSERT(0 == r.isMatch("aaaaaaaa", 8));
            ASSERT(0 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("bb", 2));
            ASSERT(0 == r.isMatch("bbb", 3));
        }
        {
            Obj r("((a{1,2}){1,3}){1,4}", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("aaaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("aaaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaa", 18));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaa", 19));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaa", 20));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaa", 21));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaa", 22));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaa", 23));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaa", 24));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaaa", 25));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaaaa", 26));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaaaaaaa", 25));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaaaaaa", 24));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaaaaa", 23));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaaaa", 22));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaaa", 21));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaaa", 20));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaaa", 19));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaaa", 18));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("baaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("baaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("baaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("baaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("baaaaaaaa", 9));
            ASSERT(1 == r.isMatch("baaaaaaa", 8));
            ASSERT(1 == r.isMatch("baaaaaa", 7));
            ASSERT(1 == r.isMatch("baaaaa", 6));
            ASSERT(1 == r.isMatch("baaaa", 5));
            ASSERT(1 == r.isMatch("baaa", 4));
            ASSERT(1 == r.isMatch("baa", 3));
            ASSERT(1 == r.isMatch("ba", 2));
            ASSERT(0 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("bb", 2));
            ASSERT(0 == r.isMatch("bbb", 3));
            ASSERT(0 == r.isMatch("bbbb", 4));
            ASSERT(0 == r.isMatch("bbbbb", 5));
            ASSERT(0 == r.isMatch("bbbbbb", 6));
            ASSERT(0 == r.isMatch("bbbbbbb", 7));
            ASSERT(0 == r.isMatch("bbbbbbbb", 8));
            ASSERT(0 == r.isMatch("bbbbbbbbb", 9));
            ASSERT(0 == r.isMatch("bbbbbbbbbb", 10));
            ASSERT(0 == r.isMatch("bbbbbbbbbbb", 11));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbb", 12));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbb", 13));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbb", 14));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbb", 15));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbb", 16));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbb", 17));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbb", 18));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbb", 19));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbb", 20));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbbb", 21));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbbbb", 22));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbbbbb", 23));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbbbbbb", 24));
            ASSERT(0 == r.isMatch("bbbbbbbbbbbbbbbbbbbbbbbbb", 25));
        }
        {
            Obj r("^((a{1,2}){1,3}){1,4}$", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("aaa", 3));
            ASSERT(1 == r.isMatch("aaaa", 4));
            ASSERT(1 == r.isMatch("aaaaa", 5));
            ASSERT(1 == r.isMatch("aaaaaa", 6));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("aaaaaaaa", 8));
            ASSERT(1 == r.isMatch("aaaaaaaaa", 9));
            ASSERT(1 == r.isMatch("aaaaaaaaaa", 10));
            ASSERT(1 == r.isMatch("aaaaaaaaaaa", 11));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaa", 12));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaa", 13));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaa", 14));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaa", 15));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaa", 16));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaa", 17));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaa", 18));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaa", 19));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaa", 20));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaa", 21));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaa", 22));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaa", 23));
            ASSERT(1 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaa", 24));
            ASSERT(0 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaaa", 25));
            ASSERT(0 == r.isMatch("aaaaaaaaaaaaaaaaaaaaaaaaaa", 26));
            ASSERT(0 == r.isMatch("b", 1));
        }
      } break;
      case 13: {
        if (verbose) cout << endl
            << "Tests for '^' and '$' anchors" << endl
            << "=============================" << endl;

        static const struct {
                int         d_lineNum;  // source line number
                const char *d_re;       // regular expression
                const char *d_str;      // string to compare against r.e.
                int         d_expect;   // 1 if should match; 0 otherwise
        } DATA[] = {
                //line  r.e.   test string  matches?
                //----  ----   -----------  --------

                { L_, "GE|HP|IBM", "GE", 1, },
                { L_, "GE|HP|IBM", "abcGE", 1, },
                { L_, "GE|HP|IBM", "GExyz", 1, },
                { L_, "GE|HP|IBM", "abcGExyz", 1, },
                { L_, "GE|HP|IBM", "HP", 1, },
                { L_, "GE|HP|IBM", "abcHP", 1, },
                { L_, "GE|HP|IBM", "HPxyz", 1, },
                { L_, "GE|HP|IBM", "abcHPxyz", 1, },
                { L_, "GE|HP|IBM", "IBM", 1, },
                { L_, "GE|HP|IBM", "abcIBM", 1, },
                { L_, "GE|HP|IBM", "IBMxyz", 1, },
                { L_, "GE|HP|IBM", "abcIBMxyz", 1, },
                { L_, "GE|^HP|IBM", "GE", 1, },
                { L_, "GE|^HP|IBM", "abcGE", 1, },
                { L_, "GE|^HP|IBM", "GExyz", 1, },
                { L_, "GE|^HP|IBM", "abcGExyz", 1, },
                { L_, "GE|^HP|IBM", "HP", 1, },
                { L_, "GE|^HP|IBM", "abcHP", 0, },
                { L_, "GE|^HP|IBM", "HPxyz", 1, },
                { L_, "GE|^HP|IBM", "abcHPxyz", 0, },
                { L_, "GE|^HP|IBM", "IBM", 1, },
                { L_, "GE|^HP|IBM", "abcIBM", 1, },
                { L_, "GE|^HP|IBM", "IBMxyz", 1, },
                { L_, "GE|^HP|IBM", "abcIBMxyz", 1, },
/* TBD way too slow
                { L_, "GE|HP$|IBM", "GE", 1, },
                { L_, "GE|HP$|IBM", "abcGE", 1, },
                { L_, "GE|HP$|IBM", "GExyz", 1, },
                { L_, "GE|HP$|IBM", "abcGExyz", 1, },
                { L_, "GE|HP$|IBM", "HP", 1, },
                { L_, "GE|HP$|IBM", "abcHP", 1, },
                { L_, "GE|HP$|IBM", "HPxyz", 0, },
                { L_, "GE|HP$|IBM", "abcHPxyz", 0, },
                { L_, "GE|HP$|IBM", "IBM", 1, },
                { L_, "GE|HP$|IBM", "abcIBM", 1, },
                { L_, "GE|HP$|IBM", "IBMxyz", 1, },
                { L_, "GE|HP$|IBM", "abcIBMxyz", 1, },
                { L_, "GE|^HP$|IBM", "GE", 1, },
                { L_, "GE|^HP$|IBM", "abcGE", 1, },
                { L_, "GE|^HP$|IBM", "GExyz", 1, },
                { L_, "GE|^HP$|IBM", "abcGExyz", 1, },
                { L_, "GE|^HP$|IBM", "HP", 1, },
                { L_, "GE|^HP$|IBM", "abcHP", 0, },
                { L_, "GE|^HP$|IBM", "HPxyz", 0, },
                { L_, "GE|^HP$|IBM", "abcHPxyz", 0, },
                { L_, "GE|^HP$|IBM", "IBM", 1, },
                { L_, "GE|^HP$|IBM", "abcIBM", 1, },
                { L_, "GE|^HP$|IBM", "IBMxyz", 1, },
                { L_, "GE|^HP$|IBM", "abcIBMxyz", 1, },
                { L_, "GE|HP|^IBM", "GE", 1, },
                { L_, "GE|HP|^IBM", "abcGE", 1, },
                { L_, "GE|HP|^IBM", "GExyz", 1, },
                { L_, "GE|HP|^IBM", "abcGExyz", 1, },
                { L_, "GE|HP|^IBM", "HP", 1, },
                { L_, "GE|HP|^IBM", "abcHP", 1, },
                { L_, "GE|HP|^IBM", "HPxyz", 1, },
                { L_, "GE|HP|^IBM", "abcHPxyz", 1, },
                { L_, "GE|HP|^IBM", "IBM", 1, },
                { L_, "GE|HP|^IBM", "abcIBM", 0, },
                { L_, "GE|HP|^IBM", "IBMxyz", 1, },
                { L_, "GE|HP|^IBM", "abcIBMxyz", 0, },
                { L_, "GE|^HP|^IBM", "GE", 1, },
                { L_, "GE|^HP|^IBM", "abcGE", 1, },
                { L_, "GE|^HP|^IBM", "GExyz", 1, },
                { L_, "GE|^HP|^IBM", "abcGExyz", 1, },
                { L_, "GE|^HP|^IBM", "HP", 1, },
                { L_, "GE|^HP|^IBM", "abcHP", 0, },
                { L_, "GE|^HP|^IBM", "HPxyz", 1, },
                { L_, "GE|^HP|^IBM", "abcHPxyz", 0, },
                { L_, "GE|^HP|^IBM", "IBM", 1, },
                { L_, "GE|^HP|^IBM", "abcIBM", 0, },
                { L_, "GE|^HP|^IBM", "IBMxyz", 1, },
                { L_, "GE|^HP|^IBM", "abcIBMxyz", 0, },
                { L_, "GE|HP$|^IBM", "GE", 1, },
                { L_, "GE|HP$|^IBM", "abcGE", 1, },
                { L_, "GE|HP$|^IBM", "GExyz", 1, },
                { L_, "GE|HP$|^IBM", "abcGExyz", 1, },
                { L_, "GE|HP$|^IBM", "HP", 1, },
                { L_, "GE|HP$|^IBM", "abcHP", 1, },
                { L_, "GE|HP$|^IBM", "HPxyz", 0, },
                { L_, "GE|HP$|^IBM", "abcHPxyz", 0, },
                { L_, "GE|HP$|^IBM", "IBM", 1, },
                { L_, "GE|HP$|^IBM", "abcIBM", 0, },
                { L_, "GE|HP$|^IBM", "IBMxyz", 1, },
                { L_, "GE|HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "GE|^HP$|^IBM", "GE", 1, },
                { L_, "GE|^HP$|^IBM", "abcGE", 1, },
                { L_, "GE|^HP$|^IBM", "GExyz", 1, },
                { L_, "GE|^HP$|^IBM", "abcGExyz", 1, },
                { L_, "GE|^HP$|^IBM", "HP", 1, },
                { L_, "GE|^HP$|^IBM", "abcHP", 0, },
                { L_, "GE|^HP$|^IBM", "HPxyz", 0, },
                { L_, "GE|^HP$|^IBM", "abcHPxyz", 0, },
                { L_, "GE|^HP$|^IBM", "IBM", 1, },
                { L_, "GE|^HP$|^IBM", "abcIBM", 0, },
                { L_, "GE|^HP$|^IBM", "IBMxyz", 1, },
                { L_, "GE|^HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "GE|HP|IBM$", "GE", 1, },
                { L_, "GE|HP|IBM$", "abcGE", 1, },
                { L_, "GE|HP|IBM$", "GExyz", 1, },
                { L_, "GE|HP|IBM$", "abcGExyz", 1, },
                { L_, "GE|HP|IBM$", "HP", 1, },
                { L_, "GE|HP|IBM$", "abcHP", 1, },
                { L_, "GE|HP|IBM$", "HPxyz", 1, },
                { L_, "GE|HP|IBM$", "abcHPxyz", 1, },
                { L_, "GE|HP|IBM$", "IBM", 1, },
                { L_, "GE|HP|IBM$", "abcIBM", 1, },
                { L_, "GE|HP|IBM$", "IBMxyz", 0, },
                { L_, "GE|HP|IBM$", "abcIBMxyz", 0, },
                { L_, "GE|^HP|IBM$", "GE", 1, },
                { L_, "GE|^HP|IBM$", "abcGE", 1, },
                { L_, "GE|^HP|IBM$", "GExyz", 1, },
                { L_, "GE|^HP|IBM$", "abcGExyz", 1, },
                { L_, "GE|^HP|IBM$", "HP", 1, },
                { L_, "GE|^HP|IBM$", "abcHP", 0, },
                { L_, "GE|^HP|IBM$", "HPxyz", 1, },
                { L_, "GE|^HP|IBM$", "abcHPxyz", 0, },
                { L_, "GE|^HP|IBM$", "IBM", 1, },
                { L_, "GE|^HP|IBM$", "abcIBM", 1, },
                { L_, "GE|^HP|IBM$", "IBMxyz", 0, },
                { L_, "GE|^HP|IBM$", "abcIBMxyz", 0, },
                { L_, "GE|HP$|IBM$", "GE", 1, },
                { L_, "GE|HP$|IBM$", "abcGE", 1, },
                { L_, "GE|HP$|IBM$", "GExyz", 1, },
                { L_, "GE|HP$|IBM$", "abcGExyz", 1, },
                { L_, "GE|HP$|IBM$", "HP", 1, },
                { L_, "GE|HP$|IBM$", "abcHP", 1, },
                { L_, "GE|HP$|IBM$", "HPxyz", 0, },
                { L_, "GE|HP$|IBM$", "abcHPxyz", 0, },
                { L_, "GE|HP$|IBM$", "IBM", 1, },
                { L_, "GE|HP$|IBM$", "abcIBM", 1, },
                { L_, "GE|HP$|IBM$", "IBMxyz", 0, },
                { L_, "GE|HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "GE|^HP$|IBM$", "GE", 1, },
                { L_, "GE|^HP$|IBM$", "abcGE", 1, },
                { L_, "GE|^HP$|IBM$", "GExyz", 1, },
                { L_, "GE|^HP$|IBM$", "abcGExyz", 1, },
                { L_, "GE|^HP$|IBM$", "HP", 1, },
                { L_, "GE|^HP$|IBM$", "abcHP", 0, },
                { L_, "GE|^HP$|IBM$", "HPxyz", 0, },
                { L_, "GE|^HP$|IBM$", "abcHPxyz", 0, },
                { L_, "GE|^HP$|IBM$", "IBM", 1, },
                { L_, "GE|^HP$|IBM$", "abcIBM", 1, },
                { L_, "GE|^HP$|IBM$", "IBMxyz", 0, },
                { L_, "GE|^HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "GE|HP|^IBM$", "GE", 1, },
                { L_, "GE|HP|^IBM$", "abcGE", 1, },
                { L_, "GE|HP|^IBM$", "GExyz", 1, },
                { L_, "GE|HP|^IBM$", "abcGExyz", 1, },
                { L_, "GE|HP|^IBM$", "HP", 1, },
                { L_, "GE|HP|^IBM$", "abcHP", 1, },
                { L_, "GE|HP|^IBM$", "HPxyz", 1, },
                { L_, "GE|HP|^IBM$", "abcHPxyz", 1, },
                { L_, "GE|HP|^IBM$", "IBM", 1, },
                { L_, "GE|HP|^IBM$", "abcIBM", 0, },
                { L_, "GE|HP|^IBM$", "IBMxyz", 0, },
                { L_, "GE|HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE|^HP|^IBM$", "GE", 1, },
                { L_, "GE|^HP|^IBM$", "abcGE", 1, },
                { L_, "GE|^HP|^IBM$", "GExyz", 1, },
                { L_, "GE|^HP|^IBM$", "abcGExyz", 1, },
                { L_, "GE|^HP|^IBM$", "HP", 1, },
                { L_, "GE|^HP|^IBM$", "abcHP", 0, },
                { L_, "GE|^HP|^IBM$", "HPxyz", 1, },
                { L_, "GE|^HP|^IBM$", "abcHPxyz", 0, },
                { L_, "GE|^HP|^IBM$", "IBM", 1, },
                { L_, "GE|^HP|^IBM$", "abcIBM", 0, },
                { L_, "GE|^HP|^IBM$", "IBMxyz", 0, },
                { L_, "GE|^HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE|HP$|^IBM$", "GE", 1, },
                { L_, "GE|HP$|^IBM$", "abcGE", 1, },
                { L_, "GE|HP$|^IBM$", "GExyz", 1, },
                { L_, "GE|HP$|^IBM$", "abcGExyz", 1, },
                { L_, "GE|HP$|^IBM$", "HP", 1, },
                { L_, "GE|HP$|^IBM$", "abcHP", 1, },
                { L_, "GE|HP$|^IBM$", "HPxyz", 0, },
                { L_, "GE|HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "GE|HP$|^IBM$", "IBM", 1, },
                { L_, "GE|HP$|^IBM$", "abcIBM", 0, },
                { L_, "GE|HP$|^IBM$", "IBMxyz", 0, },
                { L_, "GE|HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE|^HP$|^IBM$", "GE", 1, },
                { L_, "GE|^HP$|^IBM$", "abcGE", 1, },
                { L_, "GE|^HP$|^IBM$", "GExyz", 1, },
                { L_, "GE|^HP$|^IBM$", "abcGExyz", 1, },
                { L_, "GE|^HP$|^IBM$", "HP", 1, },
                { L_, "GE|^HP$|^IBM$", "abcHP", 0, },
                { L_, "GE|^HP$|^IBM$", "HPxyz", 0, },
                { L_, "GE|^HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "GE|^HP$|^IBM$", "IBM", 1, },
                { L_, "GE|^HP$|^IBM$", "abcIBM", 0, },
                { L_, "GE|^HP$|^IBM$", "IBMxyz", 0, },
                { L_, "GE|^HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|HP|IBM", "GE", 1, },
                { L_, "^GE|HP|IBM", "abcGE", 0, },
                { L_, "^GE|HP|IBM", "GExyz", 1, },
                { L_, "^GE|HP|IBM", "abcGExyz", 0, },
                { L_, "^GE|HP|IBM", "HP", 1, },
                { L_, "^GE|HP|IBM", "abcHP", 1, },
                { L_, "^GE|HP|IBM", "HPxyz", 1, },
                { L_, "^GE|HP|IBM", "abcHPxyz", 1, },
                { L_, "^GE|HP|IBM", "IBM", 1, },
                { L_, "^GE|HP|IBM", "abcIBM", 1, },
                { L_, "^GE|HP|IBM", "IBMxyz", 1, },
                { L_, "^GE|HP|IBM", "abcIBMxyz", 1, },
                { L_, "^GE|^HP|IBM", "GE", 1, },
                { L_, "^GE|^HP|IBM", "abcGE", 0, },
                { L_, "^GE|^HP|IBM", "GExyz", 1, },
                { L_, "^GE|^HP|IBM", "abcGExyz", 0, },
                { L_, "^GE|^HP|IBM", "HP", 1, },
                { L_, "^GE|^HP|IBM", "abcHP", 0, },
                { L_, "^GE|^HP|IBM", "HPxyz", 1, },
                { L_, "^GE|^HP|IBM", "abcHPxyz", 0, },
                { L_, "^GE|^HP|IBM", "IBM", 1, },
                { L_, "^GE|^HP|IBM", "abcIBM", 1, },
                { L_, "^GE|^HP|IBM", "IBMxyz", 1, },
                { L_, "^GE|^HP|IBM", "abcIBMxyz", 1, },
                { L_, "^GE|HP$|IBM", "GE", 1, },
                { L_, "^GE|HP$|IBM", "abcGE", 0, },
                { L_, "^GE|HP$|IBM", "GExyz", 1, },
                { L_, "^GE|HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE|HP$|IBM", "HP", 1, },
                { L_, "^GE|HP$|IBM", "abcHP", 1, },
                { L_, "^GE|HP$|IBM", "HPxyz", 0, },
                { L_, "^GE|HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE|HP$|IBM", "IBM", 1, },
                { L_, "^GE|HP$|IBM", "abcIBM", 1, },
                { L_, "^GE|HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE|HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE|^HP$|IBM", "GE", 1, },
                { L_, "^GE|^HP$|IBM", "abcGE", 0, },
                { L_, "^GE|^HP$|IBM", "GExyz", 1, },
                { L_, "^GE|^HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE|^HP$|IBM", "HP", 1, },
                { L_, "^GE|^HP$|IBM", "abcHP", 0, },
                { L_, "^GE|^HP$|IBM", "HPxyz", 0, },
                { L_, "^GE|^HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE|^HP$|IBM", "IBM", 1, },
                { L_, "^GE|^HP$|IBM", "abcIBM", 1, },
                { L_, "^GE|^HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE|^HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE|HP|^IBM", "GE", 1, },
                { L_, "^GE|HP|^IBM", "abcGE", 0, },
                { L_, "^GE|HP|^IBM", "GExyz", 1, },
                { L_, "^GE|HP|^IBM", "abcGExyz", 0, },
                { L_, "^GE|HP|^IBM", "HP", 1, },
                { L_, "^GE|HP|^IBM", "abcHP", 1, },
                { L_, "^GE|HP|^IBM", "HPxyz", 1, },
                { L_, "^GE|HP|^IBM", "abcHPxyz", 1, },
                { L_, "^GE|HP|^IBM", "IBM", 1, },
                { L_, "^GE|HP|^IBM", "abcIBM", 0, },
                { L_, "^GE|HP|^IBM", "IBMxyz", 1, },
                { L_, "^GE|HP|^IBM", "abcIBMxyz", 0, },
                { L_, "^GE|^HP|^IBM", "GE", 1, },
                { L_, "^GE|^HP|^IBM", "abcGE", 0, },
                { L_, "^GE|^HP|^IBM", "GExyz", 1, },
                { L_, "^GE|^HP|^IBM", "abcGExyz", 0, },
                { L_, "^GE|^HP|^IBM", "HP", 1, },
                { L_, "^GE|^HP|^IBM", "abcHP", 0, },
                { L_, "^GE|^HP|^IBM", "HPxyz", 1, },
                { L_, "^GE|^HP|^IBM", "abcHPxyz", 0, },
                { L_, "^GE|^HP|^IBM", "IBM", 1, },
                { L_, "^GE|^HP|^IBM", "abcIBM", 0, },
                { L_, "^GE|^HP|^IBM", "IBMxyz", 1, },
                { L_, "^GE|^HP|^IBM", "abcIBMxyz", 0, },
                { L_, "^GE|HP$|^IBM", "GE", 1, },
                { L_, "^GE|HP$|^IBM", "abcGE", 0, },
                { L_, "^GE|HP$|^IBM", "GExyz", 1, },
                { L_, "^GE|HP$|^IBM", "abcGExyz", 0, },
                { L_, "^GE|HP$|^IBM", "HP", 1, },
                { L_, "^GE|HP$|^IBM", "abcHP", 1, },
                { L_, "^GE|HP$|^IBM", "HPxyz", 0, },
                { L_, "^GE|HP$|^IBM", "abcHPxyz", 0, },
                { L_, "^GE|HP$|^IBM", "IBM", 1, },
                { L_, "^GE|HP$|^IBM", "abcIBM", 0, },
                { L_, "^GE|HP$|^IBM", "IBMxyz", 1, },
                { L_, "^GE|HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "^GE|^HP$|^IBM", "GE", 1, },
                { L_, "^GE|^HP$|^IBM", "abcGE", 0, },
                { L_, "^GE|^HP$|^IBM", "GExyz", 1, },
                { L_, "^GE|^HP$|^IBM", "abcGExyz", 0, },
                { L_, "^GE|^HP$|^IBM", "HP", 1, },
                { L_, "^GE|^HP$|^IBM", "abcHP", 0, },
                { L_, "^GE|^HP$|^IBM", "HPxyz", 0, },
                { L_, "^GE|^HP$|^IBM", "abcHPxyz", 0, },
                { L_, "^GE|^HP$|^IBM", "IBM", 1, },
                { L_, "^GE|^HP$|^IBM", "abcIBM", 0, },
                { L_, "^GE|^HP$|^IBM", "IBMxyz", 1, },
                { L_, "^GE|^HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "^GE|HP|IBM$", "GE", 1, },
                { L_, "^GE|HP|IBM$", "abcGE", 0, },
                { L_, "^GE|HP|IBM$", "GExyz", 1, },
                { L_, "^GE|HP|IBM$", "abcGExyz", 0, },
                { L_, "^GE|HP|IBM$", "HP", 1, },
                { L_, "^GE|HP|IBM$", "abcHP", 1, },
                { L_, "^GE|HP|IBM$", "HPxyz", 1, },
                { L_, "^GE|HP|IBM$", "abcHPxyz", 1, },
                { L_, "^GE|HP|IBM$", "IBM", 1, },
                { L_, "^GE|HP|IBM$", "abcIBM", 1, },
                { L_, "^GE|HP|IBM$", "IBMxyz", 0, },
                { L_, "^GE|HP|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|^HP|IBM$", "GE", 1, },
                { L_, "^GE|^HP|IBM$", "abcGE", 0, },
                { L_, "^GE|^HP|IBM$", "GExyz", 1, },
                { L_, "^GE|^HP|IBM$", "abcGExyz", 0, },
                { L_, "^GE|^HP|IBM$", "HP", 1, },
                { L_, "^GE|^HP|IBM$", "abcHP", 0, },
                { L_, "^GE|^HP|IBM$", "HPxyz", 1, },
                { L_, "^GE|^HP|IBM$", "abcHPxyz", 0, },
                { L_, "^GE|^HP|IBM$", "IBM", 1, },
                { L_, "^GE|^HP|IBM$", "abcIBM", 1, },
                { L_, "^GE|^HP|IBM$", "IBMxyz", 0, },
                { L_, "^GE|^HP|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|HP$|IBM$", "GE", 1, },
                { L_, "^GE|HP$|IBM$", "abcGE", 0, },
                { L_, "^GE|HP$|IBM$", "GExyz", 1, },
                { L_, "^GE|HP$|IBM$", "abcGExyz", 0, },
                { L_, "^GE|HP$|IBM$", "HP", 1, },
                { L_, "^GE|HP$|IBM$", "abcHP", 1, },
                { L_, "^GE|HP$|IBM$", "HPxyz", 0, },
                { L_, "^GE|HP$|IBM$", "abcHPxyz", 0, },
                { L_, "^GE|HP$|IBM$", "IBM", 1, },
                { L_, "^GE|HP$|IBM$", "abcIBM", 1, },
                { L_, "^GE|HP$|IBM$", "IBMxyz", 0, },
                { L_, "^GE|HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|^HP$|IBM$", "GE", 1, },
                { L_, "^GE|^HP$|IBM$", "abcGE", 0, },
                { L_, "^GE|^HP$|IBM$", "GExyz", 1, },
                { L_, "^GE|^HP$|IBM$", "abcGExyz", 0, },
                { L_, "^GE|^HP$|IBM$", "HP", 1, },
                { L_, "^GE|^HP$|IBM$", "abcHP", 0, },
                { L_, "^GE|^HP$|IBM$", "HPxyz", 0, },
                { L_, "^GE|^HP$|IBM$", "abcHPxyz", 0, },
                { L_, "^GE|^HP$|IBM$", "IBM", 1, },
                { L_, "^GE|^HP$|IBM$", "abcIBM", 1, },
                { L_, "^GE|^HP$|IBM$", "IBMxyz", 0, },
                { L_, "^GE|^HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|HP|^IBM$", "GE", 1, },
                { L_, "^GE|HP|^IBM$", "abcGE", 0, },
                { L_, "^GE|HP|^IBM$", "GExyz", 1, },
                { L_, "^GE|HP|^IBM$", "abcGExyz", 0, },
                { L_, "^GE|HP|^IBM$", "HP", 1, },
                { L_, "^GE|HP|^IBM$", "abcHP", 1, },
                { L_, "^GE|HP|^IBM$", "HPxyz", 1, },
                { L_, "^GE|HP|^IBM$", "abcHPxyz", 1, },
                { L_, "^GE|HP|^IBM$", "IBM", 1, },
                { L_, "^GE|HP|^IBM$", "abcIBM", 0, },
                { L_, "^GE|HP|^IBM$", "IBMxyz", 0, },
                { L_, "^GE|HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|^HP|^IBM$", "GE", 1, },
                { L_, "^GE|^HP|^IBM$", "abcGE", 0, },
                { L_, "^GE|^HP|^IBM$", "GExyz", 1, },
                { L_, "^GE|^HP|^IBM$", "abcGExyz", 0, },
                { L_, "^GE|^HP|^IBM$", "HP", 1, },
                { L_, "^GE|^HP|^IBM$", "abcHP", 0, },
                { L_, "^GE|^HP|^IBM$", "HPxyz", 1, },
                { L_, "^GE|^HP|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE|^HP|^IBM$", "IBM", 1, },
                { L_, "^GE|^HP|^IBM$", "abcIBM", 0, },
                { L_, "^GE|^HP|^IBM$", "IBMxyz", 0, },
                { L_, "^GE|^HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|HP$|^IBM$", "GE", 1, },
                { L_, "^GE|HP$|^IBM$", "abcGE", 0, },
                { L_, "^GE|HP$|^IBM$", "GExyz", 1, },
                { L_, "^GE|HP$|^IBM$", "abcGExyz", 0, },
                { L_, "^GE|HP$|^IBM$", "HP", 1, },
                { L_, "^GE|HP$|^IBM$", "abcHP", 1, },
                { L_, "^GE|HP$|^IBM$", "HPxyz", 0, },
                { L_, "^GE|HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE|HP$|^IBM$", "IBM", 1, },
                { L_, "^GE|HP$|^IBM$", "abcIBM", 0, },
                { L_, "^GE|HP$|^IBM$", "IBMxyz", 0, },
                { L_, "^GE|HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE|^HP$|^IBM$", "GE", 1, },
                { L_, "^GE|^HP$|^IBM$", "abcGE", 0, },
                { L_, "^GE|^HP$|^IBM$", "GExyz", 1, },
                { L_, "^GE|^HP$|^IBM$", "abcGExyz", 0, },
                { L_, "^GE|^HP$|^IBM$", "HP", 1, },
                { L_, "^GE|^HP$|^IBM$", "abcHP", 0, },
                { L_, "^GE|^HP$|^IBM$", "HPxyz", 0, },
                { L_, "^GE|^HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE|^HP$|^IBM$", "IBM", 1, },
                { L_, "^GE|^HP$|^IBM$", "abcIBM", 0, },
                { L_, "^GE|^HP$|^IBM$", "IBMxyz", 0, },
                { L_, "^GE|^HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|HP|IBM", "GE", 1, },
                { L_, "GE$|HP|IBM", "abcGE", 1, },
                { L_, "GE$|HP|IBM", "GExyz", 0, },
                { L_, "GE$|HP|IBM", "abcGExyz", 0, },
                { L_, "GE$|HP|IBM", "HP", 1, },
                { L_, "GE$|HP|IBM", "abcHP", 1, },
                { L_, "GE$|HP|IBM", "HPxyz", 1, },
                { L_, "GE$|HP|IBM", "abcHPxyz", 1, },
                { L_, "GE$|HP|IBM", "IBM", 1, },
                { L_, "GE$|HP|IBM", "abcIBM", 1, },
                { L_, "GE$|HP|IBM", "IBMxyz", 1, },
                { L_, "GE$|HP|IBM", "abcIBMxyz", 1, },
                { L_, "GE$|^HP|IBM", "GE", 1, },
                { L_, "GE$|^HP|IBM", "abcGE", 1, },
                { L_, "GE$|^HP|IBM", "GExyz", 0, },
                { L_, "GE$|^HP|IBM", "abcGExyz", 0, },
                { L_, "GE$|^HP|IBM", "HP", 1, },
                { L_, "GE$|^HP|IBM", "abcHP", 0, },
                { L_, "GE$|^HP|IBM", "HPxyz", 1, },
                { L_, "GE$|^HP|IBM", "abcHPxyz", 0, },
                { L_, "GE$|^HP|IBM", "IBM", 1, },
                { L_, "GE$|^HP|IBM", "abcIBM", 1, },
                { L_, "GE$|^HP|IBM", "IBMxyz", 1, },
                { L_, "GE$|^HP|IBM", "abcIBMxyz", 1, },
                { L_, "GE$|HP$|IBM", "GE", 1, },
                { L_, "GE$|HP$|IBM", "abcGE", 1, },
                { L_, "GE$|HP$|IBM", "GExyz", 0, },
                { L_, "GE$|HP$|IBM", "abcGExyz", 0, },
                { L_, "GE$|HP$|IBM", "HP", 1, },
                { L_, "GE$|HP$|IBM", "abcHP", 1, },
                { L_, "GE$|HP$|IBM", "HPxyz", 0, },
                { L_, "GE$|HP$|IBM", "abcHPxyz", 0, },
                { L_, "GE$|HP$|IBM", "IBM", 1, },
                { L_, "GE$|HP$|IBM", "abcIBM", 1, },
                { L_, "GE$|HP$|IBM", "IBMxyz", 1, },
                { L_, "GE$|HP$|IBM", "abcIBMxyz", 1, },
                { L_, "GE$|^HP$|IBM", "GE", 1, },
                { L_, "GE$|^HP$|IBM", "abcGE", 1, },
                { L_, "GE$|^HP$|IBM", "GExyz", 0, },
                { L_, "GE$|^HP$|IBM", "abcGExyz", 0, },
                { L_, "GE$|^HP$|IBM", "HP", 1, },
                { L_, "GE$|^HP$|IBM", "abcHP", 0, },
                { L_, "GE$|^HP$|IBM", "HPxyz", 0, },
                { L_, "GE$|^HP$|IBM", "abcHPxyz", 0, },
                { L_, "GE$|^HP$|IBM", "IBM", 1, },
                { L_, "GE$|^HP$|IBM", "abcIBM", 1, },
                { L_, "GE$|^HP$|IBM", "IBMxyz", 1, },
                { L_, "GE$|^HP$|IBM", "abcIBMxyz", 1, },
                { L_, "GE$|HP|^IBM", "GE", 1, },
                { L_, "GE$|HP|^IBM", "abcGE", 1, },
                { L_, "GE$|HP|^IBM", "GExyz", 0, },
                { L_, "GE$|HP|^IBM", "abcGExyz", 0, },
                { L_, "GE$|HP|^IBM", "HP", 1, },
                { L_, "GE$|HP|^IBM", "abcHP", 1, },
                { L_, "GE$|HP|^IBM", "HPxyz", 1, },
                { L_, "GE$|HP|^IBM", "abcHPxyz", 1, },
                { L_, "GE$|HP|^IBM", "IBM", 1, },
                { L_, "GE$|HP|^IBM", "abcIBM", 0, },
                { L_, "GE$|HP|^IBM", "IBMxyz", 1, },
                { L_, "GE$|HP|^IBM", "abcIBMxyz", 0, },
                { L_, "GE$|^HP|^IBM", "GE", 1, },
                { L_, "GE$|^HP|^IBM", "abcGE", 1, },
                { L_, "GE$|^HP|^IBM", "GExyz", 0, },
                { L_, "GE$|^HP|^IBM", "abcGExyz", 0, },
                { L_, "GE$|^HP|^IBM", "HP", 1, },
                { L_, "GE$|^HP|^IBM", "abcHP", 0, },
                { L_, "GE$|^HP|^IBM", "HPxyz", 1, },
                { L_, "GE$|^HP|^IBM", "abcHPxyz", 0, },
                { L_, "GE$|^HP|^IBM", "IBM", 1, },
                { L_, "GE$|^HP|^IBM", "abcIBM", 0, },
                { L_, "GE$|^HP|^IBM", "IBMxyz", 1, },
                { L_, "GE$|^HP|^IBM", "abcIBMxyz", 0, },
                { L_, "GE$|HP$|^IBM", "GE", 1, },
                { L_, "GE$|HP$|^IBM", "abcGE", 1, },
                { L_, "GE$|HP$|^IBM", "GExyz", 0, },
                { L_, "GE$|HP$|^IBM", "abcGExyz", 0, },
                { L_, "GE$|HP$|^IBM", "HP", 1, },
                { L_, "GE$|HP$|^IBM", "abcHP", 1, },
                { L_, "GE$|HP$|^IBM", "HPxyz", 0, },
                { L_, "GE$|HP$|^IBM", "abcHPxyz", 0, },
                { L_, "GE$|HP$|^IBM", "IBM", 1, },
                { L_, "GE$|HP$|^IBM", "abcIBM", 0, },
                { L_, "GE$|HP$|^IBM", "IBMxyz", 1, },
                { L_, "GE$|HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "GE$|^HP$|^IBM", "GE", 1, },
                { L_, "GE$|^HP$|^IBM", "abcGE", 1, },
                { L_, "GE$|^HP$|^IBM", "GExyz", 0, },
                { L_, "GE$|^HP$|^IBM", "abcGExyz", 0, },
                { L_, "GE$|^HP$|^IBM", "HP", 1, },
                { L_, "GE$|^HP$|^IBM", "abcHP", 0, },
                { L_, "GE$|^HP$|^IBM", "HPxyz", 0, },
                { L_, "GE$|^HP$|^IBM", "abcHPxyz", 0, },
                { L_, "GE$|^HP$|^IBM", "IBM", 1, },
                { L_, "GE$|^HP$|^IBM", "abcIBM", 0, },
                { L_, "GE$|^HP$|^IBM", "IBMxyz", 1, },
                { L_, "GE$|^HP$|^IBM", "abcIBMxyz", 0, },
                { L_, "GE$|HP|IBM$", "GE", 1, },
                { L_, "GE$|HP|IBM$", "abcGE", 1, },
                { L_, "GE$|HP|IBM$", "GExyz", 0, },
                { L_, "GE$|HP|IBM$", "abcGExyz", 0, },
                { L_, "GE$|HP|IBM$", "HP", 1, },
                { L_, "GE$|HP|IBM$", "abcHP", 1, },
                { L_, "GE$|HP|IBM$", "HPxyz", 1, },
                { L_, "GE$|HP|IBM$", "abcHPxyz", 1, },
                { L_, "GE$|HP|IBM$", "IBM", 1, },
                { L_, "GE$|HP|IBM$", "abcIBM", 1, },
                { L_, "GE$|HP|IBM$", "IBMxyz", 0, },
                { L_, "GE$|HP|IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|^HP|IBM$", "GE", 1, },
                { L_, "GE$|^HP|IBM$", "abcGE", 1, },
                { L_, "GE$|^HP|IBM$", "GExyz", 0, },
                { L_, "GE$|^HP|IBM$", "abcGExyz", 0, },
                { L_, "GE$|^HP|IBM$", "HP", 1, },
                { L_, "GE$|^HP|IBM$", "abcHP", 0, },
                { L_, "GE$|^HP|IBM$", "HPxyz", 1, },
                { L_, "GE$|^HP|IBM$", "abcHPxyz", 0, },
                { L_, "GE$|^HP|IBM$", "IBM", 1, },
                { L_, "GE$|^HP|IBM$", "abcIBM", 1, },
                { L_, "GE$|^HP|IBM$", "IBMxyz", 0, },
                { L_, "GE$|^HP|IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|HP$|IBM$", "GE", 1, },
                { L_, "GE$|HP$|IBM$", "abcGE", 1, },
                { L_, "GE$|HP$|IBM$", "GExyz", 0, },
                { L_, "GE$|HP$|IBM$", "abcGExyz", 0, },
                { L_, "GE$|HP$|IBM$", "HP", 1, },
                { L_, "GE$|HP$|IBM$", "abcHP", 1, },
                { L_, "GE$|HP$|IBM$", "HPxyz", 0, },
                { L_, "GE$|HP$|IBM$", "abcHPxyz", 0, },
                { L_, "GE$|HP$|IBM$", "IBM", 1, },
                { L_, "GE$|HP$|IBM$", "abcIBM", 1, },
                { L_, "GE$|HP$|IBM$", "IBMxyz", 0, },
                { L_, "GE$|HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|^HP$|IBM$", "GE", 1, },
                { L_, "GE$|^HP$|IBM$", "abcGE", 1, },
                { L_, "GE$|^HP$|IBM$", "GExyz", 0, },
                { L_, "GE$|^HP$|IBM$", "abcGExyz", 0, },
                { L_, "GE$|^HP$|IBM$", "HP", 1, },
                { L_, "GE$|^HP$|IBM$", "abcHP", 0, },
                { L_, "GE$|^HP$|IBM$", "HPxyz", 0, },
                { L_, "GE$|^HP$|IBM$", "abcHPxyz", 0, },
                { L_, "GE$|^HP$|IBM$", "IBM", 1, },
                { L_, "GE$|^HP$|IBM$", "abcIBM", 1, },
                { L_, "GE$|^HP$|IBM$", "IBMxyz", 0, },
                { L_, "GE$|^HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|HP|^IBM$", "GE", 1, },
                { L_, "GE$|HP|^IBM$", "abcGE", 1, },
                { L_, "GE$|HP|^IBM$", "GExyz", 0, },
                { L_, "GE$|HP|^IBM$", "abcGExyz", 0, },
                { L_, "GE$|HP|^IBM$", "HP", 1, },
                { L_, "GE$|HP|^IBM$", "abcHP", 1, },
                { L_, "GE$|HP|^IBM$", "HPxyz", 1, },
                { L_, "GE$|HP|^IBM$", "abcHPxyz", 1, },
                { L_, "GE$|HP|^IBM$", "IBM", 1, },
                { L_, "GE$|HP|^IBM$", "abcIBM", 0, },
                { L_, "GE$|HP|^IBM$", "IBMxyz", 0, },
                { L_, "GE$|HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|^HP|^IBM$", "GE", 1, },
                { L_, "GE$|^HP|^IBM$", "abcGE", 1, },
                { L_, "GE$|^HP|^IBM$", "GExyz", 0, },
                { L_, "GE$|^HP|^IBM$", "abcGExyz", 0, },
                { L_, "GE$|^HP|^IBM$", "HP", 1, },
                { L_, "GE$|^HP|^IBM$", "abcHP", 0, },
                { L_, "GE$|^HP|^IBM$", "HPxyz", 1, },
                { L_, "GE$|^HP|^IBM$", "abcHPxyz", 0, },
                { L_, "GE$|^HP|^IBM$", "IBM", 1, },
                { L_, "GE$|^HP|^IBM$", "abcIBM", 0, },
                { L_, "GE$|^HP|^IBM$", "IBMxyz", 0, },
                { L_, "GE$|^HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|HP$|^IBM$", "GE", 1, },
                { L_, "GE$|HP$|^IBM$", "abcGE", 1, },
                { L_, "GE$|HP$|^IBM$", "GExyz", 0, },
                { L_, "GE$|HP$|^IBM$", "abcGExyz", 0, },
                { L_, "GE$|HP$|^IBM$", "HP", 1, },
                { L_, "GE$|HP$|^IBM$", "abcHP", 1, },
                { L_, "GE$|HP$|^IBM$", "HPxyz", 0, },
                { L_, "GE$|HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "GE$|HP$|^IBM$", "IBM", 1, },
                { L_, "GE$|HP$|^IBM$", "abcIBM", 0, },
                { L_, "GE$|HP$|^IBM$", "IBMxyz", 0, },
                { L_, "GE$|HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "GE", 1, },
                { L_, "GE$|^HP$|^IBM$", "abcGE", 1, },
                { L_, "GE$|^HP$|^IBM$", "GExyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "abcGExyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "HP", 1, },
                { L_, "GE$|^HP$|^IBM$", "abcHP", 0, },
                { L_, "GE$|^HP$|^IBM$", "HPxyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "IBM", 1, },
                { L_, "GE$|^HP$|^IBM$", "abcIBM", 0, },
                { L_, "GE$|^HP$|^IBM$", "IBMxyz", 0, },
                { L_, "GE$|^HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|HP|IBM", "GE", 1, },
                { L_, "^GE$|HP|IBM", "abcGE", 0, },
                { L_, "^GE$|HP|IBM", "GExyz", 0, },
                { L_, "^GE$|HP|IBM", "abcGExyz", 0, },
                { L_, "^GE$|HP|IBM", "HP", 1, },
                { L_, "^GE$|HP|IBM", "abcHP", 1, },
                { L_, "^GE$|HP|IBM", "HPxyz", 1, },
                { L_, "^GE$|HP|IBM", "abcHPxyz", 1, },
                { L_, "^GE$|HP|IBM", "IBM", 1, },
                { L_, "^GE$|HP|IBM", "abcIBM", 1, },
                { L_, "^GE$|HP|IBM", "IBMxyz", 1, },
                { L_, "^GE$|HP|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|^HP|IBM", "GE", 1, },
                { L_, "^GE$|^HP|IBM", "abcGE", 0, },
                { L_, "^GE$|^HP|IBM", "GExyz", 0, },
                { L_, "^GE$|^HP|IBM", "abcGExyz", 0, },
                { L_, "^GE$|^HP|IBM", "HP", 1, },
                { L_, "^GE$|^HP|IBM", "abcHP", 0, },
                { L_, "^GE$|^HP|IBM", "HPxyz", 1, },
                { L_, "^GE$|^HP|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|^HP|IBM", "IBM", 1, },
                { L_, "^GE$|^HP|IBM", "abcIBM", 1, },
                { L_, "^GE$|^HP|IBM", "IBMxyz", 1, },
                { L_, "^GE$|^HP|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|HP$|IBM", "GE", 1, },
                { L_, "^GE$|HP$|IBM", "abcGE", 0, },
                { L_, "^GE$|HP$|IBM", "GExyz", 0, },
                { L_, "^GE$|HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE$|HP$|IBM", "HP", 1, },
                { L_, "^GE$|HP$|IBM", "abcHP", 1, },
                { L_, "^GE$|HP$|IBM", "HPxyz", 0, },
                { L_, "^GE$|HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|HP$|IBM", "IBM", 1, },
                { L_, "^GE$|HP$|IBM", "abcIBM", 1, },
                { L_, "^GE$|HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE$|HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|^HP$|IBM", "GE", 1, },
                { L_, "^GE$|^HP$|IBM", "abcGE", 0, },
                { L_, "^GE$|^HP$|IBM", "GExyz", 0, },
                { L_, "^GE$|^HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE$|^HP$|IBM", "HP", 1, },
                { L_, "^GE$|^HP$|IBM", "abcHP", 0, },
                { L_, "^GE$|^HP$|IBM", "HPxyz", 0, },
                { L_, "^GE$|^HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|^HP$|IBM", "IBM", 1, },
                { L_, "^GE$|^HP$|IBM", "abcIBM", 1, },
                { L_, "^GE$|^HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE$|^HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|HP|^IBM", "GE", 1, },
                { L_, "^GE$|HP|^IBM", "abcGE", 0, },
                { L_, "^GE$|HP|^IBM", "GExyz", 0, },
                { L_, "^GE$|HP|^IBM", "abcGExyz", 0, },
                { L_, "^GE$|HP|^IBM", "HP", 1, },
                { L_, "^GE$|HP|^IBM", "abcHP", 1, },
                { L_, "^GE$|HP|^IBM", "HPxyz", 1, },
                { L_, "^GE$|HP|^IBM", "abcHPxyz", 1, },
                { L_, "^GE$|HP|^IBM", "IBM", 1, },
                { L_, "^GE$|HP|^IBM", "abcIBM", 0, },
                { L_, "^GE$|HP|^IBM", "IBMxyz", 1, },
                { L_, "^GE$|HP|^IBM", "abcIBMxyz", 0, },
                { L_, "^GE$|^HP|IBM", "GE", 1, },
                { L_, "^GE$|^HP|IBM", "abcGE", 0, },
                { L_, "^GE$|^HP|IBM", "GExyz", 0, },
                { L_, "^GE$|^HP|IBM", "abcGExyz", 0, },
                { L_, "^GE$|^HP|IBM", "HP", 1, },
                { L_, "^GE$|^HP|IBM", "abcHP", 0, },
                { L_, "^GE$|^HP|IBM", "HPxyz", 1, },
                { L_, "^GE$|^HP|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|^HP|IBM", "IBM", 1, },
                { L_, "^GE$|^HP|IBM", "abcIBM", 1, },
                { L_, "^GE$|^HP|IBM", "IBMxyz", 1, },
                { L_, "^GE$|^HP|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|HP$|IBM", "GE", 1, },
                { L_, "^GE$|HP$|IBM", "abcGE", 0, },
                { L_, "^GE$|HP$|IBM", "GExyz", 0, },
                { L_, "^GE$|HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE$|HP$|IBM", "HP", 1, },
                { L_, "^GE$|HP$|IBM", "abcHP", 1, },
                { L_, "^GE$|HP$|IBM", "HPxyz", 0, },
                { L_, "^GE$|HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|HP$|IBM", "IBM", 1, },
                { L_, "^GE$|HP$|IBM", "abcIBM", 1, },
                { L_, "^GE$|HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE$|HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|^HP$|IBM", "GE", 1, },
                { L_, "^GE$|^HP$|IBM", "abcGE", 0, },
                { L_, "^GE$|^HP$|IBM", "GExyz", 0, },
                { L_, "^GE$|^HP$|IBM", "abcGExyz", 0, },
                { L_, "^GE$|^HP$|IBM", "HP", 1, },
                { L_, "^GE$|^HP$|IBM", "abcHP", 0, },
                { L_, "^GE$|^HP$|IBM", "HPxyz", 0, },
                { L_, "^GE$|^HP$|IBM", "abcHPxyz", 0, },
                { L_, "^GE$|^HP$|IBM", "IBM", 1, },
                { L_, "^GE$|^HP$|IBM", "abcIBM", 1, },
                { L_, "^GE$|^HP$|IBM", "IBMxyz", 1, },
                { L_, "^GE$|^HP$|IBM", "abcIBMxyz", 1, },
                { L_, "^GE$|HP|IBM$", "GE", 1, },
                { L_, "^GE$|HP|IBM$", "abcGE", 0, },
                { L_, "^GE$|HP|IBM$", "GExyz", 0, },
                { L_, "^GE$|HP|IBM$", "abcGExyz", 0, },
                { L_, "^GE$|HP|IBM$", "HP", 1, },
                { L_, "^GE$|HP|IBM$", "abcHP", 1, },
                { L_, "^GE$|HP|IBM$", "HPxyz", 1, },
                { L_, "^GE$|HP|IBM$", "abcHPxyz", 1, },
                { L_, "^GE$|HP|IBM$", "IBM", 1, },
                { L_, "^GE$|HP|IBM$", "abcIBM", 1, },
                { L_, "^GE$|HP|IBM$", "IBMxyz", 0, },
                { L_, "^GE$|HP|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|^HP|IBM$", "GE", 1, },
                { L_, "^GE$|^HP|IBM$", "abcGE", 0, },
                { L_, "^GE$|^HP|IBM$", "GExyz", 0, },
                { L_, "^GE$|^HP|IBM$", "abcGExyz", 0, },
                { L_, "^GE$|^HP|IBM$", "HP", 1, },
                { L_, "^GE$|^HP|IBM$", "abcHP", 0, },
                { L_, "^GE$|^HP|IBM$", "HPxyz", 1, },
                { L_, "^GE$|^HP|IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|^HP|IBM$", "IBM", 1, },
                { L_, "^GE$|^HP|IBM$", "abcIBM", 1, },
                { L_, "^GE$|^HP|IBM$", "IBMxyz", 0, },
                { L_, "^GE$|^HP|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|HP$|IBM$", "GE", 1, },
                { L_, "^GE$|HP$|IBM$", "abcGE", 0, },
                { L_, "^GE$|HP$|IBM$", "GExyz", 0, },
                { L_, "^GE$|HP$|IBM$", "abcGExyz", 0, },
                { L_, "^GE$|HP$|IBM$", "HP", 1, },
                { L_, "^GE$|HP$|IBM$", "abcHP", 1, },
                { L_, "^GE$|HP$|IBM$", "HPxyz", 0, },
                { L_, "^GE$|HP$|IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|HP$|IBM$", "IBM", 1, },
                { L_, "^GE$|HP$|IBM$", "abcIBM", 1, },
                { L_, "^GE$|HP$|IBM$", "IBMxyz", 0, },
                { L_, "^GE$|HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "GE", 1, },
                { L_, "^GE$|^HP$|IBM$", "abcGE", 0, },
                { L_, "^GE$|^HP$|IBM$", "GExyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "abcGExyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "HP", 1, },
                { L_, "^GE$|^HP$|IBM$", "abcHP", 0, },
                { L_, "^GE$|^HP$|IBM$", "HPxyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "IBM", 1, },
                { L_, "^GE$|^HP$|IBM$", "abcIBM", 1, },
                { L_, "^GE$|^HP$|IBM$", "IBMxyz", 0, },
                { L_, "^GE$|^HP$|IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|HP|^IBM$", "GE", 1, },
                { L_, "^GE$|HP|^IBM$", "abcGE", 0, },
                { L_, "^GE$|HP|^IBM$", "GExyz", 0, },
                { L_, "^GE$|HP|^IBM$", "abcGExyz", 0, },
                { L_, "^GE$|HP|^IBM$", "HP", 1, },
                { L_, "^GE$|HP|^IBM$", "abcHP", 1, },
                { L_, "^GE$|HP|^IBM$", "HPxyz", 1, },
                { L_, "^GE$|HP|^IBM$", "abcHPxyz", 1, },
                { L_, "^GE$|HP|^IBM$", "IBM", 1, },
                { L_, "^GE$|HP|^IBM$", "abcIBM", 0, },
                { L_, "^GE$|HP|^IBM$", "IBMxyz", 0, },
                { L_, "^GE$|HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|^HP|^IBM$", "GE", 1, },
                { L_, "^GE$|^HP|^IBM$", "abcGE", 0, },
                { L_, "^GE$|^HP|^IBM$", "GExyz", 0, },
                { L_, "^GE$|^HP|^IBM$", "abcGExyz", 0, },
                { L_, "^GE$|^HP|^IBM$", "HP", 1, },
                { L_, "^GE$|^HP|^IBM$", "abcHP", 0, },
                { L_, "^GE$|^HP|^IBM$", "HPxyz", 1, },
                { L_, "^GE$|^HP|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|^HP|^IBM$", "IBM", 1, },
                { L_, "^GE$|^HP|^IBM$", "abcIBM", 0, },
                { L_, "^GE$|^HP|^IBM$", "IBMxyz", 0, },
                { L_, "^GE$|^HP|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "GE", 1, },
                { L_, "^GE$|HP$|^IBM$", "abcGE", 0, },
                { L_, "^GE$|HP$|^IBM$", "GExyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "abcGExyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "HP", 1, },
                { L_, "^GE$|HP$|^IBM$", "abcHP", 1, },
                { L_, "^GE$|HP$|^IBM$", "HPxyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "IBM", 1, },
                { L_, "^GE$|HP$|^IBM$", "abcIBM", 0, },
                { L_, "^GE$|HP$|^IBM$", "IBMxyz", 0, },
                { L_, "^GE$|HP$|^IBM$", "abcIBMxyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "GE", 1, },
                { L_, "^GE$|^HP$|^IBM$", "abcGE", 0, },
                { L_, "^GE$|^HP$|^IBM$", "GExyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "abcGExyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "HP", 1, },
                { L_, "^GE$|^HP$|^IBM$", "abcHP", 0, },
                { L_, "^GE$|^HP$|^IBM$", "HPxyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "abcHPxyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "IBM", 1, },
                { L_, "^GE$|^HP$|^IBM$", "abcIBM", 0, },
                { L_, "^GE$|^HP$|^IBM$", "IBMxyz", 0, },
                { L_, "^GE$|^HP$|^IBM$", "abcIBMxyz", 0, }
*/
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *RE   = DATA[ti].d_re;
                const char *S    = DATA[ti].d_str;
                const int   E    = DATA[ti].d_expect;
                if (veryVerbose) { cout << "\t\t"; P_(RE); P_(S); P(E); }

                Obj re(RE, &testAllocator);
                LOOP_ASSERT(LINE, E == re.isMatch(S, strlen(S)));
            }
        }
      } break;
      case 12: {
        if (verbose) cout << endl
            << "Tests provided by Manny Farber" << endl
            << "==============================" << endl;

        static const struct {
                int         d_lineNum;  // source line number
                const char *d_re;       // regular expression
                const char *d_str;      // string to compare against r.e.
                int         d_expect;   // 1 if should match; 0 otherwise
        } DATA[] = {
                //line  r.e.   test string  matches?
                //----  ----   -----------  --------

                // basics
                { L_, "a", "a", 1 },
                { L_, "abc", "abc", 1 },
                { L_, "abc|de", "abc", 1 },
                { L_, "a|b|c", "abc", 1 },

                // parentheses and perversions thereof
                { L_, "a(b)c", "abc", 1 },
                { L_, "a\\(", "a(", 1 },
                { L_, "a)", "a)", 0 },
                { L_, ")", ")", 0 },
                { L_, "a()b", "ab", 1 },

                // anchoring
                { L_, "^abc$", "abc", 1 },
                { L_, "abc$", "abc", 1 },
                { L_, "abc$", "abcd", 0 },
                { L_, "a^b", "a^b", 0 },
                { L_, "a$b", "a$b", 0 },
                { L_, "^", "abc", 1 },
                { L_, "$", "abc", 1 },
                { L_, "^$", "", 1 },
                { L_, "^$", "a", 0 },
                { L_, "$^", "", 1 },
                { L_, "^^", "", 1 },
                { L_, "$$", "", 1 },
                { L_, "b$", "ab\nc", 0 },
                { L_, "^b$", "a\nb\nc", 0 },
                { L_, "a*(^b$)c*", "b", 1 },

                // certain syntax errors and non-errors
                { L_, "", "abc", 1 },
                { L_, "()", "abc", 1 },

                // metacharacters,  backslashes
                { L_, "a.c", "abc", 1 },
                { L_, "a[bc]d", "abd", 1 },
                { L_, "a\\*c", "a*c", 1 },
                { L_, "a\\\\b", "a\\b", 1 },
                { L_, "a\\\\\\*b", "a\\*b", 1 },
                { L_, "a\\bc", "abc", 1 },
                { L_, "a\\\\bc", "a\\bc", 1 },
                { L_, "a\\[b", "a[b", 1 },
                { L_, "a$", "a", 1 },
                { L_, "a$", "a$", 0 },
                { L_, "a\\$", "a", 0 },
                { L_, "a\\$", "a$", 1 },
                { L_, "a\\\\$", "a", 0 },
                { L_, "a\\\\$", "a$", 0 },
                { L_, "a\\\\$", "a\\$", 0 },
                { L_, "a\\\\$", "a\\", 1 },

                // ordinary repetition
                { L_, "ab*c", "abc", 1 },
                { L_, "ab+c", "abc", 1 },
                { L_, "ab?c", "abc", 1 },

                // the dreaded bounded repetitions
                { L_, "{", "{", 0 },
                { L_, "{abc", "{abc", 0 },
                { L_, "a{b", "a{b", 0 },
                { L_, "a{1}b", "ab", 1 },
                { L_, "a{1, }b", "ab", 1 },
                { L_, "a{1, 2}b", "aab", 1 },
                { L_, "a{, 2}", "a{, 2}", 1 },
                { L_, "a{, }", "a{, }", 1 },
                { L_, "ab{0, 0}c", "abcac", 1 },
                { L_, "ab{0, 0}c", "abc", 0 },
                { L_, "ab{0, 1}c", "abcac", 1 },
                { L_, "ab{0, 3}c", "abbcac", 1 },
                { L_, "ab{1, 3}c", "abbcac", 1 },
                { L_, "ab{1, 3}c", "abbbbcac", 0 },
                { L_, "ab{1, 1}c", "acabc", 1 },
                { L_, "ab{1, 3}c", "acabc", 1 },
                { L_, "ab{2, 2}c", "abcabbc", 1 },
                { L_, "ab{2, 4}c", "abcabbc", 1 },
                { L_, "(a{1, 10}){1, 10}", "a", 1 },
// TBD SLOW!!!!
//                { L_, "((a{1, 10}){1, 10}){1, 10}", "a", 1 },

                // multiple repetitions
                { L_, "a*{b}", "a{b}", 1 },

                // brackets,  and numerous perversions thereof
                { L_, "a[b]c", "abc", 1 },
                { L_, "a[ab]c", "abc", 1 },
                { L_, "a[^ab]c", "adc", 1 },
                { L_, "a[^ab]c", "abc", 0 },
                { L_, "a[]b]c", "a]c", 1 },
                { L_, "a[[b]c", "a[c", 1 },
                { L_, "a[]b]c", "abc", 1 },
                { L_, "a[[b]c", "abc", 1 },
                { L_, "a[]b]c", "a[c", 0 },
                { L_, "a[[b]c", "a]c", 0 },
                { L_, "a[-b]c", "a-c", 1 },
                { L_, "a[^]b]c", "adc", 1 },
                { L_, "a[^-b]c", "adc", 1 },
                { L_, "a[b-]c", "a-c", 1 },
                { L_, "a[1-3]c", "a2c", 1 },

                // complexities
                { L_, "a(((b)))c", "abc", 1 },
                { L_, "a(b|(c))d", "abd", 1 },
                { L_, "a(b*|c)d", "abbd", 1 },

                // just gotta have one DFA-buster,  of course
// TBD SLOW!!!!
//                { L_, "a[ab]{20}", "aaaaabaaaabaaaabaaaab", 1 },

                // and an inline expansion in case somebody gets tricky
// TBD SLOW!!!!
//                { L_, "a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
//               "[ab][ab][ab][ab][ab][ab][ab]", "aaaaabaaaabaaaabaaaab", 1 },

                // and in case somebody just slips in an NFA...
// TBD SLOW!!!!
//                { L_, "a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]"
//"[ab][ab][ab][ab][ab][ab][ab](wee|week)(knights|night)",
// "aaaaabaaaabaaaabaaaabweeknights", 1 },

                // fish for anomalies as the number of states passes 32
                { L_, "12345678901234567890123456789",
                      "a12345678901234567890123456789b", 1 },
                { L_, "123456789012345678901234567890",
                      "a123456789012345678901234567890b", 1 },
                { L_, "1234567890123456789012345678901",
                      "a1234567890123456789012345678901b", 1 },
                { L_, "12345678901234567890123456789012",
                      "a12345678901234567890123456789012b", 1 },
                { L_, "123456789012345678901234567890123",
                      "a123456789012345678901234567890123b", 1 },

                // and one really big one,  beyond any plausible word width
                { L_,
 "1234567890123456789012345678901234567890123456789012345678901234567890",
 "a1234567890123456789012345678901234567890123456789012345678901234567890b",
 1 },

                // fish for problems as brackets go past 8
                { L_, "[ab][cd][ef][gh][ij][kl][mn]", "xacegikmoq", 1 },
                { L_, "[ab][cd][ef][gh][ij][kl][mn][op]", "xacegikmoq", 1 },
                { L_, "[ab][cd][ef][gh][ij][kl][mn][op][qr]",
                      "xacegikmoqy", 1 },
                { L_, "[ab][cd][ef][gh][ij][kl][mn][op][q]",
                      "xacegikmoqy", 1 },

                // subtleties of matching
                { L_, "abc", "xabcy", 1 },
                { L_, "[a]b[c]", "abc", 1 },
                { L_, "[a]b[a]", "aba", 1 },
                { L_, "[abc]b[abc]", "abc", 1 },
                { L_, "[abc]b[abd]", "abd", 1 },
                { L_, "a(b?c)+d", "accd", 1 },
                { L_, "(wee|week)(knights|night)", "weeknights", 1 },
                { L_, "(we|wee|week|frob)(knights|night|day)",
                      "weeknights", 1 },
                { L_, "a[bc]d", "xyzaaabcaababdacd", 1 },
                { L_, "a[ab]c", "aaabc", 1 },
                { L_, "a*", "b", 1 }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *RE   = DATA[ti].d_re;
                const char *S    = DATA[ti].d_str;
                const int   E    = DATA[ti].d_expect;
                if (veryVerbose) { cout << "\t\t"; P_(RE); P_(S); P(E); }

                Obj re(RE, &testAllocator);
                LOOP_ASSERT(LINE, E == re.isMatch(S, strlen(S)));
            }
        }
      } break;
      case 11: {
        {
            Obj r("a", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(0 == r.isMatch("b", 1));
        }
        {
            Obj r("^a$", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(0 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("ab", 2));
        }
        {
            Obj r(".", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("b", 1));
            ASSERT(1 == r.isMatch("-", 1));
            ASSERT(1 == r.isMatch("$", 1));
            ASSERT(1 == r.isMatch("^", 1));
        }
        {
            Obj r("ab", &testAllocator);

            ASSERT(1 == r.isMatch("ab", 2));
            ASSERT(0 == r.isMatch("a", 1));
            ASSERT(0 == r.isMatch("b", 1));
        }
        {
            Obj r("[_a-z-]", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("b", 1));
            ASSERT(1 == r.isMatch("y", 1));
            ASSERT(1 == r.isMatch("z", 1));
            ASSERT(1 == r.isMatch("_", 1));
            ASSERT(1 == r.isMatch("-", 1));
            ASSERT(0 == r.isMatch("M", 1));
            ASSERT(0 == r.isMatch("Z", 1));
            ASSERT(0 == r.isMatch("?", 1));
        }
        {
            Obj r("a*", &testAllocator);

            ASSERT(1 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("aa", 1));
            ASSERT(1 == r.isMatch("aaaaaaa", 7));
            ASSERT(1 == r.isMatch("b", 1));
        }
        {
            Obj r("ba*", &testAllocator);

            ASSERT(0 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("b", 1));
            ASSERT(1 == r.isMatch("ba", 2));
            ASSERT(1 == r.isMatch("baa", 3));
            ASSERT(1 == r.isMatch("baaaaaaa", 8));
            ASSERT(0 == r.isMatch("c", 1));
        }
        {
            Obj r("a|z", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("z", 1));
            ASSERT(0 == r.isMatch("x", 1));
            ASSERT(0 == r.isMatch("", 0));
        }
        {
            Obj r("(abc)|(xyz)", &testAllocator);

            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("ab", 2));
            ASSERT(0 == r.isMatch("bc", 2));
            ASSERT(1 == r.isMatch("xyz", 3));
            ASSERT(0 == r.isMatch("yz", 2));
            ASSERT(0 == r.isMatch("xy", 2));
        }
        {
            Obj r("(abc)|(xyz)|(1|2)", &testAllocator);

            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("ab", 2));
            ASSERT(0 == r.isMatch("bc", 2));
            ASSERT(1 == r.isMatch("xyz", 3));
            ASSERT(0 == r.isMatch("yz", 2));
            ASSERT(0 == r.isMatch("xy", 2));
            ASSERT(1 == r.isMatch("1", 1));
            ASSERT(1 == r.isMatch("2", 1));
            ASSERT(1 == r.isMatch("x1", 2));
            ASSERT(1 == r.isMatch("b2", 2));
            ASSERT(1 == r.isMatch("1y", 2));
        }
        {
            Obj r("(abc)|(xyz)|(1|2)*", &testAllocator);

            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(1 == r.isMatch("abcabc", 6));
            ASSERT(1 == r.isMatch("ab", 2));
            ASSERT(1 == r.isMatch("bc", 2));
            ASSERT(1 == r.isMatch("xyz", 3));
            ASSERT(1 == r.isMatch("xyzxyz", 6));
            ASSERT(1 == r.isMatch("yz", 2));
            ASSERT(1 == r.isMatch("xy", 2));
            ASSERT(1 == r.isMatch("111", 3));
            ASSERT(1 == r.isMatch("12121", 5));
            ASSERT(1 == r.isMatch("", 0));
            ASSERT(1 == r.isMatch("x1", 2));
            ASSERT(1 == r.isMatch("b2", 2));
            ASSERT(1 == r.isMatch("1y", 2));
        }
      } break;
      case 10: {
        {
            Obj r("*", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("+", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("?", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("{6}", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("^", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("$", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r(".", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("-", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("|", &testAllocator);
            ASSERT(r.isValid());
        }
        {
            Obj r("(", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("(a", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("([abc]", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r(")", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("())", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("[", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("[^-", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("[]", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("[^]", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("[]ab", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("]", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{6", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{24, ", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{7,6,5", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{}", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("{}ab", &testAllocator);
            ASSERT(!r.isValid());
        }
        {
            Obj r("}", &testAllocator);
            ASSERT(!r.isValid());
        }
      } break;
      case 9: {
        Obj r("^.$", &testAllocator);

        ASSERT(1 == r.isMatch("a", 1));
        ASSERT(1 == r.isMatch("b", 1));
        ASSERT(0 == r.isMatch("\n", 1));
      } break;
      case 8: {
        Obj r("a\\$", &testAllocator);

        ASSERT(0 == r.isMatch("a", 1));
        ASSERT(0 == r.isMatch("b", 1));

        ASSERT(0 == r.isMatch("aa", 2));
        ASSERT(0 == r.isMatch("ba", 2));
        ASSERT(0 == r.isMatch("ab", 2));
        ASSERT(0 == r.isMatch("bb", 2));

        ASSERT(0 == r.isMatch("aaa", 3));
        ASSERT(0 == r.isMatch("aba", 3));
        ASSERT(0 == r.isMatch("baa", 3));
        ASSERT(0 == r.isMatch("bba", 3));
        ASSERT(0 == r.isMatch("aab", 3));
        ASSERT(0 == r.isMatch("abb", 3));
        ASSERT(0 == r.isMatch("bab", 3));
        ASSERT(0 == r.isMatch("bbb", 3));

        ASSERT(1 == r.isMatch("a$", 2));
        ASSERT(0 == r.isMatch("b$", 2));

        ASSERT(1 == r.isMatch("aa$", 3));
        ASSERT(1 == r.isMatch("ba$", 3));
        ASSERT(0 == r.isMatch("ab$", 3));
        ASSERT(0 == r.isMatch("bb$", 3));

        ASSERT(1 == r.isMatch("aaa$", 4));
        ASSERT(1 == r.isMatch("aba$", 4));
        ASSERT(1 == r.isMatch("baa$", 4));
        ASSERT(1 == r.isMatch("bba$", 4));
        ASSERT(0 == r.isMatch("aab$", 4));
        ASSERT(0 == r.isMatch("abb$", 4));
        ASSERT(0 == r.isMatch("bab$", 4));
        ASSERT(0 == r.isMatch("bbb$", 4));
      } break;
      case 7: {
        {
            Obj r("[ab]", &testAllocator);

            ASSERT(1 == r.isMatch("a", 1));
            ASSERT(1 == r.isMatch("b", 1));
            ASSERT(0 == r.isMatch("c", 1));

            ASSERT(1 == r.isMatch("aa", 2));
            ASSERT(1 == r.isMatch("ab", 2));
            ASSERT(1 == r.isMatch("ba", 2));
            ASSERT(1 == r.isMatch("bb", 2));

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));

            ASSERT(1 == r.isMatch("aaaac", 5));
            ASSERT(1 == r.isMatch("aaccc", 5));
            ASSERT(1 == r.isMatch("aaabc", 5));
            ASSERT(1 == r.isMatch("abccc", 5));

            ASSERT(1 == r.isMatch("aaaaccc", 7));
            ASSERT(1 == r.isMatch("aaabccc", 7));
        }
        {
            Obj r("a[ab]", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("ac", 2));

            ASSERT(1 == r.isMatch("aaaac", 5));
            ASSERT(1 == r.isMatch("aaccc", 5));
            ASSERT(1 == r.isMatch("aaabc", 5));
            ASSERT(1 == r.isMatch("abccc", 5));

            ASSERT(1 == r.isMatch("aaaaccc", 7));
            ASSERT(1 == r.isMatch("aaabccc", 7));
        }
        {
            Obj r("[ab]c", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(1 == r.isMatch("bac", 3));
            ASSERT(1 == r.isMatch("bbc", 3));
            ASSERT(1 == r.isMatch("xac", 3));
            ASSERT(1 == r.isMatch("xbc", 3));
            ASSERT(1 == r.isMatch("xxxac", 5));
            ASSERT(1 == r.isMatch("xxxbc", 5));
            ASSERT(0 == r.isMatch("cba", 3));

            ASSERT(1 == r.isMatch("aaaac", 5));
            ASSERT(1 == r.isMatch("aaccc", 5));
            ASSERT(1 == r.isMatch("aaabc", 5));
            ASSERT(1 == r.isMatch("abccc", 5));
            ASSERT(1 == r.isMatch("xaccc", 5));
            ASSERT(1 == r.isMatch("xbccc", 5));

            ASSERT(1 == r.isMatch("aaaaccc", 7));
            ASSERT(1 == r.isMatch("aaabccc", 7));
            ASSERT(1 == r.isMatch("aaxaccc", 7));
            ASSERT(1 == r.isMatch("aaxbccc", 7));
        }
        {
            Obj r("a[ab]c", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("axc", 3));
            ASSERT(0 == r.isMatch("ac", 2));

            ASSERT(1 == r.isMatch("aaaac", 5));
            ASSERT(0 == r.isMatch("aaaxc", 5));
            ASSERT(1 == r.isMatch("aaccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));
            ASSERT(1 == r.isMatch("aaabc", 5));
            ASSERT(0 == r.isMatch("aaxbc", 5));
            ASSERT(1 == r.isMatch("abccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));

            ASSERT(1 == r.isMatch("aaaaccc", 7));
            ASSERT(0 == r.isMatch("aaxaccc", 7));
            ASSERT(1 == r.isMatch("aaabccc", 7));
            ASSERT(0 == r.isMatch("aaxbccc", 7));
        }
        {
            Obj r("^a[ab]c", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("axc", 3));
            ASSERT(0 == r.isMatch("ac", 2));

            ASSERT(0 == r.isMatch("aaaac", 5));
            ASSERT(0 == r.isMatch("aaaxc", 5));
            ASSERT(1 == r.isMatch("aaccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));
            ASSERT(0 == r.isMatch("aaabc", 5));
            ASSERT(0 == r.isMatch("aaxbc", 5));
            ASSERT(1 == r.isMatch("abccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));

            ASSERT(0 == r.isMatch("aaaaccc", 7));
            ASSERT(0 == r.isMatch("aaxaccc", 7));
            ASSERT(0 == r.isMatch("aaabccc", 7));
            ASSERT(0 == r.isMatch("aaxbccc", 7));
        }
        {
            Obj r("a[ab]c$", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("axc", 3));
            ASSERT(0 == r.isMatch("ac", 2));

            ASSERT(1 == r.isMatch("aaaac", 5));
            ASSERT(0 == r.isMatch("aaaxc", 5));
            ASSERT(0 == r.isMatch("aaccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));
            ASSERT(1 == r.isMatch("aaabc", 5));
            ASSERT(1 == r.isMatch("xaabc", 5));
            ASSERT(0 == r.isMatch("aaxbc", 5));
            ASSERT(0 == r.isMatch("abccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));

            ASSERT(0 == r.isMatch("aaaaccc", 7));
            ASSERT(0 == r.isMatch("aaxaccc", 7));
            ASSERT(0 == r.isMatch("aaabccc", 7));
            ASSERT(0 == r.isMatch("aaxbccc", 7));
        }
        {
            Obj r("^a[ab]c$", &testAllocator);

            ASSERT(1 == r.isMatch("aac", 3));
            ASSERT(1 == r.isMatch("abc", 3));
            ASSERT(0 == r.isMatch("axc", 3));
            ASSERT(0 == r.isMatch("ac", 2));

            ASSERT(0 == r.isMatch("aaaac", 5));
            ASSERT(0 == r.isMatch("aaaxc", 5));
            ASSERT(0 == r.isMatch("aaccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));
            ASSERT(0 == r.isMatch("aaabc", 5));
            ASSERT(0 == r.isMatch("aaxbc", 5));
            ASSERT(0 == r.isMatch("abccc", 5));
            ASSERT(0 == r.isMatch("axccc", 5));

            ASSERT(0 == r.isMatch("aaaaccc", 7));
            ASSERT(0 == r.isMatch("aaxaccc", 7));
            ASSERT(0 == r.isMatch("aaabccc", 7));
            ASSERT(0 == r.isMatch("aaxbccc", 7));
        }
      } break;
      case 6: {
        Obj r("ab{0,0}c", &testAllocator);

        ASSERT(1 == r.isMatch("ac", 2));
        ASSERT(0 == r.isMatch("abc", 3));

        ASSERT(1 == r.isMatch("aaac", 4));
        ASSERT(0 == r.isMatch("aaabc", 5));

        ASSERT(1 == r.isMatch("accc", 4));
        ASSERT(0 == r.isMatch("abccc", 5));

        ASSERT(1 == r.isMatch("aaaccc", 6));
        ASSERT(0 == r.isMatch("aaabccc", 7));
      } break;
      case 5: {
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

        bdecs_Regex re("^([0-9]{3}-|\\([0-9]{3}\\) )?[0-9]{3}-[0-9]{4}$");

        ASSERT(1 == re.isMatch("111-222-3333"));
        ASSERT(0 == re.isMatch("111-222 3333"));
        ASSERT(0 == re.isMatch("111 222-3333"));
        ASSERT(0 == re.isMatch(" 111-222-3333"));
        ASSERT(0 == re.isMatch("111-222-3333 "));

        ASSERT(1 == re.isMatch("(111) 222-3333"));
        ASSERT(0 == re.isMatch("(111)-222-3333"));
        ASSERT(0 == re.isMatch("(111) 222 3333"));
        ASSERT(0 == re.isMatch("(111)-222 3333"));
        ASSERT(0 == re.isMatch("(111 222-3333"));
        ASSERT(0 == re.isMatch("111) 222-3333"));
        ASSERT(0 == re.isMatch(" (111) 222-3333"));
        ASSERT(0 == re.isMatch("(111) 222-3333 "));

        ASSERT(1 == re.isMatch("222-3333"));
        ASSERT(0 == re.isMatch("222 3333"));
      } break;
      case 4: {
        Obj r("^(abc)*$", &testAllocator);

        ASSERT(1 == r.isMatch("", 0));
        ASSERT(1 == r.isMatch("abc", 3));
        ASSERT(1 == r.isMatch("abcabc", 6));
        ASSERT(1 == r.isMatch("abcabcabc", 9));

        ASSERT(0 == r.isMatch("abcc", 4));
        ASSERT(0 == r.isMatch("abccc", 5));
        ASSERT(0 == r.isMatch("abcccc", 6));

        ASSERT(1 == r.isMatch("", 0));
        ASSERT(0 == r.isMatch("a", 1));
        ASSERT(0 == r.isMatch("ab", 2));

        ASSERT(0 == r.isMatch("abca", 4));

        ASSERT(0 == r.isMatch("aba", 3));
      } break;
      case 3: {
        Obj r("abc+", &testAllocator);

        ASSERT(1 == r.isMatch("abc", 3));
        ASSERT(1 == r.isMatch("abcc", 4));
        ASSERT(1 == r.isMatch("abccc", 5));
        ASSERT(1 == r.isMatch("abcccc", 6));
        ASSERT(1 == r.isMatch("abcabc", 6));

        ASSERT(0 == r.isMatch("abc", 0));
        ASSERT(0 == r.isMatch("abc", 1));
        ASSERT(0 == r.isMatch("abc", 2));

        ASSERT(1 == r.isMatch("abca", 4));

        ASSERT(0 == r.isMatch("aba", 3));
      } break;
      case 2: {
        Obj r("abcc*", &testAllocator);

        ASSERT(1 == r.isMatch("abc", 3));
        ASSERT(1 == r.isMatch("abcc", 4));
        ASSERT(1 == r.isMatch("abccc", 5));
        ASSERT(1 == r.isMatch("abcccc", 6));

        ASSERT(0 == r.isMatch("", 0));
        ASSERT(0 == r.isMatch("a", 1));
        ASSERT(0 == r.isMatch("ab", 2));

        ASSERT(1 == r.isMatch("abca", 4));

        ASSERT(0 == r.isMatch("aba", 3));
      } break;
      case 1: {
        Obj r("abc", &testAllocator);

        ASSERT(1 == r.isMatch("abc", 3));

        ASSERT(0 == r.isMatch("abc", 0));
        ASSERT(0 == r.isMatch("abc", 1));
        ASSERT(0 == r.isMatch("abc", 2));

        ASSERT(1 == r.isMatch("abca", 4));
        ASSERT(1 == r.isMatch("babc", 4));
        ASSERT(1 == r.isMatch("xabcz", 5));

        ASSERT(0 == r.isMatch("aba", 3));
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
