// bdesu_processutil.t.cpp                                            -*-C++-*-
#include <bdesu_processutil.h>

#include <bdesu_fileutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

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

//=============================================================================
//                  GLOBAL HELPER TYPES & CLASSES FOR TESTING
//-----------------------------------------------------------------------------

typedef bdesu_FileUtil    FUtil;
typedef bdesu_ProcessUtil Obj;

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    switch(test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // LEAKING FILE DESCRIPTORS TEST
        //
        // Concern:
        //   Reproduce bug where 'getProcessName' is leaking file descriptors.
        //
        // Plan:
        //   Call it many times and see if we run out of file descriptors.
        // --------------------------------------------------------------------

        const int firstId = Obj::getProcessId();
#ifdef BSLS_PLATFORM__OS_UNIX
        ASSERT(firstId > 0);

        const char *tmpFileName = "tmp.processutil.case2.eraseMe.txt";
        FUtil::remove(tmpFileName);
#endif
        bsl::string firstName;
        ASSERT(0 == Obj::getProcessName(&firstName));

        for (int i = 0; i < 100; ++i) {
            ASSERT(Obj::getProcessId() == firstId);
            bsl::string name;
            ASSERT(0 == Obj::getProcessName(&name));
            ASSERT(name == firstName);

#ifdef BSLS_PLATFORM__OS_UNIX
            FUtil::FileDescriptor fd = FUtil::open(tmpFileName,
                                                   true, false);
            LOOP2_ASSERT(i, fd, fd < 40);
            FUtil::close(fd);
            FUtil::remove(tmpFileName);
#endif
        }
      }  break;
      case 1: {
        // ------------------------------------------------------------
        // TESTING: BREATHING TEST
        // ------------------------------------------------------------

        if (verbose) {
            bsl::cout << "BREATHING TEST" << bsl::endl
                      << "==============" << bsl::endl;
        }

        ASSERT(0 != bdesu_ProcessUtil::getProcessId());

        // Some platforms may truncate the process name if, for example, it is
        // a very long absolute path.  So test for a prefix match.

        bsl::string name;
        ASSERT(0 == bdesu_ProcessUtil::getProcessName(&name));

        const int nameLen = static_cast<int>(name.size());

        ASSERT(0       <  nameLen);
        ASSERT(nameLen <= bsl::strlen(argv[0]));
        ASSERT(0       == bsl::strncmp(name.c_str(), argv[0], nameLen));

        if (veryVerbose) {
            P_(argv[0]);  P(name);
        }

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
