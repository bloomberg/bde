// bdlbb_blobutil.t.cpp                                               -*-C++-*-

#include <bdlbb_blobutil.h>

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlbb_blob.h>
#include <bdlbb_simpleblobbufferfactory.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslx_genericoutstream.h>
#include <bslx_testoutstream.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_climits.h>     // 'INT_MIN'
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcpy'
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
// [15] void prependWithCapacityBuffer(Blob*,BlobBuffer*,const char*,int);
// [14] void appendWithCapacityBuffer(Blob*,BlobBuffer*,const char*,int);
// [13] int appendBufferIfValid(Blob *d, const BlobBuffer& b);
// [13] int appendBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
// [13] int appendDataBufferIfValid(Blob *d, const BlobBuffer& b);
// [13] int appendDataBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
// [13] int insertBufferIfValid(Blob *d, int i, const BlobBuffer& b);
// [13] int insertBufferIfValid(Blob *d, int i, MovableRef<BlobBuffer> b);
// [13] int prependDataBufferIfValid(Blob *d, const BlobBuffer& b);
// [13] int prependDataBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
// [12] padToAlignment(Blob *, int, char = 0);
// [10] Testing copy to a blob
// [ 9] Testing getContiguousRangeOrCopy
// [ 8] Testing getContiguousDataBuffer
// [ 7] Testing copy from a blob
// [ 6] Testing findBufferIndexAndOffset
// [ 5] Testing erase
// [ 4] Testing HexDump with offset and length
// [ 3] Testing HexDump
// [ 2] Testing compare
// [ 1] Testing "write special cases"
//-----------------------------------------------------------------------------
// [11] CONCERN: append doesn't do excessive 'reserveBufferCapacity'.
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

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

class BlobBufferFactory : public bdlbb::BlobBufferFactory {
    // This 'class' is just like a 'SimpleBlobBufferFactory' except that it
    // initializes the first byte of each blob buffer to '#' and counts the
    // number of the 'allocate' function calls.

    // PRIVATE DATA
    int               d_numAllocateCalls;
    int               d_size;
    bslma::Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED:
    BlobBufferFactory(const BlobBufferFactory&);

  public:
    // CREATORS
    explicit
    BlobBufferFactory(int initialSize, bslma::Allocator *basicAllocator = 0)
    : d_numAllocateCalls(0)
    , d_size(initialSize)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    virtual ~BlobBufferFactory()
    {
    }

    // MANIPULATORS
    virtual void allocate(bdlbb::BlobBuffer *buffer)
    {
        ++d_numAllocateCalls;
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

    int numAllocateCalls() const
        // Return how many times the 'allocate' function was called.
    {
        return d_numAllocateCalls;
    }
};

// ============================================================================
//                              GLOBAL TYPEDEF
// ----------------------------------------------------------------------------

typedef bdlbb::BlobUtil       Util;
typedef bdlbb::Blob           Blob;
typedef bdlbb::BlobBuffer     BlobBuffer;
typedef bsls::Types::Int64    Int64;
typedef bslmf::MovableRefUtil MoveUtil;

int verbose;
int veryVerbose;
int veryVeryVerbose;

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions generate a string of repeating characters in the
// range [a-z] of a specified length.

enum FillType {
    e_LETTERS,
    e_DIGITS
};

int ggg(bsl::string *result, int length, FillType fill = e_LETTERS)
{
    // Clear the specified 'result' and append a subset of the string
    // "abcdefghijklmnopqrstuvwxyz" or "0123456789", repeating the pattern
    // until the total length of 'result' reaches the specified 'length'.
    // Optionally specify 'fill' to choose either letters or digits.

    static const char letters[] = "abcdefghijklmnopqrstuvwxyz";
    static const char digits[]  = "0123456789";

    const char * const DATA = fill == e_LETTERS ? letters : digits;
    const int k_DATA_SIZE =
                      (fill == e_LETTERS ? sizeof letters : sizeof digits) - 1;

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

bsl::string& gg(bsl::string *result, int length, FillType fill = e_LETTERS)
{
    // Return, by reference, the specified 'result' with its value adjusted
    // according to the specified 'length' and 'fill'.

    ASSERT(0 == ggg(result, length, fill));
    return *result;
}

bsl::string g(int length, FillType fill = e_LETTERS)
{
    // Return, by value, a new object corresponding to the specified 'length'
    // and 'fill'.

    bsl::string object;
    return gg(&object, length, fill);
}

// ============================================================================
//                             HELPER FUNCTIONS
// ----------------------------------------------------------------------------

void copyStringToBlob(bdlbb::Blob *dest, const bsl::string& str)
{
    dest->setLength(static_cast<int>(str.length()));
    int         numBytesRemaining = static_cast<int>(str.length());
    const char *data              = str.data();
    int         bufferIndex       = 0;
    while (numBytesRemaining) {
        bdlbb::BlobBuffer buffer         = dest->buffer(bufferIndex);
        int               numBytesToCopy =
                                    bsl::min(numBytesRemaining, buffer.size());
        bsl::memcpy(buffer.data(), data, numBytesToCopy);
        data += numBytesToCopy;
        numBytesRemaining -= numBytesToCopy;
        ++bufferIndex;
    }
    ASSERT(0 == numBytesRemaining);
}

void copyBlobToString(bsl::string *dest, const bdlbb::Blob& blob)
{
    dest->clear();
    int numBytesRemaining = blob.length();
    int bufferIndex = 0;
    while (numBytesRemaining) {
        bdlbb::BlobBuffer buffer = blob.buffer(bufferIndex);
        int numBytesToCopy = bsl::min(numBytesRemaining, buffer.size());
        dest->append(buffer.data(), buffer.data() + numBytesToCopy);
        numBytesRemaining -= numBytesToCopy;
        ++bufferIndex;
    }
    ASSERT(0 == numBytesRemaining);
}

bsl::string expectedOut2Case3[] = {
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

bsl::string expectedOut4Case3[] = {
    // 0
    "",

    // 1
    "     0:   65203136 61626364 65203137 61626364     |e 16abcde 17abcd|\n"
    "    16:   65203138 61626364 65203139 61626364     |e 18abcde 19abcd|\n"
    "    32:   65203230 61626364 65203231 61626364     |e 20abcde 21abcd|\n"
    "    48:   65203232 61626364 65203233 6162         |e 22abcde 23ab  |\n",

    // 2
    "     0:   65203136 61626364 65203137 61626364     |e 16abcde 17abcd|\n"
    "    16:   65203138 61626364 65203139 61626364     |e 18abcde 19abcd|\n"
    "    32:   65203230 61626364 65203231 61626364     |e 20abcde 21abcd|\n"
    "    48:   65203232 61626364 65203233 6162         |e 22abcde 23ab  |\n",

    // 3
    "     0:   61626364 65203137 61626364 65203138     |abcde 17abcde 18|\n"
    "    16:   61626364 65203139 61626364 65203230     |abcde 19abcde 20|\n"
    "    32:   61626364 65203231 61626364 65203232     |abcde 21abcde 22|\n"
    "    48:   61626364 65203233 61626364 65203234     |abcde 23abcde 24|\n",

    // 4
    "     0:   65203137 61626364 65203138 61626364     |e 17abcde 18abcd|\n"
    "    16:   65203139 61626364 65203230 61626364     |e 19abcde 20abcd|\n"
    "    32:   65203231 61626364 65203232 61626364     |e 21abcde 22abcd|\n"
    "    48:   65203233 61626364 65203234 61626364     |e 23abcde 24abcd|\n"
    "    64:   6520                                    |e               |\n",
};

static bool bad_jk(int j, int k, bdlbb::Blob& blob)
{
    return (j < 0 || k < 0 || j + k > blob.totalSize());
}

namespace {
namespace u {

void checkBlob(const bdlbb::Blob& dst,
               int                blobBufferSize,
               int                prevSize,
               char               otherChar,
               char               fillChar)
    // Check, with 'ASSERT', that the first specified 'prevSize' bytes in the
    // specified 'dst' have the specified value 'otherChar', and that any
    // remaining bytes, if any, have the specified value 'fillChar'.  The
    // behavior is undefined unless all the buffers in the blob are of
    // specified size 'blobBufferSize'.
{
    ASSERT(0 < blobBufferSize);
    ASSERT(0 <= prevSize);

    for (int ii = 0; ii < dst.length(); ++ii) {
        int bufIdx = ii / blobBufferSize;
        int offset = ii % blobBufferSize;

        const char bufChar = dst.buffer(bufIdx).data()[offset];
        const char exp     = ii < prevSize ? otherChar : fillChar;

        ASSERT(exp == bufChar);
    }
}

bool areBlobsBasicallyEqual(const Blob& lhs, const Blob& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' are basically equal and
    // 'false' otherwise.  Two blobs are basically equal when all their
    // parameters are exactly the same, except that the 'BlobBuffer' pointers
    // can point to different shared buffers.  This function is used instead of
    // equality comparison operator in tests of move-insertion functions.
{
    if (lhs.totalSize()            != rhs.totalSize() ||
        lhs.length()               != rhs.length() ||
        lhs.numBuffers()           != rhs.numBuffers() ||
        lhs.numDataBuffers()       != rhs.numDataBuffers() ||
        lhs.lastDataBufferLength() != rhs.lastDataBufferLength()) {
        return false;                                                 // RETURN
    }
    for (int i = 0; i < lhs.numBuffers(); ++i) {
        if (lhs.buffer(i).size() != rhs.buffer(i).size()) {
            return false;                                             // RETURN
        }
    }
    return true;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'prependWithCapacityBuffer' FUNCTION
        //
        // Concerns:
        //: 1 The function correctly adds data to the beginning of the 'Blob'.
        //:
        //: 2 The function uses the existing capacity first if blob is empty.
        //:   Otherwise uses the provided capacity buffer first.  Then
        //:   allocates memory using the factory of the 'Blob'.
        //:
        //: 3 Unused part of 'BlobBuffer' is returned after the function call.
        //:
        //: 4 The function works correctly when default-constructed (empty)
        //:   capacity buffer is provided.
        //:
        //: 5 Writing 0 bytes is a NOOP.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a table specifying various values of blob buffers length,
        //:   initital blob size and length, capacity buffer initial size,
        //:   input data size, expected capacity buffer size after the call and
        //:   expected number of buffer allocations during the call.
        //:
        //: 2 For each row in the table create a blob, capacity buffer and
        //:   input data block having the specified parameters.  Fill the blob
        //:   with sequence of letters, the capacity buffer with '#' chars,
        //:   the input data block with digits.
        //:
        //: 3 Invoke the 'prependWithCapacityBuffer' function using as
        //:   arguments the prepared objects.
        //:
        //: 4 Verify that size of the capacity buffer matches the value
        //:   specified in the table.
        //:
        //: 5 Verify that number of buffer allocations matches the value
        //:   specified in the table.
        //:
        //: 6 Verify that length of the blob is increased by the size of the
        //:   input block.
        //:
        //: 7 Verify that data in the blob that existed before the call is
        //:   unchanged.
        //:
        //: 8 Verify that the data is added to the beginning of the blob and
        //:   matches the input data block.
        //:
        //: 9 Repeat steps 2 to 8 for each row in the table.
        //:
        //:10 Do negative testing to verify that asserts catch all the
        //:   undefined behavior in the contract.
        //
        // Testing:
        //   void prependWithCapacityBuffer(Blob*,BlobBuffer*,const char*,int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'prependWithCapacityBuffer' FUNCTION\n"
                             "============================================\n";

        static const struct Data {
            int d_line;
            int d_blobBufferSize;
            int d_blobBuffersNumber; // Blob::size() == d_blobBufferSize *
                                     //                 d_blobBuffersNumber
            int d_blobLength;
            int d_capacityBufferSize;
            int d_inputDataSize;
            int d_expectedCapacityBufferSize;
            int d_expectedAllocateCalls; // during the 'prepend' call
        } DATA[] = {
            { L_, 64, 1,  0, 64,   0, 64, 0 }, // Prepend 0 bytes
            { L_, 64, 1,  0, 64,  32, 64, 0 }, // Prepend to empty blob
            { L_, 64, 1, 32,  0,  32, 32, 1 }, // Empty capacity buffer
            { L_, 64, 1, 32, 32,  32,  0, 0 }, // Non-empty capacity buffer
            { L_, 64, 1, 32, 64,  32, 32, 0 }, // Non-empty capacity buffer
            { L_, 64, 2, 70,  0,  58,  6, 1 }, // Partially filled blob
            { L_, 64, 2, 70, 64,  58,  6, 0 }, // Partially filled blob
            { L_, 64, 2,  0,  0,  70,  0, 0 }, // Empty blob but with capacity
            { L_, 64, 2,  0, 64,  70, 64, 0 }, // Empty blob but with capacity
            { L_, 64, 2,  0,  0, 128,  0, 0 }, // Fully used blob capacity
            { L_, 64, 2,  0, 64, 128, 64, 0 }, // Fully used blob capacity
            { L_, 64, 2,  0,  0, 130, 62, 1 }, // Fully used blob capacity +buf
            { L_, 64, 2,  0, 64, 130, 62, 0 }, // Fully used blob capacity +buf
            { L_, 64, 1, 64, 64, 256,  0, 3 }, // Prepend big buffer
            { L_, 64, 1, 64, 64, 257, 63, 4 }  // Prepend big buffer
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Data& data                  = DATA[ti];
            const int LINE                    = data.d_line;
            const int BLOB_BUFFER_SIZE        = data.d_blobBufferSize;
            const int BLOB_BUFFERS_NUMBER     = data.d_blobBuffersNumber;
            const int BLOB_LENGH              = data.d_blobLength;
            const int CAPACITY_BUFFER_SIZE    = data.d_capacityBufferSize;
            const int INPUT_DATA_SIZE         = data.d_inputDataSize;
            const int EXPECTED_CAPACITY_BUFFER_SIZE =
                                             data.d_expectedCapacityBufferSize;
            const int EXPECTED_ALLOCATE_CALLS = data.d_expectedAllocateCalls;

            if (veryVerbose) {
                P_(LINE) P_(BLOB_BUFFER_SIZE) P_(BLOB_BUFFERS_NUMBER)
                P_(BLOB_LENGH) P_(CAPACITY_BUFFER_SIZE)
                P_(INPUT_DATA_SIZE) P_(EXPECTED_CAPACITY_BUFFER_SIZE)
                P(EXPECTED_ALLOCATE_CALLS)
            }

            ASSERT(BLOB_BUFFER_SIZE > 0);

            BlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob       blob(&factory);

            // Set up the Blob size (copyStringToBlob() will reduce the length)
            blob.setLength(BLOB_BUFFER_SIZE * BLOB_BUFFERS_NUMBER);

            // Set up the Blob length
            bsl::string suffix = g(BLOB_LENGH);
            copyStringToBlob(&blob, suffix);
            ASSERT(blob.length() == BLOB_LENGH);
            ASSERT(blob.totalSize() == BLOB_BUFFER_SIZE * BLOB_BUFFERS_NUMBER);

            // Set up the capacity buffer
            ASSERT(CAPACITY_BUFFER_SIZE <= BLOB_BUFFER_SIZE);
            bdlbb::BlobBuffer capacityBuffer;
            if (CAPACITY_BUFFER_SIZE > 0) {
                factory.allocate(&capacityBuffer);
                capacityBuffer.trim(CAPACITY_BUFFER_SIZE);
                bsl::memset(capacityBuffer.data(), '#', capacityBuffer.size());
            }

            // Set up the input data
            bsl::string source = g(INPUT_DATA_SIZE, e_DIGITS);

            // Remember the counter's value before the call
            int numAllocateCalls = factory.numAllocateCalls();

            // TEST
            Util::prependWithCapacityBuffer(&blob,
                                            &capacityBuffer,
                                            source.data(),
                                            static_cast<int>(source.length()));

            // Calculate the number of allocations during the call
            numAllocateCalls = factory.numAllocateCalls() - numAllocateCalls;

            // Verify the capacity buffer
            ASSERT(capacityBuffer.size() == EXPECTED_CAPACITY_BUFFER_SIZE);

            // Verify the number of allocations
            ASSERTV(numAllocateCalls,
                    numAllocateCalls == EXPECTED_ALLOCATE_CALLS);

            // Verify the data is added
            ASSERT(blob.length() == BLOB_LENGH + INPUT_DATA_SIZE);

            // Verify the old data is untouched
            {
                bsl::string tmp(BLOB_LENGH, '\x0');
                Util::copy(tmp.data(), blob, INPUT_DATA_SIZE, BLOB_LENGH);
                ASSERT(tmp == suffix);
            }
            // Verify 'source' is added
            {
                bsl::string tmp(INPUT_DATA_SIZE, '\x0');
                Util::copy(tmp.data(), blob, 0, INPUT_DATA_SIZE);
                ASSERT(tmp == source);
            }
        }

        if (verbose) cout << "Negative testing\n";
        {
            bsls::AssertTestHandlerGuard hG;

            BlobBufferFactory factory(64);
            bdlbb::Blob       blob(&factory);
            bdlbb::BlobBuffer capacityBuffer;
            char              chunk[32];

            ASSERT_PASS(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk, sizeof chunk));
            ASSERT_FAIL(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                     0, &capacityBuffer, chunk, sizeof chunk));
            ASSERT_FAIL(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                 &blob, &capacityBuffer,     0, sizeof chunk));
            ASSERT_PASS(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk,            0));
            ASSERT_FAIL(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk,           -1));
            ASSERT_FAIL(bdlbb::BlobUtil::prependWithCapacityBuffer(
                                 &blob,               0, chunk, sizeof chunk));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'appendWithCapacityBuffer' FUNCTION
        //
        // Concerns:
        //: 1 The function correctly adds data to the end of the 'Blob'.
        //:
        //: 2 The function tries to take memory from the following places in
        //:   the specified order:
        //:     1 Existing capacity of the 'Blob';
        //:     2 The given 'BlobBuffer';
        //:     3 Factory of the 'Blob'.
        //:
        //: 3 Unused part of 'BlobBuffer' is returned after the function call.
        //:
        //: 4 The function works correctly when default-constructed (empty)
        //:   capacity buffer is provided.
        //:
        //: 5 Writing 0 bytes is a NOOP.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a table specifying various values of blob buffers length,
        //:   initital blob size and length, capacity buffer initial size,
        //:   input data size, expected capacity buffer size after the call and
        //:   expected number of buffer allocations during the call.
        //:
        //: 2 For each row in the table create a blob, capacity buffer and
        //:   input data block having the specified parameters.  Fill the blob
        //:   with sequence of letters, the capacity buffer with '#' chars,
        //:   the input data block with digits.
        //:
        //: 3 Invoke the 'appendWithCapacityBuffer' function using as arguments
        //:   the prepared objects.
        //:
        //: 4 Verify that size of the capacity buffer matches the value
        //:   specified in the table.
        //:
        //: 5 Verify that number of buffer allocations matches the value
        //:   specified in the table.
        //:
        //: 6 Verify that length of the blob is increased by the size of the
        //:   input block.
        //:
        //: 7 Verify that data in the blob that existed before the call is
        //:   unchanged.
        //:
        //: 8 Verify that the data is added to the end of the blob and matches
        //:   the input data block.
        //:
        //: 9 Repeat steps 2 to 8 for each row in the table.
        //:
        //:10 Do negative testing to verify that asserts catch all the
        //:   undefined behavior in the contract.
        //
        // Testing:
        //   void appendWithCapacityBuffer(Blob*,BlobBuffer*,const char*,int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'appendWithCapacityBuffer' FUNCTION\n"
                             "===========================================\n";

        static const struct Data {
            int d_line;
            int d_blobBufferSize;
            int d_blobBuffersNumber; // Blob::size() == d_blobBufferSize *
                                     //                 d_blobBuffersNumber
            int d_blobLength;
            int d_capacityBufferSize;
            int d_inputDataSize;
            int d_expectedCapacityBufferSize;
            int d_expectedAllocateCalls; // during the 'append' call
        } DATA[] = {
            { L_, 64, 1,  0, 64,   0, 64, 0 }, // Append 0 bytes
            { L_, 64, 0,  0, 64,   0, 64, 0 }, // Append 0 bytes
            { L_, 64, 1,  0, 64,  32, 64, 0 }, // Capacity buffer is unused
            { L_, 64, 1,  0, 64,  64, 64, 0 }, // Capacity buffer is unused
            { L_, 64, 1, 64, 64,  64,  0, 0 }, // Capacity buffer is used
            { L_, 64, 1, 64,  0,  32, 32, 1 }, // Empty capacity buffer,
                                               // leftover
            { L_, 64, 2,  0,  0, 128,  0, 0 }, // More than 1 empty buffers
            { L_, 64, 2,  0,  0, 127,  0, 0 }, // More than 1 empty buffers
            { L_, 64, 2,  0,  0, 150, 42, 1 }, // More than 1 empty buffers
            { L_, 64, 2,  0, 64, 150, 42, 0 }, // More than 1 empty buffers
            { L_, 64, 2, 70,  0,  32,  0, 0 }, // Partially filled blob buffer
            { L_, 64, 2, 70,  0, 100, 22, 1 }, // Partially filled blob buffer
            { L_, 64, 2, 70, 64, 100, 22, 0 }, // Partially filled blob buffer
            { L_, 64, 1,  0, 64, 256,  0, 2 }, // Append big buffer
            { L_, 64, 1,  0,  0, 256,  0, 3 }, // Append big buffer
            { L_, 64, 0,  0, 64, 257, 63, 4 }  // Append big buffer
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Data& data                  = DATA[ti];
            const int LINE                    = data.d_line;
            const int BLOB_BUFFER_SIZE        = data.d_blobBufferSize;
            const int BLOB_BUFFERS_NUMBER     = data.d_blobBuffersNumber;
            const int BLOB_LENGH              = data.d_blobLength;
            const int CAPACITY_BUFFER_SIZE    = data.d_capacityBufferSize;
            const int INPUT_DATA_SIZE         = data.d_inputDataSize;
            const int EXPECTED_CAPACITY_BUFFER_SIZE =
                                             data.d_expectedCapacityBufferSize;
            const int EXPECTED_ALLOCATE_CALLS = data.d_expectedAllocateCalls;

            if (veryVerbose) {
                P_(LINE) P_(BLOB_BUFFER_SIZE) P_(BLOB_BUFFERS_NUMBER)
                P_(BLOB_LENGH) P_(CAPACITY_BUFFER_SIZE)
                P_(INPUT_DATA_SIZE) P_(EXPECTED_CAPACITY_BUFFER_SIZE)
                P(EXPECTED_ALLOCATE_CALLS)
            }

            ASSERT(BLOB_BUFFER_SIZE > 0);

            BlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob       blob(&factory);

            // Set up the Blob size (copyStringToBlob() will reduce the length)
            blob.setLength(BLOB_BUFFER_SIZE * BLOB_BUFFERS_NUMBER);

            // Set up the Blob length
            bsl::string prefix = g(BLOB_LENGH);
            copyStringToBlob(&blob, prefix);
            ASSERT(blob.length() == BLOB_LENGH);
            ASSERT(blob.totalSize() == BLOB_BUFFER_SIZE * BLOB_BUFFERS_NUMBER);

            // Set up the capacity buffer
            ASSERT(CAPACITY_BUFFER_SIZE <= BLOB_BUFFER_SIZE);
            bdlbb::BlobBuffer capacityBuffer;
            if (CAPACITY_BUFFER_SIZE > 0) {
                factory.allocate(&capacityBuffer);
                capacityBuffer.trim(CAPACITY_BUFFER_SIZE);
                bsl::memset(capacityBuffer.data(), '#', capacityBuffer.size());
            }

            // Set up the input data
            bsl::string source = g(INPUT_DATA_SIZE, e_DIGITS);

            // Remember the counter's value before the call
            int numAllocateCalls = factory.numAllocateCalls();

            // TEST
            Util::appendWithCapacityBuffer(&blob,
                                           &capacityBuffer,
                                           source.data(),
                                           static_cast<int>(source.length()));

            // Calculate the number of allocations during the call
            numAllocateCalls = factory.numAllocateCalls() - numAllocateCalls;

            // Verify the capacity buffer
            ASSERT(capacityBuffer.size() == EXPECTED_CAPACITY_BUFFER_SIZE);

            // Verify the number of allocations
            ASSERTV(numAllocateCalls,
                    numAllocateCalls == EXPECTED_ALLOCATE_CALLS);

            // Verify the data is added
            ASSERT(blob.length() == BLOB_LENGH + INPUT_DATA_SIZE);

            // Verify the old data is untouched
            {
                bsl::string tmp(BLOB_LENGH, '\x0');
                Util::copy(tmp.data(), blob, 0, BLOB_LENGH);
                ASSERT(tmp == prefix);
            }
            // Verify 'source' is added
            {
                bsl::string tmp(INPUT_DATA_SIZE, '\x0');
                Util::copy(tmp.data(), blob, BLOB_LENGH, INPUT_DATA_SIZE);
                ASSERT(tmp == source);
            }
        }

        if (verbose) cout << "Negative testing\n";
        {
            bsls::AssertTestHandlerGuard hG;

            BlobBufferFactory factory(64);
            bdlbb::Blob       blob(&factory);
            bdlbb::BlobBuffer capacityBuffer;
            char              chunk[32];

            ASSERT_PASS(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk, sizeof chunk));
            ASSERT_FAIL(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                     0, &capacityBuffer, chunk, sizeof chunk));
            ASSERT_FAIL(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                 &blob, &capacityBuffer,     0, sizeof chunk));
            ASSERT_PASS(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk,            0));
            ASSERT_FAIL(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                 &blob, &capacityBuffer, chunk,           -1));
            ASSERT_FAIL(bdlbb::BlobUtil::appendWithCapacityBuffer(
                                 &blob,               0, chunk, sizeof chunk));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SAFE BUFFER ADD FUNCTIONS
        //   Safe functions check compliance with the required conditions and
        //   invoke appropriate 'bdlbb::Blob' method.  Therefore we use the
        //   result of the class method to verify the result of utility
        //   function.
        //   Since overloads that accept const references just call overloads
        //   that accept rvalue references, passing there a copy of incoming
        //   parameter, we are going to test the first ones superficially.
        //
        // Concerns:
        //: 1 All functions pass incoming 'BlobBuffer' reference (const or
        //:   rvalue) to the appropriate 'Blob' method.
        //:
        //: 2 All functions correctly calculate whether the incoming
        //:   'BlobBuffer' object can be added to the 'Blob'.
        //:
        //: 3 'BlobBuffer' object is moved by the functions accepting rvalue
        //:   references.
        //:
        //: 4. 'Blob' and 'BlobBuffer' objects remain unaffected on failure.
        //
        // Plan:
        //: 1 For all combinations of length, buffer size (for the factory),
        //:   and number of buffers, check that a blob after prepending and
        //:   appending data buffers using utility function accepting rvalue
        //:   reference, has the same characteristics as the blob modified by
        //:   class methods.  Verify that functions return zero value (success
        //:   result).  Verify that the 'use_count' of the inserted buffer is
        //:   equal to 1 (the buffer has been moved, not copied).
        //:
        //: 2 For some combinations of buffer size check that a blob after
        //:   prepending and appending data buffers using utility function
        //:   accepting const reference has the same characteristics as the
        //:   blob modified by function accepting rvalue reference.  Verify
        //:   that functions return zero value (success result).  Verify that
        //:   the 'use_count' of the inserted buffer is equal to 2 (the buffer
        //:   has been copied, not moved).  (C-1, 3)
        //:
        //: 3 Pass to utility function blob buffer with enormously big size
        //:   and verify that function returns non-zero value (failure result)
        //:   and blob is not changed.  (C-2, 4)
        //
        // Testing:
        //   int appendBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
        //   int appendBufferIfValid(Blob *d, const BlobBuffer& b);
        //   int appendDataBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
        //   int appendDataBufferIfValid(Blob *d, const BlobBuffer& b);
        //   int insertBufferIfValid(Blob *d, int i, MovableRef<BlobBuffer> b);
        //   int insertBufferIfValid(Blob *d, int i, const BlobBuffer& b);
        //   int prependDataBufferIfValid(Blob *d, MovableRef<BlobBuffer> b);
        //   int prependDataBufferIfValid(Blob *d, const BlobBuffer& b);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING SAFE BUFFER ADD FUNCTIONS\n"
                             "=================================\n";

        const int MAX_NUM =  5;
        const int SUCCESS =  0;
        const int FAILURE = -1;

        enum {
            APPEND_BUFFER       = 0,
            APPEND_DATA_BUFFER  = 1,
            PREPEND_DATA_BUFFER = 2,
            INSERT_BUFFER       = 3
        };

        if (verbose) cout << "Testing moving overloads" << endl;

        for (int bufferSize = 1; bufferSize <= MAX_NUM; ++bufferSize)
        for (int numBuffers = 0; numBuffers <= MAX_NUM; ++numBuffers)
        for (int dataLength = 0; dataLength <= bufferSize * numBuffers;
                                                  ++dataLength)
        for (int insertSize = 0; insertSize <= MAX_NUM; ++insertSize)
        {
            const int BUFFER_SIZE           = bufferSize;
            const int DATA_LENGTH           = dataLength;
            const int NUM_BUFFERS           = numBuffers;
            const int INSERT_BUFFER_SIZE    = insertSize;
            const int INIT_NUM_DATA_BUFFERS =
                                 (DATA_LENGTH + BUFFER_SIZE - 1) / BUFFER_SIZE;

            bslma::TestAllocator         da("default", veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            // Testing 'appendBufferIfValid', 'appendDataBufferIfValid' and
            // 'prependDataBufferIfValid'.

            for (int function = APPEND_BUFFER;
                 function <= PREPEND_DATA_BUFFER;
                 ++function) {
                const int FUNCTION = function;

                typedef int (*UtilFunction)(Blob *,
                                            bslmf::MovableRef<BlobBuffer>);
                typedef void (Blob::*MemberFunction)(
                                                bslmf::MovableRef<BlobBuffer>);

                UtilFunction   utilFunction   = 0;
                MemberFunction memberFunction = 0;
                int            index          = 0;

                switch (FUNCTION) {
                  case APPEND_BUFFER:
                    utilFunction   = &Util::appendBufferIfValid;
                    memberFunction = &Blob::appendBuffer;
                    index          = NUM_BUFFERS;
                    break;
                  case APPEND_DATA_BUFFER:
                    utilFunction   = &Util::appendDataBufferIfValid;
                    memberFunction = &Blob::appendDataBuffer;
                    index          = INIT_NUM_DATA_BUFFERS;
                    break;
                  case PREPEND_DATA_BUFFER:
                    utilFunction   = &Util::prependDataBufferIfValid;
                    memberFunction = &Blob::prependDataBuffer;
                    index          = 0;
                    break;
                  default:
                    ASSERTV(!"Unexpected function mode");
                }

                if (veryVerbose) {
                    T_; P_(BUFFER_SIZE);
                        P_(DATA_LENGTH);
                        P_(NUM_BUFFERS);
                        P_(INSERT_BUFFER_SIZE);
                        P(FUNCTION);
                }

                // Object initialization.

                Blob                           model;
                const Blob&                    MODEL = model;
                Blob                           dst;
                const Blob&                    DST   = dst;
                bdlbb::SimpleBlobBufferFactory factory(BUFFER_SIZE);

                for (int i = 0; i < NUM_BUFFERS; ++i) {
                    BlobBuffer initialBuffer;
                    factory.allocate(&initialBuffer);

                    model.appendBuffer(initialBuffer);
                    dst.appendBuffer(initialBuffer);
                }

                model.setLength(DATA_LENGTH);
                dst.setLength(DATA_LENGTH);

                ASSERT(MODEL == DST);

                // Buffer initialization

                bdlbb::SimpleBlobBufferFactory insertFactory(
                                                           INSERT_BUFFER_SIZE);
                bdlbb::BlobBuffer              modelBuffer;
                bdlbb::BlobBuffer              buffer;
                insertFactory.allocate(&modelBuffer);
                insertFactory.allocate(&buffer);

                (model.*memberFunction)(MoveUtil::move(modelBuffer));

                // Tested action.

                int result = utilFunction(&dst, MoveUtil::move(buffer));

                // Verification.

                ASSERT(SUCCESS == result);
                ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                // The following check fails because of inconsistent behavior
                // of 'bsl::vector<>::insert' method (see {DRQS 170573799}).
                // This check should be uncommented after the issue is fixed.
                //
                // ASSERT(1 == DST.buffer(index).buffer().use_count());

                // Insert empty buffer.

                bdlbb::BlobBuffer modelEmptyBuffer;
                bdlbb::BlobBuffer emptyBuffer;

                switch (FUNCTION) {
                  case APPEND_BUFFER:
                    index = NUM_BUFFERS + 1;
                    break;
                  case APPEND_DATA_BUFFER:
                    index = INIT_NUM_DATA_BUFFERS + 1;
                    break;
                  case PREPEND_DATA_BUFFER:
                    index = 0;
                    break;
                  default:
                    ASSERTV(!"Unexpected function mode");
                }

                (model.*memberFunction)(MoveUtil::move(modelEmptyBuffer));

                // Tested action.

                result = utilFunction(&dst, MoveUtil::move(emptyBuffer));

                // Verification.

                ASSERT(SUCCESS == result);
                ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                // The following check fails because of inconsistent behavior
                // of 'bsl::vector<>::insert' method (see {DRQS 170573799}).
                // This check should be uncommented after the issue is fixed.
                //
                // ASSERT(1 == DST.buffer(index).buffer().use_count());

                // Last data buffer can be trimmed during appending new data
                // buffer (but not prepending or appending non-data buffer) .
                // Therefore, the potentially allowed size of the added buffer
                // should be adjusted accordingly.

                const int TRIMMED_SIZE =
                     APPEND_BUFFER       == FUNCTION ||
                     PREPEND_DATA_BUFFER == FUNCTION
                        ? 0
                        : 0 == DST.numDataBuffers()
                              ? 0
                              : DST.buffer(DST.numDataBuffers() - 1).size() -
                                    DST.lastDataBufferLength();

                // The following blob buffers represent null buffer but have
                // non-zero size.  This can lead to undefined behavior in real
                // world, but we are not going to make read/write operations on
                // these buffers during testing.  Only their sizes are valuable
                // for our test.

                bdlbb::BlobBuffer modelMaximalValidBuffer;
                bdlbb::BlobBuffer maximalValidBuffer;
                bdlbb::BlobBuffer tinyBuffer;

                modelMaximalValidBuffer.setSize(
                                     INT_MAX - DST.totalSize() + TRIMMED_SIZE);
                maximalValidBuffer.setSize(
                                     INT_MAX - DST.totalSize() + TRIMMED_SIZE);
                tinyBuffer.setSize(1);

                switch (FUNCTION) {
                  case APPEND_BUFFER:
                    index = NUM_BUFFERS + 1;
                    break;
                  case APPEND_DATA_BUFFER:
                    index = INIT_NUM_DATA_BUFFERS + 1;
                    break;
                  case PREPEND_DATA_BUFFER:
                    index = 0;
                    break;
                  default:
                    ASSERTV(!"Unexpected function mode");
                }

                (model.*memberFunction)(MoveUtil::move(
                                                     modelMaximalValidBuffer));

                // Tested action.

                result = utilFunction(&dst, MoveUtil::move(
                                                          maximalValidBuffer));

                // Verification.

                ASSERT(SUCCESS == result);
                ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                // The following check fails because of inconsistent behavior
                // of 'bsl::vector<>::insert' method (see {DRQS 170573799}).
                // This check should be uncommented after the issue is fixed.
                //
                // ASSERT(1 == DST.buffer(index).buffer().use_count());

                result = utilFunction(&dst, MoveUtil::move(tinyBuffer));

                ASSERT(FAILURE == result);
                ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));
            }

            // Testing 'insertBufferIfValid'.
            {
                for (int position = 0; position <= NUM_BUFFERS; ++position) {
                    const int POSITION = position;

                    if (veryVerbose) {
                        T_; P_(BUFFER_SIZE);
                            P_(DATA_LENGTH);
                            P_(NUM_BUFFERS);
                            P_(INSERT_BUFFER_SIZE);
                            P(POSITION);
                    }

                    // Object initialization.

                    Blob                           model;
                    const Blob&                    MODEL = model;
                    Blob                           dst;
                    const Blob&                    DST   = dst;
                    bdlbb::SimpleBlobBufferFactory factory(BUFFER_SIZE);

                    for (int i = 0; i < NUM_BUFFERS; ++i) {
                        BlobBuffer initialBuffer;
                        factory.allocate(&initialBuffer);

                        model.appendBuffer(initialBuffer);
                        dst.appendBuffer(initialBuffer);
                    }

                    model.setLength(DATA_LENGTH);
                    dst.setLength(DATA_LENGTH);

                    ASSERT(MODEL == DST);

                    // Buffer initialization

                    bdlbb::SimpleBlobBufferFactory insertFactory(
                                                           INSERT_BUFFER_SIZE);
                    BlobBuffer                     modelBuffer;
                    BlobBuffer                     buffer;
                    insertFactory.allocate(&modelBuffer);
                    insertFactory.allocate(&buffer);

                    model.insertBuffer(POSITION, MoveUtil::move(modelBuffer));

                    // Tested action.

                    int result = Util::insertBufferIfValid(
                                                       &dst,
                                                       POSITION,
                                                       MoveUtil::move(buffer));

                    // Verification.

                    ASSERT(SUCCESS == result);
                    ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                    // Insert empty buffer.

                    BlobBuffer modelEmptyBuffer;
                    BlobBuffer emptyBuffer;

                    model.insertBuffer(POSITION,
                                       MoveUtil::move(modelEmptyBuffer));

                    // Tested action.

                    result = Util::insertBufferIfValid(
                                                  &dst,
                                                  POSITION,
                                                  MoveUtil::move(emptyBuffer));

                    // Verification.

                    ASSERT(SUCCESS == result);
                    ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                    // The following check fails because of inconsistent
                    // behavior of 'bsl::vector<>::insert' method (see
                    // {DRQS 170573799}). This check should be uncommented
                    // after the issue is fixed.
                    //
                    // ASSERT(1 == DST.buffer(index).buffer().use_count());

                    // Testing size exceeding scenario.

                    BlobBuffer modelMaximalValidBuffer;
                    BlobBuffer maximalValidBuffer;
                    BlobBuffer tinyBuffer;

                    modelMaximalValidBuffer.setSize(INT_MAX - DST.totalSize());
                    maximalValidBuffer.setSize(INT_MAX - DST.totalSize());
                    tinyBuffer.setSize(1);

                    model.insertBuffer(POSITION,
                                      MoveUtil::move(modelMaximalValidBuffer));

                    // Tested action.

                    result = Util::insertBufferIfValid(
                                           &dst,
                                           POSITION,
                                           MoveUtil::move(maximalValidBuffer));

                    // Verification.

                    ASSERT(SUCCESS == result);
                    ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));

                    // The following check fails because of inconsistent
                    // behavior of 'bsl::vector<>::insert' method (see
                    // {DRQS 170573799}). This check should be uncommented
                    // after the issue is fixed.
                    //
                    // ASSERT(1 == DST.buffer(index).buffer().use_count());

                    result = Util::insertBufferIfValid(
                                                   &dst,
                                                   POSITION,
                                                   MoveUtil::move(tinyBuffer));

                    ASSERT(FAILURE == result);
                    ASSERT(u::areBlobsBasicallyEqual(MODEL, DST));
                }
            }
        }

        if (verbose) cout << "Testing copy overloads" << endl;

        for (int insertSize = 0; insertSize <= MAX_NUM; ++insertSize)
        {
            const int INSERT_BUFFER_SIZE = insertSize;

            bslma::TestAllocator         da("default", veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            for (int function = APPEND_BUFFER;
                 function <= INSERT_BUFFER;
                 ++function) {
                const int FUNCTION = function;

                if (veryVerbose) {
                    T_ P_(INSERT_BUFFER_SIZE); P(FUNCTION);
                }

                // Object initialization.

                Blob                           model;
                const Blob&                    MODEL = model;
                Blob                           dst;
                const Blob&                    DST   = dst;

                // Buffer initialization

                bdlbb::SimpleBlobBufferFactory insertFactory(
                                                           INSERT_BUFFER_SIZE);
                bdlbb::BlobBuffer              buffer;
                insertFactory.allocate(&buffer);

                int result      = 0;
                int modelResult = 0;

                // Tested action.

                switch (FUNCTION) {
                  case APPEND_BUFFER:
                    result = Util::appendBufferIfValid(&dst, buffer);
                    ASSERT(2 == buffer.buffer().use_count());
                    modelResult = Util::appendBufferIfValid(
                                                       &model,
                                                       MoveUtil::move(buffer));
                    break;
                  case APPEND_DATA_BUFFER:
                    result = Util::appendDataBufferIfValid(&dst, buffer);
                    ASSERT(2 == buffer.buffer().use_count());
                    modelResult = Util::appendDataBufferIfValid(
                                                       &model,
                                                       MoveUtil::move(buffer));
                    break;
                  case PREPEND_DATA_BUFFER:
                    result = Util::prependDataBufferIfValid(&dst, buffer);
                    ASSERT(2 == buffer.buffer().use_count());
                    modelResult = Util::prependDataBufferIfValid(
                                                       &model,
                                                       MoveUtil::move(buffer));
                    break;
                  case INSERT_BUFFER:
                    result = Util::insertBufferIfValid(&dst, 0, buffer);
                    ASSERT(2 == buffer.buffer().use_count());
                    modelResult = Util::insertBufferIfValid(
                                                       &model,
                                                       0,
                                                       MoveUtil::move(buffer));
                    break;
                  default:
                    ASSERTV(!"Unexpected function mode");
                }

                ASSERT(modelResult == result);
                ASSERT(MODEL       == DST   );

            }
        }

        if (verbose) cout << "Testing specific failure scenarios." << endl;
        {
            bslma::TestAllocator  ta("test", veryVeryVerbose);
            bsl::shared_ptr<char> buffer((char *)ta.allocate(1), &ta);

            const BlobBuffer MODEL_EMPTY;
            const BlobBuffer EMPTY(MODEL_EMPTY);
            const BlobBuffer MODEL_TINY_DUMMY(buffer, 1);
            const BlobBuffer TINY_DUMMY(MODEL_TINY_DUMMY);

            {
                Blob        model;
                const Blob& MODEL = model;
                Blob        dst;
                const Blob& DST   = dst;

                int result = Util::insertBufferIfValid(&dst, -1, EMPTY);

                ASSERT(FAILURE     == result);
                ASSERT(MODEL       == DST   );
                ASSERT(MODEL_EMPTY == EMPTY );


                result = Util::insertBufferIfValid(&dst,
                                                   -1,
                                                   MoveUtil::move(EMPTY));

                ASSERT(FAILURE     == result);
                ASSERT(MODEL       == DST   );
                ASSERT(MODEL_EMPTY == EMPTY );

                result = Util::insertBufferIfValid(&dst, 1, EMPTY);

                ASSERT(FAILURE     == result);
                ASSERT(MODEL       == DST   );
                ASSERT(MODEL_EMPTY == EMPTY );

                result = Util::insertBufferIfValid(&dst,
                                                   1,
                                                   MoveUtil::move(EMPTY));

                ASSERT(FAILURE     == result);
                ASSERT(MODEL       == DST   );
                ASSERT(MODEL_EMPTY == EMPTY );
            }

            {
                // In addition to total size of the blob, return value of
                // 'Blob::numBuffers()' can also be overflowed.  The safe
                // functions must prevent such situations (i.e. the number of
                // buffers in blob must *not* exceed 'INT_MAX' value) so we
                // have to check it.  To simulate this scenario we need to
                // create a blob with 'INT_MAX' buffers.  But since it consumes
                // a lot of resources, we comment out this test.  The manual
                // test was performed.

                // Blob        blob;
                // const Blob& BLOB = blob;
                // Blob        model;
                // const Blob& MODEL = model;
                //
                // for (int i = 0; i < INT_MAX; ++i) {
                //     blob.appendBuffer(EMPTY);
                // }
                //
                // ASSERT(INT_MAX == BLOB.numBuffers());
                // ASSERT(0       == BLOB.totalSize() );
                //
                // const int APPEND_RESULT = Util::appendBufferIfValid(
                //                                 &blob,
                //                                 MoveUtil::move(TINY_DUMMY));
                //
                // ASSERT(FAILURE          == APPEND_RESULT);
                // ASSERT(MODEL            == BLOB         );
                // ASSERT(MODEL_TINY_DUMMY == TINY_DUMMY   );
                //
                // const int APPEND_DATA_RESULT =
                //   Util::appendDataBufferIfValid(&blob,
                //                                 MoveUtil::move(TINY_DUMMY));
                // ASSERT(FAILURE          == APPEND_DATA_RESULT);
                // ASSERT(MODEL            == BLOB              );
                // ASSERT(MODEL_TINY_DUMMY == TINY_DUMMY        );
                //
                // const int INSERT_RESULT = Util::insertBufferIfValid(
                //                                 &blob,
                //                                 0,
                //                                 MoveUtil::move(TINY_DUMMY));
                // ASSERT(FAILURE          == INSERT_RESULT);
                // ASSERT(MODEL            == BLOB         );
                // ASSERT(MODEL_TINY_DUMMY == TINY_DUMMY   );
                //
                // const int PREPEND_DATA_RESULT =
                //     Util::prependDataBufferIfValid(
                //                                 &blob,
                //                                 MoveUtil::move(TINY_DUMMY));
                // ASSERT(FAILURE          == PREPEND_DATA_RESULT);
                // ASSERT(MODEL            == BLOB               );
                // ASSERT(MODEL_TINY_DUMMY == TINY_DUMMY         );
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING PADTOALIGNMENT
        //
        // Concerns:
        //: 1 That 'padToAlignment' will properly align the buffer length.
        //:   o that the length after padding is a multiple of 'alignment'
        //:
        //:   o that the number of bytes padded was less than 'alignment'.
        //:
        //:   o that the padded bytes all match the fill char
        //:
        //:   o That the bytes in the blob prior to the padding were
        //:     unaffected.
        //:
        //: 2 That characters appended will be:
        //:   o the fill char that was passed
        //:
        //:   o '\0' if no fill char was passed
        //:
        //: 3 That 'assert's detect the undefined behavior in the contract.
        //
        // Plan:
        //: 1 Create a function 'checkBlob' in the unnamed namespace that will
        //:   examine a blob and verify that:
        //:   o the first 'prevBlobSize' bytes all match the specified
        //:     'otherChar'
        //:
        //:   o the remaining bytes all match the specified 'fillChar'
        //:
        //: 2 Write three nested loops to call 'padToAlignment' with a wide
        //:   variety of values of the blob buffer size, the preexisting blob
        //:   length, and all valid values of 'alignment'.  In these cases, set
        //:   all bytes in the preexisting blob to one byte value 'otherChar',
        //:   and specify a byte value to the 'fillChar' arg of
        //:   'padToAlignment' that is different from 'otherChar'.  After the
        //:   call to 'padToAlignment',
        //:   o check that the blob length is a multiple of 'alignment'
        //:
        //:   o check that less than 'alignment' padding bytes were added
        //:
        //:   o call 'u::checkBlob' to verify that:
        //:     1 bytes prior to the padding match 'otherChar'
        //:
        //:     2 padded bytes match 'fillChar'
        //:
        //: 3 Write 2 nested loops to call 'padToAlignment' with a variety of
        //:   two char values: 'otherChar', to which all bytes in the blob
        //:   prior to the 'padToAlignment' call are initialized, and
        //:   'fillChar', either the value passed to the 'fillChar' argument,
        //:   or '\0' in the case where that argument is to be allowed to
        //:   default.  After the call, 'u::checkBlob' is called to verify that
        //:   all the bytes in the blob have their expeted values.
        //:
        //: 4 Write a table-driven test.
        //:
        //: 5 Do negative testing to verify that asserts catch all the
        //:   undefined behavior in the contract.
        //
        // Testing:
        //   padToAlignment(Blob *, int, char = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ALIGNTO\n"
                             "===============\n";

        if (verbose) cout << "Test with wide variety of blob buffer sizes,"
                             " alignment values, and previous blob sizes.\n";
        {
            enum { k_MAX_ALIGNMENT = 64,
                   k_INIT_BUF_SIZE = 4 + 2 * k_MAX_ALIGNMENT };
            char initBuf[k_INIT_BUF_SIZE];

            const char fillChar = 'a', otherChar = 'z';

            bsl::memset(initBuf, otherChar, sizeof(initBuf));

            // Here we iterate 3 nested loops to vary 3 variables:
            //: o 'blobBufferSize' -- the size of the blob buffers.  Since the
            //:   blob buffer factory we are using is
            //:   'SimpleBlobBufferFactory', all the blob buffers are the same
            //:   size (so a blob buffer size of 0 would make no sense).
            //:
            //: o 'alignment' -- must be a power of two.  We vary this over the
            //:   range of ALL acceptable values.
            //:
            //: o 'prevBlobSize' -- the size of the blob before it is padded.

            const unsigned blobBufferSizes[] = { 1, 2, 3, 4, 7, 8, 9, 12 };
            enum { k_NUM_BLOB_BUFFER_SIZES = sizeof blobBufferSizes /
                                                     sizeof *blobBufferSizes };

            for (int ii = 0; ii < k_NUM_BLOB_BUFFER_SIZES; ++ii) {
                const int blobBufferSize = blobBufferSizes[ii];

                for (int alignment = 1; alignment <= k_MAX_ALIGNMENT;
                                                             alignment <<= 1) {
                    const int maxPrefBlobSize = 4 + 2 * alignment;

                    for (int prevBlobSize = 0; prevBlobSize <= maxPrefBlobSize;
                                                              ++prevBlobSize) {
                        ASSERT(prevBlobSize <= k_INIT_BUF_SIZE);

                        bslma::TestAllocator           bbfAllocator("bbf");
                        bdlbb::SimpleBlobBufferFactory bbf(blobBufferSize,
                                                           &bbfAllocator);
                        bslma::TestAllocator           dstAllocator("dst");
                        bdlbb::Blob                    dst(&bbf,
                                                           &dstAllocator);

                        bdlbb::BlobUtil::append(&dst, initBuf, prevBlobSize);

                        bdlbb::BlobUtil::padToAlignment(&dst,
                                                        alignment,
                                                        fillChar);

                        int guessLen = 0;
                        while (guessLen < prevBlobSize) {
                            guessLen += alignment;
                        }
                        ASSERT(dst.length() == guessLen);

                        ASSERT(0 == (dst.length() & (alignment - 1)));
                        ASSERT(prevBlobSize <= dst.length());
                        ASSERT(dst.length() - prevBlobSize < alignment);

                        u::checkBlob(dst,
                                     blobBufferSize,
                                     prevBlobSize,
                                     otherChar,
                                     fillChar);
                    }
                }
            }
        }

        if (verbose) cout << "Test with a variety of fill chars passed, and"
                             " with fill char defaulting.\n";
        {
            enum { k_BLOB_BUFFER_SIZE = 3, k_PREV_SIZE = 5, k_ALIGNMENT = 8 };

            char otherCharBuf[k_PREV_SIZE];

            const char chars[] = { '\0', 'A', 'P', '*', '6', '/' };
            enum { k_NUM_CHARS = sizeof chars / sizeof *chars };

            for (int ii = 0; ii < k_NUM_CHARS; ++ii) {
                const char otherChar = chars[ii];

                bsl::memset(otherCharBuf, otherChar, sizeof(otherCharBuf));

                for (int jj = -1; jj < k_NUM_CHARS; ++jj) {
                    const bool passChar = -1 != jj;
                    const char fillChar = passChar ? chars[jj] : '\0';

                    bslma::TestAllocator           bbfAllocator("bbf");
                    bdlbb::SimpleBlobBufferFactory bbf(k_BLOB_BUFFER_SIZE,
                                                       &bbfAllocator);
                    bslma::TestAllocator           dstAllocator("dst");
                    bdlbb::Blob                    dst(&bbf,
                                                       &dstAllocator);

                    bdlbb::BlobUtil::append(&dst, otherCharBuf, k_PREV_SIZE);

                    if (passChar) {
                        bdlbb::BlobUtil::padToAlignment(&dst,
                                                        k_ALIGNMENT,
                                                        fillChar);
                    }
                    else {
                        bdlbb::BlobUtil::padToAlignment(&dst, k_ALIGNMENT);
                    }

                    ASSERT(k_ALIGNMENT == dst.length());

                    u::checkBlob(dst,
                                 k_BLOB_BUFFER_SIZE,
                                 k_PREV_SIZE,
                                 otherChar,
                                 fillChar);
                }
            }
        }

        static const struct Data {
            int  d_line;
            char d_prevChar;
            char d_fillChar;
            int  d_blobBufferSize;
            int  d_prevBlobSize;
            int  d_alignment;
            int  d_expSize;
        } DATA[] = {
            { L_, 'a', 'z', 8,  0,  4,  0 },
            { L_, 'a', 'z', 8,  1,  4,  4 },
            { L_, 'a', 'z', 8,  2,  4,  4 },
            { L_, 'a', 'z', 8,  3,  4,  4 },
            { L_, 'a', 'z', 8,  4,  4,  4 },

            { L_, 'a', 'z', 8,  0,  8,  0 },
            { L_, 'a', 'z', 8,  1,  8,  8 },
            { L_, 'a', 'z', 8,  2,  8,  8 },
            { L_, '+', '*', 8,  3,  8,  8 },
            { L_, '+', '*', 8,  4,  8,  8 },
            { L_, '+', '*', 8,  5,  8,  8 },
            { L_, '+', '*', 8,  6,  8,  8 },
            { L_, '+', '*', 8,  7,  8,  8 },
            { L_, 'F', 'W', 8,  8,  8,  8 },
            { L_, 'F', 'W', 8,  9,  8, 16 },
            { L_, 'F', 'W', 8, 10,  8, 16 },
            { L_, 'F', 'W', 8, 11,  8, 16 },
            { L_, 'F', 'W', 8, 12,  8, 16 },
            { L_, '8', '.', 8, 13,  8, 16 },
            { L_, '8', '.', 8, 14,  8, 16 },
            { L_, '8', '.', 8, 15,  8, 16 },
            { L_, '8', '.', 8, 16,  8, 16 },

            { L_, 'a', 'z', 3,  0,  4,  0 },
            { L_, 'a', 'z', 3,  1,  4,  4 },
            { L_, 'a', 'z', 3,  2,  4,  4 },
            { L_, 'a', 'z', 3,  3,  4,  4 },
            { L_, 'a', 'z', 3,  4,  4,  4 },

            { L_, 'a', 'z', 3,  0,  8,  0 },
            { L_, 'a', 'z', 3,  1,  8,  8 },
            { L_, 'a', 'z', 3,  2,  8,  8 },
            { L_, '+', '*', 3,  3,  8,  8 },
            { L_, '+', '*', 3,  4,  8,  8 },
            { L_, '+', '*', 3,  5,  8,  8 },
            { L_, '+', '*', 3,  6,  8,  8 },
            { L_, '+', '*', 3,  7,  8,  8 },
            { L_, 'F', 'W', 3,  8,  8,  8 },
            { L_, 'F', 'W', 3,  9,  8, 16 },
            { L_, 'F', 'W', 3, 10,  8, 16 },
            { L_, 'F', 'W', 3, 11,  8, 16 },
            { L_, 'F', 'W', 3, 12,  8, 16 },
            { L_, '8', '.', 3, 13,  8, 16 },
            { L_, '8', '.', 3, 14,  8, 16 },
            { L_, '8', '.', 3, 15,  8, 16 },
            { L_, '8', '.', 3, 16,  8, 16 } };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "Table-Driven Testing\n";
        {
            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const Data& data             = DATA[ti];
                const int   LINE             = data.d_line;
                const char  PREV_CHAR        = data.d_prevChar;
                const char  FILL_CHAR_RAW    = data.d_fillChar;
                const int   BLOB_BUFFER_SIZE = data.d_blobBufferSize;
                const int   PREV_BLOB_SIZE   = data.d_prevBlobSize;
                const int   ALIGNMENT        = data.d_alignment;
                const int   EXP_SIZE         = data.d_expSize;

                for (int tj = 0; tj < 2; ++tj) {
                    const bool PASS_CHAR = tj;
                    const char FILL_CHAR = PASS_CHAR ? FILL_CHAR_RAW : '\0';

                    static char prevCharBuf[1024];
                    bsl::memset(prevCharBuf, PREV_CHAR, PREV_BLOB_SIZE);

                    bslma::TestAllocator           bbfAllocator("bbf");
                    bdlbb::SimpleBlobBufferFactory bbf(BLOB_BUFFER_SIZE,
                                                       &bbfAllocator);
                    bslma::TestAllocator           dstAllocator("dst");
                    bdlbb::Blob                    dst(&bbf, &dstAllocator);

                    bdlbb::BlobUtil::append(&dst, prevCharBuf, PREV_BLOB_SIZE);

                    if (PASS_CHAR) {
                        bdlbb::BlobUtil::padToAlignment(&dst,
                                                        ALIGNMENT,
                                                        FILL_CHAR);
                    }
                    else {
                        bdlbb::BlobUtil::padToAlignment(&dst, ALIGNMENT);
                    }

                    ASSERTV(LINE, 0 == (dst.length() & (ALIGNMENT - 1)));
                    ASSERT(PREV_BLOB_SIZE <= dst.length());
                    ASSERTV(LINE, dst.length() - PREV_BLOB_SIZE < ALIGNMENT);
                    ASSERTV(LINE, EXP_SIZE, dst.length(),
                                                     EXP_SIZE == dst.length());

                    u::checkBlob(dst,
                                 BLOB_BUFFER_SIZE,
                                 PREV_BLOB_SIZE,
                                 PREV_CHAR,
                                 FILL_CHAR);
                }
            }
        }

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;
            const int ALIGNMENT        = 4;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bdlbb::BlobUtil::padToAlignment(&blob, ALIGNMENT);

            ASSERTV(blob.length(),         0 == blob.length()        );
            ASSERTV(blob.numDataBuffers(), 1 == blob.numDataBuffers());

            blob.setLength(1);

            ASSERTV(blob.length(),         1 == blob.length()        );
            ASSERTV(blob.numDataBuffers(), 2 == blob.numDataBuffers());

            bdlbb::BlobUtil::padToAlignment(&blob, ALIGNMENT);

            ASSERTV(blob.length(),         4 == blob.length()        );
            ASSERTV(blob.numDataBuffers(), 3 == blob.numDataBuffers());
        }

        if (verbose) cout << "Negative testing\n";
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator           bbfAllocator("bbf");
            bdlbb::SimpleBlobBufferFactory bbf(7, &bbfAllocator);

            bslma::TestAllocator           dstAllocator("dst");
            bdlbb::Blob                    dst(&bbf, &dstAllocator);

            ASSERT_PASS(bdlbb::BlobUtil::padToAlignment(&dst, 2));
            ASSERT_FAIL(bdlbb::BlobUtil::padToAlignment(&dst, 128));
            ASSERT_FAIL(bdlbb::BlobUtil::padToAlignment(&dst, -2));
            ASSERT_FAIL(bdlbb::BlobUtil::padToAlignment(&dst, INT_MIN));
            ASSERT_PASS(bdlbb::BlobUtil::padToAlignment(&dst, 64));
            ASSERT_FAIL(bdlbb::BlobUtil::padToAlignment(&dst, 3));
            ASSERT_FAIL(bdlbb::BlobUtil::padToAlignment(0,    64));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FIX TO DRQS 144543867
        //
        // Concerns:
        //: 1 That the bug outlined in DRQS 144543867 has been fixed.  The
        //:   problem was that the call to 'reserveBufferCapacity' was, under
        //:   some circumstances, reserving excessive, unneeded capacity.
        //
        // Plan:
        //: 1 Create 'blob' with many buffer, and then use 'append' to copy a
        //:   very small part of it to a second blob, and observe that only a
        //:   small amount of memory is allocated by the 'vector' in the new
        //:   blob.
        //:
        //: 2 Also do the case where the destination blob has some data but a
        //:   bunch of unused buffers at the end.
        //
        // Testing:
        //   CONCERN: append doesn't do excessive 'reserveBufferCapacity'.
        // --------------------------------------------------------------------

        const int dataSize   = 40000000;
        const int bufferSize = 256;
        const int sliceSize  = 50000;
        const int shortSize  = sliceSize / 4;

        bslma::TestAllocator           bbfAllocator("bbf");
        bdlbb::SimpleBlobBufferFactory bbf(bufferSize, &bbfAllocator);

        bslma::TestAllocator           srcAllocator("src");
        bdlbb::Blob                    src(&bbf, &srcAllocator);

        src.setLength(dataSize);

        if (verbose) {
            cout << "After filling Blob with 40MB of data:\n";
            P(bbfAllocator.numBytesInUse());
            P(srcAllocator.numBytesInUse());
            P(src.numBuffers());
            cout << endl;
        }

        bslma::TestAllocator dstAllocator("dst");
        bdlbb::Blob          dst(&dstAllocator);

        bdlbb::BlobUtil::append(&dst, src, 0, sliceSize);
        ASSERTV(dstAllocator.numBytesInUse(), srcAllocator.numBytesInUse(),
                dstAllocator.numBytesInUse() * 100 <
                                                 srcAllocator.numBytesInUse());
        ASSERTV(dst.length() == sliceSize);
        const Int64 initialDstBytes = dstAllocator.numBytesInUse();

        if (verbose) {
            cout << "After test:\n";
            P(bbfAllocator.numBytesInUse());
            P(srcAllocator.numBytesInUse());
            P(src.numBuffers());
            P(dstAllocator.numBytesInUse());
            P(dst.numBuffers());
            cout << endl;
        }

        dst.setLength(shortSize);
        bdlbb::BlobUtil::append(&dst, src, 0, sliceSize);
        ASSERTV(dst.length(), sliceSize + shortSize,
                                        dst.length() == sliceSize + shortSize);
        ASSERTV(dstAllocator.numBytesInUse(), initialDstBytes,
                               initialDstBytes < dstAllocator.numBytesInUse());
        ASSERTV(dstAllocator.numBytesInUse(), initialDstBytes,
                           dstAllocator.numBytesInUse() < 3 * initialDstBytes);
      } break;
      case 10: {
        // -------------------------------------------------------------------
        // TESTING 'copy' FUNCTIONS WRITING TO BLOB
        //
        // Concerns:
        //: 1 Copying to blob works as expected for different argument values.
        //:
        //: 2 Writing within a BlobBuffer or to multiple BlobBuffers succeeds.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a table specifying various values of blob length, offset,
        //:   and number of bytes to copy.
        //:
        //: 2 For each value in the table create a blob having the length as
        //:   specified in the table entry.
        //:
        //: 3 Invoke the 'copy' method on that blob using as arguments a
        //:   string buffer or another blob, and the offset and number of
        //:   bytes values specified in the table entry.
        //:
        //: 4 Create an alternate blob with the expected output and confirm
        //:   that the two blob compare equal.
        //:
        //: 5 Repeat steps 2 to 4 for the same table entry by creating a blob
        //:   object having different buffer sizes.
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   void copy(dstBlob, int, const char *, int);
        //   void copy(dstBlob, int, srcBlob, int, int);
        // --------------------------------------------------------------------

        verbose && (cout << "\nTesting 'copy' functions writing to blob"
                            "\n========================================\n");

        const bsl::string STR = "abcdefghijklmnopqrstuvwxyz";

        if (verbose) bsl::cout << "\tCopy from character buffer" << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_blobLength;
                int d_dstOffset;
                int d_numBytesToCopy;
            } DATA[] = {
            //  LINE  BLOB_LEN    OFFSET    NUM_BYTES
            //  ----  --------    ------    ---------

            // NUM BYTES = 0

            {   L_,        0,        0,            0 },

            {   L_,        1,        0,            0 },
            {   L_,        3,        0,            0 },
            {   L_,       10,        0,            0 },
            {   L_,       25,        0,            0 },

            // Copy data at OFFSET = 0

            {   L_,        1,        0,            1 },

            {   L_,        3,        0,            1 },
            {   L_,        3,        0,            2 },
            {   L_,        3,        0,            3 },

            {   L_,       10,        0,            1 },
            {   L_,       10,        0,            5 },
            {   L_,       10,        0,           10 },

            {   L_,       25,        0,            1 },
            {   L_,       25,        0,           12 },
            {   L_,       25,        0,           25 },

            // Copy data at OFFSET = BLOB_LEN/2

            {   L_,        3,        1,            1 },
            {   L_,        3,        1,            2 },

            {   L_,       10,        5,            1 },
            {   L_,       10,        5,            3 },
            {   L_,       10,        5,            5 },

            {   L_,       25,       12,            1 },
            {   L_,       25,       12,            7 },
            {   L_,       25,       12,           13 },

            // Copy data at OFFSET = BLOB_LEN - 1

            {   L_,        3,        2,            1 },
            {   L_,       10,        9,            1 },
            {   L_,       25,       24,            1 },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const int   LEN  = DATA[i].d_blobLength;
                const int   OFF  = DATA[i].d_dstOffset;
                const int   NB   = DATA[i].d_numBytesToCopy;

                if (veryVerbose) {
                    P_(LINE) P_(LEN) P_(OFF) P(NB)
                }

                for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
                    bsl::string initialStr(LEN, 'Z');
                    bsl::string expStr(LEN, 'Z');
                    expStr.replace(expStr.begin() + OFF,
                                   expStr.begin() + OFF + NB,
                                   STR.begin() + 0,
                                   STR.begin() + NB);

                    bdlbb::SimpleBlobBufferFactory factory(bufferSize);

                    Blob exp(&factory);
                    Blob source(&factory);

                    copyStringToBlob(&source, initialStr);
                    copyStringToBlob(&exp, expStr);

                    if (veryVeryVerbose) {
                        bsl::string tmp;
                        copyBlobToString(&tmp, source);
                        bsl::cout << "\nSource = '" << tmp << '\''
                                  << bsl::endl;
                        copyBlobToString(&tmp, exp);
                        bsl::cout << "\nExp = '" << tmp << '\''
                                  << bsl::endl;
                    }

                    bsl::string src = STR;

                    Util::copy(&source, OFF, src.c_str(), NB);

                    LOOP4_ASSERT(LINE, OFF, LEN, NB,
                                 !Util::compare(exp, source));

                    if (veryVeryVerbose) {
                        bsl::string tmp;
                        copyBlobToString(&tmp, source);
                        bsl::cout << "\nSource = '" << tmp << '\''
                                  << bsl::endl;
                        copyBlobToString(&tmp, exp);
                        bsl::cout << "\nExp = '" << tmp << '\''
                                  << bsl::endl;
                    }
                }
            }

            if (verbose) cout << "\nNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;

                bdlbb::SimpleBlobBufferFactory  factory(10);
                const char        *src = "abcdef";

                if (veryVerbose) cout << "\tBad Blob pointer" << endl;
                {
                    Blob blob(&factory);
                    blob.setLength(3);

                    ASSERT_FAIL(Util::copy(0, 0, src, 1));
                    ASSERT_PASS(Util::copy(&blob, 0, src, 1));
                }

                if (veryVerbose) cout << "\tBad src pointer" << endl;
                {
                    Blob blob(&factory);
                    blob.setLength(3);

                    ASSERT_FAIL(Util::copy(&blob, 0, 0, 1));
                    ASSERT_PASS(Util::copy(&blob, 0, src, 1));
                }

                if (veryVerbose) cout << "\tBad dstOffset" << endl;
                {
                    Blob blob(&factory);

                    blob.setLength(3);

                    // Fails '0 <= dstOffset'

                    ASSERT_FAIL(Util::copy(&blob, -1, src, 0));
                    ASSERT_PASS(Util::copy(&blob,  0, src, 0));

                    ASSERT_PASS(Util::copy(&blob,  0, src, 1));

                    // Fails 'dstOffset + length <= dst->length()'

                    ASSERT_PASS(Util::copy(&blob,  2, src, 1));
                    ASSERT_FAIL(Util::copy(&blob,  2, src, 2));
                }

                if (veryVerbose) cout << "\tBad length" << endl;
                {
                    Blob blob(&factory);
                    blob.setLength(3);

                    ASSERT_FAIL(Util::copy(&blob, 0, src, -1));
                    ASSERT_PASS(Util::copy(&blob, 0, src, 0));
                }
            }
        }

        if (verbose) bsl::cout << "\tCopy from from another blob" << bsl::endl;
        {
            static const struct {
                int d_line;
                int d_dstLength;
                int d_dstOffset;
                int d_srcLength;
                int d_srcOffset;
                int d_numBytes;
            } DATA[] = {
            //  LINE    DL    DO    SL    SO     NB
            //  ----    --    --    --    --     --

            // NUM BYTES = 0

            {   L_,     0,    0,    0,    0,      0 },
            {   L_,     0,    0,    1,    0,      0 },
            {   L_,     1,    0,    0,    0,      0 },
            {   L_,     1,    0,    1,    0,      0 },

            {   L_,     3,    0,    1,    0,      0 },
            {   L_,     3,    0,    5,    0,      0 },
            {   L_,     3,    0,   10,    0,      0 },

            {   L_,    10,    0,    1,    0,      0 },
            {   L_,    10,    0,   10,    0,      0 },
            {   L_,    10,    0,   20,    0,      0 },

            {   L_,    25,    0,    1,    0,      0 },
            {   L_,    25,    0,    5,    0,      0 },
            {   L_,    25,    0,   10,    0,      0 },

            // Copy data at DEST & SRC OFFSET = 0

            {   L_,     1,    0,    1,    0,      1 },

            {   L_,     3,    0,    5,    0,      1 },
            {   L_,     3,    0,    5,    0,      2 },
            {   L_,     3,    0,    5,    0,      3 },

            {   L_,    10,    0,   10,    0,      1 },
            {   L_,    10,    0,   10,    0,      5 },
            {   L_,    10,    0,   10,    0,     10 },

            {   L_,    25,    0,   15,    0,      1 },
            {   L_,    25,    0,   15,    0,      8 },
            {   L_,    25,    0,   15,    0,     15 },

            // Copy data at DEST OFFSET = 0 & SRC OFFSET = SLEN/2

            {   L_,     3,    0,    5,    2,      1 },
            {   L_,     3,    0,    5,    2,      2 },
            {   L_,     3,    0,    5,    2,      3 },

            {   L_,    10,    0,   10,    5,      1 },
            {   L_,    10,    0,   10,    5,      2 },
            {   L_,    10,    0,   10,    5,      3 },

            {   L_,    25,    0,   15,    7,      1 },
            {   L_,    25,    0,   15,    7,      3 },
            {   L_,    25,    0,   15,    7,      8 },

            // Copy data at DEST OFFSET = DLEN/2 & SRC OFFSET = 0

            {   L_,     3,    1,    5,    0,      1 },
            {   L_,     3,    1,    5,    0,      2 },

            {   L_,    10,    5,   10,    0,      1 },
            {   L_,    10,    5,   10,    0,      3 },
            {   L_,    10,    5,   10,    0,      5 },

            {   L_,    25,   12,   15,    0,      1 },
            {   L_,    25,   12,   15,    0,      8 },
            {   L_,    25,   12,   15,    0,     13 },

            // Copy data at DEST OFFSET & SRC OFFSET = LEN/2

            {   L_,     3,    1,    5,    2,      1 },
            {   L_,     3,    1,    5,    2,      2 },

            {   L_,    10,    5,   10,    5,      1 },
            {   L_,    10,    5,   10,    5,      3 },
            {   L_,    10,    5,   10,    5,      5 },

            {   L_,    25,   12,   15,    7,      1 },
            {   L_,    25,   12,   15,    7,      3 },
            {   L_,    25,   12,   15,    7,      8 },

            // Copy data from SRC OFFSET = SLEN - 1

            {   L_,     3,    0,    5,    4,      1 },
            {   L_,     3,    1,    5,    4,      1 },
            {   L_,     3,    2,    5,    4,      1 },

            {   L_,    10,    0,   10,    9,      1 },
            {   L_,    10,    5,   10,    9,      1 },
            {   L_,    10,    9,   10,    9,      1 },

            {   L_,    25,    0,   15,   14,      1 },
            {   L_,    25,   12,   15,   14,      1 },
            {   L_,    25,   24,   15,   14,      1 },

            // Copy data at DEST OFFSET = DLEN - 1

            {   L_,     3,    2,    5,    0,      1 },
            {   L_,     3,    2,    5,    2,      1 },
            {   L_,     3,    2,    5,    4,      1 },

            {   L_,    10,    9,   10,    0,      1 },
            {   L_,    10,    9,   10,    5,      1 },
            {   L_,    10,    9,   10,    9,      1 },

            {   L_,    25,   24,   15,    0,      1 },
            {   L_,    25,   24,   15,    7,      1 },
            {   L_,    25,   24,   15,   14,      1 },

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const int   DLEN = DATA[i].d_dstLength;
                const int   DOFF = DATA[i].d_dstOffset;
                const int   SLEN = DATA[i].d_srcLength;
                const int   SOFF = DATA[i].d_srcOffset;
                const int   NB   = DATA[i].d_numBytes;

                if (veryVerbose) {
                    P_(LINE) P_(DLEN) P_(DOFF) P_(SLEN) P_(SOFF) P(NB)
                }

                for (int dstSize = 1; dstSize < 10; ++dstSize) {
                    for (int srcSize = 1; srcSize < 10; ++srcSize) {
                        bsl::string dstInitialStr(DLEN, 'Z');
                        bsl::string srcInitialStr(STR, 0, SLEN);

                        bsl::string expStr(DLEN, 'Z');
                        expStr.replace(expStr.begin() + DOFF,
                                       expStr.begin() + DOFF + NB,
                                       STR.begin() + SOFF,
                                       STR.begin() + SOFF + NB);

                        bdlbb::SimpleBlobBufferFactory dstFactory(dstSize);
                        bdlbb::SimpleBlobBufferFactory srcFactory(srcSize);

                        Blob dst(&dstFactory);
                        Blob src(&srcFactory);
                        Blob exp(&srcFactory);

                        copyStringToBlob(&dst, dstInitialStr);
                        copyStringToBlob(&src, srcInitialStr);
                        copyStringToBlob(&exp, expStr);

                        if (veryVeryVerbose) {
                            bsl::string tmp;
                            copyBlobToString(&tmp, dst);
                            bsl::cout << "\nDst = '" << tmp << '\''
                                      << bsl::endl;
                            copyBlobToString(&tmp, src);
                            bsl::cout << "\nSrc = '" << tmp << '\''
                                      << bsl::endl;
                        }

                        Util::copy(&dst, DOFF, src, SOFF, NB);

                        LOOP4_ASSERT(LINE, DOFF, DLEN, NB,
                                     !Util::compare(exp, dst));

                        if (veryVeryVerbose) {
                            bsl::string tmp;
                            copyBlobToString(&tmp, dst);
                            bsl::cout << "\nDst = '" << tmp << '\''
                                      << bsl::endl;
                            copyBlobToString(&tmp, src);
                            bsl::cout << "\nSrc = '" << tmp << '\''
                                      << bsl::endl;
                        }
                    }
                }
            }

            if (verbose) cout << "\nNegative Testing." << endl;
            {
                bsls::AssertTestHandlerGuard hG;

                bdlbb::SimpleBlobBufferFactory factory(10);

                if (veryVerbose) cout << "\tBad blob pointer" << endl;
                {
                    Blob dst(&factory);
                    Blob src(&factory);
                    dst.setLength(3);
                    src.setLength(3);

                    ASSERT_FAIL(Util::copy(0, 0, src, 0, 1));
                    ASSERT_PASS(Util::copy(&dst, 0, src, 0, 1));
                }

                if (veryVerbose) cout << "\tBad dstOffset" << endl;
                {
                    Blob dst(&factory);
                    Blob src(&factory);

                    src.setLength(3);

                    // Fails '0 <= dstOffset'

                    ASSERT_FAIL(Util::copy(&dst, -1, src, 0, 0));
                    ASSERT_PASS(Util::copy(&dst,  0, src, 0, 0));

                    dst.setLength(3);

                    // Fails 'dstOffset + length <= dst->length()'

                    ASSERT_PASS(Util::copy(&dst,  2, src, 0, 1));
                    ASSERT_FAIL(Util::copy(&dst,  2, src, 0, 2));
                }

                if (veryVerbose) cout << "\tBad srcOffset" << endl;
                {
                    Blob dst(&factory);
                    Blob src(&factory);

                    dst.setLength(3);

                    // Fails '0 <= srcOffset'

                    ASSERT_FAIL(Util::copy(&dst, 0, src, -1, 0));
                    ASSERT_PASS(Util::copy(&dst, 0, src,  0, 0));

                    src.setLength(3);

                    // Fails 'srcOffset + length <= src->length()'

                    ASSERT_PASS(Util::copy(&dst,  0, src, 2, 1));
                    ASSERT_FAIL(Util::copy(&dst,  0, src, 2, 2));
                }

                if (veryVerbose) cout << "\tBad length" << endl;
                {
                    Blob dst(&factory);
                    Blob src(&factory);
                    dst.setLength(3);
                    src.setLength(3);

                    ASSERT_FAIL(Util::copy(&dst, 0, src, 0, -1));
                    ASSERT_PASS(Util::copy(&dst, 0, src, 0, 0));
                }
            }
        }

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int   BLOB_BUFFER_SIZE = 2;
            const char *data             = "0123";

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob1(&factory);
            bdlbb::Blob                    blob2(&factory);

            blob1.appendDataBuffer(bdlbb::BlobBuffer());
            blob2.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob1.length()        );
            ASSERTV(0 == blob2.length()        );
            ASSERTV(1 == blob1.numDataBuffers());
            ASSERTV(1 == blob2.numDataBuffers());

            bdlbb::BlobUtil::copy(&blob1, 0, blob2, 0, 0);

            ASSERTV(0 == blob1.length()        );
            ASSERTV(0 == blob2.length()        );
            ASSERTV(1 == blob1.numDataBuffers());
            ASSERTV(1 == blob2.numDataBuffers());

            bdlbb::BlobUtil::copy(&blob1, 0, data, 0);

            ASSERTV(0 == blob1.length()        );
            ASSERTV(1 == blob1.numDataBuffers());
        }
      } break;
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
        //       const bdlbb::Blob&, int position, int length, int alignment)
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

        bdlbb::SimpleBlobBufferFactory factory(BIG);
        bdlbb::BlobBuffer buffer;
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

        bdlbb::BlobBuffer buffers[4];
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
            bdlbb::Blob BLOB;
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

                            bsls::AssertFailureHandlerGuard guard(
                                                     &bsls::Assert::failThrow);
                            BSLS_ASSERTTEST_ASSERT_FAIL(
                                bdlbb::BlobUtil::getContiguousRangeOrCopy(
                                              copyBufP, BLOB, POS, SIZE, ALN));
                            continue;
                        }
                        veryVeryVerbose && (bsl::cout << "\n");

                        memset(copyBufP, '#', SIZE + 1);
                        char *out = 0;
                        BSLS_ASSERTTEST_ASSERT_PASS(out =
                                   bdlbb::BlobUtil::getContiguousRangeOrCopy(
                                              copyBufP, BLOB, POS, SIZE, ALN));
                        ASSERT(0 != out);
                        bsl::pair<int, int> place =
                          bdlbb::BlobUtil::findBufferIndexAndOffset(BLOB, POS);
                        const bdlbb::BlobBuffer& buf =
                            BLOB.buffer(place.first);
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
                            bdlbb::BlobUtil::copy(dumBuf, BLOB, POS, SIZE);
                            ASSERT(0 == memcmp(dumBuf, out, SIZE + 1));
                        }
                    }
                }
            }
        }

        bdlbb::Blob BLOB(&buffers[1], 1, &factory);
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

            bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failThrow);

            BSLS_ASSERTTEST_ASSERT_FAIL(
                                   bdlbb::BlobUtil::getContiguousRangeOrCopy(
                                                    BUF, BLOB, POS, LEN, ALN));
        }
        // One-off alignment is OK for alignment 1:
        BSLS_ASSERTTEST_ASSERT_PASS(
            bdlbb::BlobUtil::getContiguousRangeOrCopy(
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
        //  char *BlobUtil::getContiguousDataBuffer(bdlbb::Blob*, int length,
        //                                 bdlbb::BlobBufferFactory *factory)
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
        bdlbb::BlobBuffer buffers[3];
        factory.allocate(&buffers[0]);
        factory.allocate(&buffers[1]);
        factory.allocate(&buffers[2]);
        bdlbb::BlobBuffer emptyBuffer(buffers[0].buffer(), 0);

        // We have not tested bdlbb::BlobUtil::append yet, so cannot use it.

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

            bdlbb::Blob BLOB1;
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
            bdlbb::Blob BLOB5;
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
             bdlbb::BlobUtil::getContiguousDataBuffer(&BLOB1, 1, &factory7));
            BSLS_ASSERTTEST_ASSERT_PASS(p5 =
             bdlbb::BlobUtil::getContiguousDataBuffer(&BLOB5, 5, &factory7));

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
                        bdlbb::BlobUtil::findBufferIndexAndOffset(BLOB5, ante);
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
                    bdlbb::BlobUtil::findBufferIndexAndOffset(BLOB5, ante - 1);
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

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bdlbb::BlobUtil::getContiguousDataBuffer(&blob, 1, &factory);

            ASSERTV(1 == blob.length()        );
            ASSERTV(2 == blob.numDataBuffers());
        }

        if (verbose) cout << "Negative Testing\n";
        {
            bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failThrow);

            bdlbb::Blob BLOB;

            ASSERT_FAIL(
                     bdlbb::BlobUtil::getContiguousDataBuffer(0, 1, &factory));
            ASSERT_FAIL(
                bdlbb::BlobUtil::getContiguousDataBuffer(&BLOB, -1, &factory));
            ASSERT_FAIL(bdlbb::BlobUtil::getContiguousDataBuffer(&BLOB, 1, 0));
            ASSERT_FAIL(
                bdlbb::BlobUtil::getContiguousDataBuffer(&BLOB, 10, &factory));
        }

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
        //   void BlobUtil::copy(char* dstBuffer, const bdlbb::Blob& srcBlob,
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

        bdlbb::SimpleBlobBufferFactory factory(5);
        bdlbb::BlobBuffer              buffers[3];
        factory.allocate(&buffers[0]);
        factory.allocate(&buffers[1]);
        factory.allocate(&buffers[2]);
        bdlbb::BlobBuffer emptyBuffer(buffers[0].buffer(), 0);
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

            bdlbb::Blob BLOB;
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
                                  bdlbb::BlobUtil::copy(tstBuf, BLOB, j, k));

                    } else if (k) {

                        BSLS_ASSERTTEST_ASSERT_PASS(
                                  bdlbb::BlobUtil::copy(tstBuf, BLOB, j, k));

                        ASSERT(bsl::memcmp(refBuf, tstBuf, k) == 0);
                        ASSERT(tstBuf[k] == 'X');
                    }
                }
            }
        }

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE    = 2;
            const int BUFFER_SIZE         = 2;
            char      buffer[BUFFER_SIZE] = {'0', '1'};

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bdlbb::BlobUtil::copy(buffer, blob, 0, 0);

            ASSERTV(0 == strncmp(buffer, "01", BUFFER_SIZE));
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
        //                                  const bdlbb::Blob&, int position)
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

        bdlbb::SimpleBlobBufferFactory factory(5);
        bdlbb::BlobBuffer buffer;
        factory.allocate(&buffer);
        bdlbb::BlobBuffer emptyBuffer(buffer.buffer(), 0);

        int NUMDATA = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUMDATA; ++i) {
            int NUMBUFS = DATA[i].d_numBufs;
            const int *SIZES = DATA[i].d_sizes;
            int POS = DATA[i].d_position;
            int EXPECTINDEX = DATA[i].d_expectIndex;
            int EXPECTOFFSET = DATA[i].d_expectOffset;

            bdlbb::Blob BLOB;
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
                       bdlbb::BlobUtil::findBufferIndexAndOffset(BLOB, POS));

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
                bsls::AssertFailureHandlerGuard guard(
                                                     &bsls::Assert::failThrow);

                BSLS_ASSERTTEST_ASSERT_FAIL(PLACE =
                       bdlbb::BlobUtil::findBufferIndexAndOffset(BLOB, POS));

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
            bdlbb::SimpleBlobBufferFactory factory(bufferSize);

            for (int offset = 0; offset < BUF_SIZE; ++offset) {
                for (int length = 0; length <= BUF_SIZE - offset; ++length) {

                    bsl::string sstr = STR;
                    bsl::string estr = STR;
                    estr.erase(offset, length);

                    bdlbb::Blob exp(&factory);
                    bdlbb::Blob source(&factory);
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

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bdlbb::BlobUtil::erase(&blob, 0, 0);

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());
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
        bslma::DefaultAllocatorGuard dag(&allocator);
        ASSERT(&allocator == bslma::Default::defaultAllocator());

        for (int bufferSize =  k_MIN_BUFFER_SIZE;
                 bufferSize <= k_MAX_BUFFER_SIZE;
                 bufferSize += k_INC_BUFFER_SIZE)
        {
            bdlbb::SimpleBlobBufferFactory factory(bufferSize);

            for (int numBuffers =  k_MIN_NUM_BUFFERS;
                     numBuffers <= k_MAX_NUM_BUFFERS;
                     numBuffers += k_INC_NUM_BUFFERS)
            {
                if (verbose) {
                    bsl::cout << "[bufferSize = " << bufferSize << ", "
                              << "numBuffers = " << numBuffers << "]"
                              <<bsl::endl;
                }

                bdlbb::Blob blob(&factory);

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
                        ASSERT(&os == &bdlbb::BlobUtil::hexDump(os,
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

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bsl::stringstream os;
            const bsl::string BEFORE = os.str();
            ASSERT(&os == &bdlbb::BlobUtil::hexDump(os,
                                                    blob,
                                                    0,
                                                    0));

            ASSERTV(BEFORE == os.str());
        }

        ASSERT(0 <  allocator.numAllocations());
        ASSERT(0 == allocator.numBlocksInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HEXDUMP
        //
        // Concerns:
        //   For performance reasons, bdlbb::BlobUtil::hexDump uses a static
        //   array of length 32 to pass in the vector of <buffer, size> pairs
        //   to bdlb_print::hexDump.  When there are more than 32 buffers, the
        //   vector passed in is dynamically allocated to the correct size, and
        //   deallocated immediately after use.  This should test all boarder
        //   cases related to hex dumping of multiple buffers, including 0
        //   buffers, 1 buffer, 31,32,33, and 42 for good measure.
        //
        // Plan:
        //   Create a situation with
        //     (a) 0  buffers.  Verify correctness.
        //     (b) 1  buffers.  Verify correctness.
        //     (c) 31 buffers.  Verify correctness.
        //     (d) 32 buffers.  Verify correctness.
        //     (e) 33 buffers.  Verify correctness.
        //
        // Testing:
        //   static bsl::ostream& hexDump(stream, source);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'hexdump' Function"
                          << "\n==========================" << endl;

        typedef bsl::pair<const char*, int> BufferInfo;

        enum { k_BUF_SIZE = 2048 };

        bslma::TestAllocator         da("default", veryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            if (verbose) cout << "(a) 0 buffers" << endl;
            bdlbb::SimpleBlobBufferFactory factory(5);
            bdlbb::Blob                    myBlob(&factory);
            bdlbb::Blob                    modelBlob(&factory);

            ASSERT(0 == myBlob.numDataBuffers());

            // Testing overload with 2 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                const bsls::Types::Int64 NUM_BYTES_TOTAL = da.numBytesTotal();

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob));

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));
                ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                ASSERT(0 == strncmp(buf, expectedOut2Case3[0].c_str(),
                                    expectedOut2Case3[0].size()));

                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                              << output
                              << "Expected String  :\n"
                              << expectedOut2Case3[0] << bsl::endl;
                }
            }

            // Testing overload with 4 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                const bsls::Types::Int64 NUM_BYTES_TOTAL = da.numBytesTotal();

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob, 0, 0));

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));
                ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                ASSERT(0 == strncmp(buf,
                                    expectedOut2Case3[0].c_str(),
                                    expectedOut2Case3[0].size()));

                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                              << output
                              << "Expected String  :\n"
                              << expectedOut2Case3[0] << bsl::endl;
                }
            }
        }

        {
            if (verbose) cout << "(b) 1 buffers" << endl;
            bdlbb::SimpleBlobBufferFactory factory(1024);
            bdlbb::Blob                    myBlob(&factory);
            bdlbb::Blob                    modelBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob,    TEST_STR);
            copyStringToBlob(&modelBlob, TEST_STR);

            ASSERT(1 == myBlob.numDataBuffers());

            const int BLOB_LENGTH = myBlob.length();

            // Testing overload with 2 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                const bsls::Types::Int64 NUM_BYTES_TOTAL = da.numBytesTotal();

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob));

                ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob, myBlob));
                ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                ASSERT(0 == strncmp(buf, expectedOut2Case3[1].c_str(),
                                    expectedOut2Case3[1].size()));

                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                              << output
                              << "Expected String  :\n"
                              << expectedOut2Case3[1] << bsl::endl;
                }
            }

            // Testing overload with 4 parameters.
            {
                {
                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             0,
                                                             BLOB_LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(buf,
                                        expectedOut2Case3[1].c_str(),
                                        expectedOut2Case3[1].length()));
                }
                {
                    const int OFFSET = BLOB_LENGTH / 2;
                    const int LENGTH = BLOB_LENGTH / 4;

                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             OFFSET,
                                                             LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERTV(buf, 0 == strncmp(buf,
                                              expectedOut4Case3[1].c_str(),
                                              LENGTH));
                }
            }
        }

        {
            if (verbose) cout << "(c) 31 buffers" << endl;
            bdlbb::SimpleBlobBufferFactory factory(8);
            bdlbb::Blob                    myBlob(&factory);
            bdlbb::Blob                    modelBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob,    TEST_STR);
            copyStringToBlob(&modelBlob, TEST_STR);

            ASSERT(31 == myBlob.numDataBuffers());

            const int BLOB_LENGTH = myBlob.length();

            // Testing overload with 2 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob));

                ASSERTV(buf, 0 == strncmp(buf,
                                          expectedOut2Case3[2].c_str(),
                                          expectedOut2Case3[2].size()));
                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                        << output
                        << "Expected String  :\n"
                        << expectedOut2Case3[2] << bsl::endl;
                }
            }

            // Testing overload with 4 parameters.
            {
                {
                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             0,
                                                             BLOB_LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(buf,
                                        expectedOut2Case3[1].c_str(),
                                        expectedOut2Case3[1].length()));
                }
                {
                    const int OFFSET = BLOB_LENGTH / 2;
                    const int LENGTH = BLOB_LENGTH / 4;

                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             OFFSET,
                                                             LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(
                                      buf,
                                      expectedOut4Case3[2].c_str(),
                                      LENGTH));
                }
            }
        }

        {
            if (verbose) cout << "(d) 32 buffers" << endl;
            bdlbb::SimpleBlobBufferFactory factory(8);
            bdlbb::Blob                    myBlob(&factory);
            bdlbb::Blob                    modelBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32";

            copyStringToBlob(&myBlob,    TEST_STR);
            copyStringToBlob(&modelBlob, TEST_STR);

            ASSERT(32 == myBlob.numDataBuffers());

            const int BLOB_LENGTH = myBlob.length();

            // Testing overload with 2 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob));

                ASSERT(0 == strncmp(buf, expectedOut2Case3[3].c_str(),
                                    expectedOut2Case3[3].size()));

                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                              << output
                              << "Expected String  :\n"
                              << expectedOut2Case3[3] << bsl::endl;
                }
            }

            // Testing overload with 4 parameters.
            {
                {
                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             0,
                                                             BLOB_LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(buf,
                                        expectedOut2Case3[3].c_str(),
                                        expectedOut2Case3[3].length()));
                }
                {
                    const int OFFSET = BLOB_LENGTH / 2;
                    const int LENGTH = BLOB_LENGTH / 4;

                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             OFFSET,
                                                             LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(
                                      buf,
                                      expectedOut4Case3[3].c_str(),
                                      LENGTH));
                }
            }
        }

        {
            if (verbose) cout << "(e) 33 buffers" << endl;
            bdlbb::SimpleBlobBufferFactory factory(8);
            bdlbb::Blob                    myBlob(&factory);
            bdlbb::Blob                    modelBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32abcde 33";

            copyStringToBlob(&myBlob,    TEST_STR);
            copyStringToBlob(&modelBlob, TEST_STR);

            ASSERT(33 == myBlob.numDataBuffers());

            const int BLOB_LENGTH = myBlob.length();

            // Testing overload with 2 parameters.
            {
                char                        buf[k_BUF_SIZE];
                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);

                const bsls::Types::Int64 NUM_BYTES_TOTAL = da.numBytesTotal();
                const bsls::Types::Int64 EXP_BYTES_TOTAL =
                                     NUM_BYTES_TOTAL + 33 * sizeof(BufferInfo);

                // Action.
                ASSERT(&out == &bdlbb::BlobUtil::hexDump(out, myBlob));

                ASSERT(EXP_BYTES_TOTAL == da.numBytesTotal());

                ASSERT(0 == strncmp(buf, expectedOut2Case3[4].c_str(),
                                    expectedOut2Case3[4].size()));
                if (veryVerbose) {
                    bsl::string output(buf, obuf.length());
                    bsl::cout << "Hexdumped String :\n"
                              << output
                              << "Expected String  :\n"
                              << expectedOut2Case3[4] << bsl::endl;
                }
            }

            // Testing overload with 4 parameters.
            {
                {
                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                            da.numBytesTotal();
                    const bsls::Types::Int64 EXP_BYTES_TOTAL =
                                     NUM_BYTES_TOTAL + 33 * sizeof(BufferInfo);

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             0,
                                                             BLOB_LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(EXP_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(buf,
                                        expectedOut2Case3[4].c_str(),
                                        expectedOut2Case3[4].length()));
                }
                {
                    const int OFFSET = BLOB_LENGTH / 2;
                    const int LENGTH = BLOB_LENGTH / 4;

                    char                        buf[k_BUF_SIZE];
                    bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                    bsl::ostream                out(&obuf);

                    const bsls::Types::Int64 NUM_BYTES_TOTAL =
                                                        da.numBytesTotal();

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));

                    // Action.
                    ASSERT(&out == &bdlbb::BlobUtil::hexDump(out,
                                                             myBlob,
                                                             OFFSET,
                                                             LENGTH));

                    ASSERT(0 == bdlbb::BlobUtil::compare(modelBlob,
                                                         myBlob));
                    ASSERT(NUM_BYTES_TOTAL == da.numBytesTotal());
                    ASSERT(0 == strncmp(
                                      buf,
                                      expectedOut4Case3[4].c_str(),
                                      LENGTH));
                }
            }
        }

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            bsl::stringstream os;
            const bsl::string BEFORE = os.str();

            ASSERT(&os == &bdlbb::BlobUtil::hexDump(os, blob));

            ASSERTV(BEFORE == os.str());

            ASSERT(&os == &bdlbb::BlobUtil::hexDump(os, blob, 0, 0));

            ASSERTV(BEFORE == os.str());
        }

        if (verbose) cout << "Negative testing\n";
        {
            bsls::AssertTestHandlerGuard hG;

            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            ASSERTV(0 == blob.length());

            bsl::stringstream os;

            ASSERT_PASS(bdlbb::BlobUtil::hexDump(os, blob,  0,  0));
            ASSERT_FAIL(bdlbb::BlobUtil::hexDump(os, blob,  0, -1));
            ASSERT_FAIL(bdlbb::BlobUtil::hexDump(os, blob, -1,  0));

            ASSERT_FAIL(bdlbb::BlobUtil::hexDump(os, blob,  0,  1));
            ASSERT_FAIL(bdlbb::BlobUtil::hexDump(os, blob,  1,  0));
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
                    bdlbb::SimpleBlobBufferFactory fa(size1), fb(size2);

                    bdlbb::Blob mX(&fa); const bdlbb::Blob& X = mX;
                    bdlbb::Blob mY(&fb); const bdlbb::Blob& Y = mY;

                    mX.setLength(LLEN);
                    mY.setLength(RLEN);

                    copyStringToBlob(&mX, LHS);
                    copyStringToBlob(&mY, RHS);

                    if (veryVerbose) {
                        P_(LINE) P_(LHS) P_(RHS) P_(RV)
                        P_(size1) P(size2)
                    }

                    const int rv1 = bdlbb::BlobUtil::compare(X, Y);
                    const int rv2 = bdlbb::BlobUtil::compare(Y, X);
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

                bdlbb::SimpleBlobBufferFactory factory(bufferSize);
                bdlbb::SimpleBlobBufferFactory factory2(bufferSize2);

                // Test same
                bdlbb::Blob b1(&factory);
                bdlbb::Blob b2(&factory2);

                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 == bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 == bdlbb::BlobUtil::compare(b2, b1));

                // Test first char diff
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR2)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR2);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bdlbb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR2)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR2);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bdlbb::BlobUtil::compare(b2, b1));

                // Test last char diff
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR3)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR3);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bdlbb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR3)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR3);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bdlbb::BlobUtil::compare(b2, b1));

                // Test 1 char diff in length
                b1.setLength(static_cast<int>(strlen(TEST_STR)));
                b2.setLength(static_cast<int>(strlen(TEST_STR4)));

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR4);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bdlbb::BlobUtil::compare(b2, b1));

                b1.setLength(static_cast<int>(strlen(TEST_STR4)));
                b2.setLength(static_cast<int>(strlen(TEST_STR)));

                copyStringToBlob(&b1, TEST_STR4);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bdlbb::BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bdlbb::BlobUtil::compare(b2, b1));
            }
        }

        if (verbose) cout << "Testing 0-sized data buffers support\n";
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob1(&factory);
            bdlbb::Blob                    blob2(&factory);

            blob1.appendDataBuffer(bdlbb::BlobBuffer());
            blob2.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob1.length()        );
            ASSERTV(0 == blob2.length()        );
            ASSERTV(1 == blob1.numDataBuffers());
            ASSERTV(1 == blob2.numDataBuffers());

            ASSERT(0 == bdlbb::BlobUtil::compare(blob1, blob2));

            blob1.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob1.length()        );
            ASSERTV(2 == blob1.numDataBuffers());

            ASSERT(0 == bdlbb::BlobUtil::compare(blob1, blob2));
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
            bdlbb::Blob emptyBlob;
            bslx::TestOutStream blobStream(20150825);
            ASSERT(bdlbb::BlobUtil::write(blobStream, emptyBlob, 0, 0) == 0);
            ASSERT(blobStream.length() == 0);
        }

        // writing non-zero bytes from an empty blob
        {
            bdlbb::Blob emptyBlob;
            bslx::TestOutStream blobStream(20150825);
            ASSERT(bdlbb::BlobUtil::write(blobStream, emptyBlob, 0, 1) != 0);
            ASSERT(blobStream.length() == 0);
        }

        // writing zero bytes from a non-empty blob
        {
            // create a non-empty blob
            size_t size = 10;
            bdlbb::SimpleBlobBufferFactory blobFactory(static_cast<int>(size));
            bdlbb::Blob nonemptyBlob(&blobFactory);
            nonemptyBlob.setLength(static_cast<int>(size));

            // write blob
            bslx::TestOutStream blobStream(20150825);
            ASSERT(bdlbb::BlobUtil::write(blobStream, nonemptyBlob, 0, 0)
                   == 0);
            ASSERT(blobStream.length() == 0);
        }

        // writing non-zero bytes from a non-empty blob having a single 0-sized
        // buffer
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            // write blob
            bslx::TestOutStream blobStream(20210203);
            ASSERT(0 != bdlbb::BlobUtil::write(blobStream, blob, 0, 1));
            ASSERT(0 == blobStream.length());
        }

        // writing zero bytes from a non-empty blob having a single 0-sized
        // buffer
        {
            const int BLOB_BUFFER_SIZE = 2;

            bdlbb::SimpleBlobBufferFactory factory(BLOB_BUFFER_SIZE);
            bdlbb::Blob                    blob(&factory);

            blob.appendDataBuffer(bdlbb::BlobBuffer());

            ASSERTV(0 == blob.length()        );
            ASSERTV(1 == blob.numDataBuffers());

            // write blob
            bslx::TestOutStream blobStream(20210203);
            ASSERT(0 == bdlbb::BlobUtil::write(blobStream, blob, 0, 0));
            ASSERT(0 == blobStream.length());
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
            bdlbb::SimpleBlobBufferFactory factory(bufferSize);

            bdlbb::Blob dest1(&factory); // append(blob, blob);
            bdlbb::Blob dest2(&factory); // append(blob, const char*, int, int)
            bdlbb::Blob dest4(&factory); // append(blob, const char*, int)
            bdlbb::Blob source(&factory);

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
// Copyright 2018 Bloomberg Finance L.P.
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
