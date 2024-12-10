// bslma_testallocator.t.cpp                                          -*-C++-*-

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bslexceptionutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <new>      // `std::bad_alloc`
#include <cstdio>   // (in `std::`) `sprintf`, `mktmp`, `fclose`, `fread`
#include <cstdlib>  // `std::atoi`
#include <cstring>  // (in `std::`) `memset`, `memcpy`, `strlen`, `strcmp`

#if defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CMP_GNU)
#include <sys/resource.h>  // `setrlimit`
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <crtdbg.h>  // `_CrtSetReportMode`, to suppress popups
#else
#include <pthread.h>
#endif

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using namespace BloombergLP;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  This "test"
// implementation calls `malloc` and `free` directly.  We must verify that
// objects of this concrete class do not call global operators `new` and
// `delete`.  We can do that by redefining these global operators and
// instrumenting them to be sure that these operators are in fact not called.
//
// We also need to verify that when bad addresses are supplied that we can
// detect them and report the problem to `stdout`.  Since this behavior is not
// an error during the testing of this component, we will first set the quiet
// flag to suppress the output part, but will still verify the status to ensure
// that the problem was in fact detected.
//
// We must also verify that when exceptions are enabled, the test allocator
// throws an exception after the number of requests exceeds the allocator's
// (non-negative) allocation limit.  We achieve this by repeatedly allocating
// memory from the test allocator until the number of requests exceeds the
// allocation limit, then verify that exceptions thrown by the allocator are
// caught and that they contain the expected contents.
//-----------------------------------------------------------------------------
// [ 1] TestAllocator(bool verboseFlag = 0);
// [ 6] TestAllocator(const char *name, bool verboseFlag = 0);
// [ 3] ~bslma::TestAllocator();
// [ 3] void *allocate(size_type size);
// [ 3] void deallocate(void *address);
// [ 2] void setAllocationLimit(Int64 limit);
// [ 2] void setNoAbort(bool flagValue);
// [ 2] void setQuiet(bool flagValue);
// [ 2] void setVerbose(bool flagValue);
// [ 2] Int64 allocationLimit() const;
// [ 2] bool isNoAbort() const;
// [ 2] bool isQuiet() const;
// [ 2] bool isVerbose() const;
// [ 1] void *lastAllocatedAddress() const;
// [ 1] size_type lastAllocatedNumBytes() const;
// [ 1] void *lastDeallocatedAddress() const;
// [ 1] size_type lastDeallocatedNumBytes() const;
// [ 6] const char *name() const;
// [ 1] Int64 numAllocations() const;
// [ 1] Int64 numBlocksInUse() const;
// [ 1] Int64 numBlocksMax() const;
// [ 1] Int64 numBlocksTotal() const;
// [10] Int64 numBoundsErrors() const;
// [ 1] Int64 numBytesInUse() const;
// [ 1] Int64 numBytesMax() const;
// [ 1] Int64 numBytesTotal() const;
// [ 1] Int64 numDeallocations() const;
// [ 1] Int64 numMismatches() const;
// [11] friend t_OS& operator<<(t_OS& stream, const TestAllocator& ta);
// [11] void print() const;
// [ 2] int status() const;
// [16] TestAllocatorStashedStatistics stashStatistics();
// [16] void restoreStatistics(const TestAllocatorStashedStatistics&);
//-----------------------------------------------------------------------------
// [18] USAGE EXAMPLE
// [17] DRQS 129104858
// [ 1] BASIC TEST
// [ 4] SIMPLE STREAMING
// [ 5] CONCERN: exception is thrown after allocation limit is exceeded.
// [ 1] CONCERN: all counts are initialized to zero (placement new).
// [ 1] CONCERN: global operators new and delete are *not* called.
// [ 3] CONCERN: the allocator is incompatible with new/delete.
// [ 3] CONCERN: the allocator is incompatible with malloc/free.
// [ 3] CONCERN: mismatched deallocations are detected/reported.
// [ 3] CONCERN: repeated deallocations are detected/reported.
// [ 3] CONCERN: an invalid cached length is reported.
// [ 3] CONCERN: deallocated memory is scribbled.
// [ 3] CONCERN: memory leaks (byte/block) are detected/reported.
// [ 7] CONCERN: memory allocation list is kept track of properly.
// [ 8] CONCERN: cross allocation/deallocation is detected immediately.
// [ 9] CONCERN: `std::bad_alloc` is thrown if `malloc` fails.
// [10] CONCERN: over and underruns are properly caught.
// [12] CONCERN: `allocate` and `deallocate` are thread-safe.
// [13] CONCERN: `allocate` obtains properly aligned memory.
// [14] CONCERN: 1:1 blocks-in-use correspondence with upstream allocator
// [15] CONCERN: Exception neutrality

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bslma::TestAllocator Obj;

#if 0
// This is copied from `bslma_testallocator.cpp` to compare with scribbled
// deallocated memory.
// Accessing deallocated memory can result in errors on some platforms.
// For this reason, this part of the test has been removed for now.
const unsigned char SCRIBBLED_MEMORY = 0xA5;   // byte used to scribble
                                               // deallocated memory
#endif

enum { PADDING_SIZE = sizeof(bsls::AlignmentUtil::MaxAlignedType) };
                                                    // size of the padding
                                                    // before and after the
                                                    // user segment

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

extern "C" {
    typedef void *(*ThreadFunction)(void *arg);
}

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// This class meets the requirements for the left-hand side of
/// `operator<<`, where the right-hand side has type `TestAllocator`.  The
/// contents of the stream are stored in an internal buffer whose size is
/// specified by template-parameter `SIZE`.
template <std::size_t SIZE = 1024>
class BufferStream {

    // DATA
    char  d_buffer[SIZE];
    char *d_next_p;       // next write position
    bool  d_overflow;     // true if overflow has occurred

    // NOT COPYABLE
    BufferStream(const BufferStream&);
    BufferStream& operator=(const BufferStream&);

  public:
    // CREATORS
    BufferStream() : d_next_p(d_buffer), d_overflow(false)
        { std::memset(d_buffer, '\0', SIZE); }

    // ACCESSORS
    const char *data()      const { return d_buffer; }
    bool        overflow()  const { return d_overflow; }
    std::size_t remaining() const { return SIZE - (d_next_p - d_buffer) - 1; }

    /// Return `true` if the buffer matches the specified `EXP` string, else
    /// `false`.
    bool operator==(const char* EXP) const
        { return 0 == strcmp(d_buffer, EXP); }

    // MANIPULATORS

    /// Append the specified `text` of the specified `len` to the internal
    /// buffer.
    void write(const char *text, std::size_t len);
};

template <std::size_t SIZE>
void BufferStream<SIZE>::write(const char *text, std::size_t len)
{
    if (len > remaining()) {
        d_overflow = true;
        len = remaining();  // truncate
    }

    // Buffer was filled with nul characters on construction and we never
    // overwrite the last byte, so the `d_buffer` string is always
    // nul-terminated.
    memcpy(d_next_p, text, len);
    d_next_p += len;
}

ThreadId createThread(ThreadFunction func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, func, arg);
    return id;
#endif
}

void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

}  // close unnamed namespace

namespace TestCase13 {

struct ThreadInfo {
    int  d_numIterations;
    Obj *d_obj_p;
};

extern "C" void *threadFunction1(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 2;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p = mX.allocate(n);  std::memset(p, 0xff, n);
        mX.deallocate(p);

        if (n > 10000) {
            n = 2;
        }
        else {
            n *= 2;
        }
    }

    return arg;
}

extern "C" void *threadFunction2(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 3;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      std::memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 2);  std::memset(p2, 0xff, n * 2);
        mX.deallocate(p1);
        mX.deallocate(p2);

        if (n > 10000) {
            n = 3;
        }
        else {
            n *= 3;
        }
    }

    return arg;
}

extern "C" void *threadFunction3(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 5;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      std::memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 3);  std::memset(p2, 0xff, n * 3);
        void *p3 = mX.allocate(n * 7);  std::memset(p3, 0xff, n * 7);
        mX.deallocate(p3);
        mX.deallocate(p2);
        mX.deallocate(p1);

        if (n > 10000) {
            n = 5;
        }
        else {
            n *= 5;
        }
    }

    return arg;
}

}  // close namespace TestCase13

//-----------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//-----------------------------------------------------------------------------

namespace {

int globalNewCalledCount = 0;
int globalNewCalledCountIsEnabled = 0;

int globalDeleteCalledCount = 0;
int globalDeleteCalledCountIsEnabled = 0;

}  // close unnamed namespace

#if defined(BDE_BUILD_TARGET_EXC) && \
   !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        printf ("global new called, count = %d: %p\n",
                globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
# if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
void operator delete(void *address) throw()
# else
void operator delete(void *address) noexcept
# endif
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        printf("global delete freeing: %p\n", address);
    }

    free(address);
}


#ifdef BDE_BUILD_TARGET_EXC
# if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
void operator delete(void *address, size_t) throw()
# else
/// Trace use of global operator delete.
void operator delete(void *address, size_t) noexcept
# endif
#else
void operator delete(void *address, size_t)
#endif
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        printf("global sized delete freeing: %p\n", address);
    }

    free(address);
}

namespace testCase14 {

                   // ===============================
                   // union MaxAlignedAllocatorBuffer
                   // ===============================

/// Maximally-aligned raw buffer big enough for a misaligned allocation
/// when it using as the underlying allocator of a `bslma::TestAllocator`.
union MaxAlignedAllocatorBuffer {

    char                                d_data[2 * 1024];
    bsls::AlignmentUtil::MaxAlignedType d_alignment;
};


                    // =============================
                    // class NaturallyAlignAllocator
                    // =============================

/// This is a mechanism that implements an allocator that allocates memory
/// with the minimum necessary alignment (always) and is able to allocate a
/// single block only in addition to the imp detail allocations the current
/// `TestAllocator` implementation does.  Note that this type is used as an
/// underlying allocator test that `bslma::TestAllocator` manipulates the
/// allocated size successfully to coerce the underlying allocator (even if
/// that provides only the weakest necessary fundamental alignment guarantee
/// for the allocated size, also called natural alignment) to allocate
/// memory properly aligned for the requirements of the `TestAllocator`.
class NaturallyAlignAllocator : public bslma::Allocator {

  private:
    // PRIVATE CONSTANTS
    static const size_type k_MAX_ALIGN;      // Strictest fundamental alignment
    static const size_type k_MAX_ALLOCS = 3; // Max supported allocated blocks

    // DATA
    MaxAlignedAllocatorBuffer d_buffers[k_MAX_ALLOCS];  // Allocations
    bool                      d_occupied[k_MAX_ALLOCS]; // Which buffer is used

    // PRIVATE CLASS METHODS

    /// Return `true` if the specified `address` points into the specified
    /// `buffer`; and return `false` otherwise.
    static bool isInBuffer(const MaxAlignedAllocatorBuffer&  buffer,
                           void                             *address);

    // PRIVATE MANIPULATORS

    /// Return the pointer to the beginning of the internal buffer at the
    /// specified `index`.
    char *buffData(size_type index);

    /// Return a pointer closest to the beginning of the next internal
    /// buffer to be used such as that it is aligned just strict enough for
    /// the specified `size`, with a fundamental alignment.
    char *allocPtr(size_type size);

    // PRIVATE ACCESSORS

    /// Return the pointer to the beginning of the next internal buffer to
    /// be used.
    size_type nextBuffer() const;

  public:
    // CREATORS

    /// Create a `NaturallyAlignAllocator` object.
    NaturallyAlignAllocator();

    // MANIPULATORS

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive `size` (in bytes).  If `size` is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a `std::bad_alloc`
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// `0 <= size`.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of `size`.  Note
    /// that this allocator is for testing only and cannot allocate more
    /// than 1 block of memory at once.
    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified `address` back to this
    /// allocator.  If `address` is 0, this function has no effect.  The
    /// behavior is undefined unless `address` was allocated using this
    /// allocator object and has not already been deallocated.
    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
};

                    // -----------------------------
                    // class NaturallyAlignAllocator
                    // -----------------------------

// PRIVATE CONSTANTS
const NaturallyAlignAllocator::size_type NaturallyAlignAllocator::k_MAX_ALIGN =
                bsls::AlignmentUtil::calculateAlignmentFromSize(
                                  sizeof(bsls::AlignmentUtil::MaxAlignedType));

// PRIVATE CLASS METHODS
inline
bool
NaturallyAlignAllocator::isInBuffer(const MaxAlignedAllocatorBuffer&  buffer,
                                    void                             *address)
{
    return address >= buffer.d_data &&
           address < buffer.d_data + sizeof(buffer.d_data);
}

// PRIVATE MANIPULATORS
inline
char *NaturallyAlignAllocator::allocPtr(size_type size)
{
    typedef bsls::AlignmentUtil Util;
    const size_type a = Util::calculateAlignmentFromSize(size);
    const size_type bufPos = nextBuffer();
    d_occupied[bufPos] = true;

    if (a < k_MAX_ALIGN) {
      return buffData(bufPos) + a;                                    // RETURN
    }

    return buffData(bufPos);
}

inline
char *NaturallyAlignAllocator::buffData(size_type index)
{
    return d_buffers[index].d_data;
}

// PRIVATE ACCESSORS
inline
NaturallyAlignAllocator::size_type NaturallyAlignAllocator::nextBuffer() const
{
    for (size_type i = 0; i < k_MAX_ALLOCS; ++i) {
        if (!d_occupied[i]) {
            return i;                                                 // RETURN
        }
    }

    bsls::BslExceptionUtil::throwBadAlloc();

    // Eliminate compiler warnings regarding non-return:
    return 0;
}

// CREATORS
inline
NaturallyAlignAllocator::NaturallyAlignAllocator()
{
    for (size_type i = 0; i < k_MAX_ALLOCS; ++i) {
        d_occupied[i] = false;
    }
}

// MANIPULATORS
void *NaturallyAlignAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    return allocPtr(size);
}

void NaturallyAlignAllocator::deallocate(void *address)
{
    if (0 == address) {
        return;                                                       // RETURN
    }

    for (size_type i = 0; i < k_MAX_ALLOCS; ++i) {
        if (d_occupied[i] && isInBuffer(d_buffers[i], address)) {
            d_occupied[i] = false;
            return;                                                   // RETURN
        }
    }

    BSLS_ASSERT(0 == "Bad address to deallocate!");
}

}  // close namespace testCase14

namespace {

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_shortarray.h

class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the `d_array_p` array (elements)
    int d_length;     // logical length of the `d_array_p` array (elements)
    bslma::Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS

    /// Create an empty array.  Optionally specify a `basicAllocator` used
    /// to supply memory.  If `basicAllocator` is 0, global operators `new`
    /// and `delete` are used.
    explicit my_ShortArray(bslma::Allocator *basicAllocator = 0);
     // ...

    ~my_ShortArray();
    void append(short value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
    operator const short *() const { return d_array_p; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.cpp

my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    size_t sz = d_size * sizeof *d_array_p;
    if (basicAllocator) {
        d_array_p = (short *) d_allocator_p->allocate(sz);
    }
    else {
        d_array_p = (short *) operator new(sz);
    }
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    if (d_allocator_p) {
        d_allocator_p->deallocate(d_array_p);
    }
    else {
        operator delete(d_array_p);
    }
}

inline
void my_ShortArray::append(short value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

/// Reallocate memory in the specified `array` to the specified `newSize`
/// using the specified `basicAllocator`, or, if `basicAllocator` is 0,
/// global operators `new` and `delete`.  The specified `length` number of
/// leading elements are preserved.  Since the class invariant requires
/// that the physical capacity of the container may grow but never shrink;
/// the behavior is undefined unless `length <= newSize`.
void reallocate(short            **array,
                int                newSize,
                int                length,
                bslma::Allocator  *basicAllocator)
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= newSize);          // enforce class invariant

    short *tmp = *array;                // support exception neutrality
    size_t sz = newSize * sizeof **array;
    if (basicAllocator) {
        *array = (short *) basicAllocator->allocate(sz);
    }
    else {
        *array = (short *) operator new(sz);
    }

    // COMMIT POINT

    std::memcpy(*array, tmp, length * sizeof **array);

    if (basicAllocator) {
        basicAllocator->deallocate(tmp);
    }
    else {
        operator delete(tmp);
    }
}

void my_ShortArray::increaseSize()
{
    int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
    ASSERT(proposedNewSize > d_length);
    reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
    d_size = proposedNewSize;                        // we're committed
}

void debugprint(const my_ShortArray& array)
{
    printf("[");
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        printf(" %d", array[i]);
    }
    printf(" ]");
    fflush(stdout);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.t.cpp

///Usage
///-----
// The `bslma::TestAllocator` defined in this component can be used in
// conjunction with the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` and
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macros to test the memory usage
// patterns of an object that uses the `bslma::Allocator` protocol in its
// interface.  In this example, we illustrate how we might test that an object
// under test is exception-neutral.  For illustration purposes, we will assume
// the existence of a `my_shortarray` component implementing an
// `std::vector`-like array type, `myShortArray`:
// ```
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//
//  #include <bslma_testallocator.h>
//  #include <bslma_testallocatorexception.h>
//
//  // ...
//
// ```
// Below we provide a `static` function, `areEqual`, that will allow us to
// compare two short arrays:
// ```

    /// Return `true` if the specified initial `numElements` in the
    /// specified `array1` and `array2` have the same values, and `false`
    /// otherwise.
    static
    bool areEqual(const short *array1, const short *array2, int numElements)
    {
        for (int i = 0; i < numElements; ++i) {
            if (array1[i] != array2[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }
//
//  // ...
//
// ```

/// Call the `print` method on the specified `ta` test allocator and capture
/// its output to the specified `buf` of size `sz`.  If the output is
/// truncated (if necessary) to `sz - 1` bytes, and is always nul
/// terminated within `buf`.
void printToBuffer(char *buf, std::size_t sz, const bslma::TestAllocator& ta)
{
    // Create a temporary file
    std::FILE* tmpf = 0;
#ifdef BSLS_PLATFORM_OS_WINDOWS
    errno_t e = tmpfile_s(&tmpf);
    BSLS_ASSERT(0 == e);
#else
    tmpf = tmpfile();
#endif
    BSLS_ASSERT(tmpf);

    // Print to temporary file
    ta.print(tmpf);

    // Read what was just written from temp file into `buf`
    std::rewind(tmpf);
    buf[sz - 1] = '\0';        // Ensure nul termination, no matter what.
    std::size_t count = std::fread(buf, 1, sz - 1, tmpf);
    BSLS_ASSERT(count < sz); // sanity check
    buf[count] = '\0';       // Add nul terminator.

    BSLS_ASSERT('\0' == buf[sz - 1]);  // overrun check

    std::fclose(tmpf);
}

/// Return `true` if the in-use block list for the specified `ta` test
/// allocator exactly matches the set of non-null pointers in the specified
/// `blocks` array and `false` otherwise.  The expected ID for any non-null
/// pointer is its index within the `blocks` array, so if `blocks[4]` is
/// non-null, then ID '4' is expected to be in the blocks list and if
/// `blocks[5]` is null, then ID '5' is expected *not* to be in the blocks
/// list.
template <std::size_t NBLOCKS>
bool verifyBlockList(const bslma::TestAllocator&    ta,
                     void                        *(&blocks)[NBLOCKS])
{
    // There is no accessor to obtain the in-use block list for a
    // `TestAllocator`, so the only way get it is to print to a string and
    // parse the string.
    BufferStream<> strm;
    strm << ta;
    BSLS_ASSERT(! strm.overflow());

    // Construct a boolean array with `true` on each block present in the block
    // list.
    bool ids[NBLOCKS] = { };  // initialized to all false

    // Find start of in-use block list
    const char *p = strm.data();
    p = std::strstr(p, "Outstanding Memory Allocations:");
    while (p && *p) {
        p += std::strcspn(p, "0123456789");  // Skip non-digits
        int id;
        std::size_t count = sscanf(p, "\t%d", &id);
        if (count < 1) {
            break;  // No more IDs.
        }

        BSLS_ASSERT(0 <= id && id < int(NBLOCKS));
        ids[id] = true;

        p += std::strspn(p, "0123456789");  // Skip digits
    }

    // Traverse both arrays looking for a mismatch
    for (size_t id = 0; id < NBLOCKS; ++id) {
        if (ids[id] != bool(blocks[id])) {
            return false;                                             // RETURN
        }
    }

    return true;
}

/// A guard object that stashes and restores test allocator statistics to make
/// verification code easier on the eyes, and closer to what it the final use
/// will look like.  See also `bslma::TestAllocatorStatisticsGuard`.  Notice
/// that this object is a `struct` to indicate that it is not meant for
/// production use and to allow access to the stashed data in case it is needed
/// in verification.
struct StatisticsStashGuard {
    // DATA
    const bslma::TestAllocatorStashedStatistics d_stash;

    // CREATORS
    StatisticsStashGuard(bslma::TestAllocator *ta)
    : d_stash(ta->stashStatistics())
    {
    }

    ~StatisticsStashGuard()
    {
        d_stash.restore();
    }
};

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // Additional code for usage test:
    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    switch (test) { case 0:
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, and replace `assert` with
        //    `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        typedef short Element;
        const Element VALUES[] = { 1, 2, 3, 4, -5 };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const Element &V0 = VALUES[0],
                      &V1 = VALUES[1],
                      &V2 = VALUES[2],
                      &V3 = VALUES[3],
                      &V4 = VALUES[4];

// The following is an abbreviated standard test driver.  Note that the number
// of arguments specify the verbosity level that the test driver uses for
// printing messages:
// ```
//  int main(int argc, char *argv[])
//  {
//      int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
//      bool             verbose = argc > 2;
//      bool         veryVerbose = argc > 3;
//      bool     veryVeryVerbose = argc > 4;
//      bool veryVeryVeryVerbose = argc > 5;
//
// ```
// We now define a `bslma::TestAllocator`, `sa`, named "supplied" to indicate
// that it is the allocator to be supplied to our object under test, as well as
// to the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` macro (below).  Note that
// if `veryVeryVeryVerbose` is `true`, then `sa` prints all allocation and
// deallocation requests to `stdout` and also prints the accumulated statistics
// on destruction:
// ```
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

//      switch (test) { case 0:
//
//        // ...
//
//        case 6: {
//
//          // ...
//
            struct {
                int   d_line;
                int   d_numElem;
                short d_exp[NUM_VALUES];
            } DATA[] = {
                { L_, 0, { } },
                { L_, 1, { V0 } },
                { L_, 5, { V0, V1, V2, V3, V4 } }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const int    NUM_ELEM = DATA[ti].d_numElem;
                const short *EXP      = DATA[ti].d_exp;

                if (veryVerbose) { T_ P_(ti) P_(NUM_ELEM) }

                // ...

// ```
// All code that we want to test for exception-safety must be enclosed within
// the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` and
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macros, which internally implement
// a `do`-`while` loop.  Code provided by the
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` macro sets the allocation limit
// of the supplied allocator to 0 causing it to throw an exception on the first
// allocation.  This exception is caught by code provided by the
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macro, which increments the
// allocation limit by 1 and re-runs the same code again.  Using this scheme we
// can check that our code does not leak memory for any memory allocation
// request.  Note that the curly braces surrounding these macros, although
// visually appealing, are not technically required:
// ```
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                  my_ShortArray mA(&sa);
                  const my_ShortArray& A = mA;
                  for (int ei = 0; ei < NUM_ELEM; ++ei) {
                      mA.append(VALUES[ei]);
                  }
                  if (veryVerbose) { T_ T_  P_(NUM_ELEM) P(A) }
                  LOOP_ASSERT(LINE, areEqual(EXP, A, NUM_ELEM));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }

// ```
// After the exception-safety test we can ensure that all the memory allocated
// from `sa` was successfully deallocated:
// ```
            if (veryVerbose) sa.print();
//
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
// ```
// Note that the `BDE_BUILD_TARGET_EXC` macro is defined at compile-time to
// indicate whether or not exceptions are enabled.

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // DRQS 129104858
        //   Ensure that the unification of the tracing strings do not change
        //   the output in the case of single thread.
        //
        // Concerns:
        // 1. The changes in DRQS 129104858 do not change the output for a
        //    single thread run.
        //
        // Plan:
        // 1. Output the old code into a string.  Output the new code into a
        //    string.  Assert that both strings are equal.  (C-1)
        //
        // Testing:
        //   DRQS 129104858
        // --------------------------------------------------------------------

        if (verbose) printf("\nDRQS 129104858"
                            "\n==============\n");

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU  // An alias for a string that can be
                                       // treated as the "%zu" format
                                       // specifier (MSVC issue).

        typedef bslma::Allocator::size_type size_type;

        struct {
            int                 d_line;
            const char*         d_name_p;
            void               *d_address;
            size_type           d_size;
            bsls::Types::Int64  d_allocationIndex;
        } DATA[] = {
            { L_, "test1", (void*)0x234567, 2, 12 },
            { L_, ""     , (void*)0x234568, 3, 15 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                 LINE     = DATA[ti].d_line;
            const char               *NAME     = DATA[ti].d_name_p;
            const void               *ADDRESS  = DATA[ti].d_address;
            const size_type           SIZE     = DATA[ti].d_size;
            const bsls::Types::Int64  ALLOCIDX = DATA[ti].d_allocationIndex;

            if (veryVerbose) {
                T_ P_(LINE) P_(ti) P_(NAME) P_(ADDRESS) P_(SIZE) P(ALLOCIDX)
            }

            char preBuf[240];
            char postBuf[240];

            std::strcpy(preBuf, "TestAllocator");

            if (NAME) {
                std::strcat(preBuf, " ");
                std::strcat(preBuf, NAME);
            }

            std::sprintf(preBuf + std::strlen(preBuf),
                         " [%lld]: Deallocated " ZU " byte%sat %p.\n",
                         ALLOCIDX,
                         SIZE,
                         1 == SIZE ? " " : "s ", ADDRESS);

            std::sprintf(
                 postBuf,
                 "TestAllocator%s%s [%lld]: Deallocated " ZU " byte%sat %p.\n",
                 NAME ? " " : "",
                 NAME ? NAME : "",
                 ALLOCIDX,
                 SIZE,
                 1 == SIZE ? " " : "s ",
                 ADDRESS);

            LOOP1_ASSERT(ti, 0 == std::strcmp(preBuf, postBuf));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // STASHING STATISTICS
        //
        // Concerns:
        // 1. Statistics are stashed and reset as expected (see contract).
        //
        // 2. Maximum are unchanged if in the scope of stash-restore they are
        //    not exceeded, but they are set to the scope's maximum if those
        //    values are larger.
        //
        // 3. All the other statistics values are restored properly.
        //
        // 4. Negative testing:
        //
        //    1. Attempt to restore statistics stashed from test allocator A to
        //       a test allocator B will assert in proper build modes.
        //
        //    2. Attempt to restore statistics twice will assert in proper
        //       build modes.
        //
        // Plan:
        // 1. Create a `TestAllocator` on which the methods will be called.
        //
        // 2. Allocate and deallocate memory to create a situation where the
        //    maximums are larger than the in-use values.
        //
        // 3. Use the `StatisticsGuard` helper class to stash and restore
        //    statistics for a scope.  In that scope
        //
        //    1. Verify that the statistics have been reset as expected.
        //
        //    2. Allocate and deallocate memory that does not raise the stashed
        //       maximums.
        //
        // 5. After the scope verify that statistics are restored correctly.
        //
        // 6. Use the `StatisticsGuard` helper class to stash and restore
        //    statistics for a scope.  In that scope
        //
        //    1. Allocate and deallocate memory that should raise the stashed
        //       maximums.  Use a nested scope with another guard to ensure
        //       nesting works.
        //
        // 7. After the scope end verify that the maximums are not overwritten
        //    by the stashed lower values.
        //
        // 8. Negative testing, if build modes where exceptions and asserts are
        //    enabled:
        //
        //    1. Create two test allocators `ta` and `tb`.  Verify that
        //       statistics stashed from `ta` won't restore into `tb` due to
        //       assertion failure.
        //
        //    2. Create a test allocators `ta`, stash its statistics, restore
        //       it then verify that it won't restore a second time due to
        //       assertion failure.
        //
        // Testing:
        //   TestAllocatorStashedStatistics stashStatistics();
        //   void restoreStatistics(const TestAllocatorStashedStatistics&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nSTASHING STATISTICS"
                            "\n===================\n");

        bslma::TestAllocator mX;

        // Step 1, setting up maximums above the in-use values
        void* p1 = mX.allocate(1024);
        void* p2 = mX.allocate(1024);
        void* p3 = mX.allocate(1024);
        mX.deallocate(mX.allocate(1024));
        mX.deallocate(p3);

        ASSERTV(mX.numAllocations(),   4 == mX.numAllocations());
        ASSERTV(mX.numDeallocations(), 2 == mX.numDeallocation());

        ASSERTV(mX.numBlocksTotal(),   4 == mX.numBlocksTotal());
        ASSERTV(mX.numBytesTotal(), 4096 == mX.numBytesTotal());

        ASSERTV(mX.numMismatches(),   0 == mX.numMismatches());
        ASSERTV(mX.numBoundsErrors(), 0 == mX.numBoundsErrors());

        ASSERTV(mX.numBlocksInUse(),   2 == mX.numBlocksInUse());
        ASSERTV(mX.numBytesInUse(), 2048 == mX.numBytesInUse());

        ASSERTV(mX.numBlocksMax(),   4 == mX.numBlocksMax());
        ASSERTV(mX.numBytesMax(), 4096 == mX.numBytesMax());

        if (veryVerbose) puts("Scope maximums are smaller than stashed");
        {
            StatisticsStashGuard guard(&mX);
            const bslma::TestAllocatorStashedStatistics& saved = guard.d_stash;

            // Totals are the in-use values after resetting
            ASSERTV(mX.numAllocations(),
                    mX.numBlocksInUse() == mX.numAllocations());
            ASSERTV(mX.numDeallocations(),
                    0 == mX.numDeallocation());
            ASSERTV(mX.numBlocksTotal(),
                    mX.numBlocksInUse() == mX.numBlocksTotal());
            ASSERTV(mX.numBytesTotal(),
                    mX.numBytesInUse() == mX.numBytesTotal());

            // Errors are set to zero
            ASSERTV(mX.numMismatches(),   0 == mX.numMismatches());
            ASSERTV(mX.numBoundsErrors(), 0 == mX.numBoundsErrors());

            // Maximums are the in-use values after reset
            ASSERTV(mX.numBlocksMax(),
                    mX.numBlocksInUse() == mX.numBlocksMax());
            ASSERTV(mX.numBytesMax(),
                    mX.numBytesInUse() == mX.numBytesMax());

            // Saved maximums are the original values
            ASSERTV(saved.numBlocksMax(),   4 == saved.numBlocksMax());
            ASSERTV(saved.numBytesMax(), 4096 == saved.numBytesMax());

            // Allocate and deallocate memory but keep the maximums below the
            // stashed values (so we need to restore them)
            mX.deallocate(mX.allocate(1024));

            ASSERTV(mX.numAllocations(),
                    mX.numBlocksInUse() + 1 == mX.numAllocations());
            ASSERTV(mX.numDeallocations(),
                    1 == mX.numDeallocation());
            ASSERTV(mX.numBlocksTotal(),
                    mX.numBlocksInUse() + 1 == mX.numBlocksTotal());
            ASSERTV(mX.numBytesTotal(),
                    mX.numBytesInUse() + 1024 == mX.numBytesTotal());

            // No errors
            ASSERTV(mX.numMismatches(),   0 == mX.numMismatches());
            ASSERTV(mX.numBoundsErrors(), 0 == mX.numBoundsErrors());

            // These are our lower, "scoped" maximums
            ASSERTV(mX.numBlocksMax(),   3 == mX.numBlocksMax());
            ASSERTV(mX.numBytesMax(), 3072 == mX.numBytesMax());

        }  // Statistics are restored here by the guard

        // Restore should make the "reset disappear"
        ASSERTV(mX.numAllocations(),   5 == mX.numAllocations());
        ASSERTV(mX.numBytesTotal(), 5120 == mX.numBytesTotal());

        ASSERTV(mX.numBlocksMax(),   4 == mX.numBlocksMax());
        ASSERTV(mX.numBytesMax(), 4096 == mX.numBytesMax());

        if (veryVerbose) puts("Scope overrides stashed maximums and nesting");
        {
            StatisticsStashGuard guard(&mX);
            const bslma::TestAllocatorStashedStatistics& saved = guard.d_stash;

            // Allocate and deallocate memory to raise the maximum above the
            // stashed maximums
            p3 = mX.allocate(1024);

            if (veryVerbose) puts("Nested scope/stash-restore");
            {
                StatisticsStashGuard guard(&mX);

                void *p4 = mX.allocate(1024);
                mX.deallocate(mX.allocate(1024));
                mX.deallocate(p4);
            }
            mX.deallocate(p3);

            // Running totals and the maximums are the same
            ASSERTV(mX.numAllocations(),   5 == mX.numAllocations());
            ASSERTV(mX.numBytesTotal(), 5120 == mX.numBytesTotal());

            // Maximums are larger than the stashed values
            ASSERTV(mX.numBlocksMax(),   5 == mX.numBlocksMax());
            ASSERTV(mX.numBytesMax(), 5120 == mX.numBytesMax());

        }  // Statistics are restored here by the guard

        // Restore should *not* change the larger maximum values back to the
        // stashed values
        ASSERTV(mX.numBlocksMax(),   5 == mX.numBlocksMax());
        ASSERTV(mX.numBytesMax(), 5120 == mX.numBytesMax());

        // Avoid memory leaks
        mX.deallocate(p2);
        mX.deallocate(p1);

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) puts("Negative testing");
        {
            bslma::TestAllocator ta;
            bslma::TestAllocator tb;

            const bslma::TestAllocatorStashedStatistics stash =
                                                          ta.stashStatistics();
            bsls::AssertTestHandlerGuard hG;
            ASSERT_FAIL(tb.restoreStatistics(stash));
            ASSERT_PASS(ta.restoreStatistics(stash));
            ASSERT_FAIL(ta.restoreStatistics(stash));
        }
#endif
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // CONCERN: Exception Neutrality
        //   Verify that `allocate` propagates exceptions from the upstream
        //   allocator and does not leak resources.
        //
        // Concerns:
        // 1. If `allocate` causes the upstream allocator to throw an
        //    exception, that exception is propagated to the caller.
        // 2. No resources are leaked if the upstream allocator throws.
        //
        // Plan:
        // 1. Create a `TestAllocator` to use as an upstream allocator.
        // 2. Within a `BSLMA_TESTALLOCATOR_EXCEPTION_TEST` loop:
        //   1. Construct a second `TestAllocator` that gets memory from the
        //      upstream allocator.
        //   2. Allocate a few bytes from the second `TestAllocator`.
        //   3. Deallocate the allocated bytes.
        //   4. Perform one more allocation and deallocation.
        // 3. Verify that no allocated blocks remain allocated from the
        //    upstream allocator once the exception loop succeeds.  (C-2)
        // 4. Verify that the exception loop ran more than one iteration,
        //    indicating that at least one exception occurred.  (C-1)
        //
        // Testing:
        //   CONCERN: Exception Neutrality
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCERN: Exception Neutrality"
                            "\n=============================\n");

#ifdef BDE_BUILD_TARGET_EXC

        bslma::TestAllocator upstream("upstream", veryVeryVeryVerbose);
        int iterations = 0;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(upstream) {
            ++iterations;
            bslma::TestAllocator ta("excTest", veryVeryVeryVerbose, &upstream);

            void *p = ta.allocate(13);  // Might throw from upstream allocator
            ASSERT(p);
            ta.deallocate(p);

            p = ta.allocate(15);        // Might throw from upstream allocator
            ASSERT(p);
            ta.deallocate(p);

            // If got here, then upstream allocator did not throw. Exception
            // loop will end.
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

        ASSERTV(upstream.numBlocksInUse(), 0 == upstream.numBlocksInUse());
        ASSERTV(upstream.numBlocksTotal(), 0 < upstream.numBlocksTotal());
        ASSERTV(iterations, 1 < iterations);

#else
        if (verbose) printf("\nNo testing.  Exceptions are not enabled.\n");
#endif // BDE_BUILD_TARGET_EXC

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // CONCERN: 1:1 Correspondence w/ Upstream Allocator
        //   Ensure that allocations and deallocations correspond 1:1 with
        //   requests from the upstream allocator.
        //
        // Concerns:
        // 1. Every allocation from a `TestAllocator` results in a single
        //    allocation from its upstream allocator (though the size and
        //    alignment might differ).
        // 2. Every deallocation from a `TestAllocator` results in a single
        //    deallocation from its upstream allocator.
        // 3. The number of bytes allocated from the upstream allocator is
        //    never less than the number of bytes allocated from the test
        //    allocator (but might be more).
        // 4. If multiple `TestAllocator` objects use the same upstream
        //    allocator, the total number of blocks allocated from the upstream
        //    allocator is the sum of the number of blocks allocated from the
        //    individual test allocators.
        //
        // Concerns:
        // 1. Construct a pair of `TestAllocator` objects to act as upstream
        //    allocators.
        // 2. Construct three downstream `TestAllocator` objects, two from one
        //    of the upstream allocators and one from the other.
        // 3. Loop through two lists of `A` and `D` characters, representing
        //    allocate and deallocate operations, such that each operation
        //    is chosen at random, but there are never more deallocations than
        //    allocations and the total is balanced.  For each operation:
        //   1. Perform the specified operation (allocate or deallocate) on the
        //      downstream allocators.
        //   2. Verify that the `numAllocations` count for each upstream
        //      allocator is the sum of the `numAllocations` for its downstream
        //      allocators.  (C-1, C-4)
        //   3. Verify that the `numDeallocations` count for each upstream
        //      allocator is the sum of the `numDeallocations` for its
        //      downstream allocators.  (C-2, C-4)
        //   4. Verify that the `numBlocksInUse` count for each upstream
        //      allocator is the sum of the `numBlocksInUse` for its downstream
        //      allocators.  (C-1, C-2, C-4)
        //   5. Verify that the `numBytesInUse` count for each upstream
        //      allocator is no smaller than the sum of the `numBytesInUse` for
        //      its downstream allocators.  (C-3, C-4)
        //
        // Testing
        //   CONCERN: 1:1 blocks-in-use correspondence with upstream allocator
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCONCERN: 1:1 Correspondence w/ Upstream Allococator"
                   "\n===================================================\n");

        const char ops1[] = "AADADAAAADDDDAADDADDADAADDADADADADAADDAD";
        const char ops2[] = "AAAAADDDADADADAAADDADDDAAADADADDAADADDDD";
        const std::size_t k_NUM_OPS = sizeof(ops1) - 1;

        bslma::TestAllocator us1("upstream1", veryVeryVeryVerbose);
        bslma::TestAllocator us2("upstream2", veryVeryVeryVerbose);
        bslma::TestAllocator ds1("downstream1", veryVeryVeryVerbose, &us1);
        bslma::TestAllocator ds2("downstream2", veryVeryVeryVerbose, &us2);
        bslma::TestAllocator ds3("downstream3", veryVeryVeryVerbose, &us2);

        void *blocks1[k_NUM_OPS/2];
        void *blocks2[k_NUM_OPS/2];
        void *blocks3[k_NUM_OPS/2];

        int nBlocks1 = 0, nBlocks2 = 0, nBlocks3 = 0;

        for (std::size_t i = 0; i < k_NUM_OPS; ++i) {
            if ('A' == ops1[i]) {
                blocks1[nBlocks1++] = ds1.allocate(i + 1);
                blocks2[nBlocks2++] = ds2.allocate(i + 1);
            }
            else {
                BSLS_ASSERT('D' == ops1[i]);
                BSLS_ASSERT(nBlocks1 > 0);
                BSLS_ASSERT(nBlocks2 > 0);
                ds1.deallocate(blocks1[--nBlocks1]);
                ds2.deallocate(blocks2[--nBlocks2]);
            }
            ASSERTV(us1.numAllocations(), ds1.numAllocations(),
                    us1.numAllocations() == ds1.numAllocations());
            ASSERTV(us2.numAllocations(),
                    ds2.numAllocations(), ds3.numAllocations(),
                    us2.numAllocations() == (ds2.numAllocations() +
                                             ds3.numAllocations()));
            ASSERTV(us1.numDeallocations(),
                    ds1.numDeallocations(),
                    us1.numDeallocations() == ds1.numDeallocations());
            ASSERTV(us2.numDeallocations(),
                    ds2.numDeallocations(), ds3.numDeallocations(),
                    us2.numDeallocations() == (ds2.numDeallocations() +
                                               ds3.numDeallocations()));
            ASSERTV(nBlocks1, ds1.numBlocksInUse(),
                    nBlocks1 == ds1.numBlocksInUse());
            ASSERTV(nBlocks2, ds2.numBlocksInUse(),
                    nBlocks2 == ds2.numBlocksInUse());
            ASSERTV(nBlocks1, us1.numBlocksInUse(),
                    nBlocks1 == us1.numBlocksInUse());
            ASSERTV(nBlocks2 + nBlocks3, us2.numBlocksInUse(),
                    nBlocks2 + nBlocks3 == us2.numBlocksInUse());
            ASSERTV(us1.numBytesInUse(), ds1.numBytesInUse(),
                    us1.numBytesInUse() >= ds1.numBytesInUse());
            ASSERTV(us2.numBytesInUse(),
                    ds2.numBytesInUse(), ds3.numBytesInUse(),
                    us2.numBytesInUse() >= (ds2.numBytesInUse() +
                                            ds3.numBytesInUse()));

            if ('A' == ops2[i]) {
                blocks3[nBlocks3++] = ds3.allocate(i + 1);
            }
            else {
                BSLS_ASSERT('D' == ops2[i]);
                BSLS_ASSERT(nBlocks3 > 0);
                ds3.deallocate(blocks3[--nBlocks3]);
            }
            ASSERTV(us2.numAllocations(),
                    ds2.numAllocations(), ds3.numAllocations(),
                    us2.numAllocations() == (ds2.numAllocations() +
                                             ds3.numAllocations()));
            ASSERTV(us2.numDeallocations(),
                    ds2.numDeallocations(), ds3.numDeallocations(),
                    us2.numDeallocations() == (ds2.numDeallocations() +
                                               ds3.numDeallocations()));
            ASSERTV(nBlocks3, ds3.numBlocksInUse(),
                    nBlocks3 == ds3.numBlocksInUse());
            ASSERTV(nBlocks2 + nBlocks3, us2.numBlocksInUse(),
                    nBlocks2 + nBlocks3 == us2.numBlocksInUse());
            ASSERTV(us2.numBytesInUse(),
                    ds2.numBytesInUse(), ds3.numBytesInUse(),
                    us2.numBytesInUse() >= (ds2.numBytesInUse() +
                                            ds3.numBytesInUse()));
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ALIGNMENT
        //   Ensure that `allocate` obtains properly aligned memory.
        //
        // Concerns:
        // 1. That `allocate` obtains properly aligned memory from an
        //    underlying allocator even if that allocator provides the weakest
        //    necessary fundamental alignment only.
        //
        // Plan:
        // 1. Use a `NaturallyAlignAllocator` as the underlying allocator for
        //    the tested (test allocator) object.
        //
        // 2. Within a loop, allocate, then release sizes up to a small
        //    multiple of the size of the maximum aligned type to ensure that
        //    internally the allocation will have the proper alignment and also
        //    verify that the returned pointer is at least naturally aligned
        //    for the allocated size (which is the minimum alignment promise
        //    every allocator must make).
        //
        // Testing:
        //   CONCERN: `allocate` obtains properly aligned memory.
        // --------------------------------------------------------------------

        if (verbose) printf("\nALIGNMENT"
                            "\n=========\n");

        typedef bslma::Allocator::size_type size_type;

        static const size_type k_MAX_SIZE =
                                   sizeof(bsls::AlignmentUtil::MaxAlignedType);

        testCase14::NaturallyAlignAllocator base;
        Obj mX("tested", veryVeryVeryVerbose, &base);
        for (size_type s = 1; s <= 10 * k_MAX_SIZE; ++s) {
            if (veryVerbose) {
                P(s);
            }
            void *allocated_p = mX.allocate(s);
            typedef bsls::AlignmentUtil AlignUtil;
            const int a = AlignUtil::calculateAlignmentFromSize(s);
            ASSERT(0 == AlignUtil::calculateAlignmentOffset(allocated_p, a));
            ASSERT(allocated_p != 0);
            mX.deallocate(allocated_p);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCURRENCY
        //   Ensure that `allocate` and `deallocate` are thread-safe.
        //
        // Concerns:
        // 1. That `allocate` and `deallocate` are thread-safe.  (Note that
        //    although all methods of `bslma::TestAllocator` are thread-safe,
        //    the thread safety of `allocate` and `deallocate` are of paramount
        //    concern.)
        //
        // Plan:
        // 1. Create a `bslma::TestAllocator`.
        //
        // 2. Within a loop, create three threads that iterate a specified
        //    number of times and that perform a different sequence of
        //    allocation and deallocation operations on the test allocator from
        //    P-1.
        //
        // 3. After each iteration, use the accessors to verify the expected
        //    state of the test allocator.  (C-1)
        //
        // Testing:
        //   CONCERN: `allocate` and `deallocate` are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCURRENCY"
                            "\n===========\n");

        using namespace TestCase13;

        Obj mX("concurrent allocator", veryVeryVeryVerbose);
        const Obj& X = mX;

        const int NUM_TEST_ITERATIONS   =  10;
        const int NUM_THREAD_ITERATIONS = 500;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mX };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {
            ThreadId id1 = createThread(&threadFunction1, &info);
            ThreadId id2 = createThread(&threadFunction2, &info);
            ThreadId id3 = createThread(&threadFunction3, &info);

            joinThread(id1);
            joinThread(id2);
            joinThread(id3);

            ASSERT(0 == X.status());

            ASSERT(X.lastAllocatedAddress());
            ASSERT(X.lastDeallocatedAddress());

            ASSERT(X.numAllocations() == X.numDeallocations());

            ASSERT(0  < X.lastAllocatedNumBytes());
            ASSERT(0  < X.lastDeallocatedNumBytes());

            ASSERT(0 == X.numBlocksInUse());
            ASSERT(0  < X.numBlocksMax());
            ASSERT(0  < X.numBlocksTotal());


            ASSERT(0 == X.numBytesInUse());
            ASSERT(0  < X.numBytesMax());
            ASSERT(0  < X.numBytesTotal());

            ASSERT(0 == X.numBoundsErrors());
            ASSERT(0 == X.numMismatches());
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST PRINTING AND STREAMING
        //
        // Concerns:
        // 1. The `print` method correctly prints the allocator state including
        //    the in use, maximum, and total number of blocks and bytes.
        //
        // 2. Any outstanding allocations are mentioned.
        //
        // 3. The allocator name is printed if specified.
        //
        // 4. The method is declared `const`.
        //
        // 5. Concerns 1-4 apply for `operator<<`.
        //
        // Plan:
        // 1. Using the table-driven technique. For each row (representing a
        //    distinct attribute value, `V`):
        //
        //   1. Generate a `TestAllocator` object, `mX` having the specified
        //      attributes and a const reference `X` to `mX`.
        //
        //   2. Use `print` to format `X` to a temporary file.  (C-4)
        //
        //   3. Verify that the contents of the temporary file matches the
        //      expected string.  (C-1, C-2, C-3)
        //
        //   4. Using `strm << X`, format `X` to a stream, `strm`, that
        //      captures the output to string.  Verify that the resulting
        //      string matches the expected string.  (C-4)
        //
        // Testing:
        //   void print() const;
        //   friend t_OS& operator<<(t_OS& stream, const TestAllocator& ta);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST PRINTING AND STREAMING"
                            "\n===========================\n");

        static const char *TEMPLATE_WITH_NAME =
                        "\n"
                        "==================================================\n"
                        "                TEST ALLOCATOR %s STATE\n"
                        "--------------------------------------------------\n"
                        "        Category\tBlocks\tBytes\n"
                        "        --------\t------\t-----\n"
                        "          IN USE\t%lld\t%lld\n"
                        "             MAX\t%lld\t%lld\n"
                        "           TOTAL\t%lld\t%lld\n"
                        "      MISMATCHES\t%lld\n"
                        "   BOUNDS ERRORS\t%lld\n"
                        "--------------------------------------------------\n";

        static const char *TEMPLATE_WITHOUT_NAME =
                        "\n"
                        "==================================================\n"
                        "                TEST ALLOCATOR STATE\n"
                        "--------------------------------------------------\n"
                        "        Category\tBlocks\tBytes\n"
                        "        --------\t------\t-----\n"
                        "          IN USE\t%lld\t%lld\n"
                        "             MAX\t%lld\t%lld\n"
                        "           TOTAL\t%lld\t%lld\n"
                        "      MISMATCHES\t%lld\n"
                        "   BOUNDS ERRORS\t%lld\n"
                        "--------------------------------------------------\n";

        const int MAX_ENTRIES = 10;
        static const
        struct DefaultDataRow {
            int         d_line;                  // source line number
            const char *d_name_p;                // allocator name
            int         d_numAllocs;             // number of allocations
            int         d_allocs[MAX_ENTRIES];   // num allocation bytes
            int         d_numDeallocs;           // number of deallocations
            int         d_deallocs[MAX_ENTRIES]; // num deallocation indices
        } DATA [] = {
            {
                L_,
                0,
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                "",
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                "FIRST_ALLOCATOR",
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                0,
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                "",
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                "SecondAllocator",
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                0,
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                "",
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                "Third Allocator",
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                0,
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                "",
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                "Fourth_Allocator",
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                0,
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
            {
                L_,
                "",
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
            {
                L_,
                "finalallocator",
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE           = DATA[ti].d_line;
            const char       *NAME           = DATA[ti].d_name_p;
            const int         NUM_ALLOCS     = DATA[ti].d_numAllocs;
            const int        *ALLOCS         = DATA[ti].d_allocs;
            const int         NUM_DEALLOCS   = DATA[ti].d_numDeallocs;
            const int        *DEALLOCS       = DATA[ti].d_deallocs;
                  void       *addresses[MAX_ENTRIES];
                  int         numRemAllocs   = 0;
                  int         remAllocs[MAX_ENTRIES] = { 0 };

            (void) LINE;
            (void) ALLOCS;
            (void) DEALLOCS;

            bsls::ObjectBuffer<Obj> mXbuf;
            Obj& mX = *(NAME ? new (mXbuf.buffer()) Obj(NAME) :
                        new (mXbuf.buffer()) Obj);
            const Obj& X = mX;

            for (int di = 0; di < NUM_ALLOCS; ++di) {
                const int BYTES = DATA[ti].d_allocs[di];

                addresses[di] = mX.allocate(BYTES);
            }

            for (int di = 0; di < NUM_DEALLOCS; ++di) {
                const int INDEX = DATA[ti].d_deallocs[di];

                ASSERT(0 <= INDEX && INDEX < NUM_ALLOCS);

                mX.deallocate(addresses[INDEX]);
            }

            const int   BUF_SIZE = 1024;
            char        expBuffer[BUF_SIZE];

            if (NAME) {
                sprintf(expBuffer,
                        TEMPLATE_WITH_NAME,
                        NAME,
                        X.numBlocksInUse(), X.numBytesInUse(),
                        X.numBlocksMax(), X.numBytesMax(),
                        X.numBlocksTotal(), X.numBytesTotal(),
                        X.numMismatches(), X.numBoundsErrors());
            }
            else {
                sprintf(expBuffer,
                        TEMPLATE_WITHOUT_NAME,
                        X.numBlocksInUse(), X.numBytesInUse(),
                        X.numBlocksMax(), X.numBytesMax(),
                        X.numBlocksTotal(), X.numBytesTotal(),
                        X.numMismatches(), X.numBoundsErrors());
            }

            size_t offset = std::strlen(expBuffer);

            if (NUM_ALLOCS != NUM_DEALLOCS) {
                for (int i = 0; i < NUM_ALLOCS; ++i) {
                    bool wasDeallocated = false;
                    for (int j = 0; j < NUM_DEALLOCS; ++j) {
                        const int INDEX = DATA[ti].d_deallocs[j];

                        if (INDEX == i) {
                            wasDeallocated = true;
                            break;
                        }
                    }

                    if (!wasDeallocated) {
                        remAllocs[numRemAllocs] = i;
                        ++numRemAllocs;
                    }
                }

                strcpy(expBuffer + offset,
                       " Indices of Outstanding Memory Allocations:\n");
                offset = std::strlen(expBuffer);

                for (int i = 0; i != numRemAllocs; ++i) {
                    sprintf(expBuffer + offset, "\t%d", remAllocs[i]);
                    offset = std::strlen(expBuffer);
                }

                if (numRemAllocs > 0) {
                    sprintf(expBuffer + offset, "\n");
                }
            }

            if (veryVerbose) printf("\tCompare with expected result.\n");
            if (veryVeryVerbose)
                printf("\nEXPECTED OUTPUT:\n[%s]\n", expBuffer);

            {
                char buffer[BUF_SIZE];
                printToBuffer(buffer, BUF_SIZE, X);
                if (veryVerbose) printf("\n'print' OUTPUT:\n[%s]\n", buffer);
                ASSERT(0 == std::strcmp(buffer, expBuffer));
            }

            {
                BufferStream<> strm;
                strm << X;
                if (veryVerbose)
                    printf("\n'operator<<' OUTPUT:\n[%s]\n", strm.data());
                ASSERT(strm == expBuffer);
            }

            for (int i = 0; i < numRemAllocs; ++i) {
                mX.deallocate(addresses[remAllocs[i]]);
            }

            mX.~Obj();
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BUFFER OVERRUN DETECTION
        //
        // Concerns:
        //   That the test allocator properly detects buffer over and underrun.
        //
        // Plan:
        //   Set the allocator in quiet mode, then deliberately cause buffer
        //   over and underruns to happen, verifying that they have happened
        //   by observer `numMismatches`.  First, verify that legitimate
        //   writing over the user segment does not trigger false errors.  At
        //   the end, verify `status` is what's expected and that
        //  `numMismatches` is still 0.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting buffer over/underrun detection"
                            "\n======================================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        alloc.setQuiet(true);

        // verify we can write all over the user segment without triggering
        // over/underrun fault
        for (int segLen = 1; segLen < 1000; ++segLen) {
            seg = (char *) alloc.allocate(segLen);
            std::memset(seg, 0x07, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());

            seg = (char *) alloc.allocate(segLen);
            std::memset(seg, 0, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());
        }

        if (veryVerbose) {
            printf("Done verifying that writing over the segment doesn't\n"
                   "    trigger over/under run errors, numMismatches = %lld\n",
                   alloc.numMismatches());
        }

        bsls::Types::Int64 expectedBoundsErrors = 0;
        bool success;

        // Verify overrun is detected -- write on the trailing pad, deallocate,
        // and verify that numMismatches() has incremented to reflect the error
        // was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[segLen + badByte] = 0x07;
                alloc.deallocate(seg);
                success = (++expectedBoundsErrors == alloc.numBoundsErrors());
                ASSERT(success);

                if (veryVerbose) {
                    printf("%successfully tested overrun at %d bytes"
                           " past the end of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        // Verify underrun is detected -- write on the trailing pad,
        // deallocate, and verify that numMismatches() has incremented to
        // reflect the error was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[-(badByte + 1)] = 0x07;
                alloc.deallocate(seg);
                success = (++expectedBoundsErrors == alloc.numBoundsErrors());
                ASSERT(success);

                if (veryVerbose) {
                    printf("%successfully tested underrun at %d bytes"
                           " before the start of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        ASSERT(!alloc.numMismatches());

        // detect how far before the segment we have to corrupt to cause a
        // mismatch error.
        int mismatchByte = PADDING_SIZE - 1;
        do {
            ++mismatchByte;
            seg = (char *) alloc.allocate(100);
            seg[-mismatchByte] = 0x07;
            alloc.deallocate(seg);
        } while (!alloc.numMismatches());

        expectedBoundsErrors = alloc.numBoundsErrors();
        int mismatchErrors = 1;
        ASSERT(alloc.numMismatches() == mismatchErrors);

        // Verify underrun is not detected when a mismatch occurs
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[-(badByte + 1)] = 0x07;
                seg[-mismatchByte] = 0x07;
                alloc.deallocate(seg);
                success = (expectedBoundsErrors == alloc.numBoundsErrors() &&
                           ++mismatchErrors     == alloc.numMismatches());
                ASSERT(success);

                if (veryVerbose) {
                    printf("%successfully tested underrun with mismatch at %d "
                           "bytes before the start of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        ASSERT(alloc.status() == expectedBoundsErrors + mismatchErrors);
        ASSERT(expectedBoundsErrors == alloc.numBoundsErrors());
        ASSERT(mismatchErrors       == alloc.numMismatches());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR THROWS `std::bad_alloc` IF `malloc` FAILS
        //
        // Testing:
        //   That the test allocator throws std::bad_alloc if it is unable
        //   to get memory.
        //
        // Concerns:
        //   That the test allocator will throw std::bad_alloc if `malloc`
        //   returns 0.
        //
        // Plan:
        //   Set up a request for memory that fails, and verify that the
        //   exception occurs.
        // --------------------------------------------------------------------

        // A number of ways of getting `malloc` to fail were considered, all
        // had problems.  Asking for huge amounts of memory often took a lot of
        // time and wasted resources before `malloc` gave up.  The best
        // approach turned out to be to set a low limit on the amount of memory
        // that could be obtained, and exceed that.  It was, however, not
        // portable, so we decided to implement the test only on Solaris.
        // TBD: Implement this test on more platforms

        if (verbose) printf(
               "\nTEST ALLOCATOR THROWS `std::bad_alloc` IF `malloc` FAILS"
               "\n========================================================\n");

#ifdef BDE_BUILD_TARGET_EXC
// TBD This test is failing under gcc 4.3.2 with an uncaught exception.  It
// does *not* appear to be an issue with EH support, but an issue with the test
// case proper.  In the debugger, it appeared that the runtime had insufficient
// resources to handle the exception, so `abort` was invoked.
#if defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CMP_GNU)
        if (verbose) printf("\nTest throwing std::bad_alloc\n");

        rlimit rl = { 1 << 20, 1 << 20 };
        int sts = setrlimit(RLIMIT_AS, &rl);    // limit us to 1 MByte
        ASSERT(0 == sts);

        sts = getrlimit(RLIMIT_AS, &rl);
        ASSERT(0 == sts);
        if (verbose) printf(sizeof(int) == sizeof(rl.rlim_cur)
                            ? "Limits: %d, %d\n" : "Limits: %lld, %lld\n",
                            rl.rlim_cur,
                            rl.rlim_max);
        ASSERT(1 << 20 == rl.rlim_cur);

        Obj ta;

        bool caught = false;
        void *p = (void *) 0x12345678;
        try {
            p = ta.allocate(1 << 21);
            printf("Error: allocate() returned\n");
            ASSERT(0);
        } catch (std::bad_alloc) {
            caught = true;
            if (verbose) printf("Exception caught, p = %p\n", p);
        }

        ASSERT(caught);
#else
        if (verbose) printf(
                           "No testing.  Testing skipped on this platform.\n");
#endif
#else
        if (verbose) printf("No testing.  Exceptions are not enabled.\n");
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST CROSS MEMORY ALLOCATION/DEALLOCATION DETECTION
        //   Testing the detection of cross allocation/deallocation.  The
        //   implementation uses the memory allocation list managed within the
        //   `bslma::TestAllocator`.  Makes sure that the number of mismatches
        //   are counted when cross allocation/deallocation occurs.  Test using
        //   two allocators allocating the same amount of memory in the same
        //   sequence, and deallocating each other's memory block.
        //
        // Testing:
        //   CONCERN: cross allocation/deallocation is detected immediately.
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTEST CROSS MEMORY ALLOCATION/DEALLOCATION DETECTION"
                    "\n===================================================\n");

        {
        if (verbose) printf("\tTest cross memory allocation list\n");

        Obj allocator1(veryVeryVerbose);
        allocator1.setNoAbort(verbose);
        allocator1.setQuiet(!veryVerbose);

        Obj allocator2(veryVeryVerbose);

        void *a1 = allocator1.allocate(40);
        void *a2 = allocator1.allocate(30);
        void *a3 = allocator1.allocate(20);

        void *b1 = allocator2.allocate(40);
        void *b2 = allocator2.allocate(30);
        void *b3 = allocator2.allocate(20);

        if (verbose) printf("Cross deallocating first block\n");
        allocator1.deallocate(b1);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(1  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Cross deallocating second block\n");
        allocator1.deallocate(b2);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(2  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Cross deallocating third block\n");
        allocator1.deallocate(b3);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try first block\n");
        allocator2.deallocate(b1);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(2  == allocator2.numBlocksInUse());
        ASSERT(50 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try second block\n");
        allocator2.deallocate(b2);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(1  == allocator2.numBlocksInUse());
        ASSERT(20 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try third block\n");
        allocator2.deallocate(b3);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try first block\n");
        allocator1.deallocate(a1);
        ASSERT(2  == allocator1.numBlocksInUse());
        ASSERT(50 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try second block\n");
        allocator1.deallocate(a2);
        ASSERT(1  == allocator1.numBlocksInUse());
        ASSERT(20 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try third block\n");
        allocator1.deallocate(a3);
        ASSERT(0  == allocator1.numBlocksInUse());
        ASSERT(0  == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());
        }
     } break;
     case 7: {
        // --------------------------------------------------------------------
        // TEST MEMORY ALLOCATION LIST
        //   Testing the memory allocation list managed internally within the
        //   `bslma::TestAllocator`.  Ensures that the list of outstanding
        //   memory blocks are kept track of.  Test the case where there's no
        //   allocated memory block outstanding, all allocated memory
        //   blocks outstanding and some allocated memory blocks outstanding.
        //
        // Testing:
        //   CONCERN: memory allocation list is kept track of properly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST MEMORY ALLOCATION LIST"
                            "\n===========================\n");

        void *blocks[10] = { };  // Initialized to all null pointers
        Obj   ta("alloc list");

        // Test initial state (no allocated memory blocks outstanding)
        ASSERT(verifyBlockList(ta, blocks));

        // Test all allocated memory blocks are still outstanding
        blocks[0] = ta.allocate(40);
        blocks[1] = ta.allocate(30);
        blocks[2] = ta.allocate(20);
        ASSERT(3 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test all allocated memory blocks are still outstanding
        blocks[3] = ta.allocate(10);
        blocks[4] = ta.allocate( 1);
        ASSERT(5 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test some allocated memory blocks outstanding after deallocations
        ta.deallocate(blocks[0]);       blocks[0] = 0;
        ta.deallocate(blocks[3]);       blocks[3] = 0;
        ASSERT(3 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test allocated memory blocks outstanding after more allocations
        blocks[5] = ta.allocate( 1);
        blocks[6] = ta.allocate( 1);
        blocks[7] = ta.allocate( 1);
        blocks[8] = ta.allocate( 1);
        ASSERT(7 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test allocated memory blocks outstanding after more deallocations
        ta.deallocate(blocks[8]);       blocks[8] = 0;
        ASSERT(6 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test allocated memory blocks outstanding after more deallocations
        ta.deallocate(blocks[7]);       blocks[7] = 0;
        ta.deallocate(blocks[6]);       blocks[6] = 0;
        ASSERT(4 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

        // Test no allocated memory blocks outstanding all blocks deallocated
        ta.deallocate(blocks[1]);       blocks[1] = 0;
        ta.deallocate(blocks[2]);       blocks[2] = 0;
        ta.deallocate(blocks[4]);       blocks[4] = 0;
        ta.deallocate(blocks[5]);       blocks[5] = 0;
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(verifyBlockList(ta, blocks));

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST NAMED CONSTRUCTOR AND NAME ACCESS
        //   Testing the `bslma::TestAllocator` constructed with a name.
        //   Ensures that the name is accessible through the `name` function.
        //
        // Testing:
        //   TestAllocator(const char *name, bool verboseFlag = 0);
        //   const char *name() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST NAMED CONSTRUCTOR AND NAME ACCESS"
                            "\n======================================\n");

        const char    NAME[] = "Test Allocator";
        const size_t  length = sizeof(NAME) - 1;
        Obj a(NAME, veryVeryVerbose);

        if (verbose) printf("Make sure all internal states initialized.\n");
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("Make sure name is correct.\n");

        ASSERT(0 == memcmp(NAME, a.name(), length));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST ALLOCATION LIMIT
        //   Create a test allocator and set its allocation limit to varying
        //   values.  Verify that exception is thrown with the correct content
        //   for every allocation request that occurs after the number of
        //   requests exceeds the current allocation limit.  Also verify that
        //   exception is never thrown for negative allocation limits.
        //   Finally, verify that nothing is allocated when an exception is
        //   thrown.
        //
        // Testing:
        //   CONCERN: exception is thrown after allocation limit is exceeded
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST ALLOCATION LIMIT"
                            "\n=====================\n");

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nTesting for exception thrown after exceeding "
                              "allocation limit.\n");

        const int NUM_ALLOC = 5;
        const int LIMIT[] = { 0, 1, 4, 5, -1, -100 };
        const int NUM_TEST = sizeof LIMIT / sizeof *LIMIT;

        Obj upstream("Upstream");
        Obj mX(veryVeryVerbose, &upstream);

        const bsls::Types::Int64 k_INITIAL_UPSTREAM_BLOCKS =
                                                     upstream.numBlocksInUse();

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            mX.setAllocationLimit(LIMIT[ti]);

            for (int ai = 0; ai < NUM_ALLOC; ++ai) {
                const bslma::Allocator::size_type SIZE = ai + 1;
                                             // alloc size varies for each test
                if (veryVerbose) { P_(ti); P_(ai); P_(SIZE); P(LIMIT[ti]); }

                const bsls::Types::Int64 NUM_ALLOCS = mX.numAllocations();

                try {
                    void *p = mX.allocate(SIZE);
                    ASSERTV(ti, ai, NUM_ALLOCS + 1 == mX.numAllocations());
                    ASSERTV(ti, ai, SIZE == mX.lastAllocatedNumBytes());
                    ASSERTV(ti, ai, p == mX.lastAllocatedAddress());
                    mX.deallocate(p);
                    ASSERTV(ti, ai, LIMIT[ti] != ai);
                }
                catch (bslma::TestAllocatorException& e) {
                    bslma::Allocator::size_type numBytes = e.numBytes();
                    if (veryVerbose) { printf("Caught: "); P(numBytes); }

                    ASSERTV(ti, ai, SIZE == numBytes);
                    ASSERTV(ti, ai, k_INITIAL_UPSTREAM_BLOCKS ==
                                                    upstream.numBlocksInUse());
                    ASSERTV(ti, ai, LIMIT[ti] == ai);

                    // An allocation will increment `numAllocations()` and set
                    // `lastAllocatedNumBytes()` and
                    // `lastDeallocatedAddress()`, even if the allocation fails
                    // by means of an exception.  This behaviour could be
                    // considered a bug, but there exist long-standing
                    // workarounds in test drivers, so the behavior is now
                    // enshrined.
                    ASSERTV(ti, ai, NUM_ALLOCS + 1 == mX.numAllocations());
                    ASSERTV(ti, ai, SIZE == mX.lastAllocatedNumBytes());
                    ASSERTV(ti, ai, 0 == mX.lastAllocatedAddress());
                }
            }
        }
#else
        if (verbose) printf("\nNo testing.  Exceptions are not enabled.\n");
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST SIMPLE STREAMING
        //   Lightly verify that the `operator<<` hidden friend works.
        //
        // Testing:
        //   SIMPLE STREAMING
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST SIMPLE STREAMING"
                            "\n=====================\n");

        if (verbose) printf("\nTest a single case with unique fields.\n");

        Obj a;

        if (verbose) printf("\tSet up unique fields.\n");
        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate( 1);

        a.deallocate(p4);
        a.deallocate(p1);

        p1 = a.allocate(10);

        ASSERT(3 == a.numBlocksInUse());  ASSERT( 60 == a.numBytesInUse());
        ASSERT(4 == a.numBlocksMax());    ASSERT( 91 == a.numBytesMax());
        ASSERT(5 == a.numBlocksTotal());  ASSERT(101 == a.numBytesTotal());
        ASSERT(0 == a.numMismatches());

        const char* const EXP =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t3\t60\n"
            "             MAX\t4\t91\n"
            "           TOTAL\t5\t101\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            " Indices of Outstanding Memory Allocations:\n"
            "\t1\t2\t4\n"
            ;

        if (veryVerbose) printf("\nEXPECTED OUTPUT:\n[%s]\n", EXP);

        BufferStream<> strm;
        strm << a;

        if (veryVerbose) printf("\nACTUAL OUTPUT:\n[%s]\n", strm.data());

        ASSERTV(strm.data(), EXP, strm == EXP);

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p3);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ERROR COUNTS
        //   Disable the abort mode and ensure that attempting failure modes
        //   is correctly detected and logged in the numMismatches counter.
        //   Also verify that status is correct.  Note that this test case
        //   will cause purify errors.
        //
        // Testing:
        //   ~TestAllocator();
        //   void *allocate(size_type size);
        //   void deallocate(void *address);
        //
        //   CONCERN: the allocator is incompatible with new/delete.
        //   CONCERN: the allocator is incompatible with malloc/free.
        //   CONCERN: mismatched deallocations are detected/reported.
        //   CONCERN: repeated deallocations are detected/reported.
        //   CONCERN: an invalid cached length is reported.
        //   CONCERN: deallocated memory is scribbled.
        //   CONCERN: memory leaks (byte/block) are detected/reported.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ERROR COUNTS"
                            "\n====================\n");

        if (verbose) printf(
                     "\nNote:\n"
                     "  Error messages can be viewed in veryVerbose mode\n"
                     "  and Memory trace messages in veryVeryVerbose mode.\n");

        const char *const LINE =
        "-------------------------------------------------------------------";

        {
            // Offset of payload within non-allocator-supplied block.
            const std::size_t offset = 12 * sizeof(void*);

            int i;
            if (verbose) printf("\nEnsure incompatibility with new/delete.\n");

            Obj a(veryVeryVerbose);
            a.setNoAbort(verbose); a.setQuiet(!veryVerbose);

            if (verbose) printf("\t[deallocate unallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            char *p = static_cast<char *>(operator new(200));
            for (i = 0; i < 200; ++i) { p[i] = (char) i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 == a.status());
                                ASSERT(0 == a.numMismatches());

            a.deallocate(p + offset);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());
            operator delete(p);
            p = static_cast<char*>(a.allocate(7));  p[0] = (char) 0xA7;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(7 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(1 == a.numMismatches());

            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                   "\nEnsure repeated deallocations are detected/reported.\n");

            if (verbose) printf("\t[deallocate deallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                               "\nEnsure incompatibility with malloc/free.\n");
            p = static_cast<char*>(malloc(200));
            for (i = 0; i < 200; ++i) { p[i] = (char) i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());
            if (verbose) printf("\t[deallocate unallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p + offset);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(3 == a.status());
                                ASSERT(3 == a.numMismatches());
            free(p);
            p = static_cast<char*>(a.allocate(5));  p[0] = (char) 0xA5;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(5 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(3 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
       "\nEnsure immediately repeated deallocations are detected/reported.\n");
            if (verbose) printf("\t[deallocate deallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p);
            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 4 == a.status());
                                ASSERT(4 == a.numMismatches());

            p = static_cast<char*>(a.allocate(3));  p[0] = (char) 0xA3;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(1 == a.numBlocksMax());
                                ASSERT(7 == a.numBytesMax());
                                ASSERT(3 == a.numBlocksTotal());
                                ASSERT(15 == a.numBytesTotal());
                                LOOP_ASSERT(a.status(), 4 == a.status());
                                ASSERT(4 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                 "\nEnsure mismatched deallocations are detected/reported.\n");

            if (verbose) printf(
                               "\t[deallocate pointer + sizeof(size_type)]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate((bslma::Allocator::size_type *)
                                                   static_cast<void *>(p) + 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(5 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                               "\t[deallocate pointer - sizeof(size_type)]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate((bslma::Allocator::size_type *)
                                                   static_cast<void *>(p) - 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(6 == a.numMismatches());
            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 6 == a.status());
                                ASSERT(6 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                           "\nEnsure an invalid cached length is reported.\n");

            struct B {
                int d_m;
                int d_s;
            } b[2] = { { (int) 0xdeadbeef, -1 }, { 0x11, 0x22 } };

            if (verbose) printf("\t[deallocate memory with bad length]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(b + 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 7 == a.status());
                                ASSERT(7 == a.numMismatches());

#if 0
    // Accessing deallocated memory can result in errors on some platforms.
    // For this reason, this part of the test has been removed for now.
            if (verbose) printf("\nEnsure deallocated memory is scribbled.\n");
            unsigned char *q = (unsigned char *) a.allocate(9);
            std::memset(q, 0, 9);
            a.deallocate(q);
            if (verbose) printf("\t[deallocate memory scribbled]\n");
            for (int mi = 0; mi < 9; ++mi) {
                LOOP_ASSERT(mi, SCRIBBLED_MEMORY == q[mi]);
            }
            if (veryVerbose) puts(LINE);
#endif

            p = static_cast<char*>(a.allocate(9));  p[0] = (char) 0xA9;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(9 == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(7 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                "\nEnsure memory leaks (byte/block) are detected/reported.\n");

            if (verbose) printf("\t[memory leak]\n");
            if (veryVerbose) puts(LINE);
        }
        if (veryVerbose) puts(LINE);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GET/SET FLAGS
        //   Verify that each of the get/set flags is working properly.
        //   Also verify that status correctly returns 0.
        //
        // Testing:
        //   void setAllocationLimit(Int64 limit);
        //   void setNoAbort(bool flagValue);
        //   void setQuiet(bool flagValue);
        //   void setVerbose(bool flagValue);
        //   Int64 allocationLimit() const;
        //   bool isNoAbort() const;
        //   bool isQuiet() const;
        //   bool isVerbose() const;
        //   int status() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nGET/SET FLAGS"
                            "\n=============\n");

        if (verbose) printf("\nTest get/set flags\n");
        {
            Obj a(veryVeryVerbose);

            ASSERT(0 == a.isQuiet());
            ASSERT(0 == a.isNoAbort());
            ASSERT(veryVeryVerbose == a.isVerbose());
            ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tVerbose\n");

            a.setVerbose(true);  ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(1 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            a.setVerbose(false); ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tQuiet\n");

            a.setQuiet(true);    ASSERT(1 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            a.setQuiet(false);   ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tNoAbort\n");

            a.setNoAbort(true);  ASSERT(0 == a.isQuiet());
                                 ASSERT(1 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            a.setNoAbort(false); ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tAllocationLimit\n");

            a.setAllocationLimit(5);
                                 ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(5 ==  a.allocationLimit());

            a.setAllocationLimit(0);
                                 ASSERT(0 == a.isQuiet());
                                 ASSERT(0 == a.isNoAbort());
                                 ASSERT(0 == a.isVerbose());
                                 ASSERT(0 == a.allocationLimit());

            a.setAllocationLimit(-1);
                                 ASSERT( 0 == a.isQuiet());
                                 ASSERT( 0 == a.isNoAbort());
                                 ASSERT( 0 == a.isVerbose());
                                 ASSERT(-1 == a.allocationLimit());

            if (verbose) printf("\tStatus\n");
                                 ASSERT(0 == a.status());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create a test allocator in as buffer to make sure each field
        //   is properly initialized (with no verbose flag).  Then create a
        //   test allocator on the program stack and verify that all of the
        //   non-error counts are working properly.  Ensure that neither
        //   `new` or `delete` are called.
        //
        // Testing:
        //   BASIC TEST
        //   TestAllocator(bool verboseFlag = 0);
        //   CONCERN: all counts are initialized to zero (placement new).
        //   CONCERN: global operators new and delete are *not* called.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

        if (verbose) printf("BREATHING TEST\n");
        {
            Obj testAllocator(veryVeryVerbose);
            bslma::Allocator *ta = &testAllocator;

            ASSERT(0 == testAllocator.numBlocksTotal());
            ASSERT(0 == testAllocator.numBlocksInUse());

            void *p = ta->allocate(123);
            ASSERT(1 == testAllocator.numBlocksTotal());
            ASSERT(1 == testAllocator.numBlocksInUse());

            void *q = ta->allocate(456);

            ASSERT(2 == testAllocator.numBlocksTotal());

            ta->deallocate(q);
            ta->deallocate(p);
        }

        if (verbose) printf("\nCreate an allocator in a buffer\n");

        bsls::ObjectBuffer<Obj> arena;

        std::memset(static_cast<void *>(arena.address()), 0xA5, sizeof arena);
        Obj *p = new(&arena) Obj;

        if (verbose) printf(
            "\nMake sure all counts/and flags are initialized\n");

        if (veryVerbose) p->print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == p->numBlocksInUse());
        ASSERT(0 == p->numBytesInUse());
        ASSERT(0 == p->numBlocksMax());
        ASSERT(0 == p->numBytesMax());
        ASSERT(0 == p->numBlocksTotal());
        ASSERT(0 == p->numBytesTotal());
        ASSERT(0 == p->isQuiet());
        ASSERT(0 == p->isNoAbort());
        ASSERT(0 == p->isVerbose());
        ASSERT(0 == p->status());
        ASSERT(0 == p->lastAllocatedNumBytes());
        ASSERT(0 == p->lastAllocatedAddress());
        ASSERT(0 == p->lastDeallocatedNumBytes());
        ASSERT(0 == p->lastDeallocatedAddress());
        ASSERT(0 == p->numAllocations());
        ASSERT(0 == p->numDeallocations());

        if (verbose) printf("\nCreate an allocator\n");

        Obj a(veryVeryVerbose);

        if (verbose) printf("\nMake sure counts work properly\n");

        if (veryVerbose) a.print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("\tallocate 1\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = a.allocate(1);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(1 == a.numBytesInUse());
        ASSERT(1 == a.numBlocksMax());
        ASSERT(1 == a.numBytesMax());
        ASSERT(1 == a.numBlocksTotal());
        ASSERT(1 == a.numBytesTotal());
        ASSERT(1 == a.lastAllocatedNumBytes());
        ASSERT(addr1 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(1 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("\tallocate 20\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = a.allocate(20);
        globalNewCalledCountIsEnabled = 0;
        ASSERT( 2 == a.numBlocksInUse());
        ASSERT(21 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT( 0 == a.lastDeallocatedNumBytes());
        ASSERT( 0 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 0 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 20\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT( 1 == a.numBlocksInUse());
        ASSERT( 1 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 1 == a.numDeallocations());

        if (verbose) printf("\tallocate 300\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr3 = a.allocate(300);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(  2 == a.numBlocksInUse());
        ASSERT(301 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  1 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 300\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr3);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  1 == a.numBlocksInUse());
        ASSERT(  1 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(300 == a.lastDeallocatedNumBytes());
        ASSERT(addr3 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  2 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 1\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  0 == a.numBlocksInUse());
        ASSERT(  0 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  3 == a.numDeallocations());

        if (verbose) printf("\nMake sure allocate/deallocate invalid "
                            "size/address is recorded\n.");

        a.setNoAbort(1);
        a.setQuiet(1);

        if (verbose) printf("\tallocate 0\n");
        a.allocate(0);
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(3 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 0\n");
        a.deallocate(0);
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(4 == a.numDeallocations());

        if (verbose) printf("\nEnsure new and delete are not called.\n");
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING BUFFER UNDERRUN ABORT
        //
        // Concerns:
        //   That when we abort do to a buffer underrun in non-quiet mode, the
        //   message comes out right.
        //
        // Plan:
        //   Deliberately do a buffer underrun and abort.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BUFFER UNDERRUN ABORT"
                            "\n=============================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        // make non-quiet underrun happen (and abort) so we can observe the
        // message
        seg = (char *) alloc.allocate(100);
        seg[-3] = 0x07;
        alloc.deallocate(seg);
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TESTING BUFFER OVERRUN ABORT
        //
        // Concerns:
        //   That when we abort do to a buffer overrun in non-quiet mode, the
        //   message comes out right.
        //
        // Plan:
        //   Deliberately do a buffer overrun and abort.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BUFFER OVERRUN ABORT"
                            "\n============================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        // make non-quiet overrun happen (and abort) so we can observe the
        // message
        seg = (char *) alloc.allocate(100);
        seg[103] = 0x07;
        alloc.deallocate(seg);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
