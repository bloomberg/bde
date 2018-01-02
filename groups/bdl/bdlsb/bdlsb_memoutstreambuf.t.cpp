// bdlsb_memoutstreambuf.t.cpp                                        -*-C++-*-
#include <bdlsb_memoutstreambuf.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver exercises all the protected virtual methods from the
// 'basic_streambuf' protocol that are overridden by the derived concrete class
// 'bdlsb::MemOutStreamBuf', as well as each new (non-protocol) public method
// added in the 'bdlsb::MemOutStreamBuf' class.
//
// Our goal here is to ensure that the implementations comply exactly with the
// IOStreams portion of the C++ standard where the standard explicitly defines
// behavior, and that they conform to a feasible interpretation of the standard
// as described in the function documentation, where the standard defined
// behavior only loosely.  For those methods that are not protocol defined, we
// check only compliance with the behavior as described in the function
// documentation.
//
// The protected methods (whose implementation or re-implementation define the
// behavior that differentiates one kind of stream buffer from another) are
// "driven", or invoked, by various (base-class) public methods provided for
// stream-buffer client use.  One recurrent concern in this test driver is to
// ensure correct interplay between these protected methods and the base-
// class-provided implementations that use them.
//
// Note that output operator used for test tracing purposes is tested in test
// case 3.
//
// Primary Constructors:
//: o MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//: o void reserveCapacity(int numElements);
//: o int sputc (char c);
//
// Basic Accessors:
//: o const char_type *data();
//: o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
// [ 5] MemOutStreamBuf(int, bslma::Allocator *basicAllocator = 0);
// [ 2] ~MemOutStreamBuf();
// MANIPULATORS
// [ 2] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 7] pos_type seekoff(off_type, seekdir, openmode);
// [ 7] pos_type seekpos(pos_type, openmode);
// [ 6] streamsize xsputn(const char_type *s, streamsize length);
// [ 2] void reserveCapacity(int numElements);
// [ 8] void reset();
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS: ostream& operator<<(os&, const MemOutStreamBuf&);
// [ 2] PROXY: int sputc (char c);
// [ 6] PROXY: streamsize sputn(const char_type *s, streamsize count);
// [ 7] PROXY: pos_type pubseekoff(off_type, seekdir, openmode);
// [ 7] PROXY: pos_type pubseekpos(pos_type, openmode);
// [ 9] USAGE EXAMPLE
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlsb::MemOutStreamBuf Obj;

const size_t INIT_BUFSIZE           = 256;
const size_t TWICE_INIT_BUFSIZE     = INIT_BUFSIZE * 2;
const size_t INIT_BUFSIZE_PLUS_44   = INIT_BUFSIZE + 44;
                                               // arbitrary number in [1 .. 2k]

const size_t INIT_BUFSIZE_MINUS_ONE = INIT_BUFSIZE - 1;
const size_t INIT_BUFSIZE_PLUS_ONE  = INIT_BUFSIZE + 1;
const size_t TRIPLE_CAPACITY        = INIT_BUFSIZE * 3;
const size_t QUADRUPLE_CAPACITY     = INIT_BUFSIZE * 4;

BSLMF_ASSERT(sizeof(int) <= sizeof(size_t));

const size_t LARGE_CAPACITY =
                      static_cast<size_t>(bsl::numeric_limits<int>::max()) + 1;

const size_t MAX_CAPACITY   = bsl::numeric_limits<size_t>::max();

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                   // =====================================
                   // operator<< for bdlsb::MemOutStreamBuf
                   // =====================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdlsb::MemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdlsb::MemOutStreamBuf& streamBuffer)
{
    const bsl::streamsize  len  = streamBuffer.length();
    const char            *data = streamBuffer.data();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;

    for (int i = 0; i < len; ++i) {
        if (0 < i && 0 != i % 8) stream << ' ';

        if (0 == i % 8) { // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }
        for (int j = 7; j >= 0; --j) stream << !!((data[i] >> j) & 0x01);
    }

    stream.flags(flags); // reset stream format flags
    return stream;
}
size_t estimateCurrentCapacity(bdlsb::MemOutStreamBuf& streamBuffer);
    // Write a sequence of characters to the specified 'streamBuffer' until
    // the stream buffer relocates its internal buffer and return the length
    // of the stream buffer before the relocation.   Note that this function
    // alters the content of the stream buffer.

size_t estimateCurrentCapacity(bdlsb::MemOutStreamBuf& streamBuffer)
{
    const char* initialBuffer     = streamBuffer.data();
    size_t      estimatedCapacity = static_cast<size_t>(streamBuffer.length());

    while (initialBuffer == streamBuffer.data()) {
        estimatedCapacity = static_cast<size_t>(streamBuffer.length());
        streamBuffer.sputc('a');
    }
    return estimatedCapacity;
}

class LimitsTestAllocator : public bslma::Allocator {
  // This 'class' implements an allocator that can be used to test allocation
  // of amounts of memory that are infeasible to allocate on a system.  It does
  // this by having a small initial buffer that it allocates exclusively out of
  // however, all statistics are updated to match allocating arbitrary amounts
  // of memory.

    // DATA
    enum { k_BUFFER_SIZE = 1024 };        // the size of the internal buffer
                                          // used for real allocations
                        // Statistics

    size_t d_numAllocations;              // total number of allocation
                                          // requests on this object (including
                                          // those for 0 bytes)

    size_t d_numDeallocations;            // total number of deallocation
                                          // requests on this object (including
                                          // those supplying a 0 address)

    size_t d_numBlocksInUse;              // number of blocks currently
                                          // allocated from this object

    size_t d_numBytesInUse;               // number of bytes currently
                                          // allocated from this object

    size_t d_numBlocksMax;                // maximum number of blocks ever
                                          // allocated from this object at any
                                          // one time

    size_t d_numBytesMax;                 // maximum number of bytes ever
                                          // allocated from this object at any
                                          // one time

    size_t d_numBlocksTotal;              // cumulative number of blocks ever
                                          // allocated from this object

    size_t d_numBytesTotal;               // cumulative number of bytes ever
                                          // allocated from this object

    bool        d_verboseFlags;

    bsl::map <void *, size_t>             // a map of all requested allocations
                d_allocations;            // mapping claimed starting address
                                          // to the size of the request served

    char        d_buffer[k_BUFFER_SIZE];  // the buffer used for real
                                          // allocations

    void       *d_nextAllocation;         // the starting address used for the
                                          // next requested allocation

  private:
    // NOT IMPLEMENTED
    LimitsTestAllocator(const LimitsTestAllocator&);             // = delete
    LimitsTestAllocator& operator=(const LimitsTestAllocator&);  // = delete

  public:
    // CREATORS
    explicit
    LimitsTestAllocator(int verbose);
        // Create an instrumented "test" allocator, which does not allocate
        // memory.

    ~LimitsTestAllocator();
        // Destroy this allocator.

    // MANIPULATORS
    void *allocate(size_type size);
        // Return a newly-allocated block of memory of the specified 'size' (in
        // bytes).  If 'size' is 0, a null pointer is returned.  Otherwise,
        // invoke the 'allocate' method of the allocator supplied at
        // construction, increment the number of currently (and cumulatively)
        // allocated blocks, and increase the number of currently allocated
        // bytes by 'size'.  Update all other fields accordingly.

    void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect (other
        // than to record relevant statistics).  Otherwise, if the memory at
        // 'address' is consistent with being allocated from this test
        // allocator, decrement the number of currently allocated blocks, and
        // decrease the number of currently allocated bytes by the size (in
        // bytes) originally requested for the block.  Although technically
        // undefined behavior, if the memory can be determined not to have been
        // allocated from this test allocator, increment the number of
        // mismatches, and -- unless in quiet mode -- immediately report the
        // details of the mismatch to 'stdout' (e.g., as an 'bsl::hex' memory
        // dump) and abort.

    // ACCESSORS
    bsls::Types::Int64 numAllocations() const;
        // Return the cumulative number of allocation requests.  Note that this
        // number is incremented for every 'allocate' invocation.

    bsls::Types::Int64 numBlocksInUse() const;
        // Return the number of blocks currently allocated from this object.
        // Note that 'numBlocksInUse() <= numBlocksMax()'.

    bsls::Types::Int64 numBlocksMax() const;
        // Return the maximum number of blocks ever allocated from this object
        // at any one time.  Note that
        // 'numBlocksInUse() <= numBlocksMax() <= numBlocksTotal()'.

    bsls::Types::Int64 numBlocksTotal() const;
        // Return the cumulative number of blocks ever allocated from this
        // object.  Note that 'numBlocksMax() <= numBlocksTotal()'.

    bsls::Types::Int64 numBytesInUse() const;
        // Return the number of bytes currently allocated from this object.
        // Note that 'numBytesInUse() <= numBytesMax()'.

    bsls::Types::Int64 numBytesMax() const;
        // Return the maximum number of bytes ever allocated from this object
        // at any one time.  Note that
        // 'numBytesInUse() <= numBytesMax() <= numBytesTotal()'.

    bsls::Types::Int64 numBytesTotal() const;
        // Return the cumulative number of bytes ever allocated from this
        // object.  Note that 'numBytesMax() <= numBytesTotal()'.

    bsls::Types::Int64 numDeallocations() const;
        // Return the cumulative number of deallocation requests.  Note that
        // this number is incremented for every 'deallocate' invocation,
        // regardless of the validity of the request.
};
// CREATORS
LimitsTestAllocator::LimitsTestAllocator(int verbose)
: d_numAllocations(0),
  d_numDeallocations(0),
  d_numBlocksInUse(0),
  d_numBytesInUse(0),
  d_numBlocksMax(0),
  d_numBytesMax(0),
  d_numBlocksTotal(0),
  d_numBytesTotal(0),
  d_verboseFlags(verbose),
  d_nextAllocation(d_buffer)
{
}

LimitsTestAllocator::~LimitsTestAllocator()
{
    ASSERT(d_allocations.empty());
    ASSERT(d_numDeallocations == d_numAllocations);
    ASSERT(0                  == d_numBytesInUse);
}

void *LimitsTestAllocator::allocate(size_type size)
{
    void *address = 0;
    d_numAllocations += 1;

    address = d_nextAllocation;
    d_allocations[address] = size;

    d_numBlocksInUse += 1;
    d_numBlocksMax = bsl::max(d_numBlocksInUse, d_numBlocksMax);

    d_numBytesInUse  += size;

    d_numBytesMax  = bsl::max(d_numBytesInUse, d_numBytesTotal);

    d_numBlocksTotal += 1;
    d_numBytesTotal  += size;
    d_nextAllocation = static_cast<char *>(d_nextAllocation) + 1;
    return address;
}

void LimitsTestAllocator::deallocate(void *address)
{
    if (address) {
        d_numDeallocations += 1;
        d_numBlocksInUse   -= 1;
        // find the allocation
        bsl::map<void *, size_t>::iterator curr = d_allocations.find(address);

        ASSERT(curr != d_allocations.end());

        const size_t  allocation_size   = curr->second;

        // remove the allocation from the map
        d_allocations.erase(curr);

        // reset the state of the allocator
        d_numBytesInUse -= allocation_size;

        curr = d_allocations.end();
        if (curr != d_allocations.begin()) {
            curr--;
            d_nextAllocation = curr->first;
        }
    }
}

// ACCESSORS
inline
bsls::Types::Int64 LimitsTestAllocator::numAllocations() const
{
    return d_numAllocations;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBlocksInUse() const
{
    return d_numBlocksInUse;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBlocksMax() const
{
    return d_numBlocksMax;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBlocksTotal() const
{
    return d_numBlocksTotal;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBytesInUse() const
{
    return d_numBytesInUse;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBytesMax() const
{
    return d_numBytesMax;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numBytesTotal() const
{
    return d_numBytesTotal;
}

inline
bsls::Types::Int64 LimitsTestAllocator::numDeallocations() const
{
    return d_numDeallocations;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
/// Example 1: Basic Use of 'bdlsb::MemOutStreamBuf'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'bdlsb::MemOutStreamBuf' in order to test
// a user defined stream type, 'CapitalizingStream'.  In this example, we'll
// define a simple example stream type 'CapitalizingStream' that capitalizing
// lower-case ASCII data written to the stream.  In order to test this
// 'CapitalizingStream' type, we'll create an instance, and supply it a
// 'bdlsb::MemOutStreamBuf' object as its stream buffer; after we write some
// character data to the 'CapitalizingStream' we'll inspect the buffer of the
// 'bdlsb::MemOutStreamBuf' and verify its contents match our expected output.
// Note that to simplify the example, we do not include the functions for
// streaming non-character data, e.g., numeric values.
//
// First, we define our example stream class, 'CapitalizingStream' (which we
// will later test using 'bdlsb::MemOutStreamBuf):
//..
    class CapitalizingStream {
        // This class capitalizes lower-case ASCII characters that are output.

        // DATA
        bsl::streambuf  *d_streamBuffer_p;   // pointer to a stream buffer

        // FRIENDS
        friend CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                              const char          *data);
      public:
        // CREATORS
        explicit CapitalizingStream(bsl::streambuf *streamBuffer);
            // Create a capitalizing stream using the specified 'streamBuffer'
            // as underlying stream buffer to the stream.
    };

    // FREE OPERATORS
    CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                   const char          *data);
        // Write the specified 'data' in capitalized form to the specified
        // 'stream'.

    CapitalizingStream::CapitalizingStream(bsl::streambuf *streamBuffer)
    : d_streamBuffer_p(streamBuffer)
    {
    }
//..
// As is typical, the streaming operators are made friends of the class.
//
// Note that we cannot directly use 'bsl::toupper' to capitalize each
// individual character, because 'bsl::toupper' operates on 'int' instead of
// 'char'.  Instead, we call a function 'ucharToUpper' that works in terms of
// 'unsigned char'.  some care must be made to avoid undefined and
// implementation-specific behavior during the conversions to and from 'int'.
// Therefore we wrap 'bsl::toupper' in an interface that works in terms of
// 'unsigned char':
//..
    static unsigned char ucharToUpper(unsigned char input)
        // Return the upper-case equivalent to the specified 'input' character.
    {
        return static_cast<unsigned char>(bsl::toupper(input));
    }
//..
// Finally, we use the 'transform' algorithm to convert lower-case characters
// to upper-case.
//..
    // FREE OPERATORS
    CapitalizingStream& operator<<(CapitalizingStream&  stream,
                                   const char          *data)
    {
        bsl::string tmp(data);
        bsl::transform(tmp.begin(),
                       tmp.end(),
                       tmp.begin(),
                       ucharToUpper);
        stream.d_streamBuffer_p->sputn(tmp.data(), tmp.length());
        return stream;
    }
//..

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, and replace 'assert' with
        //:   'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
// Now, we create an instance of 'bdlsb::MemOutStreamBuf' that will serve as
// underlying stream buffer for our 'CapitalingStream':
//..
    bdlsb::MemOutStreamBuf streamBuffer;
//..
// Now, we test our 'CapitalingStream' by supplying the created instance of
// 'bdlsb::MemOutStreamBuf' and using it to inspect the output of the stream:
//..
    CapitalizingStream  testStream(&streamBuffer);
    testStream << "Hello world.";
//..
// Finally, we verify that the streamed data has been capitalized and placed
// into dynamically allocated buffer:
//..
    ASSERT(12 == streamBuffer.length());
    ASSERT(0  == bsl::strncmp("HELLO WORLD.",
                              streamBuffer.data(),
                              streamBuffer.length()));
//..
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // RESET TEST
        //
        // Concerns:
        //: 1 Calling 'reset' on a default-constructed streambuf has no effect.
        //:
        //: 2 Calling 'reset' for object with non-zero capacity deallocates
        //:   reserved memory and sets internal pointers to null.
        //:
        //: 3 The streambuf works normally after 'reset' invocation.
        //
        // Plan:
        //: 1 Create a test allocator.
        //:
        //: 2 Default-construct and reset a streambuf and verify that no
        //:   memory is allocated and that internal pointers are set to null.
        //:   (C-1,3)
        //:
        //: 3 Do some output after reset to verify object validity.  (C-4)
        //:
        //: 4 Construct a streambuf with some initial capacity and then reset.
        //:   Verify that allocated memory is returned and that internal
        //:   pointers are set to null.  (C-2..3)
        //:
        //: 5 Do some output after reset to verify object validity.  (C-4)
        //:
        //: 6 Construct a streambuf, write some text to it, and then reset.
        //:   Verify that allocated memory is returned and that internal
        //:   pointers are set to null.  (C-2..3)
        //:
        //: 7 Do some output after reset to verify object validity.  (C-4)
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESET TESTS" << endl
                                  << "===========" << endl;

        if (verbose) cout << "\nTesting reset for empty object." << endl;
        {
            bslma::TestAllocator     ta;
            Obj                      mSB(&ta);
            const Obj&               SB = mSB;
            const bsls::Types::Int64 ND = ta.numDeallocations();

            ASSERT(0 == ta.numBlocksTotal());
            ASSERT(0 == SB.data());

            mSB.reset();
            ASSERT(0      == ta.numBlocksTotal());
            ASSERT(ND + 1 == ta.numDeallocations());
            ASSERT(0      == SB.data());
            ASSERT(0      == SB.length());

            mSB.sputc('a');
            ASSERT(1        == SB.length());
            ASSERT(1        == ta.numBlocksInUse());
            ASSERT(0        != SB.data());
            ASSERT('a'      == SB.data()[0]);
        }

        if (verbose) cout << "\nTesting reset after initial capacity" << endl;
        {
            bslma::TestAllocator     ta;
            Obj                      mSB(INIT_BUFSIZE, &ta);
            const Obj&               SB = mSB;
            const bsls::Types::Int64 ND = ta.numDeallocations();

            ASSERT(0 != ta.numBlocksTotal());
            ASSERT(0 != SB.data());
            ASSERT(0 == SB.length());

            mSB.reset();
            ASSERT(0      == ta.numBlocksInUse());
            ASSERT(ND + 1 == ta.numDeallocations());
            ASSERT(0      == SB.data());
            ASSERT(0      == SB.length());

            mSB.sputc('a');
            ASSERT(1        == SB.length());
            ASSERT(1        == ta.numBlocksInUse());
            ASSERT(0        != SB.data());
            ASSERT('a'      == SB.data()[0]);
        }

        if (verbose) cout << "\nTesting reset after some output."  << endl;
        {
            bslma::TestAllocator ta;
            Obj                  mSB(INIT_BUFSIZE, &ta);
            const Obj&           SB = mSB;

            mSB.sputc('a');
            const bsls::Types::Int64 ND = ta.numDeallocations();
            ASSERT(0 != ta.numBlocksTotal());
            ASSERT(0 != SB.data());
            ASSERT(0 != SB.length());

            mSB.reset();
            ASSERT(0      == ta.numBlocksInUse());
            ASSERT(ND + 1 == ta.numDeallocations());
            ASSERT(0      == SB.data());
            ASSERT(0      == SB.length());

            mSB.sputc('a');
            ASSERT(1        == SB.length());
            ASSERT(1        == ta.numBlocksInUse());
            ASSERT(0        != SB.data());
            ASSERT('a'      == SB.data()[0]);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SEEK TESTS
        //   As the only action performed in 'seekpos' is the call for
        //   'seekoff' with predetermined second parameter, then we can test
        //   'seekpos' superficially.
        //   Note that 'seekoff' and 'seekpos' methods are called by base class
        //   methods 'pubseekoff' and 'pubseekpos'.
        //
        // Concerns:
        //: 1 Seeking uses the correct location from which to offset.
        //:
        //: 2 Both negative and positive offsets compute correctly.
        //:
        //: 3 Seeking sets the "cursor" (i.e., the base-class' pptr()) position
        //:   to the correct location.
        //:
        //: 4 Seeking out of bounds is handled correctly and returns invalid
        //:   value.
        //:
        //: 5 Trying to seek in the "get" area has no effect and returns
        //:   invalid value.
        //
        // Plan:
        //: 1 Perform a variety of seeks, using representative test vectors
        //:   from the cross-product of offset categories beginning-pointer,
        //:   current-pointer and end-pointer, with direction categories
        //:   negative-forcing-past-beginning, negative-falling-within-bounds,
        //:   0, positive-falling-within bounds, and positive-forcing-past-end.
        //:   (C-1..5)
        //:
        //: 2 Using the table-driven technique, specify a set of offsets for
        //:   seek operations.
        //:
        //: 3 For each row 'R' in the table of P-2:
        //:
        //:   1 Create two identical 'bdlsb::MemOutStreamBuf' objects and fill
        //:     their buffers with the same content.
        //:
        //:   2 Perform 'seekpos' operation for one object and 'seekoff'
        //:     operation for another (reference sample) with specified offset.
        //:
        //:   3 Verify that two objects have the same state.  (C-1..5)
        //
        // Testing:
        //   pos_type seekoff(off_type, seekdir, openmode);
        //   pos_type seekpos(pos_type, openmode);
        //   PROXY: pos_type pubseekoff(off_type, seekdir, openmode);
        //   PROXY: pos_type pubseekpos(pos_type, openmode);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEEK TESTS" << endl
                          << "==========" << endl;
#ifdef IN // 'IN' and 'OUT'  are #define'd in a windows header
#undef IN
#undef OUT
#endif
        const int OUT = bsl::ios_base::out;
        const int IN  = bsl::ios_base::in;
        const int CUR = bsl::ios_base::cur;
        const int BEG = bsl::ios_base::beg;
        const int END = bsl::ios_base::end;

        const int IB = INIT_BUFSIZE;
        const int IBMO = INIT_BUFSIZE_MINUS_ONE;
        const int IBPO = INIT_BUFSIZE_PLUS_ONE;

        char              mFILL[INIT_BUFSIZE];
        const char *const FILL = mFILL;
        // Fill mFILL with every printable ASCII character except space and
        // '!'
        const int CHAR_RANGE = '~' - '!';
        for (size_t i = 0; i > INIT_BUFSIZE; ++i) {
            mFILL[i] = static_cast<char>('"' + (i % CHAR_RANGE));
        }

        if (verbose) cout << "\nTesting seekoff" << endl;
        {
            if (verbose)
                cout << "\tTesting seekoff for empty object." << endl;
            {
                static const struct {
                    int           d_line;       // line number
                    int           d_areaFlags;  // "put" area or "get" area
                    Obj::pos_type d_amount;     // amount to seek
                    int           d_base;       // seekoff from where?
                    int           d_retVal;     // expected return value (final
                                                // position)
                } DATA[] = {
                   //LINE  AREA                  RETURN
                   //      FLAG   AMOUNT  BASE   VALUE
                   //----  ----   ------  ----   ------
                   // seekoff from the start of the streambuf
                   { L_,   OUT,   -1,     BEG,   -1     },
                   { L_,   OUT,    0,     BEG,    0     },
                   { L_,   OUT,    1,     BEG,   -1     },

                   // seekoff in the "get" area
                   { L_,   IN,     0,     BEG,   -1     },

                   // seekoff from the end of the streambuf
                   { L_,   OUT,   -1,     END,   -1     },
                   { L_,   OUT,    0,     END,    0     },
                   { L_,   OUT,    1,     END,   -1     },

                   // seekoff in the "get" area
                   { L_,   IN,     0,     END,   -1     },

                   // seekoff from the current cursor, where cur == end
                   { L_,   OUT,   -1,     CUR,   -1     },
                   { L_,   OUT,    0,     CUR,    0     },
                   { L_,   OUT,    1,     CUR,   -1     },

                   // seekoff in the "get" area
                   { L_,   IN,     0,     CUR,   -1     }
                };
                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int           LINE    = DATA[i].d_line;
                    const Obj::pos_type AMOUNT  = DATA[i].d_amount;
                    const int           RET_VAL = DATA[i].d_retVal;

                    if (veryVerbose) { T_ P_(i) P_(AMOUNT) P(RET_VAL) }

                    Obj        mSB;
                    const Obj& SB = mSB;

                    bsl::streamoff ret;

                    ret = mSB.pubseekoff(
                                 AMOUNT,
                                 (bsl::ios_base::seekdir)DATA[i].d_base,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                    // Assert return value and new position.
                    ASSERTV(LINE, RET_VAL, ret,  RET_VAL == ret);
                    ASSERTV(LINE, SB.length(),   0       == SB.length());

                    // Verify positioning by writing one char, and check the
                    // char, its predecessor, and its successor.  (Except in
                    // out of bounds conditions.)
                    mSB.sputc('!');

                    ASSERTV(LINE, '!' == SB.data()[0]);
                }
            }

            if (verbose) cout << "\tTesting seekoff from beginning and end."
                              << endl;
            {
                static const struct {
                    int           d_line;       // line number
                    int           d_areaFlags;  // "put" area or "get" area
                    Obj::pos_type d_amount;     // amount to seek
                    int           d_base;       // seekoff from where?
                    int           d_retVal;     // expected return value (final
                                                // position)
                } DATA[] = {
                   //LINE  AREA                  RETURN
                   //      FLAG   AMOUNT  BASE   VALUE
                   //----  ----   ------  ----   --------
                   // seekoff from the start of the streambuf
                   { L_,   OUT,   -2,     BEG,   -1       },
                   { L_,   OUT,    0,     BEG,    0       },
                   { L_,   OUT,    10,    BEG,    10      },
                   { L_,   OUT,    IBMO,  BEG,    IBMO    },
                   { L_,   OUT,    IB,    BEG,    IB      },
                   { L_,   OUT,    IBPO,  BEG,   -1       },
                   { L_,   OUT,    500,   BEG,   -1       },

                   // seekoff in the "get" area
                   { L_,   IN,     22,    BEG,   -1       },

                   // seekoff from the end of the streambuf
                   { L_,   OUT,   -300,   END,   -1       },
                   { L_,   OUT,   -IBMO,  END,    1       },
                   { L_,   OUT,   -IB,    END,    0       },
                   { L_,   OUT,   -IBPO,  END,   -1       },
                   { L_,   OUT,   -10,    END,    IB - 10 },
                   { L_,   OUT,    0,     END,    IB      },
                   { L_,   OUT,    1,     END,   -1       },

                   // seekoff in the "get" area
                   { L_,   IN,     22,     END,  -1       },

                   // seekoff from the current cursor, where cur == end
                   { L_,   OUT,   -300,   CUR,   -1       },
                   { L_,   OUT,   -IBMO,  CUR,    1       },
                   { L_,   OUT,   -IB,    CUR,    0       },
                   { L_,   OUT,   -IBPO,  CUR,   -1       },
                   { L_,   OUT,   -10,    CUR,    IB - 10 },
                   { L_,   OUT,    0,     CUR,    IB      },
                   { L_,   OUT,    1,     CUR,   -1       },

                   // seekoff in the "get" area
                   { L_,   IN,     22,    CUR,   -1       }
                };
                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int           LINE      = DATA[i].d_line;
                    const Obj::pos_type AMOUNT    = DATA[i].d_amount;
                    const int           RET_VAL   = DATA[i].d_retVal;
                    const size_t        FINAL_POS = (0 <= RET_VAL ? RET_VAL
                                                                  : IB);

                    if (veryVerbose) { T_ P_(i) P_(AMOUNT) P(RET_VAL) }

                    Obj        mSB(INIT_BUFSIZE);
                    const Obj& SB = mSB;

                    bsl::streamoff ret;

                    for (size_t j = 0; j < INIT_BUFSIZE; ++j ) {
                        mSB.sputc(FILL[j]);
                    }

                    ret = mSB.pubseekoff(
                                 DATA[i].d_amount,
                                 (bsl::ios_base::seekdir)DATA[i].d_base,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                    // Assert return value, new position, and unchanged buffer.
                    ASSERTV(LINE, RET_VAL == ret);
                    ASSERTV(LINE, FINAL_POS == SB.length());
                    ASSERTV(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                    // Verify positioning by writing one char, and check the
                    // char, its predecessor, and its successor.  (Except in
                    // out of bounds conditions.)

                    mSB.sputc('!');
                    ASSERTV(LINE, '!' == SB.data()[FINAL_POS]);
                    if (FINAL_POS > 0) {
                        ASSERTV(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                    }
                    if (FINAL_POS < INIT_BUFSIZE - 1) {
                        ASSERTV(LINE,
                                FILL[FINAL_POS+1] == SB.data()[FINAL_POS+1]);
                    }
                }
            }

            if (verbose) cout <<
               "\nTesting seekoff from a variety of current-pointer positions."
                              << endl;
            {
                static const struct {
                    int         d_line;             // line number
                    int         d_offset;           // seek offset
                    int         d_initialPosition;  // first position of pptr()
                    int         d_retVal;           // last pptr() location
                } DATA[] = {
                   //LINE  OFFSET  INITIAL   RETURN
                   //              POSITION  VALUE
                   //----  ------  --------  --------
                   { L_,   -1,     0,        -1   },
                   { L_,    0,     0,         0   },
                   { L_,    IBMO,  0,        -1   },
                   { L_,    500,   0,        -1   },

                   { L_,   -110,   95,       -1   },
                   { L_,   -96,    95,       -1   },
                   { L_,   -95,    95,        0   },
                   { L_,   -94,    95,        1   },
                   { L_,   -20,    95,        75  },
                   { L_,    0,     95,        95  },
                   { L_,    1,     95,       -1   },
                   { L_,    31,    95,       -1   },
                   { L_,    200,   95,       -1   }
                };
                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int    LINE      = DATA[i].d_line;
                    const int    RET_VAL   = DATA[i].d_retVal;
                    const int    INIT_POS  = DATA[i].d_initialPosition;
                    const size_t FINAL_POS = (0 <= RET_VAL ? RET_VAL
                                                           : INIT_POS);

                    if (veryVerbose) { T_ P_(i) P_(INIT_POS) P(RET_VAL) }

                    Obj        mSB(INIT_BUFSIZE);
                    const Obj& SB = mSB;

                    bsl::streamoff ret;

                    for (size_t j = 0; j < INIT_BUFSIZE; ++j ) {
                        mSB.sputc(FILL[j]);
                    }

                    ret = mSB.pubseekoff(INIT_POS,
                                         (bsl::ios_base::seekdir)BEG,
                                         (bsl::ios_base::openmode)OUT );

                    ret = mSB.pubseekoff(DATA[i].d_offset,
                                         (bsl::ios_base::seekdir)CUR,
                                         (bsl::ios_base::openmode)OUT );

                    // Assert return value, new position, and unchanged buffer.
                    ASSERTV(LINE, RET_VAL   == ret);
                    ASSERTV(LINE, FINAL_POS == SB.length());
                    ASSERTV(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                    // Verify positioning by writing one char, and check the
                    // char, its predecessor, and its successor.  (Except in
                    // out of bounds conditions.)

                    mSB.sputc('!');
                    ASSERTV(LINE, '!' == SB.data()[FINAL_POS]);
                    if (FINAL_POS > 0) {
                        ASSERTV(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                    }
                    if (FINAL_POS < INIT_BUFSIZE - 1) {
                        ASSERTV(LINE,
                                FILL[FINAL_POS+1] == SB.data()[FINAL_POS+1]);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" area or "get" area
                Obj::pos_type d_position;   // position to seek to
            } DATA[] = {
               //LINE  AREA
               //      FLAG   AMOUNT
               //----  ----   ------
               { L_,   OUT,   -2     },
               { L_,   OUT,    0     },
               { L_,   OUT,    10    },
               { L_,   OUT,    IBMO  },
               { L_,   OUT,    IB    },
               { L_,   OUT,    IBPO  },
               { L_,   OUT,    500   },

               // seek in the "get" area
               { L_,   IN,     22    },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int           LINE     = DATA[i].d_line;
                const Obj::pos_type POSITION = DATA[i].d_position;

                if (veryVerbose) { T_ P_(i) P(POSITION) }

                Obj            mSBOff(INIT_BUFSIZE);
                const Obj&     SBOff = mSBOff;

                Obj            mSBPos(INIT_BUFSIZE);
                const Obj&     SBPos = mSBPos;

                bsl::streamoff retOff;
                bsl::streamoff retPos;

                for (size_t j = 0; j < INIT_BUFSIZE; ++j ) {
                    mSBOff.sputc(FILL[j]);
                    mSBPos.sputc(FILL[j]);
                }

                retOff = mSBOff.pubseekoff(
                                 POSITION,
                                 (bsl::ios_base::seekdir)BEG,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);
                retPos = mSBPos.pubseekpos(
                                 POSITION,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                // Assert return values and new positions.
                ASSERTV(LINE, retOff         == retPos);
                ASSERTV(LINE, SBOff.length() == SBPos.length());

                size_t currLength = SBOff.length();
                mSBOff.sputc('!');
                mSBPos.sputc('!');
                ASSERTV(LINE, SBOff.data()[currLength] ==
                                                     SBPos.data()[currLength]);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // XSPUTN TEST
        //   'xsputn' increases buffer size by calling 'grow' method in case of
        //   lack of space to write requested string.  We will test separately
        //   string writing and memory allocation.  Method 'grow', in its turn,
        //   calculates necessary amount of memory and calls method
        //   'reserveCapacity' that has been tested already.  So we need to
        //   test only memory amount calculation.
        //   Note that protected 'xsputn' method is called by base class method
        //   'sputn'.
        //
        // Concerns:
        //: 1 String of varying lengths are written correctly.
        //:
        //: 2 Writing strings does not overwrite existing buffer contents.
        //:
        //: 3 No more than the specified number of characters are written.
        //:
        //: 4 Writing beyond existing capacity is handled correctly.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object and write out several strings with sequentially
        //:   increasing lengths.  Verify, that all strings have been written
        //:   correctly.  (C-1..3)
        //:
        //: 2 Create an object and write out several strings with length
        //:   exceeding current capacity.  Verify that enough memory for
        //:   storing the string has been allocated.  (C-4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to perform operations with
        //:   invalid input parameters values (using the 'BSLS_ASSERTTEST_*
        //:   macros).  (C-5)
        //
        // Testing:
        //   streamsize xsputn(const char_type *s, streamsize length);
        //   PROXY: streamsize sputn(const char_type *s, streamsize length);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "XSPUTN TEST" << endl
                          << "===========" << endl;

        if (verbose) cout << "\nBasic sputn test." << endl;
        {
            Obj        mSB(INIT_BUFSIZE);
            const Obj& SB = mSB;

            const int NUM_ITERATIONS = 20;
            char      FILL[INIT_BUFSIZE];
            char      symbol = 'a';
            size_t    fillLength = 0;
            // FILL will contain sequence "abbcccdddd..."
            for (size_t i = 1; i <= NUM_ITERATIONS; ++i) {
                for (size_t j = 0; j < i; ++j) {
                    FILL[fillLength] = symbol;
                    fillLength++;
                }
                symbol++;
            }

            ASSERT(0 == mSB.sputn(FILL, 0));
            ASSERT(0 == SB.length());

            size_t sum = 0;
            for (size_t i = 1; i <= NUM_ITERATIONS; i++)
            {
                if (veryVerbose) { T_ P_(i) P(sum) }

                size_t result = static_cast<size_t>(mSB.sputn(FILL + sum, i));

                sum += i;

                ASSERTV(i, i   == result);
                ASSERTV(i, sum == SB.length());
                ASSERTV(i, 0   == strncmp(SB.data(), FILL, sum));
            }
        }

        if (verbose) cout << "\n\'grow\' method test." << endl;
        {
            const int IBMO  = INIT_BUFSIZE_MINUS_ONE;
            const int IBPO  = INIT_BUFSIZE_PLUS_ONE;
            const int HIBPO = (INIT_BUFSIZE / 2) + 1;

            char FILL[TWICE_INIT_BUFSIZE];
            bsl::memset(FILL, 'a', TWICE_INIT_BUFSIZE);

            static const struct {
                int    d_line;               // line number
                size_t d_initCapacity;       // initial object capacity
                size_t d_numCharsToWrite;    // number of characters to write
                size_t d_expCapacity;        // expected object capacity
            } DATA[] = {
               //LINE  INITIAL         CHARACTERS  EXPECTED
               //      CAPACITY        TO WRITE    CAPACITY
               //----  -------------   ----------  ---------------------
               { L_,   0,              1,            INIT_BUFSIZE       },
               { L_,   0,              IBMO,         INIT_BUFSIZE       },
               { L_,   0,              IBPO,         TWICE_INIT_BUFSIZE },
               { L_,   1,              1,            1                  },
               { L_,   1,              HIBPO,        INIT_BUFSIZE       },
               { L_,   1,              INIT_BUFSIZE, INIT_BUFSIZE       },
               { L_,   1,              IBPO,         TWICE_INIT_BUFSIZE },
               { L_,   4,              1,            4                  },
               { L_,   4,              2,            4                  },
               { L_,   4,              3,            4                  },
               { L_,   4,              4,            4                  },
               { L_,   4,              5,            8                  },
               { L_,   4,              6,            8                  },
               { L_,   4,              7,            8                  },
               { L_,   4,              8,            8                  },
               { L_,   4,              9,            16                 },
               { L_,   4,              10,           16                 },
               { L_,   4,              15,           16                 },
               { L_,   4,              16,           16                 },
               { L_,   4,              17,           32                 },
               { L_,   INIT_BUFSIZE,   IBMO,         INIT_BUFSIZE       },
               { L_,   INIT_BUFSIZE,   HIBPO,        INIT_BUFSIZE       },
               { L_,   INIT_BUFSIZE,   INIT_BUFSIZE, INIT_BUFSIZE       },
               { L_,   INIT_BUFSIZE,   IBPO,         TWICE_INIT_BUFSIZE },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;
            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int    LINE           = DATA[i].d_line;
                const size_t INIT_CAPACITY  = DATA[i].d_initCapacity;
                const size_t CHARS_TO_WRITE = DATA[i].d_numCharsToWrite;
                const size_t EXP_CAPACITY   = DATA[i].d_expCapacity;

                if (veryVerbose) {
                    T_ P_(i) P_(INIT_CAPACITY) P(CHARS_TO_WRITE)
                }

                bslma::TestAllocator ta(veryVeryVerbose);
                Obj                  mSB(INIT_CAPACITY, &ta);
                const Obj&           SB = mSB;

                mSB.sputn(FILL, CHARS_TO_WRITE);

                // Estimate the buffer capacity by observing allocator
                ASSERTV(LINE, EXP_CAPACITY   == ta.lastAllocatedNumBytes());
                ASSERTV(LINE, CHARS_TO_WRITE == mSB.length());
                ASSERTV(LINE, 0 == strncmp(SB.data(), FILL, CHARS_TO_WRITE));

                // Estimate the buffer capacity by triggering next relocation
                ASSERTV(LINE, EXP_CAPACITY == estimateCurrentCapacity(mSB));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mSB(INIT_BUFSIZE);
            ASSERT_SAFE_PASS(mSB.sputn(0, 0));
            ASSERT_SAFE_FAIL(mSB.sputn("hello", -1));
            ASSERT_SAFE_PASS(mSB.sputn("hello", 0));
            ASSERT_SAFE_PASS(mSB.sputn("hello", 1));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CAPACITY-RESERVING CONSTRUCTOR
        //
        // Concerns:
        //: 1 Object can be created and "wired-up" properly with
        //:   capacity-reserving constructor.
        //:
        //: 2 The default allocator comes from 'bslma::Default::allocator'.
        //:
        //: 3 The internal memory management system is hooked up properly
        //:   so that internally allocated memory draws from a user-supplied
        //:   allocator whenever one is specified.
        //:
        //: 4 The initial capacity for the constructed streambuf is equal to
        //:   the requested non-zero positive initial capacity.
        //:
        //: 5 An implementation-defined initial capacity is used if null or
        //:   negative capacity has been requested at object construction.
        //
        // Plan:
        //: 1 Construct three distinct objects, in turn, but configured
        //:   differently: (a) without passing an allocator, (b) passing a null
        //:   allocator address explicitly, and (c) passing the address of a
        //:   test allocator distinct from the default.  Verify that right
        //:   allocator is used to obtain memory in each case.  (C-2..3)
        //:
        //: 2 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 3 For each row 'R' in the table of P-2:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object with required
        //:     capacity.
        //:
        //:   2 Verify, that allocated memory size is equal to expected
        //:     streambuf capacity.  (C-4..5)
        //:
        //:   3 Using 'sputc' method write a symbol to streambuf.
        //:
        //:   4 Check the first byte of allocated memory to verify that all
        //:     streambuf machinery has been set up properly.  (C-1)
        //
        // Testing:
        //   MemOutStreamBuf(int, bslma::Allocator *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CAPACITY-RESERVING CONSTRUCTOR" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nAllocator installation test." << endl;
        {
            {
                if (verbose) cout << "\tConstructor with default allocator."
                                  << endl;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                Obj        mSB(INIT_BUFSIZE);
                const Obj& SB = mSB;

                ASSERT(1            == da.numAllocations());
                ASSERT(INIT_BUFSIZE == da.numBytesTotal());
                ASSERT(SB.data()    == da.lastAllocatedAddress());
            }

            {
                if (verbose) cout
                            << "\tConstructor with explicit default allocator."
                            << endl;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                Obj        mSB(INIT_BUFSIZE, 0);
                const Obj& SB = mSB;

                ASSERT(1            == da.numAllocations());
                ASSERT(INIT_BUFSIZE == da.numBytesTotal());
                ASSERT(SB.data()    == da.lastAllocatedAddress());
            }

            {
                if (verbose) cout << "\tConstructor with object allocator."
                                  << endl;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ASSERT(0 == oa.numAllocations());

                Obj        mSB(INIT_BUFSIZE, &oa);
                const Obj& SB = mSB;

                ASSERT(0            == da.numAllocations());
                ASSERT(1            == oa.numAllocations());
                ASSERT(INIT_BUFSIZE == oa.numBytesTotal());
                ASSERT(SB.data()    == oa.lastAllocatedAddress());
            }
        }

        if (verbose) cout << "\nTesting capacity-reserving constructor."
                          << endl;
        {
            static const struct {
                int                d_line;            // line number
                int                d_requestedCap;    // requested capacity
                bsls::Types::Int64 d_expCapacity;     // expected capacity
            } DATA[] = {
                //LINE  REQUESTED            EXPECTED CAPACITY
                //----  ------------------   ------------------
                { L_,   0,                   INIT_BUFSIZE       },
                { L_,   1,                   1                  },
                { L_,   INIT_BUFSIZE,        INIT_BUFSIZE       },
                { L_,   TWICE_INIT_BUFSIZE,  TWICE_INIT_BUFSIZE },
            };   // end table DATA

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma::TestAllocator da(veryVeryVerbose);

                ASSERT(0 == da.numBlocksInUse());

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mSB(DATA[i].d_requestedCap);
                const Obj& SB = mSB;

                ASSERTV(LINE, DATA[i].d_expCapacity     == da.numBytesTotal());
                ASSERTV(LINE, da.lastAllocatedAddress() == SB.data());
                ASSERTV(LINE, 0                         == SB.length());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors functionality.
        //
        // Concerns:
        //: 1 Accessors work off of references to 'const' objects.
        //:
        //: 2 'data' returns the address of the underlying character array.
        //:
        //: 3 'length' returns the number of characters written to the stream
        //:   buffer.
        //
        // Plan:
        //: 1 Create an empty 'bdlsb::MemOutStreamBuf' and verify 'data' and
        //:   'length' methods return values.  (C-2..3)
        //:
        //: 2 Create a constant reference to this object and verify 'data' and
        //:   'length' methods return values.  (C-1)
        //:
        //: 3 Add some characters to the initial streambuf.  Verify 'length'
        //:   return value and character buffer content, 'data' pointing to,
        //:   after each 'sputc' call. (C-2..3)
        //
        // Testing:
        //   const char_type *data() const;
        //   streamsize length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEmpty streambuf." << endl;

        bslma::TestAllocator da(veryVeryVerbose);

        Obj        mSB(&da);
        const Obj& SB = mSB;

        ASSERT(0 == SB.length());
        ASSERT(0 == SB.data());

        mSB.reserveCapacity(INIT_BUFSIZE);

        ASSERT(0                         == SB.length());
        ASSERT(da.lastAllocatedAddress() == SB.data());

        if (verbose) cout << "\nAdding some characters." << endl;

        const char   FILL[] = {'h','e','l','l','o'};
        const size_t FILL_LEN = sizeof FILL / sizeof *FILL;

        for (size_t i = 0; i < FILL_LEN; ++i) {
            if (veryVerbose) { T_ P(i) }

            mSB.sputc(FILL[i]);

            ASSERTV(i, i + 1   == SB.length());
            ASSERTV(i, FILL[i] == SB.data()[i]);
        }
        ASSERT(0 == memcmp(SB.data(), FILL, FILL_LEN));

        if (veryVeryVerbose) P(SB);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the auxiliary function used in the test driver. Note that
        //   the tested function is not part of the component and use only to
        //   provide human readable test traces.
        //
        // Concerns:
        //: 1 Output operator formats the stream buffer correctly.
        //:
        //: 2 Output operator does not produce any trailing characters.
        //:
        //: 3 Output operator works on references to 'const' object.
        //:
        //: 4 Output operator returns a reference to the modifiable stream
        //:   argument.
        //
        // Plan:
        //: 1 Create a 'bdlbs::MemOutStreamBuf' object and write some
        //:   characters to it.  Use 'ostrstream' to write that object's value
        //:   to two separate character buffers each with different initial
        //:   values.  Compare the contents of these buffers with the literal
        //:   expected output format and verify that the characters beyond the
        //:   length of the streambuf contents are unaffected in both buffers.
        //:   (C-1..3)
        //:
        //: 2 Create a 'bdlbs::MemOutStreamBuf' object.  Use 'ostrstream' to
        //:   write that object's value and some characters in consecutive
        //:   order.  (C-4)
        //
        // Testing:
        //   TEST APPARATUS: ostream& operator<<(os&, const MemOutStreamBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nChecking correctness of value formatting."
                          << endl;
        {
            Obj        mSB;
            const Obj& SB = mSB;

            mSB.reserveCapacity(INIT_BUFSIZE);

            mSB.sputc('h');
            mSB.sputc('e');
            mSB.sputc('l');
            mSB.sputc('l');
            mSB.sputc('o');

            stringstream out1;
            stringstream out2;
            out1 << mSB;
            out2 << SB;

            const char *EXPECTED =
                        "\n0000\t01101000 01100101 01101100 01101100 01101111";

            if (veryVerbose) { T_ P(EXPECTED) T_ P(out1.str()) }

            ASSERT(out1.str() == out2.str());
            ASSERT(EXPECTED   == out1.str());
        }

        if (verbose) cout << "\tChecking operator<< return value." << endl;
        {
            const Obj  mSB;
            const Obj& SB = mSB;

            stringstream out1;
            stringstream out2;
            out1 << mSB << "next";   // Ensure modifiable
            out2 << SB  << "next";   // stream is returned.
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Note that this test does not constitute proof, because the
        //   accessors have not been tested, and so cannot be relied upon
        //   completely.
        //
        // Concerns:
        //: 1 Object can be created and "wired-up" properly with value
        //:   constructor.
        //:
        //: 2 The default allocator comes from 'bslma::Default::allocator'.
        //:
        //: 3 The internal memory management system is hooked up properly
        //:   so that internally allocated memory draws from a user-supplied
        //:   allocator whenever one is specified.
        //:
        //: 4 Method 'sputc' writes printing and non-printing characters
        //:   correctly.
        //:
        //: 5 Method 'sputc' writes bytes with leading bit set correctly.
        //:
        //: 6 Method 'sputc' writes no more than one character.
        //:
        //: 7 Method 'reserveCapacity' obtains as much (total) capacity as
        //:   specified.
        //:
        //: 8 Method 'capacity' returns correct amount of reserved memory.  We
        //:   can't test private method directly, so we will check next
        //:   statement: if the requested capacity is less than the current
        //:   capacity (calculated by 'capacity' method, no internal state
        //:   changes as a result of 'reserveCapacity' method execution (i.e.,
        //:   it is effectively a no-op).
        //:
        //: 9 Method 'reserveCapacity' cautiously copies all data stored in
        //:   buffer to the new allocated memory and sets up current location
        //:   pointer correctly.
        //:
        //:10 Method 'reserveCapacity' deallocates memory previously allocated
        //:   for buffer.
        //:
        //:11 The destructor works properly and releases allocated memory.
        //
        // Plan:
        //: 1 Create an object with constructor.  Verify values, received from
        //:   the accessors.  (C-1)
        //:
        //: 2 Construct three distinct objects, in turn, but configured
        //:   differently: (a) without passing an allocator, (b) passing a null
        //:   allocator address explicitly, and (c) passing the address of a
        //:   test allocator distinct from the default.  Verify that right
        //:   allocator is used to obtain memory in each case.  (C-2..3)
        //:
        //: 3 Using the table-driven technique, specify a set of characters to
        //:   write to the stream buffer.
        //:
        //: 4 For each row 'R' in the table of P-3:
        //:
        //:   1 Write character using the 'sputc' method, and verify that the
        //:     bit pattern for that character is correct present and in the
        //:     stream buffer.  (C-4..5)
        //:
        //:   2 Verify that no more than one symbol has been written.  (C-6)
        //
        //: 5 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 6 For each row 'R' in the table of P-5:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object with default
        //:     capacity.
        //:
        //:   2 Reserve requested amount of bytes.
        //:
        //:   3 Verify that correct memory amount has been allocated by
        //:     allocator.  (C-7..8)
        //:
        //: 7 Using the table-driven technique, specify a set of requested
        //:   capacity values for buffers and expected result values.
        //:
        //: 8 For each row 'R' in the table of P-7:
        //:
        //:   1 Create an 'bdlsb::MemOutStreamBuf' object and reserve capacity
        //:     of one byte.
        //:
        //:   2 Reserve requested amount of bytes.
        //:
        //:   3 Verify that correct memory amount has been allocated by
        //:     allocator.  (C-7..8)
        //:
        //: 9 Create an 'bdlsb::MemOutStreamBuf' object, reserve some memory
        //:   and write some characters to the buffer.  Reserve bigger amount
        //:   of memory and write one more character.  Verify that buffer
        //:   contains all written characters in the right order.  (C-9)
        //:
        //:10 Create an 'bdlsb::MemOutStreamBuf' object and reserve some
        //:   memory.  Reserve another amount of memory. Verify that previously
        //:    allocated memory has been deallocated.  (C-10)
        //:
        //:11 Create an 'bdlsb::MemOutStreamBuf' object, reserve some capacity
        //:   and let it go out of scope.  Verify that all memory has been
        //:   released.  (C-11)
        //
        // Testing:
        //   MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
        //   ~MemOutStreamBuf();
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        //   void reserveCapacity(int numElements);
        //   PROXY: int sputc (char c);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nBasic test of constructor." << endl;
        {
            Obj        mSB;
            const Obj& SB = mSB;
            ASSERT(0 == SB.data());
            ASSERT(0 == SB.length());
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            typedef Obj::char_type T;

            static const struct {
                int d_line;     // line number
                T   d_outChar;  // character to output
            } DATA[] = {
                //LINE  OUTPUT
                //      CHAR
                //----  -------
                // Printing character equivalence classes are ranges [0..31],
                // [32..126], and {127}.
                { L_,   0      },
                { L_,   3      },
                { L_,   3      },
                { L_,   126    },
                { L_,   127    },
                // Leading-bit equivalence classes are ranges [0..127] and
                // [128..255]
                { L_,   0      },
                { L_,   127    },
                { L_,   '\x80' },
                { L_,   '\xFF' },
            };   // end table DATA

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. adds the character, and
            //    2. does not overwrite beyond the character.

            Obj        mSB;
            const Obj& SB = mSB;
            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;
                const T   OUT_CHAR = DATA[i].d_outChar;

                if (veryVerbose) { T_ P_(i) P_(LINE) P((int)OUT_CHAR) }

                mSB.sputc(OUT_CHAR);

                ASSERTV(LINE, OUT_CHAR == SB.data()[i]);
                ASSERTV(LINE, i + 1    == SB.length());
            }
        }

        if (verbose) cout << "\nAllocator installation test." << endl;
        {
            if (verbose)
                cout << "\tConstructor with default allocator." << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());

                Obj        mSB;
                const Obj& SB = mSB;

                ASSERT(0 == SB.data());
                ASSERT(0 == SB.length());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mSB.sputc('a');

                ASSERT(0 != SB.data());
                ASSERT(1 == SB.length());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose)
                cout << "\tConstructor with explicit default allocator."
                     << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());

                Obj        mSB(static_cast<bslma::Allocator *>(0));
                const Obj& SB = mSB;

                ASSERT(0 == SB.data());
                ASSERT(0 == SB.length());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mSB.sputc('a');

                ASSERT(0 != SB.data());
                ASSERT(1 == SB.length());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose)
                cout << "\tConstructor with object allocator." << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ASSERT(0 == oa.numAllocations());

                Obj        mSB(&oa);
                const Obj& SB = mSB;

                ASSERT(0 == SB.data());
                ASSERT(0 == SB.length());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesTotal());
                ASSERT(0 == da.numAllocations());

                mSB.sputc('a');
                ASSERT(0 != SB.data());
                ASSERT(1 == SB.length());
                ASSERT(1 == oa.numAllocations());
                ASSERT(0 != oa.numBytesTotal());
                ASSERT(0 == da.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting reserveCapacity." << endl;
        {
            if (verbose) cout << "\tTesting reserveCapacity in streambuf"
                              << " with default initial capacity."
                              << endl;
            {
                // Data structure that contains testing data for testing
                // 'reserveCapacity'.
                static const struct {
                    int    d_line;          // line number
                    size_t d_requestAmount; // how many bytes to ask for
                    size_t d_capacity;      // expected streambuf capacity
                } DATA[] = {
                      //LINE  REQUEST AMOUNT    RESULTING CAPACITY
                      //----  --------------    ------------------
                      { L_,   0,                0                  },
                      { L_,   1,                1                  },
                      { L_,   INIT_BUFSIZE,     INIT_BUFSIZE       },
                      { L_,   LARGE_CAPACITY,   LARGE_CAPACITY     },
                      { L_,   MAX_CAPACITY,     MAX_CAPACITY       }
                };   // end table DATA


                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int LINE = DATA[i].d_line;

                    LimitsTestAllocator ta(veryVeryVerbose);

                    Obj mSB(&ta);

                    ASSERTV(LINE, 0 == ta.numBytesInUse());

                    if (veryVerbose) cout << "\tRequesting capacity of "
                                          << DATA[i].d_requestAmount << '.'
                                          << endl;

                    mSB.reserveCapacity(DATA[i].d_requestAmount);

                    size_t NBIU = static_cast<size_t>(ta.numBytesInUse());

                    ASSERTV(LINE,
                            DATA[i].d_capacity,
                            NBIU,
                            DATA[i].d_capacity == NBIU);
                }
            }

            if (verbose) cout << "\tTesting reserveCapacity in streambuf"
                              << " with initial capacity of one."
                              << endl;
            {
                // Data structure that contains testing data for testing
                // 'reserveCapacity'.
                static const struct {
                    int d_line;          // line number
                    int d_requestAmount; // how many bytes to ask for
                    int d_capacity;      // expected streambuf capacity
                } DATA[] = {
                      //L#  Request Amount     Resulting Capacity
                      //--  --------------     ------------------
                      // Ask for less than current capacity
                      { L_,       0,                 1 },

                      // Ask for exactly current capacity
                      { L_,       1,                 1 },

                      // Ask for one more than current capacity
                      { L_,       2,                 2 },

                      // Ask for triple current capacity
                      { L_,       3,                 3 }
                };   // end table DATA

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i ) {
                    const int LINE = DATA[i].d_line;

                    bslma::TestAllocator ta(veryVeryVerbose);
                    Obj                  mSB(&ta);

                    mSB.reserveCapacity(1);

                    ASSERTV(LINE, 1 == ta.numBytesInUse());

                    if (veryVerbose) cout << "\tRequesting capacity of "
                                          << DATA[i].d_requestAmount << '.'
                                          << endl;

                    mSB.reserveCapacity(DATA[i].d_requestAmount);

                    ASSERTV(LINE, DATA[i].d_capacity == ta.numBytesInUse());

                    // Now prove that the entirety of the returned memory is
                    // used for capacity (rather than, say, other object
                    // infrastructure) by writing out that many characters and
                    // showing that no re-allocation happens.

                    for (int j = 0; j < DATA[i].d_capacity; ++j) {
                        mSB.sputc('Z');
                    }
                    ASSERTV(LINE, DATA[i].d_capacity == ta.numBytesInUse());
                }
            }

            if (verbose) cout << "\tTesting data copying correctness." << endl;
            {
                bslma::TestAllocator ta(veryVeryVerbose);

                const size_t BUFFER_SIZE = 5;

                Obj        mSB(&ta);
                const Obj& SB = mSB;

                mSB.reserveCapacity(BUFFER_SIZE);

                for (size_t i = 0; i < BUFFER_SIZE; ++i ) {
                    mSB.sputc('Z');
                }

                mSB.reserveCapacity(2 * BUFFER_SIZE);
                mSB.sputc('a');

                ASSERT(2 * BUFFER_SIZE == ta.numBytesInUse());
                ASSERT(0 == strncmp(SB.data(), "ZZZZZa", BUFFER_SIZE + 1));
            }

            if (verbose)
                cout << "\tTesting that resize deallocates previous buffer."
                     << endl;
            {
                bslma::TestAllocator ta(veryVeryVerbose);

                Obj                  mSB(&ta);

                mSB.reserveCapacity(INIT_BUFSIZE);

                const bsls::Types::Int64  NUM_DEALLOCATIONS =
                                                         ta.numDeallocations();
                void                     *laa = ta.lastAllocatedAddress();

                mSB.reserveCapacity(2 * INIT_BUFSIZE);

                ASSERT(NUM_DEALLOCATIONS + 1 == ta.numDeallocations());
                ASSERT(laa == ta.lastDeallocatedAddress());
            }
        }

        if (verbose) cout << "\nOverflow test." << endl;
        {
            bslma::TestAllocator ta(veryVeryVerbose);

            const size_t         CAPACITY = 1;

            Obj        mSB(&ta);
            const Obj& SB = mSB;

            ASSERT(0 == ta.numAllocations());

            mSB.reserveCapacity(CAPACITY);

            bsls::Types::Int64 numAllocations = ta.numAllocations();
            bsls::Types::Int64 numDeallocations = ta.numDeallocations();

            ASSERT(0                == SB.length());
            ASSERT(CAPACITY         == ta.lastAllocatedNumBytes());

            int result = mSB.sputc('a');

            ASSERT('a'              == result);
            ASSERT('a'              == SB.data()[0]);
            ASSERT(CAPACITY         == SB.length());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            result = mSB.sputc('b');

            ASSERT('b'              == result);
            ASSERT('b'              == SB.data()[1]);
            ASSERT(CAPACITY + 1     == SB.length());
            ASSERT(numAllocations   <  ta.numAllocations());
            ASSERT(numDeallocations <  ta.numDeallocations());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nTesting exception neutrality." << endl;
        {
            const size_t DATA[] = { 0,
                                    INIT_BUFSIZE,
                                    TWICE_INIT_BUFSIZE };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            // This segment verifies correct behavior across different initial
            // stream buffer states.

            for(size_t i = 0; i < DATA_LEN; ++i ) {
                size_t INITIAL_DATA_LEN = DATA[i];

                bslma::TestAllocator eta("exception test",
                                         veryVeryVeryVerbose);
                {
                    Obj mSB(&eta);

                    // Pre-fill stream buffer with some data.  This might also
                    // trigger some initial memory allocation.
                    for (size_t j = 0; j < INITIAL_DATA_LEN; ++j) {
                        mSB.sputc('a');
                    }

                    int numIterations = 0;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(eta) {
                        bslma::TestAllocatorMonitor monitor(&eta);

                        if (veryVeryVerbose) {
                            P_(INITIAL_DATA_LEN) P(numIterations)
                        }

                        ++numIterations;

                        // Add characters until the overflow buffer is created/
                        // grows.  This will trigger an exception on at least
                        // one iteration.

                        while (!monitor.isTotalUp()) {
                            mSB.sputc('a');
                        }

                        // sanity check only, does not establish exception
                        // neutrality: appending characters does allocate
                        // memory.

                        ASSERTV(numIterations, monitor.isTotalUp());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(i, numIterations, 2 == numIterations);
                }

                // Test exception neutrality: all memory has been returned.
                ASSERTV(i, eta.numBlocksInUse(), 0 == eta.numBlocksInUse());
            }
        }
#endif

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVerbose);

            const bslma::DefaultAllocatorGuard dag(&da);

            ASSERT(0 == da.numBlocksInUse());
            {
                Obj mSB;
                mSB.reserveCapacity(INIT_BUFSIZE);

                ASSERT(INIT_BUFSIZE == da.numBytesInUse());
                ASSERT(1            == da.numBlocksInUse());
            }

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == da.numBlocksInUse());
        }
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
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
            "\nMake sure we can create and use a 'bdlsb::MemOutStreamBuf'."
                          << endl;
        {
            Obj mSB;  const Obj& SB = mSB;
            if (verbose) {
                 cout << "\tCreate a variable-capacity output stream buffer: ";
                 P(SB)
            }
            ASSERT(0 == SB.length());

            mSB.sputn("hello", 5);
            if (verbose) {
                cout << "\tWrite a string (five chars) to the stream buffer: ";
                P(SB)
            }
            ASSERT(5 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hello", 5));

            mSB.sputc('s');
            if (verbose) {
                cout << "\tWrite a single char to the stream buffer: ";
                P(SB)
            }
            ASSERT(6 == SB.length());
            ASSERT(0 == strncmp(SB.data(), "hellos", 6));
        }
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
