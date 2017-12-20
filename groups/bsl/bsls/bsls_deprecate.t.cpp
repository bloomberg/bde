// bsls_deprecate.t.cpp                                               -*-C++-*-
#include <bsls_deprecate.h>

#if !defined(BSLS_DEPRECATE_T_DATA_COLLECTION)

// BDE_VERIFY pragma: push    // Relax some bde_verify rules
// BDE_VERIFY pragma: -TP19   // Component levelized below 'bsls_bsltestutil',
//                            // therefore cannot use usual boilerplate

#include <limits.h>  // 'INT_MIN'
#include <stdio.h>
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'strcmp'

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines macros for marking interfaces as
// deprecated.  This component has no runtime behavior, and therefore no
// primary manipulator or basic accessor.  The macros will be tested in strict
// dependency order.
//
// The 'BSLS_DEPRECATE' macro defined by the component expands either to some
// platform-dependent deprecation attribute, or to nothing.  The test for this
// macro therefore needs to observe the expansion of 'BSLS_DEPRECATE' of the
// macros under various circumstances.  Special test apparatus is provided to
// make this possible: a macro named 'MACRO_TO_STRING' that expands to a string
// literal having the expansion of the macro as its value.
//
///Table-Based Tests of Compile-Time Behavior
///------------------------------------------
// This test driver tests that have complex *compile-time* behavior.  We would
// like to be able to render the tests using tables in order to better
// visualize the assertions we are making.  Therefore, we use a non-standard
// technique to convert the component's compile-time behavior into data that
// can be loaded into a table and checked at run time.
//
// There are two parts to the facility to convert compile-time (preprocessor)
// state into data suitable for inclusion in a table:
//
//: o Data-collection sections in the test driver, in which some compile-time
//:   state is encoded in a form that can be retreived at run-time.  For cases
//:   6, 7, and 8, states of control macros are assigned to cells in an array.
//:   For case 11, functions are defined that can return a summary of the
//:   states of control macros and deprecation macros when the functions were
//:   defined.  These sections are separated from the rest of the test driver
//:   by a '#if defined(BSLS_DEPRECATE_T_DATA_COLLECTION)' directive.  When the
//:   compiler looks at the test driver, it will see *either* the body of the
//:   test driver proper, *or* a data-collection section.  (See the various
//:   sections titled "DATA COLLECTION: *")
//:
//: o Data-collection driver sections that define
//:   'BSLS_DEPRECATE_T_DATA_COLLECTION' and then repeatedly '#include' the
//:   test driver file with various values set for the control macros or other
//:   preprocessor state used by the appropriate data-collection section.  Each
//:   '#include' will make the data-collection section encode a set of
//:   preprocessor state.  For cases 6, 7, and 8, the data-collection driver
//:   designates a different combination of UOR, major version, and minor
//:   version to inspect on each '#include'.  For case 11, the data-collection
//:   driver designates a different combination of deprecation threshold and
//:   legacy deprecation macro state to inspect on each '#include'.  (See the
//:   various sections titled "DATA COLLECTION DRIVER FOR CASE *".)
//
// Additionally, deprecation control macros are defined for a set of fictional
// UORs, to be used in any test cases that need them.  (See "GLOBAL DEPRECATION
// CONTROL MACRO SETTINGS").  The UOR configuration specifies:
//:   * UOR version
//:   * UOR deprecation threshold
//:   * internal UOR build status ('BB_BUILDING_UOR_<U>')
//:   * deprecation silencing ('BB_SILENCE_DEPRECATION_<U>_<M>_<N>')
//
// These UOR configurations are used primarily in cases 6, 7, and 8.
//
// Finally, the data-collection section for cases 6, 7, and 8 uses a set of
// globally-defined macros to extract the state of each control macro, and
// store that stae in an array.  These macros depend on the internal macros
// tested in case 4, and have a breathing test in case -2.  (See "GLOBAL TEST
// DATA COLLECTION MACHINERY").
// ----------------------------------------------------------------------------
// [10] BSLS_DEPRECATE
// [ 9] BSLS_DEPRECATE_COMPILER_SUPPORT
// [ 8] BSLS_DEPRECATE_IS_ACTIVE(U, M, N)
// [ 5] BSLS_DEPRECATE_MAKE_VER(M, N)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS: MACRO_TO_STRING
// [ 3] BSLS_DEPRECATE_CAT(A, B)
// [ 4] BSLS_DEPRECATE_ISDEFINED
// [ 4] BSLS_DEPRECATE_ISNONZERO
// [ 6] BSLS_DEPRECATE_ISRETAINED
// [ 7] BSLS_DEPRECATE_ISPASTTHRESHOLD
// [11] INTERACTIONS WITH LEGACY MACROS
// [12] USAGE EXAMPLE
// [ *] CONCERN: DEPRECATION ATTRIBUTE IS VALID
// [-1] SUPPORT MACRO RESEARCH: ISZERO
// [-2] TEST APPARATUS: DATA COLLECTION

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//                  STANDARD BDE ASSERT TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT(X) do { aSsErT(!(X), #X, __LINE__); } while (false)

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) {                                                               \
        printf("%s: %d\n", #I, I);                                            \
        aSsErT(1, #X, __LINE__);                                              \
    }                                                                         \
}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) {                                                               \
        printf("%s: %d\t%s: %d\n", #I, I, #J, J);                             \
        aSsErT(1, #X, __LINE__);                                              \
    }                                                                         \
}

#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) {                                                               \
        printf("%s: %d\t%s: %d\t%s: %d\n", #I, I, #J, J, #K, K);              \
        aSsErT(1, #X, __LINE__);                                              \
    }                                                                         \
}

#define LOOP_S_ASSERT(I,X) {                                                  \
    if (!(X)) {                                                               \
        printf("%s: %s\n", #I, I);                                            \
        aSsErT(1, #X, __LINE__);                                              \
    }                                                                         \
}

#define T_  putchar('\t'); fflush(stdout);  // Print a tab (w/o newline).
#define L_  __LINE__                        // current Line number

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

#define EMPTY_STRING ""
#define MACRO_TO_STRING(...) MACRO_TO_STRING_A((__VA_ARGS__, EMPTY_STRING))
#define MACRO_TO_STRING_A(T) MACRO_TO_STRING_B T
#define MACRO_TO_STRING_B(M, ...) #M

#define STATIC_ASSERT(N, X) do{ int N[1 - (2 * !(X))]; (void) N; }while(0)
    // Poor-man's static assertion, rumored to not be guaranteed to fail on
    // Sun, and therefore usable only for expressions that will have the same
    // value on all configurations of all compilers, such as expressions of
    // internal test logic, not compiler, library or process configuration.

// ============================================================================
//                       GLOBAL TYPES FOR TESTING
// ----------------------------------------------------------------------------

struct Case2 {
    enum {
        e_DEFAULT,
        e_C_STRING,
        e_CONST_C_STRING
    };

    static int test(...                );
    static int test(char       *       );
    static int test(char const * const&);
        // Return an integer identifying which of the following three type
        // categories matches the argument passed to this function:
        //..
        //       Argument Type        Return Value
        //  +---------------------+------------------+
        //  | char const * const& | e_CONST_C_STRING |
        //  | char       *        | e_C_STRING       |
        //  | other               | e_DEFAULT        |
        //  +---------------------+------------------+
        //..
};

int Case2::test(...)
{
    return e_DEFAULT;
}

int Case2::test(char *)
{
    return e_C_STRING;
}

int Case2::test(char const * const&)
{
    return e_CONST_C_STRING;
}


struct Case5 {
    enum {
        e_DEFAULT,
        e_DOUBLE,
        e_INTEGER
    };

    static int test(...       );
    static int test(double    );
    static int test(const int&);
        // Return an integer identifying which of the following three type
        // categories matches the argument passed to this function:
        //..
        //       Argument Type        Return Value
        //  +---------------------+------------------+
        //  | const int&          | e_INTEGER        |
        //  | double              | e_DOUBLE         |
        //  | other               | e_DEFAULT        |
        //  +---------------------+------------------+
        //..
};

int Case5::test(...)
{
    return e_DEFAULT;
}

int Case5::test(double)
{
    return e_DOUBLE;
}

int Case5::test(const int&)
{
    return e_INTEGER;
}


// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                 GLOBAL DEPRECATION CONTROL MACRO SETTINGS
// ----------------------------------------------------------------------------

// For 'defaultData'-based tables

#define BBB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBD_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBE_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBF_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BBH_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)

#define BB_SILENCE_DEPRECATION_BBC_4_1
#define BB_SILENCE_DEPRECATION_BBD_4_2
#define BB_SILENCE_DEPRECATION_BBE_4_3
#define BB_SILENCE_DEPRECATION_BBG_4_2
#define BB_SILENCE_DEPRECATION_BBH_4_3

#define BB_BUILDING_UOR_BBF
#define BB_BUILDING_UOR_BBG
#define BB_BUILDING_UOR_BBH

#define CCB_VERSION_MAJOR 4
#define CCB_VERSION_MINOR 3
#define CCC_VERSION_MAJOR 4
#define CCC_VERSION_MINOR 3
#define CCD_VERSION_MAJOR 4
#define CCD_VERSION_MINOR 3
#define CCE_VERSION_MAJOR 4
#define CCE_VERSION_MINOR 3
#define CCF_VERSION_MAJOR 4
#define CCF_VERSION_MINOR 3
#define CCG_VERSION_MAJOR 4
#define CCG_VERSION_MINOR 3
#define CCH_VERSION_MAJOR 4
#define CCH_VERSION_MINOR 3

#define BB_BUILDING_UOR_CCF
#define BB_BUILDING_UOR_CCG
#define BB_BUILDING_UOR_CCH

#define BB_SILENCE_DEPRECATION_CCC_4_1
#define BB_SILENCE_DEPRECATION_CCD_4_2
#define BB_SILENCE_DEPRECATION_CCE_4_3
#define BB_SILENCE_DEPRECATION_CCG_4_2
#define BB_SILENCE_DEPRECATION_CCH_4_3

#define DDB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDD_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDE_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDF_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define DDH_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)

#define DDB_VERSION_MAJOR 4
#define DDB_VERSION_MINOR 5
#define DDC_VERSION_MAJOR 4
#define DDC_VERSION_MINOR 5
#define DDD_VERSION_MAJOR 4
#define DDD_VERSION_MINOR 5
#define DDE_VERSION_MAJOR 4
#define DDE_VERSION_MINOR 5
#define DDF_VERSION_MAJOR 4
#define DDF_VERSION_MINOR 5
#define DDG_VERSION_MAJOR 4
#define DDG_VERSION_MINOR 5
#define DDH_VERSION_MAJOR 4
#define DDH_VERSION_MINOR 5

#define BB_SILENCE_DEPRECATION_DDC_4_1
#define BB_SILENCE_DEPRECATION_DDD_4_2
#define BB_SILENCE_DEPRECATION_DDE_4_3
#define BB_SILENCE_DEPRECATION_DDG_4_2
#define BB_SILENCE_DEPRECATION_DDH_4_3

#define BB_BUILDING_UOR_DDF
#define BB_BUILDING_UOR_DDG
#define BB_BUILDING_UOR_DDH

#define EEB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EEC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EED_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EEE_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EEF_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EEG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define EEH_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)

#define EEB_VERSION_MAJOR 4
#define EEB_VERSION_MINOR 2
#define EEC_VERSION_MAJOR 4
#define EEC_VERSION_MINOR 2
#define EED_VERSION_MAJOR 4
#define EED_VERSION_MINOR 2
#define EEE_VERSION_MAJOR 4
#define EEE_VERSION_MINOR 2
#define EEF_VERSION_MAJOR 4
#define EEF_VERSION_MINOR 2
#define EEG_VERSION_MAJOR 4
#define EEG_VERSION_MINOR 2
#define EEH_VERSION_MAJOR 4
#define EEH_VERSION_MINOR 2

#define BB_SILENCE_DEPRECATION_EEC_4_1
#define BB_SILENCE_DEPRECATION_EED_4_2
#define BB_SILENCE_DEPRECATION_EEE_4_3
#define BB_SILENCE_DEPRECATION_EEG_4_2
#define BB_SILENCE_DEPRECATION_EEH_4_3

#define BB_BUILDING_UOR_EEF
#define BB_BUILDING_UOR_EEG
#define BB_BUILDING_UOR_EEH

#define BB_SILENCE_DEPRECATION_FFC_3_1
#define BB_SILENCE_DEPRECATION_FFD_3_2
#define BB_SILENCE_DEPRECATION_FFE_3_3
#define BB_SILENCE_DEPRECATION_FFG_3_2
#define BB_SILENCE_DEPRECATION_FFH_3_3

#define BB_BUILDING_UOR_FFF
#define BB_BUILDING_UOR_FFG
#define BB_BUILDING_UOR_FFH

#define GGB_VERSION_MAJOR 2
#define GGB_VERSION_MINOR 0
#define GGC_VERSION_MAJOR 2
#define GGC_VERSION_MINOR 0
#define GGD_VERSION_MAJOR 2
#define GGD_VERSION_MINOR 0
#define GGE_VERSION_MAJOR 2
#define GGE_VERSION_MINOR 0
#define GGF_VERSION_MAJOR 2
#define GGF_VERSION_MINOR 0
#define GGG_VERSION_MAJOR 2
#define GGG_VERSION_MINOR 0
#define GGH_VERSION_MAJOR 2
#define GGH_VERSION_MINOR 0

#define BB_SILENCE_DEPRECATION_GGC_1_998
#define BB_SILENCE_DEPRECATION_GGD_1_999
#define BB_SILENCE_DEPRECATION_GGE_2_0
#define BB_SILENCE_DEPRECATION_GGG_1_999
#define BB_SILENCE_DEPRECATION_GGH_2_0

#define BB_BUILDING_UOR_GGF
#define BB_BUILDING_UOR_GGG
#define BB_BUILDING_UOR_GGH

#define RST_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(7, 9)
#define BB_SILENCE_DEPRECATION_RST_7_8

#define UVW_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 5)
#define BB_BUILDING_UOR_UVW

// ============================================================================
//                   GLOBAL TEST DATA COLLECTION MACHINERY
// ----------------------------------------------------------------------------

// Data Collection Macros

#define COLLECT_SET_VALUE(A, U, M, N, V)    COLLECT_SET_VALUE_A(A, U, M, N, V)
#define COLLECT_SET_VALUE_A(A, U, M, N, V)  A[U][M][N % 20] = V;

// Data Collection Support Macros

#define COLLECT_BB_ISDEFINED_U(P, U)   COLLECT_BB_ISDEFINED_U_A(P, U)
#define COLLECT_BB_ISDEFINED_U_A(P, U) COLLECT_BB_ISDEFINED_U_B(P, U)
#define COLLECT_BB_ISDEFINED_U_B(P, U) BSLS_DEPRECATE_ISDEFINED(P ##_## U)

#define COLLECT_BB_ISDEFINED_UMN(P, U, M, N)                                  \
                                       COLLECT_BB_ISDEFINED_UMN_A(P, U, M, N)
#define COLLECT_BB_ISDEFINED_UMN_A(P, U, M, N)                                \
                                       COLLECT_BB_ISDEFINED_UMN_B(P, U, M, N)
#define COLLECT_BB_ISDEFINED_UMN_B(P, U, M, N)                                \
                            BSLS_DEPRECATE_ISDEFINED(P ##_## U ##_## M ##_## N)

#define COLLECT_MKNAME(U, V)             COLLECT_MKNAME_A(U, V)
#define COLLECT_MKNAME_A(U, V)           U ##_## V

#define COLLECT_EXTRACT1(...)            COLLECT_EXTRACT1_A((__VA_ARGS__))
#define COLLECT_EXTRACT1_A(X)            COLLECT_EXTRACT1_B X
#define COLLECT_EXTRACT1_B(A, ...)       A

#define COLLECT_EXTRACT2(...)            COLLECT_EXTRACT2_A((__VA_ARGS__))
#define COLLECT_EXTRACT2_A(X)            COLLECT_EXTRACT2_B X
#define COLLECT_EXTRACT2_B(A, B, ...)    B

#define COLLECT_EXTRACT3(...)            COLLECT_EXTRACT3_A((__VA_ARGS__))
#define COLLECT_EXTRACT3_A(X)            COLLECT_EXTRACT3_B X
#define COLLECT_EXTRACT3_B(A, B, C)      C

#define COLLECT_ISLITERALZERO(N)   COLLECT_ISLITERALZERO_A(N)
#define COLLECT_ISLITERALZERO_A(N) COLLECT_SENTINEL_ ## N
#define COLLECT_SENTINEL_0         1

#define COLLECT_ISNONZERO(U, V)  BSLS_DEPRECATE_ISNONZERO(COLLECT_MKNAME(U, V))

#define COLLECT_ISNUMERIC(U, V)                                               \
    (   COLLECT_ISNONZERO(U, V)                                               \
     || COLLECT_ISLITERALZERO(COLLECT_MKNAME(U, V)))

// ============================================================================
//                    TEST SUPPORT FOR CASES 6, 7, AND 8
// ----------------------------------------------------------------------------

namespace defaultData {

enum Trinary {
    e_UNKNOWN,
    e_YES,
    e_NO
};

enum UORS {
    AAA,
    BBB, BBC, BBD, BBE, BBF, BBG, BBH,
    CCB, CCC, CCD, CCE, CCF, CCG, CCH,
    DDB, DDC, DDD, DDE, DDF, DDG, DDH,
    EEB, EEC, EED, EEE, EEF, EEG, EEH,
    FFB, FFC, FFD, FFE, FFF, FFG, FFH,
    GGB, GGC, GGD, GGE, GGF, GGG, GGH,
    RST,
    UVW,

    ZZZ,

    NUM_UORS
};

static const int MAX_MAJ = 7 + 1;
static const int MAX_MIN = 999 + 1;

static int expectedActive   [NUM_UORS][MAX_MAJ][20] = {};
static int expectedRetained [NUM_UORS][MAX_MAJ][20] = {};
static int expectedIsPast   [NUM_UORS][MAX_MAJ][20] = {};
static int isWarnDefined    [NUM_UORS][MAX_MAJ][20] = {};
static int isSilenceDefined [NUM_UORS][MAX_MAJ][20] = {};
static int isBuildingDefined[NUM_UORS][MAX_MAJ][20] = {};
static int threshold        [NUM_UORS][MAX_MAJ][20] = {};
static int majorVersion     [NUM_UORS][MAX_MAJ][20] = {};
static int minorVersion     [NUM_UORS][MAX_MAJ][20] = {};

void collectData();
    // Populate the three-dimensional test result arrays used in cases 6,7, and
    // 8 with the states of the deprecation macros and control macros
    // corresponding to each of the fictional UORs configured in the "GLOBAL
    // DEPRECATION CONTROL MACRO SETTINGS" section.  In order to avoid compiler
    // warnings related to the sizes of the arrays, for UOR versions having a
    // minor version number in the range '[0 .. 9]', store each macro state in
    // the cell at index '(uor_id, major_version, minor_version)', and for UOR
    // versions having a minor version number in the range '[990 .. 999]',
    // store each macro state in the cell at index
    // '(uor_id, major_version, minor_version - 980)'.

void collectData()
{
    STATIC_ASSERT(minorVersionsAlignToTwenty, 0 == MAX_MIN % 20);

    // For 'minorVersion' only, '0' is a valid test value.  Initialize all
    // 'minorVersion' to 'INT_MIN' to expose unset datapoints.

    for (int i = 0; i < NUM_UORS; ++i) {
        for (int j = 0; j < MAX_MAJ; ++j) {
            for (int k = 0; k < 20; ++k) {
                minorVersion[i][j][k] = INT_MIN;
            }
        }
    }

// ============================================================================
//               DATA COLLECTION DRIVER FOR CASES 6, 7, and 8
// ----------------------------------------------------------------------------

#define BSLS_DEPRECATE_T_DATA_COLLECTION 1

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD02  // re-inclusion confuses 'bde_verify'

//                               UOR  MAJOR  MINOR
//                               ---  -----  -----
#define COLLECT_CODE             AAA,     1,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBB,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBB,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBB,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBC,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBC,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBC,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBD,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBD,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBD,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBE,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBE,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBE,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBF,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBF,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBF,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBG,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBG,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBG,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBH,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBH,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             BBH,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCB,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCB,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCB,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCC,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCC,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCC,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCD,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCD,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCD,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCE,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCE,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCE,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCF,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCF,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCF,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCG,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCG,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCG,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCH,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCH,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             CCH,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDB,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDB,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDB,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDC,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDC,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDC,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDD,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDD,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDD,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDE,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDE,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDE,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDF,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDF,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDF,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDG,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDG,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDG,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDH,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDH,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             DDH,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEB,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEB,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEB,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEC,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEC,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEC,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EED,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EED,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EED,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEE,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEE,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEE,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEF,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEF,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEF,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEG,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEG,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEG,     4,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEH,     4,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEH,     4,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             EEH,     4,     3
#include "bsls_deprecate.t.cpp"

#define BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY

#define COLLECT_CODE             FFB,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFB,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFB,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFC,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFC,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFC,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFD,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFD,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFD,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFE,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFE,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFE,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFF,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFF,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFF,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFG,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFG,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFG,     3,     3
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFH,     3,     1
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFH,     3,     2
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             FFH,     3,     3
#include "bsls_deprecate.t.cpp"

#undef BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY

#define COLLECT_CODE             GGB,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGB,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGB,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGC,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGC,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGC,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGD,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGD,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGD,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGE,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGE,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGE,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGF,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGF,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGF,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGG,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGG,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGG,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGH,     1,   998
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGH,     1,   999
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             GGH,     2,     0
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             RST,     7,     7
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             RST,     7,     8
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             RST,     7,     9
#include "bsls_deprecate.t.cpp"
#define COLLECT_CODE             UVW,     4,     7
#include "bsls_deprecate.t.cpp"

// BDE_VERIFY pragma: pop

#undef  BSLS_DEPRECATE_T_DATA_COLLECTION
}

}  // close namespace defaultData

// ============================================================================
//            TEST SUPPORT FOR CASE 11: LEGACY MACRO INTERACTION
// ----------------------------------------------------------------------------

#define BSLS_DEPRECATE_STAND_IN typedef DeprecatedTag Tag;
#define BSLS_DEPRECATE_STAND_IN_STRUCT Tag : DeprecatedTag {}; struct

namespace legacyInteractions {

enum DeprecationStatus {
    e_UNKNOWN,     // The status of the function or class has not been checked.
    e_SUPPORTED,   // The function or class is present and not deprecated.
    e_DEPRECATED,  // The function or class has been deprecated.
    e_DELETED      // The function or class has been deleted.
};

struct SupportedTag {
    enum { value = e_SUPPORTED };
};

struct DeprecatedTag {
    enum { value = e_DEPRECATED };
};

struct DeletedTag {
    enum { value = e_DELETED };
};

DeletedTag DEPRECATED_SYMBOL()
    // Return a default-constructed 'DeletedTag'.
{
    return DeletedTag();
}

typedef SupportedTag Tag;

struct Results {
    // This 'struct' encodes information about the deprecation configuration
    // during the compilation of a particular code block.

    int  d_deprecationStatus;     // The status of 'DEPRECATED_SYMBOL' in the
                                  // current scope, represented as a value
                                  // supported by 'DeprecationStatus'.

    bool d_isActive;              // Whether or not 'bsls_deprecate'
                                  // deprecations are active for version 4.3 of
                                  // 'leg'.

    bool d_isLegacyMacroDefined;  // Whether or not 'BDE_OMIT_DEPRECATED' is
                                  // defined.
};

typedef Results (*Checker)();

// ============================================================================
//                    DATA COLLECTION DRIVER FOR CASE 11
// ----------------------------------------------------------------------------

#define BSLS_DEPRECATE_T_DATA_COLLECTION 2

// Setup

#undef LEG_VERSION_DEPRECATION_THRESHOLD
#undef BDE_OMIT_DEPRECATED

// ff: LEG 4.3 deprecations inactive; no legacy deprecations

namespace ff {
#define LEG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#undef  BDE_OMIT_DEPRECATED
#include "bsls_deprecate.t.cpp"
}  // close namespace ff

// ft: LEG 4.3 deprecations inactive; with legacy deprecations

namespace ft {
#define LEG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 2)
#define BDE_OMIT_DEPRECATED
#include "bsls_deprecate.t.cpp"
}  // close namespace ft

// tf: LEG 4.3 deprecations active; no legacy deprecations

namespace tf {
#define LEG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 3)
#undef  BDE_OMIT_DEPRECATED
#include "bsls_deprecate.t.cpp"
}  // close namespace tf

// tt: LEG 4.3 deprecations active; with legacy deprecations

namespace tt {
#define LEG_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(4, 3)
#define BDE_OMIT_DEPRECATED
#include "bsls_deprecate.t.cpp"
}  // close namespace tt

#undef BSLS_DEPRECATE_T_DATA_COLLECTION

}  // close namespace legacyInteractions

// ============================================================================
//                             USAGE EXAMPLE CODE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: push    // Relax some bde_verify rules
// BDE_VERIFY pragma: -DP01   // Example calls deprecated function, necessarily
// BDE_VERIFY pragma: -FABC01 // Functions in examples are in conventional, not
//                            // alpha order
// BDE_VERIFY pragma: -FD01   // Function in examples have no contract, for
//                            // brevity

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tagging a Function as Deprecated
///- - - - - - - - - - - - - - - - - - - - - -
// When one piece of code has been superceded by another, we would like to get
// users to adopt the new code and stop using the old code.  Being able to
// remind users that they need to clean up existing uses of the old code, and
// prevent new uses of that code makes it easier to get to the point where old
// code actually reaches zero uses, and can be deleted.
// 'BSLS_DEPRECATE_IS_ACTIVE' and its associated control macros can be used to
// gradually reduce the number of users of deprecated code.
//
// Suppose we own package group 'xxx' that is currently at version 7.6.  One of
// our components contains a function 'foo' that has been superceded by another
// function 'bar'.
//..
namespace example_xxx_7_6 {
    int foo(int *coefficient, int n);
        // Load into the specified 'coefficient' the (positive) Winkelbaum
        // Coefficient of the specified 'n'.  Return 0 on success, and a
        // Negative number if there is no coefficient corresponding to 'n'.
        // Note that every integer divisible by the Winkelbaum Modulus (17),
        // has a corresponding Winkelbaum Coefficient.

    // ...

    int bar(int n);
        // Return the (positive) Winkelbaum Coefficient of the specified 'n'.
        // The behavior is undefined unless 'n' is divisible by 17 (the
        // Winkelbaum Modulus).
}  // close namespace example_xxx_7_6
//..
// First, we add a deprecation tag to the declaration of 'foo', showing that it
// will be deprecated starting with version 7.7, and update the documentation
// accordingly:
//..
namespace example_xxx_7_7 {
    #if BSLS_DEPRECATE_IS_ACTIVE(XXX, 7, 7)
    BSLS_DEPRECATE
    #endif
    int foo(int *coefficient, int n);
        // !DEPRECATED!: Use 'bar' instead.
        //
        // Load into the specified 'coefficient' the (positive) Winkelbaum
        // Coefficient of the specified 'n'.  Return 0 on success, and a
        // Negative number if there is no coefficient corresponding to 'n'.
        // Note that every integer divisible by the Winkelbaum Modulus (17),
        // has a corresponding Winkelbaum Coefficient.

    // ...

    int bar(int n);
        // Return the (positive) Winkelbaum Coefficient of the specified 'n'.
        // The behavior is undefined unless 'n' is divisible by 17 (the
        // Winkelbaum Modulus).
}  // close namespace example_xxx_7_7
//..
// When we release version 7.7, the added deprecation tag will not immediately
// affect any of the users of 'foo'.  However if any of those users do a test
// build of their code with '-DBB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY', they
// will see a warning that 'foo' has been deprecated.
//
// Finally, when enough time has passed to allow the users of 'foo' to switch
// over to using 'bar', probably on or after the release of 'xxx' version 7.8,
// we can enforce the deprecation of 'foo' by moving the deprecation threshold
// for 'xxx' to version 7.7, to indicate that all interfaces deprecated for
// version 7.7 are disallowed by default:
//..
    // xxxscm_versiontag.h

    #define XXX_VERSION_MAJOR 7
    #define XXX_VERSION_MINOR 8

    // ...

    #define XXX_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(7, 7)
//..

#undef XXX_VERSION_MAJOR
#undef XXX_VERSION_MINOR
#undef XXX_VERSION_DEPRECATION_THRESHOLD

// Define alternate overloads for 'foo' and 'bar' from the usage example, so
// that we can confirm that the declarations in the example exist.  This was
// Hyman's idea.  I love sitting next to Hyman!

namespace example_xxx_7_6 {

char foo(...);
char bar(...);

}  // close namespace example_xxx_7_6

namespace example_xxx_7_7 {

char foo(...);
char bar(...);

}  // close namespace example_xxx_7_7

// BDE_VERIFY pragma: pop  // Restore bde_verify rules

// ============================================================================
//                  CONCERN: DEPRECATION ATTRIBUTE IS VALID
// ----------------------------------------------------------------------------

#if defined(BSLS_DEPRECATE)
namespace staticconcerns {

BSLS_DEPRECATE
void someDeprecatedFunction();
    // Do something that is no longer supported through this interface.

struct
BSLS_DEPRECATE
SomeDeprecatedClass
{
};

}  // close namespace staticconcerns
#else
#error "BSLS_DEPRECATE should be defined in all build configurations"
#endif

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;  // Suppress unused-varible warnings.

    fprintf(stderr, "TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate the usage example from the header into the test
        //:   driver, remove leading comment characters, and replace 'assert'
        //:   with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING USAGE EXAMPLE"
                   "\n=====================\n");
        }

        // There is no runtime component to the usage example.  We can,
        // however, confirm that the declarations that appear in the example
        // are valid.

        int c;
        ASSERT(sizeof(int) == sizeof(example_xxx_7_6::foo(&c, 0)));
        ASSERT(sizeof(int) == sizeof(example_xxx_7_6::bar(0)));
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -DP01   // Example calls deprecated function, necessarily
        ASSERT(sizeof(int) == sizeof(example_xxx_7_7::foo(&c, 0)));
// BDE_VERIFY pragma: pop
        ASSERT(sizeof(int) == sizeof(example_xxx_7_7::bar(0)));
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // INTERACTIONS WITH LEGACY MACROS
        //   Test the intended combinations of 'BSLS_DEPRECATE_IS_ACTIVE' and
        //   'BDE_OMIT_(INTERNAL_)DEPRECATED' to ensure that migration to
        //   'bsls_deprecate' of code currently deprecated via the legacy
        //   macros will have the intended effects.
        //
        ///Notes
        ///-----
        // The difficult thing about writing this test driver is that we don't
        // have a clear spec for what the interactions should be.  In the
        // deprecation proposal 'bdlex_exampleutil' component, though, we do
        // have an illustration of the approach we intend.
        //
        ///The Path of Least Code Change
        ///- - - - - - - - - - - - - - -
        // A legacy code block should go through the following progression:
        //
        // Initially, the block uses the legacy facility, and is removed from
        // the codebase when 'BDE_OMIT_DEPRECATED' is defined:
        //..
        //  #ifndef BDE_OMIT_DEPRECATED
        //      int fieldX() const;
        //  #endif
        //..
        // Then, we decide that we want to start enforcing that deprecation
        // through the new facility beginning with version 3.2.  For clients
        // who are depending on the legacy facility, we need to make sure that
        // the interface does not also suddenly become un-removed during the
        // opt-in phase.
        //..
        //  #ifndef BDE_OMIT_DEPRECATED
        //  #if BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)
        //  BSLS_DEPRECATE
        //  #endif
        //      int fieldX() const;
        //  #endif
        //..
        // Ugly, but it should work.  The nice thing is, the process is
        // entirely additive in the header.
        //
        // The cpp file requires no changes, since activation of the warning
        // does not make the function unavailable.
        //..
        //  #ifndef BDE_OMIT_DEPRECATED
        //  int MyClass::fieldX() const
        //  {
        //      return d_x;
        //  }
        //  #endif
        //..
        // Now, consider what happens with 'BDE_OMIT_INTERNAL_DEPRECATED'.
        // First, we have the same concerns that we had with
        // 'BDE_OMIT_DEPRECATED', i.e., that users of the legacy facility
        // should not see any interfaces become un-removed.  Second, we have to
        // make sure that the code is not published to opensource.  This points
        // to a design flaw that has always been present in
        // 'BDE_OMIT_INTERNAL_DEPRECATED': it is a conflation of two
        // conditions, do I want deprecated code, and do I want internal code.
        // Really, '#ifndef BDE_OMIT_INTERNAL_DEPRECATED' should have been
        // written:
        //..
        //  #if    !defined(BDE_OMIT_DEPRECATED)          \                   .
        //      && !defined(BDE_OPENSOURCE_PUBLICATION)
        //..
        // However, we can't ever build with 'BDE_OPENSOURCE_PUBLICATION'
        // defined, because there is non-deprecated code governed by that
        // macro.  So, we retain the original macro:
        //..
        //  #ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //  #if BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)
        //  BSLS_DEPRECATE
        //  #endif
        //      int fieldX() const;
        //  #endif
        //..
        // Similarly, the cpp file would look like this:
        //..
        //  #ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //  int MyClass::fieldX() const
        //  {
        //      return d_x;
        //  }
        //  #endif
        //..
        // Well, that is one solution.  It involves the least change to our
        // code, and it keeps current deprecation macro users happy.  Is there
        // another way to go about this?
        //
        ///An ABI-Compatible Approach
        /// - - - - - - - - - - - - -
        // How about this:
        //..
        //  #ifndef BDE_OPENSOURCE_PUBLICATION
        //  #if    BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)    \                   .
        //      || defined(BDE_OMIT_INTERNAL_DEPRECATED)
        //  BSLS_DEPRECATE
        //  #endif
        //      int fieldX() const;
        //  #endif  // BOP
        //
        //  // ...
        //
        //  #ifndef BDE_OPENSOURCE_PUBLICATION
        //  int MyClass::fieldX() const
        //  {
        //      return d_x;
        //  }
        //  #endif
        //..
        // Now the users of the legacy interface will get a warning instead of
        // missing code.  From an ABI-compatibility perspective, this is
        // important.  So, let's convert both the internal and regular
        // deprecation idioms to this pattern.  That way, the regular
        // deprecation pattern is simply:
        //..
        //  #if    BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)  \                     .
        //      || defined(BDE_OMIT_DEPRECATED)
        //  BSLS_DEPRECATE
        //  #endif
        //      int fieldX() const;
        //
        //  // ...
        //
        //  int MyClass::fieldX() const  // No change here!
        //  {                            //
        //      return d_x;              //
        //  }                            //
        //..
        // A test of this approach will demonstrate that removed code
        // (represented in the test driver through dfferences in overload
        // resolution) is converted into warnings.
        //
        // Concerns:
        //: 1 In idioms 1 and 3, the function may be deprecated, but is never
        //:   deleted.
        //:
        //: 2 In idioms 2 and 4, the function is deleted whenever
        //:   'BDE_OMIT_DEPRECATED' is defined.
        //:
        //: 3 In all idioms, the function is present and not deprecated if and
        //:   only if 'BSLS_DEPRECATE_IS_ACTIVE' evaluates to 0 and
        //:   'BDE_OMIT_DEPRECATED' is not defined.
        //:
        //: 4 In all idioms, the function is present and deprecated if
        //:   'BSLS_DEPRECATE_IS_ACTIVE' evaluates to 1 but
        //:   'BDE_OMIT_DEPRECATED' is not defined.
        //:
        //: 5 In idioms 1 and 3, the function is present and deprecated
        //:   whenever 'BDE_OMIT_DEPRECATED' is defined.
        //
        // Plan:
        //: 1 Create a macro 'BSLS_DEPRECATE_STAND_IN' that can be placed
        //:   syntactically in the same position relative to a function
        //:   declaration as 'BSLS_DEPRECATE', and can change the meaning of
        //:   the function in a way that can be detected at runtime.  The best
        //:   technique we have found so far is to have
        //:   'BSLS_DEPRECATE_STAND_IN' evaluate to a 'typedef' that redefines
        //:   the return type of the marked function.
        //:
        //: 2 For each idiom, create a namespace in which a candidate function
        //:   is deprecated using the idiom, except that
        //:   'BSLS_DEPRECATE_STAND_IN' is used instead of 'BSLS_DEPRECATE'.
        //:
        //: 3 Within the idiom namespace, define a test function that reports
        //:   whether or not deprecations were active for the idiom at compile
        //:   time, whether or not 'BDE_OMIT_DEPRECATED' was defined at compile
        //:   time, and whether or not the candidate function was affected by
        //:   'BSLS_DEPRECATE_STAND_IN'.
        //:
        //: 4 Repeatedly include the code described in steps 2 and 3 four
        //:   times, to cover the possible combinations of 'bsls_deprecate'
        //:   deprecations being active and inactive, and 'BDE_OMIT_DEPRECATED'
        //:   being defined and undefined.  Embed each inclusion in a namespace
        //:   that reflects the status of 'bsls_deprecate' and
        //:   'BDE_OMIT_DEPRECATED' during that inclusion.  This step will
        //:   generate 16 cases: 4 idioms x 4 configurations.
        //:
        //: 5 Using the table-based approach, call the test function for each
        //:   of the 16 cases generated in step 4, to and assert the states
        //:   that should have been in effect in each case.  (C 1-5)
        //
        // Testing:
        //   INTERACTIONS WITH LEGACY MACROS
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nINTERACTIONS WITH LEGACY MACROS"
                   "\n===============================\n");
        }

        using namespace legacyInteractions;

        struct {
            const int     d_line;
            const Checker d_source;
            const bool    d_expectedBDIAStatus;
            const bool    d_expectedBODStatus;
            const int     d_expected;
        } DATA[] = {
            //LN  SOURCE             EXP BDIA  EXP BOD  EXPECTED STATUS
            //--  ------             --------  -------  ---------------

                         // Idiom 1: fn with or'ed tag
            { L_, ff::idiom1::check, false,    false,   e_SUPPORTED   },
            { L_, tf::idiom1::check, true,     false,   e_DEPRECATED  },
            { L_, ft::idiom1::check, false,    true,    e_DEPRECATED  },
            { L_, tt::idiom1::check, true,     true,    e_DEPRECATED  },

                      // Idiom 2: fn with tag and removal
            { L_, ff::idiom2::check, false,    false,   e_SUPPORTED   },
            { L_, tf::idiom2::check, true,     false,   e_DEPRECATED  },
            { L_, ft::idiom2::check, false,    true,    e_DELETED     },
            { L_, tt::idiom2::check, true,     true,    e_DELETED     },

                       // Idiom 3: class with or'ed tag
            { L_, ff::idiom3::check, false,    false,   e_SUPPORTED   },
            { L_, tf::idiom3::check, true,     false,   e_DEPRECATED  },
            { L_, ft::idiom3::check, false,    true,    e_DEPRECATED  },
            { L_, tt::idiom3::check, true,     true,    e_DEPRECATED  },

                    // Idiom 4: class with tag and removal
            { L_, ff::idiom4::check, false,    false,   e_SUPPORTED   },
            { L_, tf::idiom4::check, true,     false,   e_DEPRECATED  },
            { L_, ft::idiom4::check, false,    true,    e_DELETED     },
            { L_, tt::idiom4::check, true,     true,    e_DELETED     },

        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE       = DATA[ti].d_line;
            const Checker SOURCE     = DATA[ti].d_source;
            const bool    EXP_BDIA   = DATA[ti].d_expectedBDIAStatus;
            const bool    EXP_BOD    = DATA[ti].d_expectedBODStatus;
            const int     EXP_STATUS = DATA[ti].d_expected;

            if (veryVerbose) {
                printf("ti: %d\tLINE: %d\tEXP_BDIA: %d\tEXP_BOD: %d\t"
                       "EXP_STATUS: %d\n",
                       ti, LINE, EXP_BDIA, EXP_BOD, EXP_STATUS);
            }

            const Results RESULT = SOURCE();
            const int     STATUS = RESULT.d_deprecationStatus;
            const bool    BDIA   = RESULT.d_isActive;
            const bool    BOD    = RESULT.d_isLegacyMacroDefined;

            LOOP3_ASSERT(LINE, BDIA,   EXP_BDIA,   EXP_BDIA   == BDIA);
            LOOP3_ASSERT(LINE, BOD,    EXP_BOD,    EXP_BOD    == BOD);
            LOOP3_ASSERT(LINE, STATUS, EXP_STATUS, EXP_STATUS == STATUS);
        }

      } break;
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TW01  // that/which filter is not smart enough

      case 10: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE' expands to the appropriate deprecation attribute
        //:   for the compiler if 'BSLS_DEPRECATE_COMPILER_SUPPORT' expands to
        //:   '1'.
        //:
        //: 2 'BSLS_DEPRECATE' expands to nothing if
        //:   'BSLS_DEPRECATE_COMPILER_SUPPORT' expands to '0'.
        //
        // Plan:
        //: 1 Using the brute force approach, check that the expansion of
        //:   'BSLS_DEPRECATE' matches one of the expected values, depending on
        //:   the expansion of 'BSLS_DEPRECATE_COMPILER_SUPPORT'.  Note that we
        //:   can't check which exact value is appropriate without duplicating
        //:   the logic used to define the macro in the first place.
        //:
        //: 2 If the current compiler and version are supported, define a
        //:   function decorated with 'BSLS_DEPRECATE', to confirm that this
        //:   compiler supports the expansion as a function attribute.
        //
        // Testing:
        //   BSLS_DEPRECATE
        // --------------------------------------------------------------------

// BDE_VERIFY pragma: pop

        if (verbose) {
            printf("\nBSLS_DEPRECATE"
                   "\n==============\n");
        }

        if (verbose) printf("\nMacro expands to a correct string.\n");
        {
            const char *expansion = MACRO_TO_STRING(BSLS_DEPRECATE);

            if (BSLS_DEPRECATE_COMPILER_SUPPORT) {
                if (   0 != strcmp(expansion, "__attribute__ ((deprecated))")
                    && 0 != strcmp(expansion, "[[deprecated]]")) {

                    LOOP_S_ASSERT(expansion,
                                  0 == strcmp(expansion,
                                              "__declspec(deprecated)"));
                }
                else if (   0 != strcmp(expansion, "__declspec(deprecated)")
                         && 0 != strcmp(expansion, "[[deprecated]]")) {

                    LOOP_S_ASSERT(expansion,
                                  0 == strcmp(expansion,
                                              "__attribute__ ((deprecated))"));
                }
                else {
                    LOOP_S_ASSERT(expansion,
                                  0 == strcmp(expansion, "[[deprecated]]"));
                }
            }
            else {
                LOOP_S_ASSERT(expansion, 0 == strcmp("", expansion));
            }
        }

        if (verbose) printf("\nMacro is valid syntax for this compiler.\n");
        {
            struct LocalTestClass {
                BSLS_DEPRECATE
                int test()
                    // Return 0.
                {
                    return 0;
                }
            };

            // Suppress not-used warnings.

            (void) sizeof(LocalTestClass);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_COMPILER_SUPPORT
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_COMPILER_SUPPORT' expands to 1 for supported
        //:   versions of supported compilers.
        //:
        //: 2 'BSLS_DEPRECATE_COMPILER_SUPPORT' expands to 0 for non-supported
        //:   versions or non-supported compilers.
        //:
        //: 3 'BSLS_DEPRECATE_COMPILER_SUPPORT' is always defined and always
        //:   numeric '[0 .. 1]'.
        //
        // Plan:
        //: 1 Using the brute force approach, compare the expansion of
        //:   'BSLS_DEPRECATE_COMPILER_SUPPORT' with the expected value,
        //:   depending on compiler and version.
        //
        // Testing:
        //   BSLS_DEPRECATE_COMPILER_SUPPORT
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_COMPILER_SUPPORT"
                   "\n===============================\n");
        }

        if (verbose) printf("\nCheck that macro is always zero or one.\n");
        {
            const char *expansion =
                              MACRO_TO_STRING(BSLS_DEPRECATE_COMPILER_SUPPORT);

            LOOP_S_ASSERT(expansion, 1 == strlen(expansion));
            LOOP_S_ASSERT(expansion, '0' <= expansion[0]       );
            LOOP_S_ASSERT(expansion,        expansion[0] <= '1');
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_IS_ACTIVE
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_IS_ACTIVE(U, M, N)' (abbreviated as 'BSIA' below)
        //:   expands to 0 or 1 according to the formula
        //:   '(IPTumn || BWADFTO) && ! (BBUu || BSDumn)' where:
        //:
        //:   o 'IPTumn' is 1 if the deprecation threshold for the UOR has
        //:     reached the current version of the UOR, and 0 otherwise.  The
        //:     deprecation threshold can be set either explicitly by
        //:     '#define'ing '<UOR>_VERSION_DEPRECATION_THRESHOLD', or
        //:     implicitly by '#define'ing '<UOR>_VERSION_(MAJOR|MINOR)'.  If
        //:     none of these macros have been '#defined', then there is no
        //:     threshold, and 'IPTumn' is 0.
        //:
        //:   o 'BWADFTO' is 1 if 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY'
        //:     is '#define'd to nil, '0', or '1', and is 0 otherwise.
        //:
        //:   o 'BBUu' is 1 if 'BB_BUILDING_UOR_<UOR>' is '#define'd to
        //:     nil, '0', or '1', and is 0 otherwise.
        //:
        //:   o 'BSDumn' is 1 if 'BB_SILENCE_DEPRECATION_<UOR>_<M>_<N>' is
        //:     '#define'd to nil, '0', or '1', and is 0 otherwise.
        //:
        //: 2 The formula above holds when the 0 states of the control macros
        //:   are, variously:
        //:
        //:   o 0: never defined at all
        //:
        //:   o 0: '#undef'ined
        //:
        //:   o 0: '#define'd as its own symbolic name
        //:
        //:   o 0: '#define'd as some other alphanumeric symbol that eventually
        //:        fails to expand
        //:
        //: 3 The value of 'BSLS_DEPRECATE_IS_ACTIVE(U, M, N)' is not affected
        //:   by whether or not 'BDE_OMIT_DEPRECATED' and/or
        //:   'BDE_OMIT_INTERNAL_DEPRECATED' are defined.
        //
        // Plan:
        //: 1 The formula in (C-1) can be expressed as the following table:
        //..
        //           BBUu     BSDumn   IPTumn   BWADFTO    BSIAumn
        //         +========+========+========+=========++=========+
        //         |      0 |     0  |      0 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     0  |      0 |      1  ||      1  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     0  |      1 |      0  ||      1  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     0  |      1 |      1  ||      1  |
        //         +========+========+========+=========++=========+
        //         |      0 |     1  |      0 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     1  |      0 |      1  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     1  |      1 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      0 |     1  |      1 |      1  ||      0  |
        //         +========+========+========+=========++=========+
        //         |      1 |     0  |      0 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     0  |      0 |      1  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     0  |      1 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     0  |      1 |      1  ||      0  |
        //         +========+========+========+=========++=========+
        //         |      1 |     1  |      0 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     1  |      0 |      1  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     1  |      1 |      0  ||      0  |
        //         +--------+--------+--------+---------++---------+
        //         |      1 |     1  |      1 |      1  ||      0  |
        //         +========+========+========+=========++=========+
        //..
        //:   where the 'IPTumn' column depends on setting the deprecation
        //:   threshold in any of a number of ways:
        //..
        //           uVDT            BDMV(uVm,uVn) - 1    IPTumn
        //         +==============+====================++========+
        //         | undef        | undef              ||      0 |
        //         +==============+====================++========+
        //         | undef        | <  BDMV(m,n)       ||      0 |
        //         +--------------+--------------------++--------+
        //         | undef        | == BDMV(m,n)       ||      1 |
        //         +--------------+--------------------++--------+
        //         | undef        |  > BDMV(m,n)       ||      1 |
        //         +==============+====================++========+
        //         | <  BDMV(m,n) | undef              ||      0 |
        //         +--------------+--------------------++--------+
        //         | == BDMV(m,n) | undef              ||      1 |
        //         +--------------+--------------------++--------+
        //         |  > BDMV(m,n) | undef              ||      1 |
        //         +==============+====================++========+
        //         | <  BDMV(m,n) | <=>  BDMV(m,n)     ||      0 |
        //         +--------------+--------------------++--------+
        //         | == BDMV(m,n) | <=>  BDMV(m,n)     ||      1 |
        //         +--------------+--------------------++--------+
        //         |  > BDMV(m,n) | <=>  BDMV(m,n)     ||      1 |
        //         +==============+====================++========+
        //..
        //: 2 Define control macros for a set of fictional UORS, covering all
        //:   of the relationships described in the tables shown step 1.  (See
        //:   the "GLOBAL DEPRECATION CONTROL MACRO SETTINGS" section of this
        //:   test driver.)
        //:
        //: 3 Successively evaluate 'BSLS_DEPRECATE_IS_ACTIVE' for each UOR
        //:   configured in step 2, with versions less than, equal to, and
        //:   greater than the expected threshold for that UOR.
        //:
        //:   1 Store the observed value of 'BSLS_DEPRECATE_IS_ACTIVE' in an
        //:     array.
        //:
        //:   2 For each evaluation of 'BSLS_DEPRECATE_IS_ACTIVE', store in
        //:     arrays the value of each of the control macros at that point in
        //:     the code.  An unset threshold value, UOR major version number
        //:     or UOR minor version number is represented by '-1'.
        //:
        //:   (This step in the plan is performed by code generated by
        //:   '#define'ing 'BSLS_DEPRECATE_T_DATA_COLLECTION' to be '1',
        //:   setting an appropriate 'COLLECT_CODE', and repeatedly
        //:   '#include'ing the test driver file to pre-process part 1 of the
        //:   "DATA COLLECTION" section.)
        //:
        //: 3 Using the table-based approach, for each UOR configured in step
        //:   2, compare the value of 'BSLS_DEPRECATE_IS_ACTIVE(U, M, N)' with
        //:   the expected value, derived according to the tables described in
        //:   (C-1). Also compare the values of each of the control macros with
        //:   the expected value for that scenario, to check that the UOR is
        //:   correctly configured.  (C-1)
        //
        // Testing:
        //   BSLS_DEPRECATE_IS_ACTIVE(U, M, N)
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_IS_ACTIVE"
                   "\n========================\n");
        }

        using namespace defaultData;

        collectData();

        static const struct {
            int d_line;

            // Test Case
            UORS d_uor;
            int  d_major;
            int  d_minor;

            // Expected Value
            int  d_active;

            // Configuration Checks
            int  d_warn;
            int  d_silenced;
            int  d_building;
            int  d_thresh;
            int  d_uorMajor;
            int  d_uorMinor;
        } DATA[] = {
            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // No control macros
            {L_, AAA,  1,  2, e_NO,    e_NO,  e_NO,  e_NO,    -1,  -1, -1 },

            // Explicit threshold

            // SILENCE boundaries
            {L_, BBB,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBB,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBB,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,  -1, -1 },

            {L_, BBC,  4,  1, e_NO,    e_NO,  e_YES, e_NO,  4002,  -1, -1 },
            {L_, BBC,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBC,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,  -1, -1 },

            {L_, BBD,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBD,  4,  2, e_NO,    e_NO,  e_YES, e_NO,  4002,  -1, -1 },
            {L_, BBD,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,  -1, -1 },

            {L_, BBE,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBE,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,  -1, -1 },
            {L_, BBE,  4,  3, e_NO,    e_NO,  e_YES, e_NO,  4002,  -1, -1 },

            // BUILDING
            {L_, BBF,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },
            {L_, BBF,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },
            {L_, BBF,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },

            {L_, BBG,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },
            {L_, BBG,  4,  2, e_NO,    e_NO,  e_YES, e_YES, 4002,  -1, -1 },
            {L_, BBG,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },

            {L_, BBH,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },
            {L_, BBH,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,  -1, -1 },
            {L_, BBH,  4,  3, e_NO,    e_NO,  e_YES, e_YES, 4002,  -1, -1 },

            // Default threshold

            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // SILENCE boundaries
            {L_, CCB,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCB,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCB,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,    -1,   4,  3 },

            {L_, CCC,  4,  1, e_NO,    e_NO,  e_YES, e_NO,    -1,   4,  3 },
            {L_, CCC,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCC,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,    -1,   4,  3 },

            {L_, CCD,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCD,  4,  2, e_NO,    e_NO,  e_YES, e_NO,    -1,   4,  3 },
            {L_, CCD,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,    -1,   4,  3 },

            {L_, CCE,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCE,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,    -1,   4,  3 },
            {L_, CCE,  4,  3, e_NO,    e_NO,  e_YES, e_NO,    -1,   4,  3 },

            // BUILDING
            {L_, CCF,  4,  1, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },
            {L_, CCF,  4,  2, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },
            {L_, CCF,  4,  3, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },

            {L_, CCG,  4,  1, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },
            {L_, CCG,  4,  2, e_NO,    e_NO,  e_YES, e_YES,   -1,   4,  3 },
            {L_, CCG,  4,  3, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },

            {L_, CCH,  4,  1, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },
            {L_, CCH,  4,  2, e_NO,    e_NO,  e_NO,  e_YES,   -1,   4,  3 },
            {L_, CCH,  4,  3, e_NO,    e_NO,  e_YES, e_YES,   -1,   4,  3 },

            // Mixed threshold A

            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // SILENCE boundaries
            {L_, DDB,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDB,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDB,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  5 },

            {L_, DDC,  4,  1, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  5 },
            {L_, DDC,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDC,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  5 },

            {L_, DDD,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDD,  4,  2, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  5 },
            {L_, DDD,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  5 },

            {L_, DDE,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDE,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  5 },
            {L_, DDE,  4,  3, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  5 },

            // BUILDING
            {L_, DDF,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },
            {L_, DDF,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },
            {L_, DDF,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },

            {L_, DDG,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },
            {L_, DDG,  4,  2, e_NO,    e_NO,  e_YES, e_YES, 4002,   4,  5 },
            {L_, DDG,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },

            {L_, DDH,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },
            {L_, DDH,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  5 },
            {L_, DDH,  4,  3, e_NO,    e_NO,  e_YES, e_YES, 4002,   4,  5 },

            // Mixed threshold B

            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // SILENCE boundaries
            {L_, EEB,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EEB,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EEB,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  2 },

            {L_, EEC,  4,  1, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  2 },
            {L_, EEC,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EEC,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  2 },

            {L_, EED,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EED,  4,  2, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  2 },
            {L_, EED,  4,  3, e_NO,    e_NO,  e_NO,  e_NO,  4002,   4,  2 },

            {L_, EEE,  4,  1, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EEE,  4,  2, e_YES,   e_NO,  e_NO,  e_NO,  4002,   4,  2 },
            {L_, EEE,  4,  3, e_NO,    e_NO,  e_YES, e_NO,  4002,   4,  2 },

            // BUILDING
            {L_, EEF,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },
            {L_, EEF,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },
            {L_, EEF,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },

            {L_, EEG,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },
            {L_, EEG,  4,  2, e_NO,    e_NO,  e_YES, e_YES, 4002,   4,  2 },
            {L_, EEG,  4,  3, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },

            {L_, EEH,  4,  1, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },
            {L_, EEH,  4,  2, e_NO,    e_NO,  e_NO,  e_YES, 4002,   4,  2 },
            {L_, EEH,  4,  3, e_NO,    e_NO,  e_YES, e_YES, 4002,   4,  2 },

            // Warn

            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // SILENCE boundaries
            {L_, FFB,  3,  1, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFB,  3,  2, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFB,  3,  3, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },

            {L_, FFC,  3,  1, e_NO,    e_YES, e_YES, e_NO,    -1,  -1, -1 },
            {L_, FFC,  3,  2, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFC,  3,  3, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },

            {L_, FFD,  3,  1, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFD,  3,  2, e_NO,    e_YES, e_YES, e_NO,    -1,  -1, -1 },
            {L_, FFD,  3,  3, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },

            {L_, FFE,  3,  1, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFE,  3,  2, e_YES,   e_YES, e_NO,  e_NO,    -1,  -1, -1 },
            {L_, FFE,  3,  3, e_NO,    e_YES, e_YES, e_NO,    -1,  -1, -1 },

            // BUILDING
            {L_, FFF,  3,  1, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },
            {L_, FFF,  3,  2, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },
            {L_, FFF,  3,  3, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },

            {L_, FFG,  3,  1, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },
            {L_, FFG,  3,  2, e_NO,    e_YES, e_YES, e_YES,   -1,  -1, -1 },
            {L_, FFG,  3,  3, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },

            {L_, FFH,  3,  1, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },
            {L_, FFH,  3,  2, e_NO,    e_YES, e_NO,  e_YES,   -1,  -1, -1 },
            {L_, FFH,  3,  3, e_NO,    e_YES, e_YES, e_YES,   -1,  -1, -1 },

            // Default threshold for x.0 version

            //LN UOR  MJ  MN  EXPECTED WARN  SILENCE BUILD THRESH MAJ  MIN
            //-- ---  --  --  -------- ----  ------- ----- ------ ---  ---
            // SILENCE boundaries
            {L_, GGB,  1,998, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGB,  1,999, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGB,  2,  0, e_NO,    e_NO,  e_NO,  e_NO,    -1,   2,  0 },

            {L_, GGC,  1,998, e_NO,    e_NO,  e_YES, e_NO,    -1,   2,  0 },
            {L_, GGC,  1,999, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGC,  2,  0, e_NO,    e_NO,  e_NO,  e_NO,    -1,   2,  0 },

            {L_, GGD,  1,998, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGD,  1,999, e_NO,    e_NO,  e_YES, e_NO,    -1,   2,  0 },
            {L_, GGD,  2,  0, e_NO,    e_NO,  e_NO,  e_NO,    -1,   2,  0 },

            {L_, GGE,  1,998, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGE,  1,999, e_YES,   e_NO,  e_NO,  e_NO,    -1,   2,  0 },
            {L_, GGE,  2,  0, e_NO,    e_NO,  e_YES, e_NO,    -1,   2,  0 },

            // BUILDING
            {L_, GGF,  1,998, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },
            {L_, GGF,  1,999, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },
            {L_, GGF,  2,  0, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },

            {L_, GGG,  1,998, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },
            {L_, GGG,  1,999, e_NO,    e_NO,  e_YES, e_YES,   -1,   2,  0 },
            {L_, GGG,  2,  0, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },

            {L_, GGH,  1,998, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },
            {L_, GGH,  1,999, e_NO,    e_NO,  e_NO,  e_YES,   -1,   2,  0 },
            {L_, GGH,  2,  0, e_NO,    e_NO,  e_YES, e_YES,   -1,   2,  0 },

            // Other
            {L_, RST,  7,  7, e_YES,   e_NO,  e_NO,  e_NO,  7009,  -1, -1 },
            {L_, RST,  7,  8, e_NO,    e_NO,  e_YES, e_NO,  7009,  -1, -1 },
            {L_, RST,  7,  9, e_YES,   e_NO,  e_NO,  e_NO,  7009,  -1, -1 },

            {L_, UVW,  4,  7, e_NO,    e_NO,  e_NO,  e_YES, 4005,  -1, -1 },

            // Test apparatus sanity check: un-collected UORs will not match
            // any valid expected results.
            {L_, ZZZ,  0,  0, e_UNKNOWN,
                                       e_UNKNOWN,
                                              e_UNKNOWN,
                                                     e_UNKNOWN,
                                                               0,
                                                                    0,
                                                                     INT_MIN },
            {L_, ZZZ,  3,  3, e_UNKNOWN,
                                       e_UNKNOWN,
                                              e_UNKNOWN,
                                                     e_UNKNOWN,
                                                               0,
                                                                    0,
                                                                     INT_MIN },
            {L_, ZZZ,  7,999, e_UNKNOWN,
                                       e_UNKNOWN,
                                              e_UNKNOWN,
                                                     e_UNKNOWN,
                                                               0,
                                                                    0,
                                                                     INT_MIN },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            int LINE         = DATA[ti].d_line;
            int UOR          = DATA[ti].d_uor;
            int MAJOR        = DATA[ti].d_major;
            int MINOR        = DATA[ti].d_minor;
            int EXPECTED     = DATA[ti].d_active;
            int EXP_WARN     = DATA[ti].d_warn;
            int EXP_SILENCED = DATA[ti].d_silenced;
            int EXP_BUILDING = DATA[ti].d_building;
            int EXP_THRESH   = DATA[ti].d_thresh;
            int EXP_MAJOR    = DATA[ti].d_uorMajor;
            int EXP_MINOR    = DATA[ti].d_uorMinor;

            if (veryVerbose) {
                T_ printf("ti: %d\tLINE: %d\tUOR: %d\tM: %d\tN: %d\n",
                           ti,     LINE,     UOR,     MAJOR,  MINOR);
            }

            LOOP3_ASSERT(LINE,
                         EXPECTED, expectedActive[UOR][MAJOR][MINOR % 20],
                         EXPECTED == expectedActive[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_WARN,
                         isWarnDefined[UOR][MAJOR][MINOR % 20],
                         EXP_WARN == isWarnDefined[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_SILENCED,
                         isSilenceDefined[UOR][MAJOR][MINOR % 20],
                         EXP_SILENCED ==
                                     isSilenceDefined[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_BUILDING,
                         isBuildingDefined[UOR][MAJOR][MINOR % 20],
                         EXP_BUILDING ==
                                    isBuildingDefined[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_THRESH,
                         threshold[UOR][MAJOR][MINOR % 20],
                         EXP_THRESH == threshold[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MAJOR,
                         majorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MAJOR == majorVersion[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MINOR,
                         minorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MINOR == minorVersion[UOR][MAJOR][MINOR % 20]);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_ISPASTTHRESHOLD
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_ISPASTTHRESHOLD(U, M, N)' (abbreviated as
        //:   'IPTumn' below) is 1 if the deprecation threshold for the UOR has
        //:   reached the current version of the UOR, and 0 otherwise.  The
        //:   deprecation threshold can be set either explicitly by
        //:   '#define'ing '<UOR>_VERSION_DEPRECATION_THRESHOLD', or implicitly
        //:   by '#define'ing '<UOR>_VERSION_(MAJOR|MINOR)'.  If none of these
        //:   macros have been '#defined', then there is no threshold, and
        //:   'IPTumn' is 0.
        //:
        //: 2 The formula above holds when the 0 states of the control macros
        //:   are, variously:
        //:
        //:   o 0: never defined at all
        //:
        //:   o 0: '#undef'ined
        //:
        //:   o 0: '#define'd as its own symbolic name
        //:
        //:   o 0: '#define'd as some other alphanumeric symbol that eventually
        //:        fails to expand
        //:
        //
        // Plan:
        //: 1 The formula in (C-1) can be expressed as the following table:
        //..
        //           uVDT           BDMV(uVm,uVn) - 1     IPTumn
        //         +==============+====================++========+
        //         | undef        | undef              ||      0 |
        //         +==============+====================++========+
        //         | undef        | <  BDMV(m,n)       ||      0 |
        //         +--------------+--------------------++--------+
        //         | undef        | == BDMV(m,n)       ||      1 |
        //         +--------------+--------------------++--------+
        //         | undef        |  > BDMV(m,n)       ||      1 |
        //         +==============+====================++========+
        //         | <  BDMV(m,n) | undef              ||      0 |
        //         +--------------+--------------------++--------+
        //         | == BDMV(m,n) | undef              ||      1 |
        //         +--------------+--------------------++--------+
        //         |  > BDMV(m,n) | undef              ||      1 |
        //         +==============+====================++========+
        //         | <  BDMV(m,n) | <=>  BDMV(m,n)     ||      0 |
        //         +--------------+--------------------++--------+
        //         | == BDMV(m,n) | <=>  BDMV(m,n)     ||      1 |
        //         +--------------+--------------------++--------+
        //         |  > BDMV(m,n) | <=>  BDMV(m,n)     ||      1 |
        //         +==============+====================++========+
        //..
        //: 2 Define control macros for a set of fictional UORS, covering all
        //:   of the relationships described in the table shown step 1.  (See
        //:   the "GLOBAL DEPRECATION CONTROL MACRO SETTINGS" section of this
        //:   test driver, which forms a superset of the relationships required
        //:   by this test case.)
        //:
        //: 3 Successively evaluate 'BSLS_DEPRECATE_ISPASTTHRESHOLD' for each
        //:   UOR configured in step 2, with versions less than, equal to, and
        //:   greater than the expected threshold for that UOR.
        //:
        //:   1 Store the observed value of 'BSLS_DEPRECATE_ISPASTTHRESHOLD' in
        //:     an array.
        //:
        //:   2 For each evaluation of 'BSLS_DEPRECATE_ISPASTTHRESHOLD', store
        //:     in arrays the value of each of the control macros at that point
        //:     in the code.  An unset threshold value, UOR major version
        //:     number or UOR minor version number is represented by '-1'.
        //:
        //:   (This step in the plan is performed by code generated by
        //:   '#define'ing 'BSLS_DEPRECATE_T_DATA_COLLECTION' to be '1',
        //:   setting an appropriate 'COLLECT_CODE', and repeatedly
        //:   '#include'ing the test driver file to pre-process part 1 of the
        //:   "DATA COLLECTION" section.)
        //:
        //: 3 Using the table-based approach, for each UOR configured in step
        //:   2, compare the value of 'BSLS_DEPRECATE_ISPASTTHRESHOLD(U, M, N)'
        //:   with the expected value, derived according to the tables
        //:   described in (C-1). Also compare the values of each of the
        //:   control macros with the expected value for that scenario, to
        //:   check that the UOR is correctly configured.  (C-1)
        //
        // Testing:
        //   BSLS_DEPRECATE_ISPASTTHRESHOLD
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_ISPASTTHRESHOLD"
                   "\n==============================\n");
        }

        using namespace defaultData;

        collectData();

        static const struct {
            int d_line;

            // Test Case
            UORS d_uor;
            int  d_major;
            int  d_minor;

            // Expected Value
            int  d_isPastThreshold;

            // Configuration Checks
            int  d_thresh;
            int  d_uorMajor;
            int  d_uorMinor;
        } DATA[] = {
            //LN UOR  MJ  MN  EXPECTED THRESH MAJ  MIN
            //-- ---  --  --  -------- ------ ---  ---
            // No control macros
            {L_, AAA,  1,  2, e_NO,       -1,  -1, -1 },

            // Explicit threshold
            {L_, BBB,  4,  1, e_YES,    4002,  -1, -1 },
            {L_, BBB,  4,  2, e_YES,    4002,  -1, -1 },
            {L_, BBB,  4,  3, e_NO,     4002,  -1, -1 },

            // Default threshold
            {L_, CCB,  4,  1, e_YES,      -1,   4,  3 },
            {L_, CCB,  4,  2, e_YES,      -1,   4,  3 },
            {L_, CCB,  4,  3, e_NO,       -1,   4,  3 },

            // Mixed threshold A
            {L_, DDB,  4,  1, e_YES,    4002,   4,  5 },
            {L_, DDB,  4,  2, e_YES,    4002,   4,  5 },
            {L_, DDB,  4,  3, e_NO,     4002,   4,  5 },

            // Mixed threshold B
            {L_, EEB,  4,  1, e_YES,    4002,   4,  2 },
            {L_, EEB,  4,  2, e_YES,    4002,   4,  2 },
            {L_, EEB,  4,  3, e_NO,     4002,   4,  2 },

            // Default threshold for x.0 version
            {L_, GGB,  1,998, e_YES,      -1,   2,  0 },
            {L_, GGB,  1,999, e_YES,      -1,   2,  0 },
            {L_, GGB,  2,  0, e_NO,       -1,   2,  0 },

            // Other
            {L_, RST,  7,  7, e_YES,    7009,  -1, -1 },
            {L_, RST,  7,  8, e_YES,    7009,  -1, -1 },
            {L_, RST,  7,  9, e_YES,    7009,  -1, -1 },

            {L_, UVW,  4,  7, e_NO,     4005,  -1, -1 },

            // Test apparatus sanity check: un-collected UORs will not match
            // any valid expected results.
            {L_, ZZZ,  0,  0, e_UNKNOWN,   0,   0, INT_MIN },
            {L_, ZZZ,  3,  3, e_UNKNOWN,   0,   0, INT_MIN },
            {L_, ZZZ,  7,999, e_UNKNOWN,   0,   0, INT_MIN },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            int LINE         = DATA[ti].d_line;
            int UOR          = DATA[ti].d_uor;
            int MAJOR        = DATA[ti].d_major;
            int MINOR        = DATA[ti].d_minor;
            int EXPECTED     = DATA[ti].d_isPastThreshold;
            int EXP_THRESH   = DATA[ti].d_thresh;
            int EXP_MAJOR    = DATA[ti].d_uorMajor;
            int EXP_MINOR    = DATA[ti].d_uorMinor;

            if (veryVerbose) {
                T_ printf("ti: %d\tLINE: %d\tUOR: %d\tM: %d\tN: %d\n",
                           ti,     LINE,     UOR,     MAJOR,  MINOR);
            }

            LOOP3_ASSERT(LINE,
                         EXPECTED,
                         expectedIsPast[UOR][MAJOR][MINOR % 20],
                         EXPECTED == expectedIsPast[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_THRESH,
                         threshold[UOR][MAJOR][MINOR % 20],
                         EXP_THRESH == threshold[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MAJOR,
                         majorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MAJOR == majorVersion[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MINOR,
                         minorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MINOR == minorVersion[UOR][MAJOR][MINOR % 20]);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_ISRETAINED
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_ISRETAINED(U, M, N)' (abbreviated as 'IRumn'
        //:   below) expands to 0 or 1 according to the formula '(BBUu ||
        //:   BSDumn)' where:
        //:
        //:   o 'BBUu' is 1 if 'BB_BUILDING_UOR_<UOR>' is '#define'd to
        //:     nil, '0', or '1', and is 0 otherwise.
        //:
        //:   o 'BSDumn' is 1 if 'BB_SILENCE_DEPRECATION_<UOR>_<M>_<N>' is
        //:     '#define'd to nil, '0', or '1', and is 0 otherwise.
        //:
        //:
        //: 2 The formula above holds when the 0 states of the control macros
        //:   are, variously:
        //:
        //:   o 0: never defined at all
        //:
        //:   o 0: '#undef'ined
        //:
        //:   o 0: '#define'd as its own symbolic name
        //:
        //:   o 0: '#define'd as some other alphanumeric symbol that eventually
        //:        fails to expand
        //:
        //
        // Plan:
        //: 1 The formula in (C-1) can be expressed as the following table:
        //..
        //           BBUu     BSDumn    IRumn
        //         +========+========++========+
        //         |      0 |     0  ||     0  |
        //         +--------+--------++--------+
        //         |      0 |     1  ||     1  |
        //         +--------+--------++--------+
        //         |      1 |     0  ||     1  |
        //         +--------+--------++--------+
        //         |      1 |     1  ||     1  |
        //         +========+========++========+
        //..
        //: 2 Define control macros for a set of fictional UORS, covering all
        //:   of the relationships described in the table shown step 1.  For
        //:   each value of 'BSDumn', also include UORs where that value
        //:   appears for the previous or subsequent version instead.  (See the
        //:   "GLOBAL DEPRECATION CONTROL MACRO SETTINGS" section of this test
        //:   driver, which forms a superset of the relationships required by
        //:   this test case.)
        //:
        //: 3 Successively evaluate 'BSLS_DEPRECATE_ISRETAINED' for each UOR
        //:   configured in step 2, with versions less than, equal to, and
        //:   greater than the deprecation threshold designated for the UOR.
        //:
        //:   1 Store the observed value of 'BSLS_DEPRECATE_ISRETAINED' in an
        //:     array.
        //:
        //:   2 For each evaluation of 'BSLS_DEPRECATE_ISRETAINED', store in
        //:     arrays the value of each of the control macros at that point in
        //:     the code.  An unset threshold value, UOR major version number
        //:     or UOR minor version number is represented by '-1'.
        //:
        //:   (This step in the plan is performed by code generated by
        //:   '#define'ing 'BSLS_DEPRECATE_T_DATA_COLLECTION' to be '1',
        //:   setting an appropriate 'COLLECT_CODE', and repeatedly
        //:   '#include'ing the test driver file to pre-process part 1 of the
        //:   "DATA COLLECTION" section.)
        //:
        //: 3 Using the table-based approach, for each UOR configured in step
        //:   2, compare the value of 'BSLS_DEPRECATE_ISRETAINED(U, M, N)' with
        //:   the expected value, derived according to the tables described in
        //:   (C-1). Also compare the values of each of the control macros with
        //:   the expected value for that scenario, to check that the UOR is
        //:   correctly configured.  (C-1)
        //
        // Testing:
        //   BSLS_DEPRECATE_ISRETAINED
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_ISRETAINED"
                   "\n=========================\n");
        }

        using namespace defaultData;

        collectData();

        static const struct {
            int d_line;

            // Test Case
            UORS d_uor;
            int  d_major;
            int  d_minor;

            // Expected Value
            int  d_retained;

            // Configuration Checks
            int  d_silenced;
            int  d_building;
            int  d_uorMajor;
            int  d_uorMinor;
        } DATA[] = {
            //LN UOR  MJ  MN  EXPECTED SILENCE BUILD MAJ  MIN
            //-- ---  --  --  -------- ------- ----- ---  ---
            // No control macros
            {L_, AAA,  1,  2, e_NO,     e_NO,  e_NO,  -1, -1 },

            // SILENCE boundaries
            {L_, BBB,  4,  1, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBB,  4,  2, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBB,  4,  3, e_NO,     e_NO,  e_NO,  -1, -1 },

            {L_, BBC,  4,  1, e_YES,    e_YES, e_NO,  -1, -1 },
            {L_, BBC,  4,  2, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBC,  4,  3, e_NO,     e_NO,  e_NO,  -1, -1 },

            {L_, BBD,  4,  1, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBD,  4,  2, e_YES,    e_YES, e_NO,  -1, -1 },
            {L_, BBD,  4,  3, e_NO,     e_NO,  e_NO,  -1, -1 },

            {L_, BBE,  4,  1, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBE,  4,  2, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, BBE,  4,  3, e_YES,    e_YES, e_NO,  -1, -1 },

            // BUILDING
            {L_, BBF,  4,  1, e_YES,    e_NO,  e_YES, -1, -1 },
            {L_, BBF,  4,  2, e_YES,    e_NO,  e_YES, -1, -1 },
            {L_, BBF,  4,  3, e_YES,    e_NO,  e_YES, -1, -1 },

            {L_, BBG,  4,  1, e_YES,    e_NO,  e_YES, -1, -1 },
            {L_, BBG,  4,  2, e_YES,    e_YES, e_YES, -1, -1 },
            {L_, BBG,  4,  3, e_YES,    e_NO,  e_YES, -1, -1 },

            {L_, BBH,  4,  1, e_YES,    e_NO,  e_YES, -1, -1 },
            {L_, BBH,  4,  2, e_YES,    e_NO,  e_YES, -1, -1 },
            {L_, BBH,  4,  3, e_YES,    e_YES, e_YES, -1, -1 },

            // Other
            {L_, RST,  7,  7, e_NO,     e_NO,  e_NO,  -1, -1 },
            {L_, RST,  7,  8, e_YES,    e_YES, e_NO,  -1, -1 },
            {L_, RST,  7,  9, e_NO,     e_NO,  e_NO,  -1, -1 },

            {L_, UVW,  4,  7, e_YES,    e_NO,  e_YES, -1, -1 },

            // Test apparatus sanity check: un-collected UORs will not match
            // any valid expected results.
            {L_, ZZZ,  0,  0, e_UNKNOWN,
                                        e_UNKNOWN,
                                               e_UNKNOWN,
                                                      0,
                                                          INT_MIN },
            {L_, ZZZ,  3,  3, e_UNKNOWN,
                                        e_UNKNOWN,
                                               e_UNKNOWN,
                                                      0,
                                                          INT_MIN },
            {L_, ZZZ,  7,999, e_UNKNOWN,
                                        e_UNKNOWN,
                                               e_UNKNOWN,
                                                      0,
                                                          INT_MIN },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            int LINE         = DATA[ti].d_line;
            int UOR          = DATA[ti].d_uor;
            int MAJOR        = DATA[ti].d_major;
            int MINOR        = DATA[ti].d_minor;
            int EXPECTED     = DATA[ti].d_retained;
            int EXP_SILENCED = DATA[ti].d_silenced;
            int EXP_BUILDING = DATA[ti].d_building;
            int EXP_MAJOR    = DATA[ti].d_uorMajor;
            int EXP_MINOR    = DATA[ti].d_uorMinor;

            if (veryVerbose) {
                T_ printf("ti: %d\tLINE: %d\tUOR: %d\tM: %d\tN: %d\n",
                           ti,     LINE,     UOR,     MAJOR,  MINOR);
            }

            LOOP3_ASSERT(LINE,
                         EXPECTED,
                         expectedRetained[UOR][MAJOR][MINOR % 20],
                         EXPECTED == expectedRetained[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_SILENCED,
                         isSilenceDefined[UOR][MAJOR][MINOR % 20],
                         EXP_SILENCED ==
                                     isSilenceDefined[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_BUILDING,
                         isBuildingDefined[UOR][MAJOR][MINOR % 20],
                         EXP_BUILDING ==
                                    isBuildingDefined[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MAJOR,
                         majorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MAJOR == majorVersion[UOR][MAJOR][MINOR % 20]);
            LOOP3_ASSERT(LINE,
                         EXP_MINOR,
                         minorVersion[UOR][MAJOR][MINOR % 20],
                         EXP_MINOR == minorVersion[UOR][MAJOR][MINOR % 20]);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_MAKE_VER
        //
        // Concerns:
        //: 1 Expansion of 'BSLS_DEPRECATE_MAKE_VER' is a numeric expression.
        //:
        //: 2 'BSLS_DEPRECATE_MAKE_VER' supports major versions from 0 to
        //:   'INT_MAX/1000'.
        //:
        //: 3 'BSLS_DEPRECATE_MAKE_VER' supports minor versions from 0 to 999.
        //:
        //: 4 'BSLS_DEPRECATE_MAKE_VER' puts the major version in the thousands
        //:   place, and the minor version in the units place.
        //
        // Plan:
        //: 1 Confirm that when the expansion of 'BSLS_DEPRECATE_MAKE_VER' is
        //:   passed to an overloaded function, the overload chosen is the one
        //:   corresponding to an 'int' literal.
        //:
        //: 2 Using the brute force approach, compare expansions of
        //:   'BSLS_DEPRECATE_MAKE_VER' to expected values.
        //
        // Testing:
        //   BSLS_DEPRECATE_MAKE_VER(M, N)
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_MAKE_VER"
                   "\n=======================\n");
        }

        if (verbose) printf("\nExpansion is numeric.\n");
        {
            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(0, 0)),
                          Case5::e_INTEGER ==
                                   Case5::test(BSLS_DEPRECATE_MAKE_VER(0, 0)));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(1, 0)),
                          Case5::e_INTEGER ==
                                   Case5::test(BSLS_DEPRECATE_MAKE_VER(1, 0)));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(0, 1)),
                          Case5::e_INTEGER ==
                                   Case5::test(BSLS_DEPRECATE_MAKE_VER(0, 1)));
        }

        if (verbose) printf("\nCompare with expected values.\n");
        {
            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(0, 0)),
                          0 == BSLS_DEPRECATE_MAKE_VER(0, 0));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(0, 1)),
                          1 == BSLS_DEPRECATE_MAKE_VER(0, 1));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(1, 0)),
                          1000 == BSLS_DEPRECATE_MAKE_VER(1, 0));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(1, 1)),
                          1001 == BSLS_DEPRECATE_MAKE_VER(1, 1));

            LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(0, 999)),
                          999 == BSLS_DEPRECATE_MAKE_VER(0, 999));

            LOOP_S_ASSERT(
                     MACRO_TO_STRING(BSLS_DEPRECATE_MAKE_VER(INT_MAX/1000, 0)),
                     (INT_MAX - (INT_MAX % 1000)) ==
                                     BSLS_DEPRECATE_MAKE_VER(INT_MAX/1000, 0));

            LOOP_S_ASSERT(
                MACRO_TO_STRING(
                      BSLS_DEPRECATE_MAKE_VER(INT_MAX / 1000, INT_MAX % 1000)),
                INT_MAX ==
                      BSLS_DEPRECATE_MAKE_VER(INT_MAX / 1000, INT_MAX % 1000));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // OTHER SUPPORT MACROS
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_ISDEFINED(S)' expands to 0 when 'S' is a single
        //:   '#undef'ined preprocessor symbol.
        //:
        //: 2 'BSLS_DEPRECATE_ISDEFINED(S)' expands to 1 when 'S' is any other
        //:   valid input, specifically:
        //:   1 A single preprocessor symbol '#define'd as nil.
        //:   2 A single preprocessor symbol '#define'd as 0.
        //:   3 A single preprocessor symbol '#define'd as 1.
        //:
        //: 3 QoI: 'BSLS_DEPRECATE_ISDEFINED(S)' expands to 0 when 'S' is other
        //:   invalid inputs, such as:
        //:   1 A single preprocessor symbol '#define'd to a non-arithmetic
        //:     value.
        //:
        //: 4 QoI: 'BSLS_DEPRECATE_ISDEFINED(S)' expands to 1 when 'S' is other
        //:   invalid inputs, such as:
        //:   1 Literal nil.
        //:   2 Literal '0'.
        //:   3 Literal non-zero value.
        //:
        //: 5 'BSLS_DEPRECATE_ISNONZERO(S)' expands to 0 when 'S' is a single
        //:   '#undef'ined preprocessor symbol.
        //:
        //: 6 'BSLS_DEPRECATE_ISNONZERO(S)' expands to 0 when 'S' is a single
        //:   preprocessor symbol '#define'd as an arithmetic expression
        //:   evaluating to 0.
        //:
        //: 7 'BSLS_DEPRECATE_ISNONZERO(S)' expands to 1 when 'S' is a single
        //:   preprocessor symbol '#define'd as an arithmetic expression
        //:   that does not evaluate to 0.
        //:
        //: 8 QoI: 'BSLS_DEPRECATE_ISNONZERO(S)' expands to 0 for
        //:   non-arithmetic inputs.
        //
        // Plan:
        //: 1 Using the brute-force approach, individually test each of the
        //:   concerns.  (C-1..9)
        //
        // Testing:
        //   BSLS_DEPRECATE_ISDEFINED
        //   BSLS_DEPRECATE_ISNONZERO
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nOTHER SUPPORT MACROS"
                   "\n====================\n");
        }

#undef  UNDEF

#undef  NIL
#define NIL

#undef  ZERO
#define ZERO  0

#undef  ONE
#define ONE   1

#undef  ALPHA
#define ALPHA alpha


        if (verbose) printf("\nBSLS_DEPRECATE_ISDEFINED\n");
        {
            // Valid input

#if 0 != BSLS_DEPRECATE_ISDEFINED(UNDEF)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(NIL)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(ZERO)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(ONE)
            ASSERT(false);
#endif

            // Canonical input: control macros assembled with
            // 'BSLS_DEPRECATE_CAT'.

#undef BB_BUILDING_UOR_OMB
#if 0 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

#undef BB_BUILDING_UOR_OMB
#define BB_BUILDING_UOR_OMB
#if 1 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

#undef BB_BUILDING_UOR_OMB
#define BB_BUILDING_UOR_OMB 0
#if 1 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

#undef BB_BUILDING_UOR_OMB
#define BB_BUILDING_UOR_OMB 1
#if 1 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif
#undef BB_BUILDING_UOR_OMB

            // QoI

#if 0 != BSLS_DEPRECATE_ISDEFINED(ALPHA)
            ASSERT(false);
#endif

#if 1 != BSLS_DEPRECATE_ISDEFINED()
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(0)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(1)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(0123)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISDEFINED(321)
            ASSERT(false);
#endif

            // Invalid input: expressions
            //
            // Expansion of 'BSLS_DEPRECATE_ISDEFINED' varies depending on the
            // structure of the expression, hence these inputs are excluded by
            // the contract.

#if 1 != BSLS_DEPRECATE_ISDEFINED(7+2)
            ASSERT(false);
#endif

#if 0 != BSLS_DEPRECATE_ISDEFINED(UNDEF * 321)
            ASSERT(false);
#endif

#if 1 != BSLS_DEPRECATE_ISDEFINED(UNDEF + 321)
            ASSERT(false);
#endif

            // Invalid input: control macro name + '1L'
            //
            // Expansion of 'BSLS_DEPRECATE_ISDEFINED' can be fooled by
            // defining a macro consisting of a control macro name concatenated
            // with '1L', hence these inputs are excluded by the contract.

#undef BB_BUILDING_UOR_OMB1L
#define BB_BUILDING_UOR_OMB1L 0
#if 0 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

#undef BB_BUILDING_UOR_OMB1L
#define BB_BUILDING_UOR_OMB1L 1
#if 1 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

            // Invalid inputs: these uses will not compile
#if 0

#undef BB_BUILDING_UOR_OMB1L
#define BB_BUILDING_UOR_OMB1L
#if 0 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_CAT(BB_BUILDING_UOR_, OMB))
            ASSERT(false);
#endif

#if 1 != BSLS_DEPRECATE_ISDEFINED("0")
            ASSERT(false);
#endif

#if 0 != BSLS_DEPRECATE_ISDEFINED(BSLS_DEPRECATE_MAKE_VER(1,2))
            ASSERT(false);
#endif

#endif
        }

        if (verbose) printf("\nBSLS_DEPRECATE_ISNONZERO\n");
        {
            // Valid uses

#if 0 != BSLS_DEPRECATE_ISNONZERO(UNDEF)
            ASSERT(false);
#endif


#if 0 != BSLS_DEPRECATE_ISNONZERO(NIL)
            ASSERT(false);
#endif

#if 0 != BSLS_DEPRECATE_ISNONZERO(ZERO)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(ONE)
            ASSERT(false);
#endif

#if 0 != BSLS_DEPRECATE_ISNONZERO(0)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(1)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(0123)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(321)
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(7+2)
            ASSERT(false);
#endif


#if 0 != BSLS_DEPRECATE_ISNONZERO(BSLS_DEPRECATE_MAKE_VER(0,0))
            ASSERT(false);
#endif


#if 1 != BSLS_DEPRECATE_ISNONZERO(BSLS_DEPRECATE_MAKE_VER(1,2))
            ASSERT(false);
#endif


            // Canonical uses: control macros assembled with
            // 'BSLS_DEPRECATE_CAT'.

#undef UOR_NAME
#undef OMB_VERSION_DEPRECATION_THRESHOLD

#if 0 != BSLS_DEPRECATE_ISNONZERO(                                            \
                  BSLS_DEPRECATE_CAT(UOR_NAME, _VERSION_DEPRECATION_THRESHOLD))
            ASSERT(false);
#endif


#define UOR_NAME OMB

#if 0 != BSLS_DEPRECATE_ISNONZERO(                                            \
                  BSLS_DEPRECATE_CAT(UOR_NAME, _VERSION_DEPRECATION_THRESHOLD))
            ASSERT(false);
#endif


#undef  OMB_VERSION_DEPRECATION_THRESHOLD
#define OMB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(0, 0)

#if 0 != BSLS_DEPRECATE_ISNONZERO(                                            \
                  BSLS_DEPRECATE_CAT(UOR_NAME, _VERSION_DEPRECATION_THRESHOLD))
            ASSERT(false);
#endif


#undef  OMB_VERSION_DEPRECATION_THRESHOLD
#define OMB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(0, 1)

#if 1 != BSLS_DEPRECATE_ISNONZERO(                                            \
                  BSLS_DEPRECATE_CAT(UOR_NAME, _VERSION_DEPRECATION_THRESHOLD))
            ASSERT(false);
#endif


#undef  OMB_VERSION_DEPRECATION_THRESHOLD
#define OMB_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 0)

#if 1 != BSLS_DEPRECATE_ISNONZERO(                                            \
                  BSLS_DEPRECATE_CAT(UOR_NAME, _VERSION_DEPRECATION_THRESHOLD))
            ASSERT(false);
#endif


            // QoI

#if 0 != BSLS_DEPRECATE_ISNONZERO(ALPHA)
            ASSERT(false);
#endif


            // Invalid input: these uses will not compile
#if 0

#if 0 != BSLS_DEPRECATE_ISNONZERO("0")
            ASSERT(false);
#endif

#endif
        }

#undef NIL
#undef ZERO
#undef ONE
#undef ALPHA
#undef BB_BUILDING_UOR_OMB1L
#undef OMB_VERSION_DEPRECATION_THRESHOLD
#undef UOR_NAME
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BSLS_DEPRECATE_CAT
        //
        // Concerns:
        //: 1 'BSLS_DEPRECATE_CAT(A, B)' expands to '' if both 'A' and 'B' are
        //:   empty.
        //:
        //: 2 'BSLS_DEPRECATE_CAT(A, B)' expands to the expansion of 'A' if 'B'
        //:   is empty, and the expansion of 'B' if 'A' is empty.
        //:
        //: 3 The 'A' and 'B' contributions to the result are the *expansions*
        //:   of 'A' and 'B' if 'A' or 'B' are defined.
        //:
        //: 4 The 'A' and 'B' contributions to the result are the *names* of
        //:   'A' and 'B' if 'A' or 'B' are not defined.
        //:
        //: 5 The 'A' and 'B' contributions to the result are the deepest
        //:   *expansions* of 'A' and 'B' if 'A' or 'B' are defined in terms of
        //:   other macros.
        //:
        //: 6 'BSLS_DEPRECATE_CAT' can concatenate expansions of itself.
        //
        // Plan:
        //: 1 Using the brute force approach, compare the string value of
        //:   'BSLS_DEPRECATE_CAT(A, B)' with a string representing the
        //:   expected value.  (C-1..6)
        //
        // Testing:
        //   BSLS_DEPRECATE_CAT(A, B)
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBSLS_DEPRECATE_CAT"
                   "\n==================\n");
        }

#undef NO
#undef THING

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NO, THING)),
                      0 == strcmp("NOTHING",
                           MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NO, THING))));

#undef  NIL1
#define NIL1
#undef  NIL2
#define NIL2

#undef  X
#undef  Y

#undef  A
#define A X
#undef  B
#define B Y

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, B)),
                      0 == strcmp("Y",
                                MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, B))));

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, NIL1)),
                      0 == strcmp("X",
                                MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, NIL1))));

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, NIL2)),
                      0 == strcmp("",
                             MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, NIL2))));

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, B)),
                      0 == strcmp("XY",
                                  MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, B))));

#define X Ex
#define Y Why

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, B)),
                      0 == strcmp("ExWhy",
                                  MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, B))));

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, B)),
                      0 == strcmp("Why",
                                MACRO_TO_STRING(BSLS_DEPRECATE_CAT(NIL1, B))));

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, NIL1)),
                      0 == strcmp("Ex",
                                MACRO_TO_STRING(BSLS_DEPRECATE_CAT(A, NIL1))));

#undef  F
#undef  G
#undef  H
#undef  I
#define F() H()
#define G() I()
#define H() J
#define I() K

        LOOP_S_ASSERT(MACRO_TO_STRING(BSLS_DEPRECATE_CAT(F(), G())),
                      0 == strcmp("JK",
                               MACRO_TO_STRING(BSLS_DEPRECATE_CAT(F(), G()))));

#undef NIL1
#undef NIL2
#undef A
#undef B
#undef X
#undef Y
#undef F
#undef G
#undef H
#undef I

        LOOP_S_ASSERT(MACRO_TO_STRING(
                          BSLS_DEPRECATE_CAT(BSLS_DEPRECATE_CAT(A,B),
                                                   BSLS_DEPRECATE_CAT(X,Y))),
                      0 == strcmp("ABXY",
                             MACRO_TO_STRING(
                                BSLS_DEPRECATE_CAT(BSLS_DEPRECATE_CAT(A,B),
                                                   BSLS_DEPRECATE_CAT(X,Y)))));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS: MACRO_TO_STRING
        //
        // Concerns:
        //: 1 'MACRO_TO_STRING(M)' renders to '""' when 'M' expands to nothing.
        //:
        //: 2 'MACRO_TO_STRING(M)' renders to someting other than '""' when 'M'
        //:   expands to "".
        //:
        //: 3 'MACRO_TO_STRING(M)' renders to the expansion of 'M' in
        //:   non-empty, non-literal-empty-string cases.
        //:
        //: 4 'MACRO_TO_STRING(M)' renders to the final expansion of 'M', when
        //:   'M' is a macro that expands to other macros.
        //:
        //: 5 'MACRO_TO_STRING(M)' expands always to a literal string.
        //
        // Plan:
        //: 1 Using the brute force approach, compare the string value of
        //:   'MACRO_TO_STRING(M)' with a string representing the expected
        //:   value.  (C-1..4)
        //:
        //: 2 In each case covered in section 1, confirm that when the
        //:   expansion of 'MACRO_TO_STRING' is passed to an overloaded
        //:   function, the overload chosen is the one corresponding to a
        //:   'char const * const' literal.  (C-5)
        //
        // Testing:
        //   TEST APPARATUS: MACRO_TO_STRING
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTEST APPARATUS: MACRO_TO_STRING"
                   "\n===============================\n");
        }

#undef MACRO_UN_DEFINED

        if (verbose) printf("\nMacro is not defined.\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_UN_DEFINED)),
                        Case2::e_CONST_C_STRING ==
                               Case2::test(MACRO_TO_STRING(MACRO_UN_DEFINED)));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_UN_DEFINED),
                          0 == strcmp(               "MACRO_UN_DEFINED",
                                      MACRO_TO_STRING(MACRO_UN_DEFINED)));
        }

#undef  MACRO_NIL
#define MACRO_NIL

        if (verbose) printf("\nMacro expands to nil\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_NIL)),
                        Case2::e_CONST_C_STRING ==
                                      Case2::test(MACRO_TO_STRING(MACRO_NIL)));

            // Expansion is the empty string

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_NIL),
                          0 == strcmp(                "",
                                      MACRO_TO_STRING(MACRO_NIL)));
        }

#undef  MACRO_EMPTY_STRING
#define MACRO_EMPTY_STRING ""

        if (verbose) printf("\nMacro expands to literal empty string\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_EMPTY_STRING)),
                        Case2::e_CONST_C_STRING ==
                              Case2::test(
                                         MACRO_TO_STRING(MACRO_EMPTY_STRING)));

            // Expansion is not the empty string

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_EMPTY_STRING),
                          0 != strcmp(                "",
                                      MACRO_TO_STRING(MACRO_EMPTY_STRING)));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_EMPTY_STRING),
                          0 == strcmp(                "\"\"",
                                      MACRO_TO_STRING(MACRO_EMPTY_STRING)));
        }

#undef  MACRO_SYMBOL
#define MACRO_SYMBOL sOmEsYmBoL

        if (verbose) printf("\nMacro expands to other symbol\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_SYMBOL)),
                        Case2::e_CONST_C_STRING ==
                                   Case2::test(MACRO_TO_STRING(MACRO_SYMBOL)));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_SYMBOL),
                          0 == strcmp(                "sOmEsYmBoL",
                                      MACRO_TO_STRING(MACRO_SYMBOL)));
        }

#undef  MACRO_SEQUENCE
#define MACRO_SEQUENCE sequence of { "symbols" }

        if (verbose) printf("\nMacro expands to sequence of symbols\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_SEQUENCE)),
                        Case2::e_CONST_C_STRING ==
                                 Case2::test(MACRO_TO_STRING(MACRO_SEQUENCE)));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_SEQUENCE),
                          0 == strcmp(           "sequence of { \"symbols\" }",
                                      MACRO_TO_STRING(MACRO_SEQUENCE)));
        }

#undef  MACRO_MULTI_LINE
#define MACRO_MULTI_LINE while (true) {              \
                             doSomething() || break; \
                         }

        if (verbose) printf("\nMacro expands to multi-line sequence\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_MULTI_LINE)),
                        Case2::e_CONST_C_STRING ==
                               Case2::test(MACRO_TO_STRING(MACRO_MULTI_LINE)));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_MULTI_LINE),
                          0 == strcmp(
                                    "while (true) { doSomething() || break; }",
                                    MACRO_TO_STRING(MACRO_MULTI_LINE)));
        }

#undef  MACRO_MULTI_LAYER
#define MACRO_MULTI_LAYER() MACRO_ML_A()
#define MACRO_ML_A() victory MACRO_ML_B()
#define MACRO_ML_B() is MACRO_ML_C()
#define MACRO_ML_C() ours MACRO_ML_D()
#define MACRO_ML_D() !


        if (verbose) printf("\nMacro expands in multiple layers\n");
        {
            // Expansion is a string literal.

            LOOP_ASSERT(Case2::test(MACRO_TO_STRING(MACRO_MULTI_LAYER())),
                        Case2::e_CONST_C_STRING ==
                            Case2::test(MACRO_TO_STRING(MACRO_MULTI_LAYER())));

            // Expansion matches expected value.

            LOOP_S_ASSERT(MACRO_TO_STRING(MACRO_MULTI_LAYER()),
                          0 == strcmp(               "victory is ours !",
                                      MACRO_TO_STRING(MACRO_MULTI_LAYER())));
        }

#undef MACRO_NIL
#undef MACRO_EMPTY_STRING
#undef MACRO_SYMBOL
#undef MACRO_SEQUENCE
#undef MACRO_MULTI_LINE
#undef MACRO_MULTI_LAYER
#undef MACRO_ML_A
#undef MACRO_ML_B
#undef MACRO_ML_C
#undef MACRO_ML_D

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
        //: 1 Examine the expansions of the 'BSLS_DEPRECATE_IS_ACTIVE' macro
        //:   with combinations of the '*_VERSION_DEPRECATE_*_*',
        //:   'BB_SILENCE_DEPRECATION_*', 'BB_WARN_ALL_...', and
        //:   'BB_BUILDING_UOR_*' macros.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#if !BSLS_DEPRECATE_COMPILER_SUPPORT

        if (verbose) printf("\nCheck value of attribute.\n");

        ASSERT(0 == strcmp(MACRO_TO_STRING(BSLS_DEPRECATE), ""));

        if (verbose) printf("\nCheck BSLS_DEPRECATE_IS_ACTIVE is inactive.\n");

#else

        if (verbose) printf("\nCheck value of attribute.\n");

        int foundMatch = 0;
        foundMatch += (0 == strcmp(MACRO_TO_STRING(BSLS_DEPRECATE),
                                   "[[deprecated]]"));
        foundMatch += (0 == strcmp(MACRO_TO_STRING(BSLS_DEPRECATE),
                                   "__attribute__ ((deprecated))"));
        foundMatch += (0 == strcmp(MACRO_TO_STRING(BSLS_DEPRECATE),
                                   "__declspec(deprecated)"));

        ASSERT(1 == foundMatch);


        if (verbose) printf("\nNo macros defined.\n");

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif


        if (verbose) printf("\n<UOR>_VERSION_DEPRECATION_THRESHOLD <M>,<N>\n");

#define ABC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 2)
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#  define BB_SILENCE_DEPRECATION_ABC_1_2
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef  BB_SILENCE_DEPRECATION_ABC_1_2

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: silence 1.2 canceled
        ASSERT(true);
#else
        ASSERT(false);
#endif

#undef  ABC_VERSION_DEPRECATION_THRESHOLD

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: threshold 1.2 canceled
        ASSERT(false);
#else
        ASSERT(true);
#endif


        if (verbose) printf("\nBB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY\n");

#define BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#  define BB_SILENCE_DEPRECATION_ABC_1_2
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef  BB_SILENCE_DEPRECATION_ABC_1_2

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: silence 1.2 canceled
        ASSERT(true);
#else
        ASSERT(false);
#endif

#undef  BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: warn all canceled
        ASSERT(false);
#else
        ASSERT(true);
#endif


        if (verbose) printf("\nBB_BUILDING_UOR_<UOR>\n");

#define BB_BUILDING_UOR_ABC

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#define ABC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 2)
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef  ABC_VERSION_DEPRECATION_THRESHOLD

#  define BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef  BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY

#undef  BB_BUILDING_UOR_ABC


        if (verbose) printf("\nDefault threshold\n");

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: no default threshold yet
        ASSERT(false);
#else
        ASSERT(true);
#endif

#undef ABC_VERSION_MAJOR
#undef ABC_VERSION_MINOR

#define ABC_VERSION_MAJOR 1
#define ABC_VERSION_MINOR 3

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#  define BB_SILENCE_DEPRECATION_ABC_1_2
#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef  BB_SILENCE_DEPRECATION_ABC_1_2

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: silence 1.2 canceled
        ASSERT(true);
#else
        ASSERT(false);
#endif

#undef ABC_VERSION_MAJOR
#undef ABC_VERSION_MINOR

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)  // Check: default threshold canceled
        ASSERT(false);
#else
        ASSERT(true);
#endif


        if (verbose) printf("\nDefault threshold for x.0 version\n");

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 999)  // Check: no default threshold yet
        ASSERT(false);
#else
        ASSERT(true);
#endif

#define DEF_VERSION_MAJOR 3
#define DEF_VERSION_MINOR 0

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 998)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 999)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 3, 0)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#  define BB_SILENCE_DEPRECATION_DEF_2_999
#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 998)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 999)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 3, 0)
        ASSERT(false);
#else
        ASSERT(true);
#endif
#  undef BB_SILENCE_DEPRECATION_DEF_2_999

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 999)  // Check: silence 1.2 canceled
        ASSERT(true);
#else
        ASSERT(false);
#endif

#undef DEF_VERSION_MAJOR
#undef DEF_VERSION_MINOR

#if BSLS_DEPRECATE_IS_ACTIVE(DEF, 2, 999)  // Check: default threshold canceled
        ASSERT(false);
#else
        ASSERT(true);
#endif

#endif  // DEPRECATION_SUPPORTED
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // SUPPORT MACRO RESEARCH: ISZERO
        //
        // Testing:
        //   SUPPORT MACRO RESEARCH: ISZERO
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nSUPPORT MACRO RESEARCH: ISZERO"
                   "\n==============================\n");
        }

        // First, we want to figure out whether or not we can do arithmetic on
        // non-resolved macros:

// An expression that distinguishes between:
//   A: undefined or empty define or non-numeric
//   B: defined numeric value
#define TA_SENTINEL_0 1
#define TA_SENTINEL_1 0

#define TA_ISZERO(...)      TA_ISZERO_A((__VA_ARGS__, 1))
#define TA_ISZERO_A(T)      TA_ISZERO_B T
#define TA_ISZERO_B(X, ...) TA_ISZERO_C(TA_SENTINEL_ ## X)
#define TA_ISZERO_C(X)      (X + 1 == 2)

//NOK: #define TA_ISZERO(...) TA_ISZERO_A(TA_DENULLIFY((__VA_ARGS__, 1)))
//NOK: #define TA_ISZERO_A(X) TA_ISZERO_B(X)
//NOK: #define TA_ISZERO_B(X) TA_ISZERO_C(TA_SENTINEL_ ## X)
//NOK: #define TA_ISZERO_C(X) (X + 1 == 2)
//NOK:
//NOK: #define TA_DENULLIFY(T)           TA_DENULLIFY_A T
//NOK: #define TA_DENULLIFY_B(A, ...)    A

//OK: #define TA_ISZERO(X)   TA_ISZERO_A(X)
//OK: #define TA_ISZERO_A(X) TA_ISZERO_B(X)
//OK: #define TA_ISZERO_B(X) TA_ISZERO_C(TA_SENTINEL_ ## X)
//OK: #define TA_ISZERO_C(X) (X + 1 == 2)

        // Undefined Case: true

#undef TA_ABC

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(true);
#else
        ASSERT(false);
#endif

        // Defined Numeric Cases (0, 1, 9, 257): false

#define TA_ABC 0

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#undef TA_ABC
#define TA_ABC 1

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#undef TA_ABC
#define TA_ABC 9

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#undef TA_ABC
#define TA_ABC 257

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(false);
#else
        ASSERT(true);
#endif

        // Defined Non-Numeric Case: true

#undef TA_ABC
#define TA_ABC willywonka

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(true);
#else
        ASSERT(false);
#endif

        // Defined Null Case: true

#undef TA_ABC
#define TA_ABC

#if TA_ABC + 1 == 1 && !TA_ISZERO(TA_ABC)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#undef TA_ABC
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS: DATA COLLECTION
        //
        // Testing:
        //   TEST APPARATUS: DATA COLLECTION
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTEST APPARATUS: DATA COLLECTION"
                   "\n===============================\n");
        }

        if (veryVeryVerbose) {
            printf("%d\n", UVW_VERSION_DEPRECATION_THRESHOLD);
        }

        // 'PP_AUDIT' can be defined and the preprocessor invoked to allow
        // direct inspection of macro expansions.

#ifdef PP_AUDIT
"((4) * 1000 + (5))": UVW_VERSION_DEPRECATION_THRESHOLD
"0"                 : BSLS_DEPRECATE_ISDEFINED(BB_SILENCE_DEPRECATION_BBB_4_1)
"1"                 : BSLS_DEPRECATE_ISDEFINED(BB_SILENCE_DEPRECATION_BBC_4_1)
"(0 || 0)"          : BSLS_DEPRECATE_ISRETAINED(ABC, 1, 1)
"(0 || 0)"          : BSLS_DEPRECATE_ISRETAINED(ABC, 1, 2)
"(0 || 0)"          : BSLS_DEPRECATE_ISRETAINED(ABC, 1, 3)
#endif  // PP_AUDIT

#if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 1)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if BSLS_DEPRECATE_ISRETAINED(ABC, 1, 1)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if BSLS_DEPRECATE_ISRETAINED(BBB, 4, 1)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if BSLS_DEPRECATE_ISRETAINED(BBC, 4, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_ISRETAINED(BBF, 4, 1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#ifdef PP_AUDIT
1: COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, BBG));
0: COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, BBI));

1: COLLECT_ISNONZERO(BBG, VERSION_DEPRECATION_THRESHOLD));
0: COLLECT_ISNONZERO(BBI, VERSION_DEPRECATION_THRESHOLD));

1: COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, DDG));
0: COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, DDI));

1: COLLECT_ISNONZERO(DDG, VERSION_DEPRECATION_THRESHOLD));
0: COLLECT_ISNONZERO(DDI, VERSION_DEPRECATION_THRESHOLD));
#endif  // PP_AUDIT

#if COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, BBG)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, BBI)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if COLLECT_ISNONZERO(BBG, VERSION_DEPRECATION_THRESHOLD)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNONZERO(BBI, VERSION_DEPRECATION_THRESHOLD)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, DDG)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, DDI)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if COLLECT_ISNONZERO(DDG, VERSION_DEPRECATION_THRESHOLD)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNONZERO(DDI, VERSION_DEPRECATION_THRESHOLD)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if defined(CCG_VERSION_MAJOR)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNUMERIC(CCG, VERSION_MAJOR)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNUMERIC(CCI, VERSION_MAJOR)
        ASSERT(false);
#else
        ASSERT(true);
#endif

#if defined(CCG_VERSION_MINOR)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNUMERIC(CCG, VERSION_MINOR)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if COLLECT_ISNUMERIC(CCI, VERSION_MINOR)
        ASSERT(false);
#else
        ASSERT(true);
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// BDE_VERIFY pragma: pop  // Restore bde_verify rules

#else  // !defined(BSLS_DEPRECATE_T_DATA_COLLECTION): false case

#if BSLS_DEPRECATE_T_DATA_COLLECTION == 1

// ============================================================================
//                     DATA COLLECTION: CASES 6, 7 and 8
// ----------------------------------------------------------------------------

#define COLLECT_U COLLECT_EXTRACT1(COLLECT_CODE)
#define COLLECT_M COLLECT_EXTRACT2(COLLECT_CODE)
#define COLLECT_N COLLECT_EXTRACT3(COLLECT_CODE)

#ifdef PP_AUDIT
"cod" COLLECT_CODE
"U"   COLLECT_U
"M"   COLLECT_M
"N"   COLLECT_N
"ret" BSLS_DEPRECATE_ISRETAINED(COLLECT_U, COLLECT_M, COLLECT_N)
"def" BSLS_DEPRECATE_ISDEFINED(BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY)
"thr" BSLS_DEPRECATE_ISPASTTHRESHOLD(COLLECT_U, COLLECT_M, COLLECT_N)
"act" BSLS_DEPRECATE_IS_ACTIVE(COLLECT_U, COLLECT_M, COLLECT_N)
#endif  // PP_AUDIT

#undef expectedValue

#if BSLS_DEPRECATE_IS_ACTIVE(COLLECT_U, COLLECT_M, COLLECT_N)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(expectedActive,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if BSLS_DEPRECATE_ISRETAINED(COLLECT_U, COLLECT_M, COLLECT_N)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(expectedRetained,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if BSLS_DEPRECATE_ISPASTTHRESHOLD(COLLECT_U, COLLECT_M, COLLECT_N)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(expectedIsPast,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if defined(BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(isWarnDefined,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if COLLECT_BB_ISDEFINED_UMN(BB_SILENCE_DEPRECATION,                          \
                             COLLECT_U, COLLECT_M, COLLECT_N)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(isSilenceDefined,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if COLLECT_BB_ISDEFINED_U(BB_BUILDING_UOR, COLLECT_U)
#define expectedValue e_YES
#else
#define expectedValue e_NO
#endif

COLLECT_SET_VALUE(isBuildingDefined,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if COLLECT_ISNONZERO(COLLECT_U, VERSION_DEPRECATION_THRESHOLD)
#define expectedValue COLLECT_MKNAME(COLLECT_U,                               \
                                     VERSION_DEPRECATION_THRESHOLD)
#else
#define expectedValue -1
#endif

COLLECT_SET_VALUE(threshold,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if COLLECT_ISNUMERIC(COLLECT_U, VERSION_MAJOR)
#define expectedValue COLLECT_MKNAME(COLLECT_U, VERSION_MAJOR)
#else
#define expectedValue -1
#endif

COLLECT_SET_VALUE(majorVersion,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#if COLLECT_ISNUMERIC(COLLECT_U, VERSION_MINOR)
#define expectedValue COLLECT_MKNAME(COLLECT_U, VERSION_MINOR)
#else
#define expectedValue -1
#endif

COLLECT_SET_VALUE(minorVersion,
                  COLLECT_U,
                  COLLECT_M,
                  COLLECT_N,
                  expectedValue
);

#undef expectedValue

#undef COLLECT_U
#undef COLLECT_M
#undef COLLECT_N
#undef COLLECT_CODE

#elif BSLS_DEPRECATE_T_DATA_COLLECTION == 2

// ============================================================================
//                         DATA COLLECTION: CASE 11
// ----------------------------------------------------------------------------

// Idiom 1: fn with or'ed tag

namespace idiom1 {

#if    BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)  \
    || defined(BDE_OMIT_DEPRECATED)
BSLS_DEPRECATE_STAND_IN
#endif
Tag DEPRECATED_SYMBOL();
    // Return something.

Tag DEPRECATED_SYMBOL()
{
    return Tag();
}

Results check()
    // Return a 'Results' object encoding information about the environment
    // when this function was compiled.
{
    Results result;
    result.d_deprecationStatus = DEPRECATED_SYMBOL().value;
    result.d_isActive =
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
        true;
#else
        false;
#endif
    result.d_isLegacyMacroDefined =
#ifdef BDE_OMIT_DEPRECATED
        true;
#else
        false;
#endif
    return result;
}

}  // close namespace idiom1

// Idiom 2: fn with tag and removal

namespace idiom2 {

#ifndef BDE_OMIT_DEPRECATED
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
BSLS_DEPRECATE_STAND_IN
#endif
Tag DEPRECATED_SYMBOL();
    // Return something.
#endif

#ifndef BDE_OMIT_DEPRECATED
Tag DEPRECATED_SYMBOL()
{
    return Tag();
}
#endif

Results check()
    // Return a 'Results' object encoding information about the environment
    // when this function was compiled.
{
    Results result;
    result.d_deprecationStatus = DEPRECATED_SYMBOL().value;
    result.d_isActive =
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
        true;
#else
        false;
#endif
    result.d_isLegacyMacroDefined =
#ifdef BDE_OMIT_DEPRECATED
        true;
#else
        false;
#endif
    return result;
}

}  // close namespace idiom2

// Idiom 3: class with tag and removal

namespace idiom3 {

struct
#if    BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)  \
    || defined(BDE_OMIT_DEPRECATED)
BSLS_DEPRECATE_STAND_IN_STRUCT
#endif
DEPRECATED_SYMBOL : Tag {
};

Results check()
    // Return a 'Results' object encoding information about the environment
    // when this function was compiled.
{
    Results result;
    result.d_deprecationStatus = DEPRECATED_SYMBOL().value;
    result.d_isActive =
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
        true;
#else
        false;
#endif
    result.d_isLegacyMacroDefined =
#ifdef BDE_OMIT_DEPRECATED
        true;
#else
        false;
#endif
    return result;
}

}  // close namespace idiom3

// Idiom 4: class with tag and removal

namespace idiom4 {

#ifndef BDE_OMIT_DEPRECATED
struct
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
BSLS_DEPRECATE_STAND_IN_STRUCT
#endif
DEPRECATED_SYMBOL : Tag {
};
#endif

Results check()
    // Return a 'Results' object encoding information about the environment
    // when this function was compiled.
{
    Results result;
    result.d_deprecationStatus = DEPRECATED_SYMBOL().value;
    result.d_isActive =
#if BSLS_DEPRECATE_IS_ACTIVE(LEG, 4, 3)
        true;
#else
        false;
#endif
    result.d_isLegacyMacroDefined =
#ifdef BDE_OMIT_DEPRECATED
        true;
#else
        false;
#endif
    return result;
}

}  // close namespace idiom4

#undef LEG_VERSION_DEPRECATION_THRESHOLD
#undef BDE_OMIT_DEPRECATED

#else

#error "Unsupported value for 'BSLS_DEPRECATE_T_DATA_COLLECTION'"

#endif  // BSLS_DEPRECATE_T_DATA_COLLECTION == N

#endif  // !defined(BSLS_DEPRECATE_T_DATA_COLLECTION)

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
