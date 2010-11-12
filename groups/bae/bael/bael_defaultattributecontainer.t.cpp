// bael_defaultattributecontainer.t.cpp                               -*-C++-*-

#include <bael_defaultattributecontainer.h>

#include <bslma_testallocator.h>             // for testing only
#include <bslma_testallocatorexception.h>    // for testing only
#include <bdex_testinstream.h>               // for testing only
#include <bdex_testoutstream.h>              // for testing only

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is an in-core value-semantic component in that it
// supports all value-semantic operations except 'bdex' streaming.  We choose
// the default constructor, the 'addAttribute' and the 'removeAttribute'
// methods as the primary manipulators, and 'hasValue' as the basic accessor.
// The modified 10-step test procedure without the testing for 'bdex' streaming
// is then performed.
//-----------------------------------------------------------------------------
// [ 2] bael_DefaultAttributeContainer(bslma_Allocator *basicAllocator = 0);
// [ 7] bael_DefaultAttributeContainer(const bael_DefaultAttributeContainer&,
//                                     bslma_Allocator * = 0)
// [ 2] ~bael_DefaultAttributeContainer();
// [ 2] int addAttribute(const bael_Attribute& attribute);
// [ 2] int removeAttribute(const bael_Attribute& attribute);
// [12] void removeAllAttributes();
// [ 9] const bael_DefaultAttributeContainer& operator=(const bael_AS& other)
// [ 4] int numAttributes() const;
// [ 4] bool hasValue(const bael_Attribute&) const;
// [11] const_iterator begin() const;
// [11] const_iterator end() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] bool operator==(const bael_AS& lhs, const bael_AS& rhs)
// [ 6] bool operator!=(const bael_AS& lhs, const bael_AS& rhs)
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const bael_AS&) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Obj& gg(Obj *obj, const char *spec);
// [ 8] UNUSED
// [10] UNUSED
// [13] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"               \
                     << #J << ": " << J << "\t"               \
                     << #K << ": " << K << "\n";              \
                     aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"               \
                     << #J << ": " << J << "\t"               \
                     << #K << ": " << K << "\n"               \
                     << #L << ": " << L << "\n";              \
                     aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_DefaultAttributeContainer Obj;
typedef bdex_TestInStream  StreamIn;
typedef bdex_TestOutStream StreamOut;

typedef bsls_PlatformUtil::Int64 Int64;

bael_Attribute A0("", "12345678");
bael_Attribute A1("", 12345678);
bael_Attribute A2("", (Int64)12345678);
bael_Attribute A3("uuid", "12345678");
bael_Attribute A4("uuid", 12345678);
bael_Attribute A5("uuid", (Int64)12345678);
bael_Attribute A6("UUID", "12345678");
bael_Attribute A7("UUID", 12345678);
bael_Attribute A8("UUID", (Int64)12345678);

const char* NAMES[] = { "",                                       // A
                        "A",                                      // B
                        "a",                                      // C
                        "B",                                      // D
                        "b",                                      // E
                        "AA",                                     // F
                        "Aa",                                     // G
                        "AB",                                     // H
                        "Ab",                                     // I
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",             // J

                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"              // K
};

int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'bael_DefaultAttributeContainer'
// object for testing.  They interpret a given 'spec' (from left to right) to
// configure the attribute set according to a custom language.
//
// To simplify these generator functions, an attribute is represented by two
// or three characters.  The first character must be within ['A' .. 'A' +
// NUM_NAMES - 1], indicating an attribute name that can be looked up from the
// array NAMES[] defined above.  If the second character is an 'i' or 'I',
// then the third must be a character within ['0' - '9'], representing
// respectively a 32-bit or 64-bit integral value within [0 - 9].  If the
// second character is neither 'i' nor 'I' then the second character is used
// directly as the string value (with a '\0' character appended).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <ATTRIBUTE> *
//
// <ATTRIBUTE>   ::= <NAME> <VALUE>
//
// <NAME>        ::= [ 'A' .. 'A' + NUM_NAMES - 1 ]
//
// <VALUE>       ::= 'i' <INTEGER>    // 32-bit
//                 | 'I' <INTEGER>    // 64-bit
//                 | <STRING>
//
// <INTEGER>     ::= [ '0' .. '9' ]
//
// <STRING>      ::= any character except 'i' and 'I'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unaltered.
// "AA"         Produces: { A=A   } {string value}
// "A1"         Produces: { A=1   } (string value)
// "Ai1"        Produces: { A=1   } (32 bit value)
// "AI1"        Produces: { A=1   } (64 bit value)
// "AABB"       Produces: { A=A, B=B }
//-----------------------------------------------------------------------------

static Obj& gg(Obj *obj, const char *spec)
{
    const char *name;
    while (*spec) {
        if ('A' > *spec || 'A' + NUM_NAMES <= *spec) {
            return *obj;                                              // RETURN
        }
        name = NAMES[*spec - 'A'];
        ++spec;
        switch (*spec) {
          case 'i': {
            ++spec;
            bael_Attribute attr(name, *spec - '0');
            obj->addAttribute(attr);
          } break;
          case 'I': {
            ++spec;
            bael_Attribute attr(name, (Int64)*spec - '0');
            obj->addAttribute(attr);
          } break;
          default: {
            string value;
            value = *spec;
            bael_Attribute attr(name, value.c_str());
            obj->addAttribute(attr);
          }
        }
        ++spec;
    }
    return *obj;
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

    bslma_TestAllocator  testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting usage example"
                          << "\n====================="
                          << endl;

        bael_DefaultAttributeContainer attributeSet;

        bael_Attribute a1("uuid", 1111);
        bael_Attribute a2("sid", "111-1");
        ASSERT(1 == attributeSet.addAttribute(a1));
        ASSERT(1 == attributeSet.addAttribute(a2));

        bael_Attribute a3("uuid", 2222);
        bael_Attribute a4("sid", "222-2");
        ASSERT(1 == attributeSet.addAttribute(a3));
        ASSERT(1 == attributeSet.addAttribute(a4));

        bael_Attribute a5("uuid", 1111);                 // same as 'a1'
        ASSERT(0 == attributeSet.addAttribute(a5));

        bael_Attribute a6("UUID", 1111);
        ASSERT(1 == attributeSet.addAttribute(a6));

        ASSERT(true == attributeSet.hasValue(a1));
        ASSERT(true == attributeSet.hasValue(a2));
        ASSERT(true == attributeSet.hasValue(a3));
        ASSERT(true == attributeSet.hasValue(a4));
        ASSERT(true == attributeSet.hasValue(a5));
        ASSERT(true == attributeSet.hasValue(a6));

        attributeSet.removeAttribute(a1);
        ASSERT(false == attributeSet.hasValue(a1));

        bael_DefaultAttributeContainer::const_iterator iter;
        for (iter = attributeSet.begin();
             iter != attributeSet.end();
             ++iter ) {
            if (veryVerbose) {
                bsl::cout << *iter << bsl::endl;
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'removeAllAttributes'
        //   The 'removeAllAttributes' should effectively empty the
        //   'bael_DefaultAttributeContainer' object.
        //
        // Plan:
        //   Specify a set S of test vectors.  For each element in S,
        //   construct the corresponding 'bael_DefaultAttributeContainer'
        //   object x using the 'gg' function.  Copy x into another object y.
        //   After calling 'removeAllAttributes' on x, verify that the length
        //   of x is zero, none of attributes in y can be found in x.  Then
        //   reconstruct x using the 'gg' function again, and verify that
        //   x == y.
        //
        // Testing: void removeAllAttributes();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAllAttributes"
                          << "\n============================"
                          << endl;

        static const char* SPECS[] = {
            "",
            "AA",
            "Ai1",
            "AI1",
            "AABA",
            "AABi1",
            "AABI1",
            "Ai1BA",
            "Ai1Bi1",
            "Ai1BI1",
            "AI1BA",
            "AI1Bi1",
            "AI1BI1",
            "AABBCC",
            "AABBCCDD",
            "AABBCCDDEE",
            "AABBCCDDEEFF",
            "AABBCCDDEEFFGG",
            "AABBCCDDEEFFGGHH",
        };
        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPECS; ++i) {
        for (int j = 0; j < NUM_SPECS; ++j) {

            if (veryVerbose) { P_(i); P_(j); P_(SPECS[i]); P(SPECS[j]); }

            Obj mX; const Obj& X = mX;
            LOOP2_ASSERT(i, j, &mX == &gg(&mX, SPECS[i]));

            Obj mY; const Obj& Y = mY;
            LOOP2_ASSERT(i, j, &mY == &gg(&mY, SPECS[j]));

            mX.removeAllAttributes();

            LOOP2_ASSERT(i, j, 0 == X.numAttributes());
            for (Obj::const_iterator iter = Y.begin();
                 iter != Y.end();
                 ++iter) {
                LOOP2_ASSERT(i, j, false == X.hasValue(*iter));
            }

            LOOP2_ASSERT(i, j, &mX == &gg(&mX, SPECS[j]));
            LOOP2_ASSERT(i, j, X == Y);
        }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'begin()' and 'end'
        //   This will test the 'begin()' and 'end()' methods.
        //
        // Concerns:
        //   The 'begin() and 'end()' methods should return a range where each
        //   attribute in the attribute set appears exactly once.
        //
        // Plan:
        //   Construct an array consisting of 'bael_Attribute' objects having
        //   distinct values.  For each n in [0 .. N] where N is the maximum
        //   number of attributes tested, create an empty
        //   'bael_DefaultAttributeContainer' object and add the first n
        //   attributes to the set.  Verify that every added attributes appear
        //   in the set exactly once.
        //
        // Testing:
        //   const_iterator begin() const;
        //   const_iterator end() const;
        //   int numAttributes() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'begin()' and 'end()'"
                          << "\n=============================" << endl;

        const bael_Attribute* ATTRS[] = { &A0, &A1, &A2, &A3, &A4,
                                          &A5, &A6, &A7, &A8 };
        const int NUM_ATTRS = sizeof ATTRS / sizeof *ATTRS;

        int isPresentFlags[NUM_ATTRS];

        for (int i = 0; i < NUM_ATTRS; ++i) {
            Obj mX; const Obj& X = mX;

            int j, length;
            for (j = 0; j < i; ++j) {
                LOOP2_ASSERT(i, j, 1 == mX.addAttribute(*ATTRS[j]));
                LOOP2_ASSERT(i, j, X.hasValue(*ATTRS[j]));
                isPresentFlags[j] = 0;
            }

            LOOP_ASSERT(i, j == X.numAttributes());

            length = 0;
            for (Obj::const_iterator iter = X.begin();
                 iter != X.end();
                 ++iter, ++length) {
                for (j = 0; j < i; ++j) {
                    if (*iter == *ATTRS[j]) {
                        ++isPresentFlags[j];
                    }
                }
            }

            LOOP_ASSERT(i, length == X.numAttributes());

            for (j = 0; j < i; ++j) {
                LOOP2_ASSERT(i, j, 1 == isPresentFlags[j]);
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   Void for 'bael_AttributeSET'.
        // --------------------------------------------------------------------

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   const bael_DefaultAttributeContainer& operator=(
        //                                                const bael_AS& other)
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n==========================" << endl;

        static const struct {
            int         d_line;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line    spec
            // ----    ----
            {  L_,    "",                  },
            {  L_,    "AA",                },
            {  L_,    "Ai1",               },
            {  L_,    "AI1",               },
            {  L_,    "AABA",              },
            {  L_,    "AABi1",             },
            {  L_,    "AABI1",             },
            {  L_,    "Ai1BA",             },
            {  L_,    "Ai1Bi1",            },
            {  L_,    "Ai1BI1",            },
            {  L_,    "AI1BA",             },
            {  L_,    "AI1Bi1",            },
            {  L_,    "AI1BI1",            },
            {  L_,    "AABBCC",            },
            {  L_,    "AABBCCDD",          },
            {  L_,    "AABBCCDDEE",        },
            {  L_,    "AABBCCDDEEFF",      },
            {  L_,    "AABBCCDDEEFFGG",    },
            {  L_,    "AABBCCDDEEFFGGHH",  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE1 = DATA[i].d_line;

            for (int j = 0; j < NUM_DATA; ++j) {
                const int LINE2 = DATA[j].d_line;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(DATA[i].d_spec);
                    P_(LINE2);
                    P(DATA[j].d_spec);
                }

                Obj mX; const Obj& X = mX;
                LOOP_ASSERT(LINE1, &mX == &gg(&mX, DATA[i].d_spec));

                Obj mY; const Obj& Y = mY;
                LOOP_ASSERT(LINE2, &mY == &gg(&mY, DATA[j].d_spec));

                Obj mW(Y); const Obj& W = mW;

                mX = Y;

                LOOP2_ASSERT(LINE1, LINE2, Y == W);
                LOOP2_ASSERT(LINE1, LINE2, W == Y);
                LOOP2_ASSERT(LINE1, LINE2, X == W);
                LOOP2_ASSERT(LINE1, LINE2, W == X);
            }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                          << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) { P_(LINE); P_(DATA[i].d_spec); }

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            Obj mW(X); const Obj& W = mW;

            mX = X;

            LOOP_ASSERT(LINE, X == W);
            LOOP_ASSERT(LINE, W == X);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'bael_DefaultAttributeContainer'.
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using 'gg'.  Then
        //   copy construct an object y from x, and use the equality operator
        //   to assert that both x and y have the same value as w.
        //
        // Testing:
        //   bael_DefaultAttributeContainer(
        //                              const bael_DefaultAttributeContainer&,
        //                              bslma_Allocator * = 0)
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        static const struct {
            int         d_line;      // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line    spec
            // ----    ----
            {  L_,    "",                  },
            {  L_,    "AA",                },
            {  L_,    "Ai1",               },
            {  L_,    "AI1",               },
            {  L_,    "AABA",              },
            {  L_,    "AABi1",             },
            {  L_,    "AABI1",             },
            {  L_,    "Ai1BA",             },
            {  L_,    "Ai1Bi1",            },
            {  L_,    "Ai1BI1",            },
            {  L_,    "AI1BA",             },
            {  L_,    "AI1Bi1",            },
            {  L_,    "AI1BI1",            },
            {  L_,    "AABBCC",            },
            {  L_,    "AABBCCDD",          },
            {  L_,    "AABBCCDDEE",        },
            {  L_,    "AABBCCDDEEFF",      },
            {  L_,    "AABBCCDDEEFFGG",    },
            {  L_,    "AABBCCDDEEFFGGHH",  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) { P_(LINE); P(DATA[i].d_spec); }

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            Obj mW; const Obj& W = mW;
            LOOP_ASSERT(LINE, &mW == &gg(&mW, DATA[i].d_spec));

            // construct y without an allocator
            {
                Obj mY(X); const Obj& Y = mY;

                LOOP_ASSERT(LINE, Y == W);
                LOOP_ASSERT(LINE, W == Y);
                LOOP_ASSERT(LINE, X == W);
                LOOP_ASSERT(LINE, W == X);
            }

            // construct y with an allocator but no exception
            {
                bslma_TestAllocator testAllocatorY(veryVeryVerbose);
                Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                LOOP_ASSERT(LINE, Y == W);
                LOOP_ASSERT(LINE, W == Y);
                LOOP_ASSERT(LINE, X == W);
                LOOP_ASSERT(LINE, W == X);
            }

            // construct y with an allocator and exceptions
            {
                bslma_TestAllocator testAllocatorY(veryVeryVerbose);

                BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                    LOOP_ASSERT(LINE, Y == W);
                    LOOP_ASSERT(LINE, W == Y);
                    LOOP_ASSERT(LINE, X == W);
                    LOOP_ASSERT(LINE, W == X);
                } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   First, specify a set S of unique object values that may have
        //   various minor or subtle differences.  Verify the correctness of
        //   'operator==' and 'operator!=' using all elements (u, v) of the
        //   cross product S X S.
        //
        //   Next, specify another set T where each element is a pair of
        //   different specifications having the same value (the same
        //   attributes were added in different orders).  For each element (u,
        //   v) in T, verify that 'operator==' and 'operator!=' return the
        //   correct value.

        // Testing:
        //   bool operator==(const bael_AS& lhs, const bael_AS& rhs)
        //   bool operator!=(const bael_AS& lhs, const bael_AS& rhs)
        // --------------------------------------------------------------------

        static const struct {
            int         d_line;      // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line   spec
            // ----   ----
            {  L_,    ""                  },

            {  L_,    "AA"                },
            {  L_,    "Ai1"               },
            {  L_,    "AI1"               },
            {  L_,    "BA"                },
            {  L_,    "Bi1"               },
            {  L_,    "BI1"               },
            {  L_,    "BB"                },
            {  L_,    "BC"                },

            {  L_,    "AABA"              },
            {  L_,    "AAAB"              },
            {  L_,    "Ai1BA"             },
            {  L_,    "Ai1Bi1"            },
            {  L_,    "Ai1BI1"            },
            {  L_,    "AI1Bi1"            },
            {  L_,    "AI2Bi1"            },
            {  L_,    "BBAi1"             },
            {  L_,    "Ai1BI2"            },

            {  L_,    "AABBCC"            },
            {  L_,    "AABBCCDD"          },
            {  L_,    "AABBCCDDEE"        },
            {  L_,    "AABBCCDDEEFF"      },
            {  L_,    "AABBCCDDEEFFGG"    },
            {  L_,    "AABBCCDDEEFFGGHH"  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nCompare u against v for each (u, v) in S X S."
                          << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE1 = DATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE1, &mX == &gg(&mX, DATA[i].d_spec));
            for (int j = 0; j < NUM_DATA; ++j) {
                const int LINE2 = DATA[j].d_line;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(DATA[i].d_spec);
                    P_(LINE2);
                    P(DATA[j].d_spec);
                }

                Obj mY; const Obj& Y = mY;
                LOOP_ASSERT(LINE1, &mY == &gg(&mY, DATA[j].d_spec));

                LOOP2_ASSERT(LINE1, LINE2, (i==j) == (X == Y));
                LOOP2_ASSERT(LINE1, LINE2, (i!=j) == (X != Y));
            }
        }

        if (verbose) cout
            << "\nCompare w~u against v for each (w, u, v) in S X S X S."
            << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
        for (int j = 0; j < NUM_DATA; ++j) {
        for (int k = 0; k < NUM_DATA; ++k) {
            const int LINE1 = DATA[i].d_line;
            const int LINE2 = DATA[j].d_line;
            const int LINE3 = DATA[k].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE1, &mX == &gg(&mX, DATA[i].d_spec));

            mX.removeAllAttributes();

            LOOP2_ASSERT(LINE1, LINE2, &mX == &gg(&mX, DATA[j].d_spec));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE3, &mY == &gg(&mY, DATA[k].d_spec));

            if (veryVerbose) {
                P_(LINE1);
                P_(DATA[i].d_spec);
                P_(LINE2);
                P_(DATA[j].d_spec);
                P_(LINE3);
                P(DATA[k].d_spec);
            }

            LOOP3_ASSERT(LINE1, LINE2, LINE3, (j == k) == (X == Y));
            LOOP3_ASSERT(LINE1, LINE2, LINE3, (j != k) == (X != Y));
        }
        }
        }

        static const struct {
            int         d_line;       // source line number
            const char *d_spec1;      // input 'spec' string for 'gg'
            const char *d_spec2;      // input 'spec' string for 'gg'
        } TDATA[] = {
            // line    spec1              spec2
            // ----    -----              -----
            {  L_,    "",                 "",                 },
            {  L_,    "AABA",             "BAAA",             },
            {  L_,    "AABA",             "BAAA",             },
            {  L_,    "Ai1BA",            "BAAi1",            },
            {  L_,    "Ai1BI1",           "BI1Ai1",           },
            {  L_,    "Ai1BI1",           "BI1Ai1",           },
            {  L_,    "AABBCC",           "AACCBB",           },
            {  L_,    "AABBCC",           "BBAACC",           },
            {  L_,    "AABBCC",           "BBCCAA",           },
            {  L_,    "AABBCC",           "CCAABB",           },
            {  L_,    "AABBCC",           "CCBBAA",           },
            {  L_,    "AABBCCDD",         "DDCCBBAA",         },
            {  L_,    "AABBCCDDEE",       "EEDDCCBBAA",       },
            {  L_,    "AABBCCDDEEFF",     "FFEEDDCCBBAA",     },
            {  L_,    "AABBCCDDEEFFGG",   "GGFFEEDDCCBBAA",   },
            {  L_,    "AABBCCDDEEFFGGHH", "HHGGFFEEDDCCBBAA", },
        };

        const int NUM_TDATA = sizeof TDATA / sizeof *TDATA;

        if (verbose) cout << "\nCompare u against u for each u in T." << endl;

        for (int i = 0; i < NUM_TDATA; ++i) {
            const int LINE = TDATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, TDATA[i].d_spec1));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE, &mY == &gg(&mY, TDATA[i].d_spec2));

            if (veryVerbose) {
                P_(LINE);
                P_(TDATA[i].d_spec1);
                P(TDATA[i].d_spec2);
            }

            LOOP_ASSERT(LINE, 1 == (X == Y));
            LOOP_ASSERT(LINE, 0 == (X != Y));
        }

        if (verbose) cout
            << "\nCompare w~u against u in each (w, u) in S X S."
            << endl;

        for (int i = 0; i < NUM_TDATA; ++i) {
        for (int j = 0; j < NUM_TDATA; ++j) {
            const int LINE1 = TDATA[i].d_line;
            const int LINE2 = TDATA[j].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE1, &mX == &gg(&mX, TDATA[i].d_spec1));

            mX.removeAllAttributes();

            LOOP2_ASSERT(LINE1, LINE2, &mX == &gg(&mX, TDATA[j].d_spec1));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE2, &mY == &gg(&mY, TDATA[j].d_spec2));

            if (veryVerbose) {
                P_(LINE1);
                P_(TDATA[i].d_spec1);
                P_(LINE2);
                P_(TDATA[j].d_spec1);
                P(TDATA[j].d_spec2);
            }

            LOOP2_ASSERT(LINE1, LINE2, 1 == (X == Y));
            LOOP2_ASSERT(LINE1, LINE2, 0 == (X != Y));
        }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'operator<<' AND 'print':
        //   The output operator and 'print' method should print out the value
        //   of objects in the expected format.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const bael_AS&) const;
        //   bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'operator<<' and 'print'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        static const struct {
            int         d_line;            // line number
            const char *d_spec;            // spec
            const char *d_output;          // expected output format
        } DATA[] = {
            // line   spec          expected output
            // ----   ----          ---------------
            {  L_,    "",           "{ } "                                   },
            {  L_,    "AA",         "{  [  = A ] } "                        },
            {  L_,    "Ai1",        "{  [  = 1 ] } "                        },
            {  L_,    "AI1",        "{  [  = 1 ] } "                        },
            {  L_,    "BB",         "{  [ A = B ] } "                       },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            ostringstream os;
            os << X;
            LOOP_ASSERT(LINE, os.str() == DATA[i].d_output);

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_spec);
                P_(DATA[i].d_output);
                P(os.str());
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;

        static const struct {
            int         d_line;            // line number
            const char *d_spec;            // spec for gg
            int         d_level;           // tab level
            int         d_spacesPerLevel;  // spaces per level
           const char  *d_output;          // expected output format
        } PDATA[] = {
            // line spec        level space expected
            // ---- ----        ----- ----- -----------------------
            {  L_,  "BA",       1,    2,    "  {\n"
                                            "    [ A = A ]\n"
                                            "  }\n"  },
        };

        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        for (int i = 0; i < NUM_PDATA; ++i) {
            int LINE = PDATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, PDATA[i].d_spec));

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                P_(LINE);
                P_(PDATA[i].d_spec);
                P(PDATA[i].d_output);
                P(os.str());
            }

            LOOP_ASSERT(LINE, os.str() == PDATA[i].d_output);
        }

     } break;
     case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Every attribute added must be verified by 'hasValue'.
        //
        // Plan:
        //   Mechanically generate a series of specifications whose contain
        //   string attributes with names 'A' .. 'A' + NUM_NAMES and with
        //   values 'A' .. 'J', and integral attributes with name 'a' .. 'j'
        //   and with values 0 .. 9.  For each specification, create an
        //   attribute set from the specification, and verify that the
        //   attributes with the specified names having the specified values
        //   exist.
        //
        // Testing (indirectly):
        //   bool hasValue(const bael_Attribute&) const;
        //   int numAttributes() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl;

        for (int n = 0; n < NUM_NAMES; ++n) {
            bsl::string spec;
            for (int v = 0; v < n; ++v) {
                spec += (char)n + 'A';     // add a string attribute

                // don't use 'I' as the string attribute value
                spec +=  v >= 'I' - 'A'? (char)v + 'B' : (char)v + 'A' ;

                spec += (char)n + 'A';     // add an int32 attribute
                spec += 'i';
                spec += (char)v + '0';

                spec += (char)n + 'A';     // add an int64 attribute
                spec += 'I';
                spec += (char)v + '0';
            }

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(n, &mX == &gg(&mX, spec.c_str()));
            LOOP_ASSERT(n, X.numAttributes() == 3 * n);

            if (veryVerbose) { P_(n); P(spec); }

            for (int v = 0; v <= '9' - '0'; ++v) {
                bsl::string sValue;
                sValue =  v >= 'I' - 'A'? (char)v + 'B' : (char)v + 'A';
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], sValue.c_str())));

                int int32Value = v;
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], int32Value)));

                Int64 int64Value = v;
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], int64Value)));
            }
        }

      } break;
     case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'GG'
        //   The 'gg' function must create objects having the expected values.
        //
        // Plan:
        //   Mechanically generate a series of specifications whose contain
        //   string attributes with names 'A' .. 'A' + NUM_NAMES and with
        //   values 'A' .. 'J', and integral attributes with name 'a' .. 'j'
        //   and with values 0 .. 9.  For each specification, create an
        //   attribute set from the specification, and verify that the
        //   attributes with the specified names having the specified values
        //   exist.
        //
        // Testing:
        //   Obj& gg(Obj *address, const char *spec);
        // --------------------------------------------------------------------

         if (verbose) cout << endl
            << "Testing 'gg' generator function" << endl
            << "===============================" << endl;

        for (int n = 0; n < NUM_NAMES; ++n) {
            bsl::string spec;
            for (int v = 0; v < n; ++v) {
                spec += (char)n + 'A';     // add a string attribute

                // don't use 'I' as the string attribute value
                spec +=  v >= 'I' - 'A'? (char)v + 'B' : (char)v + 'A' ;

                spec += (char)n + 'A';     // add an int32 attribute
                spec += 'i';
                spec += (char)v + '0';

                spec += (char)n + 'A';     // add an int64 attribute
                spec += 'I';
                spec += (char)v + '0';
            }

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(n, &mX == &gg(&mX, spec.c_str()));
            LOOP_ASSERT(n, X.numAttributes() == 3 * n);

            if (veryVerbose) { P_(n); P(spec); }

            for (int v = 0; v <= '9' - '0'; ++v) {
                bsl::string sValue;
                sValue =  v >= 'I' - 'A'? (char)v + 'B' : (char)v + 'A';
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], sValue.c_str())));

                int int32Value = v;
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], int32Value)));

                Int64 int64Value = v;
                LOOP2_ASSERT(n,
                             v,
                             (v < n) == X.hasValue(
                                  bael_Attribute(NAMES[n], int64Value)));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   Setter functions should correctly pass the specified value to the
        //   object.
        //
        // Plan:
        //   For a sequence of independent test values, use the constructor to
        //   create an object and use the primary manipulators to set its
        //   value.  For each value that is valid, verify, using the basic
        //   accessors, that the value has been assigned to the object;
        //   otherwise verify that the original value of the object is
        //   retained.
        //
        // Testing:
        //   bael_DefaultAttributeContainer(
        //                                bslma_Allocator *basicAllocator = 0);
        //   int addAttribute(const bael_Attribute& attribute);
        //   int removeAttribute(const bael_Attribute& attribute);
        //   ~bael_DefaultAttributeContainer();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\nTesting primary manipulator." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;  const Obj& X = mX;

            Obj mY;  const Obj& Y = mY;
            ASSERT(1 == mY.addAttribute(A1));

            Obj mZ;  const Obj& Z = mZ;
            ASSERT(1 == mZ.addAttribute(A2));
            ASSERT(1 == mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.hasValue(A1));
            ASSERT(Z.hasValue(A2));
            ASSERT(Z.hasValue(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(1 == mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.hasValue(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeAttribute(A1));
            ASSERT(false == X.hasValue(A1));
            ASSERT(1 == mX.addAttribute(A2));
            ASSERT(1 == mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.hasValue(A2));
            ASSERT(X.hasValue(A3));
            ASSERT(X == Z);
        }

        if (verbose) cout << "\tWith an allocator." << endl;
        if (verbose) cout << "\t\tWithout exceptions." << endl;
        {
            bslma_TestAllocator testAllocatorX(veryVeryVerbose);
            bslma_TestAllocator testAllocatorY(veryVeryVerbose);
            bslma_TestAllocator testAllocatorZ(veryVeryVerbose);

            Obj mX(&testAllocatorX);  const Obj& X = mX;

            Obj mY(&testAllocatorY);  const Obj& Y = mY;
            ASSERT(1 == mY.addAttribute(A1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(1 == mZ.addAttribute(A2));
            ASSERT(1 == mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.hasValue(A1));
            ASSERT(Z.hasValue(A2));
            ASSERT(Z.hasValue(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(1 == mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.hasValue(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeAttribute(A1));
            ASSERT(false == X.hasValue(A1));
            ASSERT(1 == mX.addAttribute(A2));
            ASSERT(1 == mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.hasValue(A2));
            ASSERT(X.hasValue(A3));
            ASSERT(X == Z);
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            bslma_TestAllocator testAllocatorX(veryVeryVerbose);
            bslma_TestAllocator testAllocatorY(veryVeryVerbose);
            bslma_TestAllocator testAllocatorZ(veryVeryVerbose);

          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Primary Manipulator" << endl;

            Obj mX(&testAllocatorX);  const Obj& X = mX;

            Obj mY(&testAllocatorY);  const Obj& Y = mY;
            ASSERT(1 == mY.addAttribute(A1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(1 == mZ.addAttribute(A2));
            ASSERT(1 == mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.hasValue(A1));
            ASSERT(Z.hasValue(A2));
            ASSERT(Z.hasValue(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(1 == mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.hasValue(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeAttribute(A1));
            ASSERT(false == X.hasValue(A1));
            ASSERT(1 == mX.addAttribute(A2));
            ASSERT(1 == mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.hasValue(A2));
            ASSERT(X.hasValue(A3));
            ASSERT(X == Z);

          } END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 1: {
       // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the initializing and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using these test objects.  Invoke the primary
        //   manipulator [3, 6], copy constructor [2, 8], and assignment
        //   operator without [9, 10] and with [11, 12] aliasing.  Use the
        //   direct accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and
        //   'VC' denote unique, but otherwise arbitrary, object values, while
        //   '0' denotes the default object value.
        //
        // 1.  Create an default object x1.         { x1:0 }
        // 2.  Create an object x2 (copy from x1).  { x1:0  x2:0 }
        // 3.  Set x1 to VA.                        { x1:VA x2:0 }
        // 4.  Set x2 to VA.                        { x1:VA x2:VA }
        // 5.  Set x2 to VB.                        { x1:VA x2:VB }
        // 6.  Set x1 to 0.                         { x1:0  x2:VB }
        // 7.  Create an object x3 (with value VC). { x1:0  x2:VB x3:VC }
        // 8.  Create an object x4 (copy from x1).  { x1:0  x2:VB x3:VC x4:0 }
        // 9.  Assign x2 = x1.                      { x1:0  x2:0  x3:VC x4:0 }
        // 10. Assign x2 = x3.                      { x1:0  x2:VC x3:VC x4:0 }
        // 11. Assign x1 = x1 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        // 12. Assign x2 = x2 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create a default object x1." << endl;
        Obj mX1;  const Obj& X1 = mX1;
        ASSERT(0 == X1.numAttributes());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        ASSERT(0 == X2.numAttributes());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        if (verbose) cout << "\n 3. Set x1 to VA." << endl;
        mX1.addAttribute(A1);
        mX1.addAttribute(A2);
        ASSERT(2 == X1.numAttributes());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VA." << endl;
        mX2.addAttribute(A1);
        mX2.addAttribute(A2);
        ASSERT(2 == X2.numAttributes());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VB." << endl;
        mX2.removeAllAttributes();
        mX2.addAttribute(A3);
        mX2.addAttribute(A4);
        ASSERT(2 == X2.numAttributes());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to 0." << endl;
        mX1.removeAllAttributes();
        ASSERT(0 == X1.numAttributes());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VC)."
                           << endl;
        Obj mX3;  const Obj& X3 = mX3;
        mX3.addAttribute(A5);
        ASSERT(1 == X3.numAttributes());
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1);  const Obj& X4 = mX4;
        ASSERT(0 == X4.numAttributes());
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT(0 == X2.numAttributes());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT(1 == X2.numAttributes());
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT(0 == X1.numAttributes());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        if (verbose) cout << "\n 12. Assign x2 = x2 (aliasing)." << endl;
        mX2 = X2;
        ASSERT(1 == X2.numAttributes());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
