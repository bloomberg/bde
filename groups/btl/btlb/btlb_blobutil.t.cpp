// btlb_blobutil.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlb_blobutil.h>
#include <btlb_blob.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslx_genericoutstream.h>
#include <bslx_testoutstream.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcpy'
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 9] Testing getContiguousRangeOrCopy
// [ 8] Testing getContiguousDataBuffer
// [ 7] Testing copy
// [ 6] Testing findBufferIndexAndOffset
// [ 5] Testing erase
// [ 4] Testing HexDump with offset and length
// [ 3] Testing HexDump
// [ 2] Testing compare
// [ 1] Testing "write special cases"

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
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                         // =======================
                         // class BlobBufferFactory
                         // =======================

class BlobBufferFactory : public btlb::BlobBufferFactory {
    // TBD: doc

    // PRIVATE DATA MEMBERS
    int               d_size;
    bslma::Allocator *d_allocator_p;

  private:
    // Not implemented:
    BlobBufferFactory(const BlobBufferFactory&);

  public:
    // CREATORS
    BlobBufferFactory(int initialSize, bslma::Allocator *basicAllocator = 0)
    : d_size(initialSize)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    virtual ~BlobBufferFactory()
    {
    }

    // MANIPULATORS
    virtual void allocate(btlb::BlobBuffer *buffer)
    {
        bsl::shared_ptr<char> shptr((char*)d_allocator_p->allocate(d_size),
                                    d_allocator_p);
        buffer->reset(shptr, d_size);
        *buffer->data() = '#';
    }

    virtual void setBufferSize(int bufferSize)
    {
        d_size = bufferSize;
    }

    // ACCESSORS
    virtual int bufferSize() const
    {
        return d_size;
    }
};

// ============================================================================
//                              GLOBAL TYPEDEF
// ----------------------------------------------------------------------------

typedef btlb::BlobUtil Util;

int verbose;
int veryVerbose;
int veryVeryVerbose;

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions generate a string of repeating characters in the
// range [a-z] of a specified length.

int ggg(bsl::string *result, int length)
{
    // Clear the specified 'result' and append a subset of the string
    // "abcdefghijklmnopqrstuvwxyz", repeating the pattern until the total
    // length of 'result' reaches the specified 'length'.

    const char DATA[] = "abcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyz";
    enum { k_DATA_SIZE = sizeof DATA - 1 };

    int capacity = length;
    result->clear();
    result->reserve(capacity);
    do {
        int nbytes = (capacity >= k_DATA_SIZE) ? k_DATA_SIZE : capacity;
        result->append(DATA, nbytes);
        capacity -= nbytes;
    } while (capacity > 0);
    ASSERT(result->length() == size_t(length));
    return 0;
}

bsl::string& gg(bsl::string *result, int length)
{
    // Return, by reference, the specified 'result' with its value adjusted
    // according to the specified 'length'.

    ASSERT(0 == ggg(result, length));
    return *result;
}

bsl::string g(int length)
{
    // Return, by value, a new object corresponding to the specified 'spec'.

    bsl::string object;
    return gg(&object, length);
}

// ============================================================================
//                             HELPER FUNCTIONS
// ----------------------------------------------------------------------------

void copyStringToBlob(btlb::Blob *dest, const bsl::string& str)
{
    dest->setLength(static_cast<int>(str.length()));
    int numBytesRemaining = static_cast<int>(str.length());
    const char *data = str.data();
    int bufferIndex = 0;
    while (numBytesRemaining) {
        btlb::BlobBuffer buffer = dest->buffer(bufferIndex);
        int numBytesToCopy = bsl::min(numBytesRemaining, buffer.size());
        bsl::memcpy(buffer.data(), data, numBytesToCopy);
        data += numBytesToCopy;
        numBytesRemaining -= numBytesToCopy;
        ++bufferIndex;
    }
    ASSERT(0 == numBytesRemaining);
}

void copyBlobToString(bsl::string *dest, const btlb::Blob& blob)
{
    dest->clear();
    int numBytesRemaining = blob.length();
    int bufferIndex = 0;
    while (numBytesRemaining) {
        btlb::BlobBuffer buffer = blob.buffer(bufferIndex);
        int numBytesToCopy = bsl::min(numBytesRemaining, buffer.size());
        dest->append(buffer.data(), buffer.data() + numBytesToCopy);
        numBytesRemaining -= numBytesToCopy;
        ++bufferIndex;
    }
    ASSERT(0 == numBytesRemaining);
}

bsl::string expectedOutCase3[] = {
    "",

    "     0:   61626364 65662031 61626364 65662032     |abcdef 1abcdef 2|\n"
    "    16:   61626364 65662033 61626364 65662034     |abcdef 3abcdef 4|\n"
    "    32:   61626364 65662035 61626364 65662036     |abcdef 5abcdef 6|\n"
    "    48:   61626364 65662037 61626364 65662038     |abcdef 7abcdef 8|\n"
    "    64:   61626364 65662039 61626364 65203130     |abcdef 9abcde 10|\n"
    "    80:   61626364 65203131 61626364 65203132     |abcde 11abcde 12|\n"
    "    96:   61626364 65203133 61626364 65203134     |abcde 13abcde 14|\n"
    "   112:   61626364 65203135 61626364 65203136     |abcde 15abcde 16|\n"
    "   128:   61626364 65203137 61626364 65203138     |abcde 17abcde 18|\n"
    "   144:   61626364 65203139 61626364 65203230     |abcde 19abcde 20|\n"
    "   160:   61626364 65203231 61626364 65203232     |abcde 21abcde 22|\n"
    "   176:   61626364 65203233 61626364 65203234     |abcde 23abcde 24|\n"
    "   192:   61626364 65203235 61626364 65203236     |abcde 25abcde 26|\n"
    "   208:   61626364 65203237 61626364 65203238     |abcde 27abcde 28|\n"
    "   224:   61626364 65203239 61626364 65203330     |abcde 29abcde 30|\n"
    "   240:   61626364 65203331                       |abcde 31        |",

    "     0:   61626364 65662031 61626364 65662032     |abcdef 1abcdef 2|\n"
    "    16:   61626364 65662033 61626364 65662034     |abcdef 3abcdef 4|\n"
    "    32:   61626364 65662035 61626364 65662036     |abcdef 5abcdef 6|\n"
    "    48:   61626364 65662037 61626364 65662038     |abcdef 7abcdef 8|\n"
    "    64:   61626364 65662039 61626364 65203130     |abcdef 9abcde 10|\n"
    "    80:   61626364 65203131 61626364 65203132     |abcde 11abcde 12|\n"
    "    96:   61626364 65203133 61626364 65203134     |abcde 13abcde 14|\n"
    "   112:   61626364 65203135 61626364 65203136     |abcde 15abcde 16|\n"
    "   128:   61626364 65203137 61626364 65203138     |abcde 17abcde 18|\n"
    "   144:   61626364 65203139 61626364 65203230     |abcde 19abcde 20|\n"
    "   160:   61626364 65203231 61626364 65203232     |abcde 21abcde 22|\n"
    "   176:   61626364 65203233 61626364 65203234     |abcde 23abcde 24|\n"
    "   192:   61626364 65203235 61626364 65203236     |abcde 25abcde 26|\n"
    "   208:   61626364 65203237 61626364 65203238     |abcde 27abcde 28|\n"
    "   224:   61626364 65203239 61626364 65203330     |abcde 29abcde 30|\n"
    "   240:   61626364 65203331                       |abcde 31        |",

    "     0:   61626364 65662031 61626364 65662032     |abcdef 1abcdef 2|\n"
    "    16:   61626364 65662033 61626364 65662034     |abcdef 3abcdef 4|\n"
    "    32:   61626364 65662035 61626364 65662036     |abcdef 5abcdef 6|\n"
    "    48:   61626364 65662037 61626364 65662038     |abcdef 7abcdef 8|\n"
    "    64:   61626364 65662039 61626364 65203130     |abcdef 9abcde 10|\n"
    "    80:   61626364 65203131 61626364 65203132     |abcde 11abcde 12|\n"
    "    96:   61626364 65203133 61626364 65203134     |abcde 13abcde 14|\n"
    "   112:   61626364 65203135 61626364 65203136     |abcde 15abcde 16|\n"
    "   128:   61626364 65203137 61626364 65203138     |abcde 17abcde 18|\n"
    "   144:   61626364 65203139 61626364 65203230     |abcde 19abcde 20|\n"
    "   160:   61626364 65203231 61626364 65203232     |abcde 21abcde 22|\n"
    "   176:   61626364 65203233 61626364 65203234     |abcde 23abcde 24|\n"
    "   192:   61626364 65203235 61626364 65203236     |abcde 25abcde 26|\n"
    "   208:   61626364 65203237 61626364 65203238     |abcde 27abcde 28|\n"
    "   224:   61626364 65203239 61626364 65203330     |abcde 29abcde 30|\n"
    "   240:   61626364 65203331 61626364 65203332     |abcde 31abcde 32|",

    "     0:   61626364 65662031 61626364 65662032     |abcdef 1abcdef 2|\n"
    "    16:   61626364 65662033 61626364 65662034     |abcdef 3abcdef 4|\n"
    "    32:   61626364 65662035 61626364 65662036     |abcdef 5abcdef 6|\n"
    "    48:   61626364 65662037 61626364 65662038     |abcdef 7abcdef 8|\n"
    "    64:   61626364 65662039 61626364 65203130     |abcdef 9abcde 10|\n"
    "    80:   61626364 65203131 61626364 65203132     |abcde 11abcde 12|\n"
    "    96:   61626364 65203133 61626364 65203134     |abcde 13abcde 14|\n"
    "   112:   61626364 65203135 61626364 65203136     |abcde 15abcde 16|\n"
    "   128:   61626364 65203137 61626364 65203138     |abcde 17abcde 18|\n"
    "   144:   61626364 65203139 61626364 65203230     |abcde 19abcde 20|\n"
    "   160:   61626364 65203231 61626364 65203232     |abcde 21abcde 22|\n"
    "   176:   61626364 65203233 61626364 65203234     |abcde 23abcde 24|\n"
    "   192:   61626364 65203235 61626364 65203236     |abcde 25abcde 26|\n"
    "   208:   61626364 65203237 61626364 65203238     |abcde 27abcde 28|\n"
    "   224:   61626364 65203239 61626364 65203330     |abcde 29abcde 30|\n"
    "   240:   61626364 65203331 61626364 65203332     |abcde 31abcde 32|\n"
    "   256:   61626364 65203333                       |abcde 33        |"
};

static bool bad_jk(int j, int k, btlb::Blob& blob)
{
    return (j < 0 || k < 0 || j + k > blob.totalSize());
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failThrow);
    // The line above will not be needed once 'bsls' is updated.

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
        case 9: {
        // -------------------------------------------------------------------
        // TESTING 'getContiguousRangeOrCopy' FUNCTION
        //
        // Concerns:
        //   BLACK BOX:
        //  1 Works for all boundary conditions:
        //    1 default-constructed blob
        //    2 blobs with 1, 2, 3 buffers in varied arrangements
        //      position at varied alignments, and length at varied positions,
        //  2 Works for sample non-boundary conditions
        //  3 Catches bad arguments
        //  4 Tests boundaries I am not smart enough to think of
        //
        // Plan:
        //  1 Create a table defining blobs with varying composition
        //  2 Create blobs to specifications, operate on each, check results
        //  3 Test a tractable space exhaustively, up to 16-byte alignment
        //
        // Testing:
        //    char *BlobUtil::getContiguousRangeOrCopy(char* dstBuffer,
        //          const btlb::Blob&, int position, int length, int alignment)
        //
        // --------------------------------------------------------------------

        verbose && (cout << "\nTesting 'getContiguousRangeOrCopy' Function"
                            "\n===========================================\n");

        static const int MAXALN = 16;
        static const int BIG = MAXALN * 2;
        static const struct {
            int d_line;
            int d_numBufs;
            int d_sizes[3];
        } DATA[] = {
        //      NUMBUFS
        //  LINE  |    +-USE-+
            { L_, 0, { 0, 0, 0 } },
            { L_, 1, { 1, 0, 0 } },
            { L_, 1, { 2, 0, 0 } },
            { L_, 1, { 3, 0, 0 } },

            { L_, 2, { 0, 0, 0 } },
            { L_, 2, { 1, 0, 0 } },
            { L_, 2, { 0, 1, 0 } },
            { L_, 2, { 1, 2, 0 } },
            { L_, 2, { 2, 0, 0 } },
            { L_, 2, { 0, 2, 0 } },
            { L_, 2, { 2, 1, 0 } },
            { L_, 2, { 2, 2, 0 } },
            { L_, 2, { 3, 0, 0 } },
            { L_, 2, { 0, 3, 0 } },
            { L_, 2, { 3, 2, 0 } },
            { L_, 2, { 2, 3, 0 } },
            { L_, 2, { 3, 1, 0 } },
            { L_, 2, { 1, 3, 0 } },
            { L_, 2, { 3, 3, 0 } },

            { L_, 3, { 0, 0, 0 } },
            { L_, 3, { 1, 0, 0 } },
            { L_, 3, { 0, 1, 0 } },
            { L_, 3, { 0, 0, 1 } },
            { L_, 3, { 1, 1, 0 } },
            { L_, 3, { 0, 1, 1 } },
            { L_, 3, { 1, 0, 1 } },
            { L_, 3, { 1, 1, 1 } },

            { L_, 3, { 2, 0, 0 } },
            { L_, 3, { 0, 2, 0 } },
            { L_, 3, { 0, 0, 2 } },
            { L_, 3, { 2, 2, 0 } },
            { L_, 3, { 0, 2, 2 } },
            { L_, 3, { 2, 0, 2 } },
            { L_, 3, { 2, 2, 2 } },

            { L_, 3, { 3, 0, 0 } },
            { L_, 3, { 0, 3, 0 } },
            { L_, 3, { 0, 0, 3 } },
            { L_, 3, { 3, 3, 0 } },
            { L_, 3, { 0, 3, 3 } },
            { L_, 3, { 3, 0, 3 } },
            { L_, 3, { 3, 3, 3 } },

            { L_, 3, { 0, 1, 2 } },
            { L_, 3, { 0, 2, 1 } },
            { L_, 3, { 1, 0, 2 } },
            { L_, 3, { 2, 0, 1 } },
            { L_, 3, { 1, 2, 0 } },
            { L_, 3, { 2, 1, 0 } },

            { L_, 3, { 2, 1, 1 } },
            { L_, 3, { 1, 2, 1 } },
            { L_, 3, { 1, 1, 2 } },
            { L_, 3, { 1, 2, 2 } },
            { L_, 3, { 2, 1, 2 } },
            { L_, 3, { 2, 2, 1 } },

            { L_, 3, { 3, 0, 1 } },
            { L_, 3, { 0, 3, 1 } },
            { L_, 3, { 0, 1, 3 } },
            { L_, 3, { 3, 1, 0 } },
            { L_, 3, { 1, 3, 0 } },
            { L_, 3, { 1, 0, 3 } },

            { L_, 3, { 3, 1, 1 } },
            { L_, 3, { 1, 3, 1 } },
            { L_, 3, { 1, 1, 3 } },
            { L_, 3, { 1, 3, 3 } },
            { L_, 3, { 3, 1, 3 } },
            { L_, 3, { 3, 3, 1 } },

            { L_, 3, { 2, 3, 0 } },
            { L_, 3, { 3, 2, 0 } },
            { L_, 3, { 2, 0, 3 } },
            { L_, 3, { 3, 0, 2 } },
            { L_, 3, { 0, 3, 2 } },
            { L_, 3, { 0, 2, 3 } },
            { L_, 3, { 2, 2, 3 } },
            { L_, 3, { 3, 2, 2 } },
            { L_, 3, { 2, 3, 2 } },

            { L_, 3, { 1, 2, 3 } },
            { L_, 3, { 3, 1, 2 } },
            { L_, 3, { 2, 3, 1 } },
            { L_, 3, { 3, 2, 1 } },
            { L_, 3, { 2, 1, 3 } },
            { L_, 3, { 1, 3, 2 } },
        };

        BlobBufferFactory factory(BIG);
        btlb::BlobBuffer buffer;
        factory.allocate(&buffer);

        typedef bsls::Types::UintPtr UintPtr;
        char *aligned = buffer.data();
        UintPtr alInt = reinterpret_cast<UintPtr>(aligned + MAXALN - 1);
        alInt &= ~UintPtr(MAXALN - 1);
        aligned = reinterpret_cast<char*>(alInt);
        ptrdiff_t fuzz = aligned - buffer.data();

        char copyBuf[BIG * 4];
        char *copyBufP = copyBuf;
        UintPtr copyBufI = reinterpret_cast<UintPtr>(copyBufP + MAXALN - 1);
        copyBufI &= ~UintPtr(MAXALN - 1);
        copyBufP = reinterpret_cast<char*>(copyBufI);

        btlb::BlobBuffer buffers[4];
        buffers[0].reset(buffers[0].buffer(), 0); // empty
        for (int i = 1; i < 4; ++i) {
            bsl::shared_ptr<char> bufferp;
            bufferp.loadAlias(buffer.buffer(), aligned + i - 1);
            buffers[i].buffer() = bufferp;
            buffers[i].setSize(static_cast<int>(BIG - fuzz - i - 1));
        }
        // Now we have four buffers, in four different sizes.  The first is
        // empty The second is aligned to 16 bytes, size 16 The third is
        // misaligned by 1, size 15 The fourth is misaligned by 2, size 14

        int NUMDATA = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUMDATA; ++i) {

            int LINE    = DATA[i].d_line;
            int NUMBUFS = DATA[i].d_numBufs;
            const int *SIZES = DATA[i].d_sizes;
            btlb::Blob BLOB;
            for (int f = 0; f < NUMBUFS; ++f) {
                BLOB.appendBuffer(buffers[SIZES[f]]);
            }
            for (int POS = -1; POS < BLOB.totalSize() + 1; ++POS) {
                int sizeMax = BLOB.totalSize() - POS + 1;
                for (int SIZE = 0; SIZE <= sizeMax; ++SIZE) {
                    for (int ALN = 1; ALN <= MAXALN; ALN <<= 1) {

                        if (veryVeryVerbose) {
                            bsl::cout << "LINE=" << LINE << ":"
                                " DATA[" << i << "]=("
                                " NUMBUFS=" << NUMBUFS << ","
                                " (" << SIZES[0] << ","
                                " "  << SIZES[1] << ","
                                " "  << SIZES[2] << "),"
                                " POS="  << POS  << ","
                                " SIZE=" << SIZE << ","
                                " ALN="  << ALN << ")";
                        }
                        if (POS < 0 || BLOB.totalSize() <= POS
                         || BLOB.totalSize() - POS < SIZE || SIZE <= 0) {

                            veryVeryVerbose && (bsl::cout << " BAD ARG\n");
                            BSLS_ASSERTTEST_ASSERT_FAIL(
                                btlb::BlobUtil::getContiguousRangeOrCopy(
                                              copyBufP, BLOB, POS, SIZE, ALN));
                            continue;
                        }
                        veryVeryVerbose && (bsl::cout << "\n");

                        memset(copyBufP, '#', SIZE + 1);
                        char *out;
                        BSLS_ASSERTTEST_ASSERT_PASS(out =
                                      btlb::BlobUtil::getContiguousRangeOrCopy(
                                              copyBufP, BLOB, POS, SIZE, ALN));
                        ASSERT(0 != out);
                        bsl::pair<int, int> place =
                           btlb::BlobUtil::findBufferIndexAndOffset(BLOB, POS);
                        const btlb::BlobBuffer& buf = BLOB.buffer(place.first);
                        char *p = buf.data();
                        int size = buf.size() - place.second;
                        UintPtr v = reinterpret_cast<UintPtr>(
                                                             p + place.second);
                        bool isAligned = ((v & UintPtr(ALN - 1)) == 0);
                        if (SIZE <= size && isAligned) {
                            ASSERT(out == p + place.second);
                        } else {
                            char dumBuf[3 * BIG + 1];
                            dumBuf[SIZE] = '#';
                            ASSERT(out == copyBufP);
                            btlb::BlobUtil::copy(dumBuf, BLOB, POS, SIZE);
                            ASSERT(0 == memcmp(dumBuf, out, SIZE + 1));
                        }
                    }
                }
            }
        }

        btlb::Blob BLOB(&buffers[1], 1, &factory);
        char *abuf = buffers[1].data();
        struct {
            int d_line;
            int d_bufOffset;  // offset into aligned destination buffer
            int d_position;   // position of range in the blob
            int d_length;     // length of contiguous range in the blob
            int d_alignment;  // alignment to ask for
        } FAILS[] = {
        //  LINE  OFF POS LEN  ALN
            { L_, -1,  0,  1,   1 }, // NULL destination buffer
            { L_,  0, -1,  1,   1 }, // Negative position
            { L_,  0,  0, -1,   1 }, // Negative length
            { L_,  0,  0,  1,   3 }, // Non-power-of-two alignment
            { L_,  0,  0,  1,  -1 }, // likewise
            { L_,  0,  0,  1,   0 }, // likewise
            { L_,  0,  0,  1, 100 }, // likewise
            { L_,  1,  0,  0,   2 }, // misaligned destination buffer
            { L_,  1,  0,  0,   4 }, // likewise
            { L_,  1,  0,  0,   8 }, // likewise
            { L_,  1,  0,  0,  16 }, // likewise
            { L_,  1,  0,  0,  32 }, // likewise
            { L_,  0,  0, BIG+1, 1 }, // length > blob.totalSize() - position
            { L_,  0, BIG, 1,   1 }   // length > blob.totalSize() - position
        };
        int NUMFAILS = sizeof(FAILS)/sizeof(*FAILS);
        for (int i = 0; i < NUMFAILS; ++i) {
            int LINE = FAILS[i].d_line,
                OFF =  FAILS[i].d_bufOffset,
                POS =  FAILS[i].d_position,
                LEN =  FAILS[i].d_length,
                ALN =  FAILS[i].d_alignment;
            char *BUF = (OFF < 0) ? NULL : abuf + OFF;

            if (veryVeryVerbose) {
                bsl::cout << "LINE=" << LINE << ":"
                    " FAILS[" << i << "]=("
                    " OFF="  << OFF << ","
                    " POS="  << POS  << ","
                    " LEN="  << LEN << ","
                    " ALN="  << ALN << ")\n";
            }
            BSLS_ASSERTTEST_ASSERT_FAIL(
                                      btlb::BlobUtil::getContiguousRangeOrCopy(
                                                    BUF, BLOB, POS, LEN, ALN));
        }
        // One-off alignment is OK for alignment 1:
        BSLS_ASSERTTEST_ASSERT_PASS(btlb::BlobUtil::getContiguousRangeOrCopy(
                                                     abuf + 1, BLOB, 1, 1, 1));
        verbose && (cout << "\nEnd of Test.\n");
      } break;
      case 8: {
        // -------------------------------------------------------------------
        // TESTING 'getContiguousDataBuffer' FUNCTION
        //
        // Concerns:
        //   BLACK BOX:
        //  1 Works for all boundary conditions:
        //    1 default-constructed blob
        //    2 blobs with 1, 2, 3 buffers in varied arrangements
        //      and with length at varying positions
        //  2 Works for sample non-boundary conditions
        //  3 Catches bad arguments
        //
        // Plan:
        //  1 Create a table defining blobs with varying composition
        //  2 Create blobs to specifications, extend each, check results
        //    for representative extensions.
        // Testing:
        //    char *BlobUtil::getContiguousDataBuffer(btlb::Blob*, int length,
        //                                   btlb::BlobBufferFactory *factory)
        // --------------------------------------------------------------------

        verbose && (cout << "\nTesting 'getContiguousDataBuffer' Function"
                            "\n==========================================\n");

        static const struct {
            int d_line;
            int d_numBufs;   // number of buffers to put into the test blob
            int d_sizes[3];  // sizes of the buffers (ignore extras)
            int d_length;    // blob.setLength() with this value
            int d_numBufs1;  // number of buffers expected after adding 1 byte
            int d_numBufs5;  // number of buffers expected after adding 5
            char d_at1;      // byte expected at *result after adding 1 byte
            char d_at5;      // byte expected at *result after adding 5 bytes
        } DATA[] = {
        //    LINE                    NUMBUFS1
        //    |   NUMBUFS     LENGTH  |  NUMBUFS5
        //    |   |    +SIZES+     |  |  |  AT1  AT5
            { L_, 0, { 0, 0, 0 },  0, 1, 1, '#', '#' },

            { L_, 1, { 0, 0, 0 },  0, 2, 2, '#', '#' },
            { L_, 2, { 0, 0, 0 },  0, 3, 3, '#', '#' },
            { L_, 3, { 0, 0, 0 },  0, 4, 4, '#', '#' },

            { L_, 2, { 5, 0, 0 },  0, 2, 2, 'a', 'a' },
            { L_, 2, { 5, 0, 0 },  1, 2, 3, 'b', '#' },
            { L_, 2, { 5, 0, 0 },  3, 2, 3, 'd', '#' },
            { L_, 2, { 5, 0, 0 },  4, 2, 3, 'e', '#' },
            { L_, 2, { 5, 0, 0 },  5, 3, 3, '#', '#' },
            { L_, 2, { 0, 5, 0 },  0, 2, 2, 'a', 'a' },
            { L_, 2, { 0, 5, 0 },  1, 2, 3, 'b', '#' },
            { L_, 2, { 0, 5, 0 },  3, 2, 3, 'd', '#' },
            { L_, 2, { 0, 5, 0 },  4, 2, 3, 'e', '#' },
            { L_, 2, { 0, 5, 0 },  5, 3, 3, '#', '#' },
            { L_, 2, { 5, 5, 0 },  0, 2, 2, 'a', 'a' },
            { L_, 2, { 5, 5, 0 },  1, 2, 2, 'b', 'f' },
            { L_, 2, { 5, 5, 0 },  3, 2, 2, 'd', 'f' },
            { L_, 2, { 5, 5, 0 },  4, 2, 2, 'e', 'f' },
            { L_, 2, { 5, 5, 0 },  5, 2, 2, 'f', 'f' },
            { L_, 2, { 5, 5, 0 },  9, 2, 3, 'j', '#' },
            { L_, 2, { 5, 5, 0 }, 10, 3, 3, '#', '#' },

            { L_, 3, { 5, 0, 0 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 5, 0, 0 },  1, 3, 4, 'b', '#' },
            { L_, 3, { 5, 0, 0 },  3, 3, 4, 'd', '#' },
            { L_, 3, { 5, 0, 0 },  4, 3, 4, 'e', '#' },
            { L_, 3, { 5, 0, 0 },  5, 4, 4, '#', '#' },
            { L_, 3, { 0, 5, 0 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 0, 5, 0 },  1, 3, 4, 'b', '#' },
            { L_, 3, { 0, 5, 0 },  3, 3, 4, 'd', '#' },
            { L_, 3, { 0, 5, 0 },  4, 3, 4, 'e', '#' },
            { L_, 3, { 0, 5, 0 },  5, 4, 4, '#', '#' },
            { L_, 3, { 0, 0, 5 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 0, 0, 5 },  1, 3, 4, 'b', '#' },
            { L_, 3, { 0, 0, 5 },  3, 3, 4, 'd', '#' },
            { L_, 3, { 0, 0, 5 },  4, 3, 4, 'e', '#' },
            { L_, 3, { 0, 0, 5 },  5, 4, 4, '#', '#' },

            { L_, 3, { 5, 5, 0 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 5, 5, 0 },  1, 3, 3, 'b', 'f' },
            { L_, 3, { 5, 5, 0 },  4, 3, 3, 'e', 'f' },
            { L_, 3, { 5, 5, 0 },  5, 3, 3, 'f', 'f' },
            { L_, 3, { 5, 5, 0 },  6, 3, 4, 'g', '#' },
            { L_, 3, { 5, 5, 0 },  9, 3, 4, 'j', '#' },
            { L_, 3, { 5, 5, 0 }, 10, 4, 4, '#', '#' },
            { L_, 3, { 5, 0, 5 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 5, 0, 5 },  1, 3, 3, 'b', 'f' },
            { L_, 3, { 5, 0, 5 },  4, 3, 3, 'e', 'f' },
            { L_, 3, { 5, 0, 5 },  5, 3, 3, 'f', 'f' },
            { L_, 3, { 5, 0, 5 },  6, 3, 4, 'g', '#' },
            { L_, 3, { 5, 0, 5 },  9, 3, 4, 'j', '#' },
            { L_, 3, { 5, 0, 5 }, 10, 4, 4, '#', '#' },
            { L_, 3, { 0, 5, 5 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 0, 5, 5 },  1, 3, 3, 'b', 'f' },
            { L_, 3, { 0, 5, 5 },  4, 3, 3, 'e', 'f' },
            { L_, 3, { 0, 5, 5 },  5, 3, 3, 'f', 'f' },
            { L_, 3, { 0, 5, 5 },  6, 3, 4, 'g', '#' },
            { L_, 3, { 0, 5, 5 },  9, 3, 4, 'j', '#' },
            { L_, 3, { 0, 5, 5 }, 10, 4, 4, '#', '#' },

            { L_, 3, { 5, 5, 5 },  0, 3, 3, 'a', 'a' },
            { L_, 3, { 5, 5, 5 },  1, 3, 3, 'b', 'f' },
            { L_, 3, { 5, 5, 5 },  4, 3, 3, 'e', 'f' },
            { L_, 3, { 5, 5, 5 },  5, 3, 3, 'f', 'f' },
            { L_, 3, { 5, 5, 5 },  6, 3, 3, 'g', 'k' },
            { L_, 3, { 5, 5, 5 },  9, 3, 3, 'j', 'k' },
            { L_, 3, { 5, 5, 5 }, 10, 3, 3, 'k', 'k' },
            { L_, 3, { 5, 5, 5 }, 11, 3, 4, 'l', '#' },
            { L_, 3, { 5, 5, 5 }, 14, 3, 4, 'o', '#' },
            { L_, 3, { 5, 5, 5 }, 15, 4, 4, '#', '#' }
        };

        BlobBufferFactory factory(5);
        btlb::BlobBuffer buffers[3];
        factory.allocate(&buffers[0]);
        factory.allocate(&buffers[1]);
        factory.allocate(&buffers[2]);
        btlb::BlobBuffer emptyBuffer(buffers[0].buffer(), 0);

        // We have not tested btlb::BlobUtil::append yet, so cannot use it.

        bsl::memcpy(buffers[0].data(), "abcde", 5);
        bsl::memcpy(buffers[1].data(), "fghij", 5);
        bsl::memcpy(buffers[2].data(), "klmno", 5);

        int NUMDATA = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUMDATA; ++i) {
            int LINE = DATA[i].d_line;
            int NUMBUFS =  DATA[i].d_numBufs;
            const int *SIZES = DATA[i].d_sizes;
            int LENGTH = DATA[i].d_length;
            int NUMBUFS1 = DATA[i].d_numBufs1;
            int NUMBUFS5 = DATA[i].d_numBufs5;
            char AT1 = DATA[i].d_at1;
            char AT5 = DATA[i].d_at5;

            btlb::Blob BLOB1;
            {
                int bufferIndex = 0;
                for (int f = 0; f < NUMBUFS; ++f) {
                    if (SIZES[f]) {
                        BLOB1.appendBuffer(buffers[bufferIndex++]);
                    } else {
                        BLOB1.appendBuffer(emptyBuffer);
                    }
                }
                BLOB1.setLength(LENGTH);
            }
            btlb::Blob BLOB5;
            {
                int bufferIndex = 0;
                for (int f = 0; f < NUMBUFS; ++f) {
                    if (SIZES[f]) {
                        BLOB5.appendBuffer(buffers[bufferIndex++]);
                    } else {
                        BLOB5.appendBuffer(emptyBuffer);
                    }
                }
                BLOB5.setLength(LENGTH);
            }

            if (veryVerbose) {
                bsl::cout << "LINE=" << LINE << ":"
                    " [" << i << "]=(" << NUMBUFS << ","
                    " (" << SIZES[0] << ","
                    " "  << SIZES[1] << ","
                    " "  << SIZES[2] << "),"
                    " "  << LENGTH << ","
                    " "  << NUMBUFS1 << ","
                    " "  << NUMBUFS5 << ","
                    " "  << AT1 << ","
                    " "  << AT5 << ")\n";
            }

            ASSERT(BLOB1.length() == LENGTH);
            ASSERT(BLOB5.length() == LENGTH);

            ASSERT(NUMBUFS == BLOB1.numBuffers());
            ASSERT(NUMBUFS == BLOB5.numBuffers());

            BlobBufferFactory factory7(7);

            char *p1 = 0, *p5 = 0;

            BSLS_ASSERTTEST_ASSERT_PASS(p1 =
                btlb::BlobUtil::getContiguousDataBuffer(&BLOB1, 1, &factory7));
            BSLS_ASSERTTEST_ASSERT_PASS(p5 =
                btlb::BlobUtil::getContiguousDataBuffer(&BLOB5, 5, &factory7));

            ASSERT(p1);
            ASSERT(p5);
            if (!p1 || !p5) {
                continue;
            }

            ASSERT(NUMBUFS1 == BLOB1.numBuffers());
            ASSERT(NUMBUFS5 == BLOB5.numBuffers());

            ASSERT(BLOB1.length() == LENGTH + 1);
            ASSERT(BLOB5.length() == LENGTH + 5);

            ASSERT(*p1 == AT1);
            ASSERT(*p5 == AT5);

            // Did it put the new buffer, if any, in the right place?

            int ante = LENGTH;
            bsl::pair<int, int> b =
                         btlb::BlobUtil::findBufferIndexAndOffset(BLOB5, ante);
            ASSERT(b.second == 0);
            ASSERT(p5 == BLOB5.buffer(b.first).data());

            if (ante == 0) {
                if (0 == SIZES[0] + SIZES[1] + SIZES[2]) {
                    ASSERT(7 == BLOB5.buffer(b.first).size());
                } else {
                    ASSERT(5 == BLOB5.buffer(b.first).size());
                }
            } else {
                bsl::pair<int, int> a =
                     btlb::BlobUtil::findBufferIndexAndOffset(BLOB5, ante - 1);
                ASSERT(a.first == b.first - 1
                    || BLOB5.buffer(b.first - 1).size() == 0);

                // Did it trim the buffer correctly?

                if (ante % 5 == 0) {
                    ASSERT(BLOB5.buffer(a.first).size() == 5);
                } else {  // better be trimmed
                    ASSERT(BLOB5.buffer(a.first).size() == ante % 5);
                }
            }

        }

        btlb::Blob BLOB;

        BSLS_ASSERTTEST_ASSERT_FAIL(
                      btlb::BlobUtil::getContiguousDataBuffer(0, 1, &factory));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                 btlb::BlobUtil::getContiguousDataBuffer(&BLOB, -1, &factory));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                         btlb::BlobUtil::getContiguousDataBuffer(&BLOB, 1, 0));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                 btlb::BlobUtil::getContiguousDataBuffer(&BLOB, 10, &factory));

        verbose && (cout << "\nEnd of Test.\n");
      } break;
      case 7: {
        // -------------------------------------------------------------------
        // TESTING 'copy' FUNCTION
        //
        // Concerns:
        //   BLACK BOX:
        //  1 Works for all boundary conditions:
        //    1 default-constructed blob
        //    2 blobs with 1, 2, 3 buffers in varied arrangements
        //
        //  2 Works for sample non-boundary conditions
        //  3 Traps invalid arguments
        //
        // Plan:
        //  1 Create a table defining blobs with varying composition
        //  2 Create each blob, check output for all positions and lengths
        //  3 Verify that assertions fire when appropriate, not otherwise
        //
        // Testing:
        //    void BlobUtil::copy(char* dstBuffer, const btlb::Blob& srcBlob,
        //                                            int position, int length)
        //
        // --------------------------------------------------------------------

        verbose && (cout << "\nTesting 'copy' Function"
                            "\n=======================\n");

        static const struct {
            int d_line;
            int d_numBufs;  // number of buffers to add
            int d_sizes[3]; // sizes of buffers to add (ignore extras)
        } DATA[] =
        //      NUMBUFS
        //  LINE  |    +SIZES+
        {   { L_, 0, { 0, 0, 0 } },
            { L_, 1, { 5, 0, 0 } },

            { L_, 2, { 0, 0, 0 } },
            { L_, 2, { 5, 0, 0 } },
            { L_, 2, { 0, 5, 0 } },
            { L_, 2, { 5, 5, 0 } },

            { L_, 3, { 0, 0, 0 } },
            { L_, 3, { 5, 0, 0 } },
            { L_, 3, { 0, 5, 0 } },
            { L_, 3, { 5, 5, 0 } },
            { L_, 3, { 0, 0, 5 } },
            { L_, 3, { 5, 0, 5 } },
            { L_, 3, { 0, 5, 5 } },
            { L_, 3, { 5, 5, 5 } }
        };

        BlobBufferFactory factory(5);
        btlb::BlobBuffer buffers[3];
        factory.allocate(&buffers[0]);
        factory.allocate(&buffers[1]);
        factory.allocate(&buffers[2]);
        btlb::BlobBuffer emptyBuffer(buffers[0].buffer(), 0);
        bsl::memcpy(buffers[0].data(), "abcde", 5);
        bsl::memcpy(buffers[1].data(), "fghij", 5);
        bsl::memcpy(buffers[2].data(), "klmno", 5);
        static const char ref[] = "abcdefghijklmno";
        BSLS_ASSERT(sizeof(ref) == 16 && ref[15] == '\0');

        int NUMDATA = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUMDATA; ++i) {
            int LINE    = DATA[i].d_line;
            int NUMBUFS = DATA[i].d_numBufs;
            const int *SIZES = DATA[i].d_sizes;

            btlb::Blob BLOB;
            int bufferIndex = 0;
            for (int f = 0; f < NUMBUFS; ++f) {
                if (SIZES[f]) {
                    BLOB.appendBuffer(buffers[bufferIndex++]);
                } else {
                    BLOB.appendBuffer(emptyBuffer);
                }
            }

            if (veryVerbose) {
                bsl::cout << "LINE=" << LINE << ":"
                    " [" << i << "]=(" << NUMBUFS << ","
                    " (" << SIZES[0] << ","
                    " "  << SIZES[1] << ","
                    " "  << SIZES[2] << ") )\n";
            }
            for (int j = -1; j <= BLOB.totalSize() + 1; ++j) {
                for (int k = -1; k <= BLOB.totalSize() - j + 1; ++k) {
                    bsls::AssertFailureHandlerGuard guard(
                                                    &bsls::Assert::failThrow);
                    char refBuf[sizeof ref];
                    char tstBuf[sizeof ref];
                    veryVeryVerbose &&
                        (bsl::cout << "j=" << j << " k=" << k << "\n");
                    if (!bad_jk(j, k, BLOB)) {
                        bsl::memcpy(refBuf, ref + j, k);
                        bsl::memset(refBuf + j + k, '!',
                                                       sizeof(refBuf) - j - k);
                        bsl::memset(tstBuf, 'X', sizeof tstBuf);
                    }
                    if (bad_jk(j, k, BLOB)) {

                        BSLS_ASSERTTEST_ASSERT_FAIL(
                                     btlb::BlobUtil::copy(tstBuf, BLOB, j, k));

                    } else if (k) {

                        BSLS_ASSERTTEST_ASSERT_PASS(
                                     btlb::BlobUtil::copy(tstBuf, BLOB, j, k));

                        ASSERT(bsl::memcmp(refBuf, tstBuf, k) == 0);
                        ASSERT(tstBuf[k] == 'X');
                    }
                }
            }
        }

        verbose && (cout << "\nEnd of Test.\n");
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // TESTING 'findBufferIndexAndOffset' FUNCTION
        //
        // Concerns:
        //   BLACK BOX:
        //  1 Works for all boundary conditions:
        //    1 default-constructed blob
        //    2 blobs with 1, 2, 3 buffers, empty and not
        //
        //  2 Works for sample non-boundary conditions
        //
        // Plan:
        //  1 Create a table defining blobs with varying composition
        //  2 For each, identify salient positions and results
        //  3 Create each blob, check output for each salient position
        //  4 Verify that assertions fire when appropriate, not otherwise
        //
        // Testing:
        //    bsl::pair<int, int> BlobUtil::findBufferIndexAndOffset(
        //                                    const btlb::Blob&, int position)
        // --------------------------------------------------------------------

        verbose && (cout << "\nTesting 'findBufferIndexAndOffset' Function"
                            "\n===========================================\n");

        static const struct {
            int d_line;
            int d_numBufs;      // number of buffers to put in blob
            int d_sizes[3];     // sizes of buffers in blob (ignore extras)
            int d_position;     // position in blob to find
            int d_expectIndex;  // expected PLACE.first
            int d_expectOffset; // expected PLACE.second
        } DATA[] = {

         //     NUMBUFS
         //  LINE |    +SIZES+    POS    PLACE
            { L_, 3, { 5, 5, 5 },  8,    1, 3 },   // typical

            { L_, 1, { 5, 0, 0 },  0,    0, 0 },
            { L_, 1, { 5, 0, 0 },  1,    0, 1 },
            { L_, 1, { 5, 0, 0 },  5-1,  0, 5-1 },

            { L_, 2, { 0, 0, 0 },  0,   -1, 0 },
            { L_, 2, { 0, 5, 0 },  0,    1, 0 },
            { L_, 2, { 5, 0, 0 },  0,    0, 0 },
            { L_, 2, { 5, 5, 0 },  0,    0, 0 },
            { L_, 2, { 0, 5, 0 },  1,    1, 1 },
            { L_, 2, { 5, 0, 0 },  1,    0, 1 },
            { L_, 2, { 5, 5, 0 },  1,    0, 1 },
            { L_, 2, { 0, 5, 0 },  5-1,  1, 5-1 },
            { L_, 2, { 5, 0, 0 },  5-1,  0, 5-1 },
            { L_, 2, { 5, 5, 0 },  5-1,  0, 5-1 },
            { L_, 2, { 5, 5, 0 },  5,    1, 0 },
            { L_, 2, { 5, 5, 0 },  5+1,  1, 1 },
            { L_, 2, { 5, 5, 0 },  2*5-1, 1, 5-1 },

            { L_, 3, { 0, 5, 0 },  0,    1, 0 },
            { L_, 3, { 5, 0, 0 },  0,    0, 0 },
            { L_, 3, { 5, 5, 0 },  0,    0, 0 },
            { L_, 3, { 0, 0, 5 },  0,    2, 0 },
            { L_, 3, { 0, 5, 5 },  0,    1, 0 },
            { L_, 3, { 5, 0, 5 },  0,    0, 0 },
            { L_, 3, { 5, 5, 5 },  0,    0, 0 },

            { L_, 3, { 0, 5, 0 },  1,    1, 1 },
            { L_, 3, { 5, 0, 0 },  1,    0, 1 },
            { L_, 3, { 5, 5, 0 },  1,    0, 1 },
            { L_, 3, { 0, 5, 5 },  1,    1, 1 },
            { L_, 3, { 5, 0, 5 },  1,    0, 1 },
            { L_, 3, { 5, 5, 5 },  1,    0, 1 },

            { L_, 3, { 0, 5, 0 },  5-1,  1, 5-1 },
            { L_, 3, { 5, 0, 0 },  5-1,  0, 5-1 },
            { L_, 3, { 5, 5, 0 },  5-1,  0, 5-1 },
            { L_, 3, { 0, 5, 5 },  5-1,  1, 5-1 },
            { L_, 3, { 5, 0, 5 },  5-1,  0, 5-1 },
            { L_, 3, { 5, 5, 5 },  5-1,  0, 5-1 },

            { L_, 3, { 0, 5, 0 },  1,    1, 1 },
            { L_, 3, { 5, 5, 0 },  5,    1, 0 },
            { L_, 3, { 0, 5, 5 },  5,    2, 0 },
            { L_, 3, { 5, 0, 5 },  5,    2, 0 },
            { L_, 3, { 5, 5, 5 },  5,    1, 0 },

            { L_, 3, { 5, 5, 0 },  5+1,  1, 1 },
            { L_, 3, { 0, 5, 5 },  5+1,  2, 1 },
            { L_, 3, { 5, 0, 5 },  5+1,  2, 1 },
            { L_, 3, { 5, 5, 5 },  5+1,  1, 1 },

            { L_, 3, { 5, 5, 0 },  2*5-1, 1, 5-1 },
            { L_, 3, { 0, 5, 5 },  2*5-1, 2, 5-1 },
            { L_, 3, { 5, 0, 5 },  2*5-1, 2, 5-1 },
            { L_, 3, { 5, 5, 5 },  2*5-1, 1, 5-1 },

            { L_, 3, { 5, 5, 0 },  2*5, -1, 0 },
            { L_, 3, { 0, 5, 5 },  2*5, -1, 0 },
            { L_, 3, { 5, 0, 5 },  2*5, -1, 0 },
            { L_, 3, { 5, 5, 5 },  2*5,  2, 0 },

            { L_, 3, { 5, 5, 5 },  2*5+1, 2, 1 },
            { L_, 3, { 5, 5, 5 },  3*5-1, 2, 5-1 },

            // Negative tests:

            { L_, 0, { 0, 0, 0 }, -1,   -1, 0 },
            { L_, 0, { 0, 0, 0 },  0,   -1, 0 },
            { L_, 0, { 0, 0, 0 },  1,   -1, 0 },
            { L_, 1, { 5, 0, 0 },  5,   -1, 0 },
            { L_, 2, { 0, 5, 0 },  5,   -1, 0 },
            { L_, 2, { 5, 0, 0 },  5,   -1, 0 },
            { L_, 2, { 5, 5, 0 },  2*5, -1, 0 },
            { L_, 3, { 0, 0, 0 },  0,   -1, 0 },
            { L_, 3, { 0, 5, 0 },  5,   -1, 0 },
            { L_, 3, { 5, 0, 0 },  5,   -1, 0 },
            { L_, 3, { 0, 0, 5 },  5,   -1, 0 },
            { L_, 3, { 5, 5, 5 },  3*5, -1, 0 }
        };

        BlobBufferFactory factory(5);
        btlb::BlobBuffer buffer;
        factory.allocate(&buffer);
        btlb::BlobBuffer emptyBuffer(buffer.buffer(), 0);

        int NUMDATA = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUMDATA; ++i) {
            int NUMBUFS = DATA[i].d_numBufs;
            const int *SIZES = DATA[i].d_sizes;
            int POS = DATA[i].d_position;
            int EXPECTINDEX = DATA[i].d_expectIndex;
            int EXPECTOFFSET = DATA[i].d_expectOffset;

            btlb::Blob BLOB;
            for (int j = 0; j < NUMBUFS; ++j) {
                if (SIZES[j]) {
                    BLOB.appendBuffer(buffer);
                } else {
                    BLOB.appendBuffer(emptyBuffer);
                }
            }
            if (veryVerbose) {
                bsl::cout << "LINE=" << DATA[i].d_line << ":"
                    " [" << i << "]=(" << NUMBUFS << ","
                    " (" << SIZES[0] << ","
                    " "  << SIZES[1] << ","
                    " "  << SIZES[2] << "),"
                    " "  << POS << ") => ";
            }

            pair<int, int> PLACE;
            if (0 <= EXPECTINDEX) {

                BSLS_ASSERTTEST_ASSERT_PASS(PLACE =
                          btlb::BlobUtil::findBufferIndexAndOffset(BLOB, POS));

                if (veryVerbose) {
                    bsl::cout <<
                    " PLACE=(" << PLACE.first << ","
                           " " << PLACE.second << ")";
                    bsl::cout << bsl::endl;
                }
                ASSERT(EXPECTINDEX == PLACE.first);
                ASSERT(EXPECTOFFSET == PLACE.second);
                ASSERT(0 != BLOB.buffer(PLACE.first).size());
            } else {

                BSLS_ASSERTTEST_ASSERT_FAIL(PLACE =
                          btlb::BlobUtil::findBufferIndexAndOffset(BLOB, POS));

                ASSERT(EXPECTINDEX == -1);
            }
        }

        verbose && (cout << "\nEnd of Test.\n");
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ERASE FUNCTION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'erase' Function"
                          << "\n=========================" << endl;

        const bsl::string STR      = "HelloWorld";
        const int         BUF_SIZE = static_cast<int>(STR.size());
        for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
            BlobBufferFactory factory(bufferSize);

            for (int offset = 0; offset < BUF_SIZE; ++offset) {
                for (int length = 0; length <= BUF_SIZE - offset; ++length) {

                    bsl::string sstr = STR;
                    bsl::string estr = STR;
                    estr.erase(offset, length);

                    btlb::Blob exp(&factory);
                    btlb::Blob source(&factory);
                    exp.setLength(20);
                    source.setLength(20);

                    exp.setLength(0);
                    source.setLength(0);

                    copyStringToBlob(&exp, estr);
                    copyStringToBlob(&source, sstr);

                    if (veryVeryVerbose) {
                        bsl::cout << "\nSource = " << bsl::endl;
                        Util::hexDump(bsl::cout, source);
                        bsl::cout << "\nExp = " << bsl::endl;
                        Util::hexDump(bsl::cout, exp);
                    }

                    Util::erase(&source, offset, length);
                    LOOP5_ASSERT(sstr, estr, bufferSize, offset, length,
                                 !Util::compare(exp, source));

                    if (veryVeryVerbose) {
                        bsl::cout << "\nSource = " << bsl::endl;
                        Util::hexDump(bsl::cout, source);
                        bsl::cout << "\nExp = " << bsl::endl;
                        Util::hexDump(bsl::cout, exp);
                    }
                }
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING HEXDUMP WITH OFFSET AND LENGTH
        // --------------------------------------------------------------------

        enum {
            k_DYNAMIC_ALLOCATION_THRESHOLD = 32,

            k_MIN_BUFFER_SIZE = 1,
            k_MAX_BUFFER_SIZE = 4,
            k_INC_BUFFER_SIZE = 1,

            k_MIN_NUM_BUFFERS = k_DYNAMIC_ALLOCATION_THRESHOLD - 4,
            k_MAX_NUM_BUFFERS = k_DYNAMIC_ALLOCATION_THRESHOLD + 4,
            k_INC_NUM_BUFFERS = 1
        };

        bslma::TestAllocator allocator;
        bslma::Default::setDefaultAllocatorRaw(&allocator);
        ASSERT(&allocator == bslma::Default::defaultAllocator());

        for (int bufferSize =  k_MIN_BUFFER_SIZE;
                 bufferSize <= k_MAX_BUFFER_SIZE;
                 bufferSize += k_INC_BUFFER_SIZE)
        {
            BlobBufferFactory factory(bufferSize);

            for (int numBuffers =  k_MIN_NUM_BUFFERS;
                     numBuffers <= k_MAX_NUM_BUFFERS;
                     numBuffers += k_INC_NUM_BUFFERS)
            {
                if (verbose) {
                    bsl::cout << "[bufferSize = " << bufferSize << ", "
                              << "numBuffers = " << numBuffers << "]"
                              <<bsl::endl;
                }

                btlb::Blob blob(&factory);

                int totalSize = numBuffers * bufferSize;
                copyStringToBlob(&blob, g(totalSize));

                for (int offset = 0; offset < totalSize; ++offset)
                {
                    for (int length = 0; length < totalSize - offset; ++length)
                    {
                        if (veryVerbose) {
                            bsl::cout << " offset = " << offset
                                      << ", length = " << length << bsl::endl;
                        }

                        bsl::stringstream os;
                        ASSERT(&os == &btlb::BlobUtil::hexDump(os,
                                                               blob,
                                                               offset,
                                                               length));

                        if (veryVeryVerbose) {
                            bsl::cout << os.str() << bsl::endl;
                        }
                    }
                }
            }
        }

        ASSERT(0 <  allocator.numAllocations());
        ASSERT(0 == allocator.numBlocksInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HEXDUMP
        //
        // Concerns:
        //   For performance reasons, btlb::BlobUtil::hexDump uses a static
        //   array of length 32 to pass in the the vector of <buffer, size>
        //   pairs to bdlb_print::hexDump.  When there are more than 32
        //   buffers, the vector passed in is dynamically allocated to the
        //   correct size, and deallocated immediately after use.  This should
        //   test all boarder cases related to hex dumping of multiple buffers,
        //   including 0 buffers, 1 buffers, 31,32,33, and 42 for good measure.
        //
        // Plan:
        //   Create a situation with
        //     (a) 0 buffers.  Verify correctness.
        //     (b) 1 buffers.  Verify correctness.
        //     (c) 31 buffers.  Verify correctness.
        //     (d) 32 buffers.  Verify correctness.
        //     (e) 33 buffers.  Verify correctness.
        //
        // Testing:
        //   static bsl::ostream& hexDump(stream, source);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'hexdump' Function"
                          << "\n==========================" << endl;

        enum { k_BUF_SIZE = 2048 };

        {
            if (verbose) cout << "(a) 0 buffers" << endl;
            BlobBufferFactory factory(5);
            btlb::Blob        myBlob(&factory);

            ASSERT(0 == myBlob.numDataBuffers() );

            char buf[k_BUF_SIZE];  bsl::strstream out(buf, k_BUF_SIZE);
            ASSERT(&out == &btlb::BlobUtil::hexDump(out, myBlob));
            ASSERT(0 == strncmp(buf, expectedOutCase3[0].c_str(),
                                expectedOutCase3[0].size()));

            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase3[0] << bsl::endl;
            }
        }

        {
            if (verbose) cout << "(b) 1 buffers" << endl;
            BlobBufferFactory factory(1024);
            btlb::Blob        myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(1 == myBlob.numDataBuffers() );

            char buf[k_BUF_SIZE];  bsl::strstream out(buf, k_BUF_SIZE);
            ASSERT(&out == &btlb::BlobUtil::hexDump(out, myBlob));
            ASSERT(0 == strncmp(buf, expectedOutCase3[1].c_str(),
                                expectedOutCase3[1].size()));
            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase3[1] << bsl::endl;
            }
        }
        {
            if (verbose) cout << "(c) 31 buffers" << endl;
            BlobBufferFactory factory(8);
            btlb::Blob        myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(31 == myBlob.numDataBuffers() );

            char buf[k_BUF_SIZE];  bsl::strstream out(buf, k_BUF_SIZE);
            ASSERT(&out == &btlb::BlobUtil::hexDump(out, myBlob));
            ASSERT(0 == strncmp(buf, expectedOutCase3[2].c_str(),
                                expectedOutCase3[2].size()));
            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase3[2] << bsl::endl;
            }
        }
        {
            if (verbose) cout << "(d) 32 buffers" << endl;
            BlobBufferFactory factory(8);
            btlb::Blob myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(32 == myBlob.numDataBuffers() );

            char buf[k_BUF_SIZE];  bsl::strstream out(buf, k_BUF_SIZE);
            ASSERT(&out == &btlb::BlobUtil::hexDump(out, myBlob));
            ASSERT(0 == strncmp(buf, expectedOutCase3[3].c_str(),
                                expectedOutCase3[3].size()));

            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase3[3] << bsl::endl;
            }
        }
        {
            if (verbose) cout << "(e) 33 buffers" << endl;
            BlobBufferFactory factory(8);
            btlb::Blob myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32abcde 33";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(33 == myBlob.numDataBuffers() );

            char buf[k_BUF_SIZE];  bsl::strstream out(buf, k_BUF_SIZE);
            ASSERT(&out == &btlb::BlobUtil::hexDump(out, myBlob));
            ASSERT(0 == strncmp(buf, expectedOutCase3[4].c_str(),
                                expectedOutCase3[4].size()));
            if (veryVerbose) {
                bsl::string output(buf, out.pcount());
                bsl::cout << "Hexdumped String :\n"
                          << output
                          << "Expected String  :\n"
                          << expectedOutCase3[4] << bsl::endl;
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMPARE FUNCTION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'compare' Function"
                          << "\n=========================" << endl;

        const struct {
            int         d_line;
            const char *d_lhs;
            const char *d_rhs;
            const int   d_retValue;
        } DATA[] = {
            // Line LHS RHS retValue
            // ----   ---         ---          --------
            {   L_,   "",         "",                 0 },

            {   L_,   "a",        "",                 1 },
            {   L_,   " ",        "",                 1 },
            {   L_,   "A",        "",                 1 },
            {   L_,   "a",        "b",               -1 },
            {   L_,   "a",        "A",                1 },
            {   L_,   "a",        "a",                0 },
            {   L_,   "A",        "A",                0 },

            {   L_,   "ab",       "",                 1 },
            {   L_,   "AB",       "",                 1 },
            {   L_,   "ab",       "a",                1 },
            {   L_,   "ab",       "b",               -1 },
            {   L_,   "ab",       "AB",               1 },
            {   L_,   "ZY",       "YZ",               1 },
            {   L_,   "XU",       "X",                1 },
            {   L_,   "XU",       "W",                1 },
            {   L_,   "ab",       "ab",               0 },
            {   L_,   "AB",       "AB",               0 },

            {   L_,   "abc",      "",                 1 },
            {   L_,   "XYZ",      "",                 1 },
            {   L_,   "abc",      "a",                1 },
            {   L_,   "abc",      "b",               -1 },
            {   L_,   "abc",      "ab",               1 },
            {   L_,   "abc",      "ba",              -1 },
            {   L_,   "XYZ",      "Y",               -1 },
            {   L_,   "XYZ",      "W",                1 },
            {   L_,   "XYZ",      "XY",               1 },
            {   L_,   "XYZ",      "YZ",              -1 },
            {   L_,   "abc",      "abc",              0 },
            {   L_,   "XYZ",      "XYZ",              0 },

            {   L_,   "abcde",    "",                 1 },
            {   L_,   "VWXYZ",    "",                 1 },
            {   L_,   "abcde",    "a",                1 },
            {   L_,   "abcde",    "b",               -1 },
            {   L_,   "abcde",    "ab",               1 },
            {   L_,   "abcde",    "ba",              -1 },
            {   L_,   "abcde",    "abc",              1 },
            {   L_,   "abcde",    "abce",            -1 },
            //                        ^
            {   L_,   "abcde",    "abcd",             1 },
            {   L_,   "abcde",    "abcdf",           -1 },
            //                         ^
            {   L_,   "abcde",    "abcde",            0 },
            {   L_,   "VWXYZ",    "VWXYZ",            0 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdefghijklmnopqrstuvwxyz",   0 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "Abcdefghijklmnopqrstuvwxyz",   1 },
            //         ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "aBcdefghijklmnopqrstuvwxyz",   1 },
            //          ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abCdefghijklmnopqrstuvwxyz",   1 },
            //           ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcDefghijklmnopqrstuvwxyz",   1 },
            //            ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdEfghijklmnopqrstuvwxyz",   1 },
            //             ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdeFghijklmnopqrstuvwxyz",   1 },
            //              ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdefghijklmnopqrstuvwXyz",   1 },
            //                                ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdefghijklmnopqrstuvwxYz",   1 },
            //                                 ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
                      "abcdefghijklmnopqrstuvwxyZ",   1 },
            //                                  ^

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                             ^^^^^^
                      "abcdefghijklmnopqrst",         1 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                              ^^^^^
                      "abcdefghijklmnopqrstu",        1 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                               ^^^^
                      "abcdefghijklmnopqrstuv",       1 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                                ^^^
                      "abcdefghijklmnopqrstuvw",      1 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                                 ^^
                      "abcdefghijklmnopqrstuvwx",     1 },

            {   L_,   "abcdefghijklmnopqrstuvwxyz",
            //                                  ^
                      "abcdefghijklmnopqrstuvwxy",    1 },
         };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *LHS  = DATA[i].d_lhs;
            const int   LLEN = static_cast<int>(bsl::strlen(LHS));
            const char *RHS  = DATA[i].d_rhs;
            const int   RLEN = static_cast<int>(bsl::strlen(RHS));
            const int   RV   = DATA[i].d_retValue;

            for (int size1 = 1; size1 < 10; ++size1) {
                for (int size2 = 5; size2 < 20; ++size2) {
                    BlobBufferFactory fa(size1), fb(size2);

                    btlb::Blob mX(&fa); const btlb::Blob& X = mX;
                    btlb::Blob mY(&fb); const btlb::Blob& Y = mY;

                    mX.setLength(LLEN);
                    mY.setLength(RLEN);

                    copyStringToBlob(&mX, LHS);
                    copyStringToBlob(&mY, RHS);

                    if (veryVerbose) {
                        P_(LINE) P_(LHS) P_(RHS) P_(RV)
                        P_(size1) P(size2)
                    }

                    const int rv1 = btlb::BlobUtil::compare(X, Y);
                    const int rv2 = btlb::BlobUtil::compare(Y, X);
                    if (RV > 0) {
                        LOOP4_ASSERT(size1, size2, RV, rv1, rv1 > 0);
                        LOOP4_ASSERT(size1, size2, RV, rv1, rv2 < 0);
                    }
                    else if (RV < 0) {
                        LOOP4_ASSERT(size1, size2, RV, rv1, rv1 < 0);
                        LOOP4_ASSERT(size1, size2, RV, rv1, rv2 > 0);
                    }
                    else {
                        LOOP4_ASSERT(size1, size2, RV, rv1, 0 == rv1);
                        LOOP4_ASSERT(size1, size2, RV, rv1, 0 == rv2);
                    }
                }
            }
        }

        // PREVIOUS TEST CASE
        for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
            for (int bufferSize2 = 5; bufferSize2 < 20; ++bufferSize2) {
                const char *TEST_STR = "abcdefghijklmnopqrstuvwxyz";
                const char *TEST_STR2 = "Abcdefghijklmnopqrstuvwxyz";
                const char *TEST_STR3 = "abcdefghijklmnopqrstuvwxyZ";
                const char *TEST_STR4 = "abcdefghijklmnopqrstuvwxyz1";

                BlobBufferFactory factory(bufferSize);
                BlobBufferFactory factory2(bufferSize2);

                // Test same
                btlb::Blob b1(&factory);
                btlb::Blob b2(&factory2);

                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 == btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 == btlb::BlobUtil::compare(b2, b1));

                // Test first char diff
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR2)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR2);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > btlb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR2)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR2);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < btlb::BlobUtil::compare(b2, b1));

                // Test last char diff
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR3)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR3);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > btlb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR3)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR3);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < btlb::BlobUtil::compare(b2, b1));

                // Test 1 char diff in length
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR4)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR4);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < btlb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR4)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR4);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < btlb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > btlb::BlobUtil::compare(b2, b1));
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING APPEND FUNCTION and BLOB_UTIL::WRITE(stream, blob, int, int)
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'write' special cases"
                          << "\n=============================" << endl;

        // writing zero bytes from an empty blob
        {
            btlb::Blob emptyBlob;
            bslx::TestOutStream blobStream(20150825);
            ASSERT(btlb::BlobUtil::write(blobStream, emptyBlob, 0, 0) == 0);
            ASSERT(blobStream.length() == 0);
        }

        // writing non-zero bytes from an empty blob
        {
            btlb::Blob emptyBlob;
            bslx::TestOutStream blobStream(20150825);
            ASSERT(btlb::BlobUtil::write(blobStream, emptyBlob, 0, 1) != 0);
            ASSERT(blobStream.length() == 0);
        }

        // writing zero bytes from a non-empty blob
        {
            // create a non-empty blob
            size_t size = 10;
            BlobBufferFactory blobFactory(static_cast<int>(size));
            btlb::Blob nonemptyBlob(&blobFactory);
            nonemptyBlob.setLength(static_cast<int>(size));

            // write blob
            bslx::TestOutStream blobStream(20150825);
            ASSERT(btlb::BlobUtil::write(blobStream, nonemptyBlob, 0, 0) == 0);
            ASSERT(blobStream.length() == 0);
        }

        if (verbose) cout << "\nTesting 'append and write' functions"
                          << "\n====================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_dest;
            const char *d_source;
            int         d_offset;
            int         d_length;
            const char *d_result;
        } DATA[] = {
            //line  dest        source         off   len    result
            //----  ----        ------         ---   ---    -----
            { L_,   "Hello",    "World",       0,    5,     "HelloWorld",   },
            { L_,   "Hello",    "World",       0,    4,     "HelloWorl",    },
            { L_,   "Hello",    "World",       0,    3,     "HelloWor",     },
            { L_,   "Hello",    "World",       0,    2,     "HelloWo",      },
            { L_,   "Hello",    "World",       0,    1,     "HelloW",       },
            { L_,   "Hello",    "World",       0,    0,     "Hello",        },

            { L_,   "Hello",    "World",       1,    4,     "Helloorld",    },
            { L_,   "Hello",    "World",       1,    3,     "Helloorl",     },
            { L_,   "Hello",    "World",       1,    2,     "Helloor",      },
            { L_,   "Hello",    "World",       1,    1,     "Helloo",       },
            { L_,   "Hello",    "World",       1,    0,     "Hello",        },

            { L_,   "Hello",    "World",       2,    3,     "Hellorld",     },
            { L_,   "Hello",    "World",       2,    2,     "Hellorl",      },
            { L_,   "Hello",    "World",       2,    1,     "Hellor",       },
            { L_,   "Hello",    "World",       2,    0,     "Hello",        },

            { L_,   "Hello",    "World",       3,    2,     "Hellold",      },
            { L_,   "Hello",    "World",       3,    1,     "Hellol",       },
            { L_,   "Hello",    "World",       3,    0,     "Hello",        },

            { L_,   "Hello",    "World",       4,    1,     "Hellod",       },
            { L_,   "Hello",    "World",       4,    0,     "Hello",        },

            { L_,   "Hello",    "World",       5,    0,     "Hello",        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
            BlobBufferFactory factory(bufferSize);

            btlb::Blob dest1(&factory);  // append(blob, blob);
            btlb::Blob dest2(&factory);  // append(blob, const char*, int, int)
            btlb::Blob dest4(&factory);  // append(blob, const char*, int)
            btlb::Blob source(&factory);

            dest1.setLength(20);
            dest2.setLength(20);
            dest4.setLength(20);
            source.setLength(20);

            for (int i = 0; i < NUM_DATA; ++i) {
                dest1.setLength(0);
                dest2.setLength(0);
                dest4.setLength(0);
                bsl::stringbuf dest3;
                source.setLength(0);

                const int   LINE   = DATA[i].d_lineNum;
                const char *DEST   = DATA[i].d_dest;
                const char *SOURCE = DATA[i].d_source;
                const int   OFFSET = DATA[i].d_offset;
                const int   LENGTH = DATA[i].d_length;
                const char *RESULT = DATA[i].d_result;

                copyStringToBlob(&dest1,  DEST);
                copyStringToBlob(&dest2,  DEST);
                copyStringToBlob(&dest4,  DEST);
                dest3.sputn(DEST, static_cast<int>(bsl::strlen(DEST)));
                copyStringToBlob(&source, SOURCE);

                if (veryVeryVerbose) {
                    bsl::cout << "\nDest = " << bsl::endl;
                    Util::hexDump(bsl::cout, dest1);

                    bsl::cout << "\nSource = " << bsl::endl;
                    Util::hexDump(bsl::cout, source);
                }

                Util::append(&dest1, source, OFFSET, LENGTH);
                Util::append(&dest2, SOURCE, OFFSET, LENGTH);
                if (0 == OFFSET) {
                    Util::append(&dest4, SOURCE, LENGTH);
                }

                {
                bslx::GenericOutStream<bsl::stringbuf> gos(&dest3, 20150825);
                Util::write(gos, source, OFFSET, LENGTH);
                gos.flush();
                }

                if (veryVeryVerbose) {
                    bsl::cout << "\nResult = " << bsl::endl;
                    Util::hexDump(bsl::cout, dest1);
                }

                {
                bsl::string result;
                copyBlobToString(&result, dest1);
                LOOP3_ASSERT(bufferSize, LINE, result,
                             RESULT == result);
                }

                {
                bsl::string result;
                copyBlobToString(&result, dest2);
                LOOP3_ASSERT(bufferSize, LINE, result,
                             RESULT == result);
                if (0 == OFFSET) {
                    copyBlobToString(&result, dest4);
                    LOOP3_ASSERT(bufferSize, LINE, result,
                                 RESULT == result);
                }
                }

                {
                bsl::string result = dest3.str();
                LOOP3_ASSERT(bufferSize, LINE, result,
                             RESULT == result);
                }
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
