// bdls_filedescriptorguard.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_filedescriptorguard.h>

#include <bslim_testutil.h>

#include <bdls_pathutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#if BSLS_PLATFORM_OS_UNIX
# include <unistd.h>
# include <errno.h>
#else
# include <windows.h>
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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                 GLOBAL HELPER TYPE FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FileDescriptorGuard Obj;
typedef bdls::FilesystemUtil      Util;

#ifdef BSLS_PLATFORM_OS_WINDOWS
enum { PLAT_WINDOWS = 1 };
#else
enum { PLAT_WINDOWS = 0 };
#endif

int localGetPId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bsl::string rootPath;
    {
#ifdef BSLS_PLATFORM_OS_UNIX
        char host[80];
        ASSERT(0 ==::gethostname(host, sizeof(host)));
#else
        const char *host = "win";     // 'gethostname' is very difficult on
                                      // Windows, and we usually aren't using
                                      // nfs there anyway.
#endif
        bsl::ostringstream oss;
        oss << (PLAT_WINDOWS ? "temp." : "tmp.");
        oss << "bdls_filedescriptorguard.";
        oss << test << '.' << host << '.' << localGetPId();
        rootPath = oss.str();
    }
    Util::remove(rootPath, true);
    bsl::string logPath = rootPath;
    bdls::PathUtil::appendRaw(&logPath, "log");
    ASSERT(0 == Util::createDirectories(logPath, true));

    ASSERT(Util::exists(     logPath));
    ASSERT(Util::isDirectory(logPath));

    bsl::string fileName(logPath);
    bdls::PathUtil::appendRaw(&fileName, "out.txt");

    if (veryVerbose) P(fileName);

    switch(test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE\n"
                             "=====================\n";

// Suppose we want to open a file and perform some I/O operations.  We use an
// object of type 'bdls::FileDescriptorGuard' to ensure this handle is closed
// after the operations are complete.
//
// First, we create a name for our temporary file name and a few local
// varriables.

//  const bsl::string fileName = "essay.txt";
    int rc;

// Then, we open the file:

    Util::FileDescriptor fd = Util::open(fileName,
                                         Util::e_CREATE,
                                         Util::e_READ_WRITE);
    ASSERT(Util::k_INVALID_FD != fd);

// Next, we enter a lexical scope and create a guard object to manage 'fd':

    {
        bdls::FileDescriptorGuard guard(fd);

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

        rc = Util::write(fd, essay, sizeof(essay));
        ASSERT(sizeof(essay) == rc);

// Now, 'guard' goes out of scope, and its destructor closes the file
// descriptor.

    }

// Finally, we observe that further attempts to access 'fd' fail because the
// descriptor has been closed:

    Util::Offset off = Util::seek(fd,
                                  0,
                                  Util::e_SEEK_FROM_BEGINNING);
    ASSERT(-1 == off);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing 'closeAndRelease'
        //
        // Concerns:
        //   That 'closeAndRelease' properly closes the file and restores the
        //   guard to a released state.
        //
        // Plan:
        //   Repeat the breathing test, except rather than having guards
        //   go out of scope, call 'closeAndRelease' on them.  Observe the
        //   state of the released guards through the accessors to verify
        //   that they are released.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'closeAndRelease'\n"
                             "=========================\n";

        Util::FileDescriptor fd = Util::open(fileName,
                                             Util::e_CREATE,
                                             Util::e_READ_WRITE);
        ASSERT(Util::k_INVALID_FD != fd);

        Obj fileGuard(fd);
        ASSERT(fileGuard.descriptor());

        int rc = Util::write(fd, "meow meow", 9);
        ASSERT(9 == rc);

        fileGuard.closeAndRelease();
        ASSERT(Util::k_INVALID_FD == fileGuard.descriptor());

        // Now read the file and verify that it was written, which will shows
        // that a flush happened, which will tell us the file has been
        // properly closed.

        char buffer[100];

        Obj fdGuard(fd = Util::open(fileName,
                                    Util::e_OPEN,
                                    Util::e_READ_ONLY));
        ASSERT(Util::k_INVALID_FD != fdGuard.descriptor());

        bsl::memset(buffer, 0, sizeof(buffer));
        rc = Util::read(fdGuard.descriptor(), buffer, sizeof(buffer));
        ASSERT(9 == rc);
        ASSERT(!bsl::strcmp(buffer, "meow meow"));

        Util::Offset off = Util::seek(fdGuard.descriptor(),
                                      0,
                                      Util::e_SEEK_FROM_BEGINNING);
        ASSERT(0 == off);

        fdGuard.closeAndRelease();
        ASSERT(Util::k_INVALID_FD == fdGuard.descriptor());

        bsl::memset(buffer, 0, sizeof(buffer));
        rc = Util::read(fd, buffer, sizeof(buffer));
        ASSERT(rc < 0);     // A negative value will show that read failed
                            // since 'fdGuard' closed 'desc'.
#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT(EBADF == errno);
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing 'release'
        //
        // Concern:
        //   That a released guard does not close files.
        //
        // Plan:
        //   With both types of file handles, associate a guard with an open
        //   file, then destroy the guard, and observe we can still do I/O
        //   through that file handle.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'release'\n"
                             "================\n";

        Util::FileDescriptor fd = Util::open(fileName,
                                             Util::e_CREATE,
                                             Util::e_WRITE_ONLY);
        ASSERT(Util::k_INVALID_FD != fd);

        {
            Obj guard(fd);
            ASSERT(guard.descriptor() == fd);

            ASSERT(guard.release() == fd);
            ASSERT(Util::k_INVALID_FD == guard.descriptor());
        }

        // Observe we can still write to 'fd'.

        int rc = Util::write(fd, "Arf arf!", 8);
        ASSERT(8 == rc);

        Util::Offset off = Util::seek(fd,
                                      0,
                                      Util::e_SEEK_FROM_BEGINNING);
        ASSERT(0 == off);

        rc = Util::close(fd);
        ASSERT(0 == rc);

        ASSERT(8 == Util::getFileSize(fileName));

        fd = Util::open(fileName,
                        Util::e_OPEN,
                        Util::e_READ_ONLY);
        ASSERT(Util::k_INVALID_FD != fd);
        {
            Obj guard(fd);
            ASSERT(guard.descriptor() == fd);

            ASSERT(guard.release() == fd);
            ASSERT(Util::k_INVALID_FD == guard.descriptor());
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
        //   That guards properly close files upon destruction.
        //
        // Plan:
        //   Open files, create guards with them, do I/O, have the guard
        //   go out of scope, attempt further I/O, and observe that it fails.
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing Test\n"
                             "==============\n";

        int rc;
        Util::FileDescriptor fd;

        {
            fd = Util::open(fileName,
                            Util::e_CREATE,
                            Util::e_WRITE_ONLY);
            Obj guard(fd);

            ASSERT(Util::k_INVALID_FD != guard.descriptor());

            rc = Util::write(fd, "woof woof", 9);
            ASSERT(9 == rc);
        }

        // Attempt a write using 'fd' and observe that it fails since 'fd' is
        // closed.

        rc = Util::write(fd, "meow meow", 9);
        ASSERT(rc < 0);

#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT(EBADF == errno);
#endif

        // Now read the file and verify that it was written, which will shows
        // that a flush happened, which will tell us the file has been
        // properly closed.

        char buffer[100];
        {
            fd = Util::open(fileName,
                            Util::e_OPEN,
                            Util::e_READ_ONLY);
            ASSERT(Util::k_INVALID_FD != fd);
            Obj guard(fd);

            bsl::memset(buffer, 0, sizeof(buffer));
            int rc = Util::read(guard.descriptor(), buffer, sizeof(buffer));
            ASSERT(9 == rc);
            ASSERT(!bsl::strcmp(buffer, "woof woof"));

            Util::Offset off = Util::seek(fd,
                                          0,
                                          Util::e_SEEK_FROM_BEGINNING);
            ASSERT(0 == off);
        }

        rc = Util::read(fd, buffer, sizeof(buffer));
        ASSERT(rc < 0);

#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT(EBADF == errno);
#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // Sometimes the first remove fails but succeeds at removing leaf files,
    // then the second remove will succeed.  Sometimes both fail, but the test
    // directories will be cleaned up in a sweep anyway.

    Util::remove(rootPath, true);
    Util::remove(rootPath, true);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
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
