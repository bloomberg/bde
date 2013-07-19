// bslalg_arrayprimitives.t.cpp                                       -*-C++-*-

#include <bslalg_arrayprimitives.h>

#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>

#include <bslma_allocator.h>              // for testing only
#include <bslma_default.h>                // for testing only
#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only
#include <bsls_alignmentutil.h>           // for testing only
#include <bsls_objectbuffer.h>            // for testing only
#include <bsls_platform.h>                // for testing only
#include <bsls_stopwatch.h>               // for testing only

#include <bsls_types.h>

#include <cstdio>
#include <cstdlib>     // atoi()
#include <cstring>     // strlen()
#include <ctype.h>     // isalpha()

using namespace BloombergLP;
using namespace std;

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
// verify that there is no overloading ambiguity by instantiating (as well as
// running) a comprehensive selection of overloads, taking care to have
// standard as well as user conversions in the candidate selection.
//
// In order to facilitate the generation of test object instances, we make a
// text object have the value semantics of a 'char', and generate an array of
// test objects from a string specification via a generating function
// parameterized by the actual test object type.  This lets us reuse the same
// test code for bitwise-copyable/moveable test types as well as those that do
// not have those traits.
//-----------------------------------------------------------------------------
// bslalg::ArrayPrimitives public interface:
// [ 2] void uninitializedFillN(T *dstB, ne, const T& v, *a);
// [ 3] void copyConstruct(T *dstB, FWD srcB, FWD srcE, *a);
// [ 4] void destructiveMove(T *dstB, T *srcB, T *srcE, *a);
// [ 5] void insert(T *dstB, T *dstE, const T& v, ne, *a);
// [ 5] void insert(T *dstB, T *dstE, FWD srcB, FWD srcE, ne, *a);
// [ 5] void moveInsert(T *dstB, T *dstE, T **srcEp, srcB, srcE, ne, *a);
// [ 6] void destructiveMoveAndInsert(...);
// [ 4] void destructiveMoveAndMoveInsert(...);
// [ 7] void erase(T *srcE, T *srcPos, T *srcE, *a);
// [ 8] void rotate(T *dstB, T *dstE, n, *a);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::ArrayPrimitives        Obj;

// TYPES
class TestType;
class TestTypeNoAlloc;
class BitwiseMoveableTestType;
class BitwiseCopyableTestType;
typedef int (*FuncPtrType)();

template <int ADDITIONAL_FOOTPRINT>  class LargeBitwiseMoveableTestType;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef BitwiseMoveableTestType       BMT;  // uses 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // does not use 'bslma' allocators
typedef FuncPtrType                   FPT;  // does not use 'bslma' allocators

typedef LargeBitwiseMoveableTestType<8>   XXL8;   // uses 'bslma' allocators
typedef LargeBitwiseMoveableTestType<16>  XXL16;  // uses 'bslma' allocators
typedef LargeBitwiseMoveableTestType<24>  XXL24;  // uses 'bslma' allocators
typedef LargeBitwiseMoveableTestType<32>  XXL32;  // uses 'bslma' allocators
typedef LargeBitwiseMoveableTestType<64>  XXL64;  // uses 'bslma' allocators
typedef LargeBitwiseMoveableTestType<128> XXL128; // uses 'bslma' allocators

typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;
typedef bsls::Types::UintPtr    UintPtr;

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma::TestAllocator *Z;  // initialized at the start of main()

template <class C>
char getValue(const C& c)
{
    return c.datum();
}

                                // ===========
                                // FuncPtrType
                                // ===========

template <int N>
int funcTemplate()
{
    return N;
}

FuncPtrType funcPtrArray[128];

// Initialize 'funcPtrArray' so that 'funcPtrArray[N] == &funcTemplate<N>'.

template <int FROM, int TO>
struct InitFuncPtrArray {
    InitFuncPtrArray()
    {
        funcPtrArray[FROM] = &funcTemplate<FROM>;
        InitFuncPtrArray<FROM + 1, TO> recurse;
    }
};
template <int END>
struct InitFuncPtrArray<END, END> {
    InitFuncPtrArray()
    {
        funcPtrArray[END] = &funcTemplate<END>;
    }
};

InitFuncPtrArray<  0,  15> initFuncPtrArray0;
InitFuncPtrArray< 16,  31> initFuncPtrArray16;
InitFuncPtrArray< 32,  47> initFuncPtrArray32;
InitFuncPtrArray< 48,  63> initFuncPtrArray48;
InitFuncPtrArray< 64,  79> initFuncPtrArray64;
InitFuncPtrArray< 80,  95> initFuncPtrArray80;
InitFuncPtrArray< 96, 111> initFuncPtrArray96;
InitFuncPtrArray<112, 127> initFuncPtrArray112;


char getValue(const FuncPtrType& fpt)
{
    return (char) (*fpt)();
}

void setValue(FuncPtrType *fpt, char ch)
{
    *fpt = funcPtrArray[ch];
}

                              // =============
                              // MemberFuncPtr
                              // =============

struct Thing {
    template <int N>
    int memberFuncTemplate()
    {
        return N;
    }
};

typedef int (Thing::*MemberFuncPtrType)();
typedef MemberFuncPtrType MFPT;

MemberFuncPtrType memberFuncPtrArray[128];

template <int FROM, int TO>
struct InitMemberFuncPtrArray {
    InitMemberFuncPtrArray()
    {
        memberFuncPtrArray[FROM] = &Thing::memberFuncTemplate<FROM>;
        InitMemberFuncPtrArray<FROM + 1, TO> recurse;
    }
};
template <int END>
struct InitMemberFuncPtrArray<END, END> {
    InitMemberFuncPtrArray()
    {
        memberFuncPtrArray[END] = &Thing::memberFuncTemplate<END>;
    }
};

InitMemberFuncPtrArray<  0,  15> initMemberFuncPtrArray0;
InitMemberFuncPtrArray< 16,  31> initMemberFuncPtrArray16;
InitMemberFuncPtrArray< 32,  47> initMemberFuncPtrArray32;
InitMemberFuncPtrArray< 48,  63> initMemberFuncPtrArray48;
InitMemberFuncPtrArray< 64,  79> initMemberFuncPtrArray64;
InitMemberFuncPtrArray< 80,  95> initMemberFuncPtrArray80;
InitMemberFuncPtrArray< 96, 111> initMemberFuncPtrArray96;
InitMemberFuncPtrArray<112, 127> initMemberFuncPtrArray112;


char getValue(const MemberFuncPtrType& mfpt)
{
    Thing t;
    return (char) (t.*mfpt)();
}

void setValue(MemberFuncPtrType *mfpt, char ch)
{
    *mfpt = memberFuncPtrArray[ch];
}

                                // ==========
                                // char types
                                // ==========

void setValue(char *c, char ch)
{
    *c = ch;
}

void setValue(signed char *c, char ch)
{
    *c = ch;
}

void setValue(unsigned char *c, char ch)
{
    *c = ch;
}

char getValue(const char& c)
{
    return c;
}

char getValue(const signed char& c)
{
    return c;
}

char getValue(const unsigned char& c)
{
    return c;
}
                                // ===========
                                // short types
                                // ===========

void setValue(short *s, char ch)
{
    *s = ch;
}

void setValue(unsigned short *s, char ch)
{
    *s = ch;
}

char getValue(const short& s)
{
    return (char) s;
}

char getValue(const unsigned short& s)
{
    return (char) s;
}

                                // =========
                                // int types
                                // =========

void setValue(int *pi, char ch)
{
    *pi = ch;
}

void setValue(unsigned int *pi, char ch)
{
    *pi = ch;
}

char getValue(const int& i)
{
    return (char) i;
}

char getValue(const unsigned int& i)
{
    return (char) i;
}

                                // ==========
                                // long types
                                // ==========

void setValue(long *pl, char ch)
{
    *pl = ch;
}

void setValue(unsigned long *pl, char ch)
{
    *pl = ch;
}

char getValue(const long& ll)
{
    return (char) ll;
}

char getValue(const unsigned long& ll)
{
    return (char) ll;
}

                            // =====================
                            // 64 bit integral types
                            // =====================

void setValue(Int64 *p64, char ch)
{
    *p64 = ch;
}

void setValue(Uint64 *p64, char ch)
{
    *p64 = ch;
}

char getValue(const Int64& i64)
{
    return (char) i64;
}

char getValue(const Uint64& u64)
{
    return (char) u64;
}

                            // ====================
                            // floating point types
                            // ====================

void setValue(float *pf, char ch)
{
    *pf = (int) ch;
}

void setValue(double *pf, char ch)
{
    *pf = (int) ch;
}

void setValue(long double *pf, char ch)
{
    *pf = (int) ch;
}

char getValue(const float& f)
{
    return (char) ((int) f & 0xff);
}

char getValue(const double& f)
{
    return (char) ((int) f & 0xff);
}

char getValue(const long double& f)
{
    return (char) ((int) f & 0xff);
}

                                // ========
                                // void *'s
                                // ========

void setValue(void **pvs, char ch)
{
    *pvs = (void *) (UintPtr) ch;
}

void setValue(const void **pvs, char ch)
{
    *pvs = (const void *) (UintPtr) ch;
}

#ifndef BSLS_PLATFORM_OS_AIX
char getValue(void * const& vs)
{
    return (char) ((UintPtr) vs & 0xff);
}
#endif

char getValue(const void * const& vs)
{
    return (char) ((UintPtr) vs & 0xff);
}

                                // =======
                                // int *'s
                                // =======

void setValue(int **pis, char ch)
{
    *pis = (int *) (UintPtr) ch;
}

void setValue(const int **pis, char ch)
{
    *pis = (const int *) (UintPtr) ch;
}

#ifndef BSLS_PLATFORM_OS_AIX
char getValue(int * const& is)
{
    return (char) ((UintPtr) is & 0xff);
}
#endif

char getValue(const int * const& is)
{
    return (char) ((UintPtr) is & 0xff);
}

                           // ======================
                           // class ConstructEnabler
                           // ======================

struct ConstructEnabler {
    // DATA
    char d_c;

    ConstructEnabler() : d_c(0) {}
    explicit
    ConstructEnabler(char ch) : d_c(ch) {}
    ConstructEnabler& operator=(char ch)
    {
        d_c = ch;
        return *this;
    }

    operator FuncPtrType() const
    {
        FuncPtrType fpt;
        setValue(&fpt, d_c);
        return fpt;
    }

    operator char() const
    {
        return d_c;
    }

    operator void *() const
    {
        return (void *) (UintPtr) d_c;
    }

    operator int *() const
    {
        return (int *) (UintPtr) d_c;
    }

    operator MemberFuncPtrType() const
    {
        MemberFuncPtrType mfpt;
        setValue(&mfpt, d_c);
        return mfpt;
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

  protected:
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

    TestType(const ConstructEnabler cE, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = cE.d_c;
    }

    friend void setValue(TestType *c, char ch)
    {
        *c->d_data_p = ch;
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
}
}

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

  protected:
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

#if 0
    // Killed char c'tor

    TestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        ++numCharCtorCalls;
    }
#else
    TestTypeNoAlloc(const ConstructEnabler& cE)
    {
        d_u.d_char = cE.d_c;
        ++numCharCtorCalls;
    }

    friend void setValue(TestTypeNoAlloc *c, char ch)
    {
        c->d_u.d_char = ch;
    }
#endif

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

#if 0
    // Killed char c'tor

    BitwiseMoveableTestType(char c, bslma::Allocator *ba = 0)
    : TestType(c, ba)
    {
    }
#else
    BitwiseMoveableTestType(const ConstructEnabler&  cE,
                            bslma::Allocator        *ba = 0)
    : TestType(ba)
    {
        *d_data_p = cE.d_c;
    }

    friend void setValue(BitwiseMoveableTestType *c, char ch)
    {
        *c->d_data_p = ch;
    }
#endif

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
}
namespace bslmf {
template <> struct IsBitwiseMoveable<BitwiseMoveableTestType>
    : bsl::true_type {};
}
}

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

#if 0
    // Killed char c'tor

    BitwiseCopyableTestType(char c)
    : TestTypeNoAlloc(c)
    {
    }
#else
    BitwiseCopyableTestType(const ConstructEnabler& cE)
    : TestTypeNoAlloc()
    {
        d_u.d_char = cE.d_c;
    }

    friend void setValue(BitwiseCopyableTestType *c, char ch)
    {
        c->d_u.d_char = ch;
    }
#endif

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : TestTypeNoAlloc()
    {
        setValue(this, original.datum());
    }
};

// TRAITS
namespace bsl {
template <> struct is_trivially_copyable<BitwiseCopyableTestType>
    : true_type {};
}

                       // ==================================
                       // class LargeBitwiseMoveableTestType
                       // ==================================

template <int FOOTPRINT>
class LargeBitwiseMoveableTestType : public TestType {
    // This test type is identical to 'TestType' except that it has the
    // bit-wise moveable trait, and an additional parameterized 'FOOTPRINT'.
    // All members are inherited.

    // DATA
    int d_junk[FOOTPRINT];

  public:
    // CREATORS
    LargeBitwiseMoveableTestType(bslma::Allocator *ba = 0)
    : TestType(ba)
    {
        for (int i = 0; i < FOOTPRINT; ++i) {
            d_junk[i] = i;
        }
    }

#if 0
    // Killed char c'tor

    LargeBitwiseMoveableTestType(char c, bslma::Allocator *ba = 0)
    : TestType(c, ba)
    {
        for (int i = 0; i < FOOTPRINT; ++i) {
            d_junk[i] = i;
        }
    }
#else
    LargeBitwiseMoveableTestType(const ConstructEnabler&  cE,
                                 bslma::Allocator        *ba = 0)
    : TestType(ba)
    {
        d_data_p = cE.d_c;
        for (int i = 0; i < FOOTPRINT; ++i) {
            d_junk[i] = i;
        }
    }

    friend void setValue(LargeBitwiseMoveableTestType *c, char ch)
    {
        *c->d_data_p = ch;
    }
#endif

    LargeBitwiseMoveableTestType(const LargeBitwiseMoveableTestType&  original,
                                 bslma::Allocator                    *ba = 0)
    : TestType(original, ba)
    {
        for (int i = 0; i < FOOTPRINT; ++i) {
            d_junk[i] = i;
        }
    }

    ~LargeBitwiseMoveableTestType()
    {
        for (int i = 0; i < FOOTPRINT; ++i) {
            ASSERT(d_junk[i] == i);
        }
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <int FOOTPRINT>
struct UsesBslmaAllocator<LargeBitwiseMoveableTestType<FOOTPRINT> >
    : bsl::true_type {};
}

namespace bslmf {
template <int FOOTPRINT>
struct IsBitwiseMoveable<LargeBitwiseMoveableTestType<FOOTPRINT> >
    : bsl::true_type {};
}
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
class CleanupGuard {
    // This proctor is responsible to create, in an array specified at
    // construction, a sequence according to some specification.  Upon
    // destruction, it destroys elements in that array according to the current
    // specifications.  For '0 <= i < strlen(spec)', 'array[i]' is destroyed if
    // and only if '1 == isalpha(spec[i])' and in addition, if a reference to
    // an end pointer is specified at construction, if 'i < *specEnd - spec'.
    // If a tests succeeds, the specifications can be changed to allow for
    // different (un)initialized elements.

    // DATA
    TYPE        *d_array_p;
    const char  *d_spec_p;
    TYPE       **d_endPtr_p;
    TYPE        *d_initialEndPtr_p;
    size_t       d_length;

  public:
    // CREATORS
    CleanupGuard(TYPE *array, const char *spec, TYPE**endPtr = 0)
    : d_array_p(array)
    , d_spec_p(spec)
    , d_endPtr_p(endPtr)
    , d_initialEndPtr_p(endPtr ? *endPtr : 0)
    , d_length(strlen(spec))
    {
    }

    ~CleanupGuard()
    {
        for (int i = 0; d_spec_p[i] && i < d_length; ++i) {
            char c = d_spec_p[i];
            if (isalpha(c)) {
                if (d_endPtr_p && *d_endPtr_p - d_array_p <= i &&
                                           i < d_initialEndPtr_p - d_array_p) {
                    continue; // those elements have already been moved
                }
                bslalg::ScalarDestructionPrimitives::destroy(d_array_p + i);
            }
            else {
                LOOP_ASSERT(i, '_' == c);
            }
        }
    }

    // MANIPULATORS
    void setLength(int length) {
        d_length = length;
    }

    void release(const char *newSpec) {
        d_spec_p = newSpec;
        d_length = strlen(newSpec);
        d_endPtr_p = 0;
    }
};

void cleanup(char *array, const char *spec)
    // Destroy elements in the specified 'array' according to the specified
    // 'spec'.  For '0 <= i < strlen(spec)', 'array[i]' is destroyed if and
    // only if '1 == isalpha(spec[i])'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP_ASSERT(i, array[i] == c);
            array[i] = '_';
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

template <class TYPE>
void cleanup(TYPE *array, const char *spec)
    // Destroy elements in the specified 'array' according to the specified
    // 'spec'.  For '0 <= i < strlen(spec)', 'array[i]' is destroyed if and
    // only if '1 == isalpha(spec[i])'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP_ASSERT(i, getValue(array[i]) == c);
            bslalg::ScalarDestructionPrimitives::destroy(array + i);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

void verify(char *array, const char *spec)
    // Verify that elements in the specified 'array' have values according to
    // the specified 'spec'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP3_ASSERT(i, array[i], c, array[i] == c);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

template <class TYPE>
void verify(TYPE *array, const char *spec)
    // Verify that elements in the specified 'array' have values according to
    // the specified 'spec'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP3_ASSERT(i, getValue(array[i]), c, getValue(array[i]) == c);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

void fillWithJunk(void *buf, int size)
{
    const int MAX_VALUE = 127;

    char *p = reinterpret_cast<char*>(buf);

    for (int i = 0; i < size; ++i) {
        p[i] = (char)(i % MAX_VALUE) + 1;
    }
}

//=============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure an array according to a custom language.  Letters
// [a .. z, A .. Z] correspond to arbitrary (but unique) char values used to
// initialize elements of an array of 'T' objects.  An underscore ('_')
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
// ""           Leaves the array unaffected.
// "a"          ...
//-----------------------------------------------------------------------------

template <class TYPE>
int ggg(TYPE *array, const char *spec, int verboseFlag = 1)
    // Configure the specified 'array' of objects of the parameterized 'TYPE'
    // (assumed to be uninitialized) according to the specified 'spec'.
    // Optionally specify a zero 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
    //
    // Note that this generator is used in exception tests, and thus need to be
    // exception-safe.
{
    CleanupGuard<TYPE> guard(array, spec);
    guard.setLength(0);

    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i, ++array) {
        char c = spec[i];
        guard.setLength(i);
        if (isalpha(c)) {
            bslalg::ScalarPrimitives::defaultConstruct(array, Z);
            setValue(array, c);
        }
        else if ('_' == c) {
            continue;
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') in spec \"%s\""
                       " at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.
        }
    }
    guard.setLength(0);
    return SUCCESS;
}

template <class TYPE>
TYPE& gg(TYPE *array, const char *spec)
    // Return a reference to the modifiable first element of the specified
    // 'array' after the value of 'array' has been adjusted according to the
    // specified 'spec'.
{
    ASSERT(ggg(array, spec) < 0);
    return *array;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 8
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // index to start of range to rotate
    int         d_m;         // positions by which to rotate
    int         d_end;       // end of range to rotate
    const char *d_expected;  // expected result array
} DATA_8[] = {
    //line spec           begin  m   end  expected
    //---- ----           -----  --  ---  --------        // ADJUST
    { L_,  "abc",            1,  0,  1,   "abc"            },  // 0

    { L_,  "abc",            1,  0,  2,   "abc"            },  // 1
    { L_,  "abc",            1,  1,  2,   "abc"            },

    { L_,  "abcd",           1,  0,  3,   "abcd"           },  // 2
    { L_,  "abcd",           1,  1,  3,   "acbd"           },
    { L_,  "abcd",           1,  2,  3,   "abcd"           },

    { L_,  "abcde",          1,  0,  4,   "abcde"          },  // 3
    { L_,  "abcde",          1,  1,  4,   "acdbe"          },
    { L_,  "abcde",          1,  2,  4,   "adbce"          },
    { L_,  "abcde",          1,  3,  4,   "abcde"          },

    { L_,  "abcdef",         1,  0,  5,   "abcdef"         },  // 4
    { L_,  "abcdef",         1,  1,  5,   "acdebf"         },
    { L_,  "abcdef",         1,  2,  5,   "adebcf"         },
    { L_,  "abcdef",         1,  3,  5,   "aebcdf"         },
    { L_,  "abcdef",         1,  4,  5,   "abcdef"         },

    { L_,  "abcdefg",        1,  0,  6,   "abcdefg"        },  // 5
    { L_,  "abcdefg",        1,  1,  6,   "acdefbg"        },
    { L_,  "abcdefg",        1,  2,  6,   "adefbcg"        },
    { L_,  "abcdefg",        1,  3,  6,   "aefbcdg"        },
    { L_,  "abcdefg",        1,  4,  6,   "afbcdeg"        },
    { L_,  "abcdefg",        1,  5,  6,   "abcdefg"        },

    { L_,  "abcdefgh",       1,  0,  7,   "abcdefgh"       },  // 6
    { L_,  "abcdefgh",       1,  1,  7,   "acdefgbh"       },
    { L_,  "abcdefgh",       1,  2,  7,   "adefgbch"       },
    { L_,  "abcdefgh",       1,  3,  7,   "aefgbcdh"       },
    { L_,  "abcdefgh",       1,  4,  7,   "afgbcdeh"       },
    { L_,  "abcdefgh",       1,  5,  7,   "agbcdefh"       },
    { L_,  "abcdefgh",       1,  6,  7,   "abcdefgh"       },

    { L_,  "abcdefghi",      1,  0,  8,   "abcdefghi"      },  // 7
    { L_,  "abcdefghi",      1,  1,  8,   "acdefghbi"      },
    { L_,  "abcdefghi",      1,  6,  8,   "ahbcdefgi"      },
    { L_,  "abcdefghi",      1,  7,  8,   "abcdefghi"      },

    { L_,  "abcdefghij",     1,  0,  9,   "abcdefghij"     },  // 8
    { L_,  "abcdefghij",     1,  1,  9,   "acdefghibj"     },
    { L_,  "abcdefghij",     1,  2,  9,   "adefghibcj"     },
    { L_,  "abcdefghij",     1,  4,  9,   "afghibcdej"     },
    { L_,  "abcdefghij",     1,  7,  9,   "aibcdefghj"     },
    { L_,  "abcdefghij",     1,  8,  9,   "abcdefghij"     },

    { L_,  "abcdefghijk",    1,  0, 10,   "abcdefghijk"    },  // 9
    { L_,  "abcdefghijk",    1,  1, 10,   "acdefghijbk"    },
    { L_,  "abcdefghijk",    1,  3, 10,   "aefghijbcdk"    },
    { L_,  "abcdefghijk",    1,  6, 10,   "ahijbcdefgk"    },
    { L_,  "abcdefghijk",    1,  8, 10,   "ajbcdefghik"    },
    { L_,  "abcdefghijk",    1,  9, 10,   "abcdefghijk"    },

    { L_,  "abcdefghijkl",   1,  0, 11,   "abcdefghijkl"   }, // 10
    { L_,  "abcdefghijkl",   1,  1, 11,   "acdefghijkbl"   },
    { L_,  "abcdefghijkl",   1,  2, 11,   "adefghijkbcl"   },
    { L_,  "abcdefghijkl",   1,  5, 11,   "aghijkbcdefl"   },
    { L_,  "abcdefghijkl",   1,  8, 11,   "ajkbcdefghil"   },
    { L_,  "abcdefghijkl",   1,  9, 11,   "akbcdefghijl"   },
    { L_,  "abcdefghijkl",   1, 10, 11,   "abcdefghijkl"   },
};
const int NUM_DATA_8 = sizeof DATA_8 / sizeof *DATA_8;

template <class TYPE>
void testRotate(bool bitwiseMoveableFlag,
                bool,
                bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_8' elements of
    // the 'DATA_8' array, that rotating by 'd_m' positions the entries
    // between 'd_begin' until the 'd_end' indices in a buffer built according
    // to the 'd_spec' specifications results in a buffer built according to
    // the 'd_expected' specifications.  The 'd_lineNum' member is used to
    // report errors.
{
    const int MAX_SIZE = 32;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

    for (int ti = 0; ti < NUM_DATA_8; ++ti) {
        const int         LINE  = DATA_8[ti].d_lineNum;
        const char *const SPEC  = DATA_8[ti].d_spec;
        const int         BEGIN = DATA_8[ti].d_begin;
        const int         M     = DATA_8[ti].d_m;
        const int         END   = DATA_8[ti].d_end;
        const char *const EXP   = DATA_8[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, "
                   "BEGIN = %d, M = %d, END = %d, EXP = %s\n",
                   LINE, SPEC, BEGIN, M, END, EXP);
        }

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                gg(buf, SPEC);  verify(buf, SPEC);

                TYPE *end = buf + (int)std::strlen(SPEC);
                CleanupGuard<TYPE> guard(buf, SPEC, &end);

                Obj::rotate(&buf[BEGIN], &buf[BEGIN + M], &buf[END]);

                verify(buf, EXP);
                guard.release(EXP);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            gg(buf, SPEC);  verify(buf, SPEC);

            const int NUM_COPIES       = numCopyCtorCalls;
            const int NUM_DESTRUCTIONS = numDestructorCalls;

            Obj::rotate(&buf[BEGIN], &buf[BEGIN + M], &buf[END]);

            if (bitwiseMoveableFlag) {
                ASSERT(NUM_COPIES       == numCopyCtorCalls);
                ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
            }

            verify(buf, EXP);
            cleanup(buf, EXP);
        }

        ASSERT(0 == Z->numBytesInUse());
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 7
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // index to start of range to erase
    int         d_ne;        // number of elements to erase
    int         d_end;       // end of data
    const char *d_expected;  // expected result array
} DATA_7[] = {
    //line spec         begin  ne    end     expected      ordered by 'ne'
    //---- ----         -----  --    ---     --------      ---------------
    { L_,  "___",       1,     0,    1,      "___"         },  // 0
    { L_,  "_b_",       1,     0,    1,      "_b_"         },
    { L_,  "_b_",       1,     0,    2,      "_b_"         },
    { L_,  "abc",       1,     0,    1,      "abc"         },
    { L_,  "abc",       1,     0,    2,      "abc"         },

    { L_,  "abc",       1,     1,    2,      "a_c"         },  // 1
    { L_,  "abcd",      1,     1,    3,      "ac_d"        },
    { L_,  "abcde",     1,     1,    4,      "acd_e"       },
    { L_,  "abcdef",    1,     1,    5,      "acde_f"      },

    { L_,  "abcd",      1,     2,    3,      "a__d"        },  // 2
    { L_,  "abcde",     1,     2,    4,      "ad__e"       },
    { L_,  "abcdef",    1,     2,    5,      "ade__f"      },
    { L_,  "abcdefg",   1,     2,    6,      "adef__g"     },

    { L_,  "abcde",     1,     3,    4,      "a___e"       },  // 3
    { L_,  "abcdef",    1,     3,    5,      "ae___f"      },
    { L_,  "abcdefg",   1,     3,    6,      "aef___g"     },
    { L_,  "abcdefgh",  1,     3,    7,      "aefg___h"    },

    { L_,  "abcdef",    1,     4,    5,      "a____f"      },  // 4
    { L_,  "abcdefg",   1,     4,    6,      "af____g"     },
    { L_,  "abcdefgh",  1,     4,    7,      "afg____h"    },
    { L_,  "abcdefghi", 1,     4,    8,      "afgh____i"   },
};
const int NUM_DATA_7 = sizeof DATA_7 / sizeof *DATA_7;

template <class TYPE>
void testErase(bool,
               bool,
               bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_7' elements of
    // the 'DATA_7' array, that erasing the 'd_ne' entries at
    // the 'd_begin' index while shifting the entries between 'd_dst'
    // until the 'd_end' indices in a buffer built according to the 'd_spec'
    // specifications results in a buffer built according to the 'd_expected'
    // specifications.  The 'd_lineNum' member is used to report errors.
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

    for (int ti = 0; ti < NUM_DATA_7; ++ti) {
        const int         LINE  = DATA_7[ti].d_lineNum;
        const char *const SPEC  = DATA_7[ti].d_spec;
        const int         BEGIN = DATA_7[ti].d_begin;
        const int         NE    = DATA_7[ti].d_ne;
        const int         END   = DATA_7[ti].d_end;
        const char *const EXP   = DATA_7[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, "
                   "BEGIN = %d, NE = %d, END = %d, EXP = %s\n",
                   LINE, SPEC, BEGIN, NE, END, EXP);
        }

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                gg(buf, SPEC);  verify(buf, SPEC);
                CleanupGuard<TYPE> cleanup(buf, SPEC);

                Obj::erase(&buf[BEGIN], &buf[BEGIN + NE], &buf[END], Z);

                verify(buf, EXP);
                cleanup.release(EXP);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            gg(buf, SPEC);  verify(buf, SPEC);

            Obj::erase(&buf[BEGIN], &buf[BEGIN + NE], &buf[END], Z);

            verify(buf, EXP);
            cleanup(buf, EXP);
        }

        ASSERT(0 == Z->numBytesInUse());
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 6
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_srcSpec;   // specification string
    int         d_ne;        // number of elements to insert
    int         d_begin;     // start of data
    int         d_dst;       // index of insertion point
    int         d_end;       // end of data
    const char *d_srcExp;    // expected result source array
    const char *d_dstExp;    // expected result destination array
} DATA_6V[] = {
    //                       v----------------------------------------  Ordered
    //line srcSpec          ne  beg dst end srcExp           dstExp     by ne
    //---- -------          --  --- --- --- --------         ------     -------
    { L_, "___",            0,  1,  1,  1,  "___",           "___"           },
    { L_, "a_c",            0,  1,  1,  1,  "a_c",           "___"           },
    { L_, "abc",            0,  1,  1,  2,  "a_c",           "_b_"           },

    { L_, "abc",            0,  1,  2,  2,  "a_c",           "_b_"           },

    { L_, "a_c",            1,  1,  1,  1,  "a_c",           "_V_"           },
    { L_, "ab_d",           1,  1,  1,  2,  "a__d",          "_Vb_"          },
    { L_, "abc_e",          1,  1,  1,  3,  "a___e",         "_Vbc_"         },
    { L_, "abcd_f",         1,  1,  1,  4,  "a____f",        "_Vbcd_"        },
    { L_, "abcde_g",        1,  1,  1,  5,  "a_____g",       "_Vbcde_"       },

    { L_, "abc_e",          1,  1,  2,  3,  "a___e",         "_bVc_"         },

    { L_, "a_c",            1,  1,  1,  1,  "a_c",           "_V_"           },
    { L_, "ab_d",           1,  1,  2,  2,  "a__d",          "_bV_"          },
    { L_, "abc_e",          1,  1,  3,  3,  "a___e",         "_bcV_"         },
    { L_, "abcd_f",         1,  1,  4,  4,  "a____f",        "_bcdV_"        },
    { L_, "abcde_g",        1,  1,  5,  5,  "a_____g",       "_bcdeV_"       },

    { L_, "a__d",           2,  1,  1,  1,  "a__d",          "_VV_"          },
    { L_, "ab__e",          2,  1,  1,  2,  "a___e",         "_VVb_"         },
    { L_, "abc__f",         2,  1,  1,  3,  "a____f",        "_VVbc_"        },
    { L_, "abcd__g",        2,  1,  1,  4,  "a_____g",       "_VVbcd_"       },
    { L_, "abcde__h",       2,  1,  1,  5,  "a______h",      "_VVbcde_"      },
    { L_, "abcdef__i",      2,  1,  1,  6,  "a_______i",     "_VVbcdef_"     },

    { L_, "abcd__g",        2,  1,  2,  4,  "a_____g",       "_bVVcd_"       },
    { L_, "abcd__g",        2,  1,  3,  4,  "a_____g",       "_bcVVd_"       },

    { L_, "a__d",           2,  1,  1,  1,  "a__d",          "_VV_"          },
    { L_, "ab__e",          2,  1,  2,  2,  "a___e",         "_bVV_"         },
    { L_, "abc__f",         2,  1,  3,  3,  "a____f",        "_bcVV_"        },
    { L_, "abcd__g",        2,  1,  4,  4,  "a_____g",       "_bcdVV_"       },
    { L_, "abcde__h",       2,  1,  5,  5,  "a______h",      "_bcdeVV_"      },
    { L_, "abcdef__i",      2,  1,  6,  6,  "a_______i",     "_bcdefVV_"     },

    { L_, "a___e",          3,  1,  1,  1,  "a___e",         "_VVV_"         },
    { L_, "ab___f",         3,  1,  1,  2,  "a____f",        "_VVVb_"        },
    { L_, "abc___g",        3,  1,  1,  3,  "a_____g",       "_VVVbc_"       },
    { L_, "abcd___h",       3,  1,  1,  4,  "a______h",      "_VVVbcd_"      },
    { L_, "abcde___i",      3,  1,  1,  5,  "a_______i",     "_VVVbcde_"     },
    { L_, "abcdef___j",     3,  1,  1,  6,  "a________j",    "_VVVbcdef_"    },
    { L_, "abcdefg___k",    3,  1,  1,  7,  "a_________k",   "_VVVbcdefg_"   },

    { L_, "abcde___i",      3,  1,  2,  5,  "a_______i",     "_bVVVcde_"     },
    { L_, "abcde___i",      3,  1,  3,  5,  "a_______i",     "_bcVVVde_"     },
    { L_, "abcde___i",      3,  1,  4,  5,  "a_______i",     "_bcdVVVe_"     },

    { L_, "a___e",          3,  1,  1,  1,  "a___e",         "_VVV_"         },
    { L_, "ab___f",         3,  1,  2,  2,  "a____f",        "_bVVV_"        },
    { L_, "abc___g",        3,  1,  3,  3,  "a_____g",       "_bcVVV_"       },
    { L_, "abcd___h",       3,  1,  4,  4,  "a______h",      "_bcdVVV_"      },
    { L_, "abcde___i",      3,  1,  5,  5,  "a_______i",     "_bcdeVVV_"     },
    { L_, "abcdef___j",     3,  1,  6,  6,  "a________j",    "_bcdefVVV_"    },
    { L_, "abcdefg___k",    3,  1,  7,  7,  "a_________k",   "_bcdefgVVV_"   },

    { L_,  "a____f",        4,  1,  1,  1,  "a____f",        "_VVVV_"        },
    { L_,  "ab____g",       4,  1,  1,  2,  "a_____g",       "_VVVVb_"       },
    { L_,  "abc____h",      4,  1,  1,  3,  "a______h",      "_VVVVbc_"      },
    { L_,  "abcd____i",     4,  1,  1,  4,  "a_______i",     "_VVVVbcd_"     },
    { L_,  "abcde____j",    4,  1,  1,  5,  "a________j",    "_VVVVbcde_"    },
    { L_,  "abcdef____k",   4,  1,  1,  6,  "a_________k",   "_VVVVbcdef_"   },
    { L_,  "abcdefg____l",  4,  1,  1,  7,  "a__________l",  "_VVVVbcdefg_"  },
    { L_,  "abcdefgh____m", 4,  1,  1,  8,  "a___________m", "_VVVVbcdefgh_" },

    { L_,  "abcdef____k",   4,  1,  2,  6,  "a_________k",   "_bVVVVcdef_"   },
    { L_,  "abcdef____k",   4,  1,  3,  6,  "a_________k",   "_bcVVVVdef_"   },
    { L_,  "abcdef____k",   4,  1,  4,  6,  "a_________k",   "_bcdVVVVef_"   },
    { L_,  "abcdef____k",   4,  1,  5,  6,  "a_________k",   "_bcdeVVVVf_"   },

    { L_,  "a____f",        4,  1,  1,  1,  "a____f",        "_VVVV_"        },
    { L_,  "ab____g",       4,  1,  2,  2,  "a_____g",       "_bVVVV_"       },
    { L_,  "abc____h",      4,  1,  3,  3,  "a______h",      "_bcVVVV_"      },
    { L_,  "abcd____i",     4,  1,  4,  4,  "a_______i",     "_bcdVVVV_"     },
    { L_,  "abcde____j",    4,  1,  5,  5,  "a________j",    "_bcdeVVVV_"    },
    { L_,  "abcdef____k",   4,  1,  6,  6,  "a_________k",   "_bcdefVVVV_"   },
    { L_,  "abcdefg____l",  4,  1,  7,  7,  "a__________l",  "_bcdefgVVVV_"  },
    { L_,  "abcdefgh____m", 4,  1,  8,  8,  "a___________m", "_bcdefghVVVV_" },
};
const int NUM_DATA_6V = sizeof DATA_6V / sizeof *DATA_6V;

template <class TYPE>
void testDestructiveMoveAndInsertValueN(bool bitwiseMoveableFlag,
                                        bool bitwiseCopyableFlag,
                                        bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_6V' elements of
    // the 'DATA_6V' array, that inserting the 'd_ne' entries at
    // the 'd_dst' index while shifting the entries between 'd_dst'
    // until the 'd_end' indices in a buffer built according to the 'd_spec'
    // specifications results in a buffer built according to the 'd_expected'
    // specifications.  The 'd_lineNum' member is used to report errors.
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u, v;

    const char *DST_SPEC = "________________";  // add more as needed
    ASSERT(MAX_SIZE <= (int)strlen(DST_SPEC));

    {
        bsls::ObjectBuffer<TYPE> mV;
        bslalg::ScalarPrimitives::defaultConstruct(&mV.object(), Z);
        setValue(&mV.object(), 'V');
        const TYPE& V = mV.object();
        ASSERT('V' == getValue(V));

        for (int ti = 0; ti < NUM_DATA_6V; ++ti) {
            const int         LINE     = DATA_6V[ti].d_lineNum;
            const char *const SRC_SPEC = DATA_6V[ti].d_srcSpec;
            const int         NE       = DATA_6V[ti].d_ne;
            const int         BEGIN    = DATA_6V[ti].d_begin;
            const int         DST      = DATA_6V[ti].d_dst;
            const int         END      = DATA_6V[ti].d_end;
            const char *const SRC_EXP  = DATA_6V[ti].d_srcExp;
            const char *const DST_EXP  = DATA_6V[ti].d_dstExp;
            LOOP_ASSERT(ti, MAX_SIZE >= (int)std::strlen(SRC_SPEC));

            if (veryVerbose) {
                printf("LINE = %d, SRC_SPEC = %s, NE = %d, "
                       "BEGIN = %d, DST = %d, END = %d, "
                       "SRC_EXP = %s, DST_EXP = %s\n",
                       LINE, SRC_SPEC, NE, BEGIN, DST, END, SRC_EXP, DST_EXP);
            }

            TYPE *srcBuf = (TYPE*) (void*) &u.d_raw[0];
            TYPE *dstBuf = (TYPE*) (void*) &v.d_raw[0];
            TYPE *srcEnd = &srcBuf[END];

            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                    gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                    srcEnd = &srcBuf[END];
                    CleanupGuard<TYPE> srcGuard(srcBuf, SRC_SPEC, &srcEnd);

                    Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                                  &srcEnd,
                                                  &srcBuf[BEGIN],
                                                  &srcBuf[DST],
                                                  &srcBuf[END],
                                                  V,
                                                  NE,
                                                  Z);

                    ASSERT(&srcBuf[BEGIN] == srcEnd);
                    verify(srcBuf, SRC_EXP);
                    verify(dstBuf, DST_EXP);
                    srcGuard.release(SRC_EXP);
                    cleanup(dstBuf, DST_EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                              &srcEnd,
                                              &srcBuf[BEGIN],
                                              &srcBuf[DST],
                                              &srcBuf[END],
                                              V,
                                              NE,
                                              Z);

                ASSERT(&srcBuf[BEGIN] == srcEnd);
                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES       == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_COPIES + NE  == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(srcBuf, SRC_EXP);
                verify(dstBuf, DST_EXP);
                cleanup(srcBuf, SRC_EXP);
                cleanup(dstBuf, DST_EXP);
            }
        }
        bslalg::ScalarDestructionPrimitives::destroy(&mV.object());
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

static const struct {
    int         d_lineNum;   // source line number
    const char *d_srcSpec;   // specification string
    int         d_ne;        // number of elements to insert
    int         d_begin;     // start of data
    int         d_dst;       // index of insertion point
    int         d_end;       // end of data
    const char *d_srcExp;    // expected result source array
    const char *d_dstExp;    // expected result destination array
} DATA_6R[] = {
    //                       v----------------------------------------  Ordered
    //line srcSpec          ne  beg dst end srcExp           dstExp     by ne
    //---- -------          --  --- --- --- --------         ------     -------
    { L_, "___",            0,  1,  1,  1,  "___",           "___"           },
    { L_, "a_c",            0,  1,  1,  1,  "a_c",           "___"           },
    { L_, "abc",            0,  1,  1,  2,  "a_c",           "_b_"           },

    { L_, "abc",            0,  1,  2,  2,  "a_c",           "_b_"           },

    { L_, "a_c",            1,  1,  1,  1,  "a_c",           "_t_"           },
    { L_, "ab_d",           1,  1,  1,  2,  "a__d",          "_tb_"          },
    { L_, "abc_e",          1,  1,  1,  3,  "a___e",         "_tbc_"         },
    { L_, "abcd_f",         1,  1,  1,  4,  "a____f",        "_tbcd_"        },
    { L_, "abcde_g",        1,  1,  1,  5,  "a_____g",       "_tbcde_"       },

    { L_, "abc_e",          1,  1,  2,  3,  "a___e",         "_btc_"         },

    { L_, "a_c",            1,  1,  1,  1,  "a_c",           "_t_"           },
    { L_, "ab_d",           1,  1,  2,  2,  "a__d",          "_bt_"          },
    { L_, "abc_e",          1,  1,  3,  3,  "a___e",         "_bct_"         },
    { L_, "abcd_f",         1,  1,  4,  4,  "a____f",        "_bcdt_"        },
    { L_, "abcde_g",        1,  1,  5,  5,  "a_____g",       "_bcdet_"       },

    { L_, "a__d",           2,  1,  1,  1,  "a__d",          "_tu_"          },
    { L_, "ab__e",          2,  1,  1,  2,  "a___e",         "_tub_"         },
    { L_, "abc__f",         2,  1,  1,  3,  "a____f",        "_tubc_"        },
    { L_, "abcd__g",        2,  1,  1,  4,  "a_____g",       "_tubcd_"       },
    { L_, "abcde__h",       2,  1,  1,  5,  "a______h",      "_tubcde_"      },
    { L_, "abcdef__i",      2,  1,  1,  6,  "a_______i",     "_tubcdef_"     },

    { L_, "abcd__g",        2,  1,  2,  4,  "a_____g",       "_btucd_"       },
    { L_, "abcd__g",        2,  1,  3,  4,  "a_____g",       "_bctud_"       },

    { L_, "a__d",           2,  1,  1,  1,  "a__d",          "_tu_"          },
    { L_, "ab__e",          2,  1,  2,  2,  "a___e",         "_btu_"         },
    { L_, "abc__f",         2,  1,  3,  3,  "a____f",        "_bctu_"        },
    { L_, "abcd__g",        2,  1,  4,  4,  "a_____g",       "_bcdtu_"       },
    { L_, "abcde__h",       2,  1,  5,  5,  "a______h",      "_bcdetu_"      },
    { L_, "abcdef__i",      2,  1,  6,  6,  "a_______i",     "_bcdeftu_"     },

    { L_, "a___e",          3,  1,  1,  1,  "a___e",         "_tuv_"         },
    { L_, "ab___f",         3,  1,  1,  2,  "a____f",        "_tuvb_"        },
    { L_, "abc___g",        3,  1,  1,  3,  "a_____g",       "_tuvbc_"       },
    { L_, "abcd___h",       3,  1,  1,  4,  "a______h",      "_tuvbcd_"      },
    { L_, "abcde___i",      3,  1,  1,  5,  "a_______i",     "_tuvbcde_"     },
    { L_, "abcdef___j",     3,  1,  1,  6,  "a________j",    "_tuvbcdef_"    },
    { L_, "abcdefg___k",    3,  1,  1,  7,  "a_________k",   "_tuvbcdefg_"   },

    { L_, "abcde___i",      3,  1,  2,  5,  "a_______i",     "_btuvcde_"     },
    { L_, "abcde___i",      3,  1,  3,  5,  "a_______i",     "_bctuvde_"     },
    { L_, "abcde___i",      3,  1,  4,  5,  "a_______i",     "_bcdtuve_"     },

    { L_, "a___e",          3,  1,  1,  1,  "a___e",         "_tuv_"         },
    { L_, "ab___f",         3,  1,  2,  2,  "a____f",        "_btuv_"        },
    { L_, "abc___g",        3,  1,  3,  3,  "a_____g",       "_bctuv_"       },
    { L_, "abcd___h",       3,  1,  4,  4,  "a______h",      "_bcdtuv_"      },
    { L_, "abcde___i",      3,  1,  5,  5,  "a_______i",     "_bcdetuv_"     },
    { L_, "abcdef___j",     3,  1,  6,  6,  "a________j",    "_bcdeftuv_"    },
    { L_, "abcdefg___k",    3,  1,  7,  7,  "a_________k",   "_bcdefgtuv_"   },

    { L_,  "a____f",        4,  1,  1,  1,  "a____f",        "_tuvw_"        },
    { L_,  "ab____g",       4,  1,  1,  2,  "a_____g",       "_tuvwb_"       },
    { L_,  "abc____h",      4,  1,  1,  3,  "a______h",      "_tuvwbc_"      },
    { L_,  "abcd____i",     4,  1,  1,  4,  "a_______i",     "_tuvwbcd_"     },
    { L_,  "abcde____j",    4,  1,  1,  5,  "a________j",    "_tuvwbcde_"    },
    { L_,  "abcdef____k",   4,  1,  1,  6,  "a_________k",   "_tuvwbcdef_"   },
    { L_,  "abcdefg____l",  4,  1,  1,  7,  "a__________l",  "_tuvwbcdefg_"  },
    { L_,  "abcdefgh____m", 4,  1,  1,  8,  "a___________m", "_tuvwbcdefgh_" },

    { L_,  "abcdef____k",   4,  1,  2,  6,  "a_________k",   "_btuvwcdef_"   },
    { L_,  "abcdef____k",   4,  1,  3,  6,  "a_________k",   "_bctuvwdef_"   },
    { L_,  "abcdef____k",   4,  1,  4,  6,  "a_________k",   "_bcdtuvwef_"   },
    { L_,  "abcdef____k",   4,  1,  5,  6,  "a_________k",   "_bcdetuvwf_"   },

    { L_,  "a____f",        4,  1,  1,  1,  "a____f",        "_tuvw_"        },
    { L_,  "ab____g",       4,  1,  2,  2,  "a_____g",       "_btuvw_"       },
    { L_,  "abc____h",      4,  1,  3,  3,  "a______h",      "_bctuvw_"      },
    { L_,  "abcd____i",     4,  1,  4,  4,  "a_______i",     "_bcdtuvw_"     },
    { L_,  "abcde____j",    4,  1,  5,  5,  "a________j",    "_bcdetuvw_"    },
    { L_,  "abcdef____k",   4,  1,  6,  6,  "a_________k",   "_bcdeftuvw_"   },
    { L_,  "abcdefg____l",  4,  1,  7,  7,  "a__________l",  "_bcdefgtuvw_"  },
    { L_,  "abcdefgh____m", 4,  1,  8,  8,  "a___________m", "_bcdefghtuvw_" },
};
const int NUM_DATA_6R = sizeof DATA_6R / sizeof *DATA_6R;

template <class TYPE>
void testDestructiveMoveAndInsertRange(bool bitwiseMoveableFlag,
                                       bool bitwiseCopyableFlag,
                                       bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_6R' elements of
    // the 'DATA_6R' array, that inserting the 'd_neInsert' entries at the
    // 'd_dst' index while shifting the 'd_neMove' entries at and after the
    // 'd_dst' index in a buffer built according to the 'd_spec' specifications
    // results in a buffer built according to the 'd_expected' specifications.
    // The 'd_lineNum' member is used to report errors.  In the presence of
    // exceptions, check that the array has the same number of initialized
    // entries (and no more), although their values are unspecified.
{
    const char INPUT[] = { "tuvwxyz" };
    enum { INPUT_LEN = sizeof INPUT / sizeof *INPUT };
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[2 * MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u, v, w;

    const char *DST_SPEC = "________________";  // add more as needed
    ASSERT(MAX_SIZE <= (int)strlen(DST_SPEC));

    ConstructEnabler inputCe[INPUT_LEN - 1];
    for (const char *pc = INPUT; *pc; ++pc) {
        inputCe[pc - INPUT] = *pc;
    }

    TYPE *input = (TYPE*) (void*) &w.d_raw[MAX_SIZE];
    gg(input, INPUT);  verify(input, INPUT);

    for (int ti = 0; ti < NUM_DATA_6R; ++ti) {
        const int         LINE     = DATA_6R[ti].d_lineNum;
        const char *const SRC_SPEC = DATA_6R[ti].d_srcSpec;
        const int         NE       = DATA_6R[ti].d_ne;
        const int         BEGIN    = DATA_6R[ti].d_begin;
        const int         DST      = DATA_6R[ti].d_dst;
        const int         END      = DATA_6R[ti].d_end;
        const char *const SRC_EXP  = DATA_6R[ti].d_srcExp;
        const char *const DST_EXP  = DATA_6R[ti].d_dstExp;
        LOOP_ASSERT(ti, MAX_SIZE >= (int)std::strlen(SRC_SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SRC_SPEC = %s, NE = %d, "
                   "BEGIN = %d, DST = %d, END = %d, "
                   "SRC_EXP = %s, DST_EXP = %s\n",
                   LINE, SRC_SPEC, NE, BEGIN, DST, END, SRC_EXP, DST_EXP);
        }

        TYPE *srcBuf = (TYPE*) (void*) &u.d_raw[0];
        TYPE *dstBuf = (TYPE*) (void*) &v.d_raw[0];
        TYPE *srcEnd = &srcBuf[END];

        if (veryVerbose) printf("\t\t...and arbitrary FWD_ITER\n");
        {
            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                    gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                    srcEnd = &srcBuf[END];  // reset at each iteration
                    CleanupGuard<TYPE> srcGuard(srcBuf, SRC_SPEC, &srcEnd);

                    Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                                  &srcEnd,
                                                  &srcBuf[BEGIN],
                                                  &srcBuf[DST],
                                                  &srcBuf[END],
                                                  &inputCe[0],
                                                  &inputCe[NE],
                                                  NE,
                                                  Z);

                    ASSERT(&srcBuf[BEGIN] == srcEnd);
                    verify(srcBuf, SRC_EXP);
                    verify(dstBuf, DST_EXP);
                    srcGuard.release(SRC_EXP);
                    cleanup(dstBuf, DST_EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                              &srcEnd,
                                              &srcBuf[BEGIN],
                                              &srcBuf[DST],
                                              &srcBuf[END],
                                              &inputCe[0],
                                              &inputCe[NE],
                                              NE,
                                              Z);

                ASSERT(&srcBuf[BEGIN] == srcEnd);
                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(srcBuf, SRC_EXP);
                verify(dstBuf, DST_EXP);
                cleanup(srcBuf, SRC_EXP);
                cleanup(dstBuf, DST_EXP);
            }
        }

        if (veryVerbose) printf("\t\t...and FWD_ITER = TYPE*\n");
        {
            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                    gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                    srcEnd = &srcBuf[END];  // reset at each iteration
                    CleanupGuard<TYPE> srcGuard(srcBuf, SRC_SPEC, &srcEnd);

                    Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                                  &srcEnd,
                                                  &srcBuf[BEGIN],
                                                  &srcBuf[DST],
                                                  &srcBuf[END],
                                                  &input[0],
                                                  &input[NE],
                                                  NE,
                                                  Z);

                    ASSERT(&srcBuf[BEGIN] == srcEnd);
                    verify(srcBuf, SRC_EXP);
                    verify(dstBuf, DST_EXP);
                    srcGuard.release(SRC_EXP);
                    cleanup(dstBuf, DST_EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::destructiveMoveAndInsert(&dstBuf[BEGIN],
                                              &srcEnd,
                                              &srcBuf[BEGIN],
                                              &srcBuf[DST],
                                              &srcBuf[END],
                                              &input[0],
                                              &input[NE],
                                              NE,
                                              Z);

                ASSERT(&srcBuf[BEGIN] == srcEnd);
                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_COPIES + NE == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(srcBuf, SRC_EXP);
                verify(dstBuf, DST_EXP);
                cleanup(srcBuf, SRC_EXP);
                cleanup(dstBuf, DST_EXP);
            }
        }
    }
    cleanup(input, INPUT);
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

template <class TYPE>
void testDestructiveMoveAndMoveInsert(bool bitwiseMoveableFlag,
                                      bool,
                                      bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_6R' elements of
    // the 'DATA_6R' array, that inserting the 'd_neInsert' entries at the
    // 'd_dst' index while shifting the 'd_neMove' entries at and after the
    // 'd_dst' index in a buffer built according to the 'd_spec' specifications
    // results in a buffer built according to the 'd_expected' specifications.
    // The 'd_lineNum' member is used to report errors.  In the presence of
    // exceptions, check that the array has the same number of initialized
    // entries (and no more), although their values are unspecified.
{
    const char *INPUT     = "tuvwxyz";
    const char *INPUT_EXP = "_______";  // after move
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[2 * MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u, v, w;

    const char *DST_SPEC = "________________";  // add more as needed
    ASSERT(MAX_SIZE <= (int)strlen(DST_SPEC));

    if (veryVerbose) printf("\t...FWD_ITER = TYPE* (only)\n");

    for (int ti = 0; ti < NUM_DATA_6R; ++ti) {
        const int         LINE     = DATA_6R[ti].d_lineNum;
        const char *const SRC_SPEC = DATA_6R[ti].d_srcSpec;
        const int         NE       = DATA_6R[ti].d_ne;
        const int         BEGIN    = DATA_6R[ti].d_begin;
        const int         DST      = DATA_6R[ti].d_dst;
        const int         END      = DATA_6R[ti].d_end;
        const char *const SRC_EXP  = DATA_6R[ti].d_srcExp;
        const char *const DST_EXP  = DATA_6R[ti].d_dstExp;
        LOOP_ASSERT(ti, MAX_SIZE >= (int)std::strlen(SRC_SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SRC_SPEC = %s, NE = %d, "
                   "BEGIN = %d, DST = %d, END = %d, "
                   "SRC_EXP = %s, DST_EXP = %s\n",
                   LINE, SRC_SPEC, NE, BEGIN, DST, END, SRC_EXP, DST_EXP);
        }

        TYPE *srcBuf = (TYPE*) (void*) &u.d_raw[0];
        TYPE *dstBuf = (TYPE*) (void*) &v.d_raw[0];
        TYPE *srcEnd = &srcBuf[END];

        TYPE *input = (TYPE*) (void *) &w.d_raw[MAX_SIZE];
        TYPE *inputEnd = &input[NE];

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                gg(input, INPUT);      verify(input, INPUT);
                inputEnd = &input[NE];  // reset at each iteration
                CleanupGuard<TYPE> cleanupInput(input, INPUT, &inputEnd);

                gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
                gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

                srcEnd = &srcBuf[END];  // reset at each iteration
                CleanupGuard<TYPE> srcGuard(srcBuf, SRC_SPEC, &srcEnd);

                Obj::destructiveMoveAndMoveInsert(&dstBuf[BEGIN],
                                                  &srcEnd,
                                                  &inputEnd,
                                                  &srcBuf[BEGIN],
                                                  &srcBuf[DST],
                                                  &srcBuf[END],
                                                  &input[0],
                                                  &input[NE],
                                                  NE,
                                                  Z);

                ASSERT(&input[0] == inputEnd);
                ASSERT(&srcBuf[BEGIN] == srcEnd);
                verify(input, INPUT_EXP);
                verify(srcBuf, SRC_EXP);
                verify(dstBuf, DST_EXP);
                srcGuard.release(SRC_EXP);
                cleanup(dstBuf, DST_EXP);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            gg(input, INPUT);      verify(input, INPUT);
            gg(srcBuf, SRC_SPEC);  verify(srcBuf, SRC_SPEC);
            gg(dstBuf, DST_SPEC);  verify(dstBuf, DST_SPEC);

            const int NUM_COPIES = numCopyCtorCalls;
            const int NUM_DESTRUCTIONS = numDestructorCalls;

            Obj::destructiveMoveAndMoveInsert(&dstBuf[BEGIN],
                                              &srcEnd,
                                              &inputEnd,
                                              &srcBuf[BEGIN],
                                              &srcBuf[DST],
                                              &srcBuf[END],
                                              &input[0],
                                              &input[NE],
                                              NE,
                                              Z);

            ASSERT(&input[0] == inputEnd);
            ASSERT(&srcBuf[BEGIN] == srcEnd);
            if (bitwiseMoveableFlag) {
                ASSERT(NUM_COPIES == numCopyCtorCalls);
                ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
            }
            verify(input, INPUT_EXP);
            verify(srcBuf, SRC_EXP);
            verify(dstBuf, DST_EXP);
            cleanup(input + NE, INPUT + NE);  // cleanup suffix of input
            cleanup(srcBuf, SRC_EXP);
            cleanup(dstBuf, DST_EXP);
        }
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 5
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to insert
    int         d_dst;       // index of insertion point
    int         d_end;       // end of data
    const char *d_expected;  // expected result array
} DATA_5V[] = {
    //line spec            ne  dst    end  expected            ordered by ne
    //---- ----            --  ---    ---  --------            -------------
    { L_,  "___",          0,  1,     1,   "___"           },  // 0
    { L_,  "a_c",          0,  1,     1,   "a_c"           },
    { L_,  "abc",          0,  1,     2,   "abc"           },

    { L_,  "___",          1,  1,     1,   "_V_"           },  // 1
    { L_,  "a_c",          1,  1,     1,   "aVc"           },
    { L_,  "ab_d",         1,  1,     2,   "aVbd"          },
    { L_,  "abc_e",        1,  1,     3,   "aVbce"         },
    { L_,  "abcd_f",       1,  1,     4,   "aVbcdf"        },
    { L_,  "abcde_g",      1,  1,     5,   "aVbcdeg"       },

    { L_,  "a__d",         2,  1,     1,   "aVVd"          },  // 2
    { L_,  "ab__e",        2,  1,     2,   "aVVbe"         },
    { L_,  "abc__f",       2,  1,     3,   "aVVbcf"        },
    { L_,  "abcd__g",      2,  1,     4,   "aVVbcdg"       },
    { L_,  "abcde__h",     2,  1,     5,   "aVVbcdeh"      },
    { L_,  "abcdef__i",    2,  1,     6,   "aVVbcdefi"     },

    { L_,  "a___e",        3,  1,     1,   "aVVVe"         },  // 3
    { L_,  "ab___f",       3,  1,     2,   "aVVVbf"        },
    { L_,  "abc___g",      3,  1,     3,   "aVVVbcg"       },
    { L_,  "abcd___h",     3,  1,     4,   "aVVVbcdh"      },
    { L_,  "abcde___i",    3,  1,     5,   "aVVVbcdei"     },
    { L_,  "abcdef___j",   3,  1,     6,   "aVVVbcdefj"    },
    { L_,  "abcdefg___k",  3,  1,     7,   "aVVVbcdefgk"   },

    { L_,  "a____f",       4,  1,     1,   "aVVVVf"        },  // 4
    { L_,  "ab____g",      4,  1,     2,   "aVVVVbg"       },
    { L_,  "abc____h",     4,  1,     3,   "aVVVVbch"      },
    { L_,  "abcd____i",    4,  1,     4,   "aVVVVbcdi"     },
    { L_,  "abcde____j",   4,  1,     5,   "aVVVVbcdej"    },
    { L_,  "abcdef____k",  4,  1,     6,   "aVVVVbcdefk"   },
    { L_,  "abcdefg____l", 4,  1,     7,   "aVVVVbcdefgl"  },
    { L_,  "abcdefgh____m",4,  1,     8,   "aVVVVbcdefghm" },
};
const int NUM_DATA_5V = sizeof DATA_5V / sizeof *DATA_5V;

template <class TYPE>
void testInsertValueN(bool bitwiseMoveableFlag,
                      bool bitwiseCopyableFlag,
                      bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_5V' elements of
    // the 'DATA_5V' array, that inserting the 'd_ne' entries at
    // the 'd_dst' index while shifting the entries between 'd_dst'
    // until the 'd_end' indices in a buffer built according to the 'd_spec'
    // specifications results in a buffer built according to the 'd_expected'
    // specifications.  The 'd_lineNum' member is used to report errors.
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;

    {
        bsls::ObjectBuffer<TYPE> mV;
        bslalg::ScalarPrimitives::defaultConstruct(&mV.object(), Z);
        setValue(&mV.object(), 'V');
        const TYPE& V = mV.object();
        ASSERT('V' == getValue(V));

        for (int ti = 0; ti < NUM_DATA_5V; ++ti) {
            const int         LINE = DATA_5V[ti].d_lineNum;
            const char *const SPEC = DATA_5V[ti].d_spec;
            const int         NE   = DATA_5V[ti].d_ne;
            const int         DST  = DATA_5V[ti].d_dst;
            const int         END  = DATA_5V[ti].d_end;
            const char *const EXP  = DATA_5V[ti].d_expected;
            LOOP_ASSERT(ti, MAX_SIZE >= (int)std::strlen(SPEC));

            if (veryVerbose) {
                printf("LINE = %d, SPEC = %s, NE = %d, "
                        "DST = %d, END = %d, EXP = %s\n",
                        LINE, SPEC, NE, DST, END, EXP);
            }

            TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(buf, SPEC);  verify(buf, SPEC);
                    CleanupGuard<TYPE> cleanup(buf, SPEC);

                    Obj::insert(&buf[DST], &buf[END], V, NE, Z);

                    verify(buf, EXP);
                    cleanup.release(EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(buf, SPEC);  verify(buf, SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::insert(&buf[DST], &buf[END], V, NE, Z);

                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_COPIES + NE == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(buf, EXP);
                cleanup(buf, EXP);
            }

        }
        bslalg::ScalarDestructionPrimitives::destroy(&mV.object());
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_ne;        // number of elements to insert
    int         d_dst;       // index of insertion point
    int         d_end;       // end of data
    const char *d_expected;  // expected result array
} DATA_5R[] = {
    //line spec            ne  dst    end  expected            ordered by ne
    //---- ----            --  ---    ---  --------            -------------
    { L_,  "___",          0,  1,     1,   "___"           },  // 0
    { L_,  "a_c",          0,  1,     1,   "a_c"           },
    { L_,  "abc",          0,  1,     2,   "abc"           },

    { L_,  "___",          1,  1,     1,   "_t_"           },  // 1
    { L_,  "a_c",          1,  1,     1,   "atc"           },
    { L_,  "ab_d",         1,  1,     2,   "atbd"          },
    { L_,  "abc_e",        1,  1,     3,   "atbce"         },
    { L_,  "abcd_f",       1,  1,     4,   "atbcdf"        },
    { L_,  "abcde_g",      1,  1,     5,   "atbcdeg"       },

    { L_,  "a__d",         2,  1,     1,   "atud"          },  // 2
    { L_,  "ab__e",        2,  1,     2,   "atube"         },
    { L_,  "abc__f",       2,  1,     3,   "atubcf"        },
    { L_,  "abcd__g",      2,  1,     4,   "atubcdg"       },
    { L_,  "abcde__h",     2,  1,     5,   "atubcdeh"      },
    { L_,  "abcdef__i",    2,  1,     6,   "atubcdefi"     },

    { L_,  "a___e",        3,  1,     1,   "atuve"         },  // 3
    { L_,  "ab___f",       3,  1,     2,   "atuvbf"        },
    { L_,  "abc___g",      3,  1,     3,   "atuvbcg"       },
    { L_,  "abcd___h",     3,  1,     4,   "atuvbcdh"      },
    { L_,  "abcde___i",    3,  1,     5,   "atuvbcdei"     },
    { L_,  "abcdef___j",   3,  1,     6,   "atuvbcdefj"    },
    { L_,  "abcdefg___k",  3,  1,     7,   "atuvbcdefgk"   },

    { L_,  "a____f",       4,  1,     1,   "atuvwf"        },  // 4
    { L_,  "ab____g",      4,  1,     2,   "atuvwbg"       },
    { L_,  "abc____h",     4,  1,     3,   "atuvwbch"      },
    { L_,  "abcd____i",    4,  1,     4,   "atuvwbcdi"     },
    { L_,  "abcde____j",   4,  1,     5,   "atuvwbcdej"    },
    { L_,  "abcdef____k",  4,  1,     6,   "atuvwbcdefk"   },
    { L_,  "abcdefg____l", 4,  1,     7,   "atuvwbcdefgl"  },
    { L_,  "abcdefgh____m",4,  1,     8,   "atuvwbcdefghm" },
};
const int NUM_DATA_5R = sizeof DATA_5R / sizeof *DATA_5R;

template <class TYPE>
void testInsertRange(bool bitwiseMoveableFlag,
                     bool bitwiseCopyableFlag,
                     bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_5R' elements of
    // the 'DATA_5R' array, that inserting the 'd_neInsert' entries at the
    // 'd_dst' index while shifting the 'd_neMove' entries at and after the
    // 'd_dst' index in a buffer built according to the 'd_spec' specifications
    // results in a buffer built according to the 'd_expected' specifications.
    // The 'd_lineNum' member is used to report errors.  In the presence of
    // exceptions, check that the array has the same number of initialized
    // entries (and no more), although their values are unspecified.
{
    const char INPUT[] = { "tuvwxyz" };
    enum { INPUT_LEN = sizeof INPUT / sizeof *INPUT };
    ConstructEnabler inputCe[INPUT_LEN - 1];
    for (const char *pc = INPUT; *pc; ++pc) {
        inputCe[pc - INPUT] = *pc;
    }

    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[2 * MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u, v;

    TYPE *input = (TYPE*) (void*) &v.d_raw[MAX_SIZE];
    gg(input, INPUT);  verify(input, INPUT);

    for (int ti = 0; ti < NUM_DATA_5R; ++ti) {
        const int         LINE = DATA_5R[ti].d_lineNum;
        const char *const SPEC = DATA_5R[ti].d_spec;
        const int         NE   = DATA_5R[ti].d_ne;
        const int         DST  = DATA_5R[ti].d_dst;
        const int         END  = DATA_5R[ti].d_end;
        const char *const EXP  = DATA_5R[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, NE= %d, "
                    "DST = %d, END = %d, EXP = %s\n",
                    LINE, SPEC, NE, DST, END , EXP);
        }

        if (veryVerbose) printf("\t\t...and arbitrary FWD_ITER\n");
        {
            TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(buf, SPEC);  verify(buf, SPEC);
                    CleanupGuard<TYPE> cleanup(buf, SPEC);

                    Obj::insert(&buf[DST],
                                &buf[END],
                                &inputCe[0],
                                &inputCe[NE],
                                NE,
                                Z);

                    verify(buf, EXP);
                    cleanup.release(EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(buf, SPEC);  verify(buf, SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::insert(&buf[DST],
                            &buf[END],
                            &inputCe[0],
                            &inputCe[NE],
                            NE,
                            Z);

                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(buf, EXP);
                cleanup(buf, EXP);
            }
        }

        if (veryVerbose) printf("\t\t...and FWD_ITER = TYPE*\n");
        {
            TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    gg(buf, SPEC);  verify(buf, SPEC);
                    CleanupGuard<TYPE> cleanup(buf, SPEC);

                    Obj::insert(&buf[DST],
                                &buf[END],
                                &input[0],
                                &input[NE],
                                NE,
                                Z);

                    verify(buf, EXP);
                    cleanup.release(EXP);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                gg(buf, SPEC);  verify(buf, SPEC);

                const int NUM_COPIES = numCopyCtorCalls;
                const int NUM_DESTRUCTIONS = numDestructorCalls;

                Obj::insert(&buf[DST],
                            &buf[END],
                            &input[0],
                            &input[NE],
                            NE,
                            Z);

                if (bitwiseCopyableFlag) {
                    ASSERT(NUM_COPIES == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                else if (bitwiseMoveableFlag) {
                    ASSERT(NUM_COPIES + NE == numCopyCtorCalls);
                    ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
                }
                verify(buf, EXP);
                cleanup(buf, EXP);
            }
        }
    }
    cleanup(input, INPUT);
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

template <class TYPE>
void testMoveInsert(bool bitwiseMoveableFlag,
                    bool,
                    bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_5R' elements of
    // the 'DATA_5R' array, that inserting the 'd_neInsert' entries at the
    // 'd_dst' index while shifting the 'd_neMove' entries at and after the
    // 'd_dst' index in a buffer built according to the 'd_spec' specifications
    // results in a buffer built according to the 'd_expected' specifications.
    // The 'd_lineNum' member is used to report errors.  In the presence of
    // exceptions, check that the array has the same number of initialized
    // entries (and no more), although their values are unspecified.
{
    const char *INPUT     = "tuvwxyz";
    const char *INPUT_EXP = "_______";  // after move
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[2 * MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u, v;

    if (veryVerbose) printf("\t\t...FWD_ITER = TYPE* (only)\n");

    for (int ti = 0; ti < NUM_DATA_5R; ++ti) {
        const int         LINE = DATA_5R[ti].d_lineNum;
        const char *const SPEC = DATA_5R[ti].d_spec;
        const int         NE   = DATA_5R[ti].d_ne;
        const int         DST  = DATA_5R[ti].d_dst;
        const int         END  = DATA_5R[ti].d_end;
        const char *const EXP  = DATA_5R[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, NE= %d, "
                    "DST = %d, END = %d, EXP = %s\n",
                    LINE, SPEC, NE, DST, END , EXP);
        }

        TYPE *buf = (TYPE*) (void*) &u.d_raw[0];
        TYPE *input = (TYPE*) (void*) &v.d_raw[MAX_SIZE];
        TYPE *inputEnd = &input[NE];

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                gg(input, INPUT);  verify(input, INPUT);
                CleanupGuard<TYPE> cleanupInput(input, INPUT, &inputEnd);

                gg(buf, SPEC);  verify(buf, SPEC);
                CleanupGuard<TYPE> cleanupBuf(buf, SPEC);

                Obj::moveInsert(&buf[DST],
                                &buf[END],
                                &inputEnd,
                                &input[0],
                                &input[NE],
                                NE,
                                Z);

                ASSERT(&input[0] == inputEnd);
                verify(input, INPUT_EXP);
                verify(buf, EXP);
                cleanupBuf.release(EXP);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            gg(input, INPUT);  verify(input, INPUT);
            gg(buf, SPEC);  verify(buf, SPEC);

            const int NUM_COPIES = numCopyCtorCalls;
            const int NUM_DESTRUCTIONS = numDestructorCalls;

            Obj::moveInsert(&buf[DST],
                            &buf[END],
                            &inputEnd,
                            &input[0],
                            &input[NE],
                            NE,
                            Z);

            if (bitwiseMoveableFlag) {
                ASSERT(NUM_COPIES == numCopyCtorCalls);
                ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
            }
            ASSERT(&input[0] == inputEnd);
            verify(input, INPUT_EXP);
            verify(buf, EXP);
            cleanup(input + NE, INPUT + NE);  // cleanup suffix of input
            cleanup(buf, EXP);
        }
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 4
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_src;       // index to source
    int         d_ne;        // number of elements
    int         d_dst;       // index to destination
    const char *d_expected;  // expected result array
} DATA_4[] = {
    //line spec           src     ne   dst   expected
    //---- ----           ---     --   ---   --------     // ADJUST
    { L_,  "_",           0,      0,   0,    "_"           },  // 0
    { L_,  "a",           0,      0,   0,    "a"           },
    { L_,  "a_c",         0,      0,   1,    "a_c"         },
    { L_,  "ab_d",        0,      0,   2,    "ab_d"        },

    { L_,  "a_cd",        3,      0,   1,    "a_cd"        },
    { L_,  "ab_d",        3,      0,   2,    "ab_d"        },

    { L_,  "ab_d",        1,      1,   2,    "a_bd"        },  // 1
    { L_,  "abc_e",       1,      1,   3,    "a_cbe"       },
    { L_,  "abcd_f",      1,      1,   4,    "a_cdbf"      },

    { L_,  "a_cd",        2,      1,   1,    "ac_d"        },
    { L_,  "a_cde",       3,      1,   1,    "adc_e"       },
    { L_,  "a_cdef",      4,      1,   1,    "aecd_f"      },

    { L_,  "abc__d",      1,      2,   3,    "a__bcd"      },  // 2
    { L_,  "abcd__g",     1,      2,   4,    "a__dbcg"     },
    { L_,  "abcde__h",    1,      2,   5,    "a__debch"    },

    { L_,  "a__def",      3,      2,   1,    "ade__f"      },
    { L_,  "a__defg",     4,      2,   1,    "aefd__g"     },
    { L_,  "a__defgh",    5,      2,   1,    "afgde__h"    },

    { L_,  "abcd___h",    1,      3,   4,    "a___bcdh"    },  // 3
    { L_,  "abcde___i",   1,      3,   5,    "a___ebcdi"   },
    { L_,  "abcdef___j",  1,      3,   6,    "a___efbcdj"  },

    { L_,  "a___efgh",    4,      3,   1,    "aefg___h"    },
    { L_,  "a___efghi",   5,      3,   1,    "afghe___i"   },
    { L_,  "a___efghij",  6,      3,   1,    "aghief___j"  },

    { L_,  "abcde____j",  1,      4,   5,    "a____bcdej"  },  // 4
    { L_,  "abcdef____k", 1,      4,   6,    "a____fbcdek" },
    { L_,  "abcdefg____l",1,      4,   7,    "a____fgbcdel"},

    { L_,  "a____fghij",  5,      4,   1,    "afghi____j"  },
    { L_,  "a____fghijk", 6,      4,   1,    "aghijf____k" },
    { L_,  "a____fghijkl",7,      4,   1,    "ahijkfg____l"},
};
const int NUM_DATA_4 = sizeof DATA_4 / sizeof *DATA_4;

template <class TYPE>
void testDestructiveMove(bool bitwiseMoveableFlag,
                         bool, // bitwiseCopyableFlag
                         bool exceptionSafetyFlag = false)
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    TYPE *buf = (TYPE *) (void *) &u.d_raw[0];

    for (int ti = 0; ti < NUM_DATA_4; ++ti) {
        const int         LINE = DATA_4[ti].d_lineNum;
        const char *const SPEC = DATA_4[ti].d_spec;
        const int         SRC  = DATA_4[ti].d_src;
        const int         NE   = DATA_4[ti].d_ne;
        const int         DST  = DATA_4[ti].d_dst;
        const char *const EXP  = DATA_4[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, SRC = %d, "
                    "NE = %d, DST = %d, EXP = %s\n",
                    LINE, SPEC, SRC, NE, DST, EXP);
        }
        gg(buf, SPEC);  verify(buf, SPEC);

        const int NUM_COPIES = numCopyCtorCalls;

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                Obj::destructiveMove(&buf[DST],
                                     &buf[SRC],
                                     &buf[SRC + NE],
                                     Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            Obj::destructiveMove(&buf[DST],
                                 &buf[SRC],
                                 &buf[SRC + NE],
                                 Z);
        }

        if (bitwiseMoveableFlag) {
            ASSERT(NUM_COPIES == numCopyCtorCalls);
        }
        verify(buf, EXP);
        cleanup(buf, EXP);
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 3
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_src;       // index to source
    int         d_ne;        // number of elements
    int         d_dst;       // index to destination
    const char *d_expected;  // expected result array
} DATA_3[] = {
    //line spec           src    ne    dst   expected
    //---- ----           ---    --    ---   --------     // ADJUST
    { L_,  "_",           0,     0,    0,    "_"           },  // 0
    { L_,  "a",           0,     0,    0,    "a"           },
    { L_,  "a_c",         0,     0,    1,    "a_c"         },
    { L_,  "ab_d",        0,     0,    2,    "ab_d"        },

    { L_,  "a_cd",        3,     0,    1,    "a_cd"        },
    { L_,  "ab_d",        3,     0,    2,    "ab_d"        },

    { L_,  "ab_d",        1,     1,    2,    "abbd"        },  // 1
    { L_,  "abc_e",       1,     1,    3,    "abcbe"       },
    { L_,  "abcd_f",      1,     1,    4,    "abcdbf"      },

    { L_,  "a_cd",        2,     1,    1,    "accd"        },
    { L_,  "a_cde",       3,     1,    1,    "adcde"       },
    { L_,  "a_cdef",      4,     1,    1,    "aecdef"      },

    { L_,  "abc__d",      1,     2,    3,    "abcbcd"      },  // 2
    { L_,  "abcd__g",     1,     2,    4,    "abcdbcg"     },
    { L_,  "abcde__h",    1,     2,    5,    "abcdebch"    },

    { L_,  "a__def",      3,     2,    1,    "adedef"      },
    { L_,  "a__defg",     4,     2,    1,    "aefdefg"     },
    { L_,  "a__defgh",    5,     2,    1,    "afgdefgh"    },

    { L_,  "abcd___h",    1,     3,    4,    "abcdbcdh"    },  // 3
    { L_,  "abcde___i",   1,     3,    5,    "abcdebcdi"   },
    { L_,  "abcdef___j",  1,     3,    6,    "abcdefbcdj"  },

    { L_,  "a___efgh",    4,     3,    1,    "aefgefgh"    },
    { L_,  "a___efghi",   5,     3,    1,    "afghefghi"   },
    { L_,  "a___efghij",  6,     3,    1,    "aghiefghij"  },

    { L_,  "abcde____j",  1,     4,    5,    "abcdebcdej"  },  // 4
    { L_,  "abcdef____k", 1,     4,    6,    "abcdefbcdek" },
    { L_,  "abcdefg____l",1,     4,    7,    "abcdefgbcdel"},

    { L_,  "a____fghij",  5,     4,    1,    "afghifghij"  },
    { L_,  "a____fghijk", 6,     4,    1,    "aghijfghijk" },
    { L_,  "a____fghijkl",7,     4,    1,    "ahijkfghijkl"},
};
const int NUM_DATA_3 = sizeof DATA_3 / sizeof *DATA_3;

template <class TYPE>
void testCopyConstruct(bool, // bitwiseMoveableFlag
                       bool bitwiseCopyableFlag,
                       bool exceptionSafetyFlag = false)
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    TYPE *buf = (TYPE *) (void *) &u.d_raw[0];

    if (verbose) printf("\t\tfrom same type.\n");

    for (int ti = 0; ti < NUM_DATA_3; ++ti) {
        const int         LINE = DATA_3[ti].d_lineNum;
        const char *const SPEC = DATA_3[ti].d_spec;
        const int         SRC  = DATA_3[ti].d_src;
        const int         NE   = DATA_3[ti].d_ne;
        const int         DST  = DATA_3[ti].d_dst;
        const char *const EXP  = DATA_3[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, SRC = %d, "
                   "NE = %d, DST = %d, EXP = %s\n",
                   LINE, SPEC, SRC, NE, DST, EXP);
        }
        gg(buf, SPEC);  verify(buf, SPEC);

        const int NUM_COPIES = numCopyCtorCalls;
        const int NUM_CTORS  = numCharCtorCalls;

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                Obj::copyConstruct(&buf[DST], &buf[SRC], &buf[SRC + NE], Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            Obj::copyConstruct(&buf[DST], &buf[SRC], &buf[SRC + NE], Z);
        }

        if (veryVerbose) {
            printf("LINE = %d, #copy ctors = %d, #char ctors = %d.\n",
                   LINE,
                   numCopyCtorCalls - NUM_COPIES,
                   numCharCtorCalls - NUM_CTORS);
        }
        if (bitwiseCopyableFlag) {
            ASSERT(NUM_COPIES == numCopyCtorCalls);
            ASSERT(NUM_CTORS  == numCharCtorCalls);
        }
        else {
            ASSERT(NUM_COPIES + NE <= numCopyCtorCalls);
            ASSERT(NUM_CTORS       == numCharCtorCalls);
        }
        verify(buf, EXP);
        cleanup(buf, EXP);
    }

    if (verbose) printf("\t\tfrom different type.\n");

    for (int ti = 0; ti < NUM_DATA_3; ++ti) {
        const int         LINE = DATA_3[ti].d_lineNum;
        const char *const SPEC = DATA_3[ti].d_spec;
        const int         SRC  = DATA_3[ti].d_src;
        const int         NE   = DATA_3[ti].d_ne;
        const int         DST  = DATA_3[ti].d_dst;
        const char *const EXP  = DATA_3[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

        enum { SPEC_CE_LEN = 20 };
        ASSERT(SPEC_CE_LEN > std::strlen(SPEC));

        ConstructEnabler specCe[SPEC_CE_LEN];
        for (const char *pc = SPEC; *pc; ++pc) {
            specCe[pc - SPEC] = *pc;
        }

        if (veryVerbose) {
            printf("LINE = %d, SPEC = %s, SRC = %d, "
                   "NE = %d, DST = %d, EXP = %s\n",
                   LINE, SPEC, SRC, NE, DST, EXP);
        }
        gg(buf, SPEC);  verify(buf, SPEC);

        const int NUM_COPIES = numCopyCtorCalls;
        const int NUM_CTORS  = numCharCtorCalls;

        if (exceptionSafetyFlag) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                Obj::copyConstruct(&buf[DST],
                                   &specCe[SRC],
                                   &specCe[SRC + NE],
                                   Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            if (veryVerbose) printf("\n");
        } else {
            Obj::copyConstruct(&buf[DST], &specCe[SRC], &specCe[SRC + NE], Z);
        }

        if (veryVerbose) {
            printf("LINE = %d, #copy ctors = %d, #char ctors = %d.\n",
                   LINE,
                   numCopyCtorCalls - NUM_COPIES,
                   numCharCtorCalls - NUM_CTORS);
        }
        ASSERT(NUM_COPIES     == numCopyCtorCalls);
//      ASSERT(NUM_CTORS + NE <= numCharCtorCalls);

        verify(buf, EXP);
        cleanup(buf, EXP);
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 2
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // start of [begin, end) range
    int         d_ne;        // number of elements to initialize
    const char *d_expected;  // expected result array
} DATA_2[] = {
    //line spec                  begin   ne   expected            ordered by ne
    //---- ----                  -----   --   --------            -------------
    { L_,  "___",                1,      0,   "___"               },  // 0
    { L_,  "a_c",                1,      0,   "a_c"               },
    { L_,  "abc",                1,      0,   "abc"               },

    { L_,  "___",                1,      1,   "_V_"               },  // 1
    { L_,  "a_c",                1,      1,   "aVc"               },

    { L_,  "____",               1,      2,   "_VV_"              },  // 2
    { L_,  "a__d",               1,      2,   "aVVd"              },

    { L_,  "a___e",              1,      3,   "aVVVe"             },  // 3

    { L_,  "a____f",             1,      4,   "aVVVVf"            },  // 4

    { L_,  "a_____g",            1,      5,   "aVVVVVg"           },  // 5

    { L_,  "a______h",           1,      6,   "aVVVVVVh"          },  // 6

    { L_,  "a_______i",          1,      7,   "aVVVVVVVi"         },  // 7

    { L_,  "a________j",         1,      8,   "aVVVVVVVVj"        },  // 8

    { L_,  "a_____________n",    1,      13,  "aVVVVVVVVVVVVVn"   },  // 13
};
const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

template <typename TYPE>
void testUninitializedFillN(bool, // bitwiseMoveableFlag
                            bool bitwiseCopyableFlag,
                            bool exceptionSafetyFlag = false)
    // This test function verifies, for each of the 'NUM_DATA_2' elements of
    // the 'DATA_2' array, that initializing the 'd_ne' entries starting from
    // the 'd_begin' index in a buffer built according to the 'd_spec'
    // specifications results in a buffer built according to the 'd_expected'
    // specifications.  The 'd_lineNum' member is used to report errors.
{
    const int MAX_SIZE = 26;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(TYPE)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    TYPE *buf = (TYPE*) (void*) &u.d_raw[0];

    {
        bsls::ObjectBuffer<TYPE> mV;
        bslalg::ScalarPrimitives::defaultConstruct(&mV.object(), Z);
        setValue(&mV.object(), 'V');
        const TYPE& V = mV.object();
        ASSERT('V' == getValue(V));

        for (int ti = 0; ti < NUM_DATA_2; ++ti) {
            const int         LINE  = DATA_2[ti].d_lineNum;
            const char *const SPEC  = DATA_2[ti].d_spec;
            const int         BEGIN = DATA_2[ti].d_begin;
            const int         NE    = DATA_2[ti].d_ne;
            const char *const EXP   = DATA_2[ti].d_expected;
            ASSERT(MAX_SIZE >= (int)std::strlen(SPEC));

            if (veryVerbose) {
                printf("LINE = %d, SPEC = %s, "
                       "BEGIN = %d, NE = %d, EXP = %s\n",
                       LINE, SPEC, BEGIN, NE, EXP);
            }

            gg(buf, SPEC);  verify(buf, SPEC);

            const int NUM_COPIES = numCopyCtorCalls;

            if (exceptionSafetyFlag) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*Z) {
                    Obj::uninitializedFillN(&buf[BEGIN], NE, V, Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                if (veryVerbose) printf("\n");
            } else {
                Obj::uninitializedFillN(&buf[BEGIN], NE, V, Z);
            }

            if (bitwiseCopyableFlag) {
                ASSERT(NUM_COPIES == numCopyCtorCalls);
            }
            verify(buf, EXP);
            cleanup(buf, EXP);
        }
        bslalg::ScalarDestructionPrimitives::destroy(&mV.object());
    }
    ASSERT(0 == Z->numMismatches());
    ASSERT(0 == Z->numBytesInUse());
}

template <typename TYPE>
void testUninitializedFillNBCT(TYPE value)
    // This test function verifies that initializing a subset of a buffer
    // initially filled with junk with a variable number of copies of the
    // specified 'value' of the parameterized 'TYPE', starting at a variable
    // beginning position, results in the expected buffer (unmodified outside
    // the subset, and each entry equal to 'value' within the subset) in all
    // cases.  The behavior is undefined unless 'TYPE' is bitwise copyable.
{
    const int MAX_SIZE    = 15;
    const int BUFFER_SIZE = MAX_SIZE * sizeof(TYPE);

    typedef union {
        char                                d_raw[BUFFER_SIZE];
        int                                 d_enum;
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } Buffer;

    Buffer u;
    Buffer v;
    TYPE *bufU = (TYPE*) (void*) (&u.d_raw[0]);
    TYPE *bufV = (TYPE*) (void*) (&v.d_raw[0]);

    for (int arraySize = 0; arraySize < MAX_SIZE; ++arraySize) {
        for (int begin = 0; begin <= arraySize; ++begin) {
            const int maxLen = arraySize - begin;

            for (int numElements = 0; numElements < maxLen; ++numElements) {
                fillWithJunk(bufU, BUFFER_SIZE);
                memcpy(bufV, bufU, BUFFER_SIZE);

                if (veryVerbose) {
                    printf("\t\tarraySz = %d, begin = %d, numElements = %d\n",
                           arraySize, begin, numElements);
                }
                Obj::uninitializedFillN(&bufU[begin], numElements, value, Z);

                // Note: since the untouched portion of the buffer is filled
                // with junk, it doesn't make sense to compare as 'TYPE' since
                // the result might be negative (two 'NaN's for instance always
                // compare negative, even if the bit pattern is identical).
                {
                    int cmp = memcmp(bufU, bufV, begin * sizeof(TYPE));
                    LOOP4_ASSERT(arraySize, begin, numElements, cmp, 0 == cmp);
                }
                {
                    int offset = begin + numElements;
                    int cmp = memcmp(bufU + offset,
                                     bufV + offset,
                                     BUFFER_SIZE - offset * sizeof(TYPE));
                    LOOP4_ASSERT(arraySize, begin, numElements, cmp, 0 == cmp);
                }
                for (int i = begin; i < begin + numElements; ++i) {
                    LOOP4_ASSERT(arraySize, begin, numElements, i,
                                 value == bufU[i]);
                }
            }
        }
    }
}

//=============================================================================
//                          GAUNTLET MACRO
// Passed 'func', which should be a template of a function whose single
// template parameter is the type to be stored into the array, and which takes
// 3 boolean arguments telling
//    - the template parameter is a bitwise moveable type
//    - the template parameter is a bitwise copyable type
//    - exception testing is to be done.
//
// Run 'func' on a whole gauntlet of different types.
//=============================================================================

#define GAUNTLET(func) do {                                                   \
        if (verbose) printf("\t...with TestTypeNoAlloc.\n");                  \
        func<TNA>(false, false);                                              \
                                                                              \
        if (verbose) printf("\t...with TestType.\n");                         \
        func<T>(false, false);                                                \
                                                                              \
        if (verbose) printf("\t...with BitwiseMoveableTestType.\n");          \
        func<BMT>(true, false);                                               \
                                                                              \
        if (verbose) printf("\t...with BitwiseCopyableTestType.\n");          \
        func<BCT>(true, true);                                                \
                                                                              \
        if (verbose) printf("\t...with FuncPtrType.\n");                      \
        func<FPT>(true, true);                                                \
                                                                              \
        if (verbose) printf("\t...with MemberFuncPtrType.\n");                \
        func<MFPT>(true, true);                                               \
                                                                              \
        if (verbose) printf("\t...with char.\n");                             \
        func<char>(true, true);                                               \
                                                                              \
        if (verbose) printf("\t...with char.\n");                             \
        func<signed char>(true, true);                                        \
                                                                              \
        if (verbose) printf("\t...with char.\n");                             \
        func<unsigned char>(true, true);                                      \
                                                                              \
        if (verbose) printf("\t...with short.\n");                            \
        func<short>(true, true);                                              \
                                                                              \
        if (verbose) printf("\t...with unsigned short.\n");                   \
        func<unsigned short>(true, true);                                     \
                                                                              \
        if (verbose) printf("\t...with int.\n");                              \
        func<int>(true, true);                                                \
                                                                              \
        if (verbose) printf("\t...with unsigned int.\n");                     \
        func<unsigned int>(true, true);                                       \
                                                                              \
        if (verbose) printf("\t...with long.\n");                             \
        func<long>(true, true);                                               \
                                                                              \
        if (verbose) printf("\t...with unsigned long.\n");                    \
        func<unsigned long>(true, true);                                      \
                                                                              \
        if (verbose) printf("\t...with Int64.\n");                            \
        func<Int64>(true, true);                                              \
                                                                              \
        if (verbose) printf("\t...with Uint64.\n");                           \
        func<Uint64>(true, true);                                             \
                                                                              \
        if (verbose) printf("\t...with float.\n");                            \
        func<float>(true, true);                                              \
                                                                              \
        if (verbose) printf("\t...with double.\n");                           \
        func<double>(true, true);                                             \
                                                                              \
        if (verbose) printf("\t...with long double.\n");                      \
        func<long double>(true, true);                                        \
                                                                              \
        if (verbose) printf("\t...with 'void *'.\n");                         \
        func<void *>(true, true);                                             \
                                                                              \
        if (verbose) printf("\t...with 'const void *'.\n");                   \
        func<const void *>(true, true);                                       \
                                                                              \
        if (verbose) printf("\t...with 'int *'.\n");                          \
        func<int *>(true, true);                                              \
                                                                              \
        if (verbose) printf("\t...with 'const int *'.\n");                    \
        func<const int *>(true, true);                                        \
                                                                              \
        if (verbose) printf("\tException test.\n");                           \
        func<T>(false, false, true);                                          \
    } while (false)

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Defining a Vector-Like Type
/// - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a STL-vector-like type.  One requirement is that
// an object of this vector should forward its allocator to its contained
// elements when appropriate.  Another requirement is that the vector should
// take advantage of the optimizations available for certain traits of the
// contained element type.  For example, if the contained element type has the
// 'bslalg::TypeTraitBitwiseMoveable' trait, moving an element in a vector can
// be done using 'memcpy' instead of copy construction.
//
// We can utilize the class methods provided by 'bslalg::ArrayPrimitives' to
// satisfy the above requirements.  Unlike 'bslalg::ScalarPrimitives', which
// operates on a single element, 'bslalg::ArrayPrimitives' operates on arrays,
// which will further help simplify our implementation.
//
// First, we create an elided definition of the class template 'MyVector':
//..
template <class TYPE>
class MyVector {
    // This class implements a vector of elements of the (template parameter)
    // 'TYPE', which must be copy constructable.  Note that for the brevity of
    // the usage example, this class does not provide any Exception-Safety
    // guarantee.

    // DATA
    TYPE             *d_array_p;      // pointer to the allocated array
    int               d_capacity;     // capacity of the allocated array
    int               d_size;         // number of objects
    bslma::Allocator *d_allocator_p;  // allocation pointer (held, not owned)

  public:
    // TYPE TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
        MyVector,
        BloombergLP::bslmf::IsBitwiseMoveable);

    // CREATORS
    explicit MyVector(bslma::Allocator *basicAllocator = 0)
        // Construct a 'MyVector' object having a size of 0 and and a capacity
        // of 0.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.
    : d_array_p(0)
    , d_capacity(0)
    , d_size(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    MyVector(const MyVector& original, bslma::Allocator *basicAllocator = 0);
        // Create a 'MyVector' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // ...

    // MANIPULATORS
    void reserve(int capacity);
        // Change the capacity of this vector to the specified 'capacity' if it
        // is greater than the vector's current capacity.

    void insert(int dstIndex, int numElements, const TYPE& value);
        // Insert, into this vector, the specified 'numElements' of the
        // specified 'value' at the specified 'dstIndex'.  The behavior is
        // undefined unless '0 <= dstIndex <= size()'.

    // ACCESSORS
    const TYPE& operator[](int position) const
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this vector.
    {
        return d_array_p[position];
    }

    int size() const
        // Return the size of this vector.
    {
        return d_size;
    }
};
//..
// Then, we implement the copy constructor of 'MyVector':
//..
template <class TYPE>
MyVector<TYPE>::MyVector(const MyVector<TYPE>&  original,
                         bslma::Allocator      *basicAllocator)
: d_array_p(0)
, d_capacity(0)
, d_size(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    reserve(original.d_size);
//..
// Here, we call the 'bslalg::ArrayPrimitives::copyConstruct' class method to
// copy each element from 'original.d_array_p' to 'd_array_p' (When
// appropriate, this class method passes this vector's allocator to the copy
// constructor of 'TYPE' or uses bit-wise copy.):
//..
    bslalg::ArrayPrimitives::copyConstruct(
                                          d_array_p,
                                          original.d_array_p,
                                          original.d_array_p + original.d_size,
                                          d_allocator_p);

    d_size = original.d_size;
}
//..
// Now, we implement the 'reserve' method of 'MyVector':
//..
template <class TYPE>
void MyVector<TYPE>::reserve(int capacity)
{
    if (d_capacity >= capacity) return;

    TYPE *newArrayPtr = static_cast<TYPE*>(d_allocator_p->allocate(
           BloombergLP::bslma::Allocator::size_type(capacity * sizeof(TYPE))));

    if (d_array_p) {
//..
// Here, we call the 'bslalg::ArrayPrimitives::destructiveMove' class method to
// copy each original element from 'd_array_p' to 'newArrayPtr' and then
// destroy all the original elements (When appropriate, this class method
// passes this vector's allocator to the copy constructor of 'TYPE' or uses
// bit-wise copy.):
//..
        bslalg::ArrayPrimitives::destructiveMove(newArrayPtr,
                                                 d_array_p,
                                                 d_array_p + d_size,
                                                 d_allocator_p);
        d_allocator_p->deallocate(d_array_p);
    }

    d_array_p = newArrayPtr;
    d_capacity = capacity;
}

//..
// Finally, we implement the 'insert' method of 'MyVector':
//..
template <class TYPE>
void MyVector<TYPE>::insert(int dstIndex, int numElements, const TYPE& value)
{
    int newSize = d_size + numElements;

    if (newSize > d_capacity) {
        int newCapacity = d_capacity == 0 ? 2 : d_capacity * 2;
        reserve(newCapacity);
    }
//..
// Here, we call the 'bslalg::ArrayPrimitives::insert' class method to first
// move each element after 'dstIndex' by 'numElements' and then copy construct
// 'numElements' of 'value' at 'dstIndex'.  (When appropriate, this class
// method passes this vector's allocator to the copy constructor of 'TYPE' or
// uses bit-wise copy.):
//..
    bslalg::ArrayPrimitives::insert(d_array_p + dstIndex,
                                    d_array_p + d_size,
                                    value,
                                    numElements,
                                    d_allocator_p);

    d_size = newSize;
}
//..

}  // close unnamed namespace


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
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
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUsage Example"
                            "\n=============\n");


        // Do some ad-hoc breathing test for 'MyVector' type in the usage
        // example.

        MyVector<int> v;
        int DATA[] =  { 3, 2, 1, 10, 5 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            v.insert(i, 1, DATA[i]);
        }

        ASSERT(v.size() == NUM_DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            ASSERT(v[i] == DATA[i]);
        }

        MyVector<int> u(v);

        ASSERT(u.size() == NUM_DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            ASSERT(u[i] == DATA[i]);
        }
      }
      case 8: {
        // --------------------------------------------------------------------
        // TESTING rotate
        //
        // Concerns:
        //
        // Plan:
        //   Select size of range to rotate and all possible positions.
        //   Due to the use of gcd in the implementation, try all sizes <= 6,
        //   and in addition, for a small set of larger prime and non-prime
        //   sizes, try different positions in the range (relatively prime and
        //   non prime).
        //
        // Testing:
        //   void rotate(T *b, T *m, T *e)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'rotate'\n");

        GAUNTLET(testRotate);

        if (verbose) printf("\nTesting 'rotate'"
                                   "...with VeryLargeBitwiseMoveableTypes.\n");

        if (verbose) printf("\t\t...with 8 extra bytes.\n");
        testRotate<XXL8>(true, false);
        if (verbose) printf("\t\t...with 16 extra bytes.\n");
        testRotate<XXL16>(true, false);
        if (verbose) printf("\t\t...with 24 extra bytes.\n");
        testRotate<XXL24>(true, false);
        if (verbose) printf("\t\t...with 32 extra bytes.\n");
        testRotate<XXL32>(true, false);
        if (verbose) printf("\t\t...with 64 extra bytes.\n");
        testRotate<XXL64>(true, false);
        if (verbose) printf("\t\t...with 128 extra bytes.\n");
        testRotate<XXL128>(true, false);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING erase
        //
        // Concerns:
        //
        // Plan:
        //   Let ne = 'e' - 'b'.  Order test data by increasing ne.
        //
        // Testing:
        //   void erase(T *b, T *e, T *dataEnd, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'erase'."
                            "\n================\n");

        GAUNTLET(testErase);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING destructiveMoveAndInsert
        //
        // Concerns:
        //
        // Plan:
        //   insert #1: order test data by increasing 'ne'.
        //   insert #2: order test data by increasing 'ne' + ('dstE' - 'dstB').
        //
        // Testing:
        //   void destructiveMoveAndInsert(...);
        //   void destructiveMoveAndMoveInsert(...);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'destructiveMoveAndInsert'"
                            "\n==================================\n");

        if (verbose)
            printf("\nTesting 'destructiveMoveAndInsert(...).\n");

        GAUNTLET(testDestructiveMoveAndInsertValueN);

        if (verbose)
            printf("\nTesting 'destructiveMoveAndInsert(T *dstB, T *dstE, "
                                             "FWD srcB, FWD srcE, ne, *a)'\n");

        GAUNTLET(testDestructiveMoveAndInsertRange);

        if (verbose)
            printf("\nTesting 'destructiveMoveAndMoveInsert(T *dstB, T *dstE, "
                                             "FWD srcB, FWD srcE, ne, *a)'\n");

        GAUNTLET(testDestructiveMoveAndMoveInsert);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING insert
        //
        // Concerns:
        //
        // Plan:
        //   insert #1: order test data by increasing 'ne'.
        //   insert #2: order test data by increasing 'ne' + ('dstE' - 'dstB').
        //
        // Testing:
        //   void insert(T *dstB, T *dstE, const T& v, ne, *a);
        //   void insert(T *dstB, T *dstE, FWD srcB, FWD srcE, ne, *a);
        //   void moveInsert(T *dstB, T *dstE, T **srcEp, srcB, srcE, ne, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'insert'"
                            "\n================\n");

        if (verbose)
            printf("\nTesting 'insert(T *dstB, T *dstE, "
                                                     "const T& v, ne, *a)'\n");

        GAUNTLET(testInsertValueN);

        if (verbose)
            printf("\nTesting 'insert(T *dstB, T *dstE, "
                                             "FWD srcB, FWD srcE, ne, *a)'\n");

        GAUNTLET(testInsertRange);

        if (verbose)
            printf("\nTesting 'moveInsert(T *dstB, T *dstE, T **srcEp, "
                                             "FWD srcB, FWD srcE, ne, *a)'\n");

        GAUNTLET(testMoveInsert);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING destructiveMove
        //
        // Concerns:
        //
        // Plan:
        //   Let ne = 'srcE' - 'srcB'.  Order test data by increasing ne.
        //   Include test cases where (1) 'srcE' <= 'dstB', and
        //   (2) 'dstB' + ne <= 'srcB'.
        //
        // Testing:
        //   void destructiveMove(T *srcB, T *srcE, T *dstB, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'destructiveMove'\n");

        GAUNTLET(testDestructiveMove);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING copyConstruct
        //
        // Concerns:
        //
        // Plan:
        //   Let ne = 'srcE' - 'srcB'.  Order test data by increasing ne.
        //   Include test cases where (1) 'srcE' <= 'dstB', and
        //   (2) 'dstB' + ne <= 'srcB'.
        //
        // Testing:
        //   void copyConstruct(FWD srcB, FWD srcE, T *dstB, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'copyConstruct'\n");

        GAUNTLET(testCopyConstruct);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING uninitializedFillN
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void uninitializedFillN(T *b, ne, const T& v, *a);
        // --------------------------------------------------------------------


        if (verbose) printf("Testing 'uninitializedFillN'\n"
                            "============================\n");

        GAUNTLET(testUninitializedFillN);

        if (verbose) printf("\n\t...with fundamental and pointer types.\n");
        {
            if (verbose) printf("LINE = %d, TYPE = bool\n", L_);
            testUninitializedFillNBCT<bool>(false);
            testUninitializedFillNBCT<bool>(true);

            if (verbose) printf("LINE = %d, TYPE = char\n", L_);
            testUninitializedFillNBCT<char>(0);
            testUninitializedFillNBCT<char>(123);
            testUninitializedFillNBCT<char>(~0);

            if (verbose) printf("LINE = %d, TYPE = signed char\n", L_);
            testUninitializedFillNBCT<signed char>(0);
            testUninitializedFillNBCT<signed char>(123);
            testUninitializedFillNBCT<signed char>(~0);

            if (verbose) printf("LINE = %d, TYPE = unsigned char\n", L_);
            testUninitializedFillNBCT<unsigned char>(0);
            testUninitializedFillNBCT<unsigned char>(123);
            testUninitializedFillNBCT<unsigned char>(~0);

            if (verbose) printf("LINE = %d, TYPE = wchar_t\n", L_);
            testUninitializedFillNBCT<wchar_t>(0);
            testUninitializedFillNBCT<wchar_t>(123);
            testUninitializedFillNBCT<wchar_t>(~0);

            if (verbose) printf("LINE = %d, TYPE = short\n", L_);
            testUninitializedFillNBCT<short>(0);
            testUninitializedFillNBCT<short>(123);
            testUninitializedFillNBCT<short>(0x5b5b);
            testUninitializedFillNBCT<short>(~0);

            if (verbose) printf("LINE = %d, TYPE = unsigned short\n", L_);
            testUninitializedFillNBCT<unsigned short>(0);
            testUninitializedFillNBCT<unsigned short>(123);
            testUninitializedFillNBCT<unsigned short>(0x5b5b);
            testUninitializedFillNBCT<unsigned short>(~0);

            if (verbose) printf("LINE = %d, TYPE = int\n", L_);
            testUninitializedFillNBCT<int>(0);
            testUninitializedFillNBCT<int>(123);
            testUninitializedFillNBCT<int>(~0);

            if (verbose) printf("LINE = %d, TYPE = unsigned int\n", L_);
            testUninitializedFillNBCT<unsigned int>(0);
            testUninitializedFillNBCT<unsigned int>(123);
            testUninitializedFillNBCT<unsigned int>(~0);

            if (verbose) printf("LINE = %d, TYPE = long\n", L_);
            testUninitializedFillNBCT<long>(0);
            testUninitializedFillNBCT<long>(123);
            testUninitializedFillNBCT<long>(~0);

            if (verbose) printf("LINE = %d, TYPE = unsigned long\n", L_);
            testUninitializedFillNBCT<unsigned long>(0);
            testUninitializedFillNBCT<unsigned long>(123);
            testUninitializedFillNBCT<unsigned long>(~0);

            if (verbose) printf("LINE = %d, TYPE = Int64\n", L_);
            testUninitializedFillNBCT<Int64>(0);
            testUninitializedFillNBCT<Int64>(123);
            testUninitializedFillNBCT<Int64>(~0);

            if (verbose) printf("LINE = %d, TYPE = Uint64\n", L_);
            testUninitializedFillNBCT<Uint64>(0);
            testUninitializedFillNBCT<Uint64>(123);
            testUninitializedFillNBCT<Uint64>(~0);

            if (verbose) printf("LINE = %d, TYPE = float\n", L_);
            testUninitializedFillNBCT<float>(0.0);
            testUninitializedFillNBCT<float>(1.0);

            if (verbose) printf("LINE = %d, TYPE = double\n", L_);
            testUninitializedFillNBCT<double>(0.0);
            testUninitializedFillNBCT<double>(1.0);

            if (verbose) printf("LINE = %d, TYPE = long double\n", L_);
            testUninitializedFillNBCT<long double>(0.0);
            testUninitializedFillNBCT<long double>(1.0);

            if (verbose) printf("LINE = %d, TYPE = void*\n", L_);
            testUninitializedFillNBCT<void *>(0);
            testUninitializedFillNBCT<void *>((void*)&test);
            testUninitializedFillNBCT<void *>((void*)~0LL);

            if (verbose) printf("LINE = %d, TYPE = const void*\n", L_);
            testUninitializedFillNBCT<const void *>(0);
            testUninitializedFillNBCT<const void *>((const void*)&test);
            testUninitializedFillNBCT<const void *>((const void*)~0LL);

            if (verbose) printf("LINE = %d, TYPE = int*\n", L_);
            testUninitializedFillNBCT<int *>(0);
            testUninitializedFillNBCT<int *>((int *)&test);
            testUninitializedFillNBCT<int *>((int*)~0LL);

            if (verbose) printf("LINE = %d, TYPE = const int*\n", L_);
            testUninitializedFillNBCT<const int *>(0);
            testUninitializedFillNBCT<const int *>((const int*)&test);
            testUninitializedFillNBCT<const int *>((const int*)~0LL);
        }
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
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   It is unclear which of the implementations is the best for the
        //   'uninitializedFillN' algorithms.
        //
        // Plan:  Time all three implementations (exponential memcpy, memcpy
        //   by blocks of 32 bytes, or single loop) for various fundamental
        //   types and taking care to separate the care where 'memset' can be
        //   used (also time this fourth implementation).
        //
        // Testing:
        //   uninitializedFillN(char *,...);
        //   uninitializedFillN(int *,...);
        //   uninitializedFillN(double *,...);
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                           "\n===============\n");

        enum {
            BUFFER_SIZE = 1 << 24,
            NUM_ITER    = 4
        };

        const int rawBufferSize = (argc > 2) ? atoi(argv[2]) : BUFFER_SIZE;
        const int numIter = (argc > 3) ? atoi(argv[3]) : NUM_ITER;
        char *rawBuffer = (char *)Z->allocate(rawBufferSize);  // max alignment

        printf("\nUsage: %s [bufferSize] [numIter]"
               "\n\tbufferSize\tin bytes (default: 16777216)"
               "\n\tnumIter\t\tto be repeated (default: 4 times)\n", __FILE__);

        printf("\n\tuninitializedFillN with char\n");
        {
            const int bufferSize = rawBufferSize;
            char *buffer = rawBuffer;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = 0;
                }
            }
            timer.stop();
            printf("fill<char>(0) - single loop : %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer, bufferSize, (char)0, Z);
            }
            timer.stop();
            printf("fill<char>(0) - memset      : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (char)numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = buffer[0];
                }
            }
            timer.stop();
            printf("fill<char>(1) - single loop : %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer,
                                        bufferSize,
                                        buffer[0],
                                        Z);
            }
            timer.stop();
            printf("fill<char>(1) - memcpy by 32: %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                bslalg::ArrayPrimitives_Imp::bitwiseFillN(buffer,
                                                          buffer[0],
                                                          bufferSize);
            }
            timer.stop();
            printf("fill<char>(1) - exp memcpy  : %f\n", timer.elapsedTime());
        }

        printf("\n\tuninitializedFillN with int\n");
        {
            const unsigned long bufferSize = rawBufferSize / sizeof(int);
            int *buffer = (int *) (void *) rawBuffer;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = 0;
                }
            }
            timer.stop();
            printf("fill<int> (0)- single loop : %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer, bufferSize, 0, Z);
            }
            timer.stop();
            printf("fill<int>(0) - memset      : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = buffer[0];
                }
            }
            timer.stop();
            printf("fill<int>(1) - single loop : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer,
                                        bufferSize,
                                        buffer[0],
                                        Z);
            }
            timer.stop();
            printf("fill<int>(1) - memcpy by 32: %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                bslalg::ArrayPrimitives_Imp::bitwiseFillN(rawBuffer,
                                                          32,
                                                          rawBufferSize);
            }
            timer.stop();
            printf("fill<int>(1) - exp memcpy  : %f\n", timer.elapsedTime());
        }

        printf("\n\tuninitializedFillN with double\n");
        {
            const unsigned long bufferSize = rawBufferSize / sizeof(double);
            double *buffer = (double *) (void *) rawBuffer;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = 0.;
                }
            }
            timer.stop();
            printf("fill<double>(0) - single loop: %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer, bufferSize, 0., Z);
            }
            timer.stop();
            printf("fill<double>(0) - memset     : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (double)numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = buffer[0];
                }
            }
            timer.stop();
            printf("fill<double>(1) - single loop: %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (double)numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer,
                                        bufferSize,
                                        buffer[0],
                                        Z);
            }
            timer.stop();
            printf("fill<double>(1) - memcpy 32  : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (double)numIter;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                bslalg::ArrayPrimitives_Imp::bitwiseFillN(rawBuffer,
                                                          32,
                                                          rawBufferSize);
            }
            timer.stop();
            printf("fill<double>(1) - exp memcpy : %f\n", timer.elapsedTime());
        }

        printf("\n\tuninitializedFillN with void *\n");
        {
            const unsigned long bufferSize = rawBufferSize / sizeof(void *);
            void **buffer = (void **) (void *) rawBuffer;

            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = 0;
                }
            }
            timer.stop();
            printf("fill<void *>(0) - single loop: %f\n", timer.elapsedTime());

            timer.reset();
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer, bufferSize, (void*)0, Z);
            }
            timer.stop();
            printf("fill<void *>(0) - memset     : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (void *)buffer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                for (int j = 0; j < bufferSize; ++j) {
                    buffer[j] = buffer[0];
                }
            }
            timer.stop();
            printf("fill<void *>(1) - single loop: %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (void *)buffer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                Obj::uninitializedFillN(buffer,
                                        bufferSize,
                                        buffer[0],
                                        Z);
            }
            timer.stop();
            printf("fill<void *>(1) - memcpy 32  : %f\n", timer.elapsedTime());

            timer.reset();
            buffer[0] = (void *)buffer;
            timer.start();
            for (int i = 0; i < numIter; ++i) {
                bslalg::ArrayPrimitives_Imp::bitwiseFillN(rawBuffer,
                                                          32,
                                                          rawBufferSize);
            }
            timer.stop();
            printf("fill<void *>(1) - exp memcpy : %f\n", timer.elapsedTime());
        }

        Z->deallocate(rawBuffer);

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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
