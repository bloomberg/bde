// bdls_fdstreambuf.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_fdstreambuf.h>

#include <bslim_testutil.h>

#include <bdls_filesystemutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <sys/types.h>
#include <sys/stat.h>

// The following is added so that this component does not need a dependency on
// bdls_processutil, since 'getProcessId' is only used to create unique file
// names.
#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;

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
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                 GLOBAL HELPER TYPES & CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FdStreamBuf_FileHandler  ObjFileHandler;
typedef bdls::FdStreamBuf              Obj;
typedef bdls::FilesystemUtil           FileUtil;
typedef FileUtil::FileDescriptor       FdType;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

int accum = 0x73a8f325;
const int mask17 = 1 << 17;
const int mask6  = 1 << 6;

void seedRandChar(int i)
{
    accum = i;
}

char randChar()
{
    bool b17 = accum & mask17;
    bool b6  = accum & mask6;

    accum = (accum << 1) | (b17 ^ b6);

    return (char) (accum & 0xff);
}

// can't use 'random()' because it does not exist on Windows
int randInt()
{
    static int accum = 0x12345678;
    accum = accum * 1103515245 + 12345;
    int lo = (accum >> 16) & 0xffff;
    accum = accum * 1103515245 + 12345;
    return (accum & 0xffff0000) | lo;
}

int diskLength(const char *string)
    // Given the specified 'string' without CRs, calculate the length it would
    // be if the NLs were translated to CRNLs.
{
#ifdef BSLS_PLATFORM_OS_UNIX
    return bsl::strlen(string);
#else
    int unixLen = bsl::strlen(string);
    return unixLen + bsl::count(string, string + unixLen, '\n');
#endif
}

int doRead(ObjFileHandler *fh, char *buf, int len)
{
    if (fh->isInBinaryMode()) {
        return fh->read(buf, len);                                    // RETURN
    }
    else {
        int charsSoFar = 0;
        while (charsSoFar < len) {
            int charsThisTime = fh->read(buf + charsSoFar, len - charsSoFar);
            if (!charsThisTime) {
                return charsSoFar;                                    // RETURN
            }
            charsSoFar += charsThisTime;
        };

        return charsSoFar;                                            // RETURN
    }
}

int getProcessId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

const struct {
    int                d_digits;
    bsls::Types::Int64 d_cutOff;
} TENS[] = {
    { 1, 0 },
    { 2, 10 },
    { 3, 100 },
    { 4, 1000 },
    { 5, 10 * 1000 },
    { 6, 100 * 1000 },
    { 7, 1000 * 1000 },
    { 8, 10 * 1000 * 1000 },
    { 9, 100 * 1000 * 1000 },
    { 10, 1000 * 1000 * 1000 },
    { 11, 10LL * 1000LL * 1000LL * 1000LL } };

int digits(bsls::Types::Int64 n)
{
    ASSERT(n < TENS[10].d_cutOff);
    ASSERT(n >= 0);

    for (int i = 9; i >= 0; --i) {
        if (n >= TENS[i].d_cutOff) {
            return TENS[i].d_digits;                                  // RETURN
        }
    }

    ASSERT(0);
    return 0;
}

}  // close unnamed namespace

static Obj::pos_type nullSeek(int line, Obj *sb, bool checkSeek)
    // Do a null seek, making sure that
{
    enum { CUR = FileUtil::e_SEEK_FROM_CURRENT };

    Obj::pos_type ret1 =     sb->pubseekoff(0,      bsl::ios_base::cur);

    if (checkSeek) {
        int delta = ret1 ? -1 : 1;
        (void)               sb->pubseekoff( delta, bsl::ios_base::cur);
        Obj::pos_type ret2 = sb->pubseekoff(-delta, bsl::ios_base::cur);

        LOOP3_ASSERT(line, ret1, ret2, ret1 == ret2);
    }

    return ret1;
}

static Obj  *nullSeekFdStreamBuf;
static bool  nullSeekCheckSeek;
#define NULL_SEEK()    nullSeek(L_, nullSeekFdStreamBuf, nullSeekCheckSeek)

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

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

#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpDirName[] = "C:\\TEMP";
#else
    char tmpDirName[] = "/tmp";
#endif

    ASSERT(FileUtil::exists(tmpDirName) && FileUtil::isDirectory(tmpDirName,
                                                                 true));

    bslma::TestAllocator ta;
    bslma::DefaultAllocatorGuard guard(&ta);

#ifdef BSLS_PLATFORM_OS_UNIX
    const char *fileNameTemplate = "/tmp/bdls_FdStreamBuf.%s.%d.txt";
#else
    const char *fileNameTemplate = "C:\\TEMP\\bdls_FdStreamBuf.%s.%d.txt";
#endif

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // TESTING STREAMBUF USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate reading, writing, and seeking with a bdls::FdStreamBuf
        //   on Unix using the 'streambuf' interface.
        //
        // Plan:
        //   Create a file, write to it, read back from it, do some seeks,
        //   verifying that everything works.
        //
        // Preamble:
        //   In this example we open a file descriptor on a file, then create
        //   a 'bdls::FdStreamBuf' associated with that file descriptor, and
        //   perform some standard streambuf I/O (e.g., writes, reads, and
        //   seeks) on the file.  Note that the 'sputn', 'sgetn', 'pubseekoff'
        //   and 'pubseekpos' methods are all part of the standard 'streambuf'
        //   interface described in Stroustrup 21.6.4.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING STREAMBUF USAGE EXAMPLE\n"
                             "===============================\n";

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Wherever you go, there you are.  B Banzai\n";

        const int lengthLine1 = sizeof(line1) - 1;
        const int lengthLine2 = sizeof(line2) - 1;
        const int lengthLine3 = sizeof(line3) - 1;

        // We start by selecting a file name for our (temporary) file.

        char fileNameBuffer[100];
        bsl::sprintf(fileNameBuffer,
#ifdef BSLS_PLATFORM_OS_UNIX
                     "/tmp/bdls_FdStreamBuf.usage.2.%d.txt",
#else // windows
                     "C:\\TEMP\\bdls_FdStreamBuf.usage.2.%d.txt",
#endif
                     getProcessId());

        if (verbose) bsl::cout << "Filename: " << fileNameBuffer << bsl::endl;

        // Then, make sure the file does not already exist:

        bdls::FilesystemUtil::remove(fileNameBuffer);
        ASSERT(false == bdls::FilesystemUtil::exists(fileNameBuffer));

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created):

        FdType fd = bdls::FilesystemUtil::open(
                                           fileNameBuffer,
                                           bdls::FilesystemUtil::e_CREATE,
                                           bdls::FilesystemUtil::e_READ_WRITE);

        ASSERT(bdls::FilesystemUtil::k_INVALID_FD != fd);

        // Now, we create a 'bdls::FdStreamBuf' object named 'streamBuffer'
        // associated with the file descriptor 'fd'.  Note that 'streamBuffer'
        // defaults to assuming ownership of 'fd', meaning that when
        // 'streamBuffer' is cleared, reset, or destroyed, 'fd' will be closed.
        // Note that 'FdStreamBuf' implements 'streambuf', which provides the
        // public methods used in this example:

        bdls::FdStreamBuf streamBuffer(fd, true);

        ASSERT(streamBuffer.fileDescriptor() == fd);
        ASSERT(streamBuffer.isOpened());

        // Next we use the 'sputn' method to write two lines to the file:

        streamBuffer.sputn(line1, lengthLine1);
        streamBuffer.sputn(line2, lengthLine2);

        // Then we seek back to the start of the file.

        bsl::streamoff status = streamBuffer.pubseekpos(0);
        ASSERT(0 == status);

        // Next, we read the first 'lengthLine1' characters of the file into
        // 'buf', with the method 'sgetn'.

        char buf[1000];
        bsl::memset(buf, 0, sizeof(buf));
        status = streamBuffer.sgetn(buf, lengthLine1);
        ASSERT(lengthLine1 == status);
        ASSERT(!bsl::strcmp(line1, buf));

        // Next we try to read '2 * lengthLine2' characters when only
        // 'lengthLine2' characters are available in the file to read, so the
        // 'sgetn' method will stop after reading 'lengthLine2' characters.
        // The 'sgetn' method will return the number of chars successfully
        // read:

        bsl::memset(buf, 0, sizeof(buf));
        status =  streamBuffer.sgetn(buf, 2 * lengthLine2);
        ASSERT(lengthLine2 == status);
        ASSERT(!bsl::strcmp(line2, buf));

        // Trying to read past the end of the file invalidated the current
        // cursor position in the file, so we must seek from the end or the
        // beginning of the file in order to establish a new cursor position.
        // Note the 'pubseekpos' method always seeks relative to the beginning.
        // We seek back to the start of the file:

        status = streamBuffer.pubseekpos(0);
        ASSERT(0 == status);

        // Note that line1 and line3 are the same length:

        ASSERT(lengthLine1 == lengthLine3);

        // Then we write, replacing 'line1' in the file with 'line3':

        status = streamBuffer.sputn(line3, lengthLine3);
        ASSERT(lengthLine3 == status);

        // Now we seek back to the beginning of the file:

        status = streamBuffer.pubseekpos(0);

        // Next we verify we were returned to the start of the file:

        ASSERT(0 == status);

        // Then we read and verify the first line, which now contains the text
        // of 'line3':

        bsl::memset(buf, 0, sizeof(buf));
        status = streamBuffer.sgetn(buf, lengthLine3);
        ASSERT(lengthLine3 == status);
        ASSERT(!bsl::strcmp(line3, buf));

        // Now we read and verify the second line, still 'line2':

        bsl::memset(buf, 0, sizeof(buf));
        status = streamBuffer.sgetn(buf, lengthLine2);
        ASSERT(lengthLine2 == status);
        ASSERT(!bsl::strcmp(line2, buf));

        // Next we close 'fd' and disconnect 'streamBuffer' from 'fd':

        status = streamBuffer.clear();
        ASSERT(0 == status);

        // Note that 'streamBuffer' is now no longer open, and is not
        // associated with a file descriptor:

        ASSERT(!streamBuffer.isOpened());
        ASSERT(bdls::FilesystemUtil::k_INVALID_FD ==
               streamBuffer.fileDescriptor());

        // Finally, we clean up the file:

        bdls::FilesystemUtil::remove(fileNameBuffer);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING STREAM USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate using an 'FdStreamBuf' to write to and read from a
        //   file using streams.
        //
        // Plan:
        //   Initialize a non-binary 'FdStreamBuf' with a file descriptor, then
        //   initialize an ostream with the 'FdStreamBuf', then write some
        //   data to the file.  Then initialize a new binary 'FdStreamBuf' with
        //   the file descriptor, seek back to the start of the file,
        //   initialize an istream with the 'FdStreamBuf', then read in the
        //   contents of the file.  Then initialize a non-binary 'FdStreamBuf',
        //   seek back to the start of the file, read the file in again, and,
        //   if on windows, observe that the '\r\n's have been translated back
        //   to '\n's.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING STREAM USAGE EXAMPLE\n"
                             "============================\n";

        // The most common usage of this component is to initialize a stream.
        // In this case, the 'bdls::FdStreamBuf' will be used for either input
        // or output, but not both.

        // First we create a suitable file name, and make sure that no file of
        // that name already exists:

        char fileNameBuffer[100];
        bsl::sprintf(fileNameBuffer,
#ifdef BSLS_PLATFORM_OS_UNIX
                     "/tmp/bdls_FdStreamBuf.usage.1.%d.txt",
#else // windows
                     "C:\\TEMP\\bdls_FdStreamBuf.usage.1.%d.txt",
#endif
                     getProcessId());

        // Then, make sure file does not already exist:

        bdls::FilesystemUtil::remove(fileNameBuffer);
        ASSERT(0 == bdls::FilesystemUtil::exists(fileNameBuffer));

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is to be writable, and not previously
        // existing (and therefore must be created):

        FdType fd = bdls::FilesystemUtil::open(
                                           fileNameBuffer,
                                           bdls::FilesystemUtil::e_CREATE,
                                           bdls::FilesystemUtil::e_READ_WRITE);

        ASSERT(bdls::FilesystemUtil::k_INVALID_FD != fd);

        // Now, we create an 'bdls::FdStreamBuf' type of stream buffer
        // associated with file descriptor 'fd'.  Note that the 'false'
        // argument indicates that 'streamBuffer' will not assume ownership of
        // 'fd', meaning that when 'streamBuffer' is destroyed 'fd' will remain
        // open:

        {
            bdls::FdStreamBuf streamBuffer(fd,
                                           true,    // writable
                                           false);  // 'fd' won't be closed
                                                    // when 'streamBuffer' is
                                                    // destroyed

            bsl::ostream os(&streamBuffer);

            os << "Five times nine point five = " << 5 * 9.5 << endl;
        }

        // Note also that the stream buffer defaults to being in text mode on
        // Windows, and binary mode on Unix.

        // Now create a new stream buffer to read the file back, in this case
        // using binary mode so we can see exactly what was written.  The new
        // stream buf is used to initialize an input stream.

        {
            // read it in binary mode

            bdls::FdStreamBuf streamBuffer(fd,
                                           false,  // not writable
                                           false,  // 'streamBuffer' does not
                                                   // own 'fd'
                                           true);  // binary mode

            streamBuffer.pubseekpos(0);

            char buf[100];
            bsl::memset(buf, 0, sizeof(buf));

            bsl::istream is(&streamBuffer);
            char *pc = buf;
            do {
                is >> bsl::noskipws >> *pc++;
            } while ('\n' != pc[-1]);

#ifdef BSLS_PLATFORM_OS_UNIX
            ASSERT(!bsl::strcmp("Five times nine point five = 47.5\n", buf));
#else
            //On Windows we see a CRLF ('\r\n') instead of a simple LF '\n'

            ASSERT(!bsl::strcmp("Five times nine point five = 47.5\r\n", buf));
#endif
        }

        // Finally, read the file back a second time, this time in text mode.
        // Note how, on Windows, the '\r\n' is translated back to '\n'

        {
            // read it back in text mode

            bdls::FdStreamBuf streamBuffer(fd,
                                           false);  // not writable
                                                 // 'fd' will be closed when
                                                 // streamBuffer is destroyed.
                                                 // Mode will be binary on
                                                 // Unix, text on Dos.

            streamBuffer.pubseekpos(0);

            char buf[100];
            bsl::memset(buf, 0, sizeof(buf));

            bsl::istream is(&streamBuffer);
            char *pc = buf;
            do {
                is >> bsl::noskipws >> *pc++;
            } while ('\n' != pc[-1]);

            ASSERT(!bsl::strcmp("Five times nine point five = 47.5\n", buf));
        }

        // And finally, we clean up:

        bdls::FilesystemUtil::remove(fileNameBuffer);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING NULL SEEKS
        //
        // Concerns:
        //: 1 That null seeks accurately return the position as perceived by
        //:   the 'FdStreambuf' client.
        //: 2 That null seeks don't interfere with the proper functioning of
        //    the component.
        //
        // Plan:
        //
        // Take the 'alternate reads and writes test' case 13, and pepper it
        // with lots of null seeks, and see if concerns 1 and 2 are met in the
        // following situations:
        //: 1 Null seek after a read, before another read.
        //: 2 Null seek after a read, before a write.
        //: 3 Null seek after a read, before a seek.
        //: 4 Null seek after a write, before another write.
        //: 5 Null seek after a write, before a read.
        //: 6 Null seek after a write, before a seek.
        //: 7 Null seek after a seek, before a read.
        //: 8 Null seek after a seek, before a write.
        //: 9 Null seek after a seek, before a seek.
        // Also vary the buffer size of the streambuf to inflict different
        // types of overflow error.  Reads will usually go through mmap so
        // will be less affected by buffer size than writes.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING NULL SEEKS\n"
                             "==================\n";

        enum { CUR = FileUtil::e_SEEK_FROM_CURRENT };

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Wherever you go, there you are.  B Banzai\n";
        const char line4[] =
                        "This line is exactly as long as line 2,\n"
                        "and I mean EXACTLY, counting the chars EXACTLY ...\n";

        const int lLen1 = sizeof(line1) - 1;
        const int lLen2 = sizeof(line2) - 1;
        const int lLen3 = sizeof(line3) - 1;
        const int lLen4 = sizeof(line4) - 1;

        ASSERT(lLen1 == lLen3);
        ASSERT(lLen2 == lLen4);

        for (int ti = 0; ti < 4; ++ti) {
            int isText        = !!(ti & 1);
            nullSeekCheckSeek = !!(ti & 2);

#ifdef BSLS_PLATFORM_OS_UNIX
            if (isText) continue;
#endif

            const int eLen1 = lLen1 + isText;
            const int eLen3 = lLen3 + isText;
            const int eLen4 = lLen4 + 2 * isText;

            // We start by selecting a file name for our (temporary) file.

            char fileNameBuffer[100];
            bsl::sprintf(fileNameBuffer, fileNameTemplate, "15",
                                                               getProcessId());

            if (verbose) cout << "Filename: " << fileNameBuffer << endl;

            // Next, Create the file and open a file descriptor to it.  The
            // boolean flags indicate that the file is writable, and not
            // previously existing (and therefore must be created).

            int bufSizes[] = { 16, 19, 20, 21, 22,  28, 37, 50, 64, 4096 };
            enum { NUM_BUF_SIZES = sizeof bufSizes / sizeof *bufSizes };
            BSLMF_ASSERT(NUM_BUF_SIZES > 5);

            for (int tj = 0; tj < NUM_BUF_SIZES; ++tj) {
                FileUtil::remove(fileNameBuffer);

                FdType fd = FileUtil::open(fileNameBuffer,
                                           FileUtil::e_CREATE,
                                           FileUtil::e_READ_WRITE);
                ASSERT(-1 != (int) fd);

                Obj sb(fd, true, true, !isText, &ta);
                nullSeekFdStreamBuf = &sb;

                // Impose a very small buffer, so that buffer overflow code
                // will frequently be exercised.

                char sbBuf[10 * 1000];
                sb.pubsetbuf(sbBuf, bufSizes[tj]);

                ASSERT(lLen1 == sb.sputn(line1, lLen1));

                // P-4

                ASSERT(eLen1 == NULL_SEEK());

                ASSERT(lLen2 == sb.sputn(line2, lLen2));
                ASSERT(lLen3 == sb.sputn(line3, lLen3));

                ASSERT(0 == sb.pubseekpos(0));

                char buf[1000];
                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen1 == sb.sgetn(buf, lLen1));
                ASSERT(0 == bsl::strcmp(line1, buf));

                // P-1

                ASSERT(eLen1 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen2 == sb.sgetn(buf, lLen2));
                ASSERT(0 == bsl::strcmp(line2, buf));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen3 == sb.sgetn(buf, lLen3));
                ASSERT(0 == bsl::strcmp(line3, buf));

                ASSERT(0 == sb.pubseekpos(0));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen1 == sb.sgetn(buf, lLen1));
                ASSERT(0 == bsl::strcmp(line1, buf));

                // P-2

                ASSERT(eLen1 == NULL_SEEK());

                ASSERT(lLen4 == sb.sputn(line4, lLen4));

                // P-5

                LOOP3_ASSERT(eLen1, eLen4, NULL_SEEK(),
                                                 eLen1 + eLen4 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                int sts = sb.sgetn(buf, lLen3);
                LOOP2_ASSERT(lLen3, sts, lLen3 == sts);
                ASSERT(0 == bsl::strcmp(line3, buf));

                // P-3

                ASSERT(eLen1 + eLen4 + eLen3 == NULL_SEEK());

                ASSERT(0 == sb.pubseekpos(0));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen1 == sb.sgetn(buf, lLen1));
                ASSERT(0 == bsl::strcmp(line1, buf));

                ASSERT(eLen1 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen4 == sb.sgetn(buf, lLen4));
                ASSERT(0 == bsl::strcmp(line4, buf));

                // C-3

                ASSERT(eLen1 + eLen4 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen3 == sb.sgetn(buf, lLen3));
                ASSERT(0 == bsl::strcmp(line3, buf));

                ASSERT(eLen1 + eLen4 + eLen3 == NULL_SEEK());

                ASSERT(0 == sb.pubseekpos(0));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen1 + lLen4 + lLen3
                                      == sb.sgetn(buf, lLen1 + lLen4 + lLen3));
                ASSERT(bsl::string(line1) + line4 + line3 == buf);

                ASSERT(eLen1 + eLen4 + eLen3 == NULL_SEEK());

                ASSERT(0 == sb.pubseekpos(0));

                // P-8

                ASSERT(0 == NULL_SEEK());

                ASSERT(lLen3 == sb.sputn(line3, lLen3));

                ASSERT(eLen3 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen4 == sb.sgetn(buf, lLen4));
                LOOP2_ASSERT(line4, buf, 0 == bsl::strcmp(line4, buf));

                ASSERT(eLen3 + eLen4 == NULL_SEEK());

                ASSERT(lLen1 == sb.sputn(line1, lLen1));

                // P-6

                ASSERT(eLen3 + eLen4 + eLen1 == NULL_SEEK());

                ASSERT(100 == sb.pubseekpos(100));

                // P-9

                ASSERT(100 == sb.pubseekoff(  0, bsl::ios_base::cur));
                ASSERT( 80 == sb.pubseekoff(-20, bsl::ios_base::cur));
                ASSERT( 80 == sb.pubseekoff(  0, bsl::ios_base::cur));
                ASSERT( 60 == sb.pubseekoff(-20, bsl::ios_base::cur));

                ASSERT(0 == sb.pubseekpos(0));

                // P-7

                ASSERT(0 == NULL_SEEK());

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen3 == sb.sgetn(buf, lLen3));
                ASSERT(0 == bsl::strcmp(line3, buf));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen4 == sb.sgetn(buf, lLen4));
                LOOP2_ASSERT(line4, buf, 0 == bsl::strcmp(line4, buf));

                bsl::memset(buf, 0, sizeof(buf));
                ASSERT(lLen1 == sb.sgetn(buf, lLen1));
                ASSERT(0 == bsl::strcmp(line1, buf));

                ASSERT(0 == sb.pubseekpos(0));
                ASSERT(0 == NULL_SEEK());

                ASSERT(lLen3 + lLen4 + lLen1
                                      == sb.sgetn(buf, lLen3 + lLen4 + lLen1));
                ASSERT(bsl::string(line3) + line4 + line1 == buf);

                ASSERT(eLen3 + eLen4 + eLen1 == NULL_SEEK());

                sb.clear();

                // clean up

                FileUtil::remove(fileNameBuffer);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING PEEK FUNCTIONALITY
        //
        // Concerns:
        //   That the peek buffer in the file handler works properly.
        //
        // Plan:
        //   Exercise all cases of the peek functionality, verifying that it
        //   does the right thing.  Note that the comment
        //   "peek buffer is now full" refers only to Windows, the peek buffer
        //   never contains anything on Unix.  This whole test is really
        //   meant to test windows behavior, on Unix it's not really testing
        //   anything interesting.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PEEK FUNCTIONALITY\n"
                             "==========================\n";

        // We start by selecting a file name for our (temporary) file.

        char fileNameBuffer[100];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, "14", getProcessId());

        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        // Then, make sure file does not already exist.

        FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        // open a streamBuf (in text mode on Windows)

        ObjFileHandler fh;
        fh.reset(fd, true, true, false);

        const char endsWithN[] = "To be or not to be, that is the question.\n";
        const int lengthN = sizeof(endsWithN) - 1;
        const char endsWithR[] = "To be or not to be, that is the question.\r";
        const int lengthR = sizeof(endsWithR) - 1;
        const char woof[] = "Woof.";
        const int lengthWoof = sizeof(woof) - 1;

        ASSERT(0 == fh.write(endsWithN, lengthN));
        ASSERT(0 == fh.write(endsWithR, lengthR));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        char buf[1000];
        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthN == fh.read(buf, lengthN));
        ASSERT(!bsl::strcmp(buf, endsWithN));
        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        ASSERT(0 == fh.write(endsWithR, lengthR));
        ASSERT(0 == fh.write(endsWithN, lengthN));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthN == fh.read(buf, lengthN));
        ASSERT(!bsl::strcmp(buf, endsWithN));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT(lengthR + 1 ==
                     FileUtil::seek(fd, 0, FileUtil::e_SEEK_FROM_CURRENT));
#endif
        ASSERT(lengthR == fh.seek(0, FileUtil::e_SEEK_FROM_CURRENT));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthN == fh.read(buf, lengthN));
        ASSERT(!bsl::strcmp(buf, endsWithN));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

        ASSERT(lengthR + 3 == fh.seek(lengthR + 3,
                                      FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthN - 3 == fh.read(buf, lengthN - 3));
        ASSERT(!bsl::strcmp(buf, endsWithN + 3));

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

        ASSERT(lengthR + 3 == fh.seek(3, FileUtil::e_SEEK_FROM_CURRENT));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthN - 3 == fh.read(buf, lengthN - 3));
        ASSERT(!bsl::strcmp(buf, endsWithN + 3));

        // now do write with peek buffer full

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lengthR == fh.read(buf, lengthR));
        ASSERT(!bsl::strcmp(buf, endsWithR));

        // peek buffer now full

        fh.write(woof, lengthWoof);

        {
            bsl::streamoff cur = fh.seek(0, FileUtil::e_SEEK_FROM_CURRENT);

            LOOP3_ASSERT(lengthR,
                         lengthWoof,
                         cur,
                         lengthR + lengthWoof == cur);
        }

        ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

        ASSERT(0 == fh.write(endsWithR, lengthR));
        ASSERT(0 == fh.write(endsWithN, lengthN));

        // we're at end of file

#ifdef BSLS_PLATFORM_OS_WINDOWS
        buf[0] = 0x1a;    // ctrl-Z
        ASSERT(0 == fh.write(buf, 1));

        ASSERT(lengthR == fh.seek(lengthR,
                                  FileUtil::e_SEEK_FROM_BEGINNING));

        // observe when we try we don't get the ctrl-Z

        bsl::memset(buf, 0, sizeof(buf));
        {
            int charsRead = fh.read(buf, 2 * lengthN);
            LOOP2_ASSERT(lengthN, charsRead, lengthN == charsRead);
        }
        buf[lengthN] = 0;
        ASSERT(!bsl::strcmp(buf, endsWithN));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(0 == fh.read(buf, 2 * lengthN));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(0 == fh.read(buf, 2 * lengthN));
#endif

        ASSERT(0 == fh.clear());

        // clean up

        FileUtil::remove(fileNameBuffer);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATE READS AND WRITES
        //
        // Concerns:
        //   That an 'FdStreamBuf' can transition directly from read to write
        //   and back.
        //
        // Plan:
        //   Define four strings, two each of two different lengths.  Write
        //   a pattern to the file of short string, long string, short string.
        //   Then do a read-write-read on the file, then a write-read-write,
        //   always, when doing a write, toggling the strings between the two
        //   values we have for strings of the given length.  Verify that
        //   the data shows up as expected.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ALTERNATE READS AND WRITES\n"
                             "==================================\n";

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Wherever you go, there you are.  B Banzai\n";
        const char line4[] =
                        "This line is exactly as long as line 2,\n"
                        "and I mean EXACTLY, counting the chars EXACTLY ...\n";

        const int lineLength1 = sizeof(line1) - 1;
        const int lineLength2 = sizeof(line2) - 1;
        const int lineLength3 = sizeof(line3) - 1;
        const int lineLength4 = sizeof(line4) - 1;

        ASSERT(lineLength1 == lineLength3);
        ASSERT(lineLength2 == lineLength4);

        // We start by selecting a file name for our (temporary) file.

        char fileNameBuffer[100];
        bsl::sprintf(fileNameBuffer, fileNameTemplate, "13", getProcessId());

        if (verbose) cout << "Filename: " << fileNameBuffer << endl;

        // Then, make sure file does not already exist.

        FileUtil::remove(fileNameBuffer);

        // Next, Create the file and open a file descriptor to it.  The boolean
        // flags indicate that the file is writable, and not previously
        // existing (and therefore must be created).

        FdType fd = FileUtil::open(fileNameBuffer,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        Obj sb(fd, true, true, true, &ta);

        char sbBuf[16];
        sb.pubsetbuf(sbBuf, sizeof(sbBuf));

        ASSERT(lineLength1 == sb.sputn(line1, lineLength1));
        ASSERT(lineLength2 == sb.sputn(line2, lineLength2));
        ASSERT(lineLength3 == sb.sputn(line3, lineLength3));

        ASSERT(0 == sb.pubseekpos(0));

        char buf[1000];
        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength1 == sb.sgetn(buf, lineLength1));
        ASSERT(0 == bsl::strcmp(line1, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength2 == sb.sgetn(buf, lineLength2));
        ASSERT(0 == bsl::strcmp(line2, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength3 == sb.sgetn(buf, lineLength3));
        ASSERT(0 == bsl::strcmp(line3, buf));

        ASSERT(0 == sb.pubseekpos(0));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength1 == sb.sgetn(buf, lineLength1));
        ASSERT(0 == bsl::strcmp(line1, buf));

        ASSERT(lineLength4 == sb.sputn(line4, lineLength4));

        LOOP2_ASSERT(lineLength1 + lineLength4,
                                         sb.pubseekoff(0, bsl::ios_base::cur),
                     lineLength1 + lineLength4 ==
                                         sb.pubseekoff(0, bsl::ios_base::cur));

        bsl::memset(buf, 0, sizeof(buf));
        int sts = sb.sgetn(buf, lineLength3);
        LOOP2_ASSERT(lineLength3, sts, lineLength3 == sts);
        ASSERT(0 == bsl::strcmp(line3, buf));

        ASSERT(0 == sb.pubseekpos(0));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength1 == sb.sgetn(buf, lineLength1));
        ASSERT(0 == bsl::strcmp(line1, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength4 == sb.sgetn(buf, lineLength4));
        ASSERT(0 == bsl::strcmp(line4, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength3 == sb.sgetn(buf, lineLength3));
        ASSERT(0 == bsl::strcmp(line3, buf));

        ASSERT(0 == sb.pubseekpos(0));

        ASSERT(lineLength3 == sb.sputn(line3, lineLength3));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength4 == sb.sgetn(buf, lineLength4));
        LOOP2_ASSERT(line4, buf, 0 == bsl::strcmp(line4, buf));

        ASSERT(lineLength1 == sb.sputn(line1, lineLength1));

        ASSERT(0 == sb.pubseekpos(0));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength3 == sb.sgetn(buf, lineLength3));
        ASSERT(0 == bsl::strcmp(line3, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength4 == sb.sgetn(buf, lineLength4));
        LOOP2_ASSERT(line4, buf, 0 == bsl::strcmp(line4, buf));

        bsl::memset(buf, 0, sizeof(buf));
        ASSERT(lineLength1 == sb.sgetn(buf, lineLength1));
        ASSERT(0 == bsl::strcmp(line1, buf));

        sb.clear();

        // clean up

        FileUtil::remove(fileNameBuffer);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING TRANSLATION OF \n'S TO \r\n'S WITH 1 BYTE BUFFER
        //
        // Concerns:
        //   That on non-binary files on windows, '\n's are being properly
        //   translated to and from \r\n's on disk.  There was a potential for
        //   this to become problematic with a 1 byte buffer.
        //
        // Plan:
        //   Repeat test 11, but with a 1 byte buffer.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                  "TESTING TRANSLATION OF \n'S TO \r\n'S WITH 1 BYTE BUFFER\n"
                  "========================================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "12", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        const char dLine1[] = "To be or not to be, that is the question.\r\n";
        const char dLine2[] =
                         "There are more things in heaven and earth,\r\n"
                         "Horatio, than are dreamt of in your philosophy.\r\n";
#endif
        const int dLen1 = diskLength(line1);
        const int dLen2 = diskLength(line2);

        FileUtil::remove(fnBuf);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        char buf[1000];
        {
            Obj sb(fd, true);    // defaults to text mode on windows

            sb.pubsetbuf(0, 1);

            sb.sputn(line1, len1);
            sb.sputn(line2, len2);

            sb.pubseekpos(0);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len1);
            ASSERT(!bsl::strcmp(line1, buf));

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            bsl::streamoff sts = sb.pubseekoff(0,  bsl::ios_base::cur);
            ASSERT(dLen1 + dLen2 == sts);
            sts = sb.pubseekoff(-dLen2, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(-dLen2, bsl::ios_base::end);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekpos(dLen1);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(-dLen2, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(-dLen1, bsl::ios_base::cur);
            ASSERT(0 == sts);
            sts = sb.pubseekoff(   -20, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 - 20 == sts);
            sts = sb.pubseekoff( dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(    50, bsl::ios_base::cur);
            ASSERT(dLen1 + 50 == sts);
            sts = sb.pubseekoff(-dLen2, bsl::ios_base::end);
            ASSERT(dLen1 == sts);
        }

        struct stat s;
        ASSERT(0 == ::stat(fnBuf, &s));
        ASSERT(dLen1 + dLen2 == s.st_size);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT(bsl::strlen(dLine1) == dLen1);
        ASSERT(bsl::strlen(dLine2) == dLen2);

        ASSERT(dLen1 != len1);
        ASSERT(dLen2 != len2);

        // now verify for sure the \r's are in the file by reading it in
        // binary mode

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);

            Obj sb(fd, true, true, true);     // binary mode on windows

            sb.pubsetbuf(0, 1);

            int sts = sb.pubseekoff(0, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 == sts);
            sts = sb.pubseekoff(-(dLen1 + dLen2), bsl::ios_base::end);
            ASSERT(0 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen1);
            ASSERT(!bsl::strcmp(dLine1, buf));

            sts = sb.pubseekoff(0, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(-10, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 - 10 == sts);
            sts = sb.pubseekoff(-(dLen2 - 10), bsl::ios_base::cur);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));

            sts = sb.pubseekpos(dLen1);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));

            sts = sb.pubseekoff(dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));
        }
#endif

        // clean up

        FileUtil::remove(fnBuf);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TRANSLATION OF \n'S TO \r\n'S
        //
        // Concerns:
        //   That on non-binary files on windows, '\n's are being properly
        //   translated to and from \r\n's on disk.
        //
        // Plan:
        //   Write some strings with \n's in them to disk on a non-binary
        //   file, and verify that on windows, these are translated to \r\n,
        //   and translated back to \n on reading.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING TRANSLATION OF \n'S TO \r\n'S\n"
                             "=====================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "11", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        const char dLine1[] = "To be or not to be, that is the question.\r\n";
        const char dLine2[] =
                         "There are more things in heaven and earth,\r\n"
                         "Horatio, than are dreamt of in your philosophy.\r\n";
#endif
        const int dLen1 = diskLength(line1);
        const int dLen2 = diskLength(line2);

        FileUtil::remove(fnBuf);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        char buf[1000];
        {
            Obj sb(fd, true);    // defaults to text mode on windows

            sb.sputn(line1, len1);
            sb.sputn(line2, len2);

            sb.pubseekpos(0);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len1);
            ASSERT(!bsl::strcmp(line1, buf));

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            bsl::streamoff sts = sb.pubseekoff(0,  bsl::ios_base::cur);
            ASSERT(dLen1 + dLen2 == sts);
            sts = sb.pubseekoff(-dLen2, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(-dLen2, bsl::ios_base::end);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekpos(dLen1);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, len2);
            ASSERT(!bsl::strcmp(line2, buf));

            sts = sb.pubseekoff(-dLen2, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(-dLen1, bsl::ios_base::cur);
            ASSERT(0 == sts);
            sts = sb.pubseekoff(   -20, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 - 20 == sts);
            sts = sb.pubseekoff( dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(    50, bsl::ios_base::cur);
            ASSERT(dLen1 + 50 == sts);
            sts = sb.pubseekoff(-dLen2, bsl::ios_base::end);
            ASSERT(dLen1 == sts);
        }

        struct stat s;
        ASSERT(0 == ::stat(fnBuf, &s));
        ASSERT(dLen1 + dLen2 == s.st_size);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT(bsl::strlen(dLine1) == dLen1);
        ASSERT(bsl::strlen(dLine2) == dLen2);

        ASSERT(dLen1 != len1);
        ASSERT(dLen2 != len2);

        // now verify for sure the \r's are in the file by reading it in
        // binary mode

        {

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);

            Obj sb(fd, true, true, true);     // binary mode on windows

            int sts = sb.pubseekoff(0, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 == sts);
            sts = sb.pubseekoff(-(dLen1 + dLen2), bsl::ios_base::end);
            ASSERT(0 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen1);
            ASSERT(!bsl::strcmp(dLine1, buf));

            sts = sb.pubseekoff(0, bsl::ios_base::cur);
            ASSERT(dLen1 == sts);
            sts = sb.pubseekoff(-10, bsl::ios_base::end);
            ASSERT(dLen1 + dLen2 - 10 == sts);
            sts = sb.pubseekoff(-(dLen2 - 10), bsl::ios_base::cur);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));

            sts = sb.pubseekpos(dLen1);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));

            sts = sb.pubseekoff(dLen1, bsl::ios_base::beg);
            ASSERT(dLen1 == sts);

            bsl::memset(buf, 0, sizeof(buf));
            sb.sgetn(buf, dLen2);
            ASSERT(!bsl::strcmp(dLine2, buf));
        }
#endif

        // clean up

        FileUtil::remove(fnBuf);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING MULTI-PAGE FILE
        //
        // Concerns:
        //   Test that the stream buffer works correctly on a file spanning
        //   multiple pages.
        //
        // Plan:
        //   Use local routine 'randChar()' to generate chars to write to
        //   a file, write them to the file, verify that the correct chars
        //   were written.  Have a large segment of memory where we write a
        //   copy of everything, the memory should be a mirror of what's in
        //   the file, in the end verify they match.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING MULTI-PAGE FILE\n"
                             "=======================\n";

        enum { FILE_SIZE = 1 << 15 };

        char *mirror = (char *) ta.allocate(FILE_SIZE);
        ASSERT(mirror);
        bsl::memset(mirror, 0, FILE_SIZE);

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "10", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        FileUtil::remove(fnBuf);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        Obj sb(fd, true, true, true, &ta);

        for (int i = 0; i < FILE_SIZE; ++i) {
            char c = randChar();
            mirror[i] = c;
            sb.sputc(c);
        }

        ASSERT(0 == sb.pubseekpos(0));
        for (char *pc = mirror, *end = mirror + FILE_SIZE; pc < end; ++pc) {
            char c = static_cast<char>(sb.sbumpc());
            LOOP3_ASSERT(pc - mirror, c, *pc, c == *pc);
        }

        if (verbose) cout << "Writing\n";

        ASSERT(FILE_SIZE / 2 == sb.pubseekoff(-FILE_SIZE / 2,
                                              bsl::ios_base::end));
        int offset = FILE_SIZE / 2;
        for (int pass = 2; pass < 100; ++pass) {
            int r = randInt();
            int jump = (r & 0x4000) ? (r & 0x1fff) : -(r & 0x3fff);
            jump -= 0x800;

            int dest;
            while ((dest = offset + jump, dest < 0 || dest >= FILE_SIZE)) {
                jump /= 2;
            }

            if (r & 0x8000) {
                ASSERT(dest == sb.pubseekpos(dest));
            }
            else if (r & 0x10000) {
                ASSERT(dest == sb.pubseekoff(dest - offset,
                                             bsl::ios_base::cur));
            }
            else {
                ASSERT(dest == sb.pubseekoff(dest - FILE_SIZE,
                                             bsl::ios_base::end));
            }
            offset = dest;

            if (veryVerbose) cout << "Dest = " << dest << endl;

            char *pc = mirror + offset;

            int length = randInt() & 0x1fff;
            for (int end = bsl::min(offset + length, (int) FILE_SIZE - 1);
                                                offset < end; ++offset, ++pc) {
                char c = randChar();
                *pc = c;
                sb.sputc(c);
            }
        }

        ASSERT(0 == sb.pubseekpos(0));
        for (char *pc = mirror, *end = mirror + FILE_SIZE; pc < end; ++pc) {
            char c = static_cast<char>(sb.sbumpc());
            LOOP3_ASSERT(pc - mirror, c, *pc, c == *pc);
        }

        if (verbose) cout << "Reading\n";

        ASSERT(FILE_SIZE / 2 == sb.pubseekoff(-FILE_SIZE / 2,
                                              bsl::ios_base::end));
        offset = FILE_SIZE / 2;
        for (int pass = 2; pass < 100; ++pass) {
            int r = randInt();
            int jump = (r & 0x4000) ? (r & 0x1fff) : -(r & 0x1fff);

            int dest;
            while ((dest = offset + jump, dest < 0 || dest >= FILE_SIZE)) {
                jump /= 2;
            }

            if (r & 0x8000) {
                ASSERT(dest == sb.pubseekpos(dest));
            }
            else if (r & 0x10000) {
                ASSERT(dest == sb.pubseekoff(dest - offset,
                                             bsl::ios_base::cur));
            }
            else {
                ASSERT(dest == sb.pubseekoff(dest - FILE_SIZE,
                                             bsl::ios_base::end));
            }
            offset = dest;

            if (veryVerbose) cout << "Dest = " << dest << endl;

            char *pc = mirror + offset;

            int length = randInt() & 0xf;
            for (int end = bsl::min(offset + length, (int) FILE_SIZE - 1);
                                                offset < end; ++offset, ++pc) {
                const char c = static_cast<char>(sb.sbumpc());
                ASSERT(c == *pc);
            }
        }

        ta.deallocate(mirror);

        sb.clear();

        FileUtil::remove(fnBuf);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING SUNGETC, SPUTBACKC ON INPUT
        //
        // Concerns:
        //   That the 'sungetc' and 'sputbackc' methods work properly.
        //
        // Plan:
        //   Try putting back chars using both the 'sungetc' and 'sputbackc'
        //   methods, then read from the buffer, verifying that
        //   we get the expected input.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING UNGETC, PUTBACKC ON INPUT\n"
                             "=================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "9", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] = "ABCDEFGH or not to be, that is the question.\n";
        const char line2Trunc[] = "ABCDEFGH or not to be, that is the questio";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);
        const int len2Trunc = bsl::strlen(line2Trunc);

        FileUtil::remove(fnBuf);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        Obj sb(fd, true);

        ASSERT(len1 == sb.sputn(line1, len1));

        ASSERT(4 == sb.pubseekpos(4));

        char buf[1000];
        ASSERT(1 == sb.sgetn(buf, 1));          // put it into input mode

        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('H'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('G'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('F'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('E'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('D'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('C'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('B'));
        ASSERT(bsl::char_traits<char>::eof() != sb.sputbackc('A'));

        bsl::memset(buf, 0, sizeof(buf));
        int sts = sb.sgetn(buf, len2Trunc);
        LOOP2_ASSERT(len1, sts, len2Trunc == sts);
        LOOP_ASSERT(buf, !bsl::strcmp(line2Trunc, buf));

        ASSERT('o' == sb.sungetc());
        ASSERT('i' == sb.sungetc());
        ASSERT('t' == sb.sungetc());
        ASSERT('s' == sb.sungetc());
        ASSERT('e' == sb.sungetc());
        ASSERT('u' == sb.sungetc());

        bsl::memset(buf, 0, sizeof(buf));
        sts = sb.sgetn(buf, 300);
        LOOP_ASSERT(sts, 9 == sts);
        LOOP_ASSERT(buf, !bsl::strcmp(line2 + len2 - 9, buf));

                sb.clear();

        FileUtil::remove(fnBuf);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // RESET, RELEASE, CLEAR ON FDSTREAMBUF
        //
        // Concerns:
        //   That the 'reset', 'release' and 'clear' methods, as well as
        //   destruction, have the desired effects on a file descriptor.
        //
        // Plan:
        //   Perform the 'reset', 'release' and 'clear' operations, as well
        //   as destruction, and verify that they have the desired effect on
        //   the file descriptor.
        // --------------------------------------------------------------------


        if (verbose) cout << "RESET, RELEASE, CLEAR ON FDSTREAMBUF TEST\n"
                             "=========================================\n";


        // Windows FileDescriptor's are pointers, so a different type of cast
        // is needed.

        FileUtil::FileDescriptor BOGUS_HANDLE =
#ifdef BSLS_PLATFORM_OS_WINDOWS
                              reinterpret_cast<FileUtil::FileDescriptor>(100);
#else
                              static_cast<FileUtil::FileDescriptor>(100);
#endif



        char fnBuf[100];
        char fnBuf2[100];
        bsl::sprintf(fnBuf,  fileNameTemplate, "8a", getProcessId());
        bsl::sprintf(fnBuf2, fileNameTemplate, "8b", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";

        const int len1 = bsl::strlen(line1);

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        FdType fd2 = FileUtil::open(fnBuf2,
                                    FileUtil::e_CREATE,
                                    FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd2);

        {
            Obj sb(fd, true, false, true, &ta);

            ASSERT(sb.isOpened());
            ASSERT(!sb.willCloseOnReset());
            ASSERT(sb.fileDescriptor() == fd);

            ASSERT(len1 == sb.sputn(line1, len1));

            ASSERT(!sb.clear());      // willCloseOnReset is false, should not
                                      // close fd

            ASSERT(!sb.isOpened());
            ASSERT((FdType) -1 == sb.fileDescriptor());

            ASSERT(!sb.reset(fd, true, true, true));

            ASSERT(sb.willCloseOnReset());
            ASSERT(sb.fileDescriptor() == fd);

            ASSERT(0 == sb.pubseekoff(0, bsl::ios_base::beg));

            // read, verifying fd was not closed

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            // will not close fd, even though willCloseOnReset was set

            sb.release();

            ASSERT(!sb.isOpened());
            ASSERT((FdType) -1 == sb.fileDescriptor());

            ASSERT(!sb.reset(fd, true, true));

            ASSERT(sb.isOpened());
            ASSERT(sb.willCloseOnReset());
            ASSERT(sb.fileDescriptor() == fd);

            ASSERT(0 == sb.pubseekpos(0));

            // read, verifying fd was not closed

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            ASSERT(!sb.clear());         // should close fd

            ASSERT(!sb.isOpened());
            ASSERT((FdType) -1 == sb.fileDescriptor());

            // open and get same fd, which verifies clear closed fd

            ASSERT(fd == FileUtil::open(fnBuf,
                                        FileUtil::e_OPEN,
                                        FileUtil::e_READ_WRITE));

            ASSERT(!sb.reset(fd, true, false, true));

            ASSERT(len1 == sb.pubseekoff(0, bsl::ios_base::end));
            ASSERT(sb.sputn(line1, len1));      // should append to file, so
                                                // file will contain line1
                                                // twice

            ASSERT(sb.isOpened());
            ASSERT(!sb.willCloseOnReset());
            ASSERT(fd == sb.fileDescriptor());

            ASSERT(0 == sb.pubseekpos(0));

            // read, verifying fd was not closed

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            // read second copy of line1 verifying append worked

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            ASSERT(!sb.reset(fd2, true, true, true));    // should not close fd

            ASSERT(sb.isOpened());
            ASSERT(sb.willCloseOnReset());
            ASSERT(fd2 == sb.fileDescriptor());

            // destroying sb should close fd2
        }

        // open and get same fd, which verifies destruction of sb closed fd

        ASSERT(fd2 == FileUtil::open(fnBuf2,
                                     FileUtil::e_OPEN,
                                     FileUtil::e_READ_WRITE));

        {
            Obj sb(fd, true, false, true, &ta);

            ASSERT(sb.isOpened());
            ASSERT(!sb.willCloseOnReset());
            ASSERT(fd == sb.fileDescriptor());

            // destroying sb should not close fd
        }

        if (verbose) {
            cout << "\tTesting 'clear' on an invalid file handle" << endl;
        }
        {
            Obj mX(BOGUS_HANDLE, true, true, true, &ta); const Obj& X = mX;

            ASSERT(X.isOpened());
            ASSERT(X.willCloseOnReset());

            ASSERT(0 == mX.clear());

            ASSERT(!X.isOpened());
        }

        if (verbose) {
            cout << "\tTesting 'reset' on an invalid file handle" << endl;
        }

        {
            char filename[100];
            bsl::sprintf(filename,
                         fileNameTemplate, "reset-bad-handle", getProcessId());



            Obj mX(BOGUS_HANDLE, true, true, true, &ta); const Obj& X = mX;

            ASSERT(X.isOpened());
            ASSERT(X.willCloseOnReset());

            FdType fd = FileUtil::open(filename,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);

            ASSERT(0 == mX.reset(fd, 1));

            ASSERT(X.isOpened());
            ASSERT(fd == X.fileDescriptor());
        }

        // getting '0' from close verifies fd's are still open

        ASSERT(0 == FileUtil::close(fd));
        ASSERT(0 == FileUtil::close(fd2));

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // RESET, RELEASE, CLEAR ON FILEHANDLER
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "RESET, RELEASE, CLEAR ON FILEHANDLER TEST\n"
                             "=========================================\n";

        char fnBuf[100];
        char fnBuf2[100];
        bsl::sprintf(fnBuf,  fileNameTemplate, "7a", getProcessId());
        bsl::sprintf(fnBuf2, fileNameTemplate, "7b", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        FdType fd2 = FileUtil::open(fnBuf2,
                                    FileUtil::e_CREATE,
                                    FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd2);

        {
            ObjFileHandler fh;

            ASSERT(!fh.isOpened());

            ASSERT(!fh.reset(fd, true, false));

            ASSERT(fh.isOpened());
            ASSERT(!fh.willCloseOnReset());
            ASSERT(fh.isRegularFile());
            ASSERT(fh.fileDescriptor() == fd);

            ASSERT(0 == fh.write(line1, len1));

            ASSERT(!fh.clear());          // willCloseOnReset is false, should
                                          // not close fd

            ASSERT(!fh.isOpened());
            ASSERT((FdType) -1 == fh.fileDescriptor());

            ASSERT(!fh.reset(fd, true, true));

            ASSERT(fh.willCloseOnReset());
            ASSERT(fh.fileDescriptor() == fd);

            ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

            // read, verifying fd was not closed

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == doRead(&fh, buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            // will not close fd, even though willCloseOnReset was set

            fh.release();

            ASSERT(!fh.isOpened());
            ASSERT((FdType) -1 == fh.fileDescriptor());

            ASSERT(!fh.reset(fd, true, true));

            ASSERT(fh.isOpened());
            ASSERT(fh.willCloseOnReset());
            ASSERT(fh.fileDescriptor() == fd);

            ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

            // read, verifying fd was not closed

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == doRead(&fh, buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            ASSERT(!fh.clear());                 // should close fd

            ASSERT(!fh.isOpened());
            ASSERT((FdType) -1 == fh.fileDescriptor());

            // open and get same fd, which verifies clear closed fd

            ASSERT(fd == FileUtil::open(fnBuf,
                                        FileUtil::e_OPEN,
                                        FileUtil::e_READ_WRITE));

            ASSERT(!fh.reset(fd, true, false));

            fh.seek(0, FileUtil::e_SEEK_FROM_END);
            ASSERT(0 == fh.write(line1, len1));      // should append to file,
                                                     // so file will contain
                                                     // line1 twice

            ASSERT(fh.isOpened());
            ASSERT(fd == fh.fileDescriptor());

            ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

            // read, verifying fd was not closed

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == doRead(&fh, buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            // read second copy of line1 verifying append worked

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == doRead(&fh, buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            ASSERT(!fh.reset(fd2, true, false));         // should not close fd

            ASSERT(fh.isOpened());
            ASSERT(fd2 == fh.fileDescriptor());

            ASSERT(0 == fh.write(line2, len2));

            ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len2 == doRead(&fh, buf, len2));
            ASSERT(!bsl::strcmp(line2, buf));

            ASSERT(!fh.reset(fd, true, false));          // should not close fd

            ASSERT(0 == fh.seek(0, FileUtil::e_SEEK_FROM_BEGINNING));

            // verify first file still begins with line1

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == doRead(&fh, buf, len1));
            ASSERT(!bsl::strcmp(line1, buf));

            ASSERT(!fh.reset(fd2, true, true));          // should not close fd

            // destroying fh should close fd2
        }

        // open and get same fd, which verifies destruction of fh closed fd

        ASSERT(fd2 == FileUtil::open(fnBuf2,
                                     FileUtil::e_OPEN,
                                     FileUtil::e_READ_WRITE));

        {
            ObjFileHandler fh;

            ASSERT(!fh.isOpened());
            ASSERT((FdType) -1 == fh.fileDescriptor());

            ASSERT(!fh.reset(fd, true, false));

            ASSERT(fh.isOpened());
            ASSERT(!fh.willCloseOnReset());
            ASSERT(fd == fh.fileDescriptor());

            // destroying fh should not close fd
        }

        // getting '0' from close verifies fd's are still open

        ASSERT(0 == FileUtil::close(fd));
        ASSERT(0 == FileUtil::close(fd2));

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // INITIALIZING OSTREAMS AND ISTREAMS WITH FDSTREAMBUFS
        //
        // Concerns:
        //   That streams can be properly constructed using FdStreamBufs, and
        //   proper I/O can then be done using them.
        //
        // Plan:
        //   Initialize some streams with FdStreamBuf's and do some stream I/O
        //   on them.
        // --------------------------------------------------------------------

        if (verbose) cout << "ISTREAM AND OSTREAM TEST\n"
                             "========================\n";

        char fnBuf[100], fnBuf2[100];
        bsl::sprintf(fnBuf,  fileNameTemplate, "6a", getProcessId());
        bsl::sprintf(fnBuf2, fileNameTemplate, "6b", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";

        const int dLen1 = diskLength(line1);
        const int dLen2 = diskLength(line2);

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true);

            {
                bsl::ostream os(&sb);

                os << line1;
                os << line2;
            }
        }

        struct stat s;
        ASSERT(0 == ::stat(fnBuf, &s));
        ASSERT(dLen1 + dLen2 == s.st_size);

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_OPEN,
                                   FileUtil::e_READ_ONLY);
        ASSERT(-1 != (int) fd);

        {
            Obj sb(fd, true);

            {
                bsl::istream is(&sb);

                char c;
                for (const char *pc = line1; *pc; ++pc) {
                    is >> bsl::noskipws >> c;
                    LOOP2_ASSERT(*pc, c, *pc == c);
                }
                for (const char *pc = line2; *pc; ++pc) {
                    is >> bsl::noskipws >> c;
                    LOOP2_ASSERT(*pc, c, *pc == c);
                }
            }
        }

        // Verity fd was closed by opening again and verifying we get the same
        // fd.

        ASSERT(fd == FileUtil::open(fnBuf,
                                    FileUtil::e_OPEN,
                                    FileUtil::e_READ_ONLY));
        ASSERT(-1 != (int) fd);

        ASSERT(0 == FileUtil::close(fd));

        {
            FdType fd2 = FileUtil::open(fnBuf2,
                                        FileUtil::e_CREATE,
                                        FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd2);

            {
                Obj sb(fd2, true, false);

                bsl::ostream os(&sb);

                os << "Five times nine point five equals " << 5 * 9.5 << endl;
            }

            {
                Obj sb(fd2, true);

                sb.pubseekpos(0);

                bsl::istream is(&sb);

                const char *line = "Five times nine point five equals 47.5\n";
                for (const char *pc = line; *pc; ++pc) {
                    char c;
                    is >> bsl::noskipws >> c;
                    LOOP2_ASSERT(*pc, c, *pc == c);
                }
            }
        }

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf INTERMIXED READ AND WRITE, JUGGLING OPENS
        //
        // Concerns:
        //   That the constructor works properly.
        // Plan:
        //   Repeat and expand upon test 3, only the c'tor instead of the
        //   'reset' method to connect to the fd.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf C'TOR TEST\n"
                             "============================\n";

        char fnBuf[100];
        char fnBuf2[100];
        bsl::sprintf(fnBuf,  fileNameTemplate, "5a", getProcessId());
        bsl::sprintf(fnBuf2, fileNameTemplate, "5b", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Whatever you go, there you are.  B Banzai\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);
        const int len3 = bsl::strlen(line3);

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);

        // Create file

        FdType fd = FileUtil::open(fnBuf,
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        {
            Obj sb(fd, true, true, true);

            sb.sputn(line1, 20);
            sb.sputn(line1 + 20, len1 - 20);
            for (const char *pc = line2; *pc; ++pc) {
                sb.sputc(*pc);
            }

            ASSERT(0 == sb.pubseekpos(0));
            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));

            ASSERT(!bsl::strcmp(buf, line1));

            ASSERT(20 == sb.pubseekpos(20));
            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::end));

            bsl::memset(buf, 0, sizeof(buf));
            char *pcB = buf;
            for (const char *pcL = line2; *pcL; ++pcL, ++pcB) {
                LOOP2_ASSERT(*pcL, sb.sgetc(), *pcL == sb.sgetc());
                *pcB = static_cast<char>(sb.sbumpc());
                LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
            }

            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(len1 == len3);

            ASSERT(0 == sb.pubseekpos(0));
            ASSERT(len3 == sb.sputn(line3, len3));

            ASSERT(0 == sb.pubseekpos(0));
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));

            ASSERT(!bsl::strcmp(buf, line3));

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len2 == sb.sgetn(buf, len2));

            ASSERT(!bsl::strcmp(buf, line2));
        }

        {
            ASSERT(fd == FileUtil::open(fnBuf,
                                        FileUtil::e_OPEN,
                                        FileUtil::e_READ_ONLY));
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, true, true, &ta);

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));
            ASSERT(!bsl::strcmp(buf, line3));

            ASSERT(len2 == sb.sgetn(buf + len1, len2));
            ASSERT(!bsl::strcmp(buf + len1, line2));

            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::cur));
            bsl::memset(buf, 0, sizeof(buf));

            ASSERT(len2 == sb.sgetn(buf, len2));
            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(!sb.clear());

            // verify file was closed by getting same file descriptor when we
            // open again

            FdType fd2 = FileUtil::open(fnBuf2,
                                        FileUtil::e_CREATE,
                                        FileUtil::e_READ_WRITE);
            ASSERT(fd2 == fd);
            ASSERT(0 == FileUtil::close(fd2));
        }

        {
            ASSERT(fd == FileUtil::open(fnBuf,
                                        FileUtil::e_OPEN,
                                        FileUtil::e_READ_ONLY));
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, true, true);

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));
            ASSERT(!bsl::strcmp(buf, line3));

            ASSERT(len2 == sb.sgetn(buf + len1, len2));
            ASSERT(!bsl::strcmp(buf + len1, line2));

            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::cur));
            bsl::memset(buf, 0, sizeof(buf));

            ASSERT(len2 == sb.sgetn(buf, len2));
            ASSERT(!bsl::strcmp(buf, line2));

            sb.release();

            // Verify file was not closed by not getting same file descriptor
            // when we open again

            FdType fd2 = FileUtil::open(fnBuf2,
                                        FileUtil::e_OPEN,
                                        FileUtil::e_READ_WRITE);
            ASSERT(fd2 != fd);
            ASSERT(0 == FileUtil::close(fd));
            ASSERT(0 == FileUtil::close(fd2));
        }

        FileUtil::remove(fnBuf);
        FileUtil::remove(fnBuf2);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf pubsetbuf
        //
        // Concerns:
        //   That the FdStreamBuf can work properly with a modest 10 byte
        //   buffer.
        //
        // Plan:
        //   Repeat test 3, after doing a pubsetbuf.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf PUBSETBUF TEST\n"
                             "================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "4", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Whatever you go, there you are.  B Banzai\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);
        const int len3 = bsl::strlen(line3);

        FileUtil::remove(fnBuf);

        // Create file

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, false, true, &ta);

            char buffer[10];
            sb.pubsetbuf(buffer, sizeof(buffer));

            sb.sputn(line1, 20);
            sb.sputn(line1 + 20, len1 - 20);
            for (const char *pc = line2; *pc; ++pc) {
                sb.sputc(*pc);
            }

            ASSERT(0 == sb.pubseekpos(0));
            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));

            ASSERT(!bsl::strcmp(buf, line1));

            ASSERT(20 == sb.pubseekpos(20));
            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::end));

            bsl::memset(buf, 0, sizeof(buf));
            char *pcB = buf;
            for (const char *pcL = line2; *pcL; ++pcL, ++pcB) {
                LOOP2_ASSERT(*pcL, sb.sgetc(), *pcL == sb.sgetc());
                *pcB = static_cast<char>(sb.sbumpc());
                LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
            }

            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(len1 == len3);

            ASSERT(0 == sb.pubseekpos(0));
            ASSERT(len3 == sb.sputn(line3, len3));

            ASSERT(0 == sb.pubseekpos(0));
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));

            ASSERT(!bsl::strcmp(buf, line3));

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len2 == sb.sgetn(buf, len2));

            ASSERT(!bsl::strcmp(buf, line2));

            bsl::streamoff sts = sb.pubseekpos(len1);
            ASSERT(len1 == sts);

            sts = sb.in_avail();
            ASSERT(len2 == sts);

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));// verifies clear did not close fd
        }

        {

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, false, true, &ta);

            char buffer[10];
            sb.pubsetbuf(buffer, sizeof(buffer));

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));
            ASSERT(!bsl::strcmp(buf, line3));

            ASSERT(len2 == sb.sgetn(buf + len1, len2));
            ASSERT(!bsl::strcmp(buf + len1, line2));

            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::cur));
            bsl::memset(buf, 0, sizeof(buf));

            ASSERT(len2 == sb.sgetn(buf, len2));
            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));
        }

        FileUtil::remove(fnBuf);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf INTERMIXED READ AND WRITE
        //
        // Concerns:
        //   That reads and writes work properly.
        //
        // Plan:
        //   Write to and read from the file.  The two lines 'line1' and
        //   'line3' are the same length, so we will replace 'line1' with
        //   'line3' in the file.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf READ-WRITE TEST\n"
                             "=================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "3", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";
        const char line3[] = "Whatever you go, there you are.  B Banzai\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);
        const int len3 = bsl::strlen(line3);

        FileUtil::remove(fnBuf);

        // Create file

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            Obj sb((FdType) -1, false);
            ASSERT(!sb.reset(fd, true, false, true));

            sb.sputn(line1, 20);
            sb.sputn(line1 + 20, len1 - 20);
            for (const char *pc = line2; *pc; ++pc) {
                sb.sputc(*pc);
            }

            ASSERT(0 == sb.pubseekpos(0));
            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len1 == sb.sgetn(buf, len1));

            ASSERT(!bsl::strcmp(buf, line1));

            ASSERT(20 == sb.pubseekpos(20));
            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::end));

            bsl::memset(buf, 0, sizeof(buf));
            char *pcB = buf;
            for (const char *pcL = line2; *pcL; ++pcL, ++pcB) {
                LOOP2_ASSERT(*pcL, sb.sgetc(), *pcL == sb.sgetc());
                *pcB = static_cast<char>(sb.sbumpc());
                LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
            }

            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(len1 == len3);

            ASSERT(0 == sb.pubseekpos(0));
            ASSERT(len3 == sb.sputn(line3, len3));

            ASSERT(0 == sb.pubseekpos(0));
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));

            ASSERT(!bsl::strcmp(buf, line3));

            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len2 == sb.sgetn(buf, len2));

            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));
        }

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            Obj sb((FdType) -1, false);
            ASSERT(!sb.reset(fd, true, false, true));

            char buf[1000];
            bsl::memset(buf, 0, sizeof(buf));
            ASSERT(len3 == sb.sgetn(buf, len3));
            ASSERT(!bsl::strcmp(buf, line3));

            ASSERT(len2 == sb.sgetn(buf + len1, len2));
            ASSERT(!bsl::strcmp(buf + len1, line2));

            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::cur));
            bsl::memset(buf, 0, sizeof(buf));

            ASSERT(len2 == sb.sgetn(buf, len2));
            ASSERT(!bsl::strcmp(buf, line2));

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));
        }

        FileUtil::remove(fnBuf);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf BREATHING TEST
        //
        // Concerns:
        //   Basic functionality.
        //
        // Plan:
        //   Connect to a file and do some writes, reads, and seeks on it.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf BREATHING TEST\n"
                             "================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "2", getProcessId());

        if (verbose) cout << "Filename: " << fnBuf << endl;

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);

        // Create file

        {
            FileUtil::remove(fnBuf);

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, false, true, &ta);

            for (const char *pc = line1; *pc; ++pc) {
                sb.sputc(*pc);
            }
            sb.sputn(line2, len2);

            sb.pubseekpos(0);
            for (const char *pc = line1; *pc; ++pc) {
                sb.sputc(*pc);
            }
            sb.pubseekpos(0);
            sb.pubseekpos(len1);
            sb.sputn(line2, len2);

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));

            struct stat s;
            ASSERT(0 == ::stat(fnBuf, &s));

            ASSERT(len1 + len2 == s.st_size);
        }

        // read and seek on file

        {

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, false, true, &ta);

            char buf[1000], *pcB = buf;
            bsl::memset(buf, 0, sizeof(buf));
            for (const char *pcL = line1; *pcL; ++pcL, ++pcB) {
                LOOP2_ASSERT(*pcL, sb.sgetc(), *pcL == sb.sgetc());
                *pcB = static_cast<char>(sb.sbumpc());
                LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
            }

            LOOP2_ASSERT(buf, bsl::strlen(buf), !bsl::strcmp(line1, buf));

            ASSERT(len2 == sb.sgetn(pcB, len2));

            ASSERT(!bsl::strcmp(line2, pcB));

            ASSERT(!sb.clear());
            ASSERT(0 == FileUtil::close(fd));
        }

        // read and seek on file

        {

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);
            ASSERT(-1 != (int) fd);

            Obj sb(fd, true, true, true, &ta);
            bsl::streamoff sts;

            char buf[1000], *pcB = buf;
            bsl::memset(buf, 0, sizeof(buf));
            for (const char *pcL = line1; *pcL; ++pcL, ++pcB) {
                LOOP2_ASSERT(*pcL, (char) sb.sgetc(), *pcL == sb.sgetc());
                *pcB = static_cast<char>(sb.sbumpc());
                LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
            }

            ASSERT(buf + len1 == pcB);
            LOOP2_ASSERT(buf, bsl::strlen(buf), !bsl::strcmp(line1, buf));

            {
                sts = sb.pubseekoff(-10, bsl::ios_base::cur);
                LOOP2_ASSERT(len1 - 10, sts, len1 - 10 == sts);
                sts = sb.pubseekoff(-10, bsl::ios_base::cur);
                LOOP2_ASSERT(len1 - 20, sts, len1 - 20 == sts);
                pcB = buf + len1 - 20;
                bsl::memset(pcB, 0, sizeof(buf) - (len1 - 20));

                for (const char *pcL = line1 + len1 - 20; *pcL; ++pcL, ++pcB) {
                    LOOP2_ASSERT(*pcL, (char) sb.sgetc(), *pcL == sb.sgetc());
                    *pcB = static_cast<char>(sb.sbumpc());
                    LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
                }

                LOOP2_ASSERT(buf, bsl::strlen(buf), !bsl::strcmp(line1, buf));
            }

            {
                ASSERT(20 == sb.pubseekoff(20, bsl::ios_base::beg));
                pcB = buf + 20;
                bsl::memset(pcB, 0, sizeof(buf) - 20);

                for (const char *pcL = line1 + 20; *pcL; ++pcL, ++pcB) {
                    LOOP2_ASSERT(*pcL, (char) sb.sgetc(), *pcL == sb.sgetc());
                    *pcB = static_cast<char>(sb.sbumpc());
                    LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
                }

                LOOP2_ASSERT(buf, bsl::strlen(buf), !bsl::strcmp(line1, buf));
            }

            ASSERT(len1 - 20 == sb.pubseekoff(-20, bsl::ios_base::cur));
            ASSERT(len1 == sb.pubseekoff(-len2, bsl::ios_base::end));
            ASSERT(len2 == sb.sgetn(pcB, len2));

            ASSERT(!bsl::strcmp(line2, pcB));

            {
                ASSERT(2 == sb.pubseekoff(2, bsl::ios_base::beg));
                ASSERT(len1 + len2 - 20 ==
                                       sb.pubseekoff(-20, bsl::ios_base::end));
                pcB = buf + len1 + len2 - 20;
                bsl::memset(pcB, 0, sizeof(buf) - (pcB - buf));

                for (const char *pcL = line2 + len2 - 20; *pcL;
                                                                ++pcL, ++pcB) {
                    LOOP2_ASSERT(*pcL, (char) sb.sgetc(), *pcL == sb.sgetc());
                    *pcB = static_cast<char>(sb.sbumpc());
                    LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
                }

                LOOP2_ASSERT(buf + len1, bsl::strlen(buf + len1),
                                              !bsl::strcmp(line2, buf + len1));
            }

            {
                ASSERT(len1 + 20 == sb.pubseekpos(len1 + 20));
                pcB = buf + len1 + 20;
                bsl::memset(pcB, 0, sizeof(buf) - (pcB - buf));

                for (const char *pcL = line2 + 20; *pcL; ++pcL, ++pcB) {
                    LOOP2_ASSERT(*pcL, (char) sb.sgetc(), *pcL == sb.sgetc());
                    *pcB = static_cast<char>(sb.sbumpc());
                    LOOP2_ASSERT(*pcL, *pcB, *pcL == *pcB);
                }

                LOOP2_ASSERT(buf + len1, bsl::strlen(buf + len1),
                                              !bsl::strcmp(line2, buf + len1));

                ASSERT(len1 + 20 == sb.pubseekpos(len1 + 20));
                pcB = buf + len1 + 20;
                bsl::memset(pcB, 0, sizeof(buf) - (pcB - buf));

                ASSERT(len2 - 20 == sb.sgetn(pcB, len2 - 20));

                LOOP2_ASSERT(buf + len1, bsl::strlen(buf + len1),
                                              !bsl::strcmp(line2, buf + len1));
            }

            sb.release();
            ASSERT(0 == FileUtil::close(fd));

            FileUtil::remove(fnBuf);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf_FileHandler BREATHING TEST
        //
        // Concerns:
        //   Exercise FileHandler basic functionality.
        //
        // Plan:
        //   Create a FileHandler, connect it to a file, do some writes, seeks,
        //   and reads on it.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf_FileHandler BREATHING TEST\n"
                             "============================================\n";

        char fnBuf[100];
        bsl::sprintf(fnBuf, fileNameTemplate, "1", getProcessId());

        const char line1[] = "To be or not to be, that is the question.\n";
        const char line2[] =
                           "There are more things in heaven and earth,\n"
                           "Horatio, than are dreamt of in your philosophy.\n";

        const int len1 = bsl::strlen(line1);
        const int len2 = bsl::strlen(line2);

        const int dLen1 = diskLength(line1);
        const int dLen2 = diskLength(line2);

        // Create file

        {
            FileUtil::remove(fnBuf);

            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);

            ASSERT(-1 != (int) fd);

            ObjFileHandler sbFileHandler;
            ASSERT(!sbFileHandler.reset(fd, true, true, true));

            ASSERT(0 == sbFileHandler.write(line1, len1));
            ASSERT(0 == sbFileHandler.write(line2, len2));

            ASSERT(len1 + len2 == sbFileHandler.fileSize());

            ASSERT(!sbFileHandler.clear());

            struct stat s;
            ASSERT(0 == ::stat(fnBuf, &s));

            ASSERT(len1 + len2 == s.st_size);
        }

        // read and seek on file

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);
            ASSERT(-1 != (int) fd);

            ObjFileHandler sbFileHandler;
            ASSERT(!sbFileHandler.reset(fd, true, false, true));

            ASSERT(len1 + len2 == sbFileHandler.fileSize());

            char inBuf1[100];
            bsl::memset(inBuf1, 'f', sizeof(inBuf1));
            bsl::streamoff sts = sbFileHandler.read(inBuf1, len1);
            ASSERT(len1 == sts);
            inBuf1[len1] = 0;

            ASSERT(!bsl::strcmp(inBuf1, line1));

            char inBuf2[200];
            bsl::memset(inBuf2, 'b', sizeof(inBuf2));
            sts = sbFileHandler.read(inBuf2, len2);
            ASSERT(len2 == sts);
            inBuf2[len2] = 0;

            ASSERT(!bsl::strcmp(inBuf2, line2));

            sts = sbFileHandler.seek(-20, FileUtil::e_SEEK_FROM_CURRENT);
            ASSERT(len1 + len2 - 20 == sts);
            sts = sbFileHandler.seek(-(len2-20),
                                     FileUtil::e_SEEK_FROM_CURRENT);
            ASSERT(len1 == sts);

            bsl::memset(inBuf2, 'b', sizeof(inBuf2));
            sts = sbFileHandler.read(inBuf2, len2);
            ASSERT(len2 == sts);
            inBuf2[len2] = 0;

            ASSERT(!bsl::strcmp(inBuf2, line2));

            sts = sbFileHandler.seek(len1,FileUtil::e_SEEK_FROM_BEGINNING);
            ASSERT(len1 == sts);

            bsl::memset(inBuf2, 'b', sizeof(inBuf2));
            sts = sbFileHandler.read(inBuf2, len2);
            ASSERT(len2 == sts);
            inBuf2[len2] = 0;

            ASSERT(!bsl::strcmp(inBuf2, line2));

            sts = sbFileHandler.seek(-20,   FileUtil::e_SEEK_FROM_CURRENT);
            ASSERT(len1 + len2 - 20 == sts);
            sts = sbFileHandler.seek(-len2, FileUtil::e_SEEK_FROM_END);
            ASSERT(len1 == sts);

            bsl::memset(inBuf2, 'b', sizeof(inBuf2));
            sts = sbFileHandler.read(inBuf2, len2);
            ASSERT(len2 == sts);
            inBuf2[len2] = 0;

            ASSERT(!bsl::strcmp(inBuf2, line2));

            sbFileHandler.release();
            ASSERT(0 == FileUtil::close(fd));
        }

        // map file

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_OPEN,
                                       FileUtil::e_READ_ONLY);
            ASSERT(-1 != (int) fd);

            ObjFileHandler sbFileHandler;
            ASSERT(!sbFileHandler.reset(fd, true, false, true));

            ASSERT(len1 + len2 == sbFileHandler.fileSize());

            {
                char *pc = (char *) sbFileHandler.mmap(0, len1 + len2);
                ASSERT(pc);

                ASSERT(0 == bsl::memcmp(pc, line1, len1));
                ASSERT(0 == bsl::memcmp(pc + len1, line2, len2));

                sbFileHandler.unmap(pc, len1 + len2);
            }

            ASSERT(!sbFileHandler.clear());

            ASSERT(0 == FileUtil::close(fd));

            FileUtil::remove(fnBuf);
        }

        // write, read and seek on file in text mode

        {
            FdType fd = FileUtil::open(fnBuf,
                                       FileUtil::e_CREATE,
                                       FileUtil::e_READ_WRITE);
            ASSERT(-1 != (int) fd);

            ObjFileHandler sbFileHandler;
            ASSERT(!sbFileHandler.reset(fd, true, true, false));

            ASSERT(0 == sbFileHandler.write(line1, len1));
            ASSERT(dLen1 == sbFileHandler.seek(0, FileUtil::e_SEEK_FROM_END));
            ASSERT(0 == sbFileHandler.write(line2, len2));
            ASSERT(dLen1 + dLen2 ==
                        sbFileHandler.seek(0, FileUtil::e_SEEK_FROM_CURRENT));

            ASSERT(dLen1 ==
                      sbFileHandler.seek(-dLen2,
                                         FileUtil::e_SEEK_FROM_CURRENT));

            char inBuf[100];
            bsl::memset(inBuf, 0, sizeof(inBuf));
            int sts = doRead(&sbFileHandler, inBuf, len2);
            LOOP2_ASSERT(len2, sts, len2 == sts);

            ASSERT(!bsl::strcmp(inBuf, line2));

            ASSERT(0 == sbFileHandler.seek(-(dLen1 + dLen2),
                                           FileUtil::e_SEEK_FROM_END));

            bsl::memset(inBuf, 0, sizeof(inBuf));
            sts = doRead(&sbFileHandler, inBuf, len1);
            LOOP2_ASSERT(len1, sts, len1 == sts);

            ASSERT(!bsl::strcmp(inBuf, line1));
        }

         FileUtil::remove(fnBuf);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // bdls::FdStreamBuf HANDLING 5 GIGABYTE FILE
        //
        // Concerns:
        //
        // Plan:
        //   Write a 5G file using an FdStreamBuf::sputn, read it back using
        //   FdStreamBuf::sgetn, verifying the results are accurate.
        //
        // Results:
        //   Succeeds on all Unix 32 bit, fails on Windows 32 bit.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdls::FdStreamBuf 5 Gigabyte file\n"
                             "=================================\n";

        if (verbose) {
            P_(sizeof(bsl::streamoff));  P_(sizeof(bsl::streampos));
            P(sizeof(bsl::streamsize));
        }

#ifdef BSLS_PLATFORM_OS_UNIX
        const char slash = '/';
#else
        const char slash = '\\';
#endif

        cout << "Enter dirname (starts with '" << slash << "')\n" <<
                "where 5 gigabyte file should be put: "
             << bsl::flush;

        bsl::string fn;

        cin >> fn;

        ASSERT(slash == fn.c_str()[0]);
        if (slash != fn.c_str()[fn.length() - 1]) {
            fn += slash;
        }

        ASSERT(FileUtil::isDirectory(fn));

        fn += "bdls_FdStreamBuf.-1.";
        {
            bsl::stringstream s;
            s << getProcessId();

            fn += s.str();
        }

        cout << "Temp file is " << fn << " -- continue(y or n)? " <<
                bsl::flush;
        bsl::string response;

        cin >> response;

        if ("y" != response) {
            cout << "aborted\n";
            break;
        }

        typedef bsls::Types::Int64 Int64;
        const Int64 fileSize = ((Int64) 1 << 30) * 5;    // 5 Gig
        const Int64 halfGig  =  (Int64) 1 << 29;

        FileUtil::remove(fn);

        FdType fd = FileUtil::open(fn.c_str(),
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        Obj sb(fd, true, true, true);

        seedRandChar(0x12345678);

        Int64 rawBufToWrite[(1 << 16) / sizeof(Int64)];
        char *bufToWrite = (char *) rawBufToWrite;
        Int64 *first8Bytes = rawBufToWrite;
        const int sz = sizeof(rawBufToWrite);
        for (char *pc = bufToWrite; pc < bufToWrite + sz; ++pc) {
            *pc = randChar();
        }

        for (Int64 j = 0; j < fileSize; ) {
            Int64 nextGoal = j + halfGig;
            for (; j < nextGoal; j += sz) {
                *first8Bytes = j;
                int sts = sb.sputn(bufToWrite, sz);
                LOOP_ASSERT(sts, sz == sts);
            }
            cout << j << " bytes written\n";
        }

        ASSERT(fileSize == FileUtil::getFileSize(fn));

        sb.pubseekpos(0);

        char readBuf[sz];

        for (Int64 j = 0; j < fileSize; ) {
            Int64 nextGoal = j + halfGig;
            for (; j < nextGoal; j += sz) {
                *first8Bytes = j;
                bsl::memset(readBuf, 0, sz);
                ASSERT(sz == sb.sgetn(readBuf, sz));

                ASSERT(!bsl::memcmp(bufToWrite, readBuf, sz));
            }
            cout << j << " bytes read with 'sgetn'\n";
        }

        ASSERT(!sb.clear());

        ASSERT(fileSize == FileUtil::getFileSize(fn));

        fd = FileUtil::open(fn,
                            FileUtil::e_OPEN,
                            FileUtil::e_READ_ONLY);
        ASSERT(!sb.reset(fd, true, true, true));

        for (Int64 j = 0; j < fileSize; ) {
            Int64 nextGoal = j + halfGig;
            for (; j < nextGoal; j += sz) {
                *first8Bytes = j;
                bsl::memset(readBuf, 0, sz);
                ASSERT(sz == sb.sgetn(readBuf, sz));

                ASSERT(!bsl::memcmp(bufToWrite, readBuf, sz));
            }
            cout << j << " bytes read with 'sgetn' after close, open\n";
        }

        sb.release();

        FileUtil::seek(fd, 0, FileUtil::e_SEEK_FROM_BEGINNING);

        for (Int64 j = 0; j < fileSize; ) {
            Int64 nextGoal = j + halfGig;
            for (; j < nextGoal; j += sz) {
                *first8Bytes = j;
                bsl::memset(readBuf, 0, sz);
                ASSERT(sz == FileUtil::read(fd, readBuf, sz));

                ASSERT(!bsl::memcmp(bufToWrite, readBuf, sz));
            }
            cout << j << " bytes read wtih 'FileUtil::read()'\n";
        }

        ASSERT(0 == FileUtil::close(fd));

        FileUtil::remove(fn);
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // rancChar() test
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        for (int j = 0; j < 100; ++j) {
            cout << (int) randChar() << endl;
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // 5G FILE WITH OSTREAM / ISTREAM BASED ON FDSTREAMBUF
        //
        //
        // Results:
        //   This test worked on all Unix platforms, 32 bit, and on Windows 32
        //   bit.  The fact that this test succeeds on Windows 32 bit is
        //   surprising given that test case -1 failed there.
        // --------------------------------------------------------------------

        if (verbose) cout << "5G file with stream I/O test\n"
                             "============================\n";

#ifdef BSLS_PLATFORM_OS_UNIX
        const char slash = '/';
        const char *nl = " \n";
#else
        const char slash = '\\';
        const char *nl = "\n";
#endif

        cout << "Enter dirname (starts with '" << slash << "')\n" <<
                "where 5 gigabyte file should be put: "
             << bsl::flush;

        bsl::string fn;

        cin >> fn;

        ASSERT(slash == fn.c_str()[0]);
        if (slash != fn.c_str()[fn.length() - 1]) {
            fn += slash;
        }

        ASSERT(FileUtil::isDirectory(fn));

        fn += "bdls_FdStreamBuf.-3.";
        {
            bsl::stringstream s;
            s << getProcessId();

            fn += s.str();
        }

        cout << "Temp file is " << fn << " -- continue(y or n)? " <<
                bsl::flush;
        bsl::string response;

        cin >> response;

        if ("y" != response) {
            cout << "aborted\n";
            break;
        }

        typedef bsls::Types::Int64 Int64;
        const Int64 fileSize = ((Int64) 1 << 30) * 5;    // 5 Gig

        FileUtil::remove(fn);

        FdType fd = FileUtil::open(fn.c_str(),
                                   FileUtil::e_CREATE,
                                   FileUtil::e_READ_WRITE);
        ASSERT(-1 != (int) fd);

        Obj sb(fd, true, true, false);

        Int64 bytesWritten   = 0;
        Int64 mileStone      = 1 << 29;
        Int64 deltaMileStone = 1 << 29;

        Int64 numToWrite = 100000;

        bsl::ostream os(&sb);

        while (bytesWritten < fileSize) {
            os << ++numToWrite << nl;
            bytesWritten += digits(numToWrite) + 2;
            if (bytesWritten >= mileStone) {
                cout << bytesWritten << " bytes written\n";
                mileStone += deltaMileStone;
            }
        }

        ASSERT(!sb.clear());

        LOOP2_ASSERT(bytesWritten, FileUtil::getFileSize(fn),
                                   bytesWritten == FileUtil::getFileSize(fn));

        fd = FileUtil::open(fn,
                            FileUtil::e_OPEN,
                            FileUtil::e_READ_ONLY);
        ASSERT(!sb.reset(fd, true, true, false));

        bsl::istream is(&sb);
        Int64 expected = 100000;

        Int64 bytesRead = 0;
        mileStone = deltaMileStone;

        while (bytesRead < fileSize) {
            Int64 x;
            is >> x;
            if (x != ++expected) {
                LOOP2_ASSERT(x, expected, x == expected);
                break;
            }
            bytesRead += digits(x) + 2;
            if (bytesRead >= mileStone) {
                cout << bytesRead << " bytes read\n";
                mileStone += deltaMileStone;
            }
        }

        ASSERT(0 == FileUtil::close(fd));

        FileUtil::remove(fn);
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // WRITE, READ 5G FILE WITH PLAIN FSTREAM
        //
        // Results:
        //   32 bit: This test succeeds on Linux, Solaris, and Windows, fails
        //   on HPUX and AIX.
        // --------------------------------------------------------------------

        if (verbose) cout << "5G file with fstream I/O test, no FdStreamBuf\n"
                             "=============================================\n";

#ifdef BSLS_PLATFORM_OS_UNIX
        const char slash = '/';
        const char *nl = " \n";
#else
        const char slash = '\\';
        const char *nl = "\n";
#endif

        cout << "Enter dirname (starts with '" << slash << "')\n" <<
                "where 5 gigabyte file should be put: "
             << bsl::flush;

        bsl::string fn;

        cin >> fn;

        ASSERT(slash == fn.c_str()[0]);
        if (slash != fn.c_str()[fn.length() - 1]) {
            fn += slash;
        }

        ASSERT(FileUtil::isDirectory(fn));

        fn += "bdls_FdStreamBuf.-4.";
        {
            bsl::stringstream s;
            s << getProcessId();

            fn += s.str();
        }

        cout << "Temp file is " << fn << " -- continue(y or n)? " <<
                bsl::flush;
        bsl::string response;

        cin >> response;

        if ("y" != response) {
            cout << "aborted\n";
            break;
        }

        typedef bsls::Types::Int64 Int64;
        const Int64 fileSize = ((Int64) 1 << 30) * 5;    // 5 Gig

        FileUtil::remove(fn);

        bsl::fstream fs(fn.c_str(), bsl::ios_base::out);

        Int64 bytesWritten   = 0;
        Int64 mileStone      = 1 << 29;
        Int64 deltaMileStone = 1 << 29;

        Int64 numToWrite = 100000;

        while (bytesWritten < fileSize) {
            fs << ++numToWrite << nl;
            bytesWritten += digits(numToWrite) + 2;
            if (bytesWritten >= mileStone) {
                cout << bytesWritten << " bytes written\n";
                mileStone += deltaMileStone;
            }
        }

        fs.close();

        fs.open(fn.c_str(), bsl::ios_base::in);

        LOOP2_ASSERT(bytesWritten, FileUtil::getFileSize(fn),
                                   bytesWritten == FileUtil::getFileSize(fn));

        Int64 expected = 100000;

        Int64 bytesRead = 0;
        mileStone = deltaMileStone;

        while (bytesRead < fileSize) {
            Int64 x;
            fs >> x;
            if (x != ++expected) {
                LOOP2_ASSERT(x, expected, x == expected);
                break;
            }
            bytesRead += digits(x) + 2;
            if (bytesRead >= mileStone) {
                cout << bytesRead << " bytes read\n";
                mileStone += deltaMileStone;
            }
        }

        fs.close();

        FileUtil::remove(fn);
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
