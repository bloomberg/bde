// balst_resolver_filehelper.t.cpp                                    -*-C++-*-
#include <balst_resolver_filehelper.h>

#include <balst_objectfileformat.h>

#include <bdls_filesystemutil.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

// The following is added so that this component does not need a dependency
// on bdls_processutil, since `getProcessId` is only used to create unique
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
// [ 2] balst::Resolver_FileHelper
// [ 2] ~balst::Resolver_FileHelper
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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define LOOP7_ASSERT BSLIM_TESTUTIL_LOOP7_ASSERT
#define LOOP8_ASSERT BSLIM_TESTUTIL_LOOP8_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FilesystemUtil            FilesystemUtil;
typedef FilesystemUtil::FileDescriptor  FdType;           // shorthand for file
                                                          // descriptor
typedef bsl::span<char>                 Span;
typedef bsl::span<const char>           CSpan;
typedef balst::Resolver_FileHelper      Obj;
typedef bsls::Types::Int64              Int64;

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
namespace u {

enum { e_FILE, e_MAPPED_FILE };

class FileGuard {
    const char *d_fileName;

  public:
    /// keep a pointer to the specified string `fileName` so we can delete
    /// the file upon destruction
    explicit
    FileGuard(const char *fileName) : d_fileName(fileName)
    {
    }

    /// delete the file named `d_fileName`
    ~FileGuard()
    {
        bdls::FilesystemUtil::remove(d_fileName);
    }
};

}  // close namespace u
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

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // Note that we only have to create a tmp file on Unix platforms -- this
    // component is not used and thus is not tested on Windows.

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
    const char *tmpDirName       = "/tmp";
    const char *fileNameTemplate = "/tmp/balst_Resolver_FileHelper.%d.%d.txt";

    ASSERT(FilesystemUtil::exists(tmpDirName) &&
                                      FilesystemUtil::isDirectory(tmpDirName));
#endif

    bslma::TestAllocator ta;
    bslma::DefaultAllocatorGuard guard(&ta);

    switch(test) { case 0:
#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "balst::Resolver_FileHelper usage example\n"
                             "========================================\n";

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, we prepare the file to be used by this usage example:
// ```
    bslma::TestAllocator ta;

    char fileNameBuffer[100];
    sprintf(fileNameBuffer,
            "/tmp/balst_Resolver_FileHelper.usage.%d.txt",
            getProcessId());
// ```
// Make sure file does not already exist.
// ```
    bdls::FilesystemUtil::remove(fileNameBuffer);
// ```
// Next, Create the file and open a file descriptor to it.  The boolean
// flags indicate that the file is writable, and not previously existing
// (and therefore must be created).
// ```
    FdType fd = FilesystemUtil::open(
                    fileNameBuffer,
                    FilesystemUtil::e_OPEN_OR_CREATE,  // doesn't already exist
                    FilesystemUtil::e_READ_WRITE);     // writable
    ASSERT(FilesystemUtil::k_INVALID_FD != fd);
// ```
// 64 char long string
// ```
    const char *testString64 =
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                        "0123456789+-";
// ```
// Populate the file with known data, with a zero byte at a known offset.
// ```
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
        balst::Resolver_FileHelper helper;
        rc = helper.openFile(fileNameBuffer);
        ASSERT(0 == rc);

        char buf[100];    Span bufSpan(buf);
        memset(buf, 0, sizeof(buf));
        rc = helper.readExact(Span(buf, 6),
                              128);                 // offset
        ASSERT(0 == rc);
        ASSERT(!strcmp(buf, "abcdef"));
// ```
// `readExact` past EOF fails
// ```
        rc = helper.readExact(Span(buf, 6),
                              64 * 40);             // offset
        ASSERT(0 != rc);
// ```
// `loadString` will read a zero terminated string at a given offset,
// using a buffer passed in, and allocating memory for a new copy of
// the string.
// ```
        memset(buf, 'a', sizeof(buf));
        bsl::string_view result = helper.loadString(buf,
                                                    OFFSET_OF_ZERO_BYTE - 12);
        ASSERT(12 == result.length());
        ASSERT("0123456789+-" == result);
    }
    bdls::FilesystemUtil::remove(fileNameBuffer);
// ```
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD `loadString`
        //
        // Concern:
        // 1. The method is able to load a string of various size.
        //
        // 2. The method will not read over `scratchBufLength` number of
        //    characters.
        //
        // 3. The method behave as expected even if `offset` is larger than
        //    the file size.
        //
        // 4. The method is declared as `const`.
        //
        // 5. Any memory allocation is from the supplied `basicAllocator`.
        //
        // 6. Any memory allocation is exception neutral.
        //
        // 7. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create a file, open a helper on it, and call `loadString` under
        //    the following 3 conditions: the string is entirely within the
        //    file and shorter than the passed buffer, the string is not
        //    terminated before the end of the file, and the string is longer
        //    than the passed buffer.
        //
        // 2. Use the `BSLS_ASSERTTEST_*` macros to verify that, in appropriate
        //    build modes, defensive checks are triggered for invalid
        //    arguments.
        //
        // Testing:
        //   char *loadString(Offset offset, char *scratchBuf, int len, *bA);
        // --------------------------------------------------------------------

        if (verbose) cout << "loadString Test\n"
                          << "===============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        u::FileGuard fg(fileNameBuffer);

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

        bsl::string_view testString64(
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-");
        bsl::string testString1280;

        int rc;
        for (int i = 0; i < 20; ++i) {
            rc = FilesystemUtil::write(fd,
                                       testString64.data(),
                                       static_cast<int>(testString64.size()));
            ASSERT(64 == rc);
            testString1280 += testString64;
        }

        rc = (int) FilesystemUtil::seek(fd,
                                        7 * testString64.size(),
                                        FilesystemUtil::e_SEEK_FROM_BEGINNING);
        ASSERT(7 * 64 == rc);

        rc = FilesystemUtil::write(fd, "", 1);
        ASSERT(1 == rc);
        testString1280[7 * 64] = '\0';

        rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "Testing loadString\n";

        for (int ei = u::e_FILE; ei <= u::e_MAPPED_FILE; ++ei) {
            Obj mX;    const Obj& X = mX;
            switch (ei) {
              case u::e_FILE: {
                rc = mX.openFile(fileNameBuffer);
              } break;
              case u::e_MAPPED_FILE: {
                rc = mX.openMappedFile(testString1280);
              } break;
            }
            ASSERTV(ei, 0 == rc);

            char scratchBuf[2000];
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));
            Span scratchSpan(scratchBuf);

            // short strings

            bsl::string_view result = X.loadString(scratchSpan,
                                                   7 * 64 - 12);
            ASSERTV(ei, "0123456789+-" == result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // short string reaching EOF with no zero termination

            result = X.loadString(scratchSpan,
                                  20 * 64 - 12);
            ASSERTV(ei, result, "0123456789+-" == result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // big strings

            bsl::string cmpString;
            for (int i = 0; i < 6; ++i) {
                cmpString += testString64;
            }
            ASSERT(64 * 6 == cmpString.length());

            result = X.loadString(scratchSpan, 64);
            ASSERTV(ei, cmpString, result, cmpString == result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // big string reaching EOF with no zero termination

            result = X.loadString(scratchSpan,
                                  (20 - 6) * 64);
            ASSERTV(ei, cmpString, result, cmpString == result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));

            // exhausting size of scratchSpan without reaching zero termination

            scratchSpan = scratchSpan.first(26);
            result = X.loadString(scratchSpan,
                                  (20 - 6) * 64);
            ASSERTV(ei, result, u::e_MAPPED_FILE == ei ||
                                       "abcdefghijklmnopqrstuvwxyz" == result);
            bsl::memset(scratchBuf, 'a', sizeof(scratchBuf));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            for (int ei = u::e_FILE; ei <= u::e_MAPPED_FILE; ++ei) {
                Obj mX;    const Obj& X = mX;
                char buf[1];

                switch (ei) {
                  case u::e_FILE: {
                    rc = mX.openFile(fileNameBuffer);
                  } break;
                  case u::e_MAPPED_FILE: {
                    static char star[] = { "*" };
                    rc = mX.openMappedFile(star);
                  } break;
                }
                ASSERTV(ei, 0 == rc);

                ASSERT_PASS(X.loadString(Span(buf), 0));
                ASSERT_FAIL(X.loadString(Span(buf), -1));
            }
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS METHOD `readBytes`
        //
        // Concern:
        // 1. The method read no more than `numBytes`.
        //
        // 2. The method return the number of bytes read as expected.
        //
        // 3. The method can read 0 byte.
        //
        // 4. The method succeed even if it attempts to read past the end of
        //    the file.
        //
        // 5. The method behave as expected even if `offset` is larger than
        //    the file size.
        //
        // 6. The method is declared const.
        //
        // 7. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use a table based approach, create a file, open it with a helper
        //    object and call `readBytes` with varying arguments.  Verify that
        //    the results are as expected.
        //
        // 2. Use the `BSLS_ASSERTTEST_*` macros to verify that, in appropriate
        //    build modes, defensive checks are triggered for invalid
        //    arguments.
        //
        // Testing:
        //   UintPtr readBytes(void *buf, UintPtr numBytes, Offset offset);
        // --------------------------------------------------------------------

        if (verbose) cout << "readBytes Test\n"
                          << "==============\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        u::FileGuard fg(fileNameBuffer);

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

        bsl::string_view testString64(
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-");
        bsl::string testString640;

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd,
                                  testString64.data(),
                                  static_cast<int>(testString64.size()));
            testString640 += testString64;
        }

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nTesting `readBytes`" << endl;

        static const struct Data {
            int                     d_line;
            bsls::Types::UintPtr    d_numBytes;
            FilesystemUtil::Offset  d_offset;
            int                     d_eofFlag;  // reading pass EOF
            const char             *d_result;
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
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&                  data     = DATA[ti % NUM_DATA];
            const int                    MAPPED   = ti / NUM_DATA;
            const int                    LINE     = data.d_line;
            const bsls::Types::UintPtr   SIZE     = data.d_numBytes;
            const FilesystemUtil::Offset OFFSET   = data.d_offset;
            const char                   EOF_FLAG = (char) data.d_eofFlag;
            const char *const            EXP_RES  = data.d_result;
            const size_t                 EXP_LEN  = strlen(EXP_RES);

            const char FILL_CHAR = '@';
            char       buf[100];
            bsl::memset(buf, FILL_CHAR, sizeof(buf));
            Span outSpan(buf);

            Obj mX;    const Obj& X = mX;
            rc = MAPPED ? mX.openMappedFile(testString640)
                        : mX.openFile(fileNameBuffer);
            ASSERT(0 == rc);

            CSpan retSpan = X.readBytes(outSpan.first(SIZE), OFFSET);
            ASSERTV(LINE, EXP_LEN, retSpan.size(), EXP_LEN == retSpan.size());
            ASSERTV(LINE, retSpan.data() == outSpan.data());

            if ('N' == EOF_FLAG) {
                LOOP_ASSERT(LINE, SIZE == EXP_LEN);
            }
            else {
                LOOP_ASSERT(LINE, SIZE > EXP_LEN);
            }

            ASSERTV(LINE, !bsl::memcmp(EXP_RES, retSpan.data(), EXP_LEN));
            ASSERTV(LINE, FILL_CHAR == buf[EXP_LEN]);  // Check for overrun
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\treadBytes" << endl;
            for (int mapped = 0; mapped < 2; ++mapped) {
                char buf[100];    Span bufSpan(buf);
                Obj mX;    const Obj& X = mX;
                rc = mapped ? mX.openMappedFile(testString64)
                            : mX.openFile(fileNameBuffer);
                ASSERT(0 == rc);

                ASSERT_PASS(X.readBytes(bufSpan.first(0), 0));
                ASSERT_PASS(X.readBytes(bufSpan.first(1), 1));
                ASSERT_PASS(X.readBytes(Span(), 0));

                ASSERT_FAIL(X.readBytes(bufSpan.first(0), -1));
                ASSERT_FAIL(X.readBytes(bufSpan.first(1), -1));
            }
        }
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD `readExact`.
        //
        // Concern:
        // 1. The method read exactly `numBytes`.
        //
        // 2. The method return a negative value if unable to read `numBytes`
        //    number of bytes.
        //
        // 3. The method start reading from `offset`.
        //
        // 4. The method is declared const.
        //
        // 5. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use a table based approach, create a file, open it with a helper
        //    object and call `readBytes` with varying arguments.  Verify that
        //    the results are as expected.
        //
        // 2. Use the `BSLS_ASSERTTEST_*` macros to verify that, in appropriate
        //    build modes, defensive checks are triggered for invalid
        //    arguments.
        //
        // Testing:
        //   int readExact(void *buf, UintPtr numBytes, Offset offset) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "CLASS METHOD `readExact`\n"
                          << "========================\n";

        char fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        u::FileGuard fg(fileNameBuffer);

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

        bsl::string_view testString64(
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-");
        bsl::string testString640;

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd,
                                  testString64.data(),
                                  static_cast<int>(testString64.size()));
            testString640 += testString64;
        }

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nTesting `readExact`" << endl;

        static const struct Data {
            int                     d_line;
            bsls::Types::UintPtr    d_numBytes;
            FilesystemUtil::Offset  d_offset;
            int                     d_eofFlag;  // reading pass EOF
            const char             *d_result;
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
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&                  data     = DATA[ti % NUM_DATA];
            const bool                   MAPPED   = ti / NUM_DATA;
            const int                    LINE     = data.d_line;
            const bsls::Types::UintPtr   SIZE     = data.d_numBytes;
            const FilesystemUtil::Offset OFFSET   = data.d_offset;
            const char                   EOF_FLAG = (char) data.d_eofFlag;
            const char *const            EXP_RES  = data.d_result;

            const char FILL_CHAR = '@';
            char       buf[100];
            bsl::memset(buf, FILL_CHAR, sizeof(buf));

            Obj mX;    const Obj& X = mX;
            rc = MAPPED ? mX.openMappedFile(testString640)
                        : mX.openFile(fileNameBuffer);
            ASSERT(0 == rc);

            Span outSpan(buf, SIZE);
            rc = X.readExact(outSpan, OFFSET);
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
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\treadExact" << endl;
            for (int mapped = 0; mapped < 2; ++mapped) {
                char buf[100] = { 0 };
                Obj mX;    const Obj& X = mX;
                rc = mapped ? mX.openMappedFile(testString640)
                            : mX.openFile(fileNameBuffer);
                ASSERTV(mapped, 0 == rc);

                Span outSpan(buf, 1);
                ASSERT_SAFE_PASS(X.readExact(outSpan, 0));
                ASSERT_SAFE_FAIL(X.readExact(outSpan, -1));
                outSpan = outSpan.subspan(0, 0);
                ASSERT_SAFE_PASS(X.readExact(outSpan, 0));
                ASSERT_SAFE_FAIL(X.readExact(outSpan, -1));
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR & DTOR
        //
        // Concerns:
        // 1. The supplied file is opened as expected.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use the `BSLS_ASSERTTEST_*` macros to verify that, in appropriate
        //    build modes, defensive checks are triggered for invalid
        //    arguments.
        //
        // Testing:
        //   balst::Resolver_FileHelper(const char *fileName);
        //   ~balst::Resolver_FileHelper();
        // --------------------------------------------------------------------

        if (verbose) cout << "CTOR & DTOR\n"
                          << "===========\n";

        char  fileNameBuffer[256];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, test, getProcessId());
        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        u::FileGuard fg(fileNameBuffer);

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

        bsl::string_view testString64(
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-");

        FilesystemUtil::write(fd,
                              testString64.data(),
                              static_cast<int>(testString64.size()));

        int rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\tinitialize" << endl;
            {
                Obj mX;
                ASSERT_PASS(rc = mX.openFile("bogus"));
                ASSERT(0 != rc);
                ASSERT_PASS(rc = mX.openFile(NULL));
                ASSERT(0 != rc);
                ASSERT_PASS(rc = mX.openFile(fileNameBuffer));
                ASSERT(0 == rc);
                ASSERT_PASS(rc = mX.openFile(fileNameBuffer));
                ASSERT(0 == rc);
                bsl::span<char> sp(fileNameBuffer, size_t(0));
                ASSERT_PASS(rc = mX.openMappedFile(sp));
                ASSERT(0 != rc);
            }
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
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

        u::FileGuard fg(fileNameBuffer);

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

        bsl::string_view testString64(
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-");
        ASSERT('-' == testString64.back());         // no terminating '\0'
                                                    // within `testString64`.

        for (int i = 0; i < 10; ++i) {
            FilesystemUtil::write(fd,
                                  testString64.data(),
                                  static_cast<int>(testString64.size()));
        }

        Int64 rc = FilesystemUtil::close(fd);
        ASSERT(0 == rc);

        for (int mapped = 0; mapped < 2; ++mapped) {
            if (veryVerbose) P(mapped);

            Obj mX;    const Obj& X = mX;
            rc = mapped ? mX.openMappedFile(testString64)
                        : mX.openFile(fileNameBuffer);
            ASSERTV(mapped, 0 == rc);

            char buf[100];
            bsl::memset(buf, '*', sizeof(buf));
            Span outSpan(buf, 1);
            rc = X.readExact(outSpan, 0);
            ASSERT('a' == buf[0]);
            ASSERT(0 == rc);

            bsl::memset(buf, '*', sizeof(buf));
            CSpan retSpan = X.readBytes(outSpan, 0);
            ASSERT('a' == retSpan[0]);
            ASSERT(retSpan.size() == outSpan.size());
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
