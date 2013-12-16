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
// [ 3] 'closeAndRelease'
// [ 2] 'release'
// [ 1] Breathing, d'tor
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

#ifdef BSLS_PLATFORM_OS_WINDOWS
enum { PLAT_WINDOWS = 1 };
#else
enum { PLAT_WINDOWS = 0 };
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    char rootPath[100];
    bsl::sprintf(rootPath,
                 "%s.filecloseproctor.%d",
                 PLAT_WINDOWS ? "temp" : "tmp",
                 test);
    bdesu_FileUtil::remove(rootPath, true);
    bsl::string logPath = rootPath;
    bdesu_PathUtil::appendRaw(&logPath, "log");
    ASSERT(0 == bdesu_FileUtil::createDirectories(logPath, true));

    ASSERT(bdesu_FileUtil::exists(     logPath));
    ASSERT(bdesu_FileUtil::isDirectory(logPath));

    bsl::string txtPath(logPath);
    bdesu_PathUtil::appendRaw(&txtPath, "out.txt");

    if (veryVerbose) P(txtPath);

    switch(test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE\n"
                             "=====================\n";

// Suppose we want to open a file and do some I/O to it, and be sure the
// file handle will be closed when we're done with it.  We  use an object
// of type 'bdesu_FileCloseProctor' to facilitate this.
//
// First, we create a name for our temporary file and a few local varriables.

//  const bsl::string txtPath = "essay.txt";
    int rc;
    char buffer[1000];

// Then, we open the file:

    bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(
                                                txtPath,
                                                true,       // writable
                                                false);     // non-existsent
    ASSERT(bdesu_FileUtil::INVALID_FD != fd);

// Next, we enter a scope and create a proctor object to manage 'fd':

    {
        bdesu_FileCloseProctor proctor(fd);

// Then, we declare an essay we would like to write to the file:

        const char essay[] = {
                           "If you can't annoy somebody, there is little\n"
                           "point in writing.\n"
                           "                Kingsley Amis\n"
                           "\n"
                           "It takes a lifetime to build a reputation, and\n"
                           "five minutes to lose it.\n"
                           "                Warren Buffet\n"
                           "\n"
                           "Originality is stubborn but not indestructible.\n"
                           "You can't tell it what to do, and if you try too\n"
                           "hard to steer it, you either chase it away or\n"
                           "murder it.\n"
                           "                Salman Khan\n" };

// Next, we write our essay to the file:

        rc = bdesu_FileUtil::write(fd, essay, sizeof(essay));
        ASSERT(sizeof(essay) == rc);

// Then, we seek back to the beginning of the file.

        Util::Offset off = Util::seek(fd, 0, Util::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(0 == off);

// Next, we read the file back and see if it's the same thing we wrote before:

        bsl::memset(buffer, 0, sizeof(buffer));
        rc = bdesu_FileUtil::read(fd, buffer, sizeof(buffer));
        ASSERT(sizeof(essay) == rc);
        ASSERT(! bsl::strcmp(essay, buffer));

// Now, 'proctor' goes out of scope, and its destructor closes the file
// descriptor.
    }

// Finally, we observe that further attempts to write to 'fd' fail because the
// descriptor has been closed:

    const char finalWord[] = { "No matter where you go, there you are.\n"
                               "                Buckaroo Banzai\n" };

    rc = bdesu_FileUtil::write(fd, finalWord, sizeof(finalWord));
    ASSERT(rc < 0);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing 'closeAndRelease'
        //
        // Concerns:
        //   That 'closeAndRelease' properly closes the file and restores the
        //   proctor to a released state.
        //
        // Plan:
        //   Repeat the breathing test, except rather than having proctors
        //   go out of scope, call 'closeAndRelease' on them.  Observe the
        //   state of the released proctors through the accessors to verify
        //   that they are released.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'closeAndRelease'\n"
                             "=========================\n";

        Obj fileProctor(bsl::fopen(txtPath.c_str(), "w"));
        ASSERT(fileProctor.file_p());

        // Note no \n, no flush

        int rc = fprintf(fileProctor.file_p(), "meow meow");
        ASSERT(9 == rc);

        fileProctor.closeAndRelease();
        ASSERT(0 == fileProctor.file_p());
        ASSERT(Util::INVALID_FD == fileProctor.descriptor());

        // Now read the file and verify that it was written, which will shows
        // that a flush happened, which will tell us the file has been
        // properly closed.

        char buffer[100];

        Obj fdProctor(Util::open(txtPath,
                               false,    // not writable
                               true));   // already exists
        ASSERT(Util::INVALID_FD != fdProctor.descriptor());
        Util::FileDescriptor desc = fdProctor.descriptor();

        bsl::memset(buffer, 0, sizeof(buffer));
        rc = Util::read(fdProctor.descriptor(), buffer, sizeof(buffer));
        ASSERT(9 == rc);
        ASSERT(!bsl::strcmp(buffer, "meow meow"));

        Util::Offset off = Util::seek(fdProctor.descriptor(),
                                      0,
                                      Util::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(0 == off);

        fdProctor.closeAndRelease();
        ASSERT(0 == fdProctor.file_p());
        ASSERT(Util::INVALID_FD == fdProctor.descriptor());

        bsl::memset(buffer, 0, sizeof(buffer));
        rc = Util::read(desc, buffer, sizeof(buffer));
        ASSERT(rc < 0);     // A negative value will show that read failed
                            // since 'fdProctor' closed 'desc'.
#if BSLS_PLATFORM_OS_UNIX
        ASSERT(EBADF == errno);
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing 'release'
        //
        // Concern:
        //   That a released proctor does not close files.
        //
        // Plan:
        //   With both types of file handles, associate a proctor with an open
        //   file, then destroy the proctor, and observe we can still do I/O
        //   through that file handle.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'release'\n"
                             "================\n";

        bsl::FILE *fp = bsl::fopen(txtPath.c_str(), "w");
        ASSERT(fp);
        {
            Obj proctor(fp);
            ASSERT(proctor.file_p() == fp);

            proctor.release();
            ASSERT(0 == proctor.file_p());
            ASSERT(Util::INVALID_FD == proctor.descriptor());
        }

        // Observe we can still write to 'fp'.

        int rc = bsl::fprintf(fp, "Arf arf!");
        ASSERT(8 == rc);

        rc = bsl::fclose(fp);
        ASSERT(0 == rc);

        ASSERT(8 == Util::getFileSize(txtPath));

        Util::FileDescriptor fd = Util::open(txtPath,
                                             false,    // not writable
                                             true);    // already exists
        ASSERT(Util::INVALID_FD != fd);
        {
            Obj proctor(fd);
            ASSERT(proctor.descriptor() == fd);

            proctor.release();
            ASSERT(0 == proctor.file_p());
            ASSERT(Util::INVALID_FD == proctor.descriptor());
        }

        // Observe we can still read from 'fd'.

        char buffer[100];
        bsl::memset(buffer, 0, sizeof(buffer));

        rc = Util::read(fd, buffer, sizeof(buffer));
        ASSERT(8 == rc);

        ASSERT(!bsl::strcmp("Arf arf!", buffer));

        rc = Util::close(fd);
        ASSERT(0 == rc);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test
        //
        // Concern:
        //   That proctors properly close files upon destruction.
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

        if (verbose) cout << "Breathing Test\n"
                             "==============\n";

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
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    bdesu_FileUtil::remove(rootPath, true);

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
