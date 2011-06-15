// baesu_stacktraceresolver_filehelper.t.cpp                          -*-C++-*-
#include <baesu_stacktraceresolver_filehelper.h>

#include <bdesu_fileutil.h>
#include <baesu_objectfileformat.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

// The following is added so that this component does not need a dependency
// on bdesu_processutil, since 'getProcessId' is only used to create unique
// file names.
#ifndef BSLS_PLATFORM__OS_WINDOWS
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// CLASS METHODS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

#if   defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

typedef bdesu_FileUtil                      FileUtil;
typedef FileUtil::FileDescriptor            FdType;    // shorthand for file
                                                       // descriptor
typedef baesu_StackTraceResolver_FileHelper Helper;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
int getProcessId()
{
    return static_cast<int>(getpid());
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
// Ensures that the following functions in usage example 2 compiles and runs on
// all platform.
//-----------------------------------------------------------------------------

namespace {

class FileGuard {
    const char *d_fileName;

  public:
    explicit
    FileGuard(const char *fileName) : d_fileName(fileName)
        // keep a pointer to the specified string 'fileName' so we can delete
        // the file upon destruction
    {
    }

    ~FileGuard()
        // delete the file named 'd_fileName'
    {
        bdesu_FileUtil::remove(d_fileName);
    }
};

}  // close unnamed namespace
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    // Note that we only have to create a tmp file on Unix platforms -- this
    // component is not used and thus is not tested on Windows.

#if   defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    const char *tmpDirName       = "/tmp";
    const char *fileNameTemplate =
                          "/tmp/baesu_StackTraceResolver_FileHelper.%d.%d.txt";

    ASSERT(FileUtil::exists(tmpDirName) && FileUtil::isDirectory(tmpDirName));
#endif

    bslma_TestAllocator ta;
    bslma_DefaultAllocatorGuard guard(&ta);

    switch(test) { case 0:
#if   defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern: Demonstrate the use of
        //  'baesu_StackTraceResolver_FileHelper'.
        //
        // Plan:
        //   Create a file, open a helper on it, call 'readExact' and
        //   'loadString' and verify the results.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "baesu_StackTraceResolver_FileHelper usage example\n"
                         "=================================================\n";

        bslma_TestAllocator ta;

        char fileNameBuffer[100];
        sprintf(fileNameBuffer,
                "/tmp/baesu_StackTraceResolver_FileHelper.usage.%d.txt",
                getProcessId());

        // Make sure file does not already exist.

        bdesu_FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   true,          // writable
                                   false);        // doesn't already exist
        ASSERT(FileUtil::INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
                    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                            "0123456789+-";

        // Populate the file with known data, with a zero byte at a known
        // offset.

        int rc;
        for (int i = 0; i < 20; ++i) {
            rc = FileUtil::write(fd, testString64, 64);
            ASSERT(64 == rc);
        }

        enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };

        rc = (int) FileUtil::seek(fd,
                                  OFFSET_OF_ZERO_BYTE,
                                  FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(OFFSET_OF_ZERO_BYTE == rc);

        rc = FileUtil::write(fd, "", 1);        // write the zero byte
        ASSERT(1 == rc);

        rc = FileUtil::close(fd);
        ASSERT(0 == rc);

        {
            baesu_StackTraceResolver_FileHelper helper(fileNameBuffer);

            char buf[100];
            memset(buf, 0, sizeof(buf));
            rc = helper.readExact(buf,
                                  6,                    // # chars to read
                                  128);                 // offset
            ASSERT(0 == rc);
            ASSERT(!strcmp(buf, "abcdef"));

            // 'readExact' past EOF fails

            rc = helper.readExact(buf,
                                  6,                    // # chars to read
                                  64 * 40);             // offset
            ASSERT(0 != rc);

            // 'loadString' will read a zero terminated string at a given
            // offset, using a buffer passed in, and allocating memory for a
            // new copy of the string.

            memset(buf, 'a', sizeof(buf));
            char *result = helper.loadString(OFFSET_OF_ZERO_BYTE - 12,
                                             buf,
                                             sizeof(buf),
                                             &ta);

            ASSERT(12 == bsl::strlen(result));
            ASSERT(!bsl::strcmp("0123456789+-", result));

            // clean up

            ta.deallocate(result);
        }
        bdesu_FileUtil::remove(fileNameBuffer);
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // 'loadString' TEST
        //
        // Concern:
        //  That 'loadString' works as designed.
        //
        // Plan:
        //  Create a file, open a helper on it, and call 'loadString' under
        //  the following 3 conditions: the string is entirely within the file
        //  and shorter than the passed buffer, the string is not terminated
        //  before the end of the file, and the string is longer than the
        //  passed buffer.
        // --------------------------------------------------------------------

        if (verbose) cout << "loadString Test\n"
                          << "===============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdesu_FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   true,              // writable
                                   false);            // doesn't already exist
        ASSERT(FileUtil::INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        int rc;
        for (int i = 0; i < 20; ++i) {
            rc = FileUtil::write(fd, testString64, 64);
            ASSERT(64 == rc);
        }

        rc = (int) FileUtil::seek(fd,
                                  7 * 64,
                                  FileUtil::BDESU_SEEK_FROM_BEGINNING);
        ASSERT(7 * 64 == rc);

        rc = FileUtil::write(fd, "", 1);
        ASSERT(1 == rc);

        rc = FileUtil::close(fd);
        ASSERT(0 == rc);

        Helper helper(fileNameBuffer);

        char scratchBuf[2000];
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

        // short strings

        char *result = helper.loadString(7 * 64 - 12,
                                         scratchBuf,
                                         sizeof(scratchBuf),
                                         &ta);
        ASSERT(!bsl::strcmp("0123456789+-", result));

        memset(result, 0, strlen(result) + 1);
        ta.deallocate(result);
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

        // short string reaching EOF with no zero termination

        result = helper.loadString(20 * 64 - 12,
                                   scratchBuf,
                                   sizeof(scratchBuf),
                                   &ta);
        ASSERT(!bsl::strcmp("0123456789+-", result));

        memset(result, 0, strlen(result) + 1);
        ta.deallocate(result);
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

        // big strings

        char cmpBuf[2000];
        for (int i = 0; i < 6; ++i) {
            bsl::strcpy(cmpBuf + i * 64, testString64);
        }
        ASSERT(64 * 6 == bsl::strlen(cmpBuf));

        result = helper.loadString(64,
                                   scratchBuf,
                                   sizeof(scratchBuf),
                                   &ta);
        ASSERT(!bsl::strcmp(cmpBuf, result));

        memset(result, 0, strlen(result) + 1);
        ta.deallocate(result);
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

        // big string reaching EOF with no zero termination

        result = helper.loadString((20 - 6) * 64,
                                   scratchBuf,
                                   sizeof(scratchBuf),
                                   &ta);
        ASSERT(!bsl::strcmp(cmpBuf, result));

        memset(result, 0, strlen(result) + 1);
        ta.deallocate(result);
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

        // exhausting size of scratchBuf without reaching zero termination

        result = helper.loadString((20 - 6) * 64,
                                   scratchBuf,
                                   27,
                                   &ta);
        LOOP_ASSERT(result,!bsl::strcmp("abcdefghijklmnopqrstuvwxyz", result));

        memset(result, 0, strlen(result) + 1);
        ta.deallocate(result);
        bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // 'readBytes' TEST
        //
        // Concern:
        //  That 'readBytes' works as specced, whether or not it attempts to
        //  read past the end of the file.
        //
        // Plan:
        //  Create a file, open it with a helper object and call 'readBytes'
        //  twice, once to complete a read within the file and one to do a read
        //  that runs into the end of the file.
        // --------------------------------------------------------------------

        if (verbose) cout << "readBytes Test\n"
                          << "==============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdesu_FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   true,              // writable
                                   false);            // doesn't already exist
        ASSERT(FileUtil::INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        for (int i = 0; i < 10; ++i) {
            FileUtil::write(fd, testString64, 64);
        }

        int rc = FileUtil::close(fd);
        ASSERT(0 == rc);

        Helper helper(fileNameBuffer);

        char buf[1000];
        bsl::memset(buf, 0, sizeof(buf));

        rc = helper.readBytes(buf, 10, 64);
        ASSERT(10 == rc);

        ASSERT(!bsl::strcmp("abcdefghij", buf));

        bsl::memset(buf, 0, sizeof(buf));

        // test 'readBytes' handles EOF properly

        rc = helper.readBytes(buf, 100, 64 * 10 - 12);
        ASSERT(12 == rc);

        ASSERT(!bsl::strcmp("0123456789+-", buf));
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // 'readExact' test.
        //
        // Concern:
        //   That 'readExact' performs as designed.
        //
        // Plan:
        //   Write to a file, open with with a helper, do a read within the
        //   the file and verify that it succeeds and reads the correct data,
        //   and do a read past the end of the file and verify that the read
        //   fails as it should.
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing Test\n"
                          << "==============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdesu_FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   true,              // writable
                                   false);            // doesn't already exist
        ASSERT(FileUtil::INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        for (int i = 0; i < 10; ++i) {
            FileUtil::write(fd, testString64, 64);
        }

        int rc = FileUtil::close(fd);
        ASSERT(0 == rc);

        Helper helper(fileNameBuffer);

        char buf[100];
        bsl::memset(buf, 0, sizeof(buf));

        rc = helper.readExact(buf, 10, 64 * 10 - 12);
        ASSERT(0 == rc);

        ASSERT(!bsl::strcmp("0123456789", buf));

        // read past EOF fails

        rc = helper.readExact(buf, 100, 64 * 10);
        ASSERT(0 != rc);
      }  break;
#else
      case 1: {
        ;
      }
#endif
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
