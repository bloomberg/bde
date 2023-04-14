// bsltf_emplacabletesttype.t.cpp                                     -*-C++-*-
#include <bsltf_emplacabletesttype.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <limits.h>
#include <new>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
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
// The component under test implements a type that is used to ensure that
// arguments are forwarded correctly to a type's constructor.
//
// Logically, this single test type represents 15 different test types (each
// having a single value constructor with 0..14 arguments, respectively).  For
// this reason, all constructors (except the copy constructor) are tested in
// 'case 2' (Primary Manipulators).

// In a possible (alternative) implementation, the attributes should have
// "nullable" behavior (i.e., if an argument is not passed to a constructor,
// the corresponding attribute should behave as if it does not exist) and, for
// example, an object constructed with 2 arguments should never compare equal
// with any other object constructed with a different number of arguments.
// The existing implementation lacks this functionality, but is sufficient to
// test upper-level components.
//
// Global Concerns:
//: o No memory is ever allocated.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [12] static int getNumDeletes();
//
// CREATORS
// [ 2] EmplacableTestType();
// [ 2] EmplacableTestType(arg01);
// [ 2] EmplacableTestType(arg01 .. arg02);
// [ 2] EmplacableTestType(arg01 .. arg03);
// [ 2] EmplacableTestType(arg01 .. arg04);
// [ 2] EmplacableTestType(arg01 .. arg05);
// [ 2] EmplacableTestType(arg01 .. arg06);
// [ 2] EmplacableTestType(arg01 .. arg07);
// [ 2] EmplacableTestType(arg01 .. arg08);
// [ 2] EmplacableTestType(arg01 .. arg09);
// [ 2] EmplacableTestType(arg01 .. arg10);
// [ 2] EmplacableTestType(arg01 .. arg11);
// [ 2] EmplacableTestType(arg01 .. arg12);
// [ 2] EmplacableTestType(arg01 .. arg13);
// [ 2] EmplacableTestType(arg01 .. arg14);
// [ 7] EmplacableTestType(const EmplacableTestType& other);
// [ 2] ~EmplacableTestType();
//
// ACCESSORS
// [ 4] ArgType01 arg01() const;
// [ 4] ArgType02 arg02() const;
// [ 4] ArgType03 arg03() const;
// [ 4] ArgType04 arg04() const;
// [ 4] ArgType05 arg05() const;
// [ 4] ArgType06 arg06() const;
// [ 4] ArgType07 arg07() const;
// [ 4] ArgType08 arg08() const;
// [ 4] ArgType09 arg09() const;
// [ 4] ArgType10 arg10() const;
// [ 4] ArgType11 arg11() const;
// [ 4] ArgType12 arg12() const;
// [ 4] ArgType13 arg13() const;
// [ 4] ArgType14 arg14() const;
// [11] bool isEqual(const EmplacableTestType& other) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Obj& lhs, const Obj& rhs);
// [ 6] bool operator!=(const Obj& lhs, const Obj& rhs);
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
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

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
    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil         MoveUtil;

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
    //              A, B, and C, respectively.
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
    static void testCase12();
        // Test 'getNumDeletes' class method.

    static void testCase11();
        // Test 'isEqual' method.

    static void testCase9();
        // Test copy-assignment operator.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality and inequality operators ('operator==', 'operator!=').

    static void testCase4();
        // Test basic accessors.

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

void TestDriver::testCase12()
{
    // -----------------------------------------------------------------------
    // TESTING 'getNumDeletes' CLASS METHOD.
    //
    // Concerns:
    //: 1 The class correctly counts the number of destructor calls.
    //
    // Plan:
    //: 1 Create and destroy a set of test objects.  Verify that after every
    //:   destructor call 'getNumDeletes' returns the correct value.  (C-1)
    //
    // Testing:
    //   static in getNumDeletes();
    // ------------------------------------------------------------------------

    int count = 0;

    ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());

    // Testing destruction of objects created by value constructors.
    for (size_t ti = 0; ti <= 14; ++ti) {
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

        switch (ti) {
          case 0: {
            Obj X;
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 1: {
            Obj X(A01);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 2: {
            Obj X(A01, A02);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 3: {
            Obj X(A01, A02, A03);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 4: {
            Obj X(A01, A02, A03, A04);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 5: {
            Obj X(A01, A02, A03, A04, A05);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 6: {
            Obj X(A01, A02, A03, A04, A05, A06);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 7: {
            Obj X(A01, A02, A03, A04, A05, A06, A07);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 8: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 9: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 10: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 11: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10, A11);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 12: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10, A11, A12);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 13: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10, A11, A12,
                  A13);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          case 14: {
            Obj X(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10, A11, A12,
                  A13, A14);
            (void)X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
          } break;
          default: {
            ASSERTV(0);
          } break;
        }
        ++count;
        ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
    }

    // Testing destruction of objects created by the copy constructor.
    {
        {
            Obj X;
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());

            {
                Obj Y(X);
                (void)Y;
                ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
            }
            ++count;  // Y is destroyed
            ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
        }
        ++count;  // X is destroyed
        ASSERTV(Obj::getNumDeletes(), count == Obj::getNumDeletes());
    }

}

void TestDriver::testCase11()
{
    // ---------------------------------------------------------------------
    // TESTING 'isEqual' METHOD
    //
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
    //   bool isEqual(const EmplacableTestType& other) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nAssign the address of 'isEqual' to a variable.\n");
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

void TestDriver::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 The signature and return type are standard.
    //:
    //: 3 The reference returned is to the target object (i.e., '*this').
    //:
    //: 4 The value of the source object is not modified.
    //:
    //: 5 Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-2)
    //:
    //: 2 Using the table-driven technique, specify a set of distinct
    //:   object values (one per row) in terms of their attributes.
    //:
    //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
    //:
    //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', having the value of 'R1'.
    //:
    //:   2 For each row 'R2 in the table of P-2:  (C-1, 3..4)
    //:
    //:     1 Create a modifiable 'Obj', 'mX', having the value of 'R2'.
    //:
    //:     2 Assign 'mX' from 'Z'.  (C-1)
    //:
    //:     3 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-3)
    //:
    //:     4 Use the equality-comparison operator to verify that:
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
    //:
    //: 4 For each row 'N1' in table of P-2:  (C-3, 5)
    //:
    //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
    //:
    //:   1 Create a 'const' 'Obj', 'ZZ', pointing to 'N1'.
    //:
    //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   3 Assign 'mX' from 'Z'.
    //:
    //:   4 Verify that the address of the return value is the same as that of
    //:     'mX'.  (C-3)
    //:
    //:   5 Use the equal-comparison operator to verify that 'mX' has the
    //:     same value as 'ZZ'.  (C-5)
    //
    // Testing:
    //   EmplacableTestType& operator=(const EmplacableTestType& rhs);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nAssign the address of the operator to a variable.\n");
    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Obj::operator=;

        (void) operatorAssignment;  // quash potential compiler warning
    }

    if (verbose)
        printf("\nTesting copy-assignment operator.\n");

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1 = DATA[ti].d_line;
        const char *const SPEC1 = DATA[ti].d_spec;

        if (veryVerbose) { T_ P_(LINE1) P(SPEC1) }

        bsls::ObjectBuffer<Obj> bufferZ;
        const Obj& Z = gg(bufferZ.address(), SPEC1);
        bslma::DestructorGuard<Obj> guardZ(&bufferZ.object());

        bsls::ObjectBuffer<Obj> bufferZZ;
        const Obj& ZZ = gg(bufferZZ.address(), SPEC1);
        bslma::DestructorGuard<Obj> guardZZ(&bufferZZ.object());

        for (size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2 = DATA[tj].d_line;
            const char *const SPEC2 = DATA[tj].d_spec;

            if (veryVerbose) { T_ T_ P_(LINE2) P(SPEC2) }

            bsls::ObjectBuffer<Obj> bufferX;
            Obj& mX = gg(bufferX.address(), SPEC2);
            bslma::DestructorGuard<Obj> guardX(&bufferX.object());

            Obj *mR = &(mX = Z);
            ASSERTV(ti, tj, mR, &mX, mR == &mX);

            ASSERTV(ti, tj, Z == mX);
            ASSERTV(ti, tj, Z == ZZ);
        }
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_line;
        const char *const SPEC = DATA[ti].d_spec;

        if (veryVerbose) { T_ P_(LINE) P(SPEC) }

        bsls::ObjectBuffer<Obj> bufferX;
        Obj& mX = gg(bufferX.address(), SPEC);
        bslma::DestructorGuard<Obj> guardX(&bufferX.object());

        bsls::ObjectBuffer<Obj> bufferZZ;
        const Obj& ZZ = gg(bufferZZ.address(), SPEC);
        bslma::DestructorGuard<Obj> guardZZ(&bufferZZ.object());

        const Obj& Z = mX;
        Obj *mR = &(mX = Z);
        ASSERTV(ti, mR, &mX, mR == &mX);

        ASSERTV(ti, Z == mX);
        ASSERTV(ti, Z == ZZ);
    }
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
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
    // TESTING EQUALITY OPERATORS
    //
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

void TestDriver::testCase4()
{
    // --------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the corresponding attribute
    //:   of the object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //
    // Plan:
    //: 1 Use the value constructors to create objects having different
    //:   attribute values.  Verify that the accessors for the corresponding
    //:   attributes invoked on a reference providing non-modifiable access
    //:   to the object return the expected value.  (C-2)
    //:
    //: 2 Verify that the accessors for the attributes not supplied to the
    //:   value constructor return the default attribute value.  (C-1)
    //
    // Testing:
    //   ArgType01 arg01() const;
    //   ArgType02 arg02() const;
    //   ArgType03 arg03() const;
    //   ArgType04 arg04() const;
    //   ArgType05 arg05() const;
    //   ArgType06 arg06() const;
    //   ArgType07 arg07() const;
    //   ArgType08 arg08() const;
    //   ArgType09 arg09() const;
    //   ArgType10 arg10() const;
    //   ArgType11 arg11() const;
    //   ArgType12 arg12() const;
    //   ArgType13 arg13() const;
    //   ArgType14 arg14() const;
    // --------------------------------------------------------------------
    struct {
        int d_line;  // source line number
        int d_value;
    } DATA[] =
    {
        //LINE  VALUE
        //----  --------
        { L_,         0 },
        { L_,         1 },
        { L_,   INT_MAX },
    };

    const size_t NUM_DATA = sizeof DATA / sizeof DATA[0];

    for (size_t td = 0; td < NUM_DATA; ++td) {          // argument value
        const int LINE  = DATA[td].d_line;
        const int VALUE = DATA[td].d_value;

        if (veryVerbose) { T_ P_(LINE) P(VALUE) }

        for (size_t ti = 0; ti <= 14; ++ti) {           // argument position
            for (size_t tj = 0; tj <= 14; ++tj) {       // variant of ctor
                ArgType01 A01 = ti ==  1 && ti < tj ?
                                                ArgType01(VALUE) : ArgType01();
                ArgType02 A02 = ti ==  2 && ti < tj ?
                                                ArgType02(VALUE) : ArgType02();
                ArgType03 A03 = ti ==  3 && ti < tj ?
                                                ArgType03(VALUE) : ArgType03();
                ArgType04 A04 = ti ==  4 && ti < tj ?
                                                ArgType04(VALUE) : ArgType04();
                ArgType05 A05 = ti ==  5 && ti < tj ?
                                                ArgType05(VALUE) : ArgType05();
                ArgType06 A06 = ti ==  6 && ti < tj ?
                                                ArgType06(VALUE) : ArgType06();
                ArgType07 A07 = ti ==  7 && ti < tj ?
                                                ArgType07(VALUE) : ArgType07();
                ArgType08 A08 = ti ==  8 && ti < tj ?
                                                ArgType08(VALUE) : ArgType08();
                ArgType09 A09 = ti ==  9 && ti < tj ?
                                                ArgType09(VALUE) : ArgType09();
                ArgType10 A10 = ti == 10 && ti < tj ?
                                                ArgType10(VALUE) : ArgType10();
                ArgType11 A11 = ti == 11 && ti < tj ?
                                                ArgType11(VALUE) : ArgType11();
                ArgType12 A12 = ti == 12 && ti < tj ?
                                                ArgType12(VALUE) : ArgType12();
                ArgType13 A13 = ti == 13 && ti < tj ?
                                                ArgType13(VALUE) : ArgType13();
                ArgType14 A14 = ti == 14 && ti < tj ?
                                                ArgType14(VALUE) : ArgType14();

                bsls::ObjectBuffer<Obj> buffer;
                const Obj& X = buffer.object();

                if (veryVerbose) { T_ T_ P_(ti) P(tj) }

                switch (tj) {
                  case 0: {
                    new(buffer.address()) Obj();
                  } break;
                  case 1: {
                    new (buffer.address()) Obj(A01);
                  } break;
                  case 2: {
                    new (buffer.address()) Obj(A01, A02);
                  } break;
                  case 3: {
                    new (buffer.address()) Obj(A01, A02, A03);
                  } break;
                  case 4: {
                    new (buffer.address()) Obj(A01, A02, A03, A04);
                  } break;
                  case 5: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05);
                  } break;
                  case 6: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06);
                  } break;
                  case 7: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07);
                  } break;
                  case 8: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08);
                  } break;
                  case 9: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09);
                  } break;
                  case 10: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09, A10);
                  } break;
                  case 11: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09, A10, A11);
                  } break;
                  case 12: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09, A10, A11, A12);
                  } break;
                  case 13: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09, A10, A11, A12,
                                               A13);
                  } break;
                  case 14: {
                    new (buffer.address()) Obj(A01, A02, A03, A04, A05, A06,
                                               A07, A08, A09, A10, A11, A12,
                                               A13, A14);
                  } break;
                  default: {
                    ASSERTV(0);
                  } break;
                }
                bslma::DestructorGuard<Obj> guard(&buffer.object());

                ASSERTV(A01, X.arg01(), A01 == X.arg01());
                ASSERTV(A02, X.arg02(), A02 == X.arg02());
                ASSERTV(A03, X.arg03(), A03 == X.arg03());
                ASSERTV(A04, X.arg04(), A04 == X.arg04());
                ASSERTV(A05, X.arg05(), A05 == X.arg05());
                ASSERTV(A06, X.arg06(), A06 == X.arg06());
                ASSERTV(A07, X.arg07(), A07 == X.arg07());
                ASSERTV(A08, X.arg08(), A08 == X.arg08());
                ASSERTV(A09, X.arg09(), A09 == X.arg09());
                ASSERTV(A10, X.arg10(), A10 == X.arg10());
                ASSERTV(A11, X.arg11(), A11 == X.arg11());
                ASSERTV(A12, X.arg12(), A12 == X.arg12());
                ASSERTV(A13, X.arg13(), A13 == X.arg13());
                ASSERTV(A14, X.arg14(), A14 == X.arg14());
            }
        }
    }
}

void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions, we want to verify that valid generator syntax
    //   produces expected results and that invalid syntax is detected and
    //   reported.
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results.
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
    // ------------------------------------------------------------------------
    // VALUE CONSTRUCTOR TEST HELPER
    //   This helper exercises value constructors that take 0..14 arguments.
    //
    // Concerns:
    //: 1 Constructor arguments are correcty passed to the corresponding
    //:   attributes of the object.
    //
    // Plan:
    //: 1 Create 14 argument values.
    //:
    //: 2 Based on the (first) template parameter indicating the number of
    //:   arguments to pass in, call the value constructor with the
    //:   corresponding number of arguments, performing an explicit move
    //:   of the argument if so indicated by the template parameter
    //:   corresponding to the argument.
    //:
    //: 3 Verify that the argument values were passed correctly.
    //:
    //: 4 Verify that the move-state for each argument is as expected.
    // ------------------------------------------------------------------------

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

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
        new (buffer.address()) Obj();
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

    ASSERTV(MOVE_01, A01.movedFrom(),
                           MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
                           MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
                           MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
                           MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
                           MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
                           MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
                           MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
                           MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
                           MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
                           MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
                           MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
                           MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
                           MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
                           MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));


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
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
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
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'getNumDeletes' CLASS METHOD
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'getNumDeletes' class method"
                            "\n====================================\n");

        TestDriver::testCase12();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual' METHOD
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'isEqual' method"
                            "\n========================\n");

        TestDriver::testCase11();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Assignment"
                            "\n=======================\n");

        TestDriver::testCase9();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructor"
                            "\n========================\n");

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
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");
        TestDriver::testCase4();
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
        // VALUE CONSTRUCTORS
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructors can create an object having any value that
        //:   does not violate the documented constraints.
        //
        // Plan:
        //: 1 This test makes material use of template method 'testCase2'
        //:   with the first integer template parameter indicating the number
        //:   of arguments to use, the next 14 integer template parameters
        //:   indicating '0' for copy, '1' for move, and '2' for not-applicable
        //:   (i.e., beyond the number of arguments).
        //
        //:   1 Based on the (first) template parameter indicating the number
        //:     of arguments to pass in, call the value constructor with the
        //:     corresponding number of arguments, performing an explicit move
        //:     of the argument if so indicated by the template parameter
        //:     corresponding to the argument.
        //:
        //: 2 Call 'testCase2' in various configurations:
        //:   1 For 0..14 arguments, call with the move flag set to '0' and
        //:     then with the move flag set to '1' for every positional
        //:     argument.
        //:
        //:   2 For 0..14 arguments, call with move flags set to '0' and '1'
        //:     for all positional arguments.
        //
        // Testing:
        //   EmplacableTestType();
        //   EmplacableTestType(arg01);
        //   EmplacableTestType(arg01 .. arg02);
        //   EmplacableTestType(arg01 .. arg03);
        //   EmplacableTestType(arg01 .. arg04);
        //   EmplacableTestType(arg01 .. arg05);
        //   EmplacableTestType(arg01 .. arg06);
        //   EmplacableTestType(arg01 .. arg07);
        //   EmplacableTestType(arg01 .. arg08);
        //   EmplacableTestType(arg01 .. arg09);
        //   EmplacableTestType(arg01 .. arg10);
        //   EmplacableTestType(arg01 .. arg11);
        //   EmplacableTestType(arg01 .. arg12);
        //   EmplacableTestType(arg01 .. arg13);
        //   EmplacableTestType(arg01 .. arg14);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTORS"
                            "\n==========================\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase2<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 2 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");
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

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");
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

        ASSERTV(Obj::getNumDeletes(), 0 == Obj::getNumDeletes());
        {
            Obj mX; const Obj& X = mX;
            ASSERTV(X.arg01(), -1 == X.arg01());
        }
        ASSERTV(Obj::getNumDeletes(), 1 == Obj::getNumDeletes());

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
        ASSERTV(Obj::getNumDeletes(), 2 == Obj::getNumDeletes());

        {
            Obj mX(V01);    const Obj& X = mX;
            Obj mY(V01, V02);

            ASSERT(mX != mY);
            Obj& mZ = (mY = X);

            ASSERT(&mZ == &mY);
            ASSERT(mX == mY);
        }
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
