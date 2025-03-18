// bslma_allocatorutil.t.cpp                                          -*-C++-*-

#include <bslma_allocatorutil.h>

#include <bsla_maybeunused.h>

#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>

#include <bsls_annotation.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>

#include <cstdio>   // `printf`
#include <cstdlib>  // `atoi`
#include <typeinfo> // `type_info`

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using std::printf;
using std::fprintf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------

// The component under test is a utility component comprising a set of related
// independently testable functions.  A test case exists for each function.
// ----------------------------------------------------------------------------
// [ 2] bslma::Allocator *adapt(const bsl::allocator<TYPE>&);
// [ 2] ALLOCATOR         adapt(const ALLOCATOR&);
// [ 6] TYPE& assign(TYPE *, const TYPE&, bsl::true_type);
// [ 6] TYPE& assign(TYPE *, const TYPE&, bsl::false_type);
// [ 3] void_pointer allocateBytes(const ALLOC&, size_t, size_t);
// [ 4] pointer allocateObject(const ALLOCATOR&, size_t);;
// [ 3] void deallocateBytes(const ALLOC&, void_pointer, size_t, size_t);
// [ 4] void deallocateObject(const ALLOCATOR&, pointer, size_t);
// [ 5] void deleteObject(const ALLOCATOR&, pointer);
// [ 5] pointer newObject<TYPE&>(const ALLOCATOR&, ARGS&&...);
// [ 7] TYPE& swap(TYPE *, TYPE*, bsl::false_type);
// [ 7] TYPE& swap(TYPE *, TYPE*, bsl::true_type);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLES
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma::AllocatorUtil Util;

const std::size_t k_MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

/// Return `true` if the specified `p` (which was probably returned by an
/// allocator) is correctly aligned for the parameterized `TYPE`. Some
/// allocators fail to correctly handle overaligned types, yielding a
/// misaligned pointer that might cause a loss of performance, but is unlikely
/// to produce a bus fault.
template <class TYPE>
bool isCorrectlyAligned(const TYPE *p)
{
    return
        bsls::AlignmentUtil::isAligned(p,bsls::AlignmentFromType<TYPE>::VALUE);
}

template <class TYPE>
class DerivedAllocator : public bsl::allocator<TYPE> {
    typedef bsl::allocator<TYPE> Base;

  public:
    DerivedAllocator(bslma::Allocator *a = 0) : Base(a) { }

    template <class T2>
    DerivedAllocator(const DerivedAllocator<T2>& other)
        : Base(other.mechanism()) { }
};

/// A class that meets the minimum requirements for a C++17 allocator.
template <class TYPE>
class StlAllocator {

    bsl::allocator<TYPE> d_imp;

  public:
    typedef TYPE value_type;

    explicit StlAllocator(bslma::Allocator *a = 0) : d_imp(a) { }

    template <class T2>
    StlAllocator(const StlAllocator<T2>& other) : d_imp(other.bslmaAlloc()) { }

    TYPE *allocate(std::size_t n) { return d_imp.allocate(n); }
    void deallocate(TYPE *p, std::size_t n) { d_imp.deallocate(p, n); }

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // C++03 requires that the `construct` functions be defined.
    // This collection is not generalized to arbitrary arguments, but is the
    // minimum useful set for this test driver.
    void construct(TYPE *p) { ::new (static_cast<void *>(p)) TYPE(); }
    template <class ARG>
    void construct(TYPE *p, const ARG& arg)
        { ::new (static_cast<void *>(p)) TYPE(arg); }
    void construct(TYPE *p     , long a01    , long a02    ,
                   long a03 = 0, long a04 = 0, long a05 = 0, long a06 = 0,
                   long a07 = 0, long a08 = 0, long a09 = 0, long a10 = 0,
                   long a11 = 0, long a12 = 0, long a13 = 0, long a14 = 0)
    {
        ::new (static_cast<void *>(p)) TYPE(a01, a02, a03, a04, a05, a06,
                                            a07, a08, a09, a10, a11, a12,
                                            a13, a14);
    }
#endif

    bslma::Allocator *bslmaAlloc() const { return d_imp.mechanism(); }
};

template <class T1, class T2>
bool operator==(const StlAllocator<T1>& lhs, const StlAllocator<T2>& rhs)
{
    return lhs.bslmaAlloc() == rhs.bslmaAlloc();
}

template <class T1, class T2>
bool operator!=(const StlAllocator<T1>& lhs, const StlAllocator<T2>& rhs)
{
    return lhs.bslmaAlloc() != rhs.bslmaAlloc();
}

int g_ConstructingAllocCount = 0; // # calls to `ConstructingAlloc::construct`

/// An allocator class with a custom `construct` method
template <class TYPE>
class ConstructingAlloc {

    bsl::allocator<TYPE> d_imp;

  public:
    typedef TYPE value_type;

    explicit ConstructingAlloc(bslma::Allocator *a = 0) : d_imp(a) { }

    template <class T2>
    ConstructingAlloc(const ConstructingAlloc<T2>& other)
        : d_imp(other.mechanism()) { }

    TYPE *allocate(std::size_t n) { return d_imp.allocate(n); }
    void deallocate(TYPE *p, std::size_t n) { d_imp.deallocate(p, n); }

    void construct(TYPE *p)
    {
        ++g_ConstructingAllocCount;
        ::new (static_cast<void *>(p)) TYPE();
    }

    template <class ARG>
    void construct(TYPE *p, const ARG& arg)
    {
        ++g_ConstructingAllocCount;
        ::new (static_cast<void *>(p)) TYPE(arg);
    }

    bslma::Allocator *mechanism() const { return d_imp.mechanism(); }
};

template <class T1, class T2>
bool operator==(const ConstructingAlloc<T1>& lhs,
                const ConstructingAlloc<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
bool operator!=(const ConstructingAlloc<T1>& lhs,
                const ConstructingAlloc<T2>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

/// A `memory_resource` that checks that each deallocation is matched with a
/// corresponding allocation having the same address, size, and alignment.
/// Can have up to a maximum of 128 blocks outstanding.  This extension to
/// `bslma::TestAllocator` functionality will eventually be moved into
/// `bslma_testallocator` itself, at which point this class will be removed
/// and the test driver will use `bslma::TestAllocator` directly.
/// See {DRQS 174865686}.
class CheckedResource : public bslma::TestAllocator {

    // PRIVATE TYPES
    typedef bslma::TestAllocator Base;

    /// Information about a block of memory allocated from this resource.
    struct BlockInfo {
        std::size_t  d_bytes;          // requested bytes
        std::size_t  d_alignment;      // requested alignment
        void        *d_block_p;        // raw allocated block
        void        *d_alignedBlock_p; // block after alignment correction
    };

    // DATA
    enum { k_MAX_OUTSTANDING_BLOCKS = 128 };

    // Keep track of allocations in an array of `BlockInfo` objects.
    BlockInfo             d_blocks[k_MAX_OUTSTANDING_BLOCKS];
    BlockInfo            *d_nextBlock;  // Next unallocated block info

    // Counters for mismatches detected at deallocation time.
    int                   d_numPointerMismatches;
    int                   d_numSizeMismatches;
    int                   d_numAlignmentMismatches;

    // PRIVATE ACCESSORS

    /// Return the index of the block associated with `blockPtr` or -1
    /// if not found.
    int blockIndex(void *blockPtr) const;

    // PRIVATE VIRTUAL METHODS
    void *do_allocate(std::size_t bytes,
                      std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void        *p,
                       std::size_t  bytes,
                       std::size_t  alignment) BSLS_KEYWORD_OVERRIDE;
    bool do_is_equal(const memory_resource& other) const BSLS_KEYWORD_NOEXCEPT
                                                         BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS
    explicit CheckedResource(bslma::Allocator *upstream = 0);
    explicit CheckedResource(const char *name, bslma::Allocator *upstream = 0);
    explicit CheckedResource(bool              verboseFlag,
                             bslma::Allocator *upstream = 0);
    CheckedResource(const char       *name,
                    bool              verboseFlag,
                    bslma::Allocator *upstream = 0);

    ~CheckedResource() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS
    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;
    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS
    bool hasExpSize(void *block, std::size_t expSize) const;
    bool hasExpAlignment(void *block, std::size_t expAlign) const;

    int numPointerMismatches()   const { return d_numPointerMismatches;   }
    int numSizeMismatches()      const { return d_numSizeMismatches;      }
    int numAlignmentMismatches() const { return d_numAlignmentMismatches; }
};

// PRIVATE ACCESSORS
int CheckedResource::blockIndex(void *blockPtr) const
{
    for (const BlockInfo *bp = d_blocks; bp < d_nextBlock; ++bp) {
        if (blockPtr == bp->d_alignedBlock_p) {
            return int(bp - d_blocks);
        }
    }

    return -1;
}

// CREATORS
BSLS_ANNOTATION_UNUSED
CheckedResource::CheckedResource(bslma::Allocator  *upstream)
    : Base(upstream)
    , d_nextBlock(d_blocks)
    , d_numPointerMismatches(0)
    , d_numSizeMismatches(0)
    , d_numAlignmentMismatches(0)
{
}

BSLS_ANNOTATION_UNUSED
CheckedResource::CheckedResource(const char        *name,
                                 bslma::Allocator  *upstream)
    : Base(name, upstream)
    , d_nextBlock(d_blocks)
    , d_numPointerMismatches(0)
    , d_numSizeMismatches(0)
    , d_numAlignmentMismatches(0)
{
}

BSLS_ANNOTATION_UNUSED
CheckedResource::CheckedResource(bool              verboseFlag,
                                 bslma::Allocator *upstream)
    : Base(verboseFlag, upstream)
    , d_nextBlock(d_blocks)
    , d_numPointerMismatches(0)
    , d_numSizeMismatches(0)
    , d_numAlignmentMismatches(0)
{
}

BSLS_ANNOTATION_UNUSED
CheckedResource::CheckedResource(const char       *name,
                                 bool              verboseFlag,
                                 bslma::Allocator *upstream)
    : Base(name, verboseFlag, upstream)
    , d_nextBlock(d_blocks)
    , d_numPointerMismatches(0)
    , d_numSizeMismatches(0)
    , d_numAlignmentMismatches(0)
{
}

CheckedResource::~CheckedResource()
{
    ASSERTV(name(), d_nextBlock == d_blocks);  // All memory was returned
    ASSERTV(name(), 0 == numPointerMismatches());
    ASSERTV(name(), 0 == numSizeMismatches());
    ASSERTV(name(), 0 == numAlignmentMismatches());
}

// PRIVATE VIRTUAL METHODS
void *CheckedResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    BSLS_ASSERT_OPT(d_nextBlock < d_blocks + k_MAX_OUTSTANDING_BLOCKS);

    // `slack` is the amount of extra memory we need to allocate to be able to
    // correct for `alignment` greater than max alignment.
    std::ptrdiff_t slack = 0;
    if (alignment > k_MAX_ALIGNMENT)
        slack = alignment - k_MAX_ALIGNMENT;

    // Allocate memory and compute the first address with the desired
    // `alignment`.
    void *block_p        = Base::allocate(bytes + slack);
    void *alignedBlock_p = static_cast<char*>(block_p) +
        bsls::AlignmentUtil::calculateAlignmentOffset(block_p, int(alignment));

    d_nextBlock->d_bytes          = bytes;
    d_nextBlock->d_alignment      = alignment;
    d_nextBlock->d_block_p        = block_p;
    d_nextBlock->d_alignedBlock_p = alignedBlock_p;
    ++d_nextBlock;

    return alignedBlock_p;
}

void CheckedResource::do_deallocate(void        *p,
                                    std::size_t  bytes,
                                    std::size_t  alignment)
{
    int index = blockIndex(p);
    if (index < 0) {
        // Didn't find matching allocated block
        ++d_numPointerMismatches;
    }
    else {
        BlockInfo *bp = &d_blocks[index];
        d_numSizeMismatches += (0 != bytes && bp->d_bytes != bytes);
        d_numAlignmentMismatches += (bp->d_alignment != alignment);
        Base::deallocate(bp->d_block_p);
        *bp = *--d_nextBlock;  // Remove from block info array
    }
}

bool CheckedResource::do_is_equal(const bsl::memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return this == &other;
}

// MANIPULATORS
BSLS_ANNOTATION_UNUSED
void *CheckedResource::allocate(size_type size)
{
    BSLS_ASSERT(false && "Should call `memory_resource::allocate`");
    return do_allocate(size, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

BSLS_ANNOTATION_UNUSED
void CheckedResource::deallocate(void *address)
{
    BSLS_ASSERT(false && "Should call `memory_resource::deallocate`");
    do_deallocate(address, 0, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

// ACCESSORS
bool CheckedResource::hasExpSize(void *block, std::size_t expSize) const
{
    const int index = blockIndex(block);
    if (index < 0) {
        return 0 == expSize;
    }
    const std::size_t align      = d_blocks[index].d_alignment;
    const std::size_t nbytes     = d_blocks[index].d_bytes;
    const std::size_t maxExpSize = (expSize + align - 1) & ~(align - 1);
    return expSize <= nbytes && nbytes <= maxExpSize;
}

bool CheckedResource::hasExpAlignment(void *block, std::size_t expAlign) const
{
    const int index = blockIndex(block);
    const std::size_t align = index < 0 ? 0 : d_blocks[index].d_alignment;

    if (expAlign <= k_MAX_ALIGNMENT) {
        return align == expAlign;
    }
    else {
        return align == expAlign || align == k_MAX_ALIGNMENT;
    }
}

/// Allocator for which assignment allowed.
class AssignableAllocator {

    int d_id;

  public:
    typedef char           value_type;
    typedef bsl::true_type propagate_on_container_copy_construction;
    typedef bsl::true_type propagate_on_container_move_construction;
    typedef bsl::true_type propagate_on_container_swap;

    explicit AssignableAllocator(int i) : d_id(i) { }

    int id() const { return d_id; }
};

BSLA_MAYBE_UNUSED
inline bool operator==(const AssignableAllocator& lhs,
                       const AssignableAllocator& rhs)
{
    return lhs.id() == rhs.id();
}

BSLA_MAYBE_UNUSED
inline bool operator!=(const AssignableAllocator& lhs,
                       const AssignableAllocator& rhs)
{
    return lhs.id() != rhs.id();
}

/// Allocator for which assignment is not allowed.
class NonAssignableAllocator {

    int d_id;

    /// Disabled assignment
    NonAssignableAllocator& operator=(const NonAssignableAllocator&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    typedef char            value_type;
    typedef bsl::false_type propagate_on_container_copy_construction;
    typedef bsl::false_type propagate_on_container_move_construction;
    typedef bsl::false_type propagate_on_container_swap;

    explicit NonAssignableAllocator(int i) : d_id(i) { }

    int id() const { return d_id; }
};

BSLA_MAYBE_UNUSED
inline bool operator==(const NonAssignableAllocator& lhs,
                       const NonAssignableAllocator& rhs)
{
    return lhs.id() == rhs.id();
}

BSLA_MAYBE_UNUSED
inline bool operator!=(const NonAssignableAllocator& lhs,
                       const NonAssignableAllocator& rhs)
{
    return lhs.id() != rhs.id();
}

/// Non-raw pointer type.
template <class TYPE>
class FancyPtr {

    TYPE *d_ptr;

  public:
    typedef TYPE value_type;

    explicit FancyPtr(TYPE *p = 0) : d_ptr(p) { }

    template <class T2>
    FancyPtr(const FancyPtr<T2>& other,
                 typename bsl::enable_if<
                     bsl::is_convertible<T2 *, TYPE *>::value
                 >::type * = 0) : d_ptr(other.d_ptr) { }

    template <class T2>
    explicit FancyPtr(const FancyPtr<T2>& other,
                          typename bsl::enable_if<
                              bsl::is_convertible<TYPE *, T2 *>::value &&
                              ! bsl::is_convertible<T2 *, TYPE *>::value
                          >::type * = 0)
        : d_ptr(static_cast<TYPE*>(other.get())) { }

    TYPE& operator*() const { return *d_ptr; }
    TYPE *operator->() const { return d_ptr; }

    TYPE *get() const { return d_ptr; }
};

/// Non-raw void pointer type.
template <>
class FancyPtr<void> {

    void *d_ptr;

  public:
    typedef void value_type;

    explicit FancyPtr(void *p = 0) : d_ptr(p) { }

    template <class T2>
    FancyPtr(const FancyPtr<T2>& other) : d_ptr(other.get()) { }

    void *get() const { return d_ptr; }
};

template <class TYPE>
bool operator==(FancyPtr<TYPE> lhs, FancyPtr<TYPE> rhs)
{
    return lhs.get() == rhs.get();
}

template <class TYPE>
bool operator!=(FancyPtr<TYPE> lhs, FancyPtr<TYPE> rhs)
{
    return lhs.get() != rhs.get();
}

template <class T>
T* rawPtr(T* p) { return p; }

template <class T>
T* rawPtr(FancyPtr<T> p) { return p.get(); }

/// An allocator class that uses non-raw pointers and meets the requirements
/// for a C++17 allocator.
template <class TYPE>
class FancyAllocator
{

    bsl::allocator<TYPE> d_mechanism;

  public:
    typedef TYPE                     value_type;
    typedef FancyPtr<TYPE>       pointer;
    typedef FancyPtr<const TYPE> const_pointer;
    typedef FancyPtr<void>       void_pointer;
    typedef FancyPtr<const void> const_void_pointer;

    FancyAllocator(bslma::Allocator *a = 0) : d_mechanism(a) { }

    template <class T2>
    FancyAllocator(const FancyAllocator<T2>& other)
        : d_mechanism(other.mechanism()) { }

    pointer allocate(std::size_t n)
        { return pointer(d_mechanism.allocate(n)); }

    void construct(TYPE *p) { ::new (static_cast<void *>(p)) TYPE(); }

    template <class ARG>
    void construct(TYPE *p, const ARG& arg)
        { ::new (static_cast<void *>(p)) TYPE(arg); }

    void deallocate(pointer p, std::size_t n)
        { d_mechanism.deallocate(p.get(), n); }

    bsl::allocator<TYPE> mechanism() const { return d_mechanism; }
};

template <class T1, class T2>
bool operator==(const FancyAllocator<T1>& lhs, const FancyAllocator<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
bool operator!=(const FancyAllocator<T1>& lhs, const FancyAllocator<T2>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

/// Legacy-AA class.
class LegacyAAClass {

    bslma::Allocator *d_allocator_p;
    // ...

  public:
    explicit LegacyAAClass(bslma::Allocator *basicAllocator = 0)
        : d_allocator_p(basicAllocator) { /* ... */ }

    bslma::Allocator *allocator() const { return d_allocator_p; }
};

/// Bsl-AA class.
class BslAAClass {

    bsl::allocator<int> d_allocator;
    // ...

  public:
    typedef bsl::allocator<int> allocator_type;

    explicit BslAAClass(const allocator_type& allocator = allocator_type())
        : d_allocator(allocator) { /* ... */ }

    allocator_type get_allocator() const { return d_allocator; }
};

/// Stl-AA class.
class StlAAClass {

    StlAllocator<int> d_allocator;
    // ...

  public:
    typedef StlAllocator<int> allocator_type;

    explicit StlAAClass(const allocator_type& allocator = allocator_type())
        : d_allocator(allocator) { /* ... */ }

    allocator_type get_allocator() const { return d_allocator; }
};

/// Class that tracks the number of outstanding objects.
class TrackedObj {

    static int s_ctorCount;
    static int s_dtorCount;

  public:
    TrackedObj() { ++s_ctorCount; }
    TrackedObj(const TrackedObj&) { ++s_ctorCount; }
    ~TrackedObj() { ++s_dtorCount; }

    static void clearCounts() { s_ctorCount = s_dtorCount = 0; }
    static int ctorCount() { return s_ctorCount; }
    static int dtorCount() { return s_dtorCount; }
};

int TrackedObj::s_ctorCount = 0;
int TrackedObj::s_dtorCount = 0;

/// Type with alignment greater than largest native platform alignment.
/// In C++03, this type simply has the largest native platform alignment.
struct OveralignedObj {
#ifdef BSLS_ALIGNMENTTOTYPE_USES_ALIGNAS
    alignas(k_MAX_ALIGNMENT * 2) char d_data[k_MAX_ALIGNMENT * 2];
#else
    bsls::AlignmentUtil::MaxAlignedType d_data;
#endif

    OveralignedObj()
        { ASSERTV(this, isCorrectlyAligned(this)); }
    OveralignedObj(const OveralignedObj&)
        { ASSERTV(this, isCorrectlyAligned(this)); }
};

BSLA_MAYBE_UNUSED
bool operator==(const OveralignedObj&, const OveralignedObj&) { return true; }
BSLA_MAYBE_UNUSED
bool operator!=(const OveralignedObj&, const OveralignedObj&) { return false; }

/// An allocator-aware test type that tracks its construction and
/// destruction.  To enable exception testing, this class allocates memory
/// (i.e., construction can fail with an out-of-memory exception generated
/// by a test allocator).
class TestType {

    static int s_ctorCount;
    static int s_dtorCount;

    bsl::polymorphic_allocator<long>  d_allocator;
    long                             *d_valuePtr; // holds sum of ctor args

    /// Return a pointer to storage obtained from the allocator holding the
    /// specified `v` value.
    long *makeValue(long v = 0);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestType, bslmf::UsesAllocatorArgT);

    // TYPES
    typedef bsl::polymorphic_allocator<> allocator_type;

    // CLASS METHODS
    static void clearCounts() { s_ctorCount = s_dtorCount = 0; }
    static int ctorCount() { return s_ctorCount; }
    static int dtorCount() { return s_dtorCount; }

    // CREATORS
    explicit TestType(long a01 = 0, long a02 = 0, long a03 = 0, long a04 = 0,
                      long a05 = 0, long a06 = 0, long a07 = 0, long a08 = 0,
                      long a09 = 0, long a10 = 0, long a11 = 0, long a12 = 0,
                      long a13 = 0, long a14 = 0);
    TestType(bsl::allocator_arg_t, const allocator_type &alloc,
             long a01 = 0, long a02 = 0, long a03 = 0, long a04 = 0,
             long a05 = 0, long a06 = 0, long a07 = 0, long a08 = 0,
             long a09 = 0, long a10 = 0, long a11 = 0, long a12 = 0,
             long a13 = 0, long a14 = 0);

    TestType(const TestType& original)
        : d_allocator(), d_valuePtr(makeValue(original.value()))
        { ++s_ctorCount; }
    TestType(bsl::allocator_arg_t, const allocator_type &alloc,
             const TestType& original)
        : d_allocator(alloc), d_valuePtr(makeValue(original.value()))
        { ++s_ctorCount; }

    ~TestType();

    // ACCESSORS
    allocator_type get_allocator() const { return d_allocator; }
    long value() const { return *d_valuePtr; }
};

BSLA_MAYBE_UNUSED
bool operator==(const TestType& a, const TestType& b)
{
    return a.value() == b.value();
}

BSLA_MAYBE_UNUSED
bool operator!=(const TestType& a, const TestType& b)
{
    return a.value() != b.value();
}

int TestType::s_ctorCount = 0;
int TestType::s_dtorCount = 0;

TestType::TestType(long a01, long a02, long a03, long a04,
                   long a05, long a06, long a07, long a08,
                   long a09, long a10, long a11, long a12,
                   long a13, long a14)
    : d_allocator()
    , d_valuePtr(makeValue(a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 +
                           a09 +a10 + a11 + a12 + a13 + a14))
{
    ++s_ctorCount;
}

TestType::TestType(bsl::allocator_arg_t, const allocator_type &alloc,
                   long a01, long a02, long a03, long a04,
                   long a05, long a06, long a07, long a08,
                   long a09, long a10, long a11, long a12,
                   long a13, long a14)
    : d_allocator(alloc)
    , d_valuePtr(makeValue(a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 +
                           a09 +a10 + a11 + a12 + a13 + a14))
{
    ++s_ctorCount;
}

TestType::~TestType()
{
    d_allocator.deallocate(d_valuePtr, 1);
    ++s_dtorCount;
}

long *TestType::makeValue(long v)
{
    long *p = d_allocator.allocate(1);
    *p = v;
    return p;
}

// ============================================================================
//                  TEST CASE IMPLEMENTATIONS
// ----------------------------------------------------------------------------

/// Test `allocateBytes` and `deallocateBytes`
template <class ALLOCATOR, class POINTER>
void testAllocBytes(int line)
{
    static const std::size_t SIZES[] = { 1, 2, 3, 4, 8, 10, 16, 64, 256 };
    static const std::size_t NUM_SIZES = sizeof(SIZES) / sizeof(SIZES[0]);

    CheckedResource cr;
    ALLOCATOR alloc(&cr);

    for (std::size_t si = 0; si < NUM_SIZES; ++si) {
        const std::size_t nbytes = SIZES[si];

        // Test without specifying alignment (use natural alignment)
        {
            const std::size_t expAlign =
                bsls::AlignmentUtil::calculateAlignmentFromSize(nbytes);

            ASSERT(typeid(Util::allocateBytes(alloc, nbytes)) ==
                   typeid(POINTER));
            POINTER  block_p = Util::allocateBytes(alloc, nbytes);
            void    *raw_p   = rawPtr(block_p);
            ASSERTV(line, nbytes, expAlign, 0 != raw_p);
            ASSERTV(line, nbytes, expAlign, cr.numBlocksInUse() == 1);
            ASSERTV(line, nbytes, expAlign, cr.hasExpSize(raw_p, nbytes));
            ASSERTV(line, nbytes, expAlign,
                    cr.hasExpAlignment(raw_p, expAlign));

            Util::deallocateBytes(alloc, block_p, nbytes);
            ASSERTV(line, nbytes, expAlign, cr.numBlocksInUse() == 0);
            ASSERTV(line, nbytes, expAlign, cr.hasExpSize(raw_p, 0));
            ASSERTV(line, nbytes, expAlign, cr.hasExpAlignment(raw_p, 0));
        }

        // Test with explicit alignment
        for (std::size_t alignment = 1;
             alignment <= 2 * k_MAX_ALIGNMENT; alignment <<= 1) {

            ASSERT(typeid(Util::allocateBytes(alloc, nbytes, alignment)) ==
                   typeid(POINTER));
            POINTER  block_p = Util::allocateBytes(alloc, nbytes, alignment);
            void    *raw_p   = rawPtr(block_p);
            ASSERTV(line, nbytes, alignment, 0 != raw_p);
            ASSERTV(line, nbytes, alignment, cr.numBlocksInUse() == 1);
            ASSERTV(line, nbytes, alignment, cr.hasExpSize(raw_p, nbytes));
            ASSERTV(line, nbytes, alignment,
                    cr.hasExpAlignment(raw_p, alignment));

            Util::deallocateBytes(alloc, block_p, nbytes, alignment);
            ASSERTV(line, nbytes, alignment, cr.numBlocksInUse() == 0);
            ASSERTV(line, nbytes, alignment, cr.hasExpSize(raw_p, 0));
            ASSERTV(line, nbytes, alignment, cr.hasExpAlignment(raw_p, 0));
        }
    }
}

template <class ALLOCATOR, class TYPE, class POINTER>
void testAllocObjImp(int Tline, int Aline)
{
    static const std::size_t expAlign = bsls::AlignmentFromType<TYPE>::VALUE;

    CheckedResource cr;
    ALLOCATOR       alloc(&cr);

    ASSERT(typeid(Util::allocateObject<TYPE>(alloc)) == typeid(POINTER));

    // Allocate a single object
    POINTER      obj1_p  = Util::allocateObject<TYPE>(alloc);
    TYPE        *raw1_p  = rawPtr(obj1_p);
    std::size_t  expSize = sizeof(TYPE);
    ASSERTV(Tline, Aline, expSize, expAlign, (void*)raw1_p,
            isCorrectlyAligned(raw1_p));
    ASSERTV(Tline, Aline, expSize, expAlign, 0 != raw1_p);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() == 1);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw1_p, expSize));
    ASSERTV(Tline, Aline, expSize, expAlign,
            cr.hasExpAlignment(raw1_p, expAlign));

    // Allocate an array of three objects
    POINTER  obj2_p = Util::allocateObject<TYPE>(alloc, 3);
    TYPE    *raw2_p = rawPtr(obj2_p);
    expSize         = 3 * sizeof(TYPE);
    ASSERTV(Tline, Aline, expSize, expAlign, 0 != raw2_p);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() == 2);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw2_p, expSize));
    ASSERTV(Tline, Aline, expSize, expAlign,
            cr.hasExpAlignment(raw2_p, expAlign));

    // Deallocate first object
    Util::deallocateObject(alloc, obj1_p);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() == 1);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw1_p, 0));
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpAlignment(raw1_p, 0));

    // Deallocate array of 3 objects
    Util::deallocateObject(alloc, obj2_p, 3);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() == 0);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw2_p, 0));
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpAlignment(raw2_p, 0));
}

/// Test `allocateObject` and `deallocatObject`.
template <class TYPE>
void testAllocObj(int line)
{
#define TEST_IMP(ALLOC, PTR) testAllocObjImp<ALLOC, TYPE, PTR>(line, L_)

    //       Allocator type                   Ptr type
    //       ===============================  ========
    TEST_IMP(bslma::Allocator *             , TYPE *);
    TEST_IMP(bsl::allocator<int>            , TYPE *);
    TEST_IMP(bsl::polymorphic_allocator<int>, TYPE *);
    TEST_IMP(StlAllocator<int>              , TYPE *);
    TEST_IMP(DerivedAllocator<int>          , TYPE *);
    TEST_IMP(FancyAllocator<int>            , FancyPtr<TYPE>);

#undef TEST_IMP
}

template <class ALLOCATOR, class TYPE>
bool checkAlloc(const ALLOCATOR&, const TYPE&, bsl::false_type /* use-alloc */)
{
    return true;
}

template <class ALLOCATOR, class TYPE>
bool checkAlloc(const ALLOCATOR& alloc, const TYPE& obj,
                bsl::true_type /* use-alloc */)
{
    return alloc == obj.get_allocator();
}

template <class ALLOCATOR, class TYPE, class POINTER>
void testNewObjImp(int Tline, int Aline)
{
    const std::size_t expAlign = bsls::AlignmentFromType<TYPE>::VALUE;

    CheckedResource cr;
    ALLOCATOR alloc(&cr);

    ASSERT(typeid(Util::newObject<TYPE>(alloc)) == typeid(POINTER));

    POINTER      obj_p = Util::newObject<TYPE>(alloc);
    TYPE        *raw_p  = rawPtr(obj_p);
    std::size_t  expSize = sizeof(TYPE);
    ASSERTV(Tline, Aline, expSize, expAlign, 0 != raw_p);
    ASSERTV(Tline, Aline, expSize, expAlign, *obj_p == TYPE());
    ASSERTV(Tline, Aline, expSize, expAlign,
            checkAlloc(alloc, *obj_p, bsl::uses_allocator<TYPE, ALLOCATOR>()));
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() >= 1);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw_p, expSize));
    ASSERTV(Tline, Aline, expSize, expAlign,
            cr.hasExpAlignment(raw_p, expAlign));

    Util::deleteObject(alloc, obj_p);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.numBlocksInUse() == 0);
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpSize(raw_p, 0));
    ASSERTV(Tline, Aline, expSize, expAlign, cr.hasExpAlignment(raw_p, 0));
}

/// Test `allocateObject` and `deallocatObject`.
template <class TYPE>
void testNewObj(int line)
{
#define TEST_IMP(ALLOC, PTR) testNewObjImp<ALLOC, TYPE, PTR>(line, L_)

    //       Allocator type                   Ptr type
    //       ===============================  ========
    TEST_IMP(bslma::Allocator *             , TYPE *);
    TEST_IMP(bsl::allocator<int>            , TYPE *);
    TEST_IMP(bsl::polymorphic_allocator<int>, TYPE *);
    TEST_IMP(StlAllocator<int>              , TYPE *);
    TEST_IMP(DerivedAllocator<int>          , TYPE *);
    TEST_IMP(FancyAllocator<int>            , FancyPtr<TYPE>);
    g_ConstructingAllocCount = 0;
    TEST_IMP(ConstructingAlloc<int>         , TYPE *);
    ASSERTV(line, g_ConstructingAllocCount > 0);

#undef TEST_IMP
}

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: Future-proofing Member Construction
///- - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we construct an AA member variable, using
// `bslma::AllocatorUtil::adapt` so that it is both self-documenting and robust
// in case the member type is modernized from *legacy-AA* (using
// `bslma::Allocator *` directly in its interface) to *bsl-AA* (using
// `bsl::allocator` in its interface).
//
// First, we define a class, `Data1`, that has a legacy-AA interface:
// ```

    /// Legacy-AA data class.
    class Data1 {

        bslma::Allocator *d_allocator_p;
        // ...

      public:
        explicit Data1(bslma::Allocator *basicAllocator = 0)
            : d_allocator_p(basicAllocator) { /* ... */ }

        bslma::Allocator *allocator() const { return d_allocator_p; }
    };
// ```
// Next, we define a class, `MyClass1`, that has a member of type `Data1`.
// `MyClass` uses a modern, bsl-AA interface:
// ```
    class MyClass1 {
        bsl::allocator<char> d_allocator;
        Data1                d_data;

      public:
        typedef bsl::allocator<char> allocator_type;

        explicit MyClass1(const allocator_type& allocator = allocator_type());

        const Data1& data() const { return d_data; }
        allocator_type get_allocator() const { return d_allocator; }
    };
// ```
// Next, we define the constructor for `MyClass1`.  Since `MyClass1` uses
// `bsl::allocator` and the `Data1` uses `bslma::Allocator *`, we employ
// `bslma::AllocatorUtil::adapt` to obtain an allocator suitable for passing to
// the constructor for `d_data`:
// ```
    MyClass1::MyClass1(const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data(bslma::AllocatorUtil::adapt(allocator))
    {
    }
// ```
// Next, assume that we update our `Data` class from legacy-AA to bsl-AA
// (renamed from `Data1` to `Data2` for illustrative purposes):
// ```

    /// Bsl-AA data class.
    class Data2 {

        bsl::allocator<int> d_allocator;
        // ...

      public:
        typedef bsl::allocator<int> allocator_type;

        explicit Data2(const allocator_type& allocator = allocator_type())
            : d_allocator(allocator) { /* ... */ }

        allocator_type get_allocator() const { return d_allocator; }
    };
// ```
// Now, we notice that **nothing** about `MyClass` needs to change, not even
// the way its constructor passes an allocator to `d_data`:
// ```
    class MyClass2 {
        bsl::allocator<char> d_allocator;
        Data2                d_data;

      public:
        typedef bsl::allocator<char> allocator_type;

        explicit MyClass2(const allocator_type& allocator = allocator_type());

        const Data2& data() const { return d_data; }
        allocator_type get_allocator() const { return d_allocator; }
    };

    MyClass2::MyClass2(const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data(bslma::AllocatorUtil::adapt(allocator))
    {
    }
// ```
// Finally, we test both versions of `MyClass` and show that the allocator that
// is passed to the `MyClass` constructor gets forwarded to its data member:
// ```
    void usageExample1()
    {
        bslma::TestAllocator ta;
        bsl::allocator<char> alloc(&ta);

        MyClass1 obj1(alloc);
        ASSERT(&ta == obj1.data().allocator());

        MyClass2 obj2(alloc);
        ASSERT(alloc == obj2.data().get_allocator());
    }
// ```
//
///Example 2: Building an AA object on the heap
///- - - - - - - - - - - - - - - - - - - - - -
// This example shows how we can allocate a *bsl-AA* object from an allocator
// and construct the object, passing the allocator along, in one step.
//
// First, we define a simple class, `BslAAType`, that uses `bsl::allocator` to
// allocate memory (i.e., it is *bsl-AA*):
// ```
//  #include <bslma_bslallocator.h>
    class BslAAType {
        bsl::allocator<> d_allocator;
        int              d_value;

      public:
        typedef bsl::allocator<> allocator_type;

        explicit BslAAType(const allocator_type& a = allocator_type())
            : d_allocator(a), d_value(0) { }
        explicit BslAAType(int v, const allocator_type& a = allocator_type())
            : d_allocator(a), d_value(v) { }
        BslAAType(const BslAAType& other,
                  const allocator_type& a = allocator_type())
            : d_allocator(a), d_value(other.d_value) { }

        allocator_type get_allocator() const { return d_allocator; }
        int            value()         const { return d_value;     }
    };
// ```
// Now we can use `bslma::AllocatorUtil::newObject` to, in a single operation,
// allocate and construct an `BslAAType` object.  We can see that the right
// allocator and value are passed to the new object:
// ```
//  #include <bslma_testallocator.h>
    void usageExample2()
    {
        bslma::TestAllocator ta;
        BslAAType *p = bslma::AllocatorUtil::newObject<BslAAType>(&ta, 77);
        ASSERT(sizeof(BslAAType) == ta.numBytesInUse());
        ASSERT(77 == p->value());
        ASSERT(&ta == p->get_allocator().mechanism());
// ```
// Finally, we use `deleteObject` to destroy and return the object to the
// allocator:
        bslma::AllocatorUtil::deleteObject(&ta, p);
        ASSERT(0 == ta.numBytesInUse());
    }
// ```
//
///Example 3: Safe container swap
///- - - - - - - - - - - - - - -
// In this example, we see how `bslma::AllocatorUtil::swap` can be used to swap
// allocators without the risk of calling a non-existant swap.
//
// First, we create a class, `StdAAType`, that uses any valid STL-compatible
// allocator (i.e., it is *stl-AA*).  Note that this class has non-default copy
// constructor and assignment operations (whose implementation is not shown)
// and a non-default `swap` operation:
// ```
    template <class TYPE, class ALLOCATOR = bsl::allocator<TYPE> >
    class StlAAType {
        ALLOCATOR  d_allocator;
        TYPE      *d_value_p;

      public:
        typedef ALLOCATOR allocator_type;

        explicit StlAAType(const allocator_type& a = allocator_type())
            : d_allocator(a)
            , d_value_p(bslma::AllocatorUtil::newObject<TYPE>(a)) { }
        explicit StlAAType(const TYPE&           v,
                           const allocator_type& a = allocator_type())
            : d_allocator(a)
            , d_value_p(bslma::AllocatorUtil::newObject<TYPE>(a, v)) { }

        StlAAType(const StlAAType&);

        ~StlAAType() {
            bslma::AllocatorUtil::deleteObject(d_allocator, d_value_p);
        }

        StlAAType operator=(const StlAAType&);

        void swap(StlAAType& other);

        allocator_type get_allocator() const { return d_allocator; }
        const TYPE&    value()         const { return *d_value_p;  }
    };

    template <class TYPE, class ALLOCATOR>
    inline void swap(StlAAType<TYPE, ALLOCATOR>& a,
                     StlAAType<TYPE, ALLOCATOR>& b)
    {
        a.swap(b);
    }
// ```
// Next, we write the `swap` member function.  This function should follow our
// standard AA rule for member swap: if the allocators compare equal or if the
// allocators should propagate on swap, then perform a fast swap, moving only
// pointers and (possibly) allocators, rather than copying elements; otherwise
// revert to element-by-element swap:
// ```
    template <class TYPE, class ALLOCATOR>
    void StlAAType<TYPE, ALLOCATOR>::swap(StlAAType& other)
    {
        typedef typename
            bsl::allocator_traits<allocator_type>::propagate_on_container_swap
            Propagate;

        using std::swap;

        if (Propagate::value || d_allocator == other.d_allocator) {
            // Swap allocators and pointers, but not individual elements.
            bslma::AllocatorUtil::swap(&d_allocator, &other.d_allocator,
                                       Propagate());
            swap(d_value_p, other.d_value_p);
        }
        else
        {
            // Swap element values
            swap(*d_value_p, *other.d_value_p);
        }
    }
// ```
// Note that, in the above implementation of `swap`, that we swap the
// allocators using `bslma::AllocatorUtil::swap` instead of calling `swap`
// directly.  If the `ALLOCATOR` type does not propagate on container
// assignment or swap, the allocator itself is not required to support
// assignment or swap.  By using this utility, we avoid trying to compile a
// call to allocator `swap` when it is not needed.
//
// Next, we'll define an allocator that illustrates this point.  Our `MyAlloc`
// allocator does not support allocator propogation and deletes the assignment
// operators (thus also disabling swap):
// ```
//  #include <bsls_keyword.h>
    template <class TYPE>
    class MyAlloc {
        bsl::allocator<TYPE> d_imp;

        // Disable assignment
        MyAlloc operator=(const MyAlloc&) BSLS_KEYWORD_DELETED;

      public:
        typedef TYPE value_type;

        MyAlloc() { }
        MyAlloc(bslma::Allocator *allocPtr) : d_imp(allocPtr) { }   // IMPLICIT
        MyAlloc(const MyAlloc& other) : d_imp(other.d_imp) { }
        template <class U>
        MyAlloc(const MyAlloc<U>& other) : d_imp(other.d_imp) { }

        TYPE *allocate(std::size_t n) { return d_imp.allocate(n); }
        void deallocate(TYPE* p, std::size_t n) { d_imp.deallocate(p, n); }

        template <class T2>
        friend bool operator==(const MyAlloc& a, const MyAlloc<T2>& b)
            { return a.d_imp == b.d_imp; }
        template <class T2>
        friend bool operator!=(const MyAlloc& a, const MyAlloc<T2>& b)
            { return a.d_imp != b.d_imp; }

        void construct(TYPE *p) { ::new (static_cast<void *>(p)) TYPE(); }

        template <class ARG>
        void construct(TYPE *p, const ARG& arg)
            { ::new (static_cast<void *>(p)) TYPE(arg); }
    };
// ```
// Finally, we create two `StlAAType` objects with the same allocator and show
// that they can be swapped even though the allocator type cannot be swapped:
// ```
    void usageExample3()
    {
        MyAlloc<int> alloc;

        StlAAType<int, MyAlloc<int> > objA(1, alloc), objB(2, alloc);
        ASSERT(alloc == objA.get_allocator());
        ASSERT(alloc == objB.get_allocator());
        ASSERT(1 == objA.value());
        ASSERT(2 == objB.value());

        objA.swap(objB);
        ASSERT(2 == objA.value());
        ASSERT(1 == objB.value());
    }
// ```

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. That the usage examples shown in the component-level
        //    documentation compile and run as described.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, changing
        //    `assert` to `ASSERT` and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();
        usageExample2();
        usageExample3();

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING `swap`
        //
        // Concerns:
        // 1. Calling `swap` with two arguments that compare equal before the
        //    call and a `false_type` last argument succeeds and has no effect.
        // 2. Calling `swap` with two arguments that do not compare equal
        //    before the call and a `false_type` last argument is detected as a
        //    precondition violation in certain build modes.
        // 3. When the last argument of `swap` is `false_type`, the
        //    `swap` free function for the allocator type is not called (and
        //    need not be callable).
        // 4. Calling `swap` with a last argument of `true_type` will invoke
        //    the the ADL free function `swap` for the first two arguments.
        //    Note that compilation will fail if the type cannot be swapped.
        //
        // Plan:
        // 1. Given two objects `a` and `b` that compare equal, verify that
        //    `AllocatorUtil::swap(&a, &b, false_type())` has no effect.  (C-1)
        // 2. (Negative test) Given two objects `a` and `b` that do not compare
        //    equal, verify that `swap(&a, b, false_type())` fails with a
        //    precondition violation in a non-optimized build.  (C-2)
        // 3. Repeat steps 1 and 2 with a type that cannot be swapped.  Verify
        //    that compilation succeeds and the results are the same.  (C-3)
        // 4. Given two objects `a` and `b` that do not compare equal, verify
        //    that `swap(&a, &b, false_type())` invokes `swap(a, b)`.  Note
        //    that this step would fail to compile if the objects are not
        //    swappable (not testable).
        //
        // Testing:
        //    TYPE& swap(TYPE *, TYPE*, bsl::false_type);
        //    TYPE& swap(TYPE *, TYPE*, bsl::true_type);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `swap`"
                            "\n==============\n");

        const bsl::true_type  trueV;
        const bsl::false_type falseV;

        AssignableAllocator    a1a(1), a1b(1), a2(2);
        NonAssignableAllocator na1a(3), na1b(3), na2(4);

        ASSERT_PASS(Util::swap(&a1a, &a1b, falseV));
        ASSERT(1 == a1a.id());
        ASSERT(1 == a1b.id());
        ASSERT_PASS(Util::swap(&na1a, &na1b, falseV));
        ASSERT(3 == na1a.id());
        ASSERT(3 == na1b.id());

        {
            bsls::AssertTestHandlerGuard g;
            ASSERT_FAIL(Util::swap(&a1a, &a2, falseV));
            ASSERT_FAIL((Util::swap(&na1a, &na2, falseV)));
        }
        ASSERT(1 == a1a.id());
        ASSERT(2 == a2.id());
        ASSERT(3 == na1a.id());
        ASSERT(4 == na2.id());

        Util::swap(&a1a, &a1b, trueV);
        ASSERT(1 == a1a.id());
        ASSERT(1 == a1b.id());
        Util::swap(&a1a, &a2, trueV);
        ASSERT(2 == a1a.id());
        ASSERT(1 == a2.id());

        // The following should fail to compile or link
        // Util::swap(&na1a, &na1b, trueV);
        // Util::swap(&na1a, &na2, trueV);

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING `assign`
        //
        // Concerns:
        // 1. Calling `assign` with two arguments that compare equal before the
        //    call and a `false_type` last argument succeeds and has no effect.
        // 2. Calling `assign` with two arguments that do not compare equal
        //    before the call and a `false_type` last argument is detected as a
        //    precondition violation in certain build modes.
        // 3. When the last argument of `assign` is `false_type`, the
        //    assignment operator for the allocator type is not called (and
        //    need not exist).
        // 4. Calling `assign` with a last argument of `true_type` will invoke
        //    the assignment operator for the allocators.  Note that
        //    compilation will fail if the assignment operator is deleted or
        //    private.
        //
        // Plan:
        // 1. Given two objects `a` and `b` that compare equal, verify that
        //    `assign(&a, b, false_type())` has no effect.  (C-1)
        // 2. (Negative test) Given two objects `a` and `b` that do not compare
        //    equal, verify that `assign(&a, b, false_type())` fails with a
        //    precondition violation in a non-optimized build.  (C-2)
        // 3. Repeat steps 1 and 2 with a type that has a deleted assignment
        //    operator.  Verify that compilation succeeds and the results are
        //    the same.  (C-3)
        // 4. Given two objects `a` and `b` that do not compare equal, verify
        //    that `assign(&a, b, false_type())` invokes `a = b`.  Note that
        //    this step would not compile if `a` and `b` have a deleted
        //    assignment operator (not testable).
        //
        // Testing:
        //    TYPE& assign(TYPE *, const TYPE&, bsl::true_type);
        //    TYPE& assign(TYPE *, const TYPE&, bsl::false_type);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `assign`"
                            "\n================\n");

        const bsl::true_type  trueV;
        const bsl::false_type falseV;

        AssignableAllocator    a1a(1), a1b(1), a2(2);
        NonAssignableAllocator na1a(3), na1b(3), na2(4);

        ASSERT_PASS(Util::assign(&a1a, a1b, falseV));
        ASSERT(1 == a1a.id());
        ASSERT_PASS(Util::assign(&na1a, na1b, falseV));
        ASSERT(3 == na1a.id());

        {
            bsls::AssertTestHandlerGuard g;
            ASSERT_FAIL(Util::assign(&a1a, a2, falseV));
            ASSERT_FAIL((Util::assign(&na1a, na2, falseV)));
        }
        ASSERT(1 == a1a.id());
        ASSERT(3 == na1a.id());

        Util::assign(&a1a, a1b, trueV);
        ASSERT(1 == a1a.id());
        Util::assign(&a1a, a2, trueV);
        ASSERT(2 == a1a.id());

        // The following should not compile
        // Util::assign(&na1a, na1b, trueV);
        // Util::assign(&na1a, na2, trueV);

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING `newObject` AND `deleteObject`
        //
        // Concerns:
        // 1. The `newObject` method allocates a block of sufficient size
        //    to hold an object of the specified type and attempts honor the
        //    object's alignment requirements.
        // 2. The `newObject` method initializes passes its arguments through
        //    to the object's constructor and returns a pointer to the
        //    initialized object.
        // 3. The returned object is constructed via a call to the `construct`
        //    method of the allocator (if any).  Specifically, if the allocator
        //    is a scoped allocator (such as `bsl::polymorphic_allocator`,
        //    `bsl::allocator` or `bslma::Allocator *`) and the type being
        //    constructed uses a compatible allocator, the allocator is passed
        //    to the constructor of the new object.
        // 4. The `deleteObject` method calls the object destructor then
        //    returns the memory block back to the allocator.
        // 5. The previous concerns apply to any type of allocator, including
        //    `bslma::Allocator *`, `bsl::allocator`, and
        //    `bsl::polymorphic_allocator`.
        // 6. The pointer type returned by `allocateObject` and accepted by
        //    `deleteObject` matches the pointer type for the specified
        //    allocator, including for STL-style allocators that use fancy
        //    pointers.
        // 7. If an exception is thrown by the type's constructor, no memory is
        //    leaked.
        //
        // Plan:
        // 1. Using a special test resource (`CheckedResource`), to initialize
        //    an allocator, test that the specified byte count and alignment
        //    are correctly passed to the allocator by a call to `newObject`
        //    and that a pointer to the allocated block is returned by
        //    `allocateObject`.  Verify that the object was constructed using
        //    the allocator's `construct` method, if any.  (C-1, C-2, C-3)
        // 2. Given the object pointer returned in step one, call
        //    `deleteObject` and verify that the destructor is called and that
        //    the pointer, size, and alignment are correctly passed to the
        //    resource.  (C-4)
        // 3. Repeat the above steps with a representative sample of object
        //    sizes and alignments, including alignments greater than the
        //    largest native platform alignment (on platforms that support
        //    `alignas`).  (C-1)
        // 4. Repeat the above steps and with a wide variety of allocators and
        //    types being allocated.  (C-5, C-6)
        // 5. Using a test type that takes 0 - 14 integer constructor
        //    arguments, verify that the constructor arguments are passed to
        //    the constructor.  (C-2)
        // 6. Perform step 5 within an EXCEPTION_TEST_BEGIN/END pair.  Verify
        //    that no memory leaks when an exception is thrown.  (C-7)
        //
        // Testing:
        //    pointer newObject<TYPE&>(const ALLOCATOR&, ARGS&&...);
        //    void deleteObject(const ALLOCATOR&, pointer);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `newObject` AND `deleteObject`"
                   "\n======================================\n");

        testNewObj<char          >(L_);
        testNewObj<int           >(L_);
        testNewObj<double        >(L_);
        testNewObj<OveralignedObj>(L_);

        TestType::clearCounts();
        testNewObj<TestType    >(L_);
        ASSERT(TestType::ctorCount() > 0);
        ASSERT(TestType::dtorCount() > 0);
        ASSERT(TestType::ctorCount() == TestType::dtorCount());

        CheckedResource ca;

#define TEST(EXP, ...) do {                                                   \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ca) {                        \
            StlAllocator<int> stlAlloc(&ca);                                  \
            TestType *t1 = Util::newObject<TestType>(stlAlloc, __VA_ARGS__);  \
            ASSERT(EXP == t1->value());                                       \
            ASSERT(bsl::allocator<>() == t1->get_allocator());                \
            Util::deleteObject(stlAlloc, t1);                                 \
            bsl::allocator<int> bslAlloc(&ca);                                \
            TestType *t2 = Util::newObject<TestType>(bslAlloc, __VA_ARGS__);  \
            ASSERT(EXP == t2->value());                                       \
            ASSERT(&ca == t2->get_allocator());                               \
            Util::deleteObject(bslAlloc, t2);                                 \
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                              \
    } while (false)

        //    Exp
        //   Value   Ctor Arguments
        //   ------  --------------
        TEST(0x0001, 0x0001);
        TEST(0x0003, 0x0001, 0x0002);
        TEST(0x0007, 0x0001, 0x0002, 0x0004);
        TEST(0x000f, 0x0001, 0x0002, 0x0004, 0x0008);
        TEST(0x001f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010);
        TEST(0x003f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020);
        TEST(0x007f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040);
        TEST(0x00ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080);
        TEST(0x01ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100);
        TEST(0x03ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200);
        TEST(0x07ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400);
        TEST(0x0fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800);
        TEST(0x1fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000);
        TEST(0x3fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000);
#undef TEST

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING `allocateObject` AND `deallocateObject`
        //
        // Concerns:
        // 1. The `allocateObject` method allocates a block of sufficient size
        //    to hold the number of objects and attempts honor the
        //    object's alignment requirements.
        // 2. The `deallocateObject` method returns the memory block back to
        //    the allocator.
        // 3. The previous concerns apply for any object size and alignment
        //    and alignment.
        // 4. If the allocator cannot honor the specified alignment, the
        //    maximum platform alignment is used.
        // 5. The previous concerns apply to any type of allocator, including
        //    `bslma::Allocator *`, `bsl::allocator`,
        //    `bsl::polymorphic_allocator`.
        // 6. The pointer type returned by `allocateObject` and accepted by
        //    `deallocateObject` matches the pointer type for the specified
        //    allocator, including for STL-style allocators that use fancy
        //    pointers.
        // 7. No constructors or destructors are called.
        //
        // Plan:
        // 1. Using a special test resource (`CheckedResource`), to initialize
        //    an allocator, test that the specified byte count and alignment
        //    are correctly passed to the allocator by a call to
        //    `allocateObject` and that a pointer to the allocated block is
        //    returned by `allocateObject`.  (C-1)
        // 2. Given the object pointer returned in step one, call
        //    `deallocateObject` and verify that the pointer, size, and
        //    alignment are correctly passed to the resource.  (C-2)
        // 3. Repeat the above steps with a representative sample of object
        //    sizes and alignments, including alignments greater than the
        //    largest native platform alignment (on platforms that support
        //    `alignas`).  (C-3, C-4)
        // 4. Repeat the above steps and with a wide variety of allocators and
        //    types being allocated.  For allocators that use fancy pointers,
        //    verify that the returned pointer type is the appropriate void
        //    pointer type.  (C-5, C-6)
        // 5. Among the types checked in step 4, include one that tracks it
        //    constructor and destructor calls.  Verify that no constructor or
        //    destructor is called.
        //
        // Testing:
        //    pointer allocateObject(const ALLOCATOR&, size_t);;
        //    void deallocateObject(const ALLOCATOR&, pointer, size_t);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `allocateObject` AND `deallocateObject`"
                   "\n===============================================\n");

        testAllocObj<char          >(L_);
        testAllocObj<int           >(L_);
        testAllocObj<double        >(L_);
        testAllocObj<OveralignedObj>(L_);

        TestType::clearCounts();
        testAllocObj<TestType    >(L_);
        ASSERT(TestType::ctorCount() == 0);
        ASSERT(TestType::dtorCount() == 0);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING `allocateBytes` AND `deallocateBytes`
        //
        // Concerns:
        // 1. The `allocateBytes` method allocates the specified number of
        //    bytes and attempts honor the specified alignment.  If alignment
        //    is not specified, natural alignment is used.
        // 2. The `deallocateBytes` method returns the bytes back to the
        //    allocator.
        // 3. The previous concerns apply for any reasonable number of bytes
        //    and alignment.
        // 4. If the allocator cannot honor the specified alignment, the
        //    maximum platform alignment is used.
        // 5. The previous concerns apply to any type of allocator, including
        //    `bslma::Allocator *`, `bsl::allocator`, and
        //    `bsl::polymorphic_allocator`.
        // 6. The pointer type returned by `allocateBytes` and accepted by
        //    `deallocateBytes` matches the void pointer type for the specified
        //    allocator, including for STL-style allocators that use fancy
        //    pointers.
        //
        // Plan:
        // 1. Using a special test resource (`CheckedResource`), to initialize
        //    an allocator, test that the specified byte count and alignment
        //    are correctly passed to the allocator by a call to
        //    `allocateBytes` and that a pointer to the allocated block is
        //    returned by `allocateBytes`.  (C-1)
        // 2. Given the block pointer returned in step one, call
        //    `deallocateBytes` and verify that the pointer, size, and
        //    alignment are correctly passed to the allocator.  (C-2)
        // 3. Repeat the above steps with a representative sample of sizes and
        //    alignments, including alignments greater than the largest native
        //    platform alignment.  (C-3, C-4)
        // 4. Repeat the above steps and with a wide variety of allocators
        //    instantiated with at least two value types.  For allocators that
        //    use fancy pointers, verify that the returned pointer type is the
        //    appropriate void pointer type.  (C-5, C-6)
        //
        // Testing:
        //    void_pointer allocateBytes(const ALLOC&, size_t, size_t);
        //    void deallocateBytes(const ALLOC&, void_pointer, size_t, size_t);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `allocateBytes` AND `deallocateBytes`"
                   "\n=============================================\n");

        //             Allocator type                    Ptr type
        //             ================================  ========
        testAllocBytes<bslma::Allocator *              , void *>(L_);
        testAllocBytes<bsl::allocator<char>            , void *>(L_);
        testAllocBytes<bsl::allocator<float>           , void *>(L_);
        testAllocBytes<bsl::polymorphic_allocator<char>, void *>(L_);
        testAllocBytes<bsl::polymorphic_allocator<int> , void *>(L_);
        testAllocBytes<StlAllocator<char>              , void *>(L_);
        testAllocBytes<StlAllocator<const char *>      , void *>(L_);
        testAllocBytes<DerivedAllocator<char>          , void *>(L_);
        testAllocBytes<DerivedAllocator<double>        , void *>(L_);
        testAllocBytes<FancyAllocator<char>            , FancyPtr<void> >(L_);
        testAllocBytes<FancyAllocator<short>           , FancyPtr<void> >(L_);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING `adapt`
        //
        // Concerns:
        // 1. For allocators of type `bslma::Allocator *` or specializations of
        //    `bsl::allocator`, the return value of `adapt` is convertible to
        //    both `bslma::Allocator *` and `bsl::allocator<T>` (for arbitrary
        //    `T`).
        // 2. For allocators of type `bslma::Allocator *` or specializations of
        //    `bsl::allocator`, the return value of `adapt` can be passed as
        //    the allocator argument for direct initializing an object of
        //    bsl-AA or legacy-AA type.
        // 3. Concerns 1 and 2 apply equally to allocators of type derived from
        //    `bsl::allocator` or pointer to a type derived from
        //    `bslma::Allocator`.
        // 4. For all other allocator types, the return value of `adapt` can be
        //    used to initialize a an object in the same allocator family and
        //    used to initialize an AA object having a compatible
        //    `allocator_type`.
        //
        // Plan:
        // 1. Using `alloc` arguments of type `bslma::Allocator *` and
        //    `bsl::allocator<short>`, call `adapt(alloc)` and use the return
        //    value to copy-initialize an object of type `bslma::Allocator *`
        //    and an object of type `bsl:allocator<int>`.  Verify that the
        //    constructed object represents the same allocator as `alloc`.
        //    (C-1)
        // 2. With the same arguments as step 1, use the return value of
        //    `adapt` to direct-initialize an object of legacy-AA type and an
        //    object of bsl-AA type.  Verify that the allocator stored by the
        //    constructed object represents the same allocator as was passed to
        //    `adapt`.  (C-2)
        // 3. Repeat steps 1 and 2 with arguments of type 'bslma::TestAllocator
        //    *` and `DerivedAllocator<float>`, where `DerivedAllocator<float>'
        //    is derived from `bsl::allocator<float>`.  (C-3)
        // 4. Pass an object of a non-bsl allocator type to `adapt` and use the
        //    return value to initialize an allocator object belonging to the
        //    allocator same family.  Verify that the input and output
        //    represent the same result.  Initialize an object of stl-AA type
        //    with the return value of `adapt`.  Verify that the expected value
        //    was used as its allocator.  (C-4)
        //
        // TESTING
        //      bslma::Allocator *adapt(const bsl::allocator<TYPE>&);
        //      ALLOCATOR         adapt(const ALLOCATOR&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `adapt`"
                            "\n===============\n");

        bslma::TestAllocator    ta1, ta2, ta3, ta4;
        bslma::Allocator       *a1 = &ta1;
        bsl::allocator<short>   a2(&ta2);
        DerivedAllocator<float> a3(&ta3);
        StlAllocator<bool>      a4(&ta4);

        // Step 1
        ASSERT(typeid(bslma::Allocator *) == typeid(Util::adapt(a1)));
        ASSERT(a1 == Util::adapt(a1));
        ASSERT(typeid(bslma::Allocator *) == typeid(Util::adapt(a2)));
        ASSERT(a2.mechanism() == Util::adapt(a2));

        bsl::allocator<int> baa = Util::adapt(a1);
        ASSERT(a1 == baa.mechanism());
        bsl::allocator<int> bab = Util::adapt(a2);
        ASSERT(a2 == bab);

        // Step 2
        LegacyAAClass lca(Util::adapt(a1));
        ASSERT(a1 == lca.allocator());
        LegacyAAClass lcb(Util::adapt(a2));
        ASSERT(a2.mechanism() == lcb.allocator());

        BslAAClass bca(Util::adapt(a1));
        ASSERT(a1 == bca.get_allocator().mechanism());
        BslAAClass bcb(Util::adapt(a2));
        ASSERT(a2 == bcb.get_allocator());

        // Step 3
        ASSERT(typeid(bslma::TestAllocator *) == typeid(Util::adapt(&ta1)));
        bslma::Allocator *apc = Util::adapt(&ta1);
        ASSERT(&ta1 == apc);
        ASSERT(typeid(bslma::Allocator *) == typeid(Util::adapt(a3)));
        bslma::Allocator *apd = Util::adapt(a3);
        ASSERT(a3.mechanism() == apd);
        bsl::allocator<int> bac = Util::adapt(&ta1);
        ASSERT(&ta1 == bac.mechanism());
        bsl::allocator<int> bad = Util::adapt(a3);
        ASSERT(a3 == bad);

        LegacyAAClass lcc(Util::adapt(&ta1));
        ASSERT(&ta1 == lcc.allocator());
        LegacyAAClass lcd(Util::adapt(a3));
        ASSERT(a3.mechanism() == lcd.allocator());
        BslAAClass bcc(Util::adapt(&ta1));
        ASSERT(&ta1 == bcc.get_allocator().mechanism());
        BslAAClass bcd(Util::adapt(a3));
        ASSERT(a3 == bcd.get_allocator());

        // Step 4
        ASSERT(typeid(StlAllocator<bool> ) == typeid(Util::adapt(a4)));
        StlAllocator<char> saa = Util::adapt(a4);
        ASSERT(a4 == saa);

        StlAAClass sab(Util::adapt(a4));
        ASSERT(a4 == sab.get_allocator());

      } break;

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
        // 1. Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const bsl::true_type  TRUE_V  = { };
        const bsl::false_type FALSE_V = { };

        // Test `adapt`
        {
            bslma::TestAllocator ta;
            bsl::allocator<int>  alloc(&ta);

            bsl::allocator<char> a1 = Util::adapt(&ta);
            ASSERT(&ta == a1.mechanism());

            bslma::Allocator *a2p = Util::adapt(alloc);
            ASSERT(alloc.mechanism() == a2p);
        }

        // Test `allocateBytes` and `deallocateBytes`
        {
            bslma::TestAllocator ta;
            void *p = Util::allocateBytes(&ta, 12);
            ASSERT(12 == ta.numBytesInUse());
            Util::deallocateBytes(&ta, p, 12);
            ASSERT(0 == ta.numBytesInUse());

            bsl::polymorphic_allocator<int> pa(&ta);
            p = Util::allocateBytes(pa, 32);
            ASSERT(32 == ta.numBytesInUse());
            Util::deallocateBytes(pa, p, 32);
            ASSERT(0 == ta.numBytesInUse());

            FancyAllocator<int> fa(&ta);
            FancyPtr<void> fp = Util::allocateBytes(fa, 40, 8);
            ASSERT(40 == ta.numBytesInUse());
            Util::deallocateBytes(fa, fp, 40, 8);
            ASSERT(0 == ta.numBytesInUse());
        }

        // Test `allocateObject` and `deallocateObject`
        {
            bslma::TestAllocator ta;
            int *p = Util::allocateObject<int>(&ta, 2);
            ASSERT(2 * sizeof(int) == ta.numBytesInUse());
            Util::deallocateObject(&ta, p, 2);
            ASSERT(0 == ta.numBytesInUse());

            bsl::polymorphic_allocator<short> pa(&ta);
            p = Util::allocateObject<int>(pa);
            ASSERT(sizeof(int) == ta.numBytesInUse());
            Util::deallocateObject(pa, p);
            ASSERT(0 == ta.numBytesInUse());

            FancyAllocator<short> fa(&ta);
            FancyPtr<int> fp = Util::allocateObject<int>(fa, 3);
            ASSERT(3 * sizeof(int) == ta.numBytesInUse());
            Util::deallocateObject(fa, fp, 3);
            ASSERT(0 == ta.numBytesInUse());
        }

        // Test `newObject` and `deleteObject`
        {
            bslma::TestAllocator ta;
            int *p = Util::newObject<int>(&ta);
            ASSERT(sizeof(int) == ta.numBytesInUse());
            ASSERT(0 == *p);
            Util::deleteObject(&ta, p);
            ASSERT(0 == ta.numBytesInUse());

            bsl::polymorphic_allocator<float> pa(&ta);
            p = Util::newObject<int>(pa, 99);
            ASSERT(sizeof(int) == ta.numBytesInUse());
            ASSERT(99 == *p);
            Util::deleteObject(pa, p);
            ASSERT(0 == ta.numBytesInUse());

            FancyAllocator<short> fa(&ta);
            FancyPtr<int>         fp = Util::newObject<int>(fa, 99);
            ASSERT(sizeof(int) == ta.numBytesInUse());
            ASSERT(99 == *fp);
            Util::deleteObject(fa, fp);
            ASSERT(0 == ta.numBytesInUse());
        }

        // Test `assign`
        {
            AssignableAllocator aa1(1), aa2(2);
            ASSERT(1 == aa1.id());
            ASSERT(2 == aa2.id());
            AssignableAllocator& aret = Util::assign(&aa1, aa2, TRUE_V);
            ASSERT(2 == aa1.id());
            ASSERT(2 == aa2.id());
            ASSERT(&aret == &aa1);

            NonAssignableAllocator naa1(3), naa2(3);
            ASSERT(3 == naa1.id());
            ASSERT(3 == naa2.id());
            NonAssignableAllocator& naret = Util::assign(&naa1, naa2, FALSE_V);
            ASSERT(3 == naa1.id());
            ASSERT(3 == naa2.id());
            ASSERT(&naret == &naa1);
        }

        // Test `swap`
        {
            AssignableAllocator aa1(1), aa2(2);
            ASSERT(1 == aa1.id());
            ASSERT(2 == aa2.id());
            Util::swap(&aa1, &aa2, TRUE_V);
            ASSERT(2 == aa1.id());
            ASSERT(1 == aa2.id());

            NonAssignableAllocator naa1(3), naa2(3);
            ASSERT(3 == naa1.id());
            ASSERT(3 == naa2.id());
            Util::swap(&naa1, &naa2, FALSE_V);
            ASSERT(3 == naa1.id());
            ASSERT(3 == naa2.id());
        }

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
// Copyright 2022 Bloomberg Finance L.P.
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
