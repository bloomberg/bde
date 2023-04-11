// bsltf_allocemplacabletesttype.t.cpp                                -*-C++-*-
#include <bsltf_allocemplacabletesttype.h>

#include <bslma_constructionutil.h>             // for usage example only
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_objectbuffer.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <new>

#include <limits.h>
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
// arguments are forwarded correctly to a type's contructor.
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
//-----------------------------------------------------------------------------
// CLASS METHODS
// [12] static int getNumDeletes();
//
// CREATORS
// [ 2] AllocEmplacableTestType(Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg02, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg03, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg04, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg05, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg06, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg07, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg08, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg09, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg10, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg11, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg12, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg13, Allocator *ba);
// [ 2] AllocEmplacableTestType(arg01 .. arg14, Allocator *ba);
// [ 7] AllocEmplacableTestType(const AllocETestType& orig, Allocator *ba);
// [ 2] ~AllocEmplacableTestType();
//
// ACCESSORS
// [ 4] AllocArgType01 arg01() const;
// [ 4] AllocArgType02 arg02() const;
// [ 4] AllocArgType03 arg03() const;
// [ 4] AllocArgType04 arg04() const;
// [ 4] AllocArgType05 arg05() const;
// [ 4] AllocArgType06 arg06() const;
// [ 4] AllocArgType07 arg07() const;
// [ 4] AllocArgType08 arg08() const;
// [ 4] AllocArgType09 arg09() const;
// [ 4] AllocArgType10 arg10() const;
// [ 4] AllocArgType11 arg11() const;
// [ 4] AllocArgType12 arg12() const;
// [ 4] AllocArgType13 arg13() const;
// [ 4] AllocArgType14 arg14() const;
// [ 4] bslma::Allocator *allocator() const;
// [11] bool isEqual(rhs) const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [13] CONCERN: The object has the necessary type traits

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
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

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

typedef AllocEmplacableTestType Obj;

static bslma::TestAllocator g_argAlloc("global args");
static const AllocEmplacableTestType::ArgType01 V01(1,    &g_argAlloc);
static const AllocEmplacableTestType::ArgType02 V02(20,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType03 V03(23,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType04 V04(44,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType05 V05(66,   &g_argAlloc);
static const AllocEmplacableTestType::ArgType06 V06(176,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType07 V07(878,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType08 V08(8,    &g_argAlloc);
static const AllocEmplacableTestType::ArgType09 V09(912,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType10 V10(102,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType11 V11(111,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType12 V12(333,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType13 V13(712,  &g_argAlloc);
static const AllocEmplacableTestType::ArgType14 V14(1414, &g_argAlloc);

struct DefaultDataRow {
    int         d_line;   // source line number
    int         d_group;  // equality group
    const char *d_spec;   // specification string, for input to 'gg' function
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
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

                               // ================
                               // class TestDriver
                               // ================

class TestDriver {
    // This class provide a namespace for testing the
    // 'AllocEmplacableTestType'.  Each "testCase*" method tests a specific
    // aspect of 'AllocEmplacableTestType'.

  private:
    // PRIVATE TYPES

    // Shorthands
    typedef Obj::ArgType01         ArgType01;
    typedef Obj::ArgType02         ArgType02;
    typedef Obj::ArgType03         ArgType03;
    typedef Obj::ArgType04         ArgType04;
    typedef Obj::ArgType05         ArgType05;
    typedef Obj::ArgType06         ArgType06;
    typedef Obj::ArgType07         ArgType07;
    typedef Obj::ArgType08         ArgType08;
    typedef Obj::ArgType09         ArgType09;
    typedef Obj::ArgType10         ArgType10;
    typedef Obj::ArgType11         ArgType11;
    typedef Obj::ArgType12         ArgType12;
    typedef Obj::ArgType13         ArgType13;
    typedef Obj::ArgType14         ArgType14;

    typedef bslmf::MovableRefUtil  MoveUtil;
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

    static int ggg(Obj              *object,
                   const char       *spec,
                   bslma::Allocator *basicAllocator,
                   int               verbose = 1);
        // Construct the specified 'object' according to the specified 'spec',
        // using the specified 'basicAllocator' to supply memory.  Optionally
        // specify a zero 'verbose' to suppress 'spec' syntax error messages.
        // Return the index of the first invalid character, and a negative
        // value otherwise.  Note that this function is used to implement 'gg'
        // as well as allow for verification of syntax error detection.

    static Obj& gg(Obj              *object,
                   const char       *spec,
                   bslma::Allocator *basicAllocator);
        // Return, by reference, the specified 'object' with its value
        // constructed according to the specified 'spec', using the specified
        // 'basicAllocator' to supply memory.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
    {
        return  MoveUtil::move(t);
    }

    template <class T>
    static const T& testArg(T& t, bsl::false_type)
    {
        return t;
    }

  public:
    // TEST CASES
    static void testCase13();
        // Test type traits.

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
    static void testCase2a();

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

                               // ----------------
                               // class TestDriver
                               // ----------------

int TestDriver::ggg(Obj              *object,
                    const char       *spec,
                    bslma::Allocator *basicAllocator,
                    int               verbose)

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

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);;

    size_t LENGTH = strlen(spec);
    ArgType01 A01 = LENGTH >  0 && spec[ 0] != ' ' ?
                     ArgType01(spec[ 0] - 'A', &scratch) : ArgType01(&scratch);
    ArgType02 A02 = LENGTH >  1 && spec[ 1] != ' ' ?
                     ArgType02(spec[ 1] - 'A', &scratch) : ArgType02(&scratch);
    ArgType03 A03 = LENGTH >  2 && spec[ 2] != ' ' ?
                     ArgType03(spec[ 2] - 'A', &scratch) : ArgType03(&scratch);
    ArgType04 A04 = LENGTH >  3 && spec[ 3] != ' ' ?
                     ArgType04(spec[ 3] - 'A', &scratch) : ArgType04(&scratch);
    ArgType05 A05 = LENGTH >  4 && spec[ 4] != ' ' ?
                     ArgType05(spec[ 4] - 'A', &scratch) : ArgType05(&scratch);
    ArgType06 A06 = LENGTH >  5 && spec[ 5] != ' ' ?
                     ArgType06(spec[ 5] - 'A', &scratch) : ArgType06(&scratch);
    ArgType07 A07 = LENGTH >  6 && spec[ 6] != ' ' ?
                     ArgType07(spec[ 6] - 'A', &scratch) : ArgType07(&scratch);
    ArgType08 A08 = LENGTH >  7 && spec[ 7] != ' ' ?
                     ArgType08(spec[ 7] - 'A', &scratch) : ArgType08(&scratch);
    ArgType09 A09 = LENGTH >  8 && spec[ 8] != ' ' ?
                     ArgType09(spec[ 8] - 'A', &scratch) : ArgType09(&scratch);
    ArgType10 A10 = LENGTH >  9 && spec[ 9] != ' ' ?
                     ArgType10(spec[ 9] - 'A', &scratch) : ArgType10(&scratch);
    ArgType11 A11 = LENGTH > 10 && spec[10] != ' ' ?
                     ArgType11(spec[10] - 'A', &scratch) : ArgType11(&scratch);
    ArgType12 A12 = LENGTH > 11 && spec[11] != ' ' ?
                     ArgType12(spec[11] - 'A', &scratch) : ArgType12(&scratch);
    ArgType13 A13 = LENGTH > 12 && spec[12] != ' ' ?
                     ArgType13(spec[12] - 'A', &scratch) : ArgType13(&scratch);
    ArgType14 A14 = LENGTH > 13 && spec[13] != ' ' ?
                     ArgType14(spec[13] - 'A', &scratch) : ArgType14(&scratch);

    switch (LENGTH) {
      case 0: {
        new (object) Obj(basicAllocator);
      } break;
      case 1: {
        new (object) Obj(A01, basicAllocator);
      } break;
      case 2: {
        new (object) Obj(A01, A02, basicAllocator);
      } break;
      case 3: {
        new (object) Obj(A01, A02, A03, basicAllocator);
      } break;
      case 4: {
        new (object) Obj(A01, A02, A03, A04, basicAllocator);
      } break;
      case 5: {
        new (object) Obj(A01, A02, A03, A04, A05, basicAllocator);
      } break;
      case 6: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, basicAllocator);
      } break;
      case 7: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, basicAllocator);
      } break;
      case 8: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08,
                         basicAllocator);
      } break;
      case 9: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09,
                         basicAllocator);
      } break;
      case 10: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         basicAllocator);
      } break;
      case 11: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, basicAllocator);
      } break;
      case 12: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12, basicAllocator);
      } break;
      case 13: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12, A13, basicAllocator);
      } break;
      case 14: {
        new (object) Obj(A01, A02, A03, A04, A05, A06, A07, A08, A09, A10,
                         A11, A12, A13, A14, basicAllocator);
      } break;
      default: {
        ASSERTV(0);
      } break;
    }

    return SUCCESS;
}

Obj& TestDriver::gg(Obj              *object,
                    const char       *spec,
                    bslma::Allocator *basicAllocator)
{
    ASSERTV(ggg(object, spec, basicAllocator) < 0);
    return *object;
}

void TestDriver::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //   Ensure that 'MoveOnlyAllocTestType' has the necessary trait values
    //   to guarantee its expected behavior.
    //
    // Concerns:
    //: 1 The object has the 'bslma::UsesBslmaAllocator' trait.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits
    // ------------------------------------------------------------------------

    BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
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

    if (verbose) printf("\tAssign the address of 'isEqual' to a variable.\n");
    {
        typedef bool (Obj::*methodPtr)(const Obj&) const;

        // Verify that the signature and return type are correct..
        methodPtr methodIsEqual = &Obj::isEqual;

        (void) methodIsEqual;  // quash potential compiler warnings
    }

    enum { NUM_DATA = DEFAULT_NUM_DATA };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1  = DATA[ti].d_line;
            const int         GROUP1 = DATA[ti].d_group;
            const char *const SPEC1  = DATA[ti].d_spec;

            if (veryVerbose) { T_ P_(LINE1) P_(GROUP1) P(SPEC1) }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            // Ensure an object is equal to itself (alias test).
            {
                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1, &scratch);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                ASSERTV(LINE1, true == X.isEqual(X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         GROUP2 = DATA[tj].d_group;
                const char *const SPEC2  = DATA[tj].d_spec;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(GROUP2) P(SPEC2) }

                const bool EXP = GROUP1 == GROUP2;  // expected result

                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1, &scratch);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                {
                    // 'Y' is created with same allocator
                    bsls::ObjectBuffer<Obj> bufferY;
                    const Obj& Y = gg(bufferY.address(), SPEC2, &scratch);
                    bslma::DestructorGuard<Obj> guardY(&bufferY.object());

                    // Verify value and commutativity.
                    ASSERTV(LINE1, LINE2,  EXP == X.isEqual(Y));
                    ASSERTV(LINE1, LINE2,  EXP == Y.isEqual(X));
                }
                {
                    // 'Y' is created with different allocator
                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    bsls::ObjectBuffer<Obj> bufferY;
                    const Obj& Y = gg(bufferY.address(), SPEC2, &sa);
                    bslma::DestructorGuard<Obj> guardY(&bufferY.object());

                    // Verify value and commutativity.
                    ASSERTV(LINE1, LINE2,  EXP == X.isEqual(Y));
                    ASSERTV(LINE1, LINE2,  EXP == Y.isEqual(X));
                }
            }
        }
    }
}

void TestDriver::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING COPY-ASSIGNMENT OPERATOR
    //   Ensure that we can assign the value of any object of the class to
    //   any object of the class, such that the two objects subsequently
    //   have the same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable
    //:   target object to that of any source object.
    //:
    //: 2 The allocator address held by the target object is unchanged.
    //:
    //: 3 Any memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed
    //:   value allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   object values (one per row) in terms of their attributes.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in
    //:   the table described in P-3:  (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create
    //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table
    //:     described in P-3:
    //:
    //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as
    //:       that of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of
    //:         'Z'.  (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
    //:       that the respective allocator addresses held by the target
    //:       and source objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that:
    //:       (C-8, 11)
    //:
    //:       1 For an object that (a) is initialized with a value that did
    //:         NOT require memory allocation, and (b) is then assigned a
    //:         value that DID require memory allocation, the target object
    //:         DOES allocate memory from its object allocator only
    //:         (irrespective of the specific number of allocations or the
    //:         total amount of memory allocated); also cross check with
    //:         what is expected for 'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require
    //:         memory allocation, does NOT allocate memory from its object
    //:         allocator; also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.
    //:         (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-2 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both
    //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
    //:   row (representing a distinct object value, 'V') in the table
    //:   described in P-3:  (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:     (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that
    //:     of 'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the
    //:     target object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
    //:     the object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated
    //:       when assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   AllocEmplacableTestType& operator=(const AllocETestType& rhs);
    // --------------------------------------------------------------------

    if (verbose)
               printf("\tAssign the address of the operator to a variable.\n");
    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Obj::operator=;

        (void) operatorAssignment;  // quash potential compiler warning
    }

    if (verbose) printf("\tTesting copy-assignment operator.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    enum { NUM_DATA = DEFAULT_NUM_DATA };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1  = DATA[ti].d_line;
        const int         GROUP1 = DATA[ti].d_group;
        const char *const SPEC1  = DATA[ti].d_spec;

        if (veryVerbose) { T_ P_(LINE1) P(SPEC1) }

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Create source object Z.
        bsls::ObjectBuffer<Obj> bufferZ;
        const Obj& Z = gg(bufferZ.address(), SPEC1, &scratch);
        bslma::DestructorGuard<Obj> guardZ(&bufferZ.object());

        // Create control object ZZ.
        bsls::ObjectBuffer<Obj> bufferZZ;
        const Obj& ZZ = gg(bufferZZ.address(), SPEC1, &scratch);
        bslma::DestructorGuard<Obj> guardZZ(&bufferZZ.object());

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2  = DATA[tj].d_line;
            const int         GROUP2 = DATA[tj].d_group;
            const char *const SPEC2  = DATA[tj].d_spec;

            if (veryVerbose) { T_ T_ P_(LINE2) P(SPEC2) }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                // Create object X.
                bsls::ObjectBuffer<Obj> bufferX;
                Obj&       mX = gg(bufferX.address(), SPEC2, &oa);
                const Obj& X  = mX;

                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                ASSERTV(LINE1, LINE2, (Z == X) == (GROUP1 == GROUP2));

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, LINE2, Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

                ASSERTV(LINE1, LINE2, ZZ == Z);

                ASSERTV(LINE1, LINE2, &oa, X.allocator(),
                             &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &scratch, Z.allocator(),
                             &scratch == Z.allocator());

                if (GROUP1 == GROUP2) {
                    ASSERTV(LINE1, LINE2, oam.isInUseSame());
                }

                ASSERTV(LINE1, LINE2, sam.isInUseSame());
            }

            // Verify all memory is released on object destruction.

            ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                    0 == oa.numBlocksInUse());
        }

        // self-assignment
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            // Create object X.
            bsls::ObjectBuffer<Obj> bufferX;
            Obj&       mX = gg(bufferX.address(), SPEC1, &oa);
            bslma::DestructorGuard<Obj> guardX(&mX);

            // Create control object ZZ.
            bsls::ObjectBuffer<Obj> bufferZZ;
            Obj&       mZZ = gg(bufferZZ.address(), SPEC1, &scratch);
            const Obj& ZZ  = mZZ;
            bslma::DestructorGuard<Obj> guardZZ(&mZZ);

            const Obj& Z = mX;

            ASSERTV(LINE1, ZZ == Z);

            bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

            ASSERTV(LINE1, oam.isInUseSame());

            ASSERTV(LINE1, sam.isInUseSame());

            ASSERTV(LINE1, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }

        // Verify all object memory is released on destruction.

        ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
    }
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
    //   Ensure that we can create a distinct object of the class from any
    //   other one, such that the two objects have the same value.
    //
    // Concerns:
    //: 1 The copy constructor creates an object having the same value as that
    //:   of the supplied original object.
    //:
    //: 2 If an allocator is NOT supplied to the copy constructor, the default
    //:   allocator in effect at the time of construction becomes the object
    //:   allocator for the resulting object (i.e., the allocator of the
    //:   original object is never copied).
    //:
    //: 3 If an allocator IS supplied to the copy constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the copy constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 The copy constructor is exception-neutral w.r.t. memory allocation.
    //:
    //: 8 The original object is passed as a reference providing
    //:   non-modifiable access to that object.
    //:
    //: 9 The value of the original object is unchanged.
    //:
    //:10 The allocator address held by the original object is unchanged.
    //
    // Plan:
    //: 1 Using the table-driven technique, specify a set of distinct
    //:   object values (one per row) in terms of their attributes.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the
    //:   table described in P-1:  (C-1..10)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create
    //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop creating three distinct objects in turn,
    //:     each using the copy constructor in the presence of injected
    //:     exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros) on 'Z' from P-2.1, but configured differently: (a)
    //:     without passing an allocator, (b) passing a null allocator
    //:     address explicitly, and (c) passing the address of a test
    //:     allocator distinct from the default.  (C-7)
    //:
    //:   3 For each of these three iterations (P-2.2):  (C-1..10)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one
    //:       as the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the copy constructor to dynamically create an object 'X',
    //:       with its object allocator configured appropriately (see
    //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
    //:       a distinct test allocator for the object's footprint.  (C-8)
    //:
    //:     3 Use the equality-comparison operator to verify that:
    //:       (C-1, 5, 9)
    //:
    //:       1 The newly constructed object, 'X', has the same value as
    //:         that of 'Z'.  (C-1, 5)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-9)
    //:
    //:     4 Use the 'allocator' accessor of each underlying attribute
    //:       capable of allocating memory to ensure that its object
    //:       allocator is properly installed; also use the 'allocator'
    //:       accessor of 'X' to verify that its object allocator is
    //:       properly installed, and use the 'allocator' accessor of 'Z'
    //:       to verify that the allocator address that it holds is
    //:       unchanged.  (C-6, 10)
    //:
    //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
    //:       7..8)
    //:
    //:       1 An object allocates memory from the object allocator only.
    //:         (C-2..4)
    //:
    //:       2 If an allocator was supplied at construction (P-2.1c), the
    //:         current default allocator doesn't allocate any memory.
    //:         (C-3)
    //:
    //:       3 All object memory is released when the object is destroyed.
    //:         (C-8)
    //
    // Testing:
    //   AllocEmplacableTestType(const AllocETestType& orig, Allocator *ba);
    // ------------------------------------------------------------------------
    enum { NUM_DATA = DEFAULT_NUM_DATA };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE  = DATA[ti].d_line;
        const char *const SPEC  = DATA[ti].d_spec;

        if (veryVerbose) { P_(LINE) P(SPEC); }

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Create source object Z.
        bsls::ObjectBuffer<Obj> bufferZ;
        const Obj& Z = gg(bufferZ.address(), SPEC, &scratch);
        bslma::DestructorGuard<Obj> guardZ(&bufferZ.object());

        // Create control object ZZ.
        bsls::ObjectBuffer<Obj> bufferZZ;
        const Obj& ZZ = gg(bufferZZ.address(), SPEC, &scratch);
        bslma::DestructorGuard<Obj> guardZZ(&bufferZZ.object());

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
              } break;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("Bad allocator config.");
              } break;
            }

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                bslma::TestAllocatorMonitor tam(&oa);
                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(Z);
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(Z, 0);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(Z, &sa);
                  } break;
                  default: {
                    BSLS_ASSERT_INVOKE_NORETURN("Bad allocator config.");
                  } break;
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            Obj& mX = *objPtr; const Obj& X = mX;

            ASSERTV(SPEC,  Z == X);
            ASSERTV(SPEC,  Z == ZZ);


            // Verify any attribute allocators are installed properly.
            ASSERTV(SPEC, CONFIG, &oa == X.allocator());

            // Also invoke the object's 'allocator' accessor, as well as
            // that of 'Z'.

            ASSERTV(SPEC, CONFIG, &oa, X.allocator(),
                    &oa == X.allocator());

            ASSERTV(SPEC, CONFIG, &scratch, Z.allocator(),
                    &scratch == Z.allocator());

            // Verify no allocation from the non-object allocator.

            ASSERTV(SPEC, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(SPEC, CONFIG, da.numBlocksInUse(),
                         0 == da.numBlocksInUse());
            ASSERTV(SPEC, CONFIG, fa.numBlocksInUse(),
                         0 == fa.numBlocksInUse());
            ASSERTV(SPEC, CONFIG, sa.numBlocksInUse(),
                         0 == sa.numBlocksInUse());
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
              printf("\tAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
    }

    enum { NUM_DATA = DEFAULT_NUM_DATA };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tCompare every value with every value.\n");
    {

        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1  = DATA[ti].d_line;
            const int         GROUP1 = DATA[ti].d_group;
            const char *const SPEC1  = DATA[ti].d_spec;

           if (veryVerbose) { T_ P_(LINE1) P_(GROUP1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1, &oa);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                ASSERTV(LINE1,   X == X);
                ASSERTV(LINE1, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         GROUP2 = DATA[tj].d_group;
                const char *const SPEC2  = DATA[tj].d_spec;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(GROUP2) P(SPEC2) }

                const bool EXP = GROUP1 == GROUP2;  // expected result

                bslma::TestAllocator  oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator  oa2("object2", veryVeryVeryVerbose);

                bsls::ObjectBuffer<Obj> bufferX;
                const Obj& X = gg(bufferX.address(), SPEC1, &oa1);
                bslma::DestructorGuard<Obj> guardX(&bufferX.object());

                bsls::ObjectBuffer<Obj> bufferY;
                const Obj& Y = gg(bufferY.address(), SPEC2, &oa1);
                bslma::DestructorGuard<Obj> guardY(&bufferY.object());

                bsls::ObjectBuffer<Obj> bufferZ;
                const Obj& Z = gg(bufferZ.address(), SPEC2, &oa1);
                bslma::DestructorGuard<Obj> guardZ(&bufferZ.object());

                // Verify value and commutativity.
                ASSERTV(LINE1, LINE2, EXP == (X == Y));
                ASSERTV(LINE1, LINE2, EXP == (Y == X));
                ASSERTV(LINE1, LINE2, EXP != !(X == Y));
                ASSERTV(LINE1, LINE2, EXP != !(Y == X));

                ASSERTV(LINE1, LINE2, EXP != (X != Y));
                ASSERTV(LINE1, LINE2, EXP != (Y != X));
                ASSERTV(LINE1, LINE2, EXP == !(X != Y));
                ASSERTV(LINE1, LINE2, EXP == !(Y != X));

                ASSERTV(LINE1, LINE2, EXP == (X == Z));
                ASSERTV(LINE1, LINE2, EXP == (Z == X));
                ASSERTV(LINE1, LINE2, EXP != !(X == Z));
                ASSERTV(LINE1, LINE2, EXP != !(Z == X));

                ASSERTV(LINE1, LINE2, EXP != (X != Z));
                ASSERTV(LINE1, LINE2, EXP != (Z != X));
                ASSERTV(LINE1, LINE2, EXP == !(X != Z));
                ASSERTV(LINE1, LINE2, EXP == !(Z != X));
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
    //:
    //: 3 If an allocator is NOT supplied to the value constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 4 If an allocator IS supplied to the copy constructor, that allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 5 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //
    // Plan:
    //: 1 Using the table-driven technique, specify a set of distinct
    //:   attribute values (one per row).
    //:
    //: 2 For each row (representing a distinct attribute value, 'V') in the
    //:   table described in P-1:  (C-1..5)
    //:
    //:   1 Execute an inner loop creating three distinct objects in turn,
    //:     each using the value constructor from P-2.1, but configured
    //:     differently: (a) without passing an allocator, (b) passing a null
    //:     allocator address explicitly, and (c) passing the address of a test
    //:     allocator distinct from the default.  (C-1..5)
    //:
    //:   2 For each of these three iterations (P-2.1):  (C-1..5)
    //:
    //:     1 Verify that the accessors for the corresponding attributes
    //:       invoked on a reference providing non-modifiable access to the
    //:       object return the expected value.
    //:
    //:     2 Verify that the accessors for the attributes not supplied to the
    //:       value constructor return the default attribute value.  (C-1..5)
    //
    // Testing:
    //   AllocArgType01 arg01() const;
    //   AllocArgType02 arg02() const;
    //   AllocArgType03 arg03() const;
    //   AllocArgType04 arg04() const;
    //   AllocArgType05 arg05() const;
    //   AllocArgType06 arg06() const;
    //   AllocArgType07 arg07() const;
    //   AllocArgType08 arg08() const;
    //   AllocArgType09 arg09() const;
    //   AllocArgType10 arg10() const;
    //   AllocArgType11 arg11() const;
    //   AllocArgType12 arg12() const;
    //   AllocArgType13 arg13() const;
    //   AllocArgType14 arg14() const;
    //   bslma::Allocator *allocator() const;
    // --------------------------------------------------------------------

    struct {
        int d_line;  // source line number
        int d_value;
    } DATA[] =
    {
        //LINE  VALUE
        //----  --------
        { L_,        -1 },
        { L_,         0 },
        { L_,         1 },
        { L_,       256 },
        { L_,   INT_MAX },
    };

    enum { NUM_DATA = sizeof DATA / sizeof DATA[0] };

    for (int td = 0; td < NUM_DATA; ++td) {          // argument value
        const int LINE  = DATA[td].d_line;
        const int VALUE = DATA[td].d_value;

        if (veryVerbose) { T_ P_(LINE) P(VALUE) }

        bslma::TestAllocator aa("args", veryVeryVerbose);

        for (int ti = 0; ti <= 14; ++ti) {           // argument position
            for (int tj = 0; tj <= 14; ++tj) {       // variant of ctor
                ArgType01 A01 = ti ==  1 && ti < tj && -1 != VALUE ?
                                        ArgType01(VALUE, &aa) : ArgType01(&aa);
                ArgType02 A02 = ti ==  2 && ti < tj && -1 != VALUE ?
                                        ArgType02(VALUE, &aa) : ArgType02(&aa);
                ArgType03 A03 = ti ==  3 && ti < tj && -1 != VALUE ?
                                        ArgType03(VALUE, &aa) : ArgType03(&aa);
                ArgType04 A04 = ti ==  4 && ti < tj && -1 != VALUE ?
                                        ArgType04(VALUE, &aa) : ArgType04(&aa);
                ArgType05 A05 = ti ==  5 && ti < tj && -1 != VALUE ?
                                        ArgType05(VALUE, &aa) : ArgType05(&aa);
                ArgType06 A06 = ti ==  6 && ti < tj && -1 != VALUE ?
                                        ArgType06(VALUE, &aa) : ArgType06(&aa);
                ArgType07 A07 = ti ==  7 && ti < tj && -1 != VALUE ?
                                        ArgType07(VALUE, &aa) : ArgType07(&aa);
                ArgType08 A08 = ti ==  8 && ti < tj && -1 != VALUE ?
                                        ArgType08(VALUE, &aa) : ArgType08(&aa);
                ArgType09 A09 = ti ==  9 && ti < tj && -1 != VALUE ?
                                        ArgType09(VALUE, &aa) : ArgType09(&aa);
                ArgType10 A10 = ti == 10 && ti < tj && -1 != VALUE ?
                                        ArgType10(VALUE, &aa) : ArgType10(&aa);
                ArgType11 A11 = ti == 11 && ti < tj && -1 != VALUE ?
                                        ArgType11(VALUE, &aa) : ArgType11(&aa);
                ArgType12 A12 = ti == 12 && ti < tj && -1 != VALUE ?
                                        ArgType12(VALUE, &aa) : ArgType12(&aa);
                ArgType13 A13 = ti == 13 && ti < tj && -1 != VALUE ?
                                        ArgType13(VALUE, &aa) : ArgType13(&aa);
                ArgType14 A14 = ti == 14 && ti < tj && -1 != VALUE ?
                                        ArgType14(VALUE, &aa) : ArgType14(&aa);

                bsls::ObjectBuffer<Obj> buffer;
                const Obj& X = buffer.object();

                if (veryVerbose) { T_ T_ P_(ti) P(tj) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        BSLS_ASSERT_INVOKE_NORETURN("Bad allocator config.");
                      } break;
                    }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;

                    Obj* objPtr = buffer.address();

                    switch (CONFIG) {
                      case 'a': {
                        switch (tj) {
                          case 0: {
                            new (objPtr) Obj();
                          } break;
                          case 1: {
                            new (objPtr) Obj(A01);
                          } break;
                          case 2: {
                            new (objPtr) Obj(A01, A02);
                          } break;
                          case 3: {
                            new (objPtr) Obj(A01, A02, A03);
                          } break;
                          case 4: {
                            new (objPtr) Obj(A01, A02, A03, A04);
                          } break;
                          case 5: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05);
                          } break;
                          case 6: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06);
                          } break;
                          case 7: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06,
                                             A07);
                          } break;
                          case 8: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08);
                          } break;
                          case 9: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09);
                          } break;
                          case 10: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10);
                          } break;
                          case 11: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11);
                          } break;
                          case 12: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12);
                          } break;
                          case 13: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13);
                          } break;
                          case 14: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13,
                                             A14);
                          } break;
                          default: {
                            ASSERTV(0);
                          } break;
                        }
                      } break;
                      case 'b': {
                        switch (tj) {
                          case 0: {
                            new (objPtr) Obj(0);
                          } break;
                          case 1: {
                            new (objPtr) Obj(A01, 0);
                          } break;
                          case 2: {
                            new (objPtr) Obj(A01, A02, 0);
                          } break;
                          case 3: {
                            new (objPtr) Obj(A01, A02, A03, 0);
                          } break;
                          case 4: {
                            new (objPtr) Obj(A01, A02, A03, A04, 0);
                          } break;
                          case 5: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, 0);
                          } break;
                          case 6: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, 0);
                          } break;
                          case 7: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             0);
                          } break;
                          case 8: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, 0);
                          } break;
                          case 9: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, 0);
                          } break;
                          case 10: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, 0);
                          } break;
                          case 11: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, 0);
                          } break;
                          case 12: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, 0);
                          } break;
                          case 13: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13, 0);
                          } break;
                          case 14: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13, A14,
                                             0);
                          } break;
                          default: {
                            ASSERTV(0);
                          } break;
                        }
                      } break;
                      case 'c': {
                        switch (tj) {
                          case 0: {
                            new (objPtr) Obj(&oa);
                          } break;
                          case 1: {
                            new (objPtr) Obj(A01, &oa);
                          } break;
                          case 2: {
                            new (objPtr) Obj(A01, A02, &oa);
                          } break;
                          case 3: {
                            new (objPtr) Obj(A01, A02, A03, &oa);
                          } break;
                          case 4: {
                            new (objPtr) Obj(A01, A02, A03, A04, &oa);
                          } break;
                          case 5: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, &oa);
                          } break;
                          case 6: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06,
                                             &oa);
                          } break;
                          case 7: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             &oa);
                          } break;
                          case 8: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, &oa);
                          } break;
                          case 9: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, &oa);
                          } break;
                          case 10: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, &oa);
                          } break;
                          case 11: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, &oa);
                          } break;
                          case 12: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, &oa);
                          } break;
                          case 13: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13,
                                             &oa);
                          } break;
                          case 14: {
                            new (objPtr) Obj(A01, A02, A03, A04, A05, A06, A07,
                                             A08, A09, A10, A11, A12, A13, A14,
                                             &oa);
                          } break;
                          default: {
                            ASSERTV(0);
                          } break;
                        }
                      } break;
                      default: {
                        BSLS_ASSERT_INVOKE_NORETURN("Bad allocator config.");
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

                    ASSERTV(objAllocatorPtr == X.allocator());
                }
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
    //   int  ggg(Obj *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting generator on valid specs.\n");
    {
        enum { NUM_DATA = DEFAULT_NUM_DATA };
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator ta("test", veryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE = DATA[ti].d_line;
            const char *const SPEC = DATA[ti].d_spec;

            bsls::ObjectBuffer<Obj> bufferX;
            const Obj& X = gg(bufferX.address(), SPEC, &ta);   // original spec
            bslma::DestructorGuard<Obj> guardX(&bufferX.object());

            bsls::ObjectBuffer<Obj> bufferY;
            const Obj& Y = gg(bufferY.address(), SPEC, &ta);   // extended spec
            bslma::DestructorGuard<Obj> guardY(&bufferY.object());

            // TBD: we use yet untested operator== to compare
            ASSERTV(LINE, X == Y);
        }
    }

    if (verbose) printf("\tTesting generator on invalid specs.\n");
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
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator ta("test", veryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE  = DATA[ti].d_line;
            const char *const SPEC  = DATA[ti].d_spec;
            const int         INDEX = DATA[ti].d_index;

            if (veryVerbose) { P(SPEC) };

            bsls::ObjectBuffer<Obj> buffer;

            int RESULT = ggg(buffer.address(), SPEC, &ta, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
            if (-1 == RESULT) {
                bslma::DestructorGuard<Obj> guard(&buffer.object());
            }
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
void TestDriver::testCase2a()
{
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

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

    bsls::ObjectBuffer<Obj> buffer;
    Obj& mX = buffer.object(); const Obj& X = mX;

    bslma::TestAllocator ta("test", veryVeryVerbose);
    bslma::TestAllocator *objAllocator = 0;

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
        const char CONFIG = cfg;

        switch (CONFIG) {
          case 'a': {
            objAllocator = da;
          } break;
          case 'b': {
            objAllocator = &ta;
          } break;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("Invalid allocator config!");
          } break;
        }
        bslma::TestAllocator& oa = *objAllocator;

        bslma::TestAllocator aa("args", veryVeryVeryVerbose);

        Obj *objPtr = buffer.address();

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            // 14 arguments, all using a local arguments allocator
            ArgType01 A01(V01, &aa);
            ArgType02 A02(V02, &aa);
            ArgType03 A03(V03, &aa);
            ArgType04 A04(V04, &aa);
            ArgType05 A05(V05, &aa);
            ArgType06 A06(V06, &aa);
            ArgType07 A07(V07, &aa);
            ArgType08 A08(V08, &aa);
            ArgType09 A09(V09, &aa);
            ArgType10 A10(V10, &aa);
            ArgType11 A11(V11, &aa);
            ArgType12 A12(V12, &aa);
            ArgType13 A13(V13, &aa);
            ArgType14 A14(V14, &aa);

            switch (N_ARGS) {
              case 0: {
                new (objPtr) Obj(&oa);
              } break;
              case 1: {
                new (objPtr) Obj(testArg(A01, MOVE_01), &oa);
              } break;
              case 2: {
                new (objPtr)
                    Obj(testArg(A01, MOVE_01), testArg(A02, MOVE_02), &oa);
              } break;
              case 3: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 &oa);
              } break;
              case 4: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 &oa);
              } break;
              case 5: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 &oa);
              } break;
              case 6: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 &oa);
              } break;
              case 7: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 &oa);
              } break;
              case 8: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 &oa);
              } break;
              case 9: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 testArg(A09, MOVE_09),
                                 &oa);
              } break;
              case 10: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 testArg(A09, MOVE_09),
                                 testArg(A10, MOVE_10),
                                 &oa);
              } break;
              case 11: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &oa);
              } break;
              case 12: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &oa);
              } break;
              case 13: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &oa);
              } break;
              case 14: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 testArg(A14, MOVE_14),
                                 &oa);
              } break;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("Invalid # of args!");
              } break;
            }

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

            ASSERTV(V01, X.arg01(), V01 == X.arg01() || 2 == N01);
            ASSERTV(V02, X.arg02(), V02 == X.arg02() || 2 == N02);
            ASSERTV(V03, X.arg03(), V03 == X.arg03() || 2 == N03);
            ASSERTV(V04, X.arg04(), V04 == X.arg04() || 2 == N04);
            ASSERTV(V05, X.arg05(), V05 == X.arg05() || 2 == N05);
            ASSERTV(V06, X.arg06(), V06 == X.arg06() || 2 == N06);
            ASSERTV(V07, X.arg07(), V07 == X.arg07() || 2 == N07);
            ASSERTV(V08, X.arg08(), V08 == X.arg08() || 2 == N08);
            ASSERTV(V09, X.arg09(), V09 == X.arg09() || 2 == N09);
            ASSERTV(V10, X.arg10(), V10 == X.arg10() || 2 == N10);
            ASSERTV(V11, X.arg11(), V11 == X.arg11() || 2 == N11);
            ASSERTV(V12, X.arg12(), V12 == X.arg12() || 2 == N12);
            ASSERTV(V13, X.arg13(), V13 == X.arg13() || 2 == N13);
            ASSERTV(V14, X.arg14(), V14 == X.arg14() || 2 == N14);

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        bslma::DestructorGuard<Obj> guard(objPtr);
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
    bslma::TestAllocator *da =
             dynamic_cast<bslma::TestAllocator *>(bslma::Default::allocator());
    BSLS_ASSERT(da);

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

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);

    bsls::ObjectBuffer<Obj> buffer;
    Obj& mX = buffer.object(); const Obj& X = mX;

    bslma::TestAllocator ta("test", veryVeryVerbose);
    bslma::TestAllocator *objAllocator = 0;

    bslma::TestAllocatorMonitor dam(da);

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
        const char CONFIG = cfg;

        // 14 arguments, all using a local arguments allocator
        ArgType01 A01(V01, &aa);
        ArgType02 A02(V02, &aa);
        ArgType03 A03(V03, &aa);
        ArgType04 A04(V04, &aa);
        ArgType05 A05(V05, &aa);
        ArgType06 A06(V06, &aa);
        ArgType07 A07(V07, &aa);
        ArgType08 A08(V08, &aa);
        ArgType09 A09(V09, &aa);
        ArgType10 A10(V10, &aa);
        ArgType11 A11(V11, &aa);
        ArgType12 A12(V12, &aa);
        ArgType13 A13(V13, &aa);
        ArgType14 A14(V14, &aa);

        Obj* objPtr = buffer.address();

        switch (cfg) {
          case 'a': {
            dam.reset(da);
            switch (N_ARGS) {
              case 0: {
                new (objPtr) Obj();
              } break;
              case 1: {
                new (objPtr) Obj(testArg(A01, MOVE_01));
              } break;
              case 2: {
                new (objPtr) Obj(testArg(A01, MOVE_01), testArg(A02, MOVE_02));
              } break;
              case 3: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03));
              } break;
              case 4: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04));
              } break;
              case 5: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05));
              } break;
              case 6: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06));
              } break;
              case 7: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07));
              } break;
              case 8: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08));
              } break;
              case 9: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                BSLS_ASSERT_INVOKE_NORETURN("Invalid # of args!");
              } break;
            }
            objAllocator = da;
          } break;
          case 'b': {
            dam.reset(da);
            switch (N_ARGS) {
              case 0: {
                new (objPtr) Obj(&ta);
              } break;
              case 1: {
                new (objPtr) Obj(testArg(A01, MOVE_01), &ta);
              } break;
              case 2: {
                new (objPtr)
                    Obj(testArg(A01, MOVE_01), testArg(A02, MOVE_02), &ta);
              } break;
              case 3: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 &ta);
              } break;
              case 4: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 &ta);
              } break;
              case 5: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 &ta);
              } break;
              case 6: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 &ta);
              } break;
              case 7: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 &ta);
              } break;
              case 8: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 &ta);
              } break;
              case 9: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 testArg(A09, MOVE_09),
                                 &ta);
              } break;
              case 10: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
                                 testArg(A02, MOVE_02),
                                 testArg(A03, MOVE_03),
                                 testArg(A04, MOVE_04),
                                 testArg(A05, MOVE_05),
                                 testArg(A06, MOVE_06),
                                 testArg(A07, MOVE_07),
                                 testArg(A08, MOVE_08),
                                 testArg(A09, MOVE_09),
                                 testArg(A10, MOVE_10),
                                 &ta);
              } break;
              case 11: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &ta);
              } break;
              case 12: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &ta);
              } break;
              case 13: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 &ta);
              } break;
              case 14: {
                new (objPtr) Obj(testArg(A01, MOVE_01),
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
                                 testArg(A14, MOVE_14),
                                 &ta);
              } break;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("Invalid # of args!");
              } break;
            }
            objAllocator = &ta;
          } break;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("Invalid config spec!");
          }
        }
        bslma::DestructorGuard<Obj> guard(&buffer.object());

        bslma::TestAllocator& oa = *objAllocator;

        ASSERTV(CONFIG, &aa == A01.allocator());
        ASSERTV(CONFIG, &aa == A02.allocator());
        ASSERTV(CONFIG, &aa == A03.allocator());
        ASSERTV(CONFIG, &aa == A04.allocator());
        ASSERTV(CONFIG, &aa == A05.allocator());
        ASSERTV(CONFIG, &aa == A06.allocator());
        ASSERTV(CONFIG, &aa == A07.allocator());
        ASSERTV(CONFIG, &aa == A08.allocator());
        ASSERTV(CONFIG, &aa == A09.allocator());
        ASSERTV(CONFIG, &aa == A10.allocator());
        ASSERTV(CONFIG, &aa == A11.allocator());
        ASSERTV(CONFIG, &aa == A12.allocator());
        ASSERTV(CONFIG, &aa == A13.allocator());
        ASSERTV(CONFIG, &aa == A14.allocator());

        ASSERTV(CONFIG, &oa == X.arg01().allocator() || 2 == N01);
        ASSERTV(CONFIG, &oa == X.arg02().allocator() || 2 == N02);
        ASSERTV(CONFIG, &oa == X.arg03().allocator() || 2 == N03);
        ASSERTV(CONFIG, &oa == X.arg04().allocator() || 2 == N04);
        ASSERTV(CONFIG, &oa == X.arg05().allocator() || 2 == N05);
        ASSERTV(CONFIG, &oa == X.arg06().allocator() || 2 == N06);
        ASSERTV(CONFIG, &oa == X.arg07().allocator() || 2 == N07);
        ASSERTV(CONFIG, &oa == X.arg08().allocator() || 2 == N08);
        ASSERTV(CONFIG, &oa == X.arg09().allocator() || 2 == N09);
        ASSERTV(CONFIG, &oa == X.arg10().allocator() || 2 == N10);
        ASSERTV(CONFIG, &oa == X.arg11().allocator() || 2 == N11);
        ASSERTV(CONFIG, &oa == X.arg12().allocator() || 2 == N12);
        ASSERTV(CONFIG, &oa == X.arg13().allocator() || 2 == N13);
        ASSERTV(CONFIG, &oa == X.arg14().allocator() || 2 == N14);

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

        ASSERTV(V01, X.arg01(), V01 == X.arg01() || 2 == N01);
        ASSERTV(V02, X.arg02(), V02 == X.arg02() || 2 == N02);
        ASSERTV(V03, X.arg03(), V03 == X.arg03() || 2 == N03);
        ASSERTV(V04, X.arg04(), V04 == X.arg04() || 2 == N04);
        ASSERTV(V05, X.arg05(), V05 == X.arg05() || 2 == N05);
        ASSERTV(V06, X.arg06(), V06 == X.arg06() || 2 == N06);
        ASSERTV(V07, X.arg07(), V07 == X.arg07() || 2 == N07);
        ASSERTV(V08, X.arg08(), V08 == X.arg08() || 2 == N08);
        ASSERTV(V09, X.arg09(), V09 == X.arg09() || 2 == N09);
        ASSERTV(V10, X.arg10(), V10 == X.arg10() || 2 == N10);
        ASSERTV(V11, X.arg11(), V11 == X.arg11() || 2 == N11);
        ASSERTV(V12, X.arg12(), V12 == X.arg12() || 2 == N12);
        ASSERTV(V13, X.arg13(), V13 == X.arg13() || 2 == N13);
        ASSERTV(V14, X.arg14(), V14 == X.arg14() || 2 == N14);

        if (&oa == &ta) {
            ASSERTV(CONFIG, dam.isMaxSame());
            ASSERTV(CONFIG, dam.isInUseSame());
        }
    }
}
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace UsageExample {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Testing Methods With Argument Forwarding
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsltf::AllocEmplacableTestType' to
// test the implementation of a container's 'emplace' method.
//
// Note, that the example below provides separate implementations for compilers
// that support C++11 standard and those that do not.  For clarity, we limit
// our expansion of the variadic template method, 'emplace', to 2 arguments on
// platforms that don't support variadic templates.
//
// First, we define the container we intend to test:
//..
template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
class Container {
    // This class template implements a value-semantic container type holding
    // an element of the (template parameter) type 'TYPE'.  This class provides
    // an 'emplace' method that constructs the element by forwarding a variable
    // number of arguments to the 'TYPE' constructor.

  private:
    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOC> AllocatorTraits;

    // DATA
    TYPE  *d_data_p;    // pointer to value (owned)
    ALLOC  d_allocator; // allocator

  public:
    // CREATORS
    Container(const ALLOC& basicAllocator = ALLOC());
        // Create an empty container.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type 'ALLOC'
        // is used.  If the type 'ALLOC' is 'bsl::allocator' and
        // 'basicAllocator' is not supplied, the currently installed default
        // allocator is used.  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the type 'ALLOC' is 'bsl::allocator' (the
        // default).

    ~Container();
        // Destroy this object.

    // MANIPULATORS
    #if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

    template <class... Args>
    void emplace(Args&&... arguments);
        // Insert into this container a newly created 'TYPE' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'TYPE'.  Return an iterator referring to the newly
        // created object in this container.

    #elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    void emplace();

    template <class Args_01>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class Args_01,
              class Args_02>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                 BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    #else
    // The code below is a workaround for the absence of perfect forwarding in
    // some compilers.
    template <class... Args>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    #endif

    // ACCESSORS
    TYPE&       front();
    const TYPE& front() const;
        // Return a reference to the element in this container.
};

// CREATORS
template <class TYPE, class ALLOC>
Container<TYPE, ALLOC>::Container(const ALLOC& basicAllocator)
: d_data_p(0)
, d_allocator(basicAllocator)
{
}

template <class TYPE, class ALLOC>
Container<TYPE, ALLOC>::~Container()
{
    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE, class ALLOC>
template <class... Args>
void
Container<TYPE, ALLOC>::emplace(Args&&... arguments)
{
    TYPE* newData_p = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::construct(
                            d_allocator,
                            newData_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }

    d_data_p = newData_p;
}

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <class TYPE, class ALLOC>
void
Container<TYPE, ALLOC>::emplace()
{
    TYPE* newData_p = AllocatorTraits::allocate(d_allocator, 1);

    AllocatorTraits::construct(d_allocator, newData_p);

    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }

    d_data_p = newData_p;
}

template <class TYPE, class ALLOC>
template <class Args_01>
void
Container<TYPE, ALLOC>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    TYPE* newData_p = AllocatorTraits::allocate(d_allocator, 1);

    AllocatorTraits::construct(
                         d_allocator,
                         newData_p,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));

    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }

    d_data_p = newData_p;
}

template <class TYPE, class ALLOC>
template <class Args_01, class Args_02>
void
Container<TYPE, ALLOC>::emplace(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    TYPE* newData_p = AllocatorTraits::allocate(d_allocator, 1);

    AllocatorTraits::construct(
                         d_allocator,
                         newData_p,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));

    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }

    d_data_p = newData_p;
}

#else
// The code below is a workaround for the absence of perfect forwarding in
// some compilers.
template <class TYPE, class ALLOC>
template <class... Args>
void
Container<TYPE, ALLOC>::emplace(
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    TYPE* newData_p = AllocatorTraits::allocate(d_allocator, 1);

    AllocatorTraits::construct(
                            d_allocator,
                            newData_p,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

    if (d_data_p) {
        AllocatorTraits::destroy(d_allocator, d_data_p);
        AllocatorTraits::deallocate(d_allocator, d_data_p, 1);
    }

    d_data_p = newData_p;
}
#endif

// ACCESSORS
template <class TYPE, class ALLOC>
TYPE&
Container<TYPE, ALLOC>::front()
{
    return *d_data_p;
}

template <class TYPE, class ALLOC>
const TYPE&
Container<TYPE, ALLOC>::front() const
{
    return *d_data_p;
}
//..
// Then, we provide test machinery that will invoke the 'emplace' method with
// variable number of arguments:
//..
template <class T>
bslmf::MovableRef<T> forwardCtorArg(T& argument, bsl::true_type)
    // Return 'bslmf::MovableRef' to the specified 'argument'.
{
    return bslmf::MovableRefUtil::move(argument);
}

template <class T>
const T& forwardCtorArg(T& argument, bsl::false_type)
    // Return a reference providing non-modifiable access to the specified
    // 'argument'.
{
    return argument;
}

template <int N_ARGS, bool MOVE_ARG_01, bool MOVE_ARG_02>
void
testCaseHelper()
    // Call 'emplace' on the container and verify that value was correctly
    // constructed and inserted into the container.  Forward (template
    // parameter) 'N_ARGS' arguments to the 'emplace' method and ensure 1) that
    // values are properly passed to the constructor of
    // 'bsltf::AllocEmplacableTestType', 2) that the allocator is correctly
    // configured for each argument in the newly inserted element in 'target',
    // and 3) that the arguments are forwarded using copy ('false') or move
    // semantics ('true') based on bool template parameters 'MOVE_ARG_01' ...
    // 'MOVE_ARG_02'.
{
    bslma::TestAllocator ta;

    Container<bsltf::AllocEmplacableTestType>        mX(&ta);
    const Container<bsltf::AllocEmplacableTestType>& X = mX;

    // Prepare the arguments
    bslma::TestAllocator aa("args", veryVeryVeryVerbose);

    bsltf::AllocArgumentType<1> A01(18, &aa);
    bsltf::AllocArgumentType<2> A02(33, &aa);

    DECLARE_BOOL_CONSTANT(MOVE_01, MOVE_ARG_01);
    DECLARE_BOOL_CONSTANT(MOVE_02, MOVE_ARG_02);

    switch (N_ARGS) {
      case 0: {
        mX.emplace();
      } break;
      case 1: {
        mX.emplace(forwardCtorArg(A01, MOVE_01));
      } break;
      case 2: {
        mX.emplace(forwardCtorArg(A01, MOVE_01),
                   forwardCtorArg(A02, MOVE_02));
      } break;
      default: {
        ASSERT(0);
      } break;
    }

    // Verify that, depending on the corresponding template parameters,
    // arguments were copied or moved.
    ASSERT(MOVE_ARG_01 == (bsltf::MoveState::e_MOVED == A01.movedFrom()));
    ASSERT(MOVE_ARG_02 == (bsltf::MoveState::e_MOVED == A02.movedFrom()));

    // Verify that the element was constructed correctly.
    const bsltf::AllocEmplacableTestType& V = X.front();

    ASSERT(18 == V.arg01() || N_ARGS < 1);
    ASSERT(33 == V.arg02() || N_ARGS < 2);

    ASSERT(&ta == V.arg01().allocator() || N_ARGS < 1);
    ASSERT(&ta == V.arg02().allocator() || N_ARGS < 2);
}
//..
// Finally, we call our templatized test case helper with a variety of template
// arguments:
//..
#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) \
 && (defined(BSLS_PLATFORM_CMP_IBM)   \
  || defined(BSLS_PLATFORM_CMP_CLANG) \
  || defined(BSLS_PLATFORM_CMP_MSVC)  \
  ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130) \
     )
# define EXAMPLE_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

void testCase()
{
#ifndef EXAMPLE_DO_NOT_TEST_MOVE_FORWARDING
    testCaseHelper<0, false, false>();

    testCaseHelper<1, false, false>();
    testCaseHelper<1, true,  false>();

    testCaseHelper<2, false, false>();
    testCaseHelper<2, true,  false>();
    testCaseHelper<2, false, true >();
    testCaseHelper<2, true,  true >();
#else
    testCaseHelper<0, false, false>();
    testCaseHelper<1, false, false>();
    testCaseHelper<2, false, false>();
#endif
}
//..
}  // close UsageExample namespace

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

    (void)veryVerbose;      // suppress warning
    (void)veryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated from global allocator

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default
    // allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        UsageExample::testCase();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        TestDriver::testCase13();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'getNumDeletes' CLASS METHOD
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'getNumDeletes' CLASS METHOD"
                            "\n====================================\n");

        TestDriver::testCase12();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual' METHOD
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'isEqual' METHOD"
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

        if (verbose) printf("\nTESTING COPY-ASSIGNMENT"
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

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        TestDriver::testCase7();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATORS"
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

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");
        TestDriver::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'gg' AND 'ggg'"
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
        //:
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
        //:
        //: 3 Call 'testCase2a' to repeat P2 in the presence of injected
        //:   exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros).
        //
        // Testing:
        //   AllocEmplacableTestType(Allocator *ba);
        //   AllocEmplacableTestType(arg01, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg02, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg03, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg04, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg05, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg06, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg07, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg08, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg09, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg10, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg11, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg12, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg13, Allocator *ba);
        //   AllocEmplacableTestType(arg01 .. arg14, Allocator *ba);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTORS"
                            "\n==========================\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\nTesting contructor with no arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase2<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 1 argument"
                            "\n----------------------------------\n");
        TestDriver::testCase2<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 2 arguments"
                            "\n----------------------------------\n");
        TestDriver::testCase2<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 3 arguments"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 4 arguments"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 5 arguments"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 6 arguments"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 7 arguments"
                            "\n-----------------------------------\n");
        TestDriver::testCase2<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase2<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        TestDriver::testCase2a<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase2a<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 8 arguments"
                            "\n-----------------------------------\n");
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

        TestDriver::testCase2a<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        TestDriver::testCase2a<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 9 arguments"
                            "\n-----------------------------------\n");
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

        TestDriver::testCase2a<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        TestDriver::testCase2a<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        TestDriver::testCase2a<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 10 arguments"
                            "\n------------------------------------\n");
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

        TestDriver::testCase2a<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        TestDriver::testCase2a<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting contructor with 11 arguments"
                            "\n------------------------------------\n");
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

        TestDriver::testCase2a<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        TestDriver::testCase2a<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting contructor with 12 arguments"
                            "\n------------------------------------\n");
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

        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        TestDriver::testCase2a<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting contructor with 13 arguments"
                            "\n------------------------------------\n");
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

        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        TestDriver::testCase2a<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\nTesting contructor with 14 arguments"
                            "\n------------------------------------\n");
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

        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        TestDriver::testCase2a<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else
        if (verbose) printf("\nTesting ctor with 1..14 arguments, move=0"
                            "\n-----------------------------------------\n");
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

        TestDriver::testCase2a< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase2a< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase2a< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase2a<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase2a<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase2a<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase2a<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase2a<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        Obj mX(V01);    const Obj& X = mX;
        Obj mY;
        ASSERT(mY != X);
        Obj& mZ = (mY = mX);

        ASSERT(&mZ == &mY);
        ASSERT(mY == mX);
        ASSERT(mZ == mX);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
