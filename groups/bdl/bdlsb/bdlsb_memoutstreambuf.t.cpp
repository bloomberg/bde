// bdlsb_memoutstreambuf.t.cpp                                        -*-C++-*-

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdlsb::MemOutStreamBuf' is created with an allocator which cannot be
//   changed throughout the lifetime of an object, and an optional initial
//   capacity specification.  There is no particular advantage (for purposes
//   of putting the object into white-box state) to using the constructor
//   with capacity, so our set of primary constructors will be:
//
//    o bdlsb::MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   We can bring a 'bdlsb::MemOutStreamBuf' to any achievable white-box
//   state by using a combination of 'pubseekpos', which allows us to
//   reposition the "cursor" (i.e., the position that the next write operation
//   will output to) anywhere in the stream buffer, and 'sputc', which writes
//   a single character into the stream buffer.
//
//    o int_type sputc(char_type);
//    o pos_type pubseekpos(pos_type, ios_base::openmode);
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  The
//   'bdlsb::MemOutStreamBuf' component has only 'length' and 'data' as
//   accessors, and so they form our accessor set:
//
//    o const char_type *data();
//    o streamsize length();
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] bdlsb::MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
// [ 6] bdlsb::MemOutStreamBuf(int numElements, *ba = 0);
// [ 6] ~bdlsb::MemOutStreamBuf();
// MANIPULATORS
// [ 4] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 4] int_type sputc(char_type);
// [ 4] pos_type seekpos(pos_type, openmode);
// [ 9] pos_type seekoff(off_type, seekdir, openmode);
// [ 8] streamsize xsputn(const char_type, streamsize);
// [ 7] void reserveCapacity(int numElements);
// [11] void reset();
// ACCESSORS
// [ 4] const char_type *data() const;
// [ 4] streamsize length() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 5] ostream& operator<<(ostream&, const bdlsb::MemOutStreamBuf&);
// [11] USAGE EXAMPLE: Capitalizing Stream
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsb::MemOutStreamBuf Obj;

const bsl::size_t INITIAL_BUFSIZE           = 256;
const bsl::size_t TWICE_INITIAL_BUFSIZE     = 512;
const bsl::size_t INITIAL_BUFSIZE_PLUS_44   = 300;  // arbitrary number in
                                                    // [1 .. 2k]
const bsl::size_t INITIAL_BUFSIZE_MINUS_ONE = 255;
const bsl::size_t INITIAL_BUFSIZE_PLUS_ONE  = 257;
const bsl::size_t TRIPLE_CAPACITY           = 768;
const bsl::size_t QUADRUPLE_CAPACITY        = 1024;

BSLMF_ASSERT(sizeof(int) <= sizeof(bsl::size_t));

const bsl::size_t LARGE_CAPACITY            =
                 static_cast<bsl::size_t>(bsl::numeric_limits<int>::max()) + 1;

const bsl::size_t MAX_CAPACITY              =
                                       bsl::numeric_limits<bsl::size_t>::max();
// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

                  // =====================================
                  // operator<< for bdlsb::MemOutStreamBuf
                  // =====================================

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdlsb::MemOutStreamBuf& streamBuffer);
    // Write the contents of the specified 'streamBuffer' (as well as a marker
    // indicating eight bytes groupings) to the specified output 'stream' in
    // binary format, and return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&                stream,
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

class LimitsTestAllocator : public bslma::Allocator {
  // This 'class' implements an allocator which can be used to test allocation
  // of amounts of memory that are infeasible to allocate on a system.  It does
  // this by having a small initial buffer that it allocates exclusively out of
  // however, all statistics are updated to match allocating arbitrary amounts
  // of memory.

    // DATA
    enum { k_BUFFER_SIZE = 1024 };        // the size of the internal buffer
                                          // used for real allocations
                        // Statistics

    bsl::size_t d_numAllocations;         // total number of allocation
                                          // requests on this object (including
                                          // those for 0 bytes)

    bsl::size_t d_numDeallocations;       // total number of deallocation
                                          // requests on this object (including
                                          // those supplying a 0 address)

    bsl::size_t d_numBlocksInUse;         // number of blocks currently
                                          // allocated from this object

    bsl::size_t d_numBytesInUse;          // number of bytes currently
                                          // allocated from this object

    bsl::size_t d_numBlocksMax;           // maximum number of blocks ever
                                          // allocated from this object at any
                                          // one time

    bsl::size_t d_numBytesMax;            // maximum number of bytes ever
                                          // allocated from this object at any
                                          // one time

    bsl::size_t d_numBlocksTotal;         // cumulative number of blocks ever
                                          // allocated from this object

    bsl::size_t d_numBytesTotal;          // cumulative number of bytes ever
                                          // allocated from this object

    bool        d_verboseFlags;

    bsl::map <void *, bsl::size_t>        // a map of all requested allocations
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
        bsl::map<void *, bsl::size_t> ::iterator curr =
                                                   d_allocations.find(address);
        ASSERT(curr != d_allocations.end());
        const bsl::size_t  allocation_size   = curr->second;

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

// ACCESORS
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

// ============================================================================
//                    CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This example demonstrates use of a stream buffer by a stream, in this case a
// stream with simple formatting requirements -- namely, capitalizing all
// lower-case ASCII character data that is output.  To simplify the example, we
// do not include the functions for streaming non-character data, e.g., numeric
// values:
//..
    // my_capitalizingstream.h

    class my_CapitalizingStream {
        // This class capitalizes lower-case ASCII characters that are output.

        bdlsb::MemOutStreamBuf d_streamBuf;  // buffer to write to

        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&, char);
        friend
        my_CapitalizingStream& operator<<(my_CapitalizingStream&,
                                          const char *);

      public:
        // CREATORS
        my_CapitalizingStream();
            // Create a capitalizing stream.

        ~my_CapitalizingStream();
            // Destroy this capitalizing stream.

        // ACCESSORS
        const bdlsb::MemOutStreamBuf& streamBuf() { return d_streamBuf; }
            // Return the stream buffer used by this capitalizing stream.  Note
            // that this function is for debugging only.
    };

    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      char                    data);
    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data);
        // Write the specified 'data' in capitalized form to the specified
        // capitalizing 'stream', and return a reference to the modifiable
        // 'stream'.

    my_CapitalizingStream::my_CapitalizingStream()
    {
    }

    my_CapitalizingStream::~my_CapitalizingStream()
    {
    }
//..
// As is typical, the streaming operators are made friends of the class.  We
// use the 'transform' algorithm to convert lower-case characters to uppercase:
//..
    // my_capitalizingstream.cpp

    // FREE OPERATORS
    my_CapitalizingStream& operator<<(my_CapitalizingStream& stream, char data)
    {
        stream.d_streamBuf.sputc(static_cast<char>(bsl::toupper(data)));
        return stream;
    }

    my_CapitalizingStream& operator<<(my_CapitalizingStream&  stream,
                                      const char             *data)
    {
        bsl::string tmp(data);
        transform(tmp.begin(),
                  tmp.end(),
                  tmp.begin(),
                  (int(*)(int))bsl::toupper);
        stream.d_streamBuf.sputn(tmp.data(), tmp.length());
        return stream;
    }
//..
// Given the above two functions, we can now write 'main' as follows:
//..
//  // my_app.m.cpp
//
//  int main(int argc, char **argv)
//  {
//      my_CapitalizingStream cs;
//      cs << "Hello," << ' ' << "World." << '\0';
//
//      // Verify the results by writing to 'stdout'.
//      bsl::cout << cs.streamBuf().data() << bsl::endl;
//  }
//..
// Running the program above produces the following output on 'stdout':
//..
//  HELLO, WORLD.
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use an output streambuf.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE: Capitalizing Stream
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        {
            my_CapitalizingStream cs;
            cs << "Hello," << ' ' << "World." << '\0';

            if (verbose) {
                // Visually verify that the streamed data has been capitalized.
                bsl::cout << cs.streamBuf().data() << bsl::endl;
            }

            ASSERT(0 == bsl::strcmp("HELLO, WORLD.", cs.streamBuf().data()));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // RESET TEST
        //
        // Concerns:
        //   - Calling reset() on a default-constructed streambuf does nothing.
        //   - Calling reset() after reserveCapacity() returns deallocates
        //     reserved memory.
        //   - Calling reset() after adding output returns allocated memory
        //     and sets length() to zero.
        //   - The streambuf works normally after reset()
        //
        // Plan:
        //   - Create a test allocator.
        //   - Default-construct and reset a streambuf and verify that no
        //     memory is allocated and that length() remains zero.
        //   - Construct a streambuf with an initial capacity then reset().
        //     Verify that allocated memory is returned and that length()
        //     remains zero.
        //   - Construct a streambuf and add text to it, then reset().
        //     Verify that allocated memory is returned and that length()
        //     returns to zero.
        //   - In each case do some output after reset to verify function.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESET TESTS" << endl
                                  << "===========" << endl;

        static const char        DATA1[]   = "hello";
        static const bsl::size_t DATA1_LEN = sizeof(DATA1) - 1;
                                                          // No null terminator

        static const char        DATA2[] = "goodbye";
        static const bsl::size_t DATA2_LEN = sizeof(DATA2) - 1;
                                                          // No null terminator

        {
            // Reset after default construct.
            bslma::TestAllocator alloc;
            Obj mSB(&alloc);  const Obj& SB = mSB;
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksTotal());

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksTotal());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

        {
            // Reset after initial capacity
            bslma::TestAllocator alloc;
            Obj mSB(50, &alloc);  const Obj& SB = mSB;
            ASSERT(0 == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data());

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksInUse());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

        {
            // Reset after output
            bslma::TestAllocator alloc;
            Obj mSB(&alloc);  const Obj& SB = mSB;

            mSB.sputn(DATA1, DATA1_LEN);
            ASSERT(DATA1_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA1, DATA1_LEN));

            mSB.reset();
            ASSERT(0 == SB.length());
            ASSERT(0 == alloc.numBlocksInUse());
            ASSERT(0 == SB.data());

            mSB.sputn(DATA2, DATA2_LEN);
            ASSERT(DATA2_LEN == SB.length());
            ASSERT(1 == alloc.numBlocksInUse());
            ASSERT(0 != SB.data() && 0 == memcmp(SB.data(), DATA2, DATA2_LEN));
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SEEK TEST
        //
        // Concerns:
        //   - that seeking relative to a specification ('seekoff') uses
        //      the correct location from which to offset
        //   - that both negative and positive offsets compute correctly
        //   - that seeking positions the "cursor" (i.e., the base-class'
        //      pptr()) at the correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //      the program
        //   - that trying to seek in the "get" area has no effect
        //
        // Plan:
        //   Perform a variety of seeks, using representative test vectors from
        //   the cross-product of offset categories beginning-pointer, current-
        //   pointer and end-pointer, with direction categories negative-
        //   forcing-past-beginning, negative-falling-within-bounds, 0,
        //   positive-falling-within bounds, and positive-forcing-past-end.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   pos_type seekoff(off_type, seekdir, openmode);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEEKOFF TESTS" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTesting seekoff from beginning and end."
                          << endl;

#ifdef IN // 'IN' and 'OUT' are #define'd in a windows header
#undef IN
#undef OUT
#endif
        const int OUT = bsl::ios_base::out;
        const int IN  = bsl::ios_base::in;
        const int CUR = bsl::ios_base::cur;
        const int BEG = bsl::ios_base::beg;
        const int END = bsl::ios_base::end;
        char mFILL[INITIAL_BUFSIZE];

        // Fill mFILL with every printable ASCII character except space and '!'
        const int CHAR_RANGE = '~' - '!';
        for (bsl::size_t i = 0; i > INITIAL_BUFSIZE; ++i) {
            mFILL[i] = static_cast<char>('"' + (i % CHAR_RANGE));
        }

        const char *const FILL = mFILL;
        const int IB = INITIAL_BUFSIZE;
        const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
        const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

        {
            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" area or "get" area
                Obj::pos_type d_amount;     // amount to seek
                int           d_base;       // seekoff from where?
                int           d_retVal;     // expected return value
            } DATA[] = {
               //L#  area                      final
               //    flag   amount     base    position/ retVal
               //--  ----   -------    ----    ----------------
               // seekoff from the start of the streambuf
               { L_,  OUT,   -2,       BEG,      -1   },
               { L_,  OUT,    0,       BEG,      0    },
               { L_,  OUT,   10,       BEG,      10   },
               { L_,  OUT,   IBMO,     BEG,      IBMO },
               { L_,  OUT,   IB,       BEG,      IB   },
               { L_,  OUT,   IBPO,     BEG,      -1   },
               { L_,  OUT,   500,      BEG,      -1   },

               // seekoff in the "get" area
               { L_,  IN,    22,       BEG,      -1  },

               // seekoff from the end of the streambuf
               { L_,  OUT,  -300,      END,      -1  },
               { L_,  OUT,  -IBMO,     END,      1   },
               { L_,  OUT,  -IB,       END,      0   },  // excessive but good
               { L_,  OUT,  -IBPO,     END,      -1 },
               { L_,  OUT,  -10,       END,      IB - 10 },
               { L_,  OUT,    0,       END,      IB  },
               { L_,  OUT,    1,       END,      -1 },

               // seekoff in the "get" area
               { L_,  IN,    22,       END,      -1  },

               // seekoff from the current cursor, where cur == end
               { L_,  OUT,  -300,      CUR,      -1  },
               { L_,  OUT,  -IBMO,     CUR,      1   },
               { L_,  OUT,  -IB,       CUR,      0   },  // excessive but good
               { L_,  OUT,  -IBPO,     CUR,      -1 },
               { L_,  OUT,  -10,       CUR,      IB - 10 },
               { L_,  OUT,    0,       CUR,      IB  },
               { L_,  OUT,    1,       CUR,      -1 },

               // seekoff in the "get" area
               { L_,  IN,    22,       CUR,      -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int    LINE      = DATA[i].d_line;
                const int    RET_VAL   = DATA[i].d_retVal;
                const size_t FINAL_POS = (0 <= RET_VAL ? RET_VAL : IB);

                Obj mSB;  const Obj& SB = mSB;
                bsl::streamoff ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);

                ret = mSB.pubseekoff(DATA[i].d_amount,
                                 (bsl::ios_base::seekdir)DATA[i].d_base,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                // Assert return value, new position, and unchanged buffer.
                LOOP_ASSERT(LINE, RET_VAL == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                // Verify positioning by writing one char, and check the char,
                // its predecessor, and its successor.  (Except in out of
                // bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE,
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
                int         d_initialPosition;  // where to set pptr() first
                int         d_retVal;           // pptr() location after seek
            } DATA[] = {
               //L#  offset   startPoint   endPoint
               //--  ------   ----------   --------
               { L_,  -1,          0,         -1  },
               { L_,   0,          0,          0  },
               { L_,  INITIAL_BUFSIZE_MINUS_ONE,
                                   0,         -1  },
               { L_,  500,         0,         -1  },

               { L_,  -110,       95,        -1   },
               { L_,  -96,        95,        -1   },
               { L_,  -95,        95,         0   },
               { L_,  -94,        95,         1   },
               { L_,  -20,        95,         75  },
               { L_,   0,         95,         95  },
               { L_,   1,         95,         -1  },
               { L_,   31,        95,         -1  },
               { L_,   200,       95,         -1  }
            };
            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int    LINE      = DATA[i].d_line;
                const int    RET_VAL   = DATA[i].d_retVal;
                const int    INIT_POS  = DATA[i].d_initialPosition;
                const size_t FINAL_POS = (0 <= RET_VAL ? RET_VAL : INIT_POS);

                Obj mSB;  const Obj& SB = mSB;
                bsl::streamoff ret;
                mSB.sputn(FILL, INITIAL_BUFSIZE);

                mSB.pubseekpos(INIT_POS);
                ret = mSB.pubseekoff(DATA[i].d_offset,
                                     (bsl::ios_base::seekdir)CUR,
                                     (bsl::ios_base::openmode)OUT );
                if (veryVerbose) {
                    P(ret);
                }

                // Assert return value, new position, and unchanged buffer.
                LOOP_ASSERT(LINE, RET_VAL == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());
                LOOP_ASSERT(LINE, 0 == bsl::memcmp(FILL, SB.data(), IB));

                // Verify positioning by writing one char, and check the char,
                // its predecessor, and its successor.  (Except in out of
                // bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS-1] == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE,
                                FILL[FINAL_POS+1] == SB.data()[FINAL_POS+1]);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PUT string TEST
        //
        // Concerns:
        //   - that string of varying lengths are written correctly
        //   - that writing strings does not overwrite existing buffer
        //      contents
        //   - that no more than the specified number of characters are written
        //   - that writing beyond existing capacity is handled correctly
        //
        // Plan:
        //   Write out representative strings from the equivalence classes 0
        //   characters, 1 character, and > 1 character, into streambufs with
        //   representative contents "empty", substantially less than
        //   capacity, and almost-full-so-that-next-write-exceeds-capacity.
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   streamsize xsputn(const char_type, streamsize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUT STRING TEST" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting sputn." << endl;
        {
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;

            char FILL[2001];     bsl::memset(FILL, 'a', 2001);
            char abcFILL[2004];  bsl::memset(abcFILL + 3, 'a', 2001);
            abcFILL[0] = 'a';
            abcFILL[1] = 'b';
            abcFILL[2] = 'c';
            char CAPACITYFILL[INITIAL_BUFSIZE];
            bsl::memset(CAPACITYFILL, 'a', IBMO);  CAPACITYFILL[IBMO] = 0;
            const char *CF = CAPACITYFILL;

            char RES1[INITIAL_BUFSIZE];
            bsl::memset(RES1, 'a', INITIAL_BUFSIZE_MINUS_ONE);
            RES1[INITIAL_BUFSIZE_MINUS_ONE] = 's';

            char RES2[2255];      bsl::memset(RES2, 'a', 2255);
            char RES3[IBMO + 3];  bsl::memset(RES3, 'a', IBMO);
            RES3[IBMO]     = 'a';
            RES3[IBMO + 1] = 'b';
            RES3[IBMO + 2] = 'c';

            static const struct {
                int         d_line;          // line number
                const char *d_outStr;        // string to output from
                int         d_outLen;        // num chars to write
                const char *d_initialCont;   // initial contents of stream
                const char *d_result;        // expected contents after 'sputn'
                size_t      d_resultLen;     // how much to compare
            } DATA[] = {
               //L#   out    out  initial  result   result
               //    strng length content content   length
               //--  ----- ------ ------- -------   -------
               { L_,  "",     0,     "",   "",     0           },
               { L_,  "s",    1,     "",   "s",    1           },
               { L_,  "abc",  3,     "",   "abc",  3           },
               { L_,  FILL,  2000,   "",   FILL,   2000        },

               { L_,  "",     0,     "a",  "a",    1           }, //xtra
               { L_,  "s",    1,     "a",  "as",   2           },
               { L_,  "abc",  3,     "a",  "aabc", 4           },
               { L_,  FILL,  2000,   "a",  FILL,   2001        },

               // this entire quad is extra
               { L_,  "",     0,    "abc", "abc",    3         },
               { L_,  "s",    1,    "abc", "abcs",   4         },
               { L_,  "abc",  3,    "abc", "abcabc", 6         },
               { L_,  FILL,  2000,  "abc", abcFILL,  2003      },

               { L_,  "",     0,     CF,   RES1,   IBMO        },
               { L_,  "s",    1,     CF,   RES1,   IBMO + 1    },
               { L_,  "abc",  3,     CF,   RES3,   IBMO + 3    },
               { L_,  FILL, 2000,    CF,   RES2,   IBMO + 2000 }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                Obj mSB;  const Obj& SB = mSB;

                for (unsigned j = 0; j < strlen(DATA[i].d_initialCont); ++j) {
                    mSB.sputc(DATA[i].d_initialCont[j]);
                }
                bsl::streamsize retResult = mSB.sputn(DATA[i].d_outStr,
                                                      DATA[i].d_outLen);
                LOOP_ASSERT(LINE, 0 == strncmp(SB.data(), DATA[i].d_result,
                                                       DATA[i].d_resultLen));
                LOOP_ASSERT(LINE, DATA[i].d_outLen == retResult);
                LOOP_ASSERT(LINE, DATA[i].d_resultLen == SB.length())

                if (veryVerbose) { cout << '\t'; P(SB.length()); }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Reserve Capacity
        //
        // Concerns:
        //   - that the method obtains at least as much (total) capacity as
        //      specified
        //   - that growth is geometric (i.e., always increases by a fixed
        //      multiplicative factor).
        //   - that if the requested capacity is less than the current
        //      capacity, no internal state changes as a result of method
        //      execution (i.e., it is effectively a no-op).
        //
        // Plan:
        //   Test reserveCapacity with each of the following properties:
        //    - a request asking for less than the present capacity
        //    - a request for exactly the current capacity
        //    - a request for one more than current capacity
        //    - a request for triple the current capacity (to ensure that the
        //       resulting capacity is actually quadruple the initial, since we
        //       expect geometric growth with multiplicative factor 2.)
        //
        // Tactics:
        //   - Category-Partitioning Data Selection Method
        //   - Table-Based Implementation Technique
        //
        // Testing:
        //   void reserveCapacity(int numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESERVE CAPACITY TEST" << endl
                          << "=====================" << endl;

        if (verbose) cout <<
        "\nTesting reserveCapacity in streambuf with default initial capacity."
                          << endl;
        {
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
            const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

            // Data structure that contains testing data for testing
            // 'reserveCapacity'.
            static const struct {
                int         d_line;          // line number
                bsl::size_t d_requestAmount; // how many bytes to ask for
                bsl::size_t d_capacity;      // expected streambuf capacity
            } DATA[] = {
                  //L#  Request Amount     Resulting Capacity
                  //--  --------------     ------------------
                  // Ask for less than current capacity
                  { L_,       0,           INITIAL_BUFSIZE       },
                  { L_,      IBMO,         INITIAL_BUFSIZE       },

                  // Ask for exactly current capacity
                  { L_,  INITIAL_BUFSIZE,  INITIAL_BUFSIZE       },

                  // Ask for one more than current capacity
                  { L_,      IBPO,         IBPO                  },

                  // Ask for triple current capacity
                  { L_,  TRIPLE_CAPACITY,  TRIPLE_CAPACITY       },

                  // Ask for a edge case amount of memory
                  { L_,  LARGE_CAPACITY,   LARGE_CAPACITY        },

                  // For the the maximum amount of memory possible
                  { L_,  MAX_CAPACITY,     MAX_CAPACITY          }
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                LimitsTestAllocator ta(veryVeryVerbose);
                Obj mSB(&ta);
                mSB.sputc('Z');
                LOOP_ASSERT(LINE, INITIAL_BUFSIZE == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                LOOP3_ASSERT(LINE,
                             DATA[i].d_capacity,
                             ta.numBytesInUse(),
                             DATA[i].d_capacity == ta.numBytesInUse());
            }
        }

        if (verbose) cout <<
         "\nTesting reserveCapacity in streambuf with initial capacity of one."
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

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                bslma::TestAllocator ta(veryVeryVerbose);
                Obj mSB(1, &ta);
                LOOP_ASSERT(LINE, 1 == ta.numBytesInUse());
                if (veryVerbose) cout << "\tRequesting capacity of "
                                      << DATA[i].d_requestAmount << '.'
                                      << endl;
                mSB.reserveCapacity(DATA[i].d_requestAmount);
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());

                // Now prove that the entirety of the returned memory is used
                // for capacity (rather than, say, other object infrastructure)
                // by writing out that many characters and showing that no
                // realloc happens.

                for (int j = 0; j < DATA[i].d_capacity; ++j) {
                    mSB.sputc('Z');
                }
                LOOP_ASSERT(LINE, DATA[i].d_capacity == ta.numBytesInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // Concerns:
        //   - That the initial capacity for the constructed streambuf is
        //     equal to the requested initial capacity.
        //   - That the specified or bslma::Default::defaultAllocator is used.
        //   - That the destructor cleans up properly, in particular, returns
        //     the allocated memory.
        //
        // Plan:
        //   - Use 'bslma::TestAllocator' to verify that specified allocator
        //      is used.
        //   - Use 'bslma::TestAllocator' to verify initial memory request
        //     size.
        //   - Write out the requested-initial-capacity number of bytes,
        //      and verify that no reallocation is done.
        //   - Install a 'bslma::TestAllocator' in 'bslma::DefaultAllocator' to
        //      verify that 'bslma::Default::defaultAllocator()' is used by
        //      default.
        //   - Allow the destructor to execute and verify (from TestAllocator)
        //      that all memory has been released.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Techniques
        //
        // Testing:
        //   bdlsb::MemOutStreamBuf(int numElements, *ba = 0);
        //   ~bdlsb::MemOutStreamBuf();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest capacity-reserving constructors." << endl;

        static int DATA[] = {
            0, 2, 4, 8, 16, 32, 64, 128, 256, 512
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int BUFSIZE = 1024;
        char buffer[BUFSIZE];  bsl::memset(buffer, 'a', BUFSIZE);

        if (verbose) cout << "\nExplicitly specify allocator." << endl;
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const bsls::Types::Int64 NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma::DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const size_t CAPACITY = DATA[ti];

                bslma::TestAllocator ta(veryVeryVerbose);
                const bsls::Types::Int64 NUM_BLOCKS = ta.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    Obj mSB(CAPACITY, &ta);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const bsls::Types::Int64 NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma::DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const bsl::size_t CAPACITY = DATA[ti];

                bslma::TestAllocator ta(veryVeryVerbose);
                const bsls::Types::Int64 NUM_BLOCKS = ta.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    Obj mSB(CAPACITY, &ta);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1     == ta.numBlocksInUse());
                    ASSERT(NUM_BLOCKS_DEFAULT == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }

        if (verbose) cout << "\nUse default allocator." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma::TestAllocator da(veryVeryVerbose);
                const bsls::Types::Int64 NUM_BLOCKS = da.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mSB(CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const bsls::Types::Int64 NUM_BLOCKS_DEFAULT = da.numBlocksInUse();
            ASSERT(0 == NUM_BLOCKS_DEFAULT);
            bslma::DefaultAllocatorGuard dag(&da);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int CAPACITY = DATA[ti];

                bslma::TestAllocator da(veryVeryVerbose);
                const bsls::Types::Int64 NUM_BLOCKS = da.numBlocksInUse();
                ASSERT(0 == NUM_BLOCKS);
                {
                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj mSB(CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());

                    mSB.sputn(buffer, CAPACITY);
                    ASSERT(NUM_BLOCKS + 1 == da.numBlocksInUse());
                }
                ASSERT(0 == NUM_BLOCKS);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OUTPUT: bsl::ostream& 'operator<<'
        //
        // NOTE: 'op<<' is NOT part of the component.  It is, however, an
        // auxiliary function used in the test driver, and so we include the
        // 'traditional' test case 5 here in order to test the operator.
        //
        // Concerns:
        //   - that value is formatted correctly (i.e., as binary)
        //   - that no additional characters are written after terminating
        //   - that these functions work on references to 'const' objects
        //   - that each return a reference to the modifiable stream argument
        //
        // Plan:
        //   For each of a small representative set of object values use
        //   'ostrstream' to write that object's value to two separate
        //   character buffers each with different initial values.  Compare
        //   the contents of these buffers with the literal expected output
        //   format and verify that the characters beyond the length of the
        //   streambuf contents are unaffected in both buffers.
        //   - Test operator<< on the empty streambuf.
        //   - Test operator<< on a streambuf containing some characters.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //
        // Testing:
        //   ostream& operator<<(ostream&, const bdlsb::MemOutStreamBuf&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OUTPUT bsl::ostream& 'operator<<'" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nCreate stream buffers to be printed." << endl;

        {
            if (verbose) cout << "\tChecking operator<< return value." << endl;
            const Obj SB;

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << SB << "arbitrary";  // Ensure modifiable
            out2 << SB << "value";      // stream is returned.
        }

        {
            Obj mSB;  const Obj& SB = mSB;
            mSB.sputn("hello", 5);

            const int SIZE = 100;
            char buf1[SIZE], buf2[SIZE];
            bsl::memset(buf1, 'X', SIZE);
            bsl::memset(buf2, 'Y', SIZE);
            ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
            out1 << mSB;
            out2 << SB;
            char *endOfString = strchr(buf1, 'X');
            int stringLen = endOfString - buf1;
            ASSERT(0 == memcmp(buf1, buf2, stringLen));
            ASSERT('X' == buf1[stringLen]);
            ASSERT('Y' == buf2[stringLen]);

            const char *EXPECTED =
                     "\n0000\t01101000 01100101 01101100 01101100 01101111";
            ASSERT(0 == memcmp(buf1, EXPECTED, stringLen));
            buf1[stringLen] = 0;
            if (verbose) {
                T_ P(buf1)
            }
        }

        {
            // One more test, just to see something different
            Obj SB;
            SB.sputc('0');  SB.sputc('1');  SB.sputc('2');  SB.sputc('3');
            SB.sputc('4');  SB.sputc('5');  SB.sputc('6');  SB.sputc('7');
            SB.sputc('8');  SB.sputc('9');  SB.sputc('A');  SB.sputc('B');

            const char *EXPECTED =
                                 "\n0000\t00110000 00110001 00110010 00110011 "
                                         "00110100 00110101 00110110 00110111"
                                 "\n0008\t00111000 00111001 01000001 01000010";
            char ACTUAL[TWICE_INITIAL_BUFSIZE];
            char val = bsl::numeric_limits<char>::max();
            bsl::memset(ACTUAL, static_cast<int>(val), TWICE_INITIAL_BUFSIZE);
            ostrstream out(ACTUAL, TWICE_INITIAL_BUFSIZE);
            out << SB << ends;

            const int LEN = strlen(EXPECTED) + 1;
            if (verbose) {
                cout << endl;
                T_ P(EXPECTED)
                T_ P(ACTUAL)
            }

            ASSERT(val == ACTUAL[LEN]);
                                                        // check for overrun
            ASSERT(0 == memcmp(ACTUAL, EXPECTED, LEN));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a 'bdlsb::MemOutStreamBuf'.
        //
        // Concerns:
        //   (sputc)
        //   - that printing and non-printing characters are written correctly
        //   - that bytes with leading bit set are written correctly
        //   - that no more than one character is written
        //
        //   We also test overflow as triggered by 'sputc' here.
        //
        //   (seekpos)
        //   - that seeking positions the "cursor" (i.e., pptr()), at the
        //      correct location
        //   - that seeking out of bounds does not throw or abort or crash
        //      the program
        //   - that trying to seek in the "get" area has no effect
        //
        //    (data & length)
        //   - To ensure that accessors work off of references to 'const'
        //      objects.
        //   - That data return the address of the underlying character array.
        //   - That length return the number of characters written to the
        //      stream buffer.
        //
        // Plan:
        //   (sputc)
        //   - Write each range-endpoint from each category of characters
        //      using the 'sputc' method, and verify that the bit pattern for
        //      that character is present and correct in the stream buffer.
        //
        //   (seekpos)
        //   - Seek to the following equivalence classes: before the start of
        //      the buffer, inside the buffer, and after the end of the buffer.
        //      Also try seeking in the "get" area.
        //
        //   (data and length)
        //   - Create an empty 'bdlsb::MemOutStreamBuf' and verify its length.
        //   - Add a character, and verify the length and content.
        //   - Add enough characters to use all the initial capacity, and then
        //      verify length and content.
        //   - Then add one more character so as to exceed initial capacity,
        //      and verify length and content.
        //
        // Tactics:
        //   'sputc':
        //     - Category-Partitioning Data Selection Method
        //     - Table-Based and Brute-Force Implementation Technique
        //   'seekpos':
        //     - Category-Partitioning Data Selection Method
        //     - Table-Based Techniques
        //
        // Testing:
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        //   int_type sputc(char_type);
        //   pos_type seekpos(pos_type, openmode);
        //   const char_type *data() const;
        //   streamsize length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nEmpty streambuf." << endl;
        {
            bslma::TestAllocator da(veryVeryVerbose);
            const Obj SB(&da);
            ASSERT(0 == SB.length());
            ASSERT(SB.data() == da.lastAllocatedAddress());

            if (veryVerbose) P(SB);
        }

        if (verbose) cout << "\nTesting sputc." << endl;
        {
            typedef Obj::char_type T;

            static const struct {
                int            d_line;     // line number
                T              d_outChar;  // character to output
            } DATA[] = {
                  //L#  output char    resulting
                  //                 stream contents
                  //--  -----------  ---------------
                  // Printing character equivalence classes are ranges [0..31],
                  // [32..126], and {127}.
                  { L_,    0  },
                  { L_,    3  },
                  { L_,    3  },
                  { L_,   126 },
                  { L_,   127 },

                  // Leading-bit equivalence classes are ranges
                  // [0..127] and [128..255]
                  { L_,      0    },
                  { L_,    127    },
                  { L_,    '\x80' },
                  { L_,    '\xFF' },
            };   // end table DATA

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            // This loop verifies that 'sputc' both:
            //    1. adds the character, and
            //    2. does not overwrite beyond the character.

            if (verbose) {
                T_ cout << "Testing different character types." << endl;
            }

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int LINE = DATA[i].d_line;

                Obj mSB;  const Obj& SB = mSB;
                mSB.sputc(DATA[i].d_outChar);
                LOOP_ASSERT(LINE, DATA[i].d_outChar == SB.data()[0]);
                LOOP_ASSERT(LINE, 1 == SB.length());
                if (veryVerbose) { P(SB); }
            }
        }
        {
            Obj mSB;  const Obj& SB = mSB;

            // Stream out INITIAL_BUFSIZE characters.

            if (verbose) {
                T_ cout << "Using 'sputc' to fill capacity." << endl;
            }

            ASSERT(0 == SB.length());
            for (bsl::size_t i = 0; i < INITIAL_BUFSIZE; ++i ) {
                mSB.sputc('a');
            }

            // Check results.

            ASSERT(INITIAL_BUFSIZE == SB.length());
            for (bsl::size_t i = 0; i < INITIAL_BUFSIZE; ++i ) {
                LOOP_ASSERT(i, 'a' == SB.data()[i]);
                if (verbose) P(SB.data()[i]);
            }
            if (veryVerbose) { P(SB); }

            // Add one, forcing an overflow.

            if (verbose) {
                T_ cout << "Using 'sputc' to exceed capacity." << endl;
            }

            mSB.sputc('a');
            ASSERT(INITIAL_BUFSIZE_PLUS_ONE == SB.length());
            ASSERT('a' == SB.data()[INITIAL_BUFSIZE]);
        }

        if (verbose) cout << "\nTesting seekpos." << endl;
        {
            const int OUT  = bsl::ios_base::out;
            const int IN   = bsl::ios_base::in;
            const int IB   = INITIAL_BUFSIZE;
            const int IBMO = INITIAL_BUFSIZE_MINUS_ONE;
            const int IBPO = INITIAL_BUFSIZE_PLUS_ONE;

            static const struct {
                int           d_line;       // line number
                int           d_areaFlags;  // "put" or "get" area
                Obj::pos_type d_amount;     // amount to seek
                int           d_retVal;     // 'length()' location after seek
            } DATA[] = {
               //L#  area   amount  final
               //    flag           position/ retVal
               //--  ----   ------- ----------------
               // Seek to before the start of the buffer.
               { L_,  OUT,   -1,       -1   },

               // Seek to the start of the buffer.
               { L_,  OUT,    0,        0   },

               // Seek to the last char of the buffer.
               { L_,  OUT,   IBMO,      IBMO  },

               // Seek to the end of the buffer.
               { L_,  OUT,   IB,        IB    },

               // Seek to past the end of the buffer.
               { L_,  OUT,   IBPO,       -1    },

               // Seek in the "get" area, not legal for output buffers.
               { L_,  IN,    138,        -1   }
            };

            const int DATA_LEN = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_LEN; ++i ) {
                const int    LINE      = DATA[i].d_line;
                const int    RET_VAL   = DATA[i].d_retVal;
                const size_t FINAL_POS = RET_VAL >= 0
                                         ? RET_VAL
                                         : INITIAL_BUFSIZE;

                Obj mSB;  const Obj& SB = mSB;
                Obj::pos_type ret;
                for (bsl::size_t j = 0; j < INITIAL_BUFSIZE; ++j) {
                    mSB.sputc('a');
                }

                ret = mSB.pubseekpos(DATA[i].d_amount,
                                 (bsl::ios_base::openmode)DATA[i].d_areaFlags);

                if (veryVerbose) P(ret)

                LOOP_ASSERT(LINE, RET_VAL   == ret);
                LOOP_ASSERT(LINE, FINAL_POS == SB.length());

                // Verify positioning by writing one char, and check the char,
                // its predecessor, and its successor.  (Except for
                // out-of-bounds positions).

                // Verify positioning by writing one char, and check the char,
                // its predecessor, and its successor.  (Except in out of
                // bounds conditions.)

                if (veryVerbose) P(ret);
                mSB.sputc('!');
                LOOP_ASSERT(LINE, '!' == SB.data()[FINAL_POS]);
                if (FINAL_POS > 0) {
                    LOOP_ASSERT(LINE, 'a' == SB.data()[FINAL_POS-1]);
                }
                if (FINAL_POS < INITIAL_BUFSIZE - 1) {
                    LOOP_ASSERT(LINE, 'a' == SB.data()[FINAL_POS+1]);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the default allocator comes from
        //     'bslma::Default::allocator'
        //   - That changing the default allocator after construction has
        //      no effect on an existing 'bdlsb::MemOutStreamBuf' object.
        //   - That the primary constructor initializes the streambuf to
        //      have capacity INITIAL_BUF_SIZE.
        //
        // Plan:
        //   - To ensure that the basic constructor takes its allocator by
        //      default from 'bslma::Default::allocator', install a separate
        //      object of 'bdem::TestAllocator' as the default allocator and
        //      use its object-specific statistics to verify that it is in
        //      fact the source of default allocations and deallocations.
        //   - To ensure that changing the default allocator has no effect
        //      on existing table objects, swap it out and then continue to
        //      allocate additional objects.
        //   - To ensure that initial capacity is INITIAL_BUFSIZE, use the
        //      'bslma::TestAllocator' and show that right after stream buffer
        //      construction, INITIAL_BUFSIZE bytes have been explicitly
        //      requested.  As additional evidence, write out INITIAL_BUFSIZE
        //      chars; show that no new blocks have been requested; then write
        //      an additional character, and show that another block has been
        //      requested as a consequence.
        //      Note that this test does not constitute proof, because the
        //      writing function has not been tested, and so cannot be relied
        //      upon completely.  We nonetheless need this further evidence
        //      because, although the test allocator can tell us that
        //      INITIAL_BUFSIZE bytes were obtained, it cannot tell us that
        //      the streambuf is allocating them all for 'capacity' (as
        //      opposed to, say, object infrastructure).
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdlsb::MemOutStreamBuf(bslma::Allocator *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) {
            cout << "\nEnsure bdem::Default::allocator() is used by default."
                 << endl;
        }
        {

            bslma::TestAllocator da(veryVeryVerbose);

            if (verbose) cout <<
                       "\tInstall test allocator 'da' as the default." << endl;

            {
                const bslma::DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksInUse());
                Obj x;
                const bsls::Types::Int64 NBT = da.numBlocksInUse();
                ASSERT(0 == NBT);

                bslma::TestAllocator ta(veryVeryVerbose);
                ASSERT(0 == ta.numBlocksInUse());

                Obj y(&ta);
                ASSERT(NBT == da.numBlocksInUse());
                ASSERT(NBT == ta.numBlocksInUse());

                bslma::TestAllocator oa(veryVeryVerbose);
                           // Installing this other allocator should have no
                           // effect on subsequent use of pre-existing objects.
                if (verbose) cout <<
                       "\tInstall test allocator 'oa' as the default." << endl;
                {
                    const bslma::DefaultAllocatorGuard oag(&oa);

                    ASSERT(0 == oa.numBlocksInUse());
                    Obj z;
                    ASSERT(NBT == oa.numBlocksTotal());

                    ASSERT(NBT == da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());

                    const int WRITE_SIZE = 300;
                               // Note: An arbitrary number > INITIAL_BUFSIZE!
                    char bigBuf[WRITE_SIZE];
                    bsl::memset(bigBuf, 'a', WRITE_SIZE);
                    streamsize result;
                    result = x.sputn(bigBuf, WRITE_SIZE);
                                               // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    result = y.sputn(bigBuf, WRITE_SIZE);
                                                // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    result = z.sputn(bigBuf, WRITE_SIZE);
                                                // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT <  oa.numBlocksTotal());
                    ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                    ASSERT(ta.numBlocksTotal() == da.numBlocksTotal());
                    ASSERT(ta.numBlocksTotal() == oa.numBlocksTotal());
                }

                if (verbose) cout <<
                    "\tUn-install test allocator 'oa' as the default." << endl;

            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }

        if (verbose) cout <<
            "\nEnsure that stream buffers initially have INITIAL_BUFSIZE "
            "bytes by default."
                          << endl;

        {
            bslma::TestAllocator da(veryVeryVerbose);
            {
                const bslma::DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksTotal());
                Obj x;
                const bsls::Types::Int64 NBT = da.numBlocksTotal();
                ASSERT(0 == NBT);
                ASSERT(0 == da.numBytesInUse());

                const int WRITE_SIZE = INITIAL_BUFSIZE;
                char bigBuf[WRITE_SIZE];
                bsl::memset(bigBuf, 'a', WRITE_SIZE);
                streamsize result = x.sputn(bigBuf, INITIAL_BUFSIZE);

                ASSERT(INITIAL_BUFSIZE == da.numBytesInUse());
                ASSERT(1 == da.numBlocksTotal());
                ASSERT(WRITE_SIZE == result);  // ensure 'sputn' succeeded

                Obj::int_type result2 = x.sputc('f');
                ASSERT(TWICE_INITIAL_BUFSIZE == da.numBytesInUse());
                ASSERT(2 == da.numBlocksTotal());
                ASSERT('f' == result2);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //   This test driver has no test apparatus.
        //
        // Plan:
        //   N/A
        //
        // Tactics:
        //   N/A
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout << endl
                          << "This test driver has no test apparatus." << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test merely exercises basic functionality.
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Do whatever is needed:
        //   - Try writing out to a stream buffer.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
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
