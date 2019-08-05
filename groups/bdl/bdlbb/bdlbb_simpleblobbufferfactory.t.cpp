// bdlbb_simpleblobbufferfactory.t.cpp                                -*-C++-*-

#include <bdlbb_simpleblobbufferfactory.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstdio.h>      // 'fopen', etc
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <direct.h>
#endif

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] SimpleBlobBufferFactory(int, Allocator *);
// [ 2] ~SimpleBlobBufferFactory();
// [ 3] SimpleBlobBufferFactory(int);
//
// MANIPULATORS
// [ 2] void allocate(BlobBuffer *);
// [ 2] void setBufferSize(int);
//
// ACCESSORS
// [ 2] int bufferSize() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] SimpleBlobBufferFactory with Blob
// [ 4] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlbb::SimpleBlobBufferFactory Factory;
typedef bsls::Types::Int64             Int64;
using   bdlbb::BlobBuffer;
using   bdlbb::Blob;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslmf::IsBitwiseMoveable< Factory>::value);
BSLMF_ASSERT(bslma::UsesBslmaAllocator<Factory>::value);

// ============================================================================
//                             GLOBAL TEST FUNCTIONS
// ----------------------------------------------------------------------------

namespace {
namespace u {

char getAlphaData = 0;
char getAlpha()
    // Return a lowercase alphabetic character, incrementing one each time,
    // returning to 'a' after reaching 'z'.  If 'reset' is 'true', reset the
    // the counter so that the next call will return 'a'.
{
    return !getAlphaData || 'z' == getAlphaData ? (getAlphaData = 'a')
                                                : ++getAlphaData;
}

void fillAlpha(const BlobBuffer& blobBuffer)
    // Fill the memory at the specified 'blobBuffer' with the lower case
    // alphabet, repeating if necessary.
{
    char *dst    = &*blobBuffer.buffer();
    int   length = blobBuffer.size();

    getAlphaData = 0;
    for (const char * const end = dst + length; dst < end; ++dst) {
        *dst = getAlpha();
    }
}

bool checkAlpha(const BlobBuffer& blobBuffer)
    // Check that the memory of the specified 'blobBuffer' is filled with the
    // lower case alphabet, repeating if necessary.  Return 'true' if the
    // memory all matches and 'false' otherwise.
{
    const char *src    = &*blobBuffer.buffer();
    const int   length = blobBuffer.size();

    getAlphaData = 0;
    for (const char * const end = src + length; src < end; ++src) {
        if (getAlpha() != *src) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool fileExists(const char *fileName)
    // Return 'true' if the specified 'fileName' exists and 'false' otherwise.
    // Note that 'fileName' may specify a directory.
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    struct stat s;
    return 0 == ::stat(fileName, &s);
#else
    return INVALID_FILE_ATTRIBUTES != GetFileAttributesA(fileName);
#endif
}

bool isAtTopLevelDir()
    // Return 'true' if the current working directory is at the top of the file
    // system and 'false' otherwise.
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

#if defined(BSLS_PLATFORM_OS_UNIX)
    return !bsl::strcmp("/", cwd);
#else
    const char *pc = bsl::strchr(cwd, ':');
    pc = pc ? pc + 1 : cwd;
    return !bsl::strcmp("\\", pc);
#endif
}

void moveUpOneDir()
    // Change the current working directory to the parent of the current one.
    // The behavior is undefined if we were already at the top level.
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    ::chdir("..");
#else
    _chdir("..");
#endif
}

void fillSlashes(char *string)
    // Substitute all '+'s in the specified 'string' with a slash character
    // appropriate for the OS.
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    const char slash = '/';
#else
    const char slash = '\\';
#endif

    for (; *string; ++string) {
        if ('+' == *string) {
            *string = slash;
        }
    }
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;    (void)             verbose;
    bool         veryVerbose = argc > 3;    (void)         veryVerbose;
    bool     veryVeryVerbose = argc > 4;    (void)     veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator ta("ta",      veryVeryVeryVerbose);
    bslma::TestAllocator sa("sa",      veryVeryVeryVerbose);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&da);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Simple Blob Buffer Factory
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to make a blob that can be grown via calls to 'setLength',
// meaning that it must have a factory, and suppose you want all the memory for
// the blob buffers created for the factory to be allocated directly from a
// certain test allocator for test purposes.  We use a
// 'SimpleBlobBufferFactory'.
//..
// First, we create our allocator:
//..
    bslma::TestAllocator testAllocator;
//..
// Then, we create our factor using that allocator:
//..
    enum { k_BUFFER_SIZE = 1024 };

    bdlbb::SimpleBlobBufferFactory factory(k_BUFFER_SIZE, &testAllocator);
    ASSERT(factory.bufferSize() == k_BUFFER_SIZE);
//..
// Next, we create our blob using that factory:
//..
    bdlbb::Blob blob(&factory);
//..
// Next, we set the length big enough to require 20 blob buffers:
//..
    blob.setLength(1024 * 20);
//..
// Then, we verify that the memory came from 'testAllocator'.  Note that since
// the blob buffers contain shared pointers, additional memory other than the
// writable areas of the blob buffers is allocated:
//..
    ASSERT(1024 * 20 < testAllocator.numBytesInUse());
//..
// Now, we examine the state of the blob:
//..
    ASSERT(20 == blob.numDataBuffers());
    ASSERT(20 == blob.numBuffers());
    ASSERT(1024 * 20 == blob.length());
//..
// Finally, we examine the blob buffers:
//..
    for (int ii = 0; ii < blob.numDataBuffers(); ++ii) {
        ASSERT(k_BUFFER_SIZE == blob.buffer(ii).size());
    }
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: INTER-OPERABILITY WITH BLOB
        //
        // Concerns:
        //: 1 That the 'SimpleBlobBufferFactory' can be used correctly with
        //:   a 'Blob'.
        //:
        //: 2 Test calling the constructor, both with and without an allocator
        //:   specified.
        //
        // Plan:
        //: 1 Create a 'Blob' with a 'SimpleBlobBufferFactory' and have it
        //:   create a lot of blobs, write data to them, and read it back
        //:   again.
        //:
        //: 2 Open the source for this test driver in this directory, read it
        //:   all into a blob, then read it again and verify the all the
        //:   characters in the file match the characters in the blob.
        //:
        //: 3 Use 'bsl_cstdio.h' for I/O, to avoid needing to depend on
        //:   'bdls_filesystemutil', since 'bdlbb' has no dependency on 'bdls'.
        //
        // Testing:
        //   SimpleBlobBufferFactory with Blob
        //   SimpleBlobBufferFactory(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: INTER-OPERABILITY WITH BLOB\n"
                             "====================================\n";

        enum { bufferSize = 128 };

        // Move up to the directory containing the 'bde' directory, or fail if
        // we reach the top level first.

        while (!u::fileExists("bde")) {
            if (u::isAtTopLevelDir()) {
                ASSERT(0 && "reached top level without finding 'bde'");
                break;    // 'fopen' below will fail and test will abort
            }

            u::moveUpOneDir();
        }

        if (verbose) cout << "Open the test driver.\n";

        char filePath[] = {
                  "bde+groups+bdl+bdlbb+bdlbb_simpleblobbufferfactory.t.cpp" };
        u::fillSlashes(filePath);
        FILE *fp = bsl::fopen(filePath, "rb");
        if (!fp) {
            ASSERTV(filePath, 0 && "unable to fopen file");
            break;
        }

        if (verbose) cout <<
                       "Set 'fLen', the length in bytes of the test driver.\n";

        int rc = bsl::fseek(fp, 0, SEEK_END);
        BSLS_ASSERT(0 == rc);
        const int fLen = static_cast<int>(bsl::ftell(fp));
        ASSERTV(fLen, 5 * 1000 < fLen);

        if (verbose) P(fLen);

        for (int ti = 0; ti < 2; ++ti) {
            bool defaultMemory = ti & 1;

            if (veryVerbose) cout << (0 == ti ? "" : "\n") << "Pass: " << ti <<
                         (defaultMemory ? " using default memory\n"
                                        : " using memory supplied to c'tor\n");

            bsl::rewind(fp);

            if (veryVerbose) cout <<
                         "Create our factory and a blob using that factory.\n";

            Factory  daFactory(bufferSize);    // default allocator == 'da'
            Factory  taFactory(bufferSize, &ta);
            Factory& factory = defaultMemory ? daFactory : taFactory;
            ASSERT(factory.bufferSize() == bufferSize);
            Blob     blob(&factory, &sa);

            if (veryVerbose) cout <<
                "Traverse the file and load its contents into the blob,\n"
                "growing as needed.  Do a few sanity checks.\n";

            int c, blobLen = 0;
            while (EOF != (c = getc(fp))) {
                ASSERTV(c, blobLen, 0 < c && c < 128);    // ascii

                const int          iBuf  = blobLen / bufferSize;
                const int          iChar = blobLen % bufferSize;
                blob.setLength(++blobLen);
                const BlobBuffer&  bBuf  = blob.buffer(iBuf);
                ASSERTV(bBuf.size(), bufferSize == bBuf.size());
                ASSERTV(iChar, blob.lastDataBufferLength(),
                                     iChar + 1 == blob.lastDataBufferLength());
                ASSERTV(blob.numDataBuffers(), blob.numBuffers(),
                                   blob.numDataBuffers() == blob.numBuffers());
                char              *buf   = &*bBuf.buffer();

                buf[iChar] = static_cast<char>(c);
            }
            ASSERT(EOF == getc(fp));
            ASSERTV(fLen, blobLen, fLen == blobLen);
            ASSERTV(fLen, blob.length(), fLen == blob.length());

            if (veryVerbose) cout <<
                "Examing memory consumption.  Note that the blob itself\n"
                "allocates memory from 'sa', a different source.  Also note\n"
                "that since the blob buffers contain shared pointers, the\n"
                "total memory allocated by the factory's allocator will\n"
                "exceed 'fLen'.\n";

            if (defaultMemory) {
                ASSERT(da.numBytesInUse() > fLen);
                ASSERT(ta.numBytesInUse() == 0);
            }
            else {
                ASSERT(da.numBytesInUse() == 0);
                ASSERT(ta.numBytesInUse() > fLen);
            }

            if (veryVerbose) {
                cout << "Finished reading blob:\n";
                P_(blob.numBuffers());    P(blob.numDataBuffers());
                P_(blob.length());        P(blob.lastDataBufferLength());
            }

            if (veryVerbose) cout <<
                    "Traverse the file and the blob again, this time\n"
                    "comparing the file's contents with the contents of the\n"
                    "blob.\n";

            int numChars = 0;
            bsl::rewind(fp);
            for (int iBuf = 0; iBuf < blob.numBuffers(); ++iBuf) {
                const BlobBuffer&  bBuf   = blob.buffer(iBuf);
                const char        *buf    = bBuf.data();
                const int          bufLen = blob.numBuffers() - 1 == iBuf
                                          ? blob.lastDataBufferLength()
                                          : bBuf.size();
                for (int ii = 0; ii < bufLen; ++ii, ++numChars) {
                    const int c = bsl::getc(fp);
                    ASSERTV(c, iBuf, ii, 0 < c && c < 128);    // ascii
                    ASSERTV(iBuf, ii, c == buf[ii]);

                    if (veryVeryVerbose) cout << buf[ii];
                }
            }
            ASSERT(EOF == getc(fp))
            ASSERT(fLen == numChars);

            if (0 == ti) {
                ASSERT(!defaultMemory);
                ASSERT(0 == da.numAllocations());
            }
            else {
                ASSERT(0 <  da.numAllocations());
            }
        }

        bsl::fclose(fp);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: BASIC FUNCTIONALITY OF CLASS
        //
        // Concerns:
        //: 1 Test the basic functionality of the class
        //:   o primary constructor
        //:   o manipulators
        //:   o accessor
        //:
        //: 2 The factory is capable of producing buffers of any desired size.
        //:
        //: 3 The factory can vary the buffer size and subsequent buffers will
        //:   be of that size.
        //:   o the first blob buffers created will be at the size specified
        //:     at construction
        //:   o if 'setBufferSize' is called, subsequent buffers are created
        //:     of that size
        //:
        //: 4 The blob buffers created are functional and can hold data, and
        //:   free up their memory when destroyed.
        //
        // Plan:
        //: 1 Iterate over a set of random blob sizes.  Use the 'setBufferSize'
        //:   function at the beginning of each iteration to set the blob
        //:   buffer size.
        //:
        //: 2 Create a function 'u::fileAlpha' which will populate the memory
        //:   of a blob buffer with the lower case alphabet, repeated if need
        //:   be.
        //:
        //: 3 Create a function 'u::checkAlpha' which will verify that a blob
        //:   has been entirely filled with the lower case alphabet, repeated
        //:   if need be.
        //:
        //: 4 In an inner loop, create 5 blob buffers of the given size:
        //:   o verify that their length is correct
        //:   o verify that the memory allocated grew by an amount greater than
        //:     the blob size (due to shared ptr overhead)
        //:   o fill the memory with data with 'fillAlpha'.
        //:   o verify the memory has been correctly set with 'checkAlpha'.
        //:   o store the blob into a 'deque'
        //:
        //: 5 Iterate through all the blob buffers in the 'deque'.
        //:   o verify that their size is correct
        //:   o verify that the data in them is correct with 'checkAlpha'.
        //:
        //: 6 Iterate, popping items from the back of the deque and observe the
        //:   memory in use decreasing as expected as the blob buffers free
        //:   their managed memory.
        //
        // Testing:
        //   SimpleBlobBufferFactory(int, Allocator *);
        //   ~SimpleBlobBufferFactory();
        //   void allocate(BlobBuffer *);
        //   void setBufferSize(int);
        //   int bufferSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: BASIC FUNCTIONALITY OF CLASS\n"
                             "=====================================\n";

        ASSERT(0 == ta.numAllocations());

        int sizes[] = { 8, 100, 30, 10 * 1000, 47, 200 };
        enum { k_NUM_SIZES = sizeof sizes / sizeof *sizes };

        Factory factory(sizes[0], &ta);
        bsl::deque<BlobBuffer> bBufs(&sa);
        bsl::deque<Int64>      allocs(&sa);

        Int64 alloced = ta.numBytesInUse();
        ASSERT(0 == alloced);

        if (verbose) cout << "Iterate, creating & populating blob buffers\n";

        int prevSize = sizes[0], size;
        for (int ii = 0; ii < k_NUM_SIZES; ++ii, prevSize = size) {
            size = sizes[ii];

            if (veryVerbose) { P_(ii);    P(size); }

            if (0 < ii) {
                ASSERT(factory.bufferSize() == prevSize);
                factory.setBufferSize(size);
            }

            for (int jj = 0; jj < 5; ++jj) {
                if (veryVeryVerbose) cout << '.';

                ASSERT(factory.bufferSize() == size);

                BlobBuffer bb;
                factory.allocate(&bb);
                ASSERT(bb.size() == size);

                Int64 memoryGrowth = ta.numBytesInUse() - alloced;
                ASSERT(memoryGrowth >= size);
                allocs.push_back(memoryGrowth);
                alloced = ta.numBytesInUse();

                u::fillAlpha(bb);
                ASSERT(u::checkAlpha(bb));

                bBufs.push_back(bb);
            }

            if (veryVeryVerbose) cout << endl;
        }

        if (verbose) cout <<
                 "Traverse the blob buffers and verify that the data is\n"
                 "    still there\n";

        for (unsigned uu = 0; uu < bBufs.size(); ++uu) {
            size = sizes[uu / 5];

            BlobBuffer bb = bBufs[uu];

            ASSERTV(uu, size, bb.size() == size);
            ASSERTV(uu, u::checkAlpha(bb));

            ASSERTV(uu, alloced, ta.numBytesInUse(),
                                                ta.numBytesInUse() == alloced);
        }

        if (verbose) cout <<
                 "Randomly destroy buffers and observe that, in each case,\n"
                 "    exactly the amount of memory that was consumed in\n"
                 "    their creation is freed up.\n";

        while (!bBufs.empty()) {
            const int ii = bsl::rand() % static_cast<int>(bBufs.size());
            const Int64 before    = ta.numBytesInUse();
            const Int64 memoryUse = allocs[ii];
            allocs.erase(allocs.begin() + ii);
            bBufs. erase(bBufs.begin()  + ii);
            ASSERT(ta.numBytesInUse() == before - memoryUse);
        }

        ASSERT(0 == ta.numBlocksInUse());
      } break;
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
        //: 1 Create a factory and use it to create many buffers, keeping them
        //:   in a vector.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        const bsl::size_t bufferSize = 128;
        const int         numBuffers = 256;

        bsls::Types::Uint64 inUse, allocSize = 0;
        Factory factory(bufferSize, &ta);
        bsl::vector<BlobBuffer> buffers(&sa);
        buffers.reserve(numBuffers);
        for (int ii = 0; ii < numBuffers; ++ii) {
            BlobBuffer buf;
            factory.allocate(&buf);

            bsl::memset(&*buf.buffer(), 'a', bufferSize);
            const char *start = buf.data(), *end = start + bufferSize;
            for (const char *pc = start; pc < end; ++pc) {
                ASSERTV(*pc, 'a' == *pc);
            }

            buffers.push_back(buf);

            inUse = ta.numBytesInUse();
            if (!allocSize) {
                allocSize = inUse;
            }
            ASSERTV(inUse, buffers.size(), allocSize,
                                          inUse == allocSize * buffers.size());
        }

        inUse = ta.numBytesInUse();
        ASSERTV(inUse, buffers.size(), allocSize,
                                          inUse == allocSize * buffers.size());

        buffers.clear();
        ASSERT(0 == ta.numBytesInUse());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
