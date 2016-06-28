// bsltf_emplacabletesttype.t.cpp                                     -*-C++-*-
#include <bsltf_emplacabletesttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_objectbuffer.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <new>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
|| (defined(BSLS_PLATFORM_CMP_CLANG) && !defined(__GXX_EXPERIMENTAL_CXX0X__)) \
|| (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR < 1800)

# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD:
// Global Concerns:
//: o No memory is ever allocated from this component.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] EmplacableTestType();
// [ 3] EmplacableTestType(a01);
// [ 3] EmplacableTestType(a01 .. a02);
// [ 3] EmplacableTestType(a01 .. a03);
// [ 3] EmplacableTestType(a01 .. a04);
// [ 3] EmplacableTestType(a01 .. a05);
// [ 3] EmplacableTestType(a01 .. a06);
// [ 3] EmplacableTestType(a01 .. a07);
// [ 3] EmplacableTestType(a01 .. a08);
// [ 3] EmplacableTestType(a01 .. a09);
// [ 3] EmplacableTestType(a01 .. a10);
// [ 3] EmplacableTestType(a01 .. a11);
// [ 3] EmplacableTestType(a01 .. a12);
// [ 3] EmplacableTestType(a01 .. a13);
// [ 3] EmplacableTestType(a01 .. a14);
// [  ] ~EmplacableTestType();
//
// ACCESSORS
// [ 3] ArgType01 arg01() const;
// [ 3] ArgType02 arg02() const;
// [ 3] ArgType03 arg03() const;
// [ 3] ArgType04 arg04() const;
// [ 3] ArgType05 arg05() const;
// [ 3] ArgType06 arg06() const;
// [ 3] ArgType07 arg07() const;
// [ 3] ArgType08 arg08() const;
// [ 3] ArgType09 arg09() const;
// [ 3] ArgType10 arg10() const;
// [ 3] ArgType11 arg11() const;
// [ 3] ArgType12 arg12() const;
// [ 3] ArgType13 arg13() const;
// [ 3] ArgType14 arg14() const;
// [ 8] bool isEqual(const EmplacableTestType& lhs);
//
// FREE OPERATORS
// [ 6] bool operator==(const Obj& lhs, const Obj& rhs);
// [ 6] bool operator!=(const Obj& lhs, const Obj& rhs);
//
// CLASS METHODS
// [ 1] static getNumDeletes();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: No memory is ever allocated.

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
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef EmplacableTestType Obj;

static const EmplacableTestType::ArgType01 V01(1);
static const EmplacableTestType::ArgType02 V02(20);
static const EmplacableTestType::ArgType03 V03(23);
static const EmplacableTestType::ArgType04 V04(44);
static const EmplacableTestType::ArgType05 V05(66);
static const EmplacableTestType::ArgType06 V06(176);
static const EmplacableTestType::ArgType07 V07(878);
static const EmplacableTestType::ArgType08 V08(8);
static const EmplacableTestType::ArgType09 V09(912);
static const EmplacableTestType::ArgType10 V10(102);
static const EmplacableTestType::ArgType11 V11(111);
static const EmplacableTestType::ArgType12 V12(333);
static const EmplacableTestType::ArgType13 V13(712);
static const EmplacableTestType::ArgType14 V14(1414);

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_group;    // equality group
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec
    //---- ---  ----------------
    //           12345678901234
    { L_,   0,  ""                },
    { L_,   0,  " "               },
    { L_,   1,  "A"               },
    { L_,   2,  "B"               },
    { L_,   1,  "A "              },
    { L_,   3,  "AA"              },
    { L_,   4,  "AB"              },
    { L_,   4,  "AB "             },
    { L_,   5,  "ABB"             },
    { L_,   6,  "ABC"             },
    { L_,   6,  "ABC "            },
    { L_,   7,  "ABCC"            },
    { L_,   8,  "ABCD"            },
    { L_,   8,  "ABCD "           },
    { L_,   9,  "ABCDD"           },
    { L_,  10,  "ABCDE"           },
    { L_,  10,  "ABCDE "          },
    { L_,  11,  "ABCDEE"          },
    { L_,  12,  "ABCDEF"          },
    { L_,  12,  "ABCDEF "         },
    { L_,  13,  "ABCDEFF"         },
    { L_,  14,  "ABCDEFG"         },
    { L_,  14,  "ABCDEFG "        },
    { L_,  15,  "ABCDEFGG"        },
    { L_,  16,  "ABCDEFGH"        },
    { L_,  16,  "ABCDEFGH "       },
    { L_,  17,  "ABCDEFGHH"       },
    { L_,  18,  "ABCDEFGHI"       },
    { L_,  18,  "ABCDEFGHI "      },
    { L_,  19,  "ABCDEFGHII"      },
    { L_,  20,  "ABCDEFGHIJ"      },
    { L_,  20,  "ABCDEFGHIJ "     },
    { L_,  21,  "ABCDEFGHIJJ"     },
    { L_,  22,  "ABCDEFGHIJK"     },
    { L_,  22,  "ABCDEFGHIJK "    },
    { L_,  23,  "ABCDEFGHIJKK"    },
    { L_,  24,  "ABCDEFGHIJKL"    },
    { L_,  24,  "ABCDEFGHIJKL "   },
    { L_,  25,  "ABCDEFGHIJKLL"   },
    { L_,  26,  "ABCDEFGHIJKLM"   },
    { L_,  26,  "ABCDEFGHIJKLM "  },
    { L_,  27,  "ABCDEFGHIJKLMN"  },

};
static const size_t DEFAULT_NUM_DATA =
                                    sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

                       // ================
                       // class TestDriver
                       // ================

class TestDriver {
    // This class provide a namespace for testing the 'EmplacableTestType'.
    // Each "testCase*" method tests a specific aspect of 'EmplacableTestType'.

  private:
    // TYPES

    // Shorthands
    typedef EmplacableTestType            Obj;
    typedef Obj::ArgType01                ArgType01;
    typedef Obj::ArgType02                ArgType02;
    typedef Obj::ArgType03                ArgType03;
    typedef Obj::ArgType04                ArgType04;
    typedef Obj::ArgType05                ArgType05;
    typedef Obj::ArgType06                ArgType06;
    typedef Obj::ArgType07                ArgType07;
    typedef Obj::ArgType08                ArgType08;
    typedef Obj::ArgType09                ArgType09;
    typedef Obj::ArgType10                ArgType10;
    typedef Obj::ArgType11                ArgType11;
    typedef Obj::ArgType12                ArgType12;
    typedef Obj::ArgType13                ArgType13;
    typedef Obj::ArgType14                ArgType14;

    typedef bslmf::MovableRefUtil         MoveUtil;
  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to create the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be used as the constructor arguments at the same position.
    // Character ' ' (space) corresponds to a default-constructed argument
    // value.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::= ""
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z' | <DEFAULT>
    //                  // unique but otherwise arbitrary
    //
    // <DEFAULT>    ::= ' ' (space)
    //                  // Default-constructed value
    //
    // For specification string of length 'N' use object constructor taking
    // exactly 'N' arguments with values corresponding to the character at the
    // character's position.
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Construct default object.
    // "A"          Construct the object with a single argument corresponding
    //              to A.
    // "ABC"        Construct the object with three arguments corresponding to
    //              A, B and C, respectively.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Construct the specified 'object' according to the specified 'spec',
        // using the object constructor.  Optionally specify a zero 'verbose'
        // to suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value
        // constructed according to the specified 'spec'.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return  t;
    }

  public:
    // TEST CASES
    static void testCase8();
        // Test 'isEqual' method.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality and inequality operators ('operator==', 'operator!=').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase2();
        // Test value constructors for the specified (template parameter)
        // number of arguments.  See the test case function for documented
        // concerns and test plan.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

int TestDriver::ggg(Obj *object, const char *spec, int verbose)
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ( (spec[i] < 'A' || spec[i] > 'Z') && spec[i] != ' ' ) {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            // Discontinue processing this spec.
            return i;                                                 // RETURN
        }
    }

    size_t LENGTH = strlen(spec);
    ArgType01 A01 = LENGTH >  0 && spec[ 0] != ' ' ?
                                       ArgType01(spec[ 0] - 'A') : ArgType01();
    ArgType02 A02 = LENGTH >  1 && spec[ 1] != ' ' ?
                                       ArgType02(spec[ 1] - 'A') : ArgType02();
    ArgType03 A03 = LENGTH >  2 && spec[ 2] != ' ' ?
                                       ArgType03(spec[ 2] - 'A') : ArgType03();
    ArgType04 A04 = LENGTH >  3 && spec[ 3] != ' ' ?
                                       ArgType04(spec[ 3] - 'A') : ArgType04();
    ArgType05 A05 = LENGTH >  4 && spec[ 4] != ' ' ?
                                       ArgType05(spec[ 4] - 'A') : ArgType05();
    ArgType06 A06 = LENGTH >  5 && spec[ 5] != ' ' ?
                                       ArgType06(spec[ 5] - 'A') : ArgType06();
    ArgType07 A07 = LENGTH >  6 && spec[ 6] != ' ' ?
                                       ArgType07(spec[ 6] - 'A') : ArgType07();
    ArgType08 A08 = LENGTH >  7 && spec[ 7] != ' ' ?
                                       ArgType08(spec[ 7] - 'A') : ArgType08();
    ArgType09 A09 = LENGTH >  8 && spec[ 8] != ' ' ?
                                       ArgType09(spec[ 8] - 'A') : ArgType09();
    ArgType10 A10 = LENGTH >  9 && spec[ 9] != ' ' ?
                                       ArgType10(spec[ 9] - 'A') : ArgType10();
    ArgType11 A11 = LENGTH > 10 && spec[10] != ' ' ?
                                       ArgType11(spec[10] - 'A') : ArgType11();
    ArgType12 A12 = LENGTH > 11 && spec[11] != ' ' ?
                                       ArgType12(spec[11] - 'A') : ArgType12();
    ArgType13 A13 = LENGTH > 12 && spec[12] != ' ' ?
                                       ArgType13(spec[12] - 'A') : ArgType13();
    ArgType14 A14 = LENGTH > 13 && spec[13] != ' ' ?
                                       ArgType14(spec[13] - 'A') : ArgType14();

    switch (LENGTH) {
      case 0: {
        new(object) Obj();
      } break;
      case 1: {
        new (object) Obj(A01);
      } break;
      case 2: {
        new (object) Obj(A01, A02);
      } break;
      case 3: {
        new (object) Obj(A01, A02, A03);
      } break;
      case 4: {
        new (object) Obj(A01, A02, A03, A04);
      } break;
      case 5: {
        new (object) Obj(A01, A02, A03, A04, A05);
      } break;
      case 6: {
        new (object) Obj(A01, A02, A03, A04, A05, A06);
      } break;
      case 7: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07);
      } break;
      case 8: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08);
      } break;
      case 9: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09);
      } break;
      case 10: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10);
      } break;
      case 11: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11);
      } break;
      case 12: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12);
      } break;
      case 13: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12, A13);
      } break;
      case 14: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12, A13, A14);
      } break;
      default: {
        ASSERTV(0);
      } break;
    }

    return SUCCESS;
}

Obj& TestDriver::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

void TestDriver::testCase8()
{
    // ---------------------------------------------------------------------
    // TESTING 'isEqual' METHOD:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', are equal if and only if they contain
    //:   the same values.
    //:
    //: 2 'true == (X.isEqual(X))' (i.e., identity)
    //:
    //: 3 'true == X.isEqual(Y)' if and only if 'true == Y.isEqual(X)'
    //:   (i.e., commutativity)
    //:
    //: 4 'false == X.isEqual(Y)' if and only if 'false == Y.isEqual(X)'
    //:   (i.e., commutativity)
    //:
    //: 5 Method can be called for non-modifiable objects(i.e., objects or
    //:   references providing only non-modifiable access).
    //
    // Plan:
    //: 1 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 2 For each row 'R1' in the table of P-2: (C-1..5)
    //:
    //:   1 Create a single object, and use it to verify the reflexive
    //:     (anti-reflexive) property of 'isEqual' in the presence of
    //:     aliasing.  (C-2)
    //:
    //:   2 For each row 'R2' in the table of P-2: (C-1..5)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 Create an object 'X', having the value 'R1'.
    //:
    //:     3 Create an object 'Y', having the value 'R2'.
    //:
    //:     4 Verify the commutativity property and expected return value for
    //:       'isEqual'.  (C-1..5)
    //
    // Testing:
    //   bool isEqual(const EmplacableTestType& lhs);
    // ------------------------------------------------------------------------

    if (verbose)
              printf("\nAssign the address of 'isEqual' to a variable.\n");
    {
        typedef bool (Obj::*methodPtr)(const Obj&) const;

        // Verify that the signature and return type are correct..
        methodPtr methodIsEqual = &EmplacableTestType::isEqual;

        (void) methodIsEqual;  // quash potential compiler warnings
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1  = DATA[ti].d_line;
            const int         GROUP1 = DATA[ti].d_group;
            const char *const SPEC1  = DATA[ti].d_spec;

           if (veryVerbose) { T_ P_(LINE1) P_(GROUP1) P(SPEC1) }

            // Ensure an object is equal to itself (alias test).
            {
                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                ASSERTV(LINE1, true == X.isEqual(X));
            }

            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         GROUP2 = DATA[tj].d_group;
                const char *const SPEC2  = DATA[tj].d_spec;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(GROUP2) P(SPEC2) }

                const bool EXP = GROUP1 == GROUP2;  // expected result

                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                bsls::ObjectBuffer<Obj> bufferY;
                const Obj& Y = gg(bufferY.address(), SPEC2);
                bslma::DestructorGuard<Obj> guardY(&bufferY.object());

                // Verify value and commutativity.
                ASSERTV(LINE1, LINE2,  EXP == X.isEqual(Y));
                ASSERTV(LINE1, LINE2,  EXP == Y.isEqual(X));
            }
        }
    }
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator).
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //
    // Testing:
    //   EmplacableTestType(const EmplacableTestType& original);
    // ------------------------------------------------------------------------

    {
        static const char *SPECS[] = {
            "",
            "A",
            "AB",
            "ABC",
            "ABCD",
            "ABCDE",
            "ABCDEF",
            "ABCDEFG",
            "ABCDEFGH",
            "ABCDEFGHI",
            "ABCDEFGHIJ",
            "ABCDEFGHIJK",
            "ABCDEFGHIJKL",
            "ABCDEFGHIJKLM",
            "ABCDEFGHIJKLMN",
        };

        const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];

            if (veryVerbose) { P(SPEC); }

            // Create control object w.
            bsls::ObjectBuffer<Obj> bufferW;
            const Obj& W = gg(bufferW.address(), SPEC);
            bslma::DestructorGuard<Obj> guardW(&bufferW.object());

            bsls::ObjectBuffer<Obj> bufferX;
            const Obj& X = gg(bufferX.address(), SPEC);
            bslma::DestructorGuard<Obj> guardX(&bufferX.object());

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
        }
    }
}

void TestDriver::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 'true  == (X == X)' (i.e., identity)
    //:
    //: 3 'false == (X != X)' (i.e., identity)
    //:
    //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if '!(X == Y)'
    //:
    //: 7 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 8 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //: 9 The equality operator's signature and return type are standard.
    //:
    //:10 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality-comparison
    //:   operators defined in this component.  (C-7..10)
    //:
    //: 2 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 3 For each row 'R1' in the table of P-2: (C-1..7)
    //:
    //:   1 Create a single object, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-2..3)
    //:
    //:   2 For each row 'R2' in the table of P-2: (C-1..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 Create an object 'X', having the value 'R1'.
    //:
    //:     3 Create an object 'Y', having the value 'R2'.
    //:
    //:     4 Verify the commutativity property and expected return value for
    //:       both '==' and '!='.  (C-1..7)
    //
    // Testing:
    //   bool operator==(Obj& lhs, Obj& rhs);
    //   bool operator!=(Obj& lhs, Obj& rhs);
    // ------------------------------------------------------------------------

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1  = DATA[ti].d_line;
            const int         GROUP1 = DATA[ti].d_group;
            const char *const SPEC1  = DATA[ti].d_spec;

           if (veryVerbose) { T_ P_(LINE1) P_(GROUP1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                ASSERTV(LINE1,   X == X);
                ASSERTV(LINE1, !(X != X));
            }

            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         GROUP2 = DATA[tj].d_group;
                const char *const SPEC2  = DATA[tj].d_spec;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(GROUP2) P(SPEC2) }

                const bool EXP = GROUP1 == GROUP2;  // expected result

                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                bsls::ObjectBuffer<Obj> bufferY;
                const Obj& Y = gg(bufferY.address(), SPEC2);
                bslma::DestructorGuard<Obj> guardY(&bufferY.object());

                // Verify value and commutativity.
                ASSERTV(LINE1, LINE2,  EXP == (X == Y));
                ASSERTV(LINE1, LINE2,  EXP == (Y == X));

                ASSERTV(LINE1, LINE2, !EXP == (X != Y));
                ASSERTV(LINE1, LINE2, !EXP == (Y != X));
            }
        }
    }
}

void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //   Obj& gg(Obj *object, const char *spec);
    //   int ggg(Obj *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;

            bsls::ObjectBuffer<Obj> bufferX;
            const Obj& X = gg(bufferX.address(), SPEC);   // original spec
            bslma::DestructorGuard<Obj> guardX(&bufferX.object());

            bsls::ObjectBuffer<Obj> bufferY;
            const Obj& Y = gg(bufferY.address(), SPEC);   // extended spec
            bslma::DestructorGuard<Obj> guardY(&bufferY.object());

            // TBD: we use yet untested operator== to compare
            ASSERTV(LINE, X == Y);
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // valid

            { L_,   "A",      -1,     }, // valid
            { L_,   " ",      -1,     }, // valid
            { L_,   ".",       0,     },
            { L_,   "E",      -1,     }, // valid
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // valid
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",     -1,     }, // valid

            { L_,   "ABC",    -1,     }, // valid
            { L_,   " BC",    -1,     }, // valid
            { L_,   ".BC",     0,     },
            { L_,   "A C",    -1,     }, // valid
            { L_,   "A.C",     1,     },
            { L_,   "AB ",    -1,     }, // valid
            { L_,   "AB.",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",    -1,     }, // valid

            { L_,   "ABCDE",  -1,     }, // valid
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;

            if (veryVerbose) { P(SPEC) };

            bsls::ObjectBuffer<Obj> bufferX;

            int RESULT = ggg(bufferX.address(), SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void TestDriver::testCase2()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

    bslma::TestAllocatorMonitor dam(da);

    // In C++17 these become the simpler to name 'bool_constant'
    static const bsl::integral_constant<bool, N01 == 1> MOVE_01 = {};
    static const bsl::integral_constant<bool, N02 == 1> MOVE_02 = {};
    static const bsl::integral_constant<bool, N03 == 1> MOVE_03 = {};
    static const bsl::integral_constant<bool, N04 == 1> MOVE_04 = {};
    static const bsl::integral_constant<bool, N05 == 1> MOVE_05 = {};
    static const bsl::integral_constant<bool, N06 == 1> MOVE_06 = {};
    static const bsl::integral_constant<bool, N07 == 1> MOVE_07 = {};
    static const bsl::integral_constant<bool, N08 == 1> MOVE_08 = {};
    static const bsl::integral_constant<bool, N09 == 1> MOVE_09 = {};
    static const bsl::integral_constant<bool, N10 == 1> MOVE_10 = {};
    static const bsl::integral_constant<bool, N11 == 1> MOVE_11 = {};
    static const bsl::integral_constant<bool, N12 == 1> MOVE_12 = {};
    static const bsl::integral_constant<bool, N13 == 1> MOVE_13 = {};
    static const bsl::integral_constant<bool, N14 == 1> MOVE_14 = {};

    ArgType01 A01(V01);
    ArgType02 A02(V02);
    ArgType03 A03(V03);
    ArgType04 A04(V04);
    ArgType05 A05(V05);
    ArgType06 A06(V06);
    ArgType07 A07(V07);
    ArgType08 A08(V08);
    ArgType09 A09(V09);
    ArgType10 A10(V10);
    ArgType11 A11(V11);
    ArgType12 A12(V12);
    ArgType13 A13(V13);
    ArgType14 A14(V14);

    bsls::ObjectBuffer<Obj> buffer;
    const Obj& EXP = buffer.object();

    switch (N_ARGS) {
      case 0: {
        new(buffer.address()) Obj();
      } break;
      case 1: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01));
      } break;
      case 2: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02));
      } break;
      case 3: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03));
      } break;
      case 4: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04));
      } break;
      case 5: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05));
      } break;
      case 6: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06));
      } break;
      case 7: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07));
      } break;
      case 8: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08));
      } break;
      case 9: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09));
      } break;
      case 10: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10));
      } break;
      case 11: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11));
      } break;
      case 12: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12));
      } break;
      case 13: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12),
                                   testArg(A13, MOVE_13));
      } break;
      case 14: {
        new (buffer.address()) Obj(testArg(A01, MOVE_01),
                                   testArg(A02, MOVE_02),
                                   testArg(A03, MOVE_03),
                                   testArg(A04, MOVE_04),
                                   testArg(A05, MOVE_05),
                                   testArg(A06, MOVE_06),
                                   testArg(A07, MOVE_07),
                                   testArg(A08, MOVE_08),
                                   testArg(A09, MOVE_09),
                                   testArg(A10, MOVE_10),
                                   testArg(A11, MOVE_11),
                                   testArg(A12, MOVE_12),
                                   testArg(A13, MOVE_13),
                                   testArg(A14, MOVE_14));
      } break;
      default: {
        ASSERTV(0);
      } break;
    }
    bslma::DestructorGuard<Obj> guard(&buffer.object());

    ASSERTV(MOVE_01, A01.movedFrom(), MOVE_01 == A01.movedFrom());
    ASSERTV(MOVE_02, A02.movedFrom(), MOVE_02 == A02.movedFrom());
    ASSERTV(MOVE_03, A03.movedFrom(), MOVE_03 == A03.movedFrom());
    ASSERTV(MOVE_04, A04.movedFrom(), MOVE_04 == A04.movedFrom());
    ASSERTV(MOVE_05, A05.movedFrom(), MOVE_05 == A05.movedFrom());
    ASSERTV(MOVE_06, A06.movedFrom(), MOVE_06 == A06.movedFrom());
    ASSERTV(MOVE_07, A07.movedFrom(), MOVE_07 == A07.movedFrom());
    ASSERTV(MOVE_08, A08.movedFrom(), MOVE_08 == A08.movedFrom());
    ASSERTV(MOVE_09, A09.movedFrom(), MOVE_09 == A09.movedFrom());
    ASSERTV(MOVE_10, A10.movedFrom(), MOVE_10 == A10.movedFrom());
    ASSERTV(MOVE_11, A11.movedFrom(), MOVE_11 == A11.movedFrom());
    ASSERTV(MOVE_12, A12.movedFrom(), MOVE_12 == A12.movedFrom());
    ASSERTV(MOVE_13, A13.movedFrom(), MOVE_13 == A13.movedFrom());
    ASSERTV(MOVE_14, A14.movedFrom(), MOVE_14 == A14.movedFrom());

    ASSERTV(V01, EXP.arg01(), V01 == EXP.arg01() || 2 == N01);
    ASSERTV(V02, EXP.arg02(), V02 == EXP.arg02() || 2 == N02);
    ASSERTV(V03, EXP.arg03(), V03 == EXP.arg03() || 2 == N03);
    ASSERTV(V04, EXP.arg04(), V04 == EXP.arg04() || 2 == N04);
    ASSERTV(V05, EXP.arg05(), V05 == EXP.arg05() || 2 == N05);
    ASSERTV(V06, EXP.arg06(), V06 == EXP.arg06() || 2 == N06);
    ASSERTV(V07, EXP.arg07(), V07 == EXP.arg07() || 2 == N07);
    ASSERTV(V08, EXP.arg08(), V08 == EXP.arg08() || 2 == N08);
    ASSERTV(V09, EXP.arg09(), V09 == EXP.arg09() || 2 == N09);
    ASSERTV(V10, EXP.arg10(), V10 == EXP.arg10() || 2 == N10);
    ASSERTV(V11, EXP.arg11(), V11 == EXP.arg11() || 2 == N11);
    ASSERTV(V12, EXP.arg12(), V12 == EXP.arg12() || 2 == N12);
    ASSERTV(V13, EXP.arg13(), V13 == EXP.arg13() || 2 == N13);
    ASSERTV(V14, EXP.arg14(), V14 == EXP.arg14() || 2 == N14);

    ASSERT(dam.isMaxSame());
    ASSERT(dam.isInUseSame());
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual' METHOD
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'isEqual' method"
                            "\n========================\n");

        TestDriver::testCase8();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        TestDriver::testCase7();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        TestDriver::testCase6();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");
        // TBD: Effectively, tested in case 2.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg' and 'ggg'"
                            "\n======================\n");

        TestDriver::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the documented constraints.
        //
        // Plan:
        //: 1 Use value constructor to create and object, having the value
        //:   from a set of distinct object values in terms of their
        //:   attributes.
        //:
        //: 2 Use the (as yet unproven) salient attribute accessors to verify
        //:   the attributes of the object have their expected value.  (C-1)
        //
        // Testing:
        //   EmplacableTestType();
        //   EmplacableTestType(a01);
        //   EmplacableTestType(a01 .. a02);
        //   EmplacableTestType(a01 .. a03);
        //   EmplacableTestType(a01 .. a04);
        //   EmplacableTestType(a01 .. a05);
        //   EmplacableTestType(a01 .. a06);
        //   EmplacableTestType(a01 .. a07);
        //   EmplacableTestType(a01 .. a08);
        //   EmplacableTestType(a01 .. a09);
        //   EmplacableTestType(a01 .. a10);
        //   EmplacableTestType(a01 .. a11);
        //   EmplacableTestType(a01 .. a12);
        //   EmplacableTestType(a01 .. a13);
        //   EmplacableTestType(a01 .. a14);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTOR"
                            "\n=========================\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\nTesting contructor with no arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 1 argument"
                            "\n----------------------------------\n");
        TestDriver::testCase2<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 2 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 3 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 4 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 5 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 6 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 7 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 8 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        TestDriver::testCase2<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        TestDriver::testCase2<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 9 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        TestDriver::testCase2<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        TestDriver::testCase2<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 10 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        TestDriver::testCase2<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 11 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        TestDriver::testCase2<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting contructor with 12 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        TestDriver::testCase2<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting contructor with 13 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        TestDriver::testCase2<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\nTesting contructor with 14 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        TestDriver::testCase2<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else
        TestDriver::testCase2< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif
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
        //: 1 Perform an ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            Obj mX; const Obj& X = mX;
            ASSERTV(X.arg01(), -1 == X.arg01());
        }
        ASSERTV( Obj::getNumDeletes(), 1 == Obj::getNumDeletes());

        {
            Obj mX(Obj::ArgType01(2)); const Obj& X = mX;
            ASSERTV(X.arg01(), Obj::ArgType01(2) == X.arg01());
            ASSERTV(X.arg02(), Obj::ArgType02()  == X.arg02());
            ASSERTV(X.arg03(), Obj::ArgType03()  == X.arg03());
            ASSERTV(X.arg04(), Obj::ArgType04()  == X.arg04());
            ASSERTV(X.arg05(), Obj::ArgType05()  == X.arg05());
            ASSERTV(X.arg06(), Obj::ArgType06()  == X.arg06());
            ASSERTV(X.arg07(), Obj::ArgType07()  == X.arg07());
            ASSERTV(X.arg08(), Obj::ArgType08()  == X.arg08());
            ASSERTV(X.arg09(), Obj::ArgType09()  == X.arg09());
            ASSERTV(X.arg10(), Obj::ArgType10()  == X.arg10());
            ASSERTV(X.arg11(), Obj::ArgType11()  == X.arg11());
            ASSERTV(X.arg12(), Obj::ArgType12()  == X.arg12());
            ASSERTV(X.arg13(), Obj::ArgType13()  == X.arg13());
            ASSERTV(X.arg14(), Obj::ArgType14()  == X.arg14());
        }
        ASSERTV( Obj::getNumDeletes(), 2 == Obj::getNumDeletes());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
