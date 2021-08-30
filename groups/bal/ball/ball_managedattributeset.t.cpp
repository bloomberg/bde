// ball_managedattributeset.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_managedattributeset.h>

#include <ball_attributecontainerlist.h>        // for testing only
#include <ball_defaultattributecontainer.h>     // for testing only

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslim_testutil.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a value-semantic component.  For the standard
// 10-step test procedure, we choose the default constructor, 'addAttribute',
// and 'removeAttribute' as the primary manipulator, and 'isMember' and
// 'numAttributes' as the basic accessors.  Our concerns regarding the
// implementation of this component are that (1) all supported value-semantic
// methods are implemented correctly; (2) the hash values must be calculated
// correctly; and (3) the 'evaluate' method must return the correct value.
//-----------------------------------------------------------------------------
// [13] static int hash(const ball::ManagedAttributeSet&, int size);
// [ 2] ball::ManagedAttributeSet();
// [ 2] ball::ManagedAttributeSet(const allocator_type& allocator);
// [ 7] ball::ManagedAttributeSet(const MAS&, allocator);
// [ 2] ~ball::ManagedAttributeSet();
// [ 2] bool addAttribute(const ball::ManagedAttribute& attribute);
// [ 2] int addPredicate(const ball::ManagedAttribute& attribute);
// [ 2] bool removeAttribute(const ball::ManagedAttribute& attribute);
// [ 2] int removePredicate(const ball::ManagedAttribute& attribute);
// [11] void removeAll();
// [11] void removeAllPredicates();
// [ 9] const ball::ManagedAttributeSet& operator=(const MAS& other);
// [ 4] int numAttributes() const;
// [12] bool evaluate(const AttributeContainerList& containerList) const;
// [ 4] bool isMember(const ball::ManagedAttribute&) const;
// [10] const_iterator begin() const;
// [10] const_iterator end() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] bool operator==(const ball::MAS& lhs, const ball::MAS& rhs)
// [ 6] bool operator!=(const ball::MAS& lhs, const ball::MAS& rhs)
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::MAS&) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Obj& gg(Obj *address, const char *spec);
// [ 3] ManagedAttributeSet& hh(ManagedAttributeSet *obj, const char *spec);
// [ 8] UNUSED
// [14] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::ManagedAttributeSet       Obj;
typedef ball::DefaultAttributeContainer AttributeSet;

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

bool compareText(const bsl::string_view& lhs,
                 const bsl::string_view& rhs,
                 bsl::ostream&           errorStream = bsl::cout)
    // Return 'true' if the specified 'lhs' has the same value as the
    // specified' rhs' and 'false' otherwise.  Optionally specify a
    // 'errorStream', on which, if 'lhs' and 'rhs' are not the same', a
    // description of how the two strings differ will be written.  If
    // 'errorStream' is not supplied, 'stdout' will be used to report an error
    // description.
{
    for (unsigned int i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            errorStream << "lhs: \"" << lhs << "\"\n"
                        << "rhs: \"" << rhs << "\"\n"
                        << "Strings differ at index (" << i << ") "
                        << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                        << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                        << endl;
            return false;                                             // RETURN
        }
    }

    if (lhs.length() < rhs.length()) {
        bsl::size_t i = lhs.length();
        errorStream << "lhs: \"" << lhs << "\"\n"
                    << "rhs: \"" << rhs << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "lhs[i] = END-OF-STRING "
                    << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                    << endl;
        return false;                                                 // RETURN

    }
    if (lhs.length() > rhs.length()) {
        bsl::size_t i = rhs.length();
        errorStream << "lhs: \"" << lhs << "\"\n"
                    << "rhs: \"" << rhs << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                    << "rhs[i] = END-OF-STRING"
                    << endl;
        return false;                                                 // RETURN
    }
    return true;

}

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'ball::ManagedAttributeSet' object
// for testing.  They interpret a given 'spec' (from left to right) to
// configure the managed attribute set according to a custom language.
//
// To simplify these generator functions, an attribute is presented by two or
// three characters.  The first character must be within [ 'A' .. 'A' +
// NUM_NAMES - 1 ], indicating an attribute name.that can be looked up from the
// array NAMES[] defined above.  If the second character is an 'i' (or 'I'),
// then the third must be a character between '0' and '9' inclusively,
// representing an 32-bit (or 64-bit) integral value between '0' and '9'
// inclusively.  If the second character is neither 'i' nor 'I' then the second
// character is used directly as the string value (with a '\0' character
// appended).  In addition, '~' indicates the action to remove all attributes
// from the attribute set.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <PREDICATE> *
//
// <PREDICATE>   ::= <NAME> <VALUE>
//                 | ~
//
// <NAME>        ::= [ 'A' .. 'A' + NUM_NAMES - 1 ]
//
// <VALUE>       ::= 'i' <INTEGER>
//                 | 'I' <INTEGER>
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
    const char  *name;
    while (*spec) {
        if ('A' > *spec || 'A' + NUM_NAMES <= *spec) {
            return *obj;                                              // RETURN
        }
        name = NAMES[*spec - 'A'];
        ++spec;
        switch (*spec) {
          case 'i': {
            ++spec;
            ball::ManagedAttribute attr(name, *spec - '0');
            obj->addAttribute(attr);
          } break;
          case 'I': {
            ++spec;
            ball::ManagedAttribute attr(name,
                                        static_cast<long long>(*spec - '0'));
            obj->addAttribute(attr);
          } break;
          default: {
            string value;
            value = *spec;
            ball::ManagedAttribute attr(name, value.c_str());
            obj->addAttribute(attr);
          }
        }
        ++spec;
    }
    return *obj;
}

// 'gg' function for 'AttributeSet'; used for testing 'evaluate' method.

static AttributeSet& hh(AttributeSet *obj, const char *spec)
{
    const char  *name;
    while (*spec) {
        if ('A' > *spec || 'A' + NUM_NAMES <= *spec) {
            return *obj;                                              // RETURN
        }
        name = NAMES[*spec - 'A'];
        ++spec;
        switch (*spec) {
          case 'i': {
            ++spec;
            ball::Attribute attr(name, *spec - '0');
            obj->addAttribute(attr);
          } break;
          case 'I': {
            ++spec;
            ball::Attribute attr(name, static_cast<long long>(*spec - '0'));
            obj->addAttribute(attr);
          } break;
          default: {
            string value;
            value = *spec;
            ball::Attribute attr(name, value.c_str());
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

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    ball::ManagedAttribute A0("", "12345678");
    ball::ManagedAttribute A1("", 12345678);
    ball::ManagedAttribute A2("", 12345678LL);
    ball::ManagedAttribute A3("uuid", "12345678");
    ball::ManagedAttribute A4("uuid", 12345678);
    ball::ManagedAttribute A5("uuid", 12345678LL);
    ball::ManagedAttribute A6("UUID", "12345678");
    ball::ManagedAttribute A7("UUID", 12345678);
    ball::ManagedAttribute A8("UUID", 12345678LL);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Properties of 'ball::ManagedAttributeSet'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows basic operations on the managed attribute set.
//
// First, we create an empty attribute set:
//..
    ball::ManagedAttributeSet attributeSet;
//..
// Then, we add two attributes to the attribute set:
//..
    ball::ManagedAttribute p1("uuid", 4044457);
    ASSERT(attributeSet.addAttribute(p1));
    ASSERT(attributeSet.addAttribute(ball::ManagedAttribute("uuid", 3133246)));
//..
// Next, we look up (by value) via the 'isMember' method:
//..
    ASSERT(attributeSet.isMember(p1));
    ASSERT(attributeSet.isMember(ball::ManagedAttribute("uuid", 3133246)));
//..
// Then, we add duplicated value and observe the status of the operation:
//..
    ASSERT(!attributeSet.addAttribute(ball::ManagedAttribute("uuid",
                                                             3133246)));
//..
// Finally, we remove an attribute and check that it is not a member of the
// attribute set:
//..
    ASSERT(attributeSet.removeAttribute(p1));
    ASSERT(!attributeSet.isMember(p1));
//..

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION (VALUE):
        //   Verify the hash return value is constant across all platforms for
        //   a given input.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        //   Note that 7 is the smallest hash table size and 1610612741 is
        //   largest size (that can fit into an int) used by stlport
        //   hashtable.
        //
        // Testing:
        //   static int hash(const ball::ManagedAttributeSet&, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting hash function"
                          << "\n====================="
                          << endl;

        static const struct {
            int         d_line;  // source line number
            const char *d_spec;  // string
            int         d_size;  // size of hash table
            int         d_hash;  // expected output
        } DATA[] = {
            //line string               size          hash
            //---- ------               -----         ---
            {  L_, "",                  256,          0          },
            {  L_, "AA",                256,          90         },
            {  L_, "Ai1",               256,          227        },
            {  L_, "AI1",               256,          226        },
            {  L_, "AABA",              256,          244        },
            {  L_, "AABi1",             256,          125        },
            {  L_, "AABI1",             256,          124        },
            {  L_, "Ai1BA",             256,          125        },
            {  L_, "Ai1Bi1",            256,          6          },
            {  L_, "Ai1BI1",            256,          5          },
            {  L_, "AI1BA",             256,          124        },
            {  L_, "AI1Bi1",            256,          5          },
            {  L_, "AI1BI1",            256,          4          },
            {  L_, "AABBCC",            256,          255        },
            {  L_, "AABBCCDD",          256,          211        },
            {  L_, "AABBCCDDEE",        256,          216        },
            {  L_, "AABBCCDDEEFF",      256,          78         },
            {  L_, "AABBCCDDEEFFGG",    256,          4          },
            {  L_, "AABBCCDDEEFFGGHH",  256,          158        },
            {  L_, "",                  65536,        0          },
            {  L_, "AA",                65536,        7258       },
            {  L_, "Ai1",               65536,        24547      },
            {  L_, "AI1",               65536,        9442       },
            {  L_, "AABA",              65536,        45812      },
            {  L_, "AABi1",             65536,        63101      },
            {  L_, "AABI1",             65536,        47996      },
            {  L_, "Ai1BA",             65536,        63101      },
            {  L_, "Ai1Bi1",            65536,        14854      },
            {  L_, "Ai1BI1",            65536,        65285      },
            {  L_, "AI1BA",             65536,        47996      },
            {  L_, "AI1Bi1",            65536,        65285      },
            {  L_, "AI1BI1",            65536,        50180      },
            {  L_, "AABBCC",            65536,        4095       },
            {  L_, "AABBCCDD",          65536,        27091      },
            {  L_, "AABBCCDDEE",        65536,        18904      },
            {  L_, "AABBCCDDEEFF",      65536,        24654      },
            {  L_, "AABBCCDDEEFFGG",    65536,        36100      },
            {  L_, "AABBCCDDEEFFGGHH",  65536,        17054      },
            {  L_, "",                  7,            0          },
            {  L_, "AA",                7,            1          },
            {  L_, "Ai1",               7,            4          },
            {  L_, "AI1",               7,            5          },
            {  L_, "AABA",              7,            5          },
            {  L_, "AABi1",             7,            1          },
            {  L_, "AABI1",             7,            2          },
            {  L_, "Ai1BA",             7,            1          },
            {  L_, "Ai1Bi1",            7,            4          },
            {  L_, "Ai1BI1",            7,            5          },
            {  L_, "AI1BA",             7,            2          },
            {  L_, "AI1Bi1",            7,            5          },
            {  L_, "AI1BI1",            7,            6          },
            {  L_, "AABBCC",            7,            2          },
            {  L_, "AABBCCDD",          7,            6          },
            {  L_, "AABBCCDDEE",        7,            1          },
            {  L_, "AABBCCDDEEFF",      7,            6          },
            {  L_, "AABBCCDDEEFFGG",    7,            3          },
            {  L_, "AABBCCDDEEFFGGHH",  7,            6          },
            {  L_, "",                  1610612741,   0          },
            {  L_, "AA",                1610612741,   1588272218 },
            {  L_, "Ai1",               1610612741,   215900126  },
            {  L_, "AI1",               1610612741,   689251554  },
            {  L_, "AABA",              1610612741,   1097708271 },
            {  L_, "AABi1",             1610612741,   1335948920 },
            {  L_, "AABI1",             1610612741,   198687607  },
            {  L_, "Ai1BA",             1610612741,   1335948920 },
            {  L_, "Ai1Bi1",            1610612741,   1574189569 },
            {  L_, "Ai1BI1",            1610612741,   436928256  },
            {  L_, "AI1BA",             1610612741,   198687607  },
            {  L_, "AI1Bi1",            1610612741,   436928256  },
            {  L_, "AI1BI1",            1610612741,   910279684  },
            {  L_, "AABBCC",            1610612741,   1226051568 },
            {  L_, "AABBCCDD",          1610612741,   569993663  },
            {  L_, "AABBCCDDEE",        1610612741,   977619391  },
            {  L_, "AABBCCDDEEFF",      1610612741,   448946239  },
            {  L_, "AABBCCDDEEFFGG",    1610612741,   685542645  },
            {  L_, "AABBCCDDEEFFGGHH",  1610612741,   412238474  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            int LINE = DATA[i].d_line;
            Obj mX; const Obj& X = mX;
            ASSERTV(LINE, &mX == &gg(&mX, DATA[i].d_spec));
            int hash = Obj::hash(X, DATA[i].d_size);
            if (veryVerbose) {
                cout << DATA[i].d_spec
                     << " , " << DATA[i].d_size
                     << " ---> " << hash << endl;
            }
            ASSERTV(i, DATA[i].d_hash == hash);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            const Obj X;

            ASSERT_PASS(Obj::hash(X,  1));
            ASSERT_FAIL(Obj::hash(X,  0));
            ASSERT_FAIL(Obj::hash(X, -1));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'evaluate':
        //   The 'evaluate' method must return correct results as to whether
        //   every attribute in the set has an exact counterpart in the
        //   specified attribute set.
        //
        // Plan:
        //   Specify a set of pairs of a 'ball::ManagedAttributeSet' object and
        //   a 'DefaultAttributeContainer' object.  For each pair, verify that
        //   the 'evaluate' method returns the expected value.
        //
        // Testing:
        //   bool evaluate(const AttributeContainerList& containerList) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'evaluate'"
                          << "\n==================" << endl;

        static const struct {
            int         d_line;            // source line number
            const char *d_attributeSet;    // spec for the attribute set
            const char *d_attributeCont;   // spec for the attribute container
            bool        d_result;          // expected result
        } DATA[] = {
            // line   attributeSet         attributeCont        expected
            // ----   ------------         ------------         --------
            {  L_,    "",                  "",                  1           },
            {  L_,    "",                  "AA",                1           },
            {  L_,    "AA",                "AA",                1           },
            {  L_,    "AA",                "AB",                0           },
            {  L_,    "A1",                "Ai1",               0           },
            {  L_,    "A1",                "AI1",               0           },
            {  L_,    "Ai1",               "Ai1",               1           },
            {  L_,    "Ai1",               "Ai2",               0           },
            {  L_,    "Ai1",               "AI1",               0           },
            {  L_,    "AI1",               "AI1",               1           },
            {  L_,    "AI1",               "AI2",               0           },
            {  L_,    "AI1",               "Ai1",               0           },
            {  L_,    "AABA",              "BAAA",              1           },
            {  L_,    "BAAA",              "BAAA",              1           },
            {  L_,    "AABA",              "BAAC",              0           },
            {  L_,    "AABBCC",            "AABBCC",            1           },
            {  L_,    "AABBCC",            "AABBCD",            0           },
            {  L_,    "AABBCCDDEEFFGGHH",  "AABBCCDDEEFFGGHH",  1           },
            {  L_,    "AABBCCDDEEFFGGHH",  "AABBCCDDEEFFGGHA",  0           },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            int LINE = DATA[i].d_line;
            Obj mX; const Obj& X = mX;
            ASSERTV(LINE, &mX == &gg(&mX, DATA[i].d_attributeSet));

            ball::DefaultAttributeContainer mY;
            const ball::DefaultAttributeContainer& Y = mY;
            ASSERTV(LINE, &mY == &hh(&mY, DATA[i].d_attributeCont));

            if (veryVerbose) { P_(X); P(Y); }

            ball::AttributeContainerList list;
            const ball::AttributeContainerList& LIST = list;
            list.pushFront(&Y);
            ASSERTV(LINE, X.evaluate(LIST) == DATA[i].d_result);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'removeAll'
        //   The 'removeAll' should effectively empty the
        //   'ball::ManagedAttributeSet' object.
        //
        // Plan:
        //   Specify a set S of test vectors.  For each element in S, construct
        //   the corresponding 'ball::ManagedAttributeSet' object x using the
        //   'gg' function.  Copy x into another object y.  After calling
        //   'removeAll' on x, verify that the length of x is zero, none of
        //   attributes in y can be found in x.  Then reconstruct x using the
        //   'gg' function again, and verify that x == y.
        //
        // Testing:
        //  void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'"
                          << "\n===================" << endl;

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
            ASSERTV(i, j, &mX == &gg(&mX, SPECS[i]));

            Obj mY; const Obj& Y = mY;
            ASSERTV(i, j, &mY == &gg(&mY, SPECS[j]));

            mX.removeAll();

            ASSERTV(i, j, 0 == X.numAttributes());
            for (ball::ManagedAttributeSet::const_iterator iter = Y.begin();
                 iter != Y.end();
                 ++iter) {
                ASSERTV(i, j, false == X.isMember(*iter));
            }

            ASSERTV(i, j, &mX == &gg(&mX, SPECS[j]));
            ASSERTV(i, j, X == Y);
        }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'begin' and 'end'
        //   This will test the 'begin' and 'end' methods.
        //
        // Concerns:
        //   The 'begin and 'end' methods should return a range where each
        //   attribute in the attribute set appears exactly once.
        //
        // Plan:
        //   Construct an array consisting of 'ball::ManagedAttribute' objects
        //   having distinct values.  For each n in [0 .. N] where N is the
        //   maximum number of attributes tested, create an empty
        //   'ball::ManagedAttributeSet' object object and add the first n
        //   attributes to the set.  Verify that every added attribute appear
        //   in the set exactly once.
        //
        // Testing:
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'begin' and 'end' methods"
                          << "\n=================================" << endl;

        const ball::ManagedAttribute PREDS[] = { A0, A1, A2, A3, A4, A5, A6,
                                                 A7, A8 };
        const int NUM_PREDS = sizeof PREDS / sizeof *PREDS;

        int isPresentFlags[NUM_PREDS];

        for (int i = 0; i < NUM_PREDS; ++i) {
            Obj mX; const Obj& X = mX;

            int j, length;
            for (j = 0; j < i; ++j) {
                ASSERTV(i, j, mX.addAttribute(PREDS[j]));
                ASSERTV(i, j, X.isMember(PREDS[j]));
                isPresentFlags[j] = 0;
            }

            ASSERTV(i, j == X.numAttributes());

            length = 0;
            for (Obj::const_iterator iter = X.begin();
                 iter != X.end();
                 ++iter, ++length) {
                for (j = 0; j < i; ++j) {
                    if (*iter == PREDS[j]) {
                        ++isPresentFlags[j];
                    }
                }
            }

            ASSERTV(i, length == X.numAttributes());

            for (j = 0; j < i; ++j) {
                ASSERTV(i, j, 1 == isPresentFlags[j]);
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
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
        //   const ball::ManagedAttributeSet& operator=(const MAS& other);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR"
                          << "\n===========================" << endl;

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
                ASSERTV(LINE1, &mX == &gg(&mX, DATA[i].d_spec));

                Obj mY; const Obj& Y = mY;
                ASSERTV(LINE2, &mY == &gg(&mY, DATA[j].d_spec));

                Obj mW(Y); const Obj& W = mW;

                mX = Y;

                ASSERTV(LINE1, LINE2, Y == W);
                ASSERTV(LINE1, LINE2, W == Y);
                ASSERTV(LINE1, LINE2, X == W);
                ASSERTV(LINE1, LINE2, W == X);
            }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                          << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            if (veryVerbose) { P_(LINE); P_(DATA[i].d_spec); }

            Obj mX; const Obj& X = mX;
            ASSERTV(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            Obj mW(X); const Obj& W = mW;

            mX = X;

            ASSERTV(LINE, X == W);
            ASSERTV(LINE, W == X);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'ball::ManagedAttributeSet'.
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
        //   ball::ManagedAttributeSet(const ball::MAS&, allocator);
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
            ASSERTV(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            Obj mW; const Obj& W = mW;
            ASSERTV(LINE, &mW == &gg(&mW, DATA[i].d_spec));

            // construct y without an allocator
            {
                Obj mY(X); const Obj& Y = mY;

                ASSERTV(LINE, Y == W);
                ASSERTV(LINE, W == Y);
                ASSERTV(LINE, X == W);
                ASSERTV(LINE, W == X);
            }

            // construct y with an allocator but no exception
            {
                bslma::TestAllocator testAllocatorY(veryVeryVerbose);
                Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                ASSERTV(LINE, Y == W);
                ASSERTV(LINE, W == Y);
                ASSERTV(LINE, X == W);
                ASSERTV(LINE, W == X);
            }

            // construct y with an allocator and exceptions
            {
                bslma::TestAllocator testAllocatorY(veryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                    ASSERTV(LINE, Y == W);
                    ASSERTV(LINE, W == Y);
                    ASSERTV(LINE, X == W);
                    ASSERTV(LINE, W == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
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
        //   bool operator==(const ball::MAS& lhs, const ball::MAS& rhs)
        //   bool operator!=(const ball::MAS& lhs, const ball::MAS& rhs)
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
            ASSERTV(LINE1, &mX == &gg(&mX, DATA[i].d_spec));
            for (int j = 0; j < NUM_DATA; ++j) {
                const int LINE2 = DATA[j].d_line;

                if (veryVerbose) {
                    P_(LINE1);
                    P_(DATA[i].d_spec);
                    P_(LINE2);
                    P(DATA[j].d_spec);
                }

                Obj mY; const Obj& Y = mY;
                ASSERTV(LINE1, &mY == &gg(&mY, DATA[j].d_spec));

                ASSERTV(LINE1, LINE2, (i==j) == (X == Y));
                ASSERTV(LINE1, LINE2, (i!=j) == (X != Y));
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
            ASSERTV(LINE1, &mX == &gg(&mX, DATA[i].d_spec));

            mX.removeAll();

            ASSERTV(LINE1, LINE2, &mX == &gg(&mX, DATA[j].d_spec));

            Obj mY; const Obj& Y = mY;
            ASSERTV(LINE3, &mY == &gg(&mY, DATA[k].d_spec));

            if (veryVerbose) {
                P_(LINE1);
                P_(DATA[i].d_spec);
                P_(LINE2);
                P_(DATA[j].d_spec);
                P_(LINE3);
                P(DATA[k].d_spec);
            }

            ASSERTV(LINE1, LINE2, LINE3, (j == k) == (X == Y));
            ASSERTV(LINE1, LINE2, LINE3, (j != k) == (X != Y));
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
            ASSERTV(LINE, &mX == &gg(&mX, TDATA[i].d_spec1));

            Obj mY; const Obj& Y = mY;
            ASSERTV(LINE, &mY == &gg(&mY, TDATA[i].d_spec2));

            if (veryVerbose) {
                P_(LINE);
                P_(TDATA[i].d_spec1);
                P(TDATA[i].d_spec2);
            }

            ASSERTV(LINE, 1 == (X == Y));
            ASSERTV(LINE, 0 == (X != Y));
        }

        if (verbose) cout
            << "\nCompare w~u against u in each (w, u) in S X S."
            << endl;

        for (int i = 0; i < NUM_TDATA; ++i) {
        for (int j = 0; j < NUM_TDATA; ++j) {
            const int LINE1 = TDATA[i].d_line;
            const int LINE2 = TDATA[j].d_line;

            Obj mX; const Obj& X = mX;
            ASSERTV(LINE1, &mX == &gg(&mX, TDATA[i].d_spec1));

            mX.removeAll();

            ASSERTV(LINE1, LINE2, &mX == &gg(&mX, TDATA[j].d_spec1));

            Obj mY; const Obj& Y = mY;
            ASSERTV(LINE2, &mY == &gg(&mY, TDATA[j].d_spec2));

            if (veryVerbose) {
                P_(LINE1);
                P_(TDATA[i].d_spec1);
                P_(LINE2);
                P_(TDATA[j].d_spec1);
                P(TDATA[j].d_spec2);
            }

            ASSERTV(LINE1, LINE2, 1 == (X == Y));
            ASSERTV(LINE1, LINE2, 0 == (X != Y));
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
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::MAS&) const;
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
            {  L_,    "",           "[ ]"                                 },
            {  L_,    "AA",         "[  [ \"\" = A ] ]"                   },
            {  L_,    "Ai1",        "[  [ \"\" = 1 ] ]"                   },
            {  L_,    "BA",         "[  [ \"A\" = A ] ]"                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            Obj mX; const Obj& X = mX;
            ASSERTV(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            ostringstream os;
            os << X;
            ASSERTV(LINE, compareText(os.str(), DATA[i].d_output));

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
            {  L_,  "BA", 1,    2,    "  [\n"
                                      "          [ \"A\" = A ]\n"
                                      "  ]\n"  },
        };

        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        for (int i = 0; i < NUM_PDATA; ++i) {
            int LINE = PDATA[i].d_line;

            Obj mX; const Obj& X = mX;
            ASSERTV(LINE, &mX == &gg(&mX, PDATA[i].d_spec));

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                P_(LINE);
                P_(PDATA[i].d_spec);
                P(PDATA[i].d_output);
                P(os.str());
            }

            ASSERTV(LINE, compareText(os.str(), PDATA[i].d_output));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Every attribute added must be verified by 'isMember'.
        //
        // Plan:
        //   Mechanically generate a series of specifications that contain
        //   string attribute with names 'A' .. 'A' + NUM_NAMES and with
        //   values 'A' .. 'J', and integral attribute with name 'a' .. 'j'
        //   and with values 0 .. 9.  For each specification, create a
        //   attribute set from the specification, and verify that the
        //   attribute with the specified names having the specified values
        //   exist.
        //
        // Testing:
        //   bool isMember(const ball::ManagedAttribute&) const;
        //   int numAttributes() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl;

        for (int n = 0; n < NUM_NAMES; ++n) {
            bsl::string spec;
            for (int v = 0; v < n; ++v) {
                spec += static_cast<char>(n + 'A');  // add a string attribute

                // don't use 'I' as the string attribute value
                spec += v >= 'I' - 'A'
                      ? static_cast<char>(v + 'B')
                      : static_cast<char>(v + 'A');

                spec += static_cast<char>(n + 'A');  // add an int32 attribute
                spec += 'i';
                spec += static_cast<char>(v + '0');

                spec += static_cast<char>(n + 'A');  // add an int64 attribute
                spec += 'I';
                spec += static_cast<char>(v + '0');
            }

            Obj mX; const Obj& X = mX;
            ASSERTV(n, &mX == &gg(&mX, spec.c_str()));
            ASSERTV(n, X.numAttributes() == 3 * n);

            if (veryVerbose) { P_(n); P(spec); }

            for (int v = 0; v <= '9' - '0'; ++v) {
                bsl::string sValue;
                sValue = v >= 'I' - 'A'
                       ? static_cast<char>(v + 'B')
                       : static_cast<char>(v + 'A');
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], sValue.c_str())));

                int int32Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], int32Value)));

                long long int64Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], int64Value)));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //   The 'gg' function must create objects having the expected values.
        //
        // Plan:
        //   Mechanically generate a series of specifications that contain
        //   string attributes with names 'A' .. 'A' + NUM_NAMES and with
        //   values 'A' .. 'J', and integral attributes with name 'a' .. 'j'
        //   and with values 0 .. 9.  For each specification, create a
        //   attribute set from the specification, and verify that the
        //   attribute with the specified names having the specified values
        //   exist.
        //
        // Testing:
        //   Obj& gg(Obj *address, const char *spec);
        //   AttributeSet& hh(AttributeSet *obj, const char *spec);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "Testing 'gg' and 'hh' generator functions" << endl
            << "=========================================" << endl;

        if (veryVerbose) cout << "\n\tTesting 'gg'." << endl;

        for (int n = 0; n < NUM_NAMES; ++n) {
            bsl::string spec;
            for (int v = 0; v < n; ++v) {
                spec += static_cast<char>(n + 'A');  // add a string attribute

                // don't use 'I' as the string attribute value
                spec +=  v >= 'I' - 'A'
                      ? static_cast<char>(v + 'B')
                      : static_cast<char>(v + 'A');

                spec += static_cast<char>(n + 'A');  // add an int32 attribute
                spec += 'i';
                spec += static_cast<char>(v + '0');

                spec += static_cast<char>(n + 'A');  // add an int64 attribute
                spec += 'I';
                spec += static_cast<char>(v + '0');
            }

            Obj mX; const Obj& X = mX;
            ASSERTV(n, &mX == &gg(&mX, spec.c_str()));
            ASSERTV(n, X.numAttributes() == 3 * n);

            if (veryVerbose) { P_(n); P(spec); }

            for (int v = 0; v <= '9' - '0'; ++v) {
                bsl::string sValue;
                sValue = v >= 'I' - 'A'
                       ? static_cast<char>(v + 'B')
                       : static_cast<char>(v + 'A');
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], sValue.c_str())));

                int int32Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], int32Value)));

                long long int64Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.isMember(
                            ball::ManagedAttribute(NAMES[n], int64Value)));
            }
        }

        if (verbose) cout << "\n\tTesting 'hh'." << endl;

        for (int n = 0; n < NUM_NAMES; ++n) {
            bsl::string spec;
            for (int v = 0; v < n; ++v) {
                spec += static_cast<char>(n + 'A');  // add a string attribute

                // don't use 'I' as the string attribute value
                spec +=  v >= 'I' - 'A'
                      ? static_cast<char>(v + 'B')
                      : static_cast<char>(v + 'A');

                spec += static_cast<char>(n + 'A');  // add an int32 attribute
                spec += 'i';
                spec += static_cast<char>(v + '0');

                spec += static_cast<char>(n + 'A');  // add an int64 attribute
                spec += 'I';
                spec += static_cast<char>(v + '0');
            }

            AttributeSet mX; const AttributeSet& X = mX;
            ASSERTV(n, &mX == &hh(&mX, spec.c_str()));
            ASSERTV(n, X.numAttributes() == 3 * n);

            if (veryVerbose) { P_(n); P(spec); }

            for (int v = 0; v <= '9' - '0'; ++v) {
                bsl::string sValue;
                sValue =  v >= 'I' - 'A'
                       ? static_cast<char>(v + 'B')
                       : static_cast<char>(v + 'A');
                ASSERTV(n,
                        v,
                        (v < n) == X.hasValue(
                            ball::Attribute(NAMES[n], sValue.c_str())));

                int int32Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.hasValue(
                            ball::Attribute(NAMES[n], int32Value)));

                long long int64Value = v;
                ASSERTV(n,
                        v,
                        (v < n) == X.hasValue(
                            ball::Attribute(NAMES[n], int64Value)));
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
        //   ball::ManagedAttributeSet();
        //   ball::ManagedAttributeSet(const allocator_type& allocator);
        //   bool addAttribute(const ball::ManagedAttribute& attribute);
        //   bool removeAttribute(const ball::ManagedAttribute& attribute);
        //   ~ball::ManagedAttributeSet();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X;
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
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) cout << "\nTesting primary manipulator." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;  const Obj& X = mX;

            Obj mY;  const Obj& Y = mY;
            ASSERT(mY.addAttribute(A1));

            Obj mZ;  const Obj& Z = mZ;
            ASSERT(mZ.addAttribute(A2));
            ASSERT(mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.isMember(A1));
            ASSERT(Z.isMember(A2));
            ASSERT(Z.isMember(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.isMember(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(mX.removeAttribute(A1));
            ASSERT(false == X.isMember(A1));
            ASSERT(mX.addAttribute(A2));
            ASSERT(mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.isMember(A2));
            ASSERT(X.isMember(A3));
            ASSERT(X == Z);
        }

        if (verbose) cout << "\tWith an allocator." << endl;
        if (verbose) cout << "\t\tWithout exceptions." << endl;
        {
            bslma::TestAllocator testAllocatorX(veryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVerbose);

            Obj mX(&testAllocatorX);  const Obj& X = mX;

            Obj mY(&testAllocatorY);  const Obj& Y = mY;
            ASSERT(mY.addAttribute(A1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(mZ.addAttribute(A2));
            ASSERT(mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.isMember(A1));
            ASSERT(Z.isMember(A2));
            ASSERT(Z.isMember(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.isMember(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(mX.removeAttribute(A1));
            ASSERT(!X.isMember(A1));
            ASSERT(mX.addAttribute(A2));
            ASSERT(mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.isMember(A2));
            ASSERT(X.isMember(A3));
            ASSERT(X == Z);
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            bslma::TestAllocator testAllocatorX(veryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVerbose);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Primary Manipulator" << endl;

            Obj mX(&testAllocatorX);  const Obj& X = mX;

            Obj mY(&testAllocatorY);  const Obj& Y = mY;
            ASSERT(mY.addAttribute(A1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(mZ.addAttribute(A2));
            ASSERT(mZ.addAttribute(A3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numAttributes());
            ASSERT(1 == Y.numAttributes());
            ASSERT(2 == Z.numAttributes());
            ASSERT(Y.isMember(A1));
            ASSERT(Z.isMember(A2));
            ASSERT(Z.isMember(A3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(mX.addAttribute(A1));
            ASSERT(1 == X.numAttributes());
            ASSERT(X.isMember(A1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(mX.removeAttribute(A1));
            ASSERT(!X.isMember(A1));
            ASSERT(mX.addAttribute(A2));
            ASSERT(mX.addAttribute(A3));
            ASSERT(2 == X.numAttributes());
            ASSERT(X.isMember(A2));
            ASSERT(X.isMember(A3));
            ASSERT(X == Z);

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
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
        mX2.removeAll();
        mX2.addAttribute(A3);
        mX2.addAttribute(A4);
        ASSERT(2 == X2.numAttributes());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to 0." << endl;
        mX1.removeAll();
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
