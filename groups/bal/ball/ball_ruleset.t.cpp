// ball_ruleset.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_ruleset.h>

#include <ball_predicate.h>
#include <ball_severity.h>                      // for testing only

#include <bslim_testutil.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a value-semantic class 'ball::RuleSet'.
// We choose the default constructor as well as 'addRule' and 'removeRule' as
// the primary manipulators, 'ruleId' and 'getRuleById' as the basic accessors.
// We then apply the standard 10-step test procedure as well as a few test
// cases methods that are specific for this component.
//-----------------------------------------------------------------------------
// [ 4] static int maxNumRules();
// [ 2] ball::RuleSet();
// [ 7] ball::Rule(const ball::Rule&, bdema::Alct * = 0);
// [ 2] ~ball::RuleSet();
// [ 2] int addRule(const ball::Rule& value);
// [10] int addRules(const ball::RuleSet& rules);
// [10] int removeRuleById(int id);
// [ 2] int removeRule(const ball::Rule& value);
// [10] int removeRules(const ball::RuleSet& rules);
// [10] void removeAllRules();
// [ 9] const ball::Rule& operator=(const ball::Rule& other);
// [ 4] int ruleId(const ball::Rule& value) const;
// [ 4] const ball::Rule *getRuleById(int id) const;
// [ 4] int numRules() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] bool operator==(const ball::Rule& lhs, const ball::Rule& rhs)
// [ 6] bool operator!=(const ball::Rule& lhs, const ball::Rule& rhs)
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::PS&) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMITIVE TEST APPARATUS: 'gg'
// [ 8] UNUSED
// [11] USAGE EXAMPLE

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
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

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

typedef ball::RuleSet       Obj;
typedef bsls::Types::Int64 Int64;

const ball::Predicate P1("A", "1",       bslma::Default::globalAllocator());
const ball::Predicate P2("A", 1,         bslma::Default::globalAllocator());
const ball::Predicate P3("A", (Int64)1,  bslma::Default::globalAllocator());
const ball::Predicate P4("",  "",        bslma::Default::globalAllocator());
const ball::Predicate P5("B", INT_MAX,   bslma::Default::globalAllocator());
const ball::Predicate P6("B", LLONG_MAX, bslma::Default::globalAllocator());
const ball::Predicate P7("C", LLONG_MIN, bslma::Default::globalAllocator());
const ball::Predicate P8("a", 1,         bslma::Default::globalAllocator());
const ball::Predicate P9("a", (Int64)1,  bslma::Default::globalAllocator());

const ball::Predicate PREDICATES[] = {
    P1, P2, P3, P4, P5, P6, P7, P8, P9
};

const int NUM_PREDICATES = sizeof PREDICATES / sizeof *PREDICATES;

ball::Rule mR0("",    0,   0,  0,  0, bslma::Default::globalAllocator());
ball::Rule mR1("eq",  16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR2("eq",  16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR3("eq",  16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR4("eq*", 16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR5("eq*", 16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR6("eq*", 16, 32, 48, 64, bslma::Default::globalAllocator());
ball::Rule mR7("eq",  64, 48, 32, 16, bslma::Default::globalAllocator());
ball::Rule mR8("eq",  64, 48, 32, 16, bslma::Default::globalAllocator());

const ball::Rule& R0 = mR0;
const ball::Rule& R1 = mR1;
const ball::Rule& R2 = mR2;
const ball::Rule& R3 = mR3;
const ball::Rule& R4 = mR4;
const ball::Rule& R5 = mR5;
const ball::Rule& R6 = mR6;
const ball::Rule& R7 = mR7;
const ball::Rule& R8 = mR8;

const ball::Rule* RULES[] = {
    &R0, &R1, &R2, &R3, &R4, &R5, &R6, &R7, &R8
};

const int NUM_RULES = sizeof RULES / sizeof *RULES;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bool verifyRuleSet(const ball::RuleSet& ruleSet)
    // Return 'true' if, for every rule in the set, 'ruleId' retrieves the
    // correct id and if the number of rules in the set is consistent with
    // that returned by 'numRules', and 'false' otherwise.
{
    int count = 0;
    for (int i = 0; i < ruleSet.maxNumRules(); i++) {
        const ball::Rule* rule = ruleSet.getRuleById(i);
        if (rule) {
            ++count;
            if (ruleSet.ruleId(*rule) != i) {
                return false;                                         // RETURN
            }
        }
    }

    return count == ruleSet.numRules();
}

bool compareText(bslstl::StringRef lhs,
                 bslstl::StringRef rhs,
                 bsl::ostream&     errorStream = bsl::cout)
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
        unsigned int i = lhs.length();
        errorStream << "lhs: \"" << lhs << "\"\n"
                    << "rhs: \"" << rhs << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "lhs[i] = END-OF-STRING "
                    << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                    << endl;
        return false;                                                 // RETURN

    }
    if (lhs.length() > rhs.length()) {
        unsigned int i = rhs.length();
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
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'ball::Rule' object for testing.
// They interpret a given 'spec' (from left to right) to configure the
// predicate set according to a custom language.
//
// The specification for a rule set specifies a number of 'ball::Rule' objects
// that will be added to the rule set sequentially.  Each 'ball::Rule' object
// is indicated by the character 'R' followed by another character that
// denotes the position of the rule in the global rule array 'RULES' defined
// above (i.e., 'R0' is the first rule in the array, 'R1' is the second, and
// so on).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <ACTION> *
//
// <ACTION>     ::= <RULE>
//
// <RULE>       ::= 'R' [ '0' .. '0' + NUM_RULES - 1 ]
//
// Spec String      Description
// -----------      -----------------------------------------------------------
// ""               Has no effect; leaves the object unaltered.
// "R0"             Produces: { R0 }
// "R0R1R2"         Produces: { R1 R2 R3 }
//-----------------------------------------------------------------------------

static Obj& gg(Obj *obj, const char *spec)
{
    const char *name;
    while (*spec) {
        const char c = *spec;
        ++spec;
        switch (c) {
          case 'r': {
            char *next;
            int start = (int)strtol(spec, &next, 10);
            spec = next;
            spec++;
            int end = (int)strtol(spec, &next, 10);
            spec = next;
            for (int i = start; i < end; i++) {
                ball::Rule rule("", 0, 0, 0, 0);

                // add a subset of predicates to 'rule' so that each 'j'
                // corresponds to a unique rule.
                for (int j = 0; j < NUM_PREDICATES; ++j) {
                    if ( i & (1 << j)) {
                        rule.addPredicate(PREDICATES[j]);
                    }
                }

                obj->addRule(rule);
            }
          } break;
          case 'R': {
            obj->addRule(*RULES[(int)(*spec) - '0']);
            spec++;
          } break;
          default: {
            ASSERT(c);
          }
        }
    };

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

    mR1.addPredicate(P1);
    mR2.addPredicate(P1);
    mR2.addPredicate(P2);
    mR3.addPredicate(P1);
    mR3.addPredicate(P2);
    mR3.addPredicate(P3);
    mR4.addPredicate(P1);
    mR5.addPredicate(P1);
    mR5.addPredicate(P2);
    mR6.addPredicate(P1);
    mR6.addPredicate(P2);
    mR6.addPredicate(P3);
    mR8.addPredicate(P1);
    mR8.addPredicate(P2);
    mR8.addPredicate(P3);
    mR8.addPredicate(P4);
    mR8.addPredicate(P5);
    mR8.addPredicate(P6);
    mR8.addPredicate(P7);
    mR8.addPredicate(P8);
    mR8.addPredicate(P9);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
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

///Usage
///-----
// The following code fragments illustrate how to use a rule set.
//
// We first create a rule whose pattern is 'WEEKEND*' and whose threshold
// levels are all 'ball::Severity::e_OFF' except the 'pass-through' level.  A
// 'pass-through' level of 'ball::Severity::e_INFO' indicates that whenever the
// rule is active and the severity equals or exceeds 'ball::Severity::e_INFO',
// log records will be passed to the observer:
//..
    ball::Rule rule1("WEEKEND*",               // pattern
                    ball::Severity::e_OFF,     // record level
                    ball::Severity::e_INFO,    // pass-through level
                    ball::Severity::e_OFF,     // trigger level
                    ball::Severity::e_OFF);    // triggerAll level
//..
// Next, we create another rule having a different pattern, but the same
// threshold levels:
//..
    ball::Rule rule2("WEEKDAY*",               // pattern
                    ball::Severity::e_OFF,     // record level
                    ball::Severity::e_INFO,    // pass-through level
                    ball::Severity::e_OFF,     // trigger level
                    ball::Severity::e_OFF);    // triggerAll level
//..
// We then create a 'ball::RuleSet' object, add the two rules, and verify that
// rules were added correctly:
//..
    ball::RuleSet ruleSet;
    ASSERT(0 <= ruleSet.addRule(rule1));
    ASSERT(0 <= ruleSet.addRule(rule2));
    ASSERT(2 == ruleSet.numRules());
//..
// Duplicate rules cannot be added:
//..
    ASSERT(-1 == ruleSet.addRule(rule1));
    ASSERT(-1 == ruleSet.addRule(rule2));
    ASSERT( 2 == ruleSet.numRules());
//..
// Rules in a rule set can be looked up by the 'ruleId' method:
//..
    int i1 = ruleSet.ruleId(rule1);
    int i2 = ruleSet.ruleId(rule2);
    ASSERT(0 <= i1); ASSERT(i1 < ruleSet.maxNumRules());
    ASSERT(0 <= i2); ASSERT(i2 < ruleSet.maxNumRules());
    ASSERT(i1 != i2);
//..
// The 'removeRule' method can be used to remove rules from a rule set.
//..
    ASSERT(ruleSet.removeRule(rule1));
    ASSERT(1 == ruleSet.numRules());
    ASSERT(ruleSet.ruleId(rule1) < 0);
    ASSERT(ruleSet.ruleId(rule2) == i2);
//..

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING NON-PRIMARY MANIPULATORS
        //   Non-primary manipulators such as 'addRules', 'deleteRules', and
        //   'removeRuleById' should work as expected.
        //
        // Plan:
        //   For each method to be tested, specify a table each entry of which
        //   contains the specification of the objects to be manipulated, the
        //   argument to be passed to the method, the specification of the
        //   resultant object, and the expected returned value.
        //
        // Testing:
        //   int addRules(const ball::RuleSet& rules);
        //   int removeRuleById(int id);
        //   int removeRules(const ball::RuleSet& rules);
        //   int removeAllRules();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Non-Primary Manipulators"
                          << "\n================================" << endl;

        if (verbose) cout << "\nTesting 'addRules." << endl;

        static const struct {
            int         d_line;       // source line number
            const char *d_spec1;      // spec for first rule set
            const char *d_spec2;      // spec for second rule set
            const char *d_spec3;      // spec for third rule set
            int         d_result;     // value returned by addRules
        } ADATA[] = {
            // line spec1           spec2           spec3           result
            // ---- -----           -----           -----           ------
            {  L_, "",              "",             "",             0       },

            {  L_, "R0",            "",             "R0",           0       },
            {  L_, "R0",            "R0",           "R0",           0       },
            {  L_, "",              "R0",           "R0",           1       },

            {  L_, "R0R1",          "",             "R0R1",         0       },
            {  L_, "R0R1",          "R0",           "R0R1",         0       },
            {  L_, "R0R1",          "R1",           "R0R1",         0       },
            {  L_, "R0R1",          "R0R1",         "R0R1",         0       },
            {  L_, "R0",            "R1",           "R0R1",         1       },
            {  L_, "R1",            "R0",           "R0R1",         1       },
            {  L_, "",              "R0R1",         "R0R1",         2       },

            {  L_, "R0R1R2",        "",             "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R0",           "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R1",           "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R2",           "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R0R1",         "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R2R1",         "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R0R2",         "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R0R1R2",       "R0R1R2",       0       },
            {  L_, "R0R1",          "R2",           "R0R1R2",       1       },
            {  L_, "R0R1",          "R0R2",         "R0R1R2",       1       },
            {  L_, "R0R1",          "R1R2",         "R0R1R2",       1       },
            {  L_, "R0R1",          "R0R1R2",       "R0R1R2",       1       },
            {  L_, "R1R2",          "R0",           "R0R1R2",       1       },
            {  L_, "R1R2",          "R1R0",         "R0R1R2",       1       },
            {  L_, "R1R2",          "R2R0",         "R0R1R2",       1       },
            {  L_, "R1R2",          "R1R2R0",       "R0R1R2",       1       },
            {  L_, "R0R2",          "R1",           "R0R1R2",       1       },
            {  L_, "R0R2",          "R0R1",         "R0R1R2",       1       },
            {  L_, "R0R2",          "R2R1",         "R0R1R2",       1       },
            {  L_, "R0R2",          "R0R2R1",       "R0R1R2",       1       },
            {  L_, "R0",            "R1R2",         "R0R1R2",       2       },
            {  L_, "R0",            "R0R1R2",       "R0R1R2",       2       },
            {  L_, "R1",            "R0R2",         "R0R1R2",       2       },
            {  L_, "R1",            "R1R0R2",       "R0R1R2",       2       },
            {  L_, "R2",            "R0R1",         "R0R1R2",       2       },
            {  L_, "R2",            "R0R1R2",       "R0R1R2",       2       },
            {  L_, "",              "R0R1R2",       "R0R1R2",       3       },

            // testing maximum capacity
            {  L_, "",              "r0:32",        "r0:32",        32      },

            {  L_, "r0:1",          "r1:32",        "r0:32",        31      },
            {  L_, "r0:1",          "r0:32",        "r0:32",        31      },

            {  L_, "r0:2",          "r2:32",        "r0:32",        30      },
            {  L_, "r0:2",          "r0:32",        "r0:32",        30      },

            {  L_, "r0:31",         "r31:31",       "r0:31",        0       },
            {  L_, "r0:31",         "r31:32",       "r0:32",        1       },
            {  L_, "r0:31",         "r0:31",        "r0:31",        0       },
            {  L_, "r0:31",         "r30:31",       "r0:31",        0       },
            {  L_, "r0:31",         "r30:32",       "r0:32",        1       },
            {  L_, "r0:31",         "r30:62",       "r0:31r61:62",  1       },

            {  L_, "r0:32",         "r0:32",        "r0:32",        0       },
            {  L_, "r0:32",         "r0:33",        "r0:32",        0       },
            {  L_, "r0:32",         "r31:32",       "r0:32",        0       },
            {  L_, "r0:32",         "r32:33",       "r0:32",        0       },
            {  L_, "r0:32",         "r32:64",       "r0:32",        0       },
        };

        const int NUM_ADATA = sizeof ADATA / sizeof *ADATA;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in T." << endl;

        for (int i = 0; i < NUM_ADATA; ++i) {
            const int LINE = ADATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, ADATA[i].d_spec1));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE, &mY == &gg(&mY, ADATA[i].d_spec2));

            Obj mZ; const Obj& Z = mZ;
            LOOP_ASSERT(LINE, &mZ == &gg(&mZ, ADATA[i].d_spec3));

            if (veryVerbose) {
                P_(LINE);
                P_(ADATA[i].d_spec1);
                P_(ADATA[i].d_spec2);
                P_(ADATA[i].d_spec3);
                P(ADATA[i].d_result);
            }

            int r = mX.addRules(Y);

            LOOP_ASSERT(LINE, Z == X);
            LOOP_ASSERT(LINE, r == ADATA[i].d_result);
        }

        if (verbose) cout << "\nTesting 'removeRules'." << endl;

        static const struct {
            int         d_line;       // source line number
            const char *d_spec1;      // spec for first rule set
            const char *d_spec2;      // spec for second rule set
            const char *d_spec3;      // spec for third rule set
            int         d_result;     // value returned by removeRules
        } RDATA[] = {
            // line spec1           spec2           spec3           result
            // ---- -----           -----           -----           ------
            {  L_, "",              "",             "",             0       },

            {  L_, "R0",            "",             "R0",           0       },
            {  L_, "R0",            "R0",           "",             1       },
            {  L_, "",              "R0",           "",             0       },

            {  L_, "R0R1",          "",             "R0R1",         0       },
            {  L_, "R0R1",          "R0",           "R1",           1       },
            {  L_, "R0R1",          "R1",           "R0",           1       },
            {  L_, "R0R1",          "R0R1",         "",             2       },
            {  L_, "R0",            "R1",           "R0",           0       },
            {  L_, "R1",            "R0",           "R1",           0       },
            {  L_, "",              "R0R1",         "",             0       },

            {  L_, "R0R1R2",        "",             "R0R1R2",       0       },
            {  L_, "R0R1R2",        "R0",           "R1R2",         1       },
            {  L_, "R0R1R2",        "R1",           "R0R2",         1       },
            {  L_, "R0R1R2",        "R2",           "R0R1",         1       },
            {  L_, "R0R1R2",        "R0R1",         "R2",           2       },
            {  L_, "R0R1R2",        "R2R1",         "R0",           2       },
            {  L_, "R0R1R2",        "R0R2",         "R1",           2       },
            {  L_, "R0R1R2",        "R0R1R2",       "",             3       },
            {  L_, "R0R1",          "R2",           "R0R1",         0       },
            {  L_, "R0R1",          "R0R2",         "R1",           1       },
            {  L_, "R0R1",          "R1R2",         "R0",           1       },
            {  L_, "R0R1",          "R0R1R2",       "",             2       },
            {  L_, "R1R2",          "R0",           "R1R2",         0       },
            {  L_, "R1R2",          "R1R0",         "R2",           1       },
            {  L_, "R1R2",          "R2R0",         "R1",           1       },
            {  L_, "R1R2",          "R1R2R0",       "",             2       },
            {  L_, "R0R2",          "R1",           "R0R2",         0       },
            {  L_, "R0R2",          "R0R1",         "R2",           1       },
            {  L_, "R0R2",          "R2R1",         "R0",           1       },
            {  L_, "R0R2",          "R0R2R1",       "",             2       },
            {  L_, "R0",            "R1R2",         "R0",           0       },
            {  L_, "R0",            "R0R1R2",       "",             1       },
            {  L_, "R1",            "R0R2",         "R1",           0       },
            {  L_, "R1",            "R1R0R2",       "",             1       },
            {  L_, "R2",            "R0R1",         "R2",           0       },
            {  L_, "R2",            "R0R1R2",       "",             1       },
            {  L_, "",              "R0R1R2",       "",             0       },

            {  L_, "r0:31",         "r30:31",       "r0:30",        1       },
            {  L_, "r0:31",         "r0:1",         "r1:31",        1       },
            {  L_, "r0:31",         "r0:31",        "",             31      },

            {  L_, "r0:32",         "r31:32",       "r0:31",        1       },
            {  L_, "r0:32",         "r0:1",         "r1:32",        1       },
            {  L_, "r0:32",         "r0:32",        "",             32      },
        };

        const int NUM_RDATA = sizeof RDATA / sizeof *RDATA;

        for (int i = 0; i < NUM_RDATA; ++i) {
            const int LINE = RDATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, RDATA[i].d_spec1));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE, &mY == &gg(&mY, RDATA[i].d_spec2));

            Obj mZ; const Obj& Z = mZ;
            LOOP_ASSERT(LINE, &mZ == &gg(&mZ, RDATA[i].d_spec3));

            if (veryVerbose) {
                P_(LINE);
                P_(RDATA[i].d_spec1);
                P_(RDATA[i].d_spec2);
                P_(RDATA[i].d_spec3);
                P(RDATA[i].d_result);
            }

            int r = mX.removeRules(Y);

            LOOP_ASSERT(LINE, Z == X);
            LOOP_ASSERT(LINE, r == RDATA[i].d_result);
        }

        if (verbose) cout << "\nTesting 'removeRuleById." << endl;

        static const struct {
            int         d_line;       // source line number
            const char *d_spec1;      // spec for first rule set
            int         d_id;         // the id of the rule to be removed
            const char *d_spec2;      // spec for second rule set
            int         d_result;     // value returned by removeRuleById
        } IDATA[] = {
            // line spec1           id           spec2           result
            // ---- -----           -----           -----           ------
            {  L_, "",              0,              "",             0       },
            {  L_, "",              1,              "",             0       },
            {  L_, "",              30,             "",             0       },
            {  L_, "",              31,             "",             0       },

            {  L_, "R0",            0,              "R0",           0       },
            {  L_, "R0",            1,              "R0",           0       },
            {  L_, "R0",            30,             "R0",           0       },
            {  L_, "R0",            31,             "",             1       },

            {  L_, "R0R1",          0,              "R0R1",         0       },
            {  L_, "R0R1",          1,              "R0R1",         0       },
            {  L_, "R0R1",          30,             "R0",           1       },
            {  L_, "R0R1",          31,             "R1",           1       },

            {  L_, "R0R1R2",        0,              "R0R1R2",       0       },
            {  L_, "R0R1R2",        1,              "R0R1R2",       0       },
            {  L_, "R0R1R2",        2,              "R0R1R2",       0       },
            {  L_, "R0R1R2",        29,             "R0R1",         1       },
            {  L_, "R0R1R2",        30,             "R0R2",         1       },
            {  L_, "R0R1R2",        31,             "R1R2",         1       },

            {  L_, "r0:31",         0,              "r0:31",        0       },
            {  L_, "r0:31",         1,              "r0:30",        1       },
            {  L_, "r0:31",         30,             "r0:1r2:31",    1       },
            {  L_, "r0:31",         31,             "r1:31",        1       },

            {  L_, "r0:32",         0,              "r0:31",        1       },
            {  L_, "r0:32",         1,              "r0:30r31:32",  1       },
            {  L_, "r0:32",         30,             "r0:1:r2:32",   1       },
            {  L_, "r0:32",         31,             "r1:32",        1       },
        };

        const int NUM_IDATA = sizeof IDATA / sizeof *IDATA;

        for (int i = 0; i < NUM_IDATA; ++i) {
            const int LINE = IDATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, IDATA[i].d_spec1));

            Obj mY; const Obj& Y = mY;
            LOOP_ASSERT(LINE, &mY == &gg(&mY, IDATA[i].d_spec2));

            if (veryVerbose) {
                P_(LINE);
                P_(IDATA[i].d_spec1);
                P_(IDATA[i].d_id);
                P_(IDATA[i].d_spec2);
                P(IDATA[i].d_result);
            }

            int r = mX.removeRuleById(IDATA[i].d_id);

            LOOP_ASSERT(LINE, Y == X);
            LOOP_ASSERT(LINE, r == IDATA[i].d_result);
        }
        if (verbose) cout << "\nTesting 'removeAllRules'." << endl;

        static const char* SPECS[] = {
            "",
            "R0",
            "R1",
            "R2",
            "R3",
            "R4",
            "R5",
            "R6",
            "R7",
            "R8",

            "R0R1",
            "R1R2",
            "R2R3",
            "R3R4",
            "R4R5",

            "R0R1R2",
            "R1R2R3",
            "R2R3R4",
            "R3R4R5",
            "R4R5R6",

            "R0R1R2R3",
            "R0R1R2R3R4"
            "R0R1R2R3R4R5",
            "R0R1R2R3R4R5R6",
            "R0R1R2R3R4R5R6R7",
            "R0R1R2R3R4R5R6R7R8",

            "r0:0",
            "r0:1",
            "r0:2",
            "r0:4",
            "r0:8",
            "r0:16",
            "r0:31",
            "r0:32",
            "r0:33",
            "r0:64",

            "r0:0",
            "r1:2",
            "r2:4",
            "r4:8",
            "r8:16",
            "r16:32",
            "r32:63",
            "r32:64",
            "r32:65",
            "r32:96"
        };

        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPECS; ++i) {
        for (int j = 0; j < NUM_SPECS; ++j) {

            if (veryVerbose) { P_(i); P_(j); P_(SPECS[i]); P(SPECS[j]); }

            Obj mX; const Obj& X = mX;
            LOOP2_ASSERT(i, j, &mX == &gg(&mX, SPECS[i]));

            Obj mY; const Obj& Y = mY;
            LOOP2_ASSERT(i, j, &mY == &gg(&mY, SPECS[j]));

            mX.removeAllRules();

            LOOP2_ASSERT(i, j, 0 == X.numRules());

            LOOP2_ASSERT(i, j, &mX == &gg(&mX, SPECS[j]));

            LOOP2_ASSERT(i, j, X == Y);
            LOOP2_ASSERT(i, j, !(X != Y));
        }
        }

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
        //   const ball::Rule& operator=(const ball::Rule& other);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n==========================" << endl;

        static const struct {
            int         d_line;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line    spec
            // ----    ----
            {  L_,    "",                           },
            {  L_,    "R0",                         },
            {  L_,    "R1",                         },
            {  L_,    "R2",                         },
            {  L_,    "R3",                         },
            {  L_,    "R4",                         },
            {  L_,    "R5",                         },
            {  L_,    "R6",                         },
            {  L_,    "R7",                         },
            {  L_,    "R8",                         },

            {  L_,    "R0R1",                       },
            {  L_,    "R1R2",                       },
            {  L_,    "R2R3",                       },
            {  L_,    "R3R4",                       },
            {  L_,    "R4R5",                       },

            {  L_,    "R0R1R2",                     },
            {  L_,    "R1R2R3",                     },
            {  L_,    "R2R3R4",                     },
            {  L_,    "R3R4R5",                     },
            {  L_,    "R4R5R6",                     },

            {  L_,    "R0R1R2R3",                   },
            {  L_,    "R0R1R2R3R4",                 },
            {  L_,    "R0R1R2R3R4R5",               },
            {  L_,    "R0R1R2R3R4R5R6",             },
            {  L_,    "R0R1R2R3R4R5R6R7",           },
            {  L_,    "R0R1R2R3R4R5R6R7R8",         },
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
        //   Void for 'ball::RuleSet'.
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
        //   ball::Rule(const ball::Rule&, bdema::Alct * = 0);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        static const struct {
            int         d_line;      // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line    spec
            // ----    ----
            {  L_,    "",                           },
            {  L_,    "R0",                         },
            {  L_,    "R1",                         },
            {  L_,    "R2",                         },
            {  L_,    "R3",                         },
            {  L_,    "R4",                         },
            {  L_,    "R5",                         },
            {  L_,    "R6",                         },
            {  L_,    "R7",                         },
            {  L_,    "R8",                         },

            {  L_,    "R0R1",                       },
            {  L_,    "R1R2",                       },
            {  L_,    "R2R3",                       },
            {  L_,    "R3R4",                       },
            {  L_,    "R4R5",                       },

            {  L_,    "R0R1R2",                     },
            {  L_,    "R1R2R3",                     },
            {  L_,    "R2R3R4",                     },
            {  L_,    "R3R4R5",                     },
            {  L_,    "R4R5R6",                     },

            {  L_,    "R0R1R2R3",                   },
            {  L_,    "R0R1R2R3R4",                 },
            {  L_,    "R0R1R2R3R4R5",               },
            {  L_,    "R0R1R2R3R4R5R6",             },
            {  L_,    "R0R1R2R3R4R5R6R7",           },
            {  L_,    "R0R1R2R3R4R5R6R7R8",         },
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
                bslma::TestAllocator testAllocatorY(veryVeryVerbose);
                Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                LOOP_ASSERT(LINE, Y == W);
                LOOP_ASSERT(LINE, W == Y);
                LOOP_ASSERT(LINE, X == W);
                LOOP_ASSERT(LINE, W == X);
            }

            // construct y with an allocator and exceptions
            {
                bslma::TestAllocator testAllocatorY(veryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mY(X, &testAllocatorY); const Obj& Y = mY;

                    LOOP_ASSERT(LINE, Y == W);
                    LOOP_ASSERT(LINE, W == Y);
                    LOOP_ASSERT(LINE, X == W);
                    LOOP_ASSERT(LINE, W == X);
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
        //   predicates were added in different orders).  For each element (u,
        //   v) in T, verify that 'operator==' and 'operator!=' return the
        //   correct value.

        // Testing:
        //   bool operator==(const ball::Rule& lhs, const ball::Rule& rhs)
        //   bool operator!=(const ball::Rule& lhs, const ball::Rule& rhs)
        // --------------------------------------------------------------------

        static const struct {
            int         d_line;      // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
        } DATA[] = {
            // line   spec
            // ----   ----
            {  L_,    "",                           },
            {  L_,    "R0",                         },
            {  L_,    "R1",                         },
            {  L_,    "R2",                         },
            {  L_,    "R3",                         },
            {  L_,    "R4",                         },
            {  L_,    "R5",                         },
            {  L_,    "R6",                         },
            {  L_,    "R7",                         },
            {  L_,    "R8",                         },

            {  L_,    "R0R1",                       },
            {  L_,    "R1R2",                       },
            {  L_,    "R2R3",                       },
            {  L_,    "R3R4",                       },
            {  L_,    "R4R5",                       },

            {  L_,    "R0R1R2",                     },
            {  L_,    "R1R2R3",                     },
            {  L_,    "R2R3R4",                     },
            {  L_,    "R3R4R5",                     },
            {  L_,    "R4R5R6",                     },

            {  L_,    "R0R1R2R3",                   },
            {  L_,    "R0R1R2R3R4",                 },
            {  L_,    "R0R1R2R3R4R5",               },
            {  L_,    "R0R1R2R3R4R5R6",             },
            {  L_,    "R0R1R2R3R4R5R6R7",           },
            {  L_,    "R0R1R2R3R4R5R6R7R8",         },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;

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

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in T." << endl;

        static const struct {
            int         d_line;       // source line number
            const char *d_spec1;      // input 'spec' string for 'gg'
            const char *d_spec2;      // input 'spec' string for 'gg'
        } TDATA[] = {
            // line spec1                      spec2
            // ---- -----                      -----
            {  L_, "",                         ""                    },

            {  L_, "R1",                       "R1"                  },

            {  L_, "R1R2",                     "R2R1"                },
            {  L_, "R1R1R2R2",                 "R2R1"                },
            {  L_, "R1R2R1R2",                 "R2R1"                },

            {  L_, "R1R2R3",                   "R1R3R2"              },
            {  L_, "R1R2R3",                   "R2R1R3"              },
            {  L_, "R1R2R3",                   "R2R3R1"              },
            {  L_, "R1R2R3",                   "R3R1R2"              },
            {  L_, "R1R2R3",                   "R3R2R1"              },
            {  L_, "R1R2R3",                   "R3R2R1R1R2R3"        },

            {  L_, "R0R1R2R3R4",               "R4R3R2R1R0"          },
            {  L_, "R0R1R2R3R4R5",             "R5R4R3R2R1R0"        },
            {  L_, "R0R1R2R3R4R5R6",           "R6R5R4R3R2R1R0"      },
            {  L_, "R0R1R2R3R4R5R6R7",         "R7R6R5R4R3R2R1R0"    },
            {  L_, "R0R1R2R3R4R5R6R7R8",       "R8R7R6R5R4R3R2R1R0"  },
        };

        const int NUM_TDATA = sizeof TDATA / sizeof *TDATA;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in T." << endl;

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
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::PS&) const;
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
 // line spec      expected output
 // ---- ----      ---------------
 {  L_,  "R1",     "[ [ pattern = \"eq\" "
                   "thresholds = [    16    32    48    64   ] "
                   " predicateSet = [  [ \"A\" = 1 ] ] ] ]" },
 {  L_,  "R1R2R3", "[ [ pattern = \"eq\" "
                   "thresholds = [    16    32    48    64   ] "
                   " predicateSet = [  [ \"A\" = 1 ]  [ \"A\" = 1 ] "
                   " [ \"A\" = 1 ] ] ] "
                   "[ pattern = \"eq\" "
                   "thresholds = [    16    32    48    64   ]  "
                   "predicateSet = [  [ \"A\" = 1 ] "
                   " [ \"A\" = 1 ] ] ] "
                   "[ pattern = \"eq\" "
                   "thresholds = [    16    32    48    64   ]  "
                   "predicateSet = [  [ \"A\" = 1 ] ] ] ]"                  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(LINE, &mX == &gg(&mX, DATA[i].d_spec));

            ostringstream os;
            os << X;

            LOOP_ASSERT(LINE, compareText(os.str(), DATA[i].d_output));

            if (veryVerbose) {
                P_(LINE);
                P_(DATA[i].d_spec);
                P_(os.str());
                P(DATA[i].d_output);
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
    // line spec     level space expected
    // ---- ----     ----- ----- -----------------------
 {  L_,  "R1R2R3",    1,   2,    "  [\n"
                                 "    [\n"
                                 "      pattern = \"eq\"\n"
                                 "      thresholds = [\n"
                                 "        16\n"
                                 "        32\n"
                                 "        48\n"
                                 "        64\n"
                                 "      ]\n"
                                 "      predicateSet = [\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "      ]\n"
                                 "    ]\n"
                                 "    [\n"
                                 "      pattern = \"eq\"\n"
                                 "      thresholds = [\n"
                                 "        16\n"
                                 "        32\n"
                                 "        48\n"
                                 "        64\n"
                                 "      ]\n"
                                 "      predicateSet = [\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "      ]\n"
                                 "    ]\n"
                                 "    [\n"
                                 "      pattern = \"eq\"\n"
                                 "      thresholds = [\n"
                                 "        16\n"
                                 "        32\n"
                                 "        48\n"
                                 "        64\n"
                                 "      ]\n"
                                 "      predicateSet = [\n"
                                 "                  [ \"A\" = 1 ]\n"
                                 "      ]\n"
                                 "    ]\n"
                                 "  ]\n"
 },
 {  L_,  "R1R2R3",    -1,   -2,
    "[ [ pattern = \"eq\" "
    "thresholds = [         16         32         48         64       ] "
    " predicateSet = [  [ \"A\" = 1 ]  [ \"A\" = 1 ]  [ \"A\" = 1 ] ] ] "
    "[ pattern = \"eq\" "
    "thresholds = [         16         32         48         64       ] "
    " predicateSet = [  [ \"A\" = 1 ]  [ \"A\" = 1 ] ] ] "
    "[ pattern = \"eq\" "
    "thresholds = [         16         32         48         64       ] "
    " predicateSet = [  [ \"A\" = 1 ] ] ] ]" }
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
                P_(os.str());
                P(PDATA[i].d_output);
            }

            LOOP_ASSERT(LINE, compareText(os.str(), PDATA[i].d_output));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Rules added to a 'bal_RuleSet' object must be verifiable by the
        //   basic accessors.
        //
        // Plan:
        //   Specify a set P of unique 'ball::Predicate' objects.  Construct a
        //   set of R of 'ball::Rule' objects using the superset of P.  Then
        //   add elements of R to a rule set one by one, and verify that
        //   'addRule' returns 1 if the number of existing rules is less than
        //   'maxNumRules'.  After having adding each rule, for every existing
        //   rule check that the same rule cannot be added, 'getRuleById'
        //   returns the same rule, and 'ruleId' returns the correct id.
        //   Repeat the verification with another 'ball::RuleSet' object
        //   copy-constructed from the original and with yet another
        //   'ball::RuleSet' object assigned from the original.
        //
        // Testing:
        //   static int maxNumRules();
        //   int ruleId(const ball::Rule& value) const;
        //   const ball::Rule *getRuleById(int id) const;
        //   int numRules() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing basic accessors" << endl
                          << "=======================" << endl;

        // calculate n = 2 ** NUM_PREDICATES
        int n = 1 << NUM_PREDICATES;

        Obj mX; const Obj& X = mX;
        vector<int> indices(Obj::maxNumRules());

        // use 2 * maxNumRules() + 1 as the number of trials to save time
        ASSERT(n >= 2 * Obj::maxNumRules() + 1);

        for (int i = 0; i < 2 * Obj::maxNumRules() + 1; ++i) {

            ball::Rule rule1("", 0, 0, 0, 0);

            // add a subset of predicates to 'rule1' so that each 'j'
            // corresponds to a unique rule.
            for (int j = 0; j < NUM_PREDICATES; ++j) {
                if ( i & (1 << j)) {
                    rule1.addPredicate(PREDICATES[j]);
                }
            }

            if (i < Obj::maxNumRules()) {
                LOOP_ASSERT(i, X.numRules() == i);
                indices[i] = mX.addRule(rule1);
                LOOP_ASSERT(i, indices[i] >= 0);
                LOOP_ASSERT(i, indices[i] == X.ruleId(rule1));
                LOOP_ASSERT(i, X.numRules() == i + 1);
            }
            else
            {
                LOOP_ASSERT(i, X.numRules() == X.maxNumRules());
                LOOP_ASSERT(i, -2 == mX.addRule(rule1));
                LOOP_ASSERT(i, X.numRules() == X.maxNumRules());
            }

            // check every rule already in the set
            for (int j = 0; j <= i && j < Obj::maxNumRules(); ++j) {

                ball::Rule rule2("", 0, 0, 0, 0);

                for (int k = 0; k < NUM_PREDICATES; ++k) {
                    if ( j & (1 << k)) {
                        rule2.addPredicate(PREDICATES[k]);
                    }
                }

                LOOP2_ASSERT(i, j, -1 == mX.addRule(rule2));
                LOOP2_ASSERT(i, j, indices[j] == X.ruleId(rule2));
                LOOP2_ASSERT(i, j, X.getRuleById(indices[j]));
                LOOP2_ASSERT(i, j, rule2 == *X.getRuleById(indices[j]));

                // test a rule set created by copy ctor
                Obj mY(X); const Obj& Y = mY;
                LOOP2_ASSERT(i, j, -1 == mY.addRule(rule2));
                LOOP2_ASSERT(i, j, 0 <= Y.ruleId(rule2));
                LOOP2_ASSERT(i, j, Y.getRuleById(Y.ruleId(rule2)));
                LOOP2_ASSERT(i, j,
                             rule2 == *Y.getRuleById(Y.ruleId(rule2)));

                // test a rule set created by assignment
                Obj mZ; const Obj& Z = mZ;
                mZ = X;
                LOOP2_ASSERT(i, j, -1 == mZ.addRule(rule2));
                LOOP2_ASSERT(i, j, 0 <= Z.ruleId(rule2));
                LOOP2_ASSERT(i, j, Z.getRuleById(Z.ruleId(rule2)));
                LOOP2_ASSERT(i, j,
                             rule2 == *Z.getRuleById(Z.ruleId(rule2)));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'GG'
        //
        // Plan:
        //   Test the behavior of 'gg'.
        //   * Verify that all test values are distinct.
        //   * Show 'gg' returns a correct reference to its first argument.
        //   * Verify length
        //
        // Testing:
        //   Obj& gg(Obj *address, const char *spec);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "Testing 'gg' generator function" << endl
            << "===============================" << endl;

        if (verbose) cout << "\tTesting 'R'-form specification." << endl;

        int n = 1 << NUM_RULES;

        for (int i = 0; i < n; ++i) {
            string spec;
            for (int j = 0; j < NUM_RULES; ++j) {
                if (i & 1 << j) {
                    spec += 'R';
                    spec += '0' + (char)j;
                }
            }

            Obj mX; const Obj& X = mX;
            LOOP_ASSERT(i, &mX == &gg(&mX, spec.c_str()));

            if (veryVerbose) { P_(i); P_(spec); P(X); }

            int length = 0;
            for (int j = 0; j < NUM_RULES; ++j) {
                bool exist = i & 1 << j;
                LOOP2_ASSERT(i, j, exist == X.ruleId(*RULES[j]) >= 0);
                length += exist;
            }
            LOOP_ASSERT(i, length == mX.numRules());
        }

        if (verbose) cout << "\tTesting 'r'-form specification." << endl;

        n = 1 << NUM_PREDICATES;

        for (int i = 0; i < 2 * Obj::maxNumRules(); ++i) {
        for (int j = i; j < n && j < i + 2 * Obj::maxNumRules(); ++j) {
            ostringstream spec;
            spec << 'r' << i << ':' << j;
            Obj mX(&testAllocator); const Obj& X = mX;
            LOOP_ASSERT(i, &mX == &gg(&mX, spec.str().c_str()));

            if (veryVerbose) { P_(i); P_(spec.str()); P(X) }

            for (int k = i; k < j && k < i + X.maxNumRules(); ++k) {

                ball::Rule rule("", 0, 0, 0, 0);

                for (int l = 0; l < NUM_PREDICATES; ++l) {
                    if ( k & (1 << l)) {
                        rule.addPredicate(PREDICATES[l]);
                    }
                }

                LOOP3_ASSERT(i, j, k, X.ruleId(rule) >= 0);
            }

            int length = j - i > X.maxNumRules() ? X.maxNumRules() : j - i;
            LOOP2_ASSERT(i, j, length == X.numRules());

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
        //   ball::RuleSet();
        //   int addRule(const ball::Rule& value);
        //   int removeRule(const ball::Rule& value);
        //   ~ball::RuleSet();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma::Allocator *)0);
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
            ASSERT(0 <= mY.addRule(R1));

            Obj mZ;  const Obj& Z = mZ;
            ASSERT(0 <= mZ.addRule(R2));
            ASSERT(0 <= mZ.addRule(R3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numRules());
            ASSERT(1 == Y.numRules());
            ASSERT(2 == Z.numRules());
            ASSERT(0 <= Y.ruleId(R1));
            ASSERT(0 <= Z.ruleId(R2));
            ASSERT(0 <= Z.ruleId(R3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(0 <= mX.addRule(R1));
            ASSERT(1 == X.numRules());
            ASSERT(0 <= X.ruleId(R1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeRule(R1));
            ASSERT(0 > X.ruleId(R1));
            ASSERT(0 <= mX.addRule(R2));
            ASSERT(0 <= mX.addRule(R3));
            ASSERT(2 == X.numRules());
            ASSERT(0 <= X.ruleId(R2));
            ASSERT(0 <= X.ruleId(R3));
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
            ASSERT(0 <= mY.addRule(R1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(0 <= mZ.addRule(R2));
            ASSERT(0 <= mZ.addRule(R3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numRules());
            ASSERT(1 == Y.numRules());
            ASSERT(2 == Z.numRules());
            ASSERT(0 <= Y.ruleId(R1));
            ASSERT(0 <= Z.ruleId(R2));
            ASSERT(0 <= Z.ruleId(R3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(0 <= mX.addRule(R1));
            ASSERT(1 == X.numRules());
            ASSERT(0 <= X.ruleId(R1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeRule(R1));
            ASSERT(0 > X.ruleId(R1));
            ASSERT(0 <= mX.addRule(R2));
            ASSERT(0 <= mX.addRule(R3));
            ASSERT(2 == X.numRules());
            ASSERT(0 <= X.ruleId(R2));
            ASSERT(0 <= X.ruleId(R3));
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
            ASSERT(0 <= mY.addRule(R1));

            Obj mZ(&testAllocatorZ);  const Obj& Z = mZ;
            ASSERT(0 <= mZ.addRule(R2));
            ASSERT(0 <= mZ.addRule(R3));

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(0 == (X == Y));
            ASSERT(0 == (X == Z));
            ASSERT(0 == (Y == Z));

            ASSERT(0 == X.numRules());
            ASSERT(1 == Y.numRules());
            ASSERT(2 == Z.numRules());
            ASSERT(0 <= Y.ruleId(R1));
            ASSERT(0 <= Z.ruleId(R2));
            ASSERT(0 <= Z.ruleId(R3));

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                            << endl;
            ASSERT(0 <= mX.addRule(R1));
            ASSERT(1 == X.numRules());
            ASSERT(0 <= X.ruleId(R1));
            ASSERT(X == Y);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            ASSERT(1 == mX.removeRule(R1));
            ASSERT(0 > X.ruleId(R1));
            ASSERT(0 <= mX.addRule(R2));
            ASSERT(0 <= mX.addRule(R3));
            ASSERT(2 == X.numRules());
            ASSERT(0 <= X.ruleId(R2));
            ASSERT(0 <= X.ruleId(R3));
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
        //   operator without [9, 10] and with [11] aliasing.  Use the direct
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values.
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

        int c1, c2, c3, c4, c5, c6;

        Obj mX1;  const Obj& X1 = mX1;

        ASSERT(0 == X1.numRules());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;

        Obj mX2(X1);  const Obj& X2 = mX2;

        ASSERT(0 == X2.numRules());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 3. Set x1 to VA." << endl;

        c1 = mX1.addRule(R1);
        ASSERT(0 <= c1);

        ASSERT(1 == X1.numRules());
        ASSERT(c1 == X1.ruleId(R1));
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VA." << endl;

        c1 = mX2.addRule(R1);
        ASSERT(0 <= c1);

        ASSERT(1 == X2.numRules());
        ASSERT(c1 == X2.ruleId(R1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VB." << endl;

        ASSERT(1 == mX2.removeRule(R1));
        ASSERT(0 == X2.numRules());
        c2 = mX2.addRule(R2);
        c3 = mX2.addRule(R3);
        ASSERT(c2 == X2.ruleId(R2));
        ASSERT(c3 == X2.ruleId(R3));

        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to 0." << endl;

        ASSERT(1 == mX1.removeRule(R1));
        ASSERT(0 == X1.numRules());

        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VC)."
                           << endl;

        Obj mX3;  const Obj& X3 = mX3;
        c4 = mX3.addRule(R4);
        c5 = mX3.addRule(R5);
        c6 = mX3.addRule(R6);
        ASSERT(c4 == X3.ruleId(R4));
        ASSERT(c5 == X3.ruleId(R5));
        ASSERT(c6 == X3.ruleId(R6));

        ASSERT(3 == X3.numRules());
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;

        Obj mX4(X1);  const Obj& X4 = mX4;

        ASSERT(0 == X4.numRules());
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;

        mX2 = X1;

        ASSERT(0 == X2.numRules());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;

        mX2 = X3;

        ASSERT(3 == X2.numRules());
        ASSERT(0 <= X2.ruleId(R4));
        ASSERT(0 <= X2.ruleId(R5));
        ASSERT(0 <= X2.ruleId(R6));
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;

        mX1 = X1;

        ASSERT(0 == X1.numRules());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
        } break;

      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    };

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
