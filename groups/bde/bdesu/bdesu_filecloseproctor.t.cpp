// bdesu_filecloseproctor.t.cpp                                       -*-C++-*-
#include <bdesu_filecloseproctor.h>

#include <bdesu_pathutil.h>

#include <bsl_cstdlib.h>    // atoi
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#if BSLS_PLATFORM_OS_UNIX
# include <errno.h>
#endif

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                  GLOBAL HELPER TYPE FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdesu_FileCloseProctor Obj;
typedef bdesu_FileUtil         Util;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch(test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test
        //
        // Concern:
        //   That proctors properly close files.
        //
        // Plan:
        //: 1 Create a proctor that manages a 'FILE *' file pointer. write some
        //:   bufferred output to the file with no '\n' so it won't be flushed.
        //:   Then have the proctor go out of scope, and open the file with
        //:   another proctor, this time managing a
        //:   'bdesu_FileUtil::FileDescriptor', and read the file, verifying
        //:   that all the output is there, which will show that it was flushed
        //:   by the first proctor closing the 'FILE *' pointer.
        //: 2 Save the descriptor from the second proctor and have that proctor
        //:   go out of scope.  Try to read from the saved descriptor and
        //:   observe that the read fails because the descriptor was closed
        //:   by the second proctor.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage Example like Testing"
                          << "\n==========================" << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
        bsl::string rootPath = "temp.filecloseproctor.1";
#else
        bsl::string rootPath = "tmp.filecloseproctor.1";
#endif
        bdesu_FileUtil::remove(rootPath, true);

        bsl::string logPath = rootPath;
        bdesu_PathUtil::appendRaw(&logPath, "log");

        ASSERT(0 == bdesu_FileUtil::createDirectories(logPath,
                                                      true));
        ASSERT(bdesu_FileUtil::exists(     logPath));
        ASSERT(bdesu_FileUtil::isDirectory(logPath));

        bsl::string txtPath(logPath);
        bdesu_PathUtil::appendRaw(&txtPath, "out.txt");

        P(txtPath);

        {
            Obj proctor(bsl::fopen(txtPath.c_str(), "w"));
            ASSERT(proctor.file_p());

            // Note no \n, no flush

            int rc = fprintf(proctor.file_p(), "woof woof");
            ASSERT(9 == rc);
        }

        // Now read the file and verify that it was written, which will shows
        // that a flush happened, which will tell us the file has been
        // properly closed.

        char buffer[100];
        Util::FileDescriptor desc;
        {
            Obj proctor(Util::open(txtPath,
                                   false,    // not writable
                                   true));   // already exists
            ASSERT(Util::INVALID_FD != proctor.descriptor());
            desc = proctor.descriptor();

            bsl::memset(buffer, 0, sizeof(buffer));
            int rc = Util::read(proctor.descriptor(), buffer, sizeof(buffer));
            ASSERT(9 == rc);
            ASSERT(!bsl::strcmp(buffer, "woof woof"));

            Util::Offset off = Util::seek(proctor.descriptor(),
                                          0,
                                          Util::BDESU_SEEK_FROM_BEGINNING);
            ASSERT(0 == off);
        }

        bsl::memset(buffer, 0, sizeof(buffer));
        int rc = Util::read(desc, buffer, sizeof(buffer));
        ASSERT(rc < 0);     // A negative value will show that read failed
                            // since 'proctor' closed 'desc'.
#if BSLS_PLATFORM_OS_UNIX
        ASSERT(EBADF == errno);
#endif

        bdesu_FileUtil::remove(rootPath, true);
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
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
