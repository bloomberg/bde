// bslfmt_testspecificationgenerator.t.cpp                            -*-C++-*-
#include <bslfmt_testspecificationgenerator.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>  // `strlen`

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>  //  oracle tests
#endif

using namespace BloombergLP;
// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// TBD.
// The test driver for this component is in its infancy.  The test plan will be
// filled in as the test driver is filled with content.
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslfmt::TestSpecificationGenerator<char>    CharObj;
typedef bslfmt::TestSpecificationGenerator<wchar_t> WcharObj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage of Generator
///- - - - - - - - - - - - - - - - -
// The generator is a state machine, each state of which generates a unique
// format specification.  Depending on the set of instructions, the number of
// possible states also changes.  For simplicity, we will take one parameter,
// the sign, that takes three possible values: `+`, `-` and ` `. Accordingly,
// we expect that the generator will go through four states, because the
// default state always comes first, generating an empty specification.
// First, let's create the generator and configure it:
// ```
    typedef bslfmt::TestSpecificationGenerator<char> Generator;

    const Generator::Sign POSITIVE = Generator::e_SIGN_POSITIVE;
    const Generator::Sign NEGATIVE = Generator::e_SIGN_NEGATIVE;
    const Generator::Sign SPACE    = Generator::e_SIGN_SPACE;

    bslfmt::TestSpecificationGenerator<char> generator;
// ```
// Note that the instructions are given to the generator in a special language,
// a description of which can be found in the component documentation:
// ```
    generator.setup("+");
// ```
// You can get information about the current state of the generator using
// multiple accessors.  Since we requested only one option, most of these
// accessors return false when asked about the presence of a particular option.
// Note that a request to get the value of an option, if this option is not
// present in the current state, is undefined behavior:
// ```
    ASSERT(true    == generator.isStateValidForParse());
    ASSERT(true    == generator.isStateValidForFormat());

    ASSERT(false   == generator.isValueIdOptionPresent());
    ASSERT(false   == generator.isFillCharacterPresent());
    ASSERT(false   == generator.isAlignOptionPresent());
    ASSERT(false   == generator.isSignOptionPresent());
    ASSERT(false   == generator.isHashOptionPresent());
    ASSERT(false   == generator.isZeroOptionPresent());
    ASSERT(false   == generator.isWidthOptionPresent());
    ASSERT(false   == generator.isNestedWidthPresent());
    ASSERT(false   == generator.isPrecisionOptionPresent());
    ASSERT(false   == generator.isNestedPrecisionPresent());
    ASSERT(false   == generator.isLocaleOptionPresent());
    ASSERT(false   == generator.isTypeOptionPresent());
// ```
// As we have already said, the initial state of the generator generates an
// empty string.  Therefore, even though we added the sign to the instruction,
// this option is not present in this state.  Keep in mind that the absence of
// an option is always another variant when iterating through the states of the
// generator.
// For each state, the generator creates two strings: one that the
// `bsl::format` passes to the formatter's `parse` function, and one that can
// be passed to the `bsl::format` function itself:
// ```
    ASSERT(""      == generator.spec());
    ASSERT("{:}"   == generator.formatSpec());
// ```
// Then, let's move on to the next state of the generator. When the generator
// has iterated over all possible states, the function `next` will return
// `false`, but for now we expect it to return `true`:
// ```
    ASSERT(true == generator.next());
// ```
// For ease of reading, we will not check all the options each time, but only
// those that are significant for our example:
    ASSERT(true     == generator.isStateValidForParse());
    ASSERT(true     == generator.isStateValidForFormat());
    ASSERT(true     == generator.isSignOptionPresent());
    ASSERT(POSITIVE == generator.sign());
    ASSERT("+"      == generator.spec());
    ASSERT("{:+}"   == generator.formatSpec());
// ```
// The function `next` iterates over all possible states of the generator,
// including those that generate strings, the processing of which by the
// corresponding functions will lead to an exception being thrown. Such states
// can be determined using the functions `isStateValidForParse` and
// `isStateValidForFormat`.  Another option can be to skip invalid states using
// the functions `nextValidForParse` and `nextValidForFormat`, respectively:
// ```
    ASSERT(true     == generator.nextValidForParse());
    ASSERT(true     == generator.isStateValidForParse());
    ASSERT(true     == generator.isStateValidForFormat());
    ASSERT(true     == generator.isSignOptionPresent());
    ASSERT(NEGATIVE == generator.sign());
    ASSERT("-"      == generator.spec());
    ASSERT("{:-}"   == generator.formatSpec());

    ASSERT(true     == generator.nextValidForFormat());
    ASSERT(true     == generator.isStateValidForParse());
    ASSERT(true     == generator.isStateValidForFormat());
    ASSERT(true     == generator.isSignOptionPresent());
    ASSERT(SPACE    == generator.sign());
    ASSERT(" "      == generator.spec());
    ASSERT("{: }"   == generator.formatSpec());
// ```
// Now, the generator is in its last state, so the `next` function should
// return` false`:
// ```
    ASSERT(false == generator.next());
// ```
// Finally, let's make sure that the generator is back in its initial state and
// ready to go for the second round:
// ```
    ASSERT(""    == generator.spec());
    ASSERT("{:}" == generator.formatSpec());
    ASSERT(true  == generator.next());
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 Generator produces all possible variations of specification
        //:   according to the received instruction.
        //:
        //: 2 The main loop of the generator states can be run over and over,
        //:   producing the same specifications.
        //
        // Plan:
        //: 1 Specify some option and consequently check all generator states
        //:   using table-based approach.
        //:
        //: 2. Setup generator and iterate through all its states once.  Setup
        //:    second generator using the same instructions that have been used
        //:    for the first one.  Now run both generators simultaneously and
        //:    verify that they produce the same results.  (C-2)
        //:
        //: 3. Setup generator with nested width and nested precision options.
        //:    Iterate through the certain number of generator states and
        //:    verify its attributes using table-based approach.
        //:
        //: 4. Use standard formatting implementation (if available) to verify
        //:    validity of the specifications produced by the generator.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef CharObj::Alignment Alignment;

        const char       charNA = 0;
        const Alignment alignNA = CharObj::e_ALIGN_LEFT;

        static const struct {
            int         d_line;                   // source line number
            bool        d_fillCharacterPresence;  // fill character presence
            char        d_fillCharacter;          // fill character
            bool        d_alignmentPresence;      // align optionpresence
            Alignment   d_alignment;              // alignment type
            const char *d_spec_p;                 // specification

        } DATA[] = {
            //LINE  FILL_P  FILL_V   ALIGN_P  ALIGN_V                   SPEC
            //----  ------  ------   -------  ------------------------  ----
            { L_,   false,  charNA,  false,   alignNA,                  ""   },
            { L_,   false,  charNA,  true,    CharObj::e_ALIGN_LEFT,    "<"  },
            { L_,   false,  charNA,  true,    CharObj::e_ALIGN_MIDDLE,  "^"  },
            { L_,   false,  charNA,  true,    CharObj::e_ALIGN_RIGHT,   ">"  },
        //  { L_,   true,   '*',     false,   alignNA,                  "*"  },
            { L_,   true ,  '*',     true,    CharObj::e_ALIGN_LEFT,    "*<" },
            { L_,   true,   '*',     true,    CharObj::e_ALIGN_MIDDLE,  "*^" },
            { L_,   true,   '*',     true,    CharObj::e_ALIGN_RIGHT,   "*>" },
        //  { L_,   true,   '=',     false,   alignNA,                  "="  },
            { L_,   true ,  '=',     true,    CharObj::e_ALIGN_LEFT,    "=<" },
            { L_,   true,   '=',     true,    CharObj::e_ALIGN_MIDDLE,  "=^" },
            { L_,   true,   '=',     true,    CharObj::e_ALIGN_RIGHT,   "=>" },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\tTesting align and fill option\n");
        {
            CharObj generator;
            generator.setup("F^");
            const bsl::string& spec = generator.spec();

            size_t i = 0;
            do {
                int               LINE           = DATA[i].d_line;
                bool              FILL_PRESENCE  =
                                               DATA[i].d_fillCharacterPresence;
                char              FILL_CHARACTER = DATA[i].d_fillCharacter;
                bool              ALIGN_PRESENCE = DATA[i].d_alignmentPresence;
                Alignment         ALIGNMENT      = DATA[i].d_alignment;
                const bsl::string EXP_SPEC       = DATA[i].d_spec_p;

                ASSERTV(LINE,
                        FILL_PRESENCE == generator.isFillCharacterPresent());
                if (FILL_PRESENCE) {
                    ASSERTV(LINE,
                            FILL_CHARACTER,
                            generator.fillCharacter(),
                            FILL_CHARACTER == generator.fillCharacter());
                  }
                  ASSERTV(LINE,
                          ALIGN_PRESENCE == generator.isAlignOptionPresent());
                  if (ALIGN_PRESENCE) {
                      ASSERTV(LINE,
                              ALIGNMENT,
                              generator.alignment(),
                              ALIGNMENT == generator.alignment());
                  }
                  ASSERTV(LINE,
                          EXP_SPEC.c_str(),
                          spec.c_str(),
                          EXP_SPEC == spec);

                  ++i;
            } while (generator.nextValidForParse());
            ASSERTV(NUM_DATA, i, NUM_DATA == i);
        }

        if (verbose) printf("\tTesting consecutive generator cycles.\n");
        {
            const char *generatorInstruction = "V{}";
            CharObj origin;
            origin.setup(generatorInstruction);
            const bsl::string& originSpec       = origin.spec();
            const bsl::string& originFormatSpec = origin.formatSpec();
            int originCounter = 0;

            // Run the full cycle once.

            do {
               ++originCounter;
            } while(origin.next());

            if (veryVerbose) printf("\t\tTotal: %d\n", originCounter);

            CharObj checker;
            checker.setup(generatorInstruction);
            const bsl::string& checkerSpec       = checker.spec();
            const bsl::string& checkerFormatSpec = checker.formatSpec();

            ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                    checkerSpec == originSpec);
            ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                    checkerFormatSpec == originFormatSpec);

            // Now run two generators simultaneously and compare the results.

            while (checker.next()) {
               ASSERT(origin.next());

               ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                       checkerSpec == originSpec);
               ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                       checkerFormatSpec == originFormatSpec);
               --originCounter;
            }
            ASSERT(!origin.next());
            --originCounter;

            ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                    checkerSpec == originSpec);
            ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                    checkerFormatSpec == originFormatSpec);
            ASSERTV(originCounter, 0 == originCounter);
        }

        if (verbose)
            printf("\tTesting the iteration of the generator states\n");
        {
            typedef CharObj::NestedVariant NestedVar;

            // Table abbreviations
            const bool      F   = false;
            const bool      T   = true;
            const NestedVar DFT = CharObj::e_NESTED_DEFAULT;
            const NestedVar A_1 = CharObj::e_NESTED_ARG_1;
            const NestedVar A_2 = CharObj::e_NESTED_ARG_2;

            static const struct {
                int              d_line;
                                     // source line number
                const char      *d_spec_p;
                                     // expected `parse` specification
                const char      *d_format_p;
                                     // expected `format` specification

                const bool       d_widthPresence;
                                     // expected width option presence

                const int        d_widthValue;
                                     // expected width value

                const bool       d_nestedWidthPresence;
                                     // expected nested width option presence

                const NestedVar  d_nestedWidthValue;
                                     // expected nested width value

                const bool       d_precisionPresence;
                                     // expected precision option presence

                const int        d_precisionValue;
                                     // expected precision value

                const bool       d_nestedPrecisionPresence;
                                     // expected nested precision presence

                const NestedVar  d_nestedPrecisionValue;
                                     // expected nested precision value

                const bool       d_isValidForParse;
                                     // expected specification validity for
                                     // the formatters `parse` method

                const bool       d_isValidForFormat;
                                     // expected specification validity for
                                     // the formatters `format` method

            } DATA[] = {
  //------------^
  //L   PARSE      FORMAT        WP  WV  NWP NWV   PP  PV  NPP NPV   PVD  FVD
  //--- ---------- ------------  --- --- --- ---   --- --- --- ---   ---  ---
  { L_, "",        "{:}",        F,  1,  F,  DFT,  F,  0,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  F,  0,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  F,  0,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  F,  0,  T,  A_2,  T,   F },
  { L_, ".0",      "{:.0}",      F,  1,  F,  DFT,  T,  0,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  0,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  0,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  0,  T,  A_2,  T,   F },
  { L_, ".5",      "{:.5}",      F,  1,  F,  DFT,  T,  5,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  5,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  5,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  5,  T,  A_2,  T,   F },
  { L_, ".6",      "{:.6}",      F,  1,  F,  DFT,  T,  6,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  6,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  6,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  6,  T,  A_2,  T,   F },
  { L_, ".10",     "{:.10}",     F,  1,  F,  DFT,  T,  10, F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  10, T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  10, T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  10, T,  A_2,  T,   F },
  { L_, "{}",      "{:{}}",      F,  1,  T,  DFT,  F,  0,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  F,  0,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  F,  0,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  F,  0,  T,  A_2,  F,   F },
  { L_, "{}.0",    "{:{}.0}",    F,  1,  T,  DFT,  T,  0,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  0,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  0,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  0,  T,  A_2,  F,   F },
  { L_, "{}.5",    "{:{}.5}",    F,  1,  T,  DFT,  T,  5,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  5,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  5,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  5,  T,  A_2,  F,   F },
  { L_, "{}.6",    "{:{}.6}",    F,  1,  T,  DFT,  T,  6,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  6,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  6,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  6,  T,  A_2,  F,   F },
  { L_, "{}.10",   "{:{}.10}",   F,  1,  T,  DFT,  T,  10, F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  10, T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  10, T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  10, T,  A_2,  F,   F },
  { L_, "{1}",     "{:{1}}",     F,  1,  T,  A_1,  F,  0,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  F,  0,  T,  DFT,  F,   F },
  { L_, "{1}.{1}", "{:{1}.{1}}", F,  1,  T,  A_1,  F,  0,  T,  A_1,  T,   F },
  { L_, "{1}.{2}", "{:{1}.{2}}", F,  1,  T,  A_1,  F,  0,  T,  A_2,  T,   F },
  { L_, "{1}.0",   "{:{1}.0}",   F,  1,  T,  A_1,  T,  0,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  T,  0,  T,  DFT,  F,   F },
  { L_, "{1}.{1}", "{:{1}.{1}}", F,  1,  T,  A_1,  T,  0,  T,  A_1,  T,   F },
  { L_, "{1}.{2}", "{:{1}.{2}}", F,  1,  T,  A_1,  T,  0,  T,  A_2,  T,   F },
  { L_, "{1}.5",   "{:{1}.5}",   F,  1,  T,  A_1,  T,  5,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  T,  5,  T,  DFT,  F,   F },
  //------------v
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            CharObj generator;
            generator.setup("{}");

            for (size_t i = 0; i < NUM_DATA; ++i) {
                int              LINE        = DATA[i].d_line;
                const char      *SPEC        = DATA[i].d_spec_p;
                const char      *FMT_SPEC    = DATA[i].d_format_p;
                const bool       W_PRESENCE  = DATA[i].d_widthPresence;
                const int        W_VALUE     = DATA[i].d_widthValue;
                const bool       NW_PRESENCE = DATA[i].d_nestedWidthPresence;
                const NestedVar  NW_VALUE    = DATA[i].d_nestedWidthValue;
                const bool       P_PRESENCE  = DATA[i].d_precisionPresence;
                const int        P_VALUE     = DATA[i].d_precisionValue;
                const bool       NP_PRESENCE =
                                             DATA[i].d_nestedPrecisionPresence;
                const NestedVar  NP_VALUE    = DATA[i].d_nestedPrecisionValue;
                const bool       PRS_VALID   = DATA[i].d_isValidForParse;
                const bool       FMT_VALID   = DATA[i].d_isValidForFormat;

                ASSERTV(LINE, SPEC, generator.spec().c_str(),
                        SPEC == generator.spec());
                ASSERTV(LINE, FMT_SPEC, generator.formatSpec().c_str(),
                        FMT_SPEC == generator.formatSpec());
                ASSERTV(LINE, generator.isWidthOptionPresent(),
                        W_PRESENCE == generator.isWidthOptionPresent());
                ASSERTV(LINE, W_VALUE, generator.width(),
                        W_VALUE == generator.width());
                ASSERTV(LINE, generator.isNestedWidthPresent(),
                        NW_PRESENCE == generator.isNestedWidthPresent());
                ASSERTV(LINE, NW_VALUE, generator.nestedWidthVariant(),
                        NW_VALUE == generator.nestedWidthVariant());
                ASSERTV(LINE, generator.isPrecisionOptionPresent(),
                        P_PRESENCE == generator.isPrecisionOptionPresent());
                ASSERTV(LINE, P_VALUE,generator.precision(),
                        P_VALUE == generator.precision());
                ASSERTV(LINE, generator.isNestedPrecisionPresent(),
                        NP_PRESENCE == generator.isNestedPrecisionPresent());
                ASSERTV(LINE, NP_VALUE, generator.nestedPrecisionVariant(),
                        NP_VALUE == generator.nestedPrecisionVariant());
                ASSERTV(LINE, generator.isStateValidForParse(),
                        PRS_VALID == generator.isStateValidForParse());
                ASSERTV(LINE, generator.isStateValidForFormat(),
                        FMT_VALID == generator.isStateValidForFormat());

                ASSERTV(LINE, generator.next());
            }
        }

        // Testing generator using standard library implementation

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\tTesting generator with oracle.\n");
        {
            CharObj generator;
            generator.setup("VF^+#0{}Lf");

            const bsl::string& spec    = generator.formatSpec();
            int                counter = 0;
            float              arg0    = 1;
            int                arg1    = 2;
            int                arg2    = 3;
            do {
                try {
                    if (generator.isNestedWidthPresent()) {
                        if (generator.isNestedPrecisionPresent()) {
                            (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                        }
                        else {
                            (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                        }
                    }
                    else if (generator.isNestedPrecisionPresent()) {
                        (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                    }
                    else {
                        (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                    }

                    // Verify that we do not miss states that are considered
                    // valid by the standard implementation.

                    ASSERTV(counter,
                            spec.c_str(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            generator.isStateValidForParse() &&
                                generator.isStateValidForFormat());
                }
                catch (std::format_error& err) {

                    // Verify that if the standard implementation considers
                    // the specification invalid then the generator considers
                    // it invalid too.
                    ASSERTV(counter,
                            spec.c_str(),
                            err.what(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            !generator.isStateValidForParse() ||
                                !generator.isStateValidForFormat());
                }
                ++counter;
            } while (generator.next());

            if (veryVerbose) {
                T_ T_ P(counter);
            }
        }

        // wchar_t
        {
            WcharObj  generator;
            generator.setup("VF^+#0{}Lf");

            const bsl::wstring& spec    = generator.formatSpec();
            int                 counter = 0;
            float               arg0    = 1;
            int                 arg1    = 2;
            int                 arg2    = 3;
            do {
                try {
                    if (generator.isNestedWidthPresent()) {
                        if (generator.isNestedPrecisionPresent()) {
                            (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                        }
                        else {
                            (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                        }
                    }
                    else if (generator.isNestedPrecisionPresent()) {
                        (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                    }
                    else {
                        (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                    }

                     // Verify that we do not miss states that are considered
                     // valid by the standard implementation.

                     ASSERTV(counter,
                             spec.c_str(),
                             generator.isStateValidForParse(),
                             generator.isStateValidForFormat(),
                             generator.isStateValidForParse() &&
                                 generator.isStateValidForFormat());
                }
                catch (std::format_error& err) {
                    // Verify that if the standard implementation considers
                    // the specification invalid then the generator considers
                    // it invalid too.

                    ASSERTV(counter,
                            spec.c_str(),
                            err.what(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            !generator.isStateValidForParse() ||
                                !generator.isStateValidForFormat());
                }
                ++counter;
            } while (generator.next());

            if (veryVerbose) {
                T_ T_ P(counter);
            }
        }
#endif
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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

