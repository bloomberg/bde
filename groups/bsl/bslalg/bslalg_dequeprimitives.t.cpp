// bslalg_dequeprimitives.t.cpp                                       -*-C++-*-

#include <bslalg_dequeprimitives.h>
#include <bslalg_dequeiterator.h>

#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>

#include <bslma_allocator.h>              // for testing only
#include <bslma_default.h>                // for testing only
#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only
#include <bsls_alignmentutil.h>           // for testing only
#include <bsls_bsltestutil.h>             // for testing only
#include <bsls_objectbuffer.h>            // for testing only
#include <bsls_platform.h>                // for testing only
#include <bsls_stopwatch.h>               // for testing only
#include <bsls_types.h>                   // for testing only

#include <ctype.h>     // 'isalpha'
#include <stdio.h>
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strlen'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides basic algorithms on ranges, taking great
// care to define behavior in case of exceptions.  This test driver verifies
// all this behavior, including the behavior on exception by using a test class
// that may throw on construction or assignment, depending on whether
// allocation succeeds or not.  The testing is rigorous and includes making
// sure that the correct implementation is selected according to the traits of
// the class to be moved, by verifying that bitwise copy was used rather than
// construction, assignment, or destruction, whenever traits demand so.
// Finally, one more concern is with the possible overloading ambiguity when
// using 'FWD_ITER' instead of pointer types to specify an input range.  We
// verify that there is no overloading ambiguity by instantiating (but not
// running) a comprehensive selection of overloads, taking care to have
// standard as well as user conversions in the candidate selection.
//
// In order to facilitate the generation of test object instances, we make a
// text object have the value semantics of a 'char', and generate an deque of
// test objects from a string specification via a generating function
// parameterized by the actual test object type.  This lets us reuse the same
// test code for bitwise-copyable/moveable test types as well as those that do
// not have those traits.
//-----------------------------------------------------------------------------
// [ 2] void destruct(Itr beg, Itr end);
// [ 3] Itr erase(Itr *toBeg, Itr *toEnd, Itr fromBeg,
//                Itr      f,      Itr l, Itr fromEnd, bslma::Allocator *a)
// [ 4] void uninitializedFillNBack(Itr *toEnd, Itr fromEnd, size_type n,
//                                  const T& v, bslma::Allocator *a)
// [ 5] void uninitializedFillNFront(Itr *toBeg, Itr fromBeg, size_type n,
//                                   const T& v, bslma::Allocator *a)
// [ 6] void insertAndMoveToBack(Itr *toEnd, Itr fromEnd, Itr p, size_type n,
//                               const T& v, bslma::Allocator *a)
// [ 7] void insertAndMoveToFront(Itr *toBeg, Itr fromBeg, Itr p, size_type n,
//                                const T& v, bslma::Allocator *a)
// [ 8] void insertAndMoveToBack(Itr *toEnd, Itr fromEnd, Itr p, FWD_ITR f,
//                               FWD_ITR l, size_type n, bslma::Allocator *a)
// [ 9] void insertAndMoveToFront(Itr *toEnd, Itr fromEnd, Itr p, FWD_ITR f,
//                                FWD_ITR l, size_type n, bslma::Allocator *a)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

//=============================================================================
//                  STANDARD bslma EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_bslma_EXCEPTION_TEST {                                         \
    {                                                                        \
        static int firstTime = 1;                                            \
        if (g_veryVerbose && firstTime) printf(                              \
            "### bslma EXCEPTION TEST -- (ENABLED) --\n");                   \
        firstTime = 0;                                                       \
    }                                                                        \
    if (g_veryVeryVerbose) printf("### Begin bslma exception test.\n");      \
    int bslmaExceptionCounter = 0;                                           \
    static int bslmaExceptionLimit = 1000;                                   \
    testAllocator.setAllocationLimit(bslmaExceptionCounter);                 \
    do {                                                                     \
        try {

#define END_bslma_EXCEPTION_TEST                                             \
        } catch (bslma::TestAllocatorException& e) {                         \
            if ((g_veryVerbose && bslmaExceptionLimit) || g_veryVeryVerbose){\
                --bslmaExceptionLimit;                                       \
                printf("(*** %d)", bslmaExceptionCounter);                   \
                if (g_veryVeryVerbose) {                                     \
                    printf(" bslma::EXCEPTION:"                              \
                           " alloc limit = %d,",                             \
                           bslmaExceptionCounter);                           \
                    printf(" last alloc size = ");                           \
                    bsls::BslTestUtil::callDebugprint(e.numBytes());         \
                    printf("\n");                                            \
                }                                                            \
                else if (0 == bslmaExceptionLimit) {                         \
                    printf(" [ Note: 'bslmaExceptionLimit' reached. ]\n");   \
                }                                                            \
            }                                                                \
            testAllocator.setAllocationLimit(++bslmaExceptionCounter);       \
            continue;                                                        \
        }                                                                    \
        testAllocator.setAllocationLimit(-1);                                \
        break;                                                               \
    } while (1);                                                             \
    if (g_veryVeryVerbose) printf("### End bslma exception test.\n");        \
}
#else
#define BEGIN_bslma_EXCEPTION_TEST                                           \
{                                                                            \
    static int firstTime = 1;                                                \
    if (g_verbose && firstTime) { printf(                                    \
        "### bslma EXCEPTION TEST -- (NOT ENABLED) --\n");                   \
        firstTime = 0;                                                       \
    }                                                                        \
}
#define END_bslma_EXCEPTION_TEST
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------
static bool             g_verbose = false;
static bool         g_veryVerbose = false;
static bool     g_veryVeryVerbose = false;
static bool g_veryVeryVeryVerbose = false;;


// TYPES
class TestType;
class TestTypeNoAlloc;
class BitwiseMoveableTestType;
class BitwiseCopyableTestType;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef BitwiseMoveableTestType       BMT;  // uses 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // uses 'bslma' allocators

typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

// STATIC DATA
const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

// CONFIGURATION FOR BLOCK LENGTHS AND NUMBER OF BLOCKS
const struct {
    int         d_blockLength;
    int         d_blockNumMin;
    int         d_blockNumMax;
} CONFIG[] = {
    {0,  0,  0},  // Not used
    {1, 15, 17},
    {2,  8, 10},
    {3,  5,  7},
    {4,  4,  6},
    {5,  3,  5}
};

bslma::TestAllocator *Z;  // initialized at the start of main()

                               // ===============
                               // class TestDeque
                               // ===============

template <class VALUE_TYPE, int BLOCK_LENGTH>
class TestDeque {
    // This test deque is created to allow testing of the
    // 'bslalg::DequePrimitives' on a data structure that is modeled after
    // 'bslstl_Deque'.  It has a block length of 3 and a total of 5 blocks for
    // holding a total of 15 objects of type "VALUE_TYPE".

  public:
    // PUBLIC TYPES
    enum {
        NOMINAL_BLOCK_BYTES = sizeof(VALUE_TYPE) * BLOCK_LENGTH
    };

    typedef bslalg::DequeImpUtil<VALUE_TYPE, BLOCK_LENGTH>       Imp;
    typedef typename Imp::Block                                  Block;
    typedef typename Imp::BlockPtr                               BlockPtr;
    typedef bslalg::DequeIterator<VALUE_TYPE, BLOCK_LENGTH>      Iterator;

  private:
    // DATA
    BlockPtr         *d_blocks_p;     // array of block pointers (owned)
    int               d_blockNum;
    Iterator          d_start;        // start of the 'TestDeque'
    Iterator          d_finish;       // end of the 'TestDeque'
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    TestDeque(int n, int blockNum, bslma::Allocator *ba = 0)
    : d_blocks_p(0)
    , d_blockNum(blockNum)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ASSERT(n <= BLOCK_LENGTH * d_blockNum);
        ASSERT(n >= 0);

        d_blocks_p = (BlockPtr *)d_allocator_p->allocate(
                                                sizeof(BlockPtr) * d_blockNum);

        for (int i = 0; i < d_blockNum; ++i) {
            d_blocks_p[i] = (Block *) d_allocator_p->allocate(sizeof(Block));
        }

        int offset = (d_blockNum * BLOCK_LENGTH - n) / 2;
        int blkOffset = offset / BLOCK_LENGTH;
        int aryOffset = offset % BLOCK_LENGTH;

        d_start = d_finish = Iterator(d_blocks_p + blkOffset,
                                     (d_blocks_p[blkOffset]->d_data
                                                                 + aryOffset));
    }

    ~TestDeque()
    {
        for (int i = 0; i < d_blockNum; ++i) {
            d_allocator_p->deallocate(d_blocks_p[i]);
        }
        d_allocator_p->deallocate(d_blocks_p);
    }

    // MANIPULATORS
    Iterator begin()
    {
        return d_start;
    }

    Iterator end()
    {
        return d_finish;
    }

    void setStart(Iterator itr)
    {
        d_start = itr;
    }

    void setEnd(Iterator itr)
    {
        d_finish = itr;
    }

    VALUE_TYPE& operator[](int i)
    {
        return *(begin() + i);
    }

    // ACCESSOR
    BlockPtr *getBlockPtr()
    {
        return d_blocks_p;
    }
};

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char             *d_data_p;
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }

    TestType(char c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        if (&original != this) {
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }
    }

    ~TestType() {
        ++numDestructorCalls;
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
        char *newData = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p  = newData;
        *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c) {
        *d_data_p = c;
    }

    // ACCESSORS
    char datum() const {
        return *d_data_p;
    }

    void print() const
    {
        if (d_data_p) {
        ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <> struct UsesBslmaAllocator<TestType> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

bool operator==(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

                       // =====================
                       // class TestTypeNoAlloc
                       // =====================

class TestTypeNoAlloc {
    // This test type has footprint and interface identical to 'TestType'.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.  It does not allocate, and thus could have the
    // bit-wise copyable trait, but we defer this to the
    // 'BitwiseCopyableTestType'.

    // DATA
    union {
        char                                    d_char;
        char                                    d_fill[sizeof(TestType)];
        bsls::AlignmentFromType<TestType>::Type d_align;
    } d_u;

  public:
    // CREATORS
    TestTypeNoAlloc()
    {
        d_u.d_char = '?';
        ++numDefaultCtorCalls;
    }

    TestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        ++numCharCtorCalls;
    }

    TestTypeNoAlloc(const TestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~TestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    TestTypeNoAlloc& operator=(const TestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
    char datum() const
    {
        return d_u.d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }
};

bool operator==(const TestTypeNoAlloc& lhs,
                const TestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

                       // =============================
                       // class BitwiseMoveableTestType
                       // =============================

class BitwiseMoveableTestType : public TestType {
    // This test type is identical to 'TestType' except that it has the
    // bit-wise moveable trait.  All members are inherited.

  public:
    // CREATORS
    BitwiseMoveableTestType(bslma::Allocator *ba = 0)
    : TestType(ba)
    {
    }

    BitwiseMoveableTestType(char c, bslma::Allocator *ba = 0)
    : TestType(c, ba)
    {
    }

    BitwiseMoveableTestType(const BitwiseMoveableTestType&  original,
                            bslma::Allocator               *ba = 0)
    : TestType(original, ba)
    {
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <> struct UsesBslmaAllocator<BitwiseMoveableTestType>
    : bsl::true_type {};
}  // close namespace bslma

namespace bslmf {
template <> struct IsBitwiseMoveable<BitwiseMoveableTestType>
    : bsl::true_type {};
}  // close namespace bslmf
}  // close enterprise namespace

                       // =============================
                       // class BitwiseCopyableTestType
                       // =============================

class BitwiseCopyableTestType : public TestTypeNoAlloc {
    // This test type is identical to 'TestTypeNoAlloc' except that it has the
    // bit-wise copyable trait.  All members are inherited.

  public:
    // CREATORS
    BitwiseCopyableTestType()
    : TestTypeNoAlloc()
    {
    }

    BitwiseCopyableTestType(char c)
    : TestTypeNoAlloc(c)
    {
        ++numCharCtorCalls;
    }

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : TestTypeNoAlloc(original.datum())
    {
    }
};

// TRAITS
namespace bsl {
template <> struct is_trivially_copyable<BitwiseCopyableTestType>
    : true_type {};
}  // close namespace bsl

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
template <class TYPE, int BLOCK_LENGTH>
void cleanup(TestDeque<TYPE, BLOCK_LENGTH> *deque, const char *spec)
    // Destroy elements in the specified 'deque' according to the specified
    // 'spec'.  For '0 <= i < strlen(spec)', 'deque[i]' is destroyed if and
    // only if '1 == isalpha(spec[i])'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            bslalg::ScalarDestructionPrimitives::destroy(&(*deque)[i]);
        }
    }
}

template <class TYPE, int BLOCK_LENGTH>
void verify(TestDeque<TYPE, BLOCK_LENGTH> *deque, const char *spec)
    // Verify that elements in the specified 'deque' have values according to
    // the specified 'spec'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP3_ASSERT(i, (*deque)[i].datum(), c, (*deque)[i].datum() == c);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

template <class TYPE>
void generateAry(TYPE *ary, const char *spec)
    // Generate an array of objects of 'TYPE' in the specified memory location
    // 'ary' according to the specified 'SPEC'.
{
    for (int i = 0; spec[i]; ++i) {
        bslalg::ScalarPrimitives::construct(&ary[i], spec[i], Z);
    }
}

template <class TYPE>
void destroyAry(TYPE *ary, const char *spec)
    // Destroy an array of objects of 'TYPE' in the specified memory location
    // 'ary' according to the specified 'SPEC.'
{
    for (int i = 0; spec[i]; ++i) {
        bslalg::ScalarDestructionPrimitives::destroy(&ary[i]);
    }
}

template <class TYPE, int BLOCK_LENGTH>
void setInitPos(TestDeque<TYPE, BLOCK_LENGTH> *deque,
                int                            /*blockNum*/,
                int                            position)
    // Set the start position (starting at 0) of an empty deque.  The behavior
    // is undefined unless the deque is empty and
    // 'position <= BLOCK_LENGTH * blockNum'.
{
    typedef TestDeque<TYPE, BLOCK_LENGTH> Deque;
    typedef typename Deque::Iterator      Iterator;

    int block  = position / BLOCK_LENGTH;
    int offset = position % BLOCK_LENGTH;

    Iterator start = Iterator(deque->getBlockPtr() + block,
                              deque->getBlockPtr()[block]->d_data + offset);
    deque->setStart(start);
    deque->setEnd(start);
}

template <class TYPE, int BLOCK_LENGTH>
class CleanupGuard {

    // PRIVATE TYPES
    typedef TestDeque<TYPE, BLOCK_LENGTH> Deque;
    typedef typename Deque::Iterator      Iterator;

    // DATA
    Deque      *d_deque_p;
    Iterator   *d_start;
    Iterator    d_stop;
    const char *d_spec_p;
    bool        d_isFront;  // whether the guard is guarding new elements added
                            // in front

  public:
    // CREATORS
    CleanupGuard(TestDeque<TYPE, BLOCK_LENGTH> *deque,
                 Iterator                      *start,
                 const Iterator&                stop,
                 const char                    *spec,
                 bool                           isFront)
    : d_deque_p(deque)
    , d_start(start)
    , d_stop(stop)
    , d_spec_p(spec)
    , d_isFront(isFront)
    {
    }

    ~CleanupGuard()
    {
        if (0 == d_deque_p) {
            // guard released
            return;                                                   // RETURN
        }

        // First clean up according to the spec
        cleanup(d_deque_p, d_spec_p);

        // Then clean up any other constructions
        if (d_isFront) {
            Iterator itr = *d_start;
            for (; itr != d_stop; ++itr) {
                bslalg::ScalarDestructionPrimitives::destroy(itr.valuePtr());
            }
        }
        else {
            Iterator itr = d_stop;
            for (; itr != *d_start; ++itr) {
                bslalg::ScalarDestructionPrimitives::destroy(itr.valuePtr());
            }
        }
    }

    void release() {
        d_deque_p = 0;
    }
};

//=============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure an deque according to a custom language.  Letters
// [a .. z, A .. Z] correspond to arbitrary (but unique) char values used to
// initialize elements of an deque of 'T' objects.  An underscore ('_')
// indicates that an element should be left uninitialized.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>   ::=  <EMPTY> | <LIST>
//
// <EMPTY>  ::=
//
// <LIST>   ::=  <ITEM>  | <ITEM><LIST>
//
// <ITEM>   ::=  <VALUE> | <UNSET>
//
// <VALUE>  ::=  'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' |
//               'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' |
//               's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' |
//               'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//               'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//               'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
//
// <UNSET>  ::=  '_'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Leaves the deque unaffected.
// "a"          ...
//-----------------------------------------------------------------------------

template <class TYPE, int BLOCK_LENGTH>
int ggg(TestDeque<TYPE, BLOCK_LENGTH> *deque,
        const char                    *spec,
        int                            verboseFlag = 1)
    // Configure the specified 'deque' of objects of the parameterized 'TYPE'
    // (assumed to be uninitialized) according to the specified 'spec'.
    // Optionally specify a zero 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
{
    enum { SUCCESS = -1 };
    typedef typename TestDeque<TYPE, BLOCK_LENGTH>::Iterator Iterator;

    Iterator itr = deque->begin();
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            bslalg::ScalarPrimitives::construct(itr.valuePtr(), c, Z);
            ++itr;
            deque->setEnd(itr);
        }
        else if ('_' == c) {
            ++itr;
            deque->setEnd(itr);
            continue;
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') in spec \"%s\""
                       " at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.           // RETURN
        }
    }
    return SUCCESS;
}

template <class TYPE, int BLOCK_LENGTH>
TestDeque<TYPE, BLOCK_LENGTH>&
gg(TestDeque<TYPE, BLOCK_LENGTH> *deque, const char *spec)
    // Return a modifiable reference to the specified 'deque' after the value
    // of 'deque' had been adjusted according to the specified 'spec'.
{
    ASSERT(ggg(deque, spec) < 0);
    return *deque;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 9
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to initialize
    int         d_dst;       // insert destination
    int         d_begin;     // current begin
    const char *d_expected;  // expected result array
    int         d_eb;        // expected begin
} DATA_9[] = {
    //line spec              ne  dst  begin  expected         EB  ordered by ne
    //---- ----              --  ---  -----  --------         --  -------------
    { L_,  "___",            0,  2,     2,   "___",            2 },  // 0
    { L_,  "a_c",            0,  2,     2,   "a_c",            2 },
    { L_,  "abc",            0,  2,     1,   "abc",            1 },

    { L_,  "___",            1,  2,     2,   "_t_",            1 },  // 1
    { L_,  "a_c",            1,  2,     2,   "atc",            1 },
    { L_,  "a_cd",           1,  3,     2,   "actd",           1 },
    { L_,  "a_cde",          1,  4,     2,   "acdte",          1 },
    { L_,  "a_cdef",         1,  5,     2,   "acdetf",         1 },
    { L_,  "a_cdefg",        1,  6,     2,   "acdeftg",        1 },

    { L_,  "a__d",           2,  3,     3,   "atud",           1 },  // 2
    { L_,  "a__de",          2,  4,     3,   "adtue",          1 },
    { L_,  "a__def",         2,  5,     3,   "adetuf",         1 },
    { L_,  "a__defg",        2,  6,     3,   "adeftug",        1 },
    { L_,  "a__defgh",       2,  7,     3,   "adefgtuh",       1 },
    { L_,  "a__defghi",      2,  8,     3,   "adefghtui",      1 },

    { L_,  "a___e",          3,  4,     4,   "atuve",          1 },  // 3
    { L_,  "a___ef",         3,  5,     4,   "aetuvf",         1 },
    { L_,  "a___efg",        3,  6,     4,   "aeftuvg",        1 },
    { L_,  "a___efgh",       3,  7,     4,   "aefgtuvh",       1 },
    { L_,  "a___efghi",      3,  8,     4,   "aefghtuvi",      1 },
    { L_,  "a___efghij",     3,  9,     4,   "aefghituvj",     1 },
    { L_,  "a___efghijk",    3, 10,     4,   "aefghijtuvk",    1 },

    { L_,  "a____f",         4,  5,     5,   "atuvwf",         1 },  // 4
    { L_,  "a____fg",        4,  6,     5,   "aftuvwg",        1 },
    { L_,  "a____fgh",       4,  7,     5,   "afgtuvwh",       1 },
    { L_,  "a____fghi",      4,  8,     5,   "afghtuvwi",      1 },
    { L_,  "a____fghij",     4,  9,     5,   "afghituvwj",     1 },
    { L_,  "a____fghijk",    4, 10,     5,   "afghijtuvwk",    1 },
    { L_,  "a____fghijkl",   4, 11,     5,   "afghijktuvwl",   1 },
    { L_,  "a____fghijklm",  4, 12,     5,   "afghijkltuvwm",  1 },

    { L_,  "a_____g",        5,  6,     6,   "atuvwxg",        1 },  // 5
    { L_,  "a_____gh",       5,  7,     6,   "agtuvwxh",       1 },
    { L_,  "a_____ghi",      5,  8,     6,   "aghtuvwxi",      1 },
    { L_,  "a_____ghij",     5,  9,     6,   "aghituvwxj",     1 },
    { L_,  "a_____ghijk",    5, 10,     6,   "aghijtuvwxk",    1 },
    { L_,  "a_____ghijkl",   5, 11,     6,   "aghijktuvwxl",   1 },
    { L_,  "a_____ghijklm",  5, 12,     6,   "aghijkltuvwxm",  1 },
    { L_,  "a_____ghijklmn", 5, 13,     6,   "aghijklmtuvwxn", 1 },
    { L_,  "a_____ghijklmno",5, 14,     6,   "aghijklmntuvwxo",1 },
};
const int NUM_DATA_9 = sizeof DATA_9 / sizeof *DATA_9;

template <class TYPE, int BLOCK_LENGTH>
void testInsertAndMoveToFrontRange(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    const char *INPUTREF = "tuvwxyz";
    const int   MAX_SIZE = 15;

    ASSERT(std::strlen(INPUTREF) < 8);
    bsls::ObjectBuffer<TYPE> input[8];
    generateAry(&(input[0].object()), INPUTREF);

    const TYPE *INPUT = &(input[0].object());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_9; ++ti) {

            const int         LINE  = DATA_9[ti].d_lineNum;
            const char *const SPEC  = DATA_9[ti].d_spec;
            const int         NE    = DATA_9[ti].d_ne;
            const int         DST   = DATA_9[ti].d_dst;
            const int         BEGIN = DATA_9[ti].d_begin;
            const char *const EXP   = DATA_9[ti].d_expected;
            const int         EB    = DATA_9[ti].d_eb;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, NE = %d, EXP = %s, EB = %d\n",
                       LINE, SPEC, BEGIN, NE, EXP, EB);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC); verify(&deque, SPEC);

                        typename Deque::Iterator begin;

                        // Guard just have to destruct according to the given
                        // 'SPEC'.  'insertAndMoveToBack' will not modify the
                        // existing range of objects in case of an exception.
                        CleanupGuard<TYPE, BLOCK_LENGTH>
                                     guard(&deque, &begin, begin, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        Obj::insertAndMoveToFront(&begin,
                                                  deque.begin() + BEGIN,
                                                  deque.begin() + DST,
                                                  INPUT,
                                                  INPUT + NE,
                                                  NE,
                                                  Z);

                        guard.release();

                        ASSERT(deque.begin() + EB == begin);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator begin;
                    Obj::insertAndMoveToFront(&begin,
                                              deque.begin() + BEGIN,
                                              deque.begin() + DST,
                                              INPUT,
                                              INPUT + NE,
                                              NE,
                                              Z);

                    ASSERT(deque.begin() + EB == begin);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    destroyAry(&input[0].object(), INPUTREF);

    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 8
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to insert
    int         d_dst;       // index of insertion point
    int         d_end;       // end of data
    const char *d_expected;  // expected result array
    int         d_ee;        // expected end
} DATA_8[] = {
    //line spec              ne  dst    end  expected         EE  ordered by ne
    //---- ----              --  ---    ---  --------         --  -------------
    { L_,  "___",            0,  1,     1,   "___",            1 },  // 0
    { L_,  "a_c",            0,  1,     1,   "a_c",            1 },
    { L_,  "abc",            0,  1,     2,   "abc",            2 },

    { L_,  "___",            1,  1,     1,   "_t_",            2 },  // 1
    { L_,  "a_c",            1,  1,     1,   "atc",            2 },
    { L_,  "ab_d",           1,  1,     2,   "atbd",           3 },
    { L_,  "abc_e",          1,  1,     3,   "atbce",          4 },
    { L_,  "abcd_f",         1,  1,     4,   "atbcdf",         5 },
    { L_,  "abcde_g",        1,  1,     5,   "atbcdeg",        6 },

    { L_,  "a__d",           2,  1,     1,   "atud",           3 },  // 2
    { L_,  "ab__e",          2,  1,     2,   "atube",          4 },
    { L_,  "abc__f",         2,  1,     3,   "atubcf",         5 },
    { L_,  "abcd__g",        2,  1,     4,   "atubcdg",        6 },
    { L_,  "abcde__h",       2,  1,     5,   "atubcdeh",       7 },
    { L_,  "abcdef__i",      2,  1,     6,   "atubcdefi",      8 },

    { L_,  "a___e",          3,  1,     1,   "atuve",          4 },  // 3
    { L_,  "ab___f",         3,  1,     2,   "atuvbf",         5 },
    { L_,  "abc___g",        3,  1,     3,   "atuvbcg",        6 },
    { L_,  "abcd___h",       3,  1,     4,   "atuvbcdh",       7 },
    { L_,  "abcde___i",      3,  1,     5,   "atuvbcdei",      8 },
    { L_,  "abcdef___j",     3,  1,     6,   "atuvbcdefj",     9 },
    { L_,  "abcdefg___k",    3,  1,     7,   "atuvbcdefgk",    10},

    { L_,  "a____f",         4,  1,     1,   "atuvwf",         5 },  // 4
    { L_,  "ab____g",        4,  1,     2,   "atuvwbg",        6 },
    { L_,  "abc____h",       4,  1,     3,   "atuvwbch",       7 },
    { L_,  "abcd____i",      4,  1,     4,   "atuvwbcdi",      8 },
    { L_,  "abcde____j",     4,  1,     5,   "atuvwbcdej",     9 },
    { L_,  "abcdef____k",    4,  1,     6,   "atuvwbcdefk",    10},
    { L_,  "abcdefg____l",   4,  1,     7,   "atuvwbcdefgl",   11},
    { L_,  "abcdefgh____m",  4,  1,     8,   "atuvwbcdefghm",  12},

    { L_,  "a_____g",        5,  1,     1,   "atuvwxg",        6 },  // 5
    { L_,  "ab_____h",       5,  1,     2,   "atuvwxbh",       7 },
    { L_,  "abc_____i",      5,  1,     3,   "atuvwxbci",      8 },
    { L_,  "abcd_____j",     5,  1,     4,   "atuvwxbcdj",     9 },
    { L_,  "abcde_____k",    5,  1,     5,   "atuvwxbcdek",    10},
    { L_,  "abcdef_____l",   5,  1,     6,   "atuvwxbcdefl",   11},
    { L_,  "abcdefg_____m",  5,  1,     7,   "atuvwxbcdefgm",  12},
    { L_,  "abcdefgh_____n", 5,  1,     8,   "atuvwxbcdefghn", 13},
    { L_,  "abcdefghi_____o",5,  1,     9,   "atuvwxbcdefghio",14},
};
const int NUM_DATA_8 = sizeof DATA_8 / sizeof *DATA_8;

template <class TYPE, int BLOCK_LENGTH>
void testInsertAndMoveToBackRange(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    const char *INPUTREF = "tuvwxyz";
    const int   MAX_SIZE = 15;

    ASSERT(std::strlen(INPUTREF) < 8);
    bsls::ObjectBuffer<TYPE> input[8];
    generateAry(&(input[0].object()), INPUTREF);

    const TYPE *INPUT = &(input[0].object());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_8; ++ti) {
            const int         LINE  = DATA_8[ti].d_lineNum;
            const char *const SPEC  = DATA_8[ti].d_spec;
            const int         NE    = DATA_8[ti].d_ne;
            const int         DST   = DATA_8[ti].d_dst;
            const int         END   = DATA_8[ti].d_end;
            const char *const EXP   = DATA_8[ti].d_expected;
            const int         EE    = DATA_8[ti].d_ee;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "END = %d, NE = %d, EXP = %s, EE = %d\n",
                       LINE, SPEC, END, NE, EXP, EE);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC);  verify(&deque, SPEC);

                        typename Deque::Iterator end;

                        // Guard just have to destruct according to the given
                        // 'SPEC'.  'insertAndMoveToBack' will not modify the
                        // existing range of objects in case of an exception.
                        CleanupGuard<TYPE, BLOCK_LENGTH>
                                         guard(&deque, &end, end, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        Obj::insertAndMoveToBack(&end,
                                                 deque.begin() + END,
                                                 deque.begin() + DST,
                                                 INPUT,
                                                 INPUT + NE,
                                                 NE,
                                                 Z);
                        guard.release();

                        ASSERT(deque.begin() + EE == end);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator end;
                    Obj::insertAndMoveToBack(&end,
                                             deque.begin() + END,
                                             deque.begin() + DST,
                                             INPUT,
                                             INPUT + NE,
                                             NE,
                                             Z);

                    ASSERT(deque.begin() + EE == end);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    destroyAry(&input[0].object(), INPUTREF);

    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 7
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to initialize
    int         d_dst;       // insert destination
    int         d_begin;     // current begin
    const char *d_expected;  // expected result array
    int         d_eb;        // expected begin
} DATA_7[] = {
    //line spec              ne  dst  begin  expected         EB  ordered by ne
    //---- ----              --  ---  -----  --------         --  -------------
    { L_,  "___",            0,  2,     2,   "___",            2 },  // 0
    { L_,  "a_c",            0,  2,     2,   "a_c",            2 },
    { L_,  "abc",            0,  2,     1,   "abc",            1 },

    { L_,  "___",            1,  2,     2,   "_V_",            1 },  // 1
    { L_,  "a_c",            1,  2,     2,   "aVc",            1 },
    { L_,  "a_cd",           1,  3,     2,   "acVd",           1 },
    { L_,  "a_cde",          1,  4,     2,   "acdVe",          1 },
    { L_,  "a_cdef",         1,  5,     2,   "acdeVf",         1 },
    { L_,  "a_cdefg",        1,  6,     2,   "acdefVg",        1 },

    { L_,  "a__d",           2,  3,     3,   "aVVd",           1 },  // 2
    { L_,  "a__de",          2,  4,     3,   "adVVe",          1 },
    { L_,  "a__def",         2,  5,     3,   "adeVVf",         1 },
    { L_,  "a__defg",        2,  6,     3,   "adefVVg",        1 },
    { L_,  "a__defgh",       2,  7,     3,   "adefgVVh",       1 },
    { L_,  "a__defghi",      2,  8,     3,   "adefghVVi",      1 },

    { L_,  "a___e",          3,  4,     4,   "aVVVe",          1 },  // 3
    { L_,  "a___ef",         3,  5,     4,   "aeVVVf",         1 },
    { L_,  "a___efg",        3,  6,     4,   "aefVVVg",        1 },
    { L_,  "a___efgh",       3,  7,     4,   "aefgVVVh",       1 },
    { L_,  "a___efghi",      3,  8,     4,   "aefghVVVi",      1 },
    { L_,  "a___efghij",     3,  9,     4,   "aefghiVVVj",     1 },
    { L_,  "a___efghijk",    3, 10,     4,   "aefghijVVVk",    1 },

    { L_,  "a____f",         4,  5,     5,   "aVVVVf",         1 },  // 4
    { L_,  "a____fg",        4,  6,     5,   "afVVVVg",        1 },
    { L_,  "a____fgh",       4,  7,     5,   "afgVVVVh",       1 },
    { L_,  "a____fghi",      4,  8,     5,   "afghVVVVi",      1 },
    { L_,  "a____fghij",     4,  9,     5,   "afghiVVVVj",     1 },
    { L_,  "a____fghijk",    4, 10,     5,   "afghijVVVVk",    1 },
    { L_,  "a____fghijkl",   4, 11,     5,   "afghijkVVVVl",   1 },
    { L_,  "a____fghijklm",  4, 12,     5,   "afghijklVVVVm",  1 },

    { L_,  "a_____g",        5,  6,     6,   "aVVVVVg",        1 },  // 5
    { L_,  "a_____gh",       5,  7,     6,   "agVVVVVh",       1 },
    { L_,  "a_____ghi",      5,  8,     6,   "aghVVVVVi",      1 },
    { L_,  "a_____ghij",     5,  9,     6,   "aghiVVVVVj",     1 },
    { L_,  "a_____ghijk",    5, 10,     6,   "aghijVVVVVk",    1 },
    { L_,  "a_____ghijkl",   5, 11,     6,   "aghijkVVVVVl",   1 },
    { L_,  "a_____ghijklm",  5, 12,     6,   "aghijklVVVVVm",  1 },
    { L_,  "a_____ghijklmn", 5, 13,     6,   "aghijklmVVVVVn", 1 },
    { L_,  "a_____ghijklmno",5, 14,     6,   "aghijklmnVVVVVo",1 },
};
const int NUM_DATA_7 = sizeof DATA_7 / sizeof *DATA_7;

template <class TYPE, int BLOCK_LENGTH>
void testInsertAndMoveToFrontRaw(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    const int MAX_SIZE = 15;

    bsls::ObjectBuffer<TYPE> mV;
    bslalg::ScalarPrimitives::construct(&mV.object(), 'V', Z);
    const TYPE& V = mV.object();
    ASSERT('V' == V.datum());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_7; ++ti) {
            const int         LINE  = DATA_7[ti].d_lineNum;
            const char *const SPEC  = DATA_7[ti].d_spec;
            const int         NE    = DATA_7[ti].d_ne;
            const int         DST   = DATA_7[ti].d_dst;
            const int         BEGIN = DATA_7[ti].d_begin;
            const char *const EXP   = DATA_7[ti].d_expected;
            const int         EB    = DATA_7[ti].d_eb;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, NE = %d, EXP = %s, EB = %d\n",
                       LINE, SPEC, BEGIN, NE, EXP, EB);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC); verify(&deque, SPEC);

                        typename Deque::Iterator begin;

                        // Guard just have to destruct according to the given
                        // 'SPEC'.  'insertAndMovetoBack' will not modify the
                        // existing range of objects in case of an exception.
                        CleanupGuard<TYPE, BLOCK_LENGTH>
                                     guard(&deque, &begin, begin, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        Obj::insertAndMoveToFront(&begin,
                                                  deque.begin() + BEGIN,
                                                  deque.begin() + DST,
                                                  NE,
                                                  V,
                                                  Z);

                        guard.release();

                        ASSERT(deque.begin() + EB == begin);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC); verify(&deque, SPEC);

                    typename Deque::Iterator begin;
                    Obj::insertAndMoveToFront(&begin,
                                              deque.begin() + BEGIN,
                                              deque.begin() + DST,
                                              NE,
                                              V,
                                              Z);

                    ASSERT(deque.begin() + EB == begin);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    bslalg::ScalarDestructionPrimitives::destroy(&mV.object());

    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 6
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to initialize
    int         d_dst;       // insert destination
    int         d_end;       // current end
    const char *d_expected;  // expected result array
    int         d_ee;        // expected end
} DATA_6[] = {
    //line spec              ne  dst    end  expected         EE  ordered by ne
    //---- ----              --  ---    ---  --------         --  -------------
    { L_,  "___",            0,  1,     1,   "___",            1 },  // 0
    { L_,  "a_c",            0,  1,     1,   "a_c",            1 },
    { L_,  "abc",            0,  1,     2,   "abc",            2 },

    { L_,  "___",            1,  1,     1,   "_V_",            2 },  // 1
    { L_,  "a_c",            1,  1,     1,   "aVc",            2 },
    { L_,  "ab_d",           1,  1,     2,   "aVbd",           3 },
    { L_,  "abc_e",          1,  1,     3,   "aVbce",          4 },
    { L_,  "abcd_f",         1,  1,     4,   "aVbcdf",         5 },
    { L_,  "abcde_g",        1,  1,     5,   "aVbcdeg",        6 },

    { L_,  "a__d",           2,  1,     1,   "aVVd",           3 },  // 2
    { L_,  "ab__e",          2,  1,     2,   "aVVbe",          4 },
    { L_,  "abc__f",         2,  1,     3,   "aVVbcf",         5 },
    { L_,  "abcd__g",        2,  1,     4,   "aVVbcdg",        6 },
    { L_,  "abcde__h",       2,  1,     5,   "aVVbcdeh",       7 },
    { L_,  "abcdef__i",      2,  1,     6,   "aVVbcdefi",      8 },

    { L_,  "a___e",          3,  1,     1,   "aVVVe",          4 },  // 3
    { L_,  "ab___f",         3,  1,     2,   "aVVVbf",         5 },
    { L_,  "abc___g",        3,  1,     3,   "aVVVbcg",        6 },
    { L_,  "abcd___h",       3,  1,     4,   "aVVVbcdh",       7 },
    { L_,  "abcde___i",      3,  1,     5,   "aVVVbcdei",      8 },
    { L_,  "abcdef___j",     3,  1,     6,   "aVVVbcdefj",     9 },
    { L_,  "abcdefg___k",    3,  1,     7,   "aVVVbcdefgk",    10},

    { L_,  "a____f",         4,  1,     1,   "aVVVVf",         5 },  // 4
    { L_,  "ab____g",        4,  1,     2,   "aVVVVbg",        6 },
    { L_,  "abc____h",       4,  1,     3,   "aVVVVbch",       7 },
    { L_,  "abcd____i",      4,  1,     4,   "aVVVVbcdi",      8 },
    { L_,  "abcde____j",     4,  1,     5,   "aVVVVbcdej",     9 },
    { L_,  "abcdef____k",    4,  1,     6,   "aVVVVbcdefk",    10},
    { L_,  "abcdefg____l",   4,  1,     7,   "aVVVVbcdefgl",   11},
    { L_,  "abcdefgh____m",  4,  1,     8,   "aVVVVbcdefghm",  12},

    { L_,  "a_____g",        5,  1,     1,   "aVVVVVg",        6 },  // 5
    { L_,  "ab_____h",       5,  1,     2,   "aVVVVVbh",       7 },
    { L_,  "abc_____i",      5,  1,     3,   "aVVVVVbci",      8 },
    { L_,  "abcd_____j",     5,  1,     4,   "aVVVVVbcdj",     9 },
    { L_,  "abcde_____k",    5,  1,     5,   "aVVVVVbcdek",    10},
    { L_,  "abcdef_____l",   5,  1,     6,   "aVVVVVbcdefl",   11},
    { L_,  "abcdefg_____m",  5,  1,     7,   "aVVVVVbcdefgm",  12},
    { L_,  "abcdefgh_____n", 5,  1,     8,   "aVVVVVbcdefghn", 13},
    { L_,  "abcdefghi_____o",5,  1,     9,   "aVVVVVbcdefghio",14},
};
const int NUM_DATA_6 = sizeof DATA_6 / sizeof *DATA_6;

template <class TYPE, int BLOCK_LENGTH>
void testInsertAndMoveToBackRaw(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>                                Deque;

    typedef bslalg::DequePrimitives<
            TYPE,
            BLOCK_LENGTH>                                                Obj;

    const int MAX_SIZE = 15;

    bsls::ObjectBuffer<TYPE> mV;
    bslalg::ScalarPrimitives::construct(&mV.object(), 'V', Z);
    const TYPE& V = mV.object();
    ASSERT('V' == V.datum());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }


        for (int ti = 0; ti < NUM_DATA_6; ++ti) {
            const int         LINE  = DATA_6[ti].d_lineNum;
            const char *const SPEC  = DATA_6[ti].d_spec;
            const int         NE    = DATA_6[ti].d_ne;
            const int         DST   = DATA_6[ti].d_dst;
            const int         END   = DATA_6[ti].d_end;
            const char *const EXP   = DATA_6[ti].d_expected;
            const int         EE    = DATA_6[ti].d_ee;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "END = %d, NE = %d, EXP = %s, EE = %d\n",
                       LINE, SPEC, END, NE, EXP, EE);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC);  verify(&deque, SPEC);

                        typename Deque::Iterator end;

                        // Guard just have to destruct according to the given
                        // 'SPEC'.  'insertAndMovetoBack' will not modify the
                        // existing range of objects in case of an exception.
                        CleanupGuard<TYPE, BLOCK_LENGTH>
                                         guard(&deque, &end, end, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        Obj::insertAndMoveToBack(&end,
                                                 deque.begin() + END,
                                                 deque.begin() + DST,
                                                 NE,
                                                 V,
                                                 Z);
                        guard.release();

                        ASSERT(deque.begin() + EE == end);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator end;
                    Obj::insertAndMoveToBack(&end,
                                             deque.begin() + END,
                                             deque.begin() + DST,
                                             NE,
                                             V,
                                             Z);

                    ASSERT(deque.begin() + EE == end);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    bslalg::ScalarDestructionPrimitives::destroy(&mV.object());

    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 5
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // start of [begin, end) range
    int         d_ne;        // number of elements to initialize
    const char *d_expected;  // expected result array
    int         d_eb;        // expected end
} DATA_5[] = {
    //line spec             begin   ne   expected          eb    ordered by ne
    //---- ----             -----   --   --------          --    -------------
    { L_,  "___",             2,    0,   "___",             2    },  // 0
    { L_,  "a_c",             2,    0,   "a_c",             2    },
    { L_,  "abc",             2,    0,   "abc",             2    },

    { L_,  "___",             2,    1,   "_V_",             1    },  // 1
    { L_,  "a_c",             2,    1,   "aVc",             1    },

    { L_,  "____",            3,    2,   "_VV_",            1    },  // 2
    { L_,  "a__d",            3,    2,   "aVVd",            1    },

    { L_,  "a___e",           4,    3,   "aVVVe",           1    },  // 3

    { L_,  "a____f",          5,    4,   "aVVVVf",          1    },  // 4

    { L_,  "a_____g",         6,    5,   "aVVVVVg",         1    },  // 5

    { L_,  "a______h",        7,    6,   "aVVVVVVh",        1    },  // 6

    { L_,  "a_______i",       8,    7,   "aVVVVVVVi",       1    },  // 7

    { L_,  "a________j",      9,    8,   "aVVVVVVVVj",      1    },  // 8

    { L_,  "a____________n", 13,   12,   "aVVVVVVVVVVVVn",  1    },  // 12
};
const int NUM_DATA_5 = sizeof DATA_5 / sizeof *DATA_5;

template <class TYPE, int BLOCK_LENGTH>
void testUninitializedFillNFront(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>                                Deque;

    typedef bslalg::DequePrimitives<
            TYPE,
            BLOCK_LENGTH>                                                  Obj;
    const int MAX_SIZE = 15;

    bsls::ObjectBuffer<TYPE> mV;
    bslalg::ScalarPrimitives::construct(&mV.object(), 'V', Z);
    const TYPE& V = mV.object();
    ASSERT('V' == V.datum());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_5; ++ti) {
            const int         LINE  = DATA_5[ti].d_lineNum;
            const char *const SPEC  = DATA_5[ti].d_spec;
            const int         BEGIN = DATA_5[ti].d_begin;
            const int         NE    = DATA_5[ti].d_ne;
            const char *const EXP   = DATA_5[ti].d_expected;
            const int         EB    = DATA_5[ti].d_eb;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, NE = %d, EXP = %s, EB = %d\n",
                       LINE, SPEC, BEGIN, NE, EXP, EB);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC);  verify(&deque, SPEC);

                        typename Deque::Iterator begin = deque.begin() + BEGIN;

                        CleanupGuard<TYPE, BLOCK_LENGTH> guard(
                                            &deque, &begin,
                                            deque.begin() + BEGIN, SPEC, true);

                        testAllocator.setAllocationLimit(AL);


                        Obj::uninitializedFillNFront(&begin,
                                                     deque.begin() + BEGIN,
                                                     NE,
                                                     V,
                                                     Z);
                        guard.release();

                        ASSERT(deque.begin() + EB == begin);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator begin;
                    Obj::uninitializedFillNFront(&begin,
                                                 deque.begin() + BEGIN,
                                                 NE,
                                                 V,
                                                 Z);

                    ASSERT(deque.begin() + EB == begin);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    bslalg::ScalarDestructionPrimitives::destroy(&mV.object());

    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 4
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_end;       // end of [begin, end) range
    int         d_ne;        // number of elements to initialize
    const char *d_expected;  // expected result array
    int         d_ee;        // expected end
} DATA_4[] = {
    //line spec               end   ne   expected          ee    ordered by ne
    //---- ----               ---   --   --------          --    -------------
    { L_,  "___",             1,    0,   "___",             1    },  // 0
    { L_,  "a_c",             1,    0,   "a_c",             1    },
    { L_,  "abc",             1,    0,   "abc",             1    },

    { L_,  "___",             1,    1,   "_V_",             2    },  // 1
    { L_,  "a_c",             1,    1,   "aVc",             2    },

    { L_,  "____",            1,    2,   "_VV_",            3    },  // 2
    { L_,  "a__d",            1,    2,   "aVVd",            3    },

    { L_,  "a___e",           1,    3,   "aVVVe",           4    },  // 3

    { L_,  "a____f",          1,    4,   "aVVVVf",          5    },  // 4

    { L_,  "a_____g",         1,    5,   "aVVVVVg",         6    },  // 5

    { L_,  "a______h",        1,    6,   "aVVVVVVh",        7    },  // 6

    { L_,  "a_______i",       1,    7,   "aVVVVVVVi",       8    },  // 7

    { L_,  "a________j",      1,    8,   "aVVVVVVVVj",      9    },  // 8

    { L_,  "a____________n",  1,   12,   "aVVVVVVVVVVVVn", 13    },  // 12
};
const int NUM_DATA_4 = sizeof DATA_4 / sizeof *DATA_4;

template <class TYPE, int BLOCK_LENGTH>
void testUninitializedFillNBack(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    const int MAX_SIZE = 15;

    bsls::ObjectBuffer<TYPE> mV;
    bslalg::ScalarPrimitives::construct(&mV.object(), 'V', Z);
    const TYPE& V = mV.object();
    ASSERT('V' == V.datum());

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_4; ++ti) {
            const int         LINE = DATA_4[ti].d_lineNum;
            const char *const SPEC = DATA_4[ti].d_spec;
            const int         END  = DATA_4[ti].d_end;
            const int         NE   = DATA_4[ti].d_ne;
            const char *const EXP  = DATA_4[ti].d_expected;
            const int         EE   = DATA_4[ti].d_ee;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "END = %d, NE = %d, EXP = %s, EE = %d\n",
                       LINE, SPEC, END, NE, EXP, EE);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC);  verify(&deque, SPEC);

                        typename Deque::Iterator end(deque.begin() + END);

                        CleanupGuard<TYPE, BLOCK_LENGTH> guard(
                                             &deque, &end,
                                             deque.begin() + END, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        Obj::uninitializedFillNBack(&end,
                                                    deque.begin() + END,
                                                    NE,
                                                    V,
                                                    Z);

                        guard.release();

                        ASSERT(deque.begin() + EE == end);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator end;

                    Obj::uninitializedFillNBack(&end,
                                                deque.begin() + END,
                                                NE,
                                                V,
                                                Z);

                    ASSERT(deque.begin() + EE == end);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    bslalg::ScalarDestructionPrimitives::destroy(&mV.object());
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 3
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // beginning of data
    int         d_first;     // index to start of range to erase (include)
    int         d_last;      // index to end of range to erase (exclude)
    int         d_end;       // end of data
    const char *d_expected;  // expected result array
    int         d_eb;        // expected begin pointer returned
    int         d_ee;        // expected end pointer returned
    int         d_ei;        // expected iterator returned
} DATA_3[] = {
    //                                                                 ordered
    //line spec      begin  first  last   end  expected    eb  ee  ei  by 'end'
    //---- ----      -----  -----  ----   ---  --------    --  --  --  --------
    { L_,  "___",       1,     1,     1,    2, "___",      1,  2,  1},  // 2
    { L_,  "_b_",       1,     1,     1,    2, "_b_",      1,  2,  1},
    { L_,  "abc",       1,     1,     1,    2, "abc",      1,  2,  1},
    { L_,  "abc",       1,     1,     2,    2, "a_c",      1,  1,  1},
    { L_,  "abc",       1,     2,     2,    2, "abc",      1,  2,  2},

    { L_,  "abcd",      1,     1,     1,    3, "abcd",     1,  3,  1},  // 3
    { L_,  "abcd",      1,     1,     2,    3, "a_cd",     2,  3,  2},
    { L_,  "abcd",      1,     1,     3,    3, "a__d",     1,  1,  1},
    { L_,  "abcd",      1,     2,     2,    3, "abcd",     1,  3,  2},
    { L_,  "abcd",      1,     2,     3,    3, "ab_d",     1,  2,  2},
    { L_,  "abcd",      1,     3,     3,    3, "abcd",     1,  3,  3},

    { L_,  "abcde",     1,     1,     1,    4, "abcde",    1,  4,  1},  // 4
    { L_,  "abcde",     1,     1,     2,    4, "a_cde",    2,  4,  2},
    { L_,  "abcde",     1,     1,     3,    4, "a__de",    3,  4,  3},
    { L_,  "abcde",     1,     1,     4,    4, "a___e",    1,  1,  1},
    { L_,  "abcde",     1,     2,     2,    4, "abcde",    1,  4,  2},
    { L_,  "abcde",     1,     2,     3,    4, "abd_e",    1,  3,  2},
    { L_,  "abcde",     1,     2,     4,    4, "ab__e",    1,  2,  2},
    { L_,  "abcde",     1,     3,     3,    4, "abcde",    1,  4,  3},
    { L_,  "abcde",     1,     3,     4,    4, "abc_e",    1,  3,  3},
    { L_,  "abcde",     1,     4,     4,    4, "abcde",    1,  4,  4},

    { L_,  "abcdef",    1,     1,     1,    5, "abcdef",   1,  5,  1},  // 5
    { L_,  "abcdef",    1,     1,     2,    5, "a_cdef",   2,  5,  2},
    { L_,  "abcdef",    1,     1,     3,    5, "a__def",   3,  5,  3},
    { L_,  "abcdef",    1,     1,     4,    5, "a___ef",   4,  5,  4},
    { L_,  "abcdef",    1,     1,     5,    5, "a____f",   1,  1,  1},
    { L_,  "abcdef",    1,     2,     2,    5, "abcdef",   1,  5,  2},
    { L_,  "abcdef",    1,     2,     3,    5, "a_bdef",   2,  5,  3},
    { L_,  "abcdef",    1,     2,     4,    5, "abe__f",   1,  3,  2},
    { L_,  "abcdef",    1,     2,     5,    5, "ab___f",   1,  2,  2},
    { L_,  "abcdef",    1,     3,     3,    5, "abcdef",   1,  5,  3},
    { L_,  "abcdef",    1,     3,     4,    5, "abce_f",   1,  4,  3},
    { L_,  "abcdef",    1,     3,     5,    5, "abc__f",   1,  3,  3},
    { L_,  "abcdef",    1,     4,     4,    5, "abcdef",   1,  5,  4},
    { L_,  "abcdef",    1,     4,     5,    5, "abcd_f",   1,  4,  4},
    { L_,  "abcdef",    1,     5,     5,    5, "abcdef",   1,  5,  5},

    { L_,  "abcdefg",   1,     1,     1,    6, "abcdefg",  1,  6,  1},  // 6
    { L_,  "abcdefg",   1,     1,     2,    6, "a_cdefg",  2,  6,  2},
    { L_,  "abcdefg",   1,     1,     3,    6, "a__defg",  3,  6,  3},
    { L_,  "abcdefg",   1,     1,     4,    6, "a___efg",  4,  6,  4},
    { L_,  "abcdefg",   1,     1,     5,    6, "a____fg",  5,  6,  5},
    { L_,  "abcdefg",   1,     1,     6,    6, "a_____g",  1,  1,  1},
    { L_,  "abcdefg",   1,     2,     2,    6, "abcdefg",  1,  6,  2},
    { L_,  "abcdefg",   1,     2,     3,    6, "a_bdefg",  2,  6,  3},
    { L_,  "abcdefg",   1,     2,     4,    6, "a__befg",  3,  6,  4},
    { L_,  "abcdefg",   1,     2,     5,    6, "abf___g",  1,  3,  2},
    { L_,  "abcdefg",   1,     2,     6,    6, "ab____g",  1,  2,  2},
    { L_,  "abcdefg",   1,     3,     3,    6, "abcdefg",  1,  6,  3},
    { L_,  "abcdefg",   1,     3,     4,    6, "abcef_g",  1,  5,  3},
    { L_,  "abcdefg",   1,     3,     5,    6, "abcf__g",  1,  4,  3},
    { L_,  "abcdefg",   1,     3,     6,    6, "abc___g",  1,  3,  3},
    { L_,  "abcdefg",   1,     4,     4,    6, "abcdefg",  1,  6,  4},
    { L_,  "abcdefg",   1,     4,     5,    6, "abcdf_g",  1,  5,  4},
    { L_,  "abcdefg",   1,     4,     6,    6, "abcd__g",  1,  4,  4},
    { L_,  "abcdefg",   1,     5,     5,    6, "abcdefg",  1,  6,  5},
    { L_,  "abcdefg",   1,     5,     6,    6, "abcde_g",  1,  5,  5},
    { L_,  "abcdefg",   1,     6,     6,    6, "abcdefg",  1,  6,  6},

    { L_,  "abcdefgh",  1,     1,     1,    7, "abcdefgh", 1,  7,  1},  // 7
    { L_,  "abcdefgh",  1,     1,     2,    7, "a_cdefgh", 2,  7,  2},
    { L_,  "abcdefgh",  1,     1,     3,    7, "a__defgh", 3,  7,  3},
    { L_,  "abcdefgh",  1,     1,     4,    7, "a___efgh", 4,  7,  4},
    { L_,  "abcdefgh",  1,     1,     5,    7, "a____fgh", 5,  7,  5},
    { L_,  "abcdefgh",  1,     1,     6,    7, "a_____gh", 6,  7,  6},
    { L_,  "abcdefgh",  1,     1,     7,    7, "a______h", 1,  1,  1},
    { L_,  "abcdefgh",  1,     2,     2,    7, "abcdefgh", 1,  7,  2},
    { L_,  "abcdefgh",  1,     2,     3,    7, "a_bdefgh", 2,  7,  3},
    { L_,  "abcdefgh",  1,     2,     4,    7, "a__befgh", 3,  7,  4},
    { L_,  "abcdefgh",  1,     2,     5,    7, "a___bfgh", 4,  7,  5},
    { L_,  "abcdefgh",  1,     2,     6,    7, "abg____h", 1,  3,  2},
    { L_,  "abcdefgh",  1,     2,     7,    7, "ab_____h", 1,  2,  2},
    { L_,  "abcdefgh",  1,     3,     3,    7, "abcdefgh", 1,  7,  3},
    { L_,  "abcdefgh",  1,     3,     4,    7, "a_bcefgh", 2,  7,  4},
    { L_,  "abcdefgh",  1,     3,     5,    7, "abcfg__h", 1,  5,  3},
    { L_,  "abcdefgh",  1,     3,     6,    7, "abcg___h", 1,  4,  3},
    { L_,  "abcdefgh",  1,     3,     7,    7, "abc____h", 1,  3,  3},
    { L_,  "abcdefgh",  1,     4,     4,    7, "abcdefgh", 1,  7,  4},
    { L_,  "abcdefgh",  1,     4,     5,    7, "abcdfg_h", 1,  6,  4},
    { L_,  "abcdefgh",  1,     4,     6,    7, "abcdg__h", 1,  5,  4},
    { L_,  "abcdefgh",  1,     4,     7,    7, "abcd___h", 1,  4,  4},
    { L_,  "abcdefgh",  1,     5,     5,    7, "abcdefgh", 1,  7,  5},
    { L_,  "abcdefgh",  1,     5,     6,    7, "abcdeg_h", 1,  6,  5},
    { L_,  "abcdefgh",  1,     5,     7,    7, "abcde__h", 1,  5,  5},
    { L_,  "abcdefgh",  1,     6,     6,    7, "abcdefgh", 1,  7,  6},
    { L_,  "abcdefgh",  1,     6,     7,    7, "abcdef_h", 1,  6,  6},
    { L_,  "abcdefgh",  1,     7,     7,    7, "abcdefgh", 1,  7,  7},

};
const int NUM_DATA_3 = sizeof DATA_3 / sizeof *DATA_3;

template <class TYPE, int BLOCK_LENGTH>
void testErase(bool exceptionSafetyFlag = false)
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    const int MAX_SIZE = 15;

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_3; ++ti) {
            const int         LINE  = DATA_3[ti].d_lineNum;
            const char *const SPEC  = DATA_3[ti].d_spec;
            const int         BEGIN = DATA_3[ti].d_begin;
            const int         FIRST = DATA_3[ti].d_first;
            const int         LAST  = DATA_3[ti].d_last;
            const int         END   = DATA_3[ti].d_end;
            const char *const EXP   = DATA_3[ti].d_expected;
            const int         EB    = DATA_3[ti].d_eb;
            const int         EE    = DATA_3[ti].d_ee;
            const int         EI    = DATA_3[ti].d_ei;

            ASSERT(MAX_SIZE >= static_cast<int>(std::strlen(SPEC)));
            const int SIZE = static_cast<int>(strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, FIRST = %d, LAST = %d, END = %d, "
                       "EXP = %s, EB = %d, EE = %d, EI = %d\n",
                       LINE, SPEC, BEGIN, FIRST, LAST, END, EXP, EB, EE, EI);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                if (exceptionSafetyFlag) {
                    bslma::TestAllocator& testAllocator = *Z;
                    BEGIN_bslma_EXCEPTION_TEST {

                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Deque deque(SIZE, numBlocks, Z);
                        setInitPos(&deque, numBlocks, ip);
                        gg(&deque, SPEC);  verify(&deque, SPEC);

                        typename Deque::Iterator begin;
                        typename Deque::Iterator end;
                        typename Deque::Iterator ret;

                        // Guard just have to destruct according to the given
                        // 'SPEC'.  'erase' will not modify the existing range
                        // of objects in case of an exception.
                        CleanupGuard<TYPE, BLOCK_LENGTH> guard(
                                                           &deque, &begin,
                                                           begin, SPEC, false);

                        testAllocator.setAllocationLimit(AL);

                        ret = Obj::erase(&begin,
                                         &end,
                                         deque.begin() + BEGIN,
                                         deque.begin() + FIRST,
                                         deque.begin() + LAST,
                                         deque.begin() + END,
                                         Z);

                        guard.release();

                        ASSERT(deque.begin() + EB == begin);
                        ASSERT(deque.begin() + EE == end);
                        ASSERT(deque.begin() + EI == ret);

                        verify(&deque, EXP);
                        cleanup(&deque, EXP);

                    } END_bslma_EXCEPTION_TEST
                }
                else {
                    Deque deque(SIZE, numBlocks, Z);
                    setInitPos(&deque, numBlocks, ip);
                    gg(&deque, SPEC);  verify(&deque, SPEC);

                    typename Deque::Iterator begin;
                    typename Deque::Iterator end;
                    typename Deque::Iterator ret;

                    ret = Obj::erase(&begin,
                                     &end,
                                     deque.begin() + BEGIN,
                                     deque.begin() + FIRST,
                                     deque.begin() + LAST,
                                     deque.begin() + END,
                                     Z);

                    ASSERT(deque.begin() + EB == begin);
                    ASSERT(deque.begin() + EE == end);
                    ASSERT(deque.begin() + EI == ret);

                    verify(&deque, EXP);
                    cleanup(&deque, EXP);
                }
            }
        }
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 2
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // start of [begin, end) range
    int         d_ne;        // number of elements (ne = end - begin).
    const char *d_expected;  // expected result array
} DATA_2[] = {
    // Order test data by increasing 'ne'.

    //line spec         begin    ne      expected
    //---- ----         -----    --      --------
    { L_,  "___",       1,       0,      "___"             },  // 0
    { L_,  "_b_",       1,       0,      "_b_"             },
    { L_,  "abc",       1,       0,      "abc"             },

    { L_,  "_b_",       1,       1,      "___"             },  // 1
    { L_,  "abc",       1,       1,      "a_c"             },

    { L_,  "_bc_",      1,       2,      "____"            },  // 2
    { L_,  "abcd",      1,       2,      "a__d"            },

    { L_,  "abcde",     1,       3,      "a___e"           },  // 3

    { L_,  "abcdef",    1,       4,      "a____f"          },  // 4

    { L_,  "abcdefg",   1,       5,      "a_____g"         },  // 5
};
const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

template <class TYPE, int BLOCK_LENGTH>
void testDestruct()
{
    typedef TestDeque<TYPE, BLOCK_LENGTH>               Deque;
    typedef bslalg::DequePrimitives<TYPE, BLOCK_LENGTH> Obj;

    for (int numBlocks  = CONFIG[BLOCK_LENGTH].d_blockNumMin;
             numBlocks <= CONFIG[BLOCK_LENGTH].d_blockNumMax;
           ++numBlocks) {

        if (g_veryVerbose) {
            printf("BLOCK_LENGTH = %d, BLOCK_NUM = %d\n",
                   BLOCK_LENGTH, numBlocks);
        }

        for (int ti = 0; ti < NUM_DATA_2; ++ti) {
            const int         LINE  = DATA_2[ti].d_lineNum;
            const char *const SPEC  = DATA_2[ti].d_spec;
            const int         BEGIN = DATA_2[ti].d_begin;
            const int         NE    = DATA_2[ti].d_ne;
            const char *const EXP   = DATA_2[ti].d_expected;
            ASSERT((int)std::strlen(SPEC) < BLOCK_LENGTH * numBlocks);

            const int SIZE = static_cast<int>(std::strlen(SPEC));

            if (g_veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, NE = %d, EXP = %s\n",
                       LINE, SPEC, BEGIN, NE, EXP);
            }

            for (int ip = 0; ip < numBlocks * BLOCK_LENGTH - SIZE; ++ip) {

                if (g_veryVerbose) {
                    printf("INITIAL POSITION = %d\n", ip);
                }

                Deque deque(SIZE, numBlocks, Z);
                setInitPos(&deque, numBlocks, ip);
                gg(&deque, SPEC);  verify(&deque, SPEC);

                Obj::destruct(deque.begin() + BEGIN,
                              deque.begin() + BEGIN + NE);

                verify(&deque, EXP);
                cleanup(&deque, EXP);
            }
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

                g_verbose =             verbose;
            g_veryVerbose =         veryVerbose;
        g_veryVeryVerbose =     veryVeryVerbose;
    g_veryVeryVeryVerbose = veryVeryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING insertAndMoveToFront(p, f, l)
        //
        // Concerns:
        //   1. 'insertAndMoveToFront' properly moves the original
        //      [fromBegin, position) objects to
        //      [fromBegin - numElements, position - numElements).
        //   2. 'insertAndMoveToFront' properly fills the range
        //      [position - numElements, position) with the values under the
        //      specified range [first, last).
        //   3. 'toBegin' is properly updated with the new start position.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'insertAndMoveToFront' on the deque and check with the
        //   "expected SPEC" (concerns 1, 2).  Also check the returned
        //   'toBegin' iterator (concern 3).  For exception safety, run the
        //   test under the standard bslma exception macro.
        //
        // Testing:
        //   void insertAndMoveToFront(Iterator          *toBegin,
        //                             Iterator           fromBegin,
        //                             Iterator           position,
        //                             FWD_ITER           first,
        //                             FWD_ITER           last,
        //                             size_type          numElements,
        //                             bslma::Allocator  *allocator);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'testInsertAndMoveToFrontRange'"
                            "\n=======================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testInsertAndMoveToFrontRange<TNA,1>(false);
        testInsertAndMoveToFrontRange<TNA,2>(false);
        testInsertAndMoveToFrontRange<TNA,3>(false);
        testInsertAndMoveToFrontRange<TNA,4>(false);
        testInsertAndMoveToFrontRange<TNA,3>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testInsertAndMoveToFrontRange<T,1>(false);
        testInsertAndMoveToFrontRange<T,2>(false);
        testInsertAndMoveToFrontRange<T,3>(false);
        testInsertAndMoveToFrontRange<T,4>(false);
        testInsertAndMoveToFrontRange<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testInsertAndMoveToFrontRange<BMT,1>(false);
        testInsertAndMoveToFrontRange<BMT,2>(false);
        testInsertAndMoveToFrontRange<BMT,3>(false);
        testInsertAndMoveToFrontRange<BMT,4>(false);
        testInsertAndMoveToFrontRange<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testInsertAndMoveToFrontRange<BCT,1>(false);
        testInsertAndMoveToFrontRange<BCT,2>(false);
        testInsertAndMoveToFrontRange<BCT,3>(false);
        testInsertAndMoveToFrontRange<BCT,4>(false);
        testInsertAndMoveToFrontRange<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testInsertAndMoveToFrontRange<T,1>(true);
        testInsertAndMoveToFrontRange<T,2>(true);
        testInsertAndMoveToFrontRange<T,3>(true);
        testInsertAndMoveToFrontRange<T,4>(true);
        testInsertAndMoveToFrontRange<T,5>(true);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING insertAndMoveToBack(p, f, l)
        //
        // Concerns:
        //   1. 'insertAndMoveToBack' properly moves the original
        //      [position, fromEnd) objects to
        //      [position + numElements, fromEnd + numElements).
        //   2. 'insertAndMoveToBack' properly fills the range
        //      [position, position + numElements) with the values under the
        //      specified range [first, last).
        //   3. 'toEnd' is properly updated with the new end position.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'insertAndMoveToBack' on the deque and check with the
        //   "expected SPEC" (concern 1, 2).  Also check the returned 'toEnd'
        //   iterator (concern 3).  For exception safety, run the test under
        //   the standard bslma exception macro.
        //
        // Testing:
        //   void insertAndMoveToBack(Iterator         *toEnd,
        //                            Iterator          fromEnd,
        //                            Iterator          position,
        //                            FWD_ITER          first,
        //                            FWD_ITER          last,
        //                            size_type         numElements,
        //                            bslma::Allocator *allocator);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'testInsertAndMoveToBack(p,f,l)'"
                            "\n========================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testInsertAndMoveToBackRange<TNA,1>(false);
        testInsertAndMoveToBackRange<TNA,2>(false);
        testInsertAndMoveToBackRange<TNA,3>(false);
        testInsertAndMoveToBackRange<TNA,4>(false);
        testInsertAndMoveToBackRange<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testInsertAndMoveToBackRange<T,1>(false);
        testInsertAndMoveToBackRange<T,2>(false);
        testInsertAndMoveToBackRange<T,3>(false);
        testInsertAndMoveToBackRange<T,4>(false);
        testInsertAndMoveToBackRange<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testInsertAndMoveToBackRange<BMT,1>(false);
        testInsertAndMoveToBackRange<BMT,2>(false);
        testInsertAndMoveToBackRange<BMT,3>(false);
        testInsertAndMoveToBackRange<BMT,4>(false);
        testInsertAndMoveToBackRange<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testInsertAndMoveToBackRange<BCT,1>(false);
        testInsertAndMoveToBackRange<BCT,2>(false);
        testInsertAndMoveToBackRange<BCT,3>(false);
        testInsertAndMoveToBackRange<BCT,4>(false);
        testInsertAndMoveToBackRange<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testInsertAndMoveToBackRange<T,1>(true);
        testInsertAndMoveToBackRange<T,2>(true);
        testInsertAndMoveToBackRange<T,3>(true);
        testInsertAndMoveToBackRange<T,4>(true);
        testInsertAndMoveToBackRange<T,5>(true);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING insertAndMoveToFront(p, n, v)
        //
        // Concerns:
        //   1. 'insertAndMoveToFront' properly moves the original
        //      [position, fromEnd) objects to
        //      [position + numElements, fromEnd + numElements).
        //   2. 'insertAndMoveToFront' properly fills the range
        //      [position, position + numElements) with the specified 'value'.
        //   3. 'toBegin' is properly updated with the new start position.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'insertAndMoveToFront' on the deque and check with the
        //   "expected SPEC" (concern 1, 2).  Also check the returned 'toEnd'
        //   iterator (concern 3).  For exception safety, run the test under
        //   the standard bslma exception macro.
        //
        // Testing:
        //   void insertAndMoveToFront(Iterator           *toBegin,
        //                             Iterator            fromBegin,
        //                             Iterator            position,
        //                             size_type           numElements,
        //                             const VALUE_TYPE&   value,
        //                             bslma::Allocator   *allocator);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'insertAndMoveToFront(n,p)'"
                            "\n===================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testInsertAndMoveToFrontRaw<TNA,1>(false);
        testInsertAndMoveToFrontRaw<TNA,2>(false);
        testInsertAndMoveToFrontRaw<TNA,3>(false);
        testInsertAndMoveToFrontRaw<TNA,4>(false);
        testInsertAndMoveToFrontRaw<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testInsertAndMoveToFrontRaw<T,1>(false);
        testInsertAndMoveToFrontRaw<T,2>(false);
        testInsertAndMoveToFrontRaw<T,3>(false);
        testInsertAndMoveToFrontRaw<T,4>(false);
        testInsertAndMoveToFrontRaw<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testInsertAndMoveToFrontRaw<BMT,1>(false);
        testInsertAndMoveToFrontRaw<BMT,2>(false);
        testInsertAndMoveToFrontRaw<BMT,3>(false);
        testInsertAndMoveToFrontRaw<BMT,4>(false);
        testInsertAndMoveToFrontRaw<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testInsertAndMoveToFrontRaw<BCT,1>(false);
        testInsertAndMoveToFrontRaw<BCT,2>(false);
        testInsertAndMoveToFrontRaw<BCT,3>(false);
        testInsertAndMoveToFrontRaw<BCT,4>(false);
        testInsertAndMoveToFrontRaw<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testInsertAndMoveToFrontRaw<T,1>(true);
        testInsertAndMoveToFrontRaw<T,2>(true);
        testInsertAndMoveToFrontRaw<T,3>(true);
        testInsertAndMoveToFrontRaw<T,4>(true);
        testInsertAndMoveToFrontRaw<T,5>(true);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING insertAndMoveToBack(p, n, v)
        //
        // Concerns:
        //   1. 'insertAndMoveToBack' properly moves the original
        //      [position, fromEnd) objects to
        //      [position + numElements, fromEnd + numElements).
        //   2. 'insertAndMoveToBack' properly fills the range
        //      [position, position + numElements) with the specified 'value'.
        //   3. 'toEnd' is properly updated with the new end position.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'insertAndMoveToBack' on the deque and check with the
        //   "expected SPEC" (concern 1, 2).  Also check the returned 'toEnd'
        //   iterator (concern 3).  For exception safety, run the test under
        //   the standard bslma exception macro.
        //
        // Testing:
        //   void insertAndMoveToBack(Iterator           *toEnd,
        //                            Iterator            fromEnd,
        //                            Iterator            position,
        //                            size_type           numElements,
        //                            const VALUE_TYPE&   value,
        //                            bslma::Allocator   *allocator);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'insertAndMoveToBack(p,n,v)'"
                            "\n====================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testInsertAndMoveToBackRaw<TNA,1>(false);
        testInsertAndMoveToBackRaw<TNA,2>(false);
        testInsertAndMoveToBackRaw<TNA,3>(false);
        testInsertAndMoveToBackRaw<TNA,4>(false);
        testInsertAndMoveToBackRaw<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testInsertAndMoveToBackRaw<T,1>(false);
        testInsertAndMoveToBackRaw<T,2>(false);
        testInsertAndMoveToBackRaw<T,3>(false);
        testInsertAndMoveToBackRaw<T,4>(false);
        testInsertAndMoveToBackRaw<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testInsertAndMoveToBackRaw<BMT,1>(false);
        testInsertAndMoveToBackRaw<BMT,2>(false);
        testInsertAndMoveToBackRaw<BMT,3>(false);
        testInsertAndMoveToBackRaw<BMT,4>(false);
        testInsertAndMoveToBackRaw<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testInsertAndMoveToBackRaw<BCT,1>(false);
        testInsertAndMoveToBackRaw<BCT,2>(false);
        testInsertAndMoveToBackRaw<BCT,3>(false);
        testInsertAndMoveToBackRaw<BCT,4>(false);
        testInsertAndMoveToBackRaw<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testInsertAndMoveToBackRaw<T,1>(true);
        testInsertAndMoveToBackRaw<T,2>(true);
        testInsertAndMoveToBackRaw<T,3>(true);
        testInsertAndMoveToBackRaw<T,4>(true);
        testInsertAndMoveToBackRaw<T,5>(true);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING uninitializedFillNFront
        //
        // Concerns:
        //   1. 'uninitializedFillNFront' properly fills
        //      [fromBegin - numElements, fromBegin) with the specified
        //      'value'.
        //   2. Nothing outside the range [fromBegin - numElements, fromBegin)
        //      is polluted.
        //   3. The 'toBegin' iterator is properly updated to reflect the new
        //      start of the deque.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'uninitializedFillNFront' on the deque and check with
        //   the "expected SPEC" (concern 1, 2).  Also check the returned
        //   'toBegin' iterator (concern 3).  For exception safety, run the
        //   test under the standard bslma exception macro.
        //
        // Testing:
        //   void uninitializedFillNFront(Iterator           *toBegin,
        //                                Iterator            fromBegin,
        //                                size_type           numElements,
        //                                const VALUE_TYPE&   value,
        //                                bslma::Allocator   *allocator)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'uninitializedFillNFront'"
                            "\n=================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testUninitializedFillNFront<TNA,1>(false);
        testUninitializedFillNFront<TNA,2>(false);
        testUninitializedFillNFront<TNA,3>(false);
        testUninitializedFillNFront<TNA,4>(false);
        testUninitializedFillNFront<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testUninitializedFillNFront<T,1>(false);
        testUninitializedFillNFront<T,2>(false);
        testUninitializedFillNFront<T,3>(false);
        testUninitializedFillNFront<T,4>(false);
        testUninitializedFillNFront<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testUninitializedFillNFront<BMT,1>(false);
        testUninitializedFillNFront<BMT,2>(false);
        testUninitializedFillNFront<BMT,3>(false);
        testUninitializedFillNFront<BMT,4>(false);
        testUninitializedFillNFront<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testUninitializedFillNFront<BCT,1>(false);
        testUninitializedFillNFront<BCT,2>(false);
        testUninitializedFillNFront<BCT,3>(false);
        testUninitializedFillNFront<BCT,4>(false);
        testUninitializedFillNFront<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testUninitializedFillNFront<T,1>(true);
        testUninitializedFillNFront<T,2>(true);
        testUninitializedFillNFront<T,3>(true);
        testUninitializedFillNFront<T,4>(true);
        testUninitializedFillNFront<T,5>(true);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING uninitializedFillNBack
        //
        // Concerns:
        //   1. 'uninitializedFillNBack' properly fills
        //      [fromEnd, fromEnd + numElements) with the specified 'value'.
        //   2. Nothing outside the range [fromEnd, fromEnd + numElements)
        //      is polluted.
        //   3. The 'toEnd' iterator is properly updated to reflect the new
        //      end of the deque.
        //   4. Exception safety.
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'uninitializedFillNBack' on the deque and check with the
        //   "expected SPEC" (concern 1, 2).  Also check the returned and
        //   'toEnd' iterators (concern 3).  For exception safety, run the test
        //   under the standard bslma exception macro.
        //
        // Testing:
        //   void uninitializedFillNBack(Iterator           *toEnd,
        //                               Iterator            fromEnd,
        //                               size_type           numElements,
        //                               const VALUE_TYPE&   value,
        //                               bslma::Allocator   *allocator)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'uninitializedFillNBack'"
                            "\n================================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testUninitializedFillNBack<TNA,1>(false);
        testUninitializedFillNBack<TNA,2>(false);
        testUninitializedFillNBack<TNA,3>(false);
        testUninitializedFillNBack<TNA,4>(false);
        testUninitializedFillNBack<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testUninitializedFillNBack<T,1>(false);
        testUninitializedFillNBack<T,2>(false);
        testUninitializedFillNBack<T,3>(false);
        testUninitializedFillNBack<T,4>(false);
        testUninitializedFillNBack<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testUninitializedFillNBack<BMT,1>(false);
        testUninitializedFillNBack<BMT,2>(false);
        testUninitializedFillNBack<BMT,3>(false);
        testUninitializedFillNBack<BMT,4>(false);
        testUninitializedFillNBack<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testUninitializedFillNBack<BCT,1>(false);
        testUninitializedFillNBack<BCT,2>(false);
        testUninitializedFillNBack<BCT,3>(false);
        testUninitializedFillNBack<BCT,4>(false);
        testUninitializedFillNBack<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testUninitializedFillNBack<T,1>(true);
        testUninitializedFillNBack<T,2>(true);
        testUninitializedFillNBack<T,3>(true);
        testUninitializedFillNBack<T,4>(true);
        testUninitializedFillNBack<T,5>(true);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING erase
        //
        // Concerns:
        //   1. 'erase' properly destroys objects in the range
        //      [first, last).
        //   2. 'erase' properly updates 'toBegin' and 'toEnd'.
        //   3. 'erase' properly shifts the smaller portion of the front / end
        //      to fill in the gaps after erasing.
        //   4. Exception safety
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'erase' over a range of the deque and check with the
        //   "expected SPEC" (concern 1, 3).  Also check the returned 'toBegin'
        //   and 'toEnd' iterators (concern 2).  For exception safety, run
        //   the test under the standard bslma exception macro.
        //
        // Testing:
        //   void erase(Iterator         *toBegin,
        //              Iterator         *toEnd,
        //              Iterator          fromBegin
        //              Iterator          first,
        //              Iterator          last,
        //              Iterator          fromEnd,
        //              bslma::Allocator *allocator)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'erase'"
                            "\n===============\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testErase<TNA,1>(false);
        testErase<TNA,2>(false);
        testErase<TNA,3>(false);
        testErase<TNA,4>(false);
        testErase<TNA,5>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testErase<T,1>(false);
        testErase<T,2>(false);
        testErase<T,3>(false);
        testErase<T,4>(false);
        testErase<T,5>(false);

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testErase<BMT,1>(false);
        testErase<BMT,2>(false);
        testErase<BMT,3>(false);
        testErase<BMT,4>(false);
        testErase<BMT,5>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testErase<BCT,1>(false);
        testErase<BCT,2>(false);
        testErase<BCT,3>(false);
        testErase<BCT,4>(false);
        testErase<BCT,5>(false);

        if (verbose) printf("\n\tException test.\n");
        testErase<T,1>(true);
        testErase<T,2>(true);
        testErase<T,3>(true);
        testErase<T,4>(true);
        testErase<T,5>(true);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING destruct
        //
        // Concerns:
        //   1. 'destruct' properly destroys objects in range [begin, end).
        //   2. 'destruct' does not destroy objects outside the range
        //      [begin, end).
        //
        // Plan:
        //   Create objects in a deque-like structure using a "source SPEC".
        //   Then run 'destruct' over a range of the deque and check with the
        //   "expected SPEC" (concern 2).  Clean up according to the
        //   "expected SPEC".  If destruct did not work as expected, the test
        //   allocator will detect memory leaks (concern 1).
        //
        // Testing:
        //   void destruct(Iterator begin, Iterator end)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'destruct'"
                            "\n=================\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testDestruct<TNA,1>();
        testDestruct<TNA,2>();
        testDestruct<TNA,3>();
        testDestruct<TNA,4>();
        testDestruct<TNA,5>();

        if (verbose) printf("\n\t...with TestType.\n");
        testDestruct<T,1>();
        testDestruct<T,2>();
        testDestruct<T,3>();
        testDestruct<T,4>();
        testDestruct<T,5>();

        if (verbose) printf("\n\t...with BitwiseMoveableTestType.\n");
        testDestruct<BMT,1>();
        testDestruct<BMT,2>();
        testDestruct<BMT,3>();
        testDestruct<BMT,4>();
        testDestruct<BMT,5>();

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testDestruct<BCT,1>();
        testDestruct<BCT,2>();
        testDestruct<BCT,3>();
        testDestruct<BCT,4>();
        testDestruct<BCT,5>();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This test exercises the component but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

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
