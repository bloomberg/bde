// balst_stacktraceresolver_filehelper.t.cpp                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolver_filehelper.h>

#include <bdls_filesystemutil.h>
#include <balst_objectfileformat.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

// The following is added so that this component does not need a dependency
// on bdls_processutil, since 'getProcessId' is only used to create unique
// file names.
#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] balst::StackTraceResolver_FileHelper
// [ 2] ~balst::StackTraceResolver_FileHelper
//
// ACCESSORS
// [ 3] readExact
// [ 4] readBytes
// [ 5] loadString
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
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

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FilesystemUtil                 FilesystemUtil;
typedef FilesystemUtil::FileDescriptor       FdType;    // shorthand for file
                                                        // descriptor
typedef balst::StackTraceResolver_FileHelper Obj;
typedef bsls::Types::Int64                   Int64;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
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
        bdls::FilesystemUtil::remove(d_fileName);
    }
};

}  // close unnamed namespace
#endif

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Note that we only have to create a tmp file on Unix platforms -- this
    // component is not used and thus is not tested on Windows.

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    const char *tmpDirName       = "/tmp";
    const char *fileNameTemplate =
                          "/tmp/balst_StackTraceResolver_FileHelper.%d.%d.txt";

    ASSERT(FilesystemUtil::exists(tmpDirName) &&
                                      FilesystemUtil::isDirectory(tmpDirName));
#endif

    bslma::TestAllocator ta;
    bslma::DefaultAllocatorGuard guard(&ta);

    switch(test) { case 0:
#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF) \
   || defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "balst::StackTraceResolver_FileHelper usage example\n"
                         "=================================================\n";

//..
    bslma::TestAllocator ta;

    char fileNameBuffer[100];
    sprintf(fileNameBuffer,
            "/tmp/balst_StackTraceResolver_FileHelper.usage.%d.txt",
            getProcessId());
//..
// Make sure file does not already exist.
//..
    bdls::FilesystemUtil::remove(fileNameBuffer);
//..
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
//..
    FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
    ASSERT(FilesystemUtil::k_INVALID_FD != fd);
//..
// 64 char long string
//..
    const char *testString64 =
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                        "0123456789+-";
//..
// Populate the file with known data, with a zero byte at a known offset.
//..
    int rc;
    for (int i = 0; i < 20; ++i) {
        rc = FilesystemUtil::write(fd, testString64, 64);
        ASSERT(64 == rc);
    }

    enum { OFFSET_OF_ZERO_BYTE = 7 * 64 };

    rc = (int) FilesystemUtil::seek(fd,
                              OFFSET_OF_ZERO_BYTE,
                              FilesystemUtil::e_SEEK_FROM_BEGINNING);
    ASSERT(OFFSET_OF_ZERO_BYTE == rc);

    rc = FilesystemUtil::write(fd, "", 1);        // write the zero byte
    ASSERT(1 == rc);

    rc = FilesystemUtil::close(fd);
    ASSERT(0 == rc);

    {
        balst::StackTraceResolver_FileHelper helper(fileNameBuffer);

        char buf[100];
        memset(buf, 0, sizeof(buf));
        rc = helper.readExact(buf,
                              6,                    // # chars to read
                              128);                 // offset
        ASSERT(0 == rc);
        ASSERT(!strcmp(buf, "abcdef"));
//..
// 'readExact' past EOF fails
//..
        rc = helper.readExact(buf,
                              6,                    // # chars to read
                              64 * 40);             // offset
        ASSERT(0 != rc);
//..
// 'loadString' will read a zero terminated string at a given offset,
// using a buffer passed in, and allocating memory for a new copy of
// the string.
//..
        memset(buf, 'a', sizeof(buf));
        char *result = helper.loadString(OFFSET_OF_ZERO_BYTE - 12,
                                         buf,
                                         sizeof(buf),
                                         &ta);

        ASSERT(12 == bsl::strlen(result));
        ASSERT(!bsl::strcmp("0123456789+-", result));
//..
// clean up
//..
        ta.deallocate(result);
    }
    bdls::FilesystemUtil::remove(fileNameBuffer);
//..
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'loadString'
        //
        // Concern:
        //: 1 The method is able to load a string of various size.
        //:
        //: 2 The method will not read over 'scratchBufLength' number of
        //:   characters.
        //:
        //: 3 The method behave as expected even if 'offset' is larger than
        //:   the file size.
        //:
        //: 4 The method is declared as 'const'.
        //:
        //: 5 Any memory allocation is from the supplied 'basicAllocator'.
        //:
        //: 6 Any memory allocation is exception neutral.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a file, open a helper on it, and call 'loadString' under
        //:   the following 3 conditions: the string is entirely within the
        //:   file and shorter than the passed buffer, the string is not
        //:   terminated before the end of the file, and the string is longer
        //:   than the passed buffer.
        //:
        //: 2 Use the 'BSLS_ASSERTTEST_*' macros to verify that, in appropriate
        //:   build modes, defensive checks are triggered for invalid
        //:   arguments.
        //
        // Testing:
        //   char *loadString(Offset offset, char *scratchBuf, int len, *bA);
        // --------------------------------------------------------------------

        if (verbose) cout << "loadString Test\n"
                          << "===============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdls::FilesystemUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
        ASSERT(FilesystemUtil::k_INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        int rc;
        for (int i = 0; i < 20; ++i) {
            rc = FilesystemUtil::write(fd, testString64, 64);
            ASSERT(64 == rc);
        }

        rc = (int) FilesystemUtil::seek(fd,
                                  7 * 64,
                                  FilesystemUtil::e_SEEK_FROM_BEGINNING);
        ASSERT(7 * 64 == rc);

        rc = FilesystemUtil::write(fd, "", 1);
        ASSERT(1 == rc);

        rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nloadString" << endl;
        {
            const Obj X(fileNameBuffer);

            char scratchBuf[2000];
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // short strings

            char *result = X.loadString(7 * 64 - 12,
                                        scratchBuf,
                                        sizeof(scratchBuf),
                                        &ta);
            ASSERT(!bsl::strcmp("0123456789+-", result));

            memset(result, 0, strlen(result) + 1);
            ta.deallocate(result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // short string reaching EOF with no zero termination

            result = X.loadString(20 * 64 - 12,
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

            result = X.loadString(64,
                                  scratchBuf,
                                  sizeof(scratchBuf),
                                  &ta);
            ASSERT(!bsl::strcmp(cmpBuf, result));

            memset(result, 0, strlen(result) + 1);
            ta.deallocate(result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // big string reaching EOF with no zero termination

            result = X.loadString((20 - 6) * 64,
                                  scratchBuf,
                                  sizeof(scratchBuf),
                                  &ta);
            ASSERT(!bsl::strcmp(cmpBuf, result));

            memset(result, 0, strlen(result) + 1);
            ta.deallocate(result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // exhausting size of scratchBuf without reaching zero termination

            result = X.loadString((20 - 6) * 64,
                                  scratchBuf,
                                  27,
                                  &ta);
            LOOP_ASSERT(result,
                        !bsl::strcmp("abcdefghijklmnopqrstuvwxyz", result));

            memset(result, 0, strlen(result) + 1);
            ta.deallocate(result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tloadString" << endl;
            {
                char buf[1];
                const Obj X(fileNameBuffer);
                char *result = 0;
                ASSERT_PASS(result = X.loadString(0, buf, 1, &ta));
                if (result) ta.deallocate(result);

                ASSERT_FAIL(result = X.loadString(0,   0, 1, &ta));
                ASSERT_FAIL(result = X.loadString(0, buf, 0, &ta));
            }
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'readBytes'
        //
        // Concern:
        //: 1 The method read no more than 'numBytes'.
        //:
        //: 2 The method return the number of bytes read as expected.
        //:
        //: 3 The method can read 0 byte.
        //:
        //: 4 The method succeed even if it attempts to read past the end of
        //:   the file.
        //:
        //: 5 The method behave as expected even if 'offset' is larger than
        //:   the file size.
        //:
        //: 6 The method is declared const.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use a table based approach, create a file, open it with a helper
        //:   object and call 'readBytes' with varying arguments.  Verify that
        //:   the results are as expected.
        //:
        //: 2 Use the 'BSLS_ASSERTTEST_*' macros to verify that, in appropriate
        //:   build modes, defensive checks are triggered for invalid
        //:   arguments.
        //
        // Testing:
        //   UintPtr readBytes(void *buf, UintPtr numBytes, Offset offset);
        // --------------------------------------------------------------------

        if (verbose) cout << "readBytes Test\n"
                          << "==============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdls::FilesystemUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
        ASSERT(FilesystemUtil::k_INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd, testString64, 64);
        }

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nTesting 'readBytes'" << endl;

        static const struct {
            int                   d_line;
            bsls::Types::UintPtr  d_numBytes;
            FilesystemUtil::Offset      d_offset;
            int                   d_eofFlag;  // reading pass EOF
            const char           *d_result;
        } DATA[] = {
            //LINE  SIZE         OFFSET  FLAG   EXPECTED
            //----  ----         ------  ----   --------

            { L_,      0,             0,  'N',  "" },
            { L_,      1,             0,  'N',  "a" },
            { L_,     10,             0,  'N',  "abcdefghij" },
            { L_,     10,            64,  'N',  "abcdefghij" },
            { L_,      1,   64 * 10 - 1,  'N',  "-" },
            { L_,     12,  64 * 10 - 12,  'N',  "0123456789+-" },
            { L_,      0,       64 * 11,  'N',  "" },
            { L_,      1,       64 * 10,  'Y',  "" },
            { L_,     90,  64 * 10 - 12,  'Y',  "0123456789+-" },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                  LINE      = DATA[ti].d_line;
            const bsls::Types::UintPtr SIZE      = DATA[ti].d_numBytes;
            const FilesystemUtil::Offset     OFFSET    = DATA[ti].d_offset;
            const char                 EOF_FLAG  = (char) DATA[ti].d_eofFlag;
            const char *const          EXP_RES   = DATA[ti].d_result;
            const bsls::Types::UintPtr EXP_RC    = strlen(EXP_RES);

            const char FILL_CHAR = '@';
            char       buf[100];
            bsl::memset(buf, FILL_CHAR, sizeof(buf));

            const Obj X(fileNameBuffer);

            const bsls::Types::UintPtr RC = X.readBytes(buf, SIZE, OFFSET);
            LOOP_ASSERT(LINE, EXP_RC == RC);

            if ('N' == EOF_FLAG) {
                LOOP_ASSERT(LINE, SIZE == EXP_RC);
            }
            else {
                LOOP_ASSERT(LINE, SIZE > EXP_RC);
            }

            LOOP_ASSERT(LINE, !bsl::memcmp(EXP_RES, buf, EXP_RC));
            LOOP_ASSERT(LINE, FILL_CHAR == buf[EXP_RC]);
                                                           // Check for overrun
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\treadBytes" << endl;
            {
                char buf[100];
                const Obj X(fileNameBuffer);
                ASSERT_PASS(X.readBytes(buf, 0, 0));
                ASSERT_FAIL(X.readBytes(0,   0, 0));
                ASSERT_FAIL(X.readBytes(buf, 0, -1));
            }
        }
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'readExact'.
        //
        // Concern:
        //: 1 The method read exactly 'numBytes'.
        //:
        //: 2 The method return a negative value if unable to read 'numBytes'
        //:   number of bytes.
        //:
        //: 3 The method start reading from 'offset'.
        //:
        //: 4 The method is declared const.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use a table based approach, create a file, open it with a helper
        //:   object and call 'readBytes' with varying arguments.  Verify that
        //:   the results are as expected.
        //:
        //: 2 Use the 'BSLS_ASSERTTEST_*' macros to verify that, in appropriate
        //:   build modes, defensive checks are triggered for invalid
        //:   arguments.
        //
        // Testing:
        //   int readExact(void *buf, UintPtr numBytes, Offset offset) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "CLASS METHOD 'readExact'\n"
                          << "========================\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdls::FilesystemUtil::remove(fileNameBuffer);

        // Next, create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
        ASSERT(FilesystemUtil::k_INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd, testString64, 64);
        }

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nTesting 'readExact'" << endl;

        static const struct {
            int                  d_line;
            bsls::Types::UintPtr d_numBytes;
            FilesystemUtil::Offset     d_offset;
            int                  d_eofFlag;  // reading pass EOF
            const char          *d_result;
        } DATA[] = {
            //LINE  SIZE         OFFSET  FLAG   EXPECTED
            //----  ----         ------  ----   --------

            { L_,      0,             0,  'N',  "" },
            { L_,      1,             0,  'N',  "a" },
            { L_,     10,             0,  'N',  "abcdefghij" },
            { L_,     10,            64,  'N',  "abcdefghij" },
            { L_,      1,   64 * 10 - 1,  'N',  "-" },
            { L_,     12,  64 * 10 - 12,  'N',  "0123456789+-" },
            { L_,      0,       64 * 11,  'N',  "" },
            { L_,      1,       64 * 10,  'Y',  "" }, // read past EOF fails
            { L_,     90,        64 * 9,  'Y',  "" }, // read past EOF fails
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                  LINE     = DATA[ti].d_line;
            const bsls::Types::UintPtr SIZE     = DATA[ti].d_numBytes;
            const FilesystemUtil::Offset     OFFSET   = DATA[ti].d_offset;
            const char                 EOF_FLAG = (char) DATA[ti].d_eofFlag;
            const char *const          EXP_RES  = DATA[ti].d_result;

            const char FILL_CHAR = '@';
            char       buf[100];
            bsl::memset(buf, FILL_CHAR, sizeof(buf));

            const Obj X(fileNameBuffer);

            rc = X.readExact(buf, SIZE, OFFSET);
            if ('N' == EOF_FLAG) {
                LOOP_ASSERT(LINE, 0 == rc);
                LOOP_ASSERT(LINE, !bsl::memcmp(EXP_RES, buf, SIZE));
                LOOP_ASSERT(LINE, FILL_CHAR == buf[SIZE]); // Check for overrun
            }
            else {
                LOOP_ASSERT(LINE, 0 > rc);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\treadExact" << endl;
            {
                char buf[100] = { "" };
                ++buf[0];                  // suppress unused variable warning
                const Obj X(fileNameBuffer);
                ASSERT_SAFE_PASS(X.readExact(buf, 0, 0));
                ASSERT_SAFE_FAIL(X.readExact(0,   0, 0));
                ASSERT_SAFE_FAIL(X.readExact(buf, 0, -1));
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR & DTOR
        //
        // Concerns:
        //: 1 The supplied file is opened as expected.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the 'BSLS_ASSERTTEST_*' macros to verify that, in appropriate
        //:   build modes, defensive checks are triggered for invalid
        //:   arguments.
        //
        // Testing:
        //   balst::StackTraceResolver_FileHelper(const char *fileName);
        //   ~balst::StackTraceResolver_FileHelper();
        // --------------------------------------------------------------------

        if (verbose) cout << "CTOR & DTOR\n"
                          << "===========\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdls::FilesystemUtil::remove(fileNameBuffer);

        // Next, create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
        ASSERT(FilesystemUtil::k_INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        FilesystemUtil::write(fd, testString64, 64);

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tconstructor" << endl;
            {
                ASSERT_PASS(Obj((const char *)fileNameBuffer));
                ASSERT_FAIL(Obj("bogus"));
                ASSERT_FAIL(Obj(NULL));
            }
        }

      }  break;
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing Test\n"
                          << "==============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        FileGuard fg(fileNameBuffer);

        // Then, make sure file does not already exist.

        bdls::FilesystemUtil::remove(fileNameBuffer);

        // Next, create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
        ASSERT(FilesystemUtil::k_INVALID_FD != fd);

        // 64 char long string

        const char *testString64 =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd, testString64, 64);
        }

        Int64 rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        {
            const Obj X(fileNameBuffer);

            char buf[100];
            rc = X.readExact(buf, 1, 0);
            ASSERT('a' == buf[0]);
            ASSERT(0 == rc);

            rc = X.readBytes(buf, 1, 0);
            ASSERT('a' == buf[0]);
            ASSERT(1 == rc);
        }

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
