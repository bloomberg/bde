// balb_xxxcommandlineutil.t.cpp                                      -*-C++-*-

#include <balb_xxxcommandlineutil.h>
#include <balb_xxxcommandline.h>  // for testing only

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                  -------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


// ============================================================================
//                     SEMI STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample1 {

void forkAndExecute(const char  *pathToScript,
                    int          numScriptArgs,
                    char       **scriptArgs)
{
    (void)pathToScript;
    (void)numScriptArgs;
    (void)scriptArgs;
}

void process(const bsl::string& configFilename)
{
    (void)configFilename;
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Multiple Argument Sets
///- - - - - - - - - - - - - - - - -
// Suppose that we need to modify an existing program so that it spawns a
// script, and that furthermore, the script processes a set of command-line
// arguments that are different from the command-line arguments specified to
// the host program.  We can simply concatenate the script's command-line
// arguments to those of the host program, separated by the string '--', and
// then parse the single command-line into two sets of arguments, one of which
// can be passed to a function that executes the script.
//..
    int main(int argc, char **argv)
    {
        bsl::vector<char *> hostArgs;
        bsl::vector<char *> scriptArgs;

        if (0 != balb::CommandLineUtil::splitCommandLineArguments(&hostArgs,
                                                                 &scriptArgs,
                                                                 argc,
                                                                 argv)) {
            // ...handle error...
        }

        // Execute the script with its own command-line arguments.

        forkAndExecute("/path/to/script", scriptArgs.size(), &scriptArgs[0]);

        // Execute the remainder of the original program.

        bsl::string                configFilename;
        balb::CommandLineOptionInfo specTable[] = {
            {
                "",
                "filename",
                "configuration file name",
                balb::CommandLineTypeInfo(&configFilename),
                balb::CommandLineOccurrenceInfo::BAEA_REQUIRED,
            },
        };

        balb::CommandLine commandLine(specTable);
        if (0 != commandLine.parse(hostArgs.size(), &hostArgs[0])) {
            commandLine.printUsage();
            bsl::exit(-1);
        }

        process(configFilename);

        return 0;
    }
//..
}  // close namespace UsageExample1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4; (void)veryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "USAGE EXAMPLE" << bsl::endl
                               << "=============" << bsl::endl;

        const char *argv[] = { "myTaskname",
                               "myFilename",
                               "--",
                               "scriptName",
                               "scriptArg0",
                               "scriptArg1",
                               "scriptArg2"
                            };
        const int   argc = sizeof(argv)/sizeof(*argv);

        UsageExample1::main(argc, const_cast<char **>(argv));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BACKWARDS COMPATIBILITY: 'bael::CommandLineUtil'
        //
        // Note that this is a duplicate of the breathing test, but uses the
        // deprecated typename 'bael::CommandLineUtil'.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING: bael::CommandLineUtil" << bsl::endl
                               << "=============================" << bsl::endl;

        char ARGV0[] = "case1";
        char ARGV1[] = "1111";
        char ARGV2[] = "2222";
        char ARGV3[] = "3333";
        char ARGV4[] = "4444";

        char * testArgs[] = {
            ARGV0,
            ARGV1,
            ARGV2,
            ARGV3,
            ARGV4
        };
        int NUM_ARGS = sizeof testArgs / sizeof *testArgs;

        bsl::vector<char *> v1;
        bsl::vector<char *> v2;

        if (veryVerbose) {
            bsl::cout << "a) left args = 0, right args = 3" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = bael::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV1);

            ASSERT(0 == rc);
            ASSERT(1 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(4 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV2, v2[1]));
            ASSERT(!bsl::strcmp(ARGV3, v2[2]));
            ASSERT(!bsl::strcmp(ARGV4, v2[3]));
        }

        if (veryVerbose) {
            bsl::cout << "b) left args = 1, right args = 2" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = bael::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV2);

            ASSERT(0 == rc);
            ASSERT(2 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(3 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV3, v2[1]));
            ASSERT(!bsl::strcmp(ARGV4, v2[2]));
        }

        if (veryVerbose) {
            bsl::cout << "c) left args = 2, right args = 1" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = bael::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV3);

            ASSERT(0 == rc);
            ASSERT(3 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(2 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV4, v2[1]));
        }

        if (veryVerbose) {
            bsl::cout << "d) left args = 3, right args = 0" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = bael::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV4);
            ASSERT(0 == rc);
            ASSERT(4 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(!bsl::strcmp(ARGV3, v1[3]));
            ASSERT(1 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
        }

        if (veryVerbose) {
            bsl::cout << "e) left args = 4, right args = 0" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = bael::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs);
            ASSERT(0 == rc);
            ASSERT(5 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(!bsl::strcmp(ARGV3, v1[3]));
            ASSERT(!bsl::strcmp(ARGV4, v1[4]));
            ASSERT(1 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test: Implement a dummy protocol and verify that we can
        // invoke appropriate functions through the base class.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

        char ARGV0[] = "case1";
        char ARGV1[] = "1111";
        char ARGV2[] = "2222";
        char ARGV3[] = "3333";
        char ARGV4[] = "4444";

        char * testArgs[] = {
            ARGV0,
            ARGV1,
            ARGV2,
            ARGV3,
            ARGV4
        };
        int NUM_ARGS = sizeof testArgs / sizeof *testArgs;

        bsl::vector<char *> v1;
        bsl::vector<char *> v2;

        if (veryVerbose) {
            bsl::cout << "a) left args = 0, right args = 3" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = balb::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV1);

            ASSERT(0 == rc);
            ASSERT(1 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(4 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV2, v2[1]));
            ASSERT(!bsl::strcmp(ARGV3, v2[2]));
            ASSERT(!bsl::strcmp(ARGV4, v2[3]));
        }

        if (veryVerbose) {
            bsl::cout << "b) left args = 1, right args = 2" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = balb::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV2);

            ASSERT(0 == rc);
            ASSERT(2 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(3 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV3, v2[1]));
            ASSERT(!bsl::strcmp(ARGV4, v2[2]));
        }

        if (veryVerbose) {
            bsl::cout << "c) left args = 2, right args = 1" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = balb::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV3);

            ASSERT(0 == rc);
            ASSERT(3 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(2 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
            ASSERT(!bsl::strcmp(ARGV4, v2[1]));
        }

        if (veryVerbose) {
            bsl::cout << "d) left args = 3, right args = 0" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = balb::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs,
                                                                     ARGV4);
            ASSERT(0 == rc);
            ASSERT(4 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(!bsl::strcmp(ARGV3, v1[3]));
            ASSERT(1 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
        }

        if (veryVerbose) {
            bsl::cout << "e) left args = 4, right args = 0" << bsl::endl
                      << "--------------------------------" << bsl::endl;
        }
        {
            bsl::vector<char*> v1;
            bsl::vector<char*> v2;
            int rc = balb::CommandLineUtil::splitCommandLineArguments(&v1,
                                                                     &v2,
                                                                     NUM_ARGS,
                                                                     testArgs);
            ASSERT(0 == rc);
            ASSERT(5 == v1.size());
            ASSERT(!bsl::strcmp("case1", v1[0]));
            ASSERT(!bsl::strcmp(ARGV1, v1[1]));
            ASSERT(!bsl::strcmp(ARGV2, v1[2]));
            ASSERT(!bsl::strcmp(ARGV3, v1[3]));
            ASSERT(!bsl::strcmp(ARGV4, v1[4]));
            ASSERT(1 == v2.size());
            ASSERT(!bsl::strcmp("case1", v2[0]));
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
