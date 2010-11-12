// bcema_blobutil.t.cpp            -*-C++-*-
#include <bcema_blobutil.h>
#include <bcema_blob.h>
#include <bcema_sharedptr.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bdex_byteoutstreamformatter.h>

#include <cstdlib>     // atoi()
#include <bsl_iostream.h>
#include <cstring>     // memcpy()
#include <bsl_strstream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace std;
using namespace bsl;  // automatically added by script


//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 3] Testing HexDump
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

//=============================================================================
//                            CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                          // =======================
                          // class BlobBufferFactory
                          // =======================

class BlobBufferFactory : public bcema_BlobBufferFactory {
    // TBD: doc

    // PRIVATE DATA MEMBERS
    int              d_size;
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    BlobBufferFactory(int              initialSize,
                      bslma_Allocator *basicAllocator = 0)
    : d_size(initialSize)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    virtual ~BlobBufferFactory()
    {
    }

    // MANIPULATORS
    virtual void allocate(bcema_BlobBuffer *buffer)
    {
        bcema_SharedPtr<char> shptr((char*)d_allocator_p->allocate(d_size),
                                    d_allocator_p);
        buffer->reset(shptr, d_size);
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

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bcema_BlobUtil Util;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

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
    enum { DATA_SIZE = sizeof DATA - 1 };

    int capacity = length;
    result->clear();
    result->reserve(capacity);
    do {
        int nbytes = (capacity >= DATA_SIZE) ? DATA_SIZE : capacity;
        result->append(DATA, nbytes);
        capacity -= nbytes;
    } while (capacity > 0);
    ASSERT(result->length() == length);
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

//=============================================================================
//                               HELPER FUNCTIONS
//-----------------------------------------------------------------------------

void copyStringToBlob(bcema_Blob *dest, const bsl::string& str)
{
    dest->setLength(str.length());
    int numBytesRemaining = str.length();
    const char *data = str.data();
    int bufferIndex = 0;
    while (numBytesRemaining) {
        bcema_BlobBuffer buffer = dest->buffer(bufferIndex);
        int numBytesToCopy = bsl::min(numBytesRemaining, buffer.size());
        std::memcpy(buffer.data(), data, numBytesToCopy);
        data += numBytesToCopy;
        numBytesRemaining -= numBytesToCopy;
        ++bufferIndex;
    }
    ASSERT(0 == numBytesRemaining);
}

void copyBlobToString(bsl::string *dest, const bcema_Blob& blob)
{
    dest->clear();
    int numBytesRemaining = blob.length();
    int bufferIndex = 0;
    while (numBytesRemaining) {
        bcema_BlobBuffer buffer = blob.buffer(bufferIndex);
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

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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

        const std::string STR  = "HelloWorld";
        const int         SIZE = STR.size();
        for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
            BlobBufferFactory factory(bufferSize);

            for (int offset = 0; offset < SIZE; ++offset) {
                for (int length = 0; length <= SIZE - offset; ++length) {

                    std::string sstr = STR;
                    std::string estr = STR;
                    estr.erase(offset, length);

                    bcema_Blob exp(&factory);
                    bcema_Blob source(&factory);
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
            DYNAMIC_ALLOCATION_THRESHOLD = 32,

            MIN_BUFFER_SIZE = 1,
            MAX_BUFFER_SIZE = 4,
            INC_BUFFER_SIZE = 1,

            MIN_NUM_BUFFERS = DYNAMIC_ALLOCATION_THRESHOLD - 4,
            MAX_NUM_BUFFERS = DYNAMIC_ALLOCATION_THRESHOLD + 4,
            INC_NUM_BUFFERS = 1
        };

        bslma_TestAllocator allocator;
        bslma_Default::setDefaultAllocatorRaw(&allocator);
        ASSERT(&allocator == bslma_Default::defaultAllocator());

        for (int bufferSize =  MIN_BUFFER_SIZE;
                 bufferSize <= MAX_BUFFER_SIZE;
                 bufferSize += INC_BUFFER_SIZE)
        {
            BlobBufferFactory factory(bufferSize);

            for (int numBuffers =  MIN_NUM_BUFFERS;
                     numBuffers <= MAX_NUM_BUFFERS;
                     numBuffers += INC_NUM_BUFFERS)
            {
                if (verbose) {
                    bsl::cout << "[bufferSize = " << bufferSize << ", "
                              << "numBuffers = " << numBuffers << "]"
                              <<bsl::endl;
                }

                bcema_Blob blob(&factory);

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
                        ASSERT(os == bcema_BlobUtil::hexDump(os, blob, offset,
                                                             length));

                        if (veryVeryVerbose) {
                            bsl::cout << os.str() << bsl::endl;
                        }
                    }
                }
            }
        }

        ASSERT(0 <  allocator.numAllocation());
        ASSERT(0 == allocator.numBlocksInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HEXDUMP
        //
        // Concerns:
        //   For performance reasons, bcema_BlobUtil::hexDump uses a static
        //   array of length 32 to pass in the the vector of <buffer, size>
        //   pairs to bdeu_print::hexDump.  When there are more than 32
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
        // static bsl::ostream& hexDump(bsl::ostream&     stream,
        //                              const bcema_Blob& source);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'hexdump' Function"
                          << "\n==========================" << endl;

        enum { SIZE = 2048 };

        {
            if (verbose) cout << "(a) 0 buffers" << endl;
            BlobBufferFactory factory(5);
            bcema_Blob        myBlob(&factory);

            ASSERT(0 == myBlob.numDataBuffers() );

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            ASSERT(out == bcema_BlobUtil::hexDump(out, myBlob));
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
            bcema_Blob        myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(1 == myBlob.numDataBuffers() );

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            ASSERT(out == bcema_BlobUtil::hexDump(out, myBlob));
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
            bcema_Blob        myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(31 == myBlob.numDataBuffers() );

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            ASSERT(out == bcema_BlobUtil::hexDump(out, myBlob));
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
            bcema_Blob myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(32 == myBlob.numDataBuffers() );

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            ASSERT(out == bcema_BlobUtil::hexDump(out, myBlob));
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
            bcema_Blob myBlob(&factory);

            const char *TEST_STR = "abcdef 1abcdef 2abcdef 3abcdef 4abcdef 5"
                "abcdef 6abcdef 7abcdef 8abcdef 9abcde 10abcde 11abcde 12"
                "abcde 13abcde 14abcde 15abcde 16abcde 17abcde 18abcde 19"
                "abcde 20abcde 21abcde 22abcde 23abcde 24abcde 25abcde 26"
                "abcde 27abcde 28abcde 29abcde 30abcde 31abcde 32abcde 33";

            copyStringToBlob(&myBlob, TEST_STR);
            ASSERT(33 == myBlob.numDataBuffers() );

            char buf[SIZE];  bsl::strstream out(buf, SIZE);
            ASSERT(out == bcema_BlobUtil::hexDump(out, myBlob));
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

        for (int bufferSize = 1; bufferSize < 10; ++bufferSize) {
            for (int bufferSize2 = 5; bufferSize2 < 20; ++bufferSize2) {
                const char *TEST_STR = "abcdefghijklmnopqrstuvwxyz";
                const char *TEST_STR2 = "Abcdefghijklmnopqrstuvwxyz";
                const char *TEST_STR3 = "abcdefghijklmnopqrstuvwxyZ";
                const char *TEST_STR4 = "abcdefghijklmnopqrstuvwxyz1";

                BlobBufferFactory factory(bufferSize);
                BlobBufferFactory factory2(bufferSize2);

                // Test same
                bcema_Blob b1(&factory);
                bcema_Blob b2(&factory2);

                b1.setLength(strlen(TEST_STR) + 1);
                b2.setLength(strlen(TEST_STR) + 1);

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 == bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 == bcema_BlobUtil::compare(b2, b1));

                // Test first char diff
                b1.setLength(strlen(TEST_STR) + 1);
                b2.setLength(strlen(TEST_STR2) + 1);

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR2);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bcema_BlobUtil::compare(b2, b1));

                b1.setLength(strlen(TEST_STR2) + 1);
                b2.setLength(strlen(TEST_STR) + 1);

                copyStringToBlob(&b1, TEST_STR2);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bcema_BlobUtil::compare(b2, b1));

                // Test last char diff
                b1.setLength(strlen(TEST_STR) + 1);
                b2.setLength(strlen(TEST_STR3) + 1);

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR3);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bcema_BlobUtil::compare(b2, b1));

                b1.setLength(strlen(TEST_STR3) + 1);
                b2.setLength(strlen(TEST_STR) + 1);

                copyStringToBlob(&b1, TEST_STR3);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bcema_BlobUtil::compare(b2, b1));

                // Test 1 char diff in length
                b1.setLength(strlen(TEST_STR) + 1);
                b2.setLength(strlen(TEST_STR4) + 1);

                copyStringToBlob(&b1, TEST_STR);
                copyStringToBlob(&b2, TEST_STR4);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 > bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 < bcema_BlobUtil::compare(b2, b1));

                b1.setLength(strlen(TEST_STR4) + 1);
                b2.setLength(strlen(TEST_STR) + 1);

                copyStringToBlob(&b1, TEST_STR4);
                copyStringToBlob(&b2, TEST_STR);

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 != bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize, bufferSize2,
                             0 < bcema_BlobUtil::compare(b1, b2));

                LOOP2_ASSERT(bufferSize2, bufferSize,
                             0 > bcema_BlobUtil::compare(b2, b1));
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

        if (verbose) cout << "\nTesting 'append' Function"
                          << "\n=========================" << endl;

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

            bcema_Blob dest1(&factory);  // append(blob, blob);
            bcema_Blob dest2(&factory);  // append(blob, const char*, int, int)
            bcema_Blob dest4(&factory);  // append(blob, const char*, int)
            bcema_Blob source(&factory);

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
                dest3.sputn(DEST, std::strlen(DEST));
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
                bdex_ByteOutStreamFormatter bosf(&dest3);
                Util::write(bosf, source, OFFSET, LENGTH);
                bosf.flush();
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
