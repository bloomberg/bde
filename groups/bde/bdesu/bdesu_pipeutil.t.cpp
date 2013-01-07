// bdesu_pipeutil.t.cpp                                               -*-C++-*-
#include <bdesu_pipeutil.h>

#include <bsls_platform.h>
#include <bdesu_pathutil.h>
#include <bdesu_fileutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

static int testStatus = 0;
static int verbose, veryVerbose, veryVeryVerbose;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
#define V(X) { if (verbose) P(X) }            // Print in verbose mode

int main(int argc, char *argv[]) {
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    switch(test) { case 0:
      case 1: {
        ///////////////////////////////////////////////////////
        // makeCanonicalName test
        //
        // makeCanonicalName() is, well, canonical.  Here we
        // re-implement its canonical behavior and ensure that
        // the function behaves that way.
        ///////////////////////////////////////////////////////

        if (verbose) {
           cout << "makeCanonicalName test" << endl;
        }

#ifdef BSLS_PLATFORM_OS_WINDOWS
        bsl::string compare("\\\\.\\pipe\\foo.bar");
#else
        bsl::string compare;
           const char* tmpdirPtr;
           if (0 != (tmpdirPtr = bsl::getenv("TMPDIR"))) {
               compare = tmpdirPtr;
           }
           else {
               bdesu_FileUtil::getWorkingDirectory(&compare);
           }

           ASSERT(0 == bdesu_PathUtil::appendIfValid(&compare, "foo.bar"));
#endif

           bsl::string name;
           ASSERT(0 == bdesu_PipeUtil::makeCanonicalName(&name, "FOO.Bar"));
           LOOP2_ASSERT(name, compare, name == compare);
           V(name);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
